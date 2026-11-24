/** @file       pwt.c
 *  @brief      C file of PWT device for Renesas RA2
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Dilasude GOKTURK
 *  @author     Burak OZAN
 *  @date       18.06.2022
 */
#define FILE_PWT_C

/** INCLUDES ******************************************************************/
#include "pwt.h"

/**
 *  @addtogroup RA2_PWT
 *  @{
 */

/** CONSTANTS *****************************************************************/

/** TYPEDEFS ******************************************************************/

/** MACROS ********************************************************************/

/** LOCAL FUNCTION DECLARATIONS ***********************************************/
static DEV_IO_FUNC_INIT(pwtInit);
static DEV_IO_FUNC_DEINIT(pwtDeinit);
static DEV_IO_FUNC_GET(pwtGet);

/** VARIABLES *****************************************************************/
uint16_t signalMissed = 0, consecutiveFalseSignalCnt = 0;

/// @brief  Structure that defines the functions for pwt
const tsDevIoFuncs devIoPwtFuncs =
    {
        pwtInit,
        pwtDeinit,
        pwtGet,
        NULL,
};

/** INTERFACE FUNCTION DEFINITIONS ********************************************/
/// @brief  Interrupt callback function with device parameter
void pwtIsrCallBack(timer_callback_args_t *p_args, const tsDevIo *device)
{
    tsPwtParams *params                = device->parameters;
    const tsPwtConsts *consts          = device->constants;
    uint32_t newCapture                = 0;
    uint32_t dummyCapture              = 0;
    fsp_err_t err                      = FSP_SUCCESS;

    if (p_args->event == TIMER_EVENT_CYCLE_END)
    {
        params->timerOverflowCnt++;
    }
    else if (p_args->event == consts->captureEvent)
    {
        timer_info_t info;

        err = consts->timer->p_api->infoGet(consts->timer->p_ctrl, &info);
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

        /* Calculate capture (peak to peak) ***********************************************************************/
        if (p_args->capture > info.period_counts && ((p_args->capture >> 16) == 0xFFFF)) //Renesas drivers has been caused a fault for 16 bit timers, so it is a precaution.
        {
            newCapture = ((uint16_t)(info.period_counts) * params->timerOverflowCnt) + (uint16_t)(p_args->capture);
        }
        else
        {
            newCapture = ((info.period_counts) * params->timerOverflowCnt) + p_args->capture;
        }
        params->timerOverflowCnt = 0;

        /* Add new capture to queue *****************************************************************/
#ifdef PWT_CAC_ACTIVE
        {//If this macro is active, actual and expected cac values are used for accurated measurement
            uint32_t tempFreqRatio = devIoGet(consts->cac);
            uint64_t tempCapture = 0;

            RCOS_ASSERT(((tempFreqRatio != 0) && (consts->clockRatioConstant != 0)), RASSERT_PRI_LOW);
            if ((tempFreqRatio != 0) && (consts->clockRatioConstant != 0))
            {
                tempCapture = (uint64_t)(newCapture) * (uint64_t)(consts->clockRatioConstant);
                tempCapture /= (uint64_t)(tempFreqRatio);
                newCapture = (uint32_t)(tempCapture);
            }
        }
#endif

        if (DEQUE_IS_FULL(params->queue))
        {
            dequePopFront(params->queue, &dummyCapture, SIZE_OF_CAPTURE); // Remove one
        }
        dequePushBack(params->queue, &newCapture, SIZE_OF_CAPTURE); // Add a new one
        params->captured = TRUE;

        if(params->pwtCallback)
        {//Calling the callback parameter if defined
            params->pwtCallback(newCapture, info.clock_frequency);
        }
    }
}

/// @brief  Init function for a pwt
static DEV_IO_FUNC_INIT(pwtInit)
{
    tsPwtParams *params = device->parameters;
    const tsPwtConsts *consts = device->constants;
    fsp_err_t err       = FSP_SUCCESS;

    ADD_RCOS_VERSION("ra2/pwt", "1.0.0")

    params->pwtCallback = (pwtCallbackFunctionType *)config;//If callback function given, it will be called when a capture occured

    err = consts->timer->p_api->open(consts->timer->p_ctrl, consts->timer->p_cfg);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

#ifdef PWT_CAC_ACTIVE
    {
        const tsPwtConsts *consts = device->constants;
        devIoInit(consts->cac, NULL);
    }
#endif

    err = consts->timer->p_api->enable(consts->timer->p_ctrl);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a pwt
static DEV_IO_FUNC_DEINIT(pwtDeinit)
{
    tsPwtParams *params = device->parameters;
    const tsPwtConsts *consts = device->constants;
    fsp_err_t err       = FSP_SUCCESS;

    device->sys->initialized = 0;

    err = consts->timer->p_api->stop(consts->timer->p_ctrl);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    err = consts->timer->p_api->close(consts->timer->p_ctrl);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

#ifdef PWT_CAC_ACTIVE
    {
        const tsPwtConsts *consts = device->constants;
        devIoDeinit(consts->cac);
    }
#endif
    params->pwtCallback = NULL;

    return EXIT_SUCCESS;
}

/// @brief  Get function for a pwt
static DEV_IO_FUNC_GET(pwtGet)
{
    tsPwtParams *params       = device->parameters;
    const tsPwtConsts *consts = device->constants;

    uint32_t returnValue = 0;

    if (params->captured != FALSE)
    {
        timer_info_t info;
        uint32_t *buffer            = (uint32_t *)params->queue->buffer; // Calculate average of all values inside the queue
        uint64_t tempTotal          = 0;
        uint64_t meanCapture        = 0;
        uint8_t bufferCaptureCount  = params->queue->count / SIZE_OF_CAPTURE;
        fsp_err_t err               = FSP_SUCCESS;

        /* Calculate average capture value from queue *************************************************************/
        while (bufferCaptureCount--)
        {
            tempTotal += *buffer++;
        }

        if (params->queue->count != 0)
        {
            meanCapture = tempTotal / (params->queue->count / SIZE_OF_CAPTURE);
            if(consts->resolutionCoef)
            {
                meanCapture *= consts->resolutionCoef;
            }
        }

        /* Get the timer's clock ************************************************************************************/
        err = consts->timer->p_api->infoGet(consts->timer->p_ctrl, &info);
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

        /* Calculate the pulse width time ***************************************************************************/
        if (info.clock_frequency != 0)
        {
            returnValue = meanCapture / (info.clock_frequency);
        }
        else
        {
            returnValue = 0;
        }

        params->captured = FALSE; //Reset the flag to prevent to sending the same value
    }
    else
    {
        returnValue = 0;
    }

    return returnValue;
}

/** @} */

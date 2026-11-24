/** @file       pwt2freq.c
 *  @brief      C file of PWT 2 FREQ device for  RA2 (It is generic for all mcus)
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Dilasude GOKTURK
 *  @author     Burak OZAN
 *  @date       18.06.2022
 */
#define FILE_PWT_2_FREQ_C

/** INCLUDES ******************************************************************/
#include "pwt2freq.h"

/**
 *  @addtogroup RA2_PWT2FREQ
 *  @{
 */

/** CONSTANTS *****************************************************************/
#define CONSECUTIVE_FALSE_SIGNAL_COUNT_LIMIT (10)
//
#define HALF_OF_PERIOD(period)                      (period >> 1) //period / 2
#define TOLERANCE_OF_HALF_PERIOD(period)            (period >> 5) //period / 32 (period / (2 ^ 5))

#define TOLERANCE_OF_JUST_AFTER_CAPTURE(period)     (period >> 6) //period / 64 (period / (2 ^ 6))

#define DOUBLE_OF_PERIOD(period)                    (period << 1) //period * 2
#define TOLERANCE_OF_DOUBLE_PERIOD(period)          (period >> 4) //period / 16 (period / (2 ^ 4))
//
/** TYPEDEFS ******************************************************************/

/** MACROS ********************************************************************/

/** LOCAL FUNCTION DECLARATIONS ***********************************************/
static DEV_IO_FUNC_INIT(pwt2freqInit);
static DEV_IO_FUNC_DEINIT(pwt2freqDeinit);
static DEV_IO_FUNC_GET(pwt2freqGet);

/** VARIABLES *****************************************************************/

/// @brief  Structure that defines the functions for pwt2freq
const tsDevIoFuncs devIoPwt2FreqFuncs =
    {
        pwt2freqInit,
        pwt2freqDeinit,
        pwt2freqGet,
        NULL,
};

/** INTERFACE FUNCTION DEFINITIONS ********************************************/
/// @brief  Interrupt callback function
void pwt2freqCallBack(uint32_t capture, uint32_t clockFrequency, const tsDevIo *device)
{
    tsPwt2FreqParams *params       = device->parameters;
    const tsPwt2FreqConsts *consts = device->constants;
    uint8_t isSignalReady          = FALSE;
    uint64_t tempValue             = 0;
    uint32_t tempFreq              = 0;
    uint32_t dummyFreq             = 0;

    /* Filter the signal to avoid save a false signal *****************************************************************/
    if (consts->falseSignalFilterActive)
    {
        if ((((capture > (HALF_OF_PERIOD(params->prevCapture) - TOLERANCE_OF_HALF_PERIOD(params->prevCapture))) &&  // <there might be a false capture when half of the period
              (capture < (HALF_OF_PERIOD(params->prevCapture) + TOLERANCE_OF_HALF_PERIOD(params->prevCapture)))) || // due to signal will change direction(assumed %50 duty)>
             (capture < TOLERANCE_OF_JUST_AFTER_CAPTURE(params->prevCapture))) &&                                   // there might be an additional false capture just after the correct one.
            (params->consecutiveFalseSignalCnt < CONSECUTIVE_FALSE_SIGNAL_COUNT_LIMIT))                                        // there should be a limit to prevent any potential infinite loops.
        {
            params->consecutiveFalseSignalCnt++; // do nothing. It's a false signal.
        }
        else
        {
            if ((capture > (DOUBLE_OF_PERIOD(params->prevCapture) - TOLERANCE_OF_DOUBLE_PERIOD(params->prevCapture))) && // if period is doubled
                (capture < (DOUBLE_OF_PERIOD(params->prevCapture) + TOLERANCE_OF_DOUBLE_PERIOD(params->prevCapture))) && // there might be a missing true signal interrupt.
                (!(params->signalMissed)))
            {
                params->signalMissed = 1; // do nothing. It's a doubled value.
            }
            else
            {
                params->signalMissed     = 0;

                params->prevCapture = capture;
                isSignalReady = TRUE;
            }
            params->consecutiveFalseSignalCnt = 0;
        }
    }
    else
    {
        isSignalReady = TRUE;
    }
    //
    if(isSignalReady)
    {
        /* Calculate the frequence **********************************************************************************/
        tempValue = (consts->resolutionCoef != 0) ? ((uint64_t)(clockFrequency) * (uint64_t)(consts->resolutionCoef)) : (clockFrequency);

        if (capture != 0)
        {
            tempFreq = tempValue / (capture);
        }
        else
        {
            tempFreq = 0;
        }
        if (DEQUE_IS_FULL(params->queue))
        {
            dequePopFront(params->queue, &dummyFreq, SIZE_OF_FREQ); // Remove one
        }
        dequePushBack(params->queue, &tempFreq, SIZE_OF_FREQ); // Add a new one
        params->newValueFlag = TRUE;
    }
}

/// @brief  Init function for a pwt2freq
static DEV_IO_FUNC_INIT(pwt2freqInit)
{
    const tsPwt2FreqConsts *consts = device->constants;

    ADD_RCOS_VERSION("ra2/pwt2freq", "1.0.0")

    devIoInit(consts->pwtDevice, (void *)(consts->callbackFunc));//Init pwt device with giving callback function

    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a pwt2freq
static DEV_IO_FUNC_DEINIT(pwt2freqDeinit)
{
    const tsPwt2FreqConsts *consts = device->constants;

    devIoDeinit(consts->pwtDevice);

    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

/// @brief  Get function for a pwt2freq
static DEV_IO_FUNC_GET(pwt2freqGet)
{
    tsPwt2FreqParams *params = device->parameters;
    uint32_t returnValue     = 0;

    if (params->newValueFlag != FALSE)
    {
        uint32_t *buffer            = (uint32_t *)params->queue->buffer; // Calculate average of all values inside the queue
        uint64_t tempTotal          = 0;
        uint8_t bufferCaptureCount  = params->queue->count / SIZE_OF_FREQ;

        /* Calculate average frequency value from queue *************************************************************/
        while (bufferCaptureCount--)
        {
            tempTotal += *buffer++;
        }

        if (params->queue->count != 0)
        {
            returnValue = tempTotal / (params->queue->count / SIZE_OF_FREQ);
        }

        params->newValueFlag = FALSE; //Reset the flag to prevent to sending the same value
    }
    else
    {
        returnValue = 0;
    }

    return returnValue;
}

/** @} */

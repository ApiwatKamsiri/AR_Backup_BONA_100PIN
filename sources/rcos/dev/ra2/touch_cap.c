/** @file       touch_cap.c
 *  @brief      C file of TOUCH_CAP device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ayberk Tarcin
 *  @date       3.09.2021
 */
#define FILE_TOUCH_CAP_C

#include "touch_cap.h"

/**
 *  @addtogroup TOUCH_CAP
 *  @{
 */

static DEV_IO_FUNC_INIT(touchcapInit);
static DEV_IO_FUNC_DEINIT(touchcapDeinit);
static DEV_IO_FUNC_GET(touchcapGet);


/// @brief  Structure that defines the functions for touchcap
const tsDevIoFuncs devIoTouchcapFuncs =
{
    touchcapInit,
    touchcapDeinit,
    touchcapGet,
    NULL,
};

TIMER_CALLBACK_FUNC(touchcapISR)
{
    const tsDevIo *device          = parameter;
    const tsTouchcapConsts *consts = device->constants;
    tsTouchcapParams *params       = device->parameters;
    
    params->fsp_err = RM_TOUCH_ScanStart(consts->touch->p_ctrl);

    params->fsp_err = RM_TOUCH_DataGet(consts->touch->p_ctrl, &(params->buttonStatus), NULL, NULL);

    return consts->refreshTime;
}

/// @brief  Init function for a touchcap
static DEV_IO_FUNC_INIT(touchcapInit)
{
    tsTouchcapParams *params       = device->parameters;
    const tsTouchcapConsts *consts = device->constants;
    
    ADD_RCOS_VERSION("ra2/touch_cap", "1.0.0")

    UNUSED(config);

    params->fsp_err = RM_TOUCH_Open(consts->touch->p_ctrl, consts->touch->p_cfg);

    // Checks if Renesas Middleware is opened
    if (FSP_SUCCESS != params->fsp_err)
    {
        return EXIT_FAILURE;
    }

    if (consts->refreshTime == 0)
    {
        params->fsp_err = RM_TOUCH_ScanStart(consts->touch->p_ctrl);
    }
    else
    {
        timerCallBackStart(&params->timercb, consts->refreshTime);

    }

    // Checks if Renesas Middleware is refreshed
    if (FSP_SUCCESS != params->fsp_err)
    {
        return EXIT_FAILURE;
    }

    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a touchcap
static DEV_IO_FUNC_DEINIT(touchcapDeinit)
{
    tsTouchcapParams *params       = device->parameters;
    const tsTouchcapConsts *consts = device->constants;

    timerCallBackStop(&params->timercb);
    RM_TOUCH_Close (consts->touch->p_ctrl);

    device->sys->initialized = 0;
    return EXIT_SUCCESS;
}

/// @brief  Get function for a touchcap
static DEV_IO_FUNC_GET(touchcapGet)
{
    tsTouchcapParams *params       = device->parameters;

    return (uint32_t) params->buttonStatus;
}

/** @} */

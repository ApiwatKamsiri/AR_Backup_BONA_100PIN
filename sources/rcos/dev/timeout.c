/** @file       timeout.c
 *  @brief      C file of TIMEOUT device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       26.12.2019
 */
#define FILE_TIMEOUT_C

#include "timeout.h"

/**
 *  @addtogroup TIMEOUT
 *  @{
 */

static DEV_IO_FUNC_INIT(timeoutInit);
static DEV_IO_FUNC_DEINIT(timeoutDeinit);
static DEV_IO_FUNC_GET(timeoutGet);
static DEV_IO_FUNC_PUT(timeoutPut);

/// @brief  Structure that defines the functions for timeout
const tsDevIoFuncs devIoTimeoutFuncs =
    {
        timeoutInit,
        timeoutDeinit,
        timeoutGet,
        timeoutPut,
};

TIMER_CALLBACK_FUNC(timeoutTcbFunc)
{
    const tsDevIo *device   = parameter;
    tsTimeoutParams *params = device->parameters;

    if (params->value == 0)
    {
        return 0;
    }
    else
    {
        params->value--;
        return 1;
    }
}

/// @brief  Init function for a timeout
static DEV_IO_FUNC_INIT(timeoutInit)
{
    ADD_RCOS_VERSION("timeout", "2.0.0")

    UNUSED(config);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a timeout
static DEV_IO_FUNC_DEINIT(timeoutDeinit)
{
    tsTimeoutParams *params = device->parameters;

    device->sys->initialized = 0;
    timerCallBackStop(&params->tcb);
    params->value = 0;

    return EXIT_SUCCESS;
}

/// @brief  Get function for a timeout
static DEV_IO_FUNC_GET(timeoutGet)
{
    tsTimeoutParams *params = device->parameters;

    return params->value;
}

/// @brief  Put function for a timeout
static DEV_IO_FUNC_PUT(timeoutPut)
{
    tsTimeoutParams *params = device->parameters;

    params->value = data;
    if (data)
    {
        timerCallBackStart(&params->tcb, 1);
    }

    return EXIT_SUCCESS;
}

/** @} */

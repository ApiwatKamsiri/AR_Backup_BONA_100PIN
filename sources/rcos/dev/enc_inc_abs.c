/** @file       enc_inc_abs.c
 *  @brief      C file of ENC_INC_ABS device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       2.12.2019
 */
#define FILE_ENC_INC_ABS_C

#include "enc_inc_abs.h"

/**
 *  @addtogroup ENC_INC_ABS
 *  @{
 */

static DEV_IO_FUNC_INIT(encIncAbsInit);
static DEV_IO_FUNC_DEINIT(encIncAbsDeinit);
static DEV_IO_FUNC_GET(encIncAbsGet);
static DEV_IO_FUNC_PUT(encIncAbsPut);

/// @brief  Structure that defines the functions for encIncAbs
const tsDevIoFuncs devIoEncIncAbsFuncs =
    {
        encIncAbsInit,
        encIncAbsDeinit,
        encIncAbsGet,
        encIncAbsPut,
};

TIMER_CALLBACK_FUNC(encIncAbsCb)
{
    const tsDevIo *device     = parameter;
    tsEncIncAbsParams *params = device->parameters;
    uint32_t readValue        = devIoGet(params->encAbs);

    if (params->encPos != readValue)
    {
        int32_t diff;

        diff = readValue - params->encPos;
        if (diff < 0)
        {
            diff += params->stepCount;
        }
        if (diff > (params->stepCount / 2))
        {
            diff -= params->stepCount;
        }
        params->value += diff;
        params->encPos = readValue;
    }

    return params->timeout;
}

/// @brief  Init function for a encIncAbs
static DEV_IO_FUNC_INIT(encIncAbsInit)
{
    tsEncIncAbsParams *params = device->parameters;
    ADD_RCOS_VERSION("encincabs","2.0.0")

    UNUSED(config);
    devIoInit(params->encAbs, NULL);
    params->encPos = devIoGet(params->encAbs);
    timerCallBackStart(&params->tcb, params->timeout);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a encIncAbs
static DEV_IO_FUNC_DEINIT(encIncAbsDeinit)
{
    device->sys->initialized = 0;
    timerCallBackStop(&((tsEncIncAbsParams *)device->parameters)->tcb);

    return EXIT_SUCCESS;
}

/// @brief  Get function for a encIncAbs
static DEV_IO_FUNC_GET(encIncAbsGet)
{
    tsEncIncAbsParams *params = device->parameters;
    uint32_t retVal           = params->value;
    params->value             = 0;

    return retVal;
}

/// @brief  Put function for a encIncAbs
static DEV_IO_FUNC_PUT(encIncAbsPut)
{
    ((tsEncIncAbsParams *)device->parameters)->value = data;

    return EXIT_SUCCESS;
}

/** @} */

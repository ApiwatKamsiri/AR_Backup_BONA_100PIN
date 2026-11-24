/** @file       accelerate.c
 *  @brief      C file of ACCELERATE device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       25.12.2019
 */
#define FILE_ACCELERATE_C

#include "accelerate.h"

/**
 *  @addtogroup ACCELERATE
 *  @{
 */

static DEV_IO_FUNC_INIT(accelerateInit);
static DEV_IO_FUNC_DEINIT(accelerateDeinit);
static DEV_IO_FUNC_GET(accelerateGet);
static DEV_IO_FUNC_PUT(acceleratePut);

/// @brief  Structure that defines the functions for accelerate
const tsDevIoFuncs devIoAccelerateFuncs =
    {
        accelerateInit,
        accelerateDeinit,
        accelerateGet,
        acceleratePut,
};

/// @brief  Init function for a accelerate
static DEV_IO_FUNC_INIT(accelerateInit)
{
    const tsAccelerateConsts *consts = device->constants;
    ADD_RCOS_VERSION("accelerate", "2.0.0")

    UNUSED(config);
    devIoInit(consts->target, NULL);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a accelerate
static DEV_IO_FUNC_DEINIT(accelerateDeinit)
{
    tsAccelerateParams *params       = device->parameters;
    const tsAccelerateConsts *consts = device->constants;

    UNUSED(params);
    UNUSED(consts);
    device->sys->initialized = 0;
    acceleratePut(device, 0);

    return EXIT_SUCCESS;
}

/// @brief  Get function for a accelerate
static DEV_IO_FUNC_GET(accelerateGet)
{
    tsAccelerateParams *params = device->parameters;

    return params->acceleration;
}

/// @brief  Put function for a accelerate
static DEV_IO_FUNC_PUT(acceleratePut)
{
    tsAccelerateParams *params       = device->parameters;
    const tsAccelerateConsts *consts = device->constants;

    timerCallBackStop(&params->tcb);
    params->acceleration = data;
    if (data != 0)
    {
        timerCallBackStart(&params->tcb, consts->timeout);
    }

    return EXIT_SUCCESS;
}

TIMER_CALLBACK_FUNC(accelFunc)
{
    const tsDevIo *device            = parameter;
    tsAccelerateParams *params       = device->parameters;
    const tsAccelerateConsts *consts = device->constants;

    uint32_t data;

    data = devIoGet(consts->target) + params->acceleration;
    devIoPut(consts->target, data);

    return consts->timeout;
}

/** @} */

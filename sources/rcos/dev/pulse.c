/** @file       pulse.c
 *  @brief      Output a pulse for a certain duration
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       16.11.2018
 */
#define FILE_PULSE_C

/**
 *  @addtogroup PULSE
 *  @{
 */
#include "pulse.h"

static DEV_IO_FUNC_INIT(pulseInit);
static DEV_IO_FUNC_DEINIT(pulseDeinit);
static DEV_IO_FUNC_GET(pulseGet);
static DEV_IO_FUNC_PUT(pulsePut);

const tsDevIoFuncs devIoPulseFuncs =
    {
        pulseInit,
        pulseDeinit,
        pulseGet,
        pulsePut,
};

TIMER_CALLBACK_FUNC(pulseHandler)
{
    const tsDevIo *device       = parameter;
    const tsPulseConsts *consts = device->constants;

    devIoPut(consts->target, consts->offValue);

    return 0;
}

static DEV_IO_FUNC_INIT(pulseInit)
{
    tsPulseParams *params       = device->parameters;
    const tsPulseConsts *consts = device->constants;
    ADD_RCOS_VERSION("pulse", "2.0.0")

    UNUSED(config);

    timerCallBackStop(&params->timer);
    devIoInit(consts->target, NULL);
    devIoPut(consts->target, consts->offValue);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(pulseDeinit)
{
    tsPulseParams *params       = device->parameters;
    const tsPulseConsts *consts = device->constants;

    device->sys->initialized = 0;
    timerCallBackStop(&params->timer);
    devIoPut(consts->target, consts->offValue);

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(pulseGet)
{
    UNUSED(device);

    return 0;
}

static DEV_IO_FUNC_PUT(pulsePut)
{
    tsPulseParams *params       = device->parameters;
    const tsPulseConsts *consts = device->constants;

    if (data)
    {
        devIoPut(consts->target, consts->onValue);
        timerCallBackStart(&params->timer, data);
    }
    else
    {
        timerCallBackStop(&params->timer);
        devIoPut(consts->target, consts->offValue);
    }

    return EXIT_SUCCESS;
}

/** @} */

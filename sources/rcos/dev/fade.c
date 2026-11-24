/** @file       fade.c
 *  @brief      Fade in/out another devIo
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       16.11.2018
 */
#define FILE_FADE_C

/**
 *  @addtogroup FADE
 *  @{
 */
#include "fade.h"

static DEV_IO_FUNC_INIT(fadeInit);
static DEV_IO_FUNC_DEINIT(fadeDeinit);
static DEV_IO_FUNC_GET(fadeGet);
static DEV_IO_FUNC_PUT(fadePut);

const tsDevIoFuncs devIoFadeFuncs =
    {
        fadeInit,
        fadeDeinit,
        fadeGet,
        fadePut,
};

TIMER_CALLBACK_FUNC(fadeHandler)
{
    const tsDevIo *device      = parameter;
    tsFadeParams *params       = device->parameters;
    const tsFadeConsts *consts = device->constants;
    uint32_t curVal            = devIoGet(consts->target);

    if (params->current != curVal)
    {
        /// Someone else modified the target, hence finish operation
        params->finish  = curVal;
        params->current = curVal;
        return 0;
    }

    if (params->current == params->finish)
    {
        /// End of operation
        return 0;
    }

    /// Change output value
    if (params->current < params->finish)
    {
        params->current += consts->change;
    }
    else
    {
        params->current -= consts->change;
    }
    devIoPut(consts->target, params->current);

    return consts->duration;
}

static DEV_IO_FUNC_INIT(fadeInit)
{
    tsFadeParams *params       = device->parameters;
    const tsFadeConsts *consts = device->constants;
    ADD_RCOS_VERSION("fade", "2.0.0")

    UNUSED(config);

    timerCallBackStop(&params->timer);
    devIoInit(consts->target, NULL);
    params->current = devIoGet(consts->target);
    params->finish  = params->current;

    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(fadeDeinit)
{
    tsFadeParams *params       = device->parameters;
    const tsFadeConsts *consts = device->constants;

    device->sys->initialized = 0;

    timerCallBackStop(&params->timer);
    devIoPut(consts->target, 0);

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(fadeGet)
{
    const tsFadeConsts *consts = device->constants;

    return devIoGet(consts->target);
}

static DEV_IO_FUNC_PUT(fadePut)
{
    tsFadeParams *params       = device->parameters;
    const tsFadeConsts *consts = device->constants;
    int32_t diff;

    timerCallBackStop(&params->timer);

    if (params->finish == data)
    {
        params->current = params->finish;
    }
    else
    {
        params->current = devIoGet(consts->target);
        params->finish  = data;
    }

    diff = (int32_t)params->finish - params->current;          ///< Calculate difference
    diff -= (diff / (int32_t)consts->change) * consts->change; ///< Calculate fraction
    params->current += diff;                                   ///< Add fraction

    devIoPut(consts->target, params->current);            ///< Place current value to target
    timerCallBackStart(&params->timer, consts->duration); ///< Start transition

    return EXIT_SUCCESS;
}

/** @} */

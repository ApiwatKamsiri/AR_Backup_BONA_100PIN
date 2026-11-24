/** @file       led.c
 *  @brief      LED driver with on/off/blink options
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     
 *  @date       24.04.2019
 */
#define FILE_LED_C

/**
 *  @addtogroup LED
 *  @{
 */
#include "led.h"

const tsPatternNode ledPatternFastOnOff[] =
    {
        PATTERN_NODE(LED_FULL_ON_DUTY, LED_FAST_ON_OFF_LONG_ON_TIME),
        PATTERN_NODE(LED_FULL_OFF_DUTY, LED_FAST_ON_OFF_OFF_TIME),
        PATTERN_NODE(LED_FULL_ON_DUTY, LED_FAST_ON_OFF_SHORT_ON_TIME),
        PATTERN_NODE(LED_FULL_OFF_DUTY, LED_FAST_ON_OFF_OFF_TIME),
        PATTERN_NODE_REPEAT(),
};

static DEV_IO_FUNC_INIT(ledInit);
static DEV_IO_FUNC_DEINIT(ledDeinit);
static DEV_IO_FUNC_GET(ledGet);
static DEV_IO_FUNC_PUT(ledPut);

const tsDevIoFuncs devIoLedFuncs =
    {
        ledInit,
        ledDeinit,
        ledGet,
        ledPut,
};

static DEV_IO_FUNC_INIT(ledInit)
{
    const tsLedConsts *consts = device->constants;
    ADD_RCOS_VERSION("led", "2.0.0")

    UNUSED(config);
    devIoInit(consts->io, NULL);
    devIoInit(consts->fade, NULL);
    devIoInit(consts->pattern, NULL);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(ledDeinit)
{
    const tsLedConsts *consts = device->constants;

    device->sys->initialized = 0;
    devIoDeinit(consts->io);
    devIoDeinit(consts->fade);
    devIoDeinit(consts->pattern);

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(ledGet)
{
    tsLedParams *params = device->parameters;

    return params->state;
}

static DEV_IO_FUNC_PUT(ledPut)
{
    tsLedParams *params       = device->parameters;
    const tsLedConsts *consts = device->constants;

    if (params->state != data) // Capture change
    {
        params->state = data;

        // Prepare generic settings
        devIoPut(consts->pattern, 0);
        ((tsPatternParams *)consts->pattern->parameters)->target = consts->io;

        switch (params->state)
        {
            default:
            case eLdtFullOff:
                devIoPut(consts->io, LED_FULL_OFF_DUTY);
                break;
            case eLdtFullOn:
                devIoPut(consts->io, LED_FULL_ON_DUTY);
                break;
            case eLdtFullOnOff:
                params->blinkPattern[0].value = LED_FULL_ON_DUTY;
                params->blinkPattern[0].time  = LED_FULL_ON_OFF_ON_TIME;
                params->blinkPattern[1].value = LED_FULL_OFF_DUTY;
                params->blinkPattern[1].time  = LED_FULL_ON_OFF_OFF_TIME;
                devIoPut(consts->pattern, (uint32_t)params->blinkPattern);
                break;
            case eLdtFadeInOut:
                ((tsPatternParams *)consts->pattern->parameters)->target = consts->fade;

                params->blinkPattern[0].value = LED_FULL_ON_DUTY;
                params->blinkPattern[0].time  = LED_FADE_IN_TIME + LED_FADE_IN_WAIT_REGION;
                params->blinkPattern[1].value = LED_FULL_OFF_DUTY;
                params->blinkPattern[1].time  = LED_FADE_IN_TIME + LED_FADE_IN_WAIT_REGION;
                devIoPut(consts->pattern, (uint32_t)params->blinkPattern);
                break;
            case eLdtFastOnOff:
                devIoPut(consts->pattern, (uint32_t)ledPatternFastOnOff);
                break;
            case eLdtSlowOnOff:
                params->blinkPattern[0].value = LED_FULL_ON_DUTY;
                params->blinkPattern[0].time  = LED_SLOW_ON_OFF_ON_TIME;
                params->blinkPattern[1].value = LED_FULL_OFF_DUTY;
                params->blinkPattern[1].time  = LED_SLOW_ON_OFF_OFF_TIME;
                devIoPut(consts->pattern, (uint32_t)params->blinkPattern);
                break;
            case eLdtHalfOnOff:
                params->blinkPattern[0].value = LED_FULL_ON_DUTY;
                params->blinkPattern[0].time  = LED_HALF_ON_OFF_ON_TIME;
                params->blinkPattern[1].value = LED_FULL_OFF_DUTY;
                params->blinkPattern[1].time  = LED_HALF_ON_OFF_OFF_TIME;
                devIoPut(consts->pattern, (uint32_t)params->blinkPattern);
                break;
            case eLdtSoftOn:
                devIoPut(consts->io, LED_SOFT_DUTY);
                break;
            case eLdtSoftOnOff:
                params->blinkPattern[0].value = LED_SOFT_DUTY;
                params->blinkPattern[0].time  = LED_SOFT_ON_OFF_ON_TIME;
                params->blinkPattern[1].value = LED_FULL_OFF_DUTY;
                params->blinkPattern[1].time  = LED_SOFT_ON_OFF_OFF_TIME;
                devIoPut(consts->pattern, (uint32_t)params->blinkPattern);
                break;
        }
    }

    return EXIT_SUCCESS;
}

/** @} */

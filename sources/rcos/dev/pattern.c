/** @file       pattern.c
 *  @brief      Output a pattern which is an array of value with timings
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       16.11.2018
 */
#define FILE_PATTERN_C

/**
 *  @addtogroup PATTERN
 *  @{
 */
#include "pattern.h"

#define OVERTIME_LIMIT 15 ///< Bit count for tsPatternNode.time values that are low enough to operate using normal timerCallback operation

static DEV_IO_FUNC_INIT(patternInit);
static DEV_IO_FUNC_DEINIT(patternDeinit);
static DEV_IO_FUNC_GET(patternGet);
static DEV_IO_FUNC_PUT(patternPut);

const tsDevIoFuncs devIoPatternFuncs =
    {
        patternInit,
        patternDeinit,
        patternGet,
        patternPut,
};

TIMER_CALLBACK_FUNC(patternPutHandler)
{
    const tsDevIo *device         = parameter;
    tsPatternParams *params       = device->parameters;
    const tsPatternConsts *consts = device->constants;

    UNUSED(consts);

    /// Go to next node in list
    params->node++;
    if (params->node->time == (uint32_t)-1ul)
    {
        /// PATTERN_NODE_REPEAT
        params->node = params->list; /// Go to beginning
    }

    devIoPut(params->target, params->node->value); ///< Output next node value
    return params->node->time;                     ///< Update time
}

static DEV_IO_FUNC_INIT(patternInit)
{
    tsPatternParams *params       = device->parameters;
    const tsPatternConsts *consts = device->constants;
    ADD_RCOS_VERSION("pattern", "2.0.0")

    UNUSED(config);

    timerCallBackStop(&params->timer);
    params->list = NULL;
    params->node = params->list;
    devIoInit(params->target, NULL);
    devIoPut(params->target, consts->offValue);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(patternDeinit)
{
    tsPatternParams *params       = device->parameters;
    const tsPatternConsts *consts = device->constants;

    UNUSED(consts);

    device->sys->initialized = 0;
    timerCallBackStop(&params->timer);
    params->list = NULL;
    params->node = params->list;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(patternGet)
{
    tsPatternParams *params       = device->parameters;
    const tsPatternConsts *consts = device->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    return 0;
}

static DEV_IO_FUNC_PUT(patternPut)
{
    tsPatternParams *params       = device->parameters;
    const tsPatternConsts *consts = device->constants;

    timerCallBackStop(&params->timer);

    if (data)
    {
        params->list = (tsPatternNode *)data;
        params->node = params->list;

        /// Output first node value
        devIoPut(params->target, params->node->value);

        /// Update time
        if (params->node->time)
        {
            timerCallBackStart(&params->timer, params->node->time);
        }
    }
    else
    {
        /// Output offValue
        devIoPut(params->target, consts->offValue);
    }

    return EXIT_SUCCESS;
}

/** @} */

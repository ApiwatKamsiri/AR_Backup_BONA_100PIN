/** @file       debounce.c
 *  @brief      Debounce engine
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer, Edited: Muhammed Sairoglu
 *  @date       7.12.2018
 */
#define FILE_DEBOUNCE_C

/**
 *  @addtogroup DEBOUNCE
 *  @{
 */
#include "debounce_patch.h"

static DEV_CPX_FUNC_INIT(debounceInit);
static DEV_CPX_FUNC_DEINIT(debounceDeinit);

const tsDevCpxFuncs devCpxDebounceFuncs =
    {
        debounceInit,
        debounceDeinit,
};

TIMER_CALLBACK_FUNC(debounceCheck)
{
    const tsDevCpx *device = parameter;
    tsDebounceParams *params = device->parameters;
    tsDebounceItem *ioItem = params->ioList;

    for (int idx = params->ioCount; idx; idx--, ioItem++)
    {
        if (ioItem->initialized)
        {
            uint32_t current = devIoGet(ioItem->devIo);

            if (ioItem->stable != current)
            {
                if (ioItem->changing == current)
                {
                    if (--ioItem->count == 0)
                    {
                        ioItem->stable = current;
                    }
                }
                else
                {
                    ioItem->changing = current;
                    ioItem->count = ioItem->limit;
                }
            }
        }
    } // for (int idx = params->ioCount; idx; idx--, ioItem++)

    return params->period;
}

static DEV_CPX_FUNC_INIT(debounceInit)
{
    tsDebounceParams *params = device->parameters;
    //int idx;
    ADD_RCOS_VERSION("debounce", "2.1.0")

    UNUSED(config); // REMOVE IF USED

    // Muhammed Edit: Don't initialize inputs when core is initialized
    // Initialize them only if their init function is called
    // for (idx = 0; idx < params->ioCount; idx++)
    // {
    //     devIoInit(params->ioList[idx].devIo, NULL);
    //     params->ioList[idx].stable = devIoGet(params->ioList[idx].devIo); // Fill initial values
    // }
    timerCallBackStart(&params->timer, params->period); // Start timer
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_CPX_FUNC_DEINIT(debounceDeinit)
{
    tsDebounceParams *params = device->parameters;

    device->sys->initialized = 0;
    timerCallBackStop(&params->timer);

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_INIT(debIpInit);
static DEV_IO_FUNC_DEINIT(debIpDeinit);
static DEV_IO_FUNC_GET(debIpGet);

const tsDevIoFuncs devIoDebIpFuncs =
    {
        debIpInit,
        debIpDeinit,
        debIpGet,
        NULL,
};

static DEV_IO_FUNC_INIT(debIpInit)
{
    tsDebIpParams *params = device->parameters;
    tsDebounceParams *debParams = params->debounce->parameters;

    UNUSED(config); // REMOVE IF USED
    device->sys->initialized = 1;

    size_t i = params->idx;
    devIoInit(debParams->ioList[i].devIo, NULL);
    debParams->ioList[i].stable = devIoGet(debParams->ioList[i].devIo); // Fill initial value
    debParams->ioList[i].initialized = true;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(debIpDeinit)
{
    tsDebIpParams *params = device->parameters;
    tsDebounceParams *debParams = params->debounce->parameters;

    device->sys->initialized = 0;

    size_t i = params->idx;
    devIoDeinit(debParams->ioList[i].devIo);
    debParams->ioList[i].initialized = false;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(debIpGet)
{
    tsDebIpParams *params = device->parameters;
    tsDebounceParams *debParams = params->debounce->parameters;

    return debParams->ioList[params->idx].stable;
}

/** @} */

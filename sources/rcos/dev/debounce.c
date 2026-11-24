/** @file       debounce.c
 *  @brief      Debounce engine
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       7.12.2018
 *  @author     Burak OZAN
 *  @date       6.11.2022
 */
#define FILE_DEBOUNCE_C

/**
 *  @addtogroup DEBOUNCE
 *  @{
 */
#include "debounce.h"

static DEV_CPX_FUNC_INIT(debounceInit);
static DEV_CPX_FUNC_DEINIT(debounceDeinit);

const tsDevCpxFuncs devCpxDebounceFuncs =
    {
        debounceInit,
        debounceDeinit,
};

TIMER_CALLBACK_FUNC(debounceCheck)
{
    const tsDevCpx *device   = parameter;
    tsDebounceParams *params = device->parameters;
    tsDebounceItem *ioItem   = params->ioList;

    for (int idx = params->ioCount; idx; idx--, ioItem++)
    {
        uint32_t current = devIoGet(ioItem->devIo);

        if (ioItem->stable != current)
        {
            if (ioItem->changing == current)
            {
                if (ioItem->stabilityCnt)
                {
                    --ioItem->stabilityCnt;
                }
                if (ioItem->stabilityCnt == 0)
                {
                    ioItem->stable        = current;
                    ioItem->hysteresisCnt = 0; // Measurement completed, so set 0 for new changing
                }
            }
            else
            {
                if (ioItem->hysteresisCnt == 0)
                { //If hysteresisCnt is not zero, no change until hysteresis reaches 0
                    //Calculating hysteresis counter value
                    if (ioItem->hysteresisPer < DEB_HYS_PER_MAX_VAL)
                    {
                        ioItem->hysteresisCnt = ((uint16_t)(ioItem->limit) * (DEB_HYS_PER_MAX_VAL - (uint16_t)(ioItem->hysteresisPer))) / DEB_HYS_PER_MAX_VAL;
                    }
                    else
                    {
                        ioItem->hysteresisCnt = 0;
                    }
                    //Setting new changing value and set the counter
                    ioItem->changing     = current;
                    ioItem->stabilityCnt = ioItem->limit - ioItem->hysteresisCnt; //Stability is limit - hysteresis
                }
                else
                {
                    ioItem->hysteresisCnt--;
                }
            }
        }
        else if (ioItem->changing != ioItem->stable)
        {
            if (ioItem->hysteresisCnt == 0)
            { //If hysteresisCnt is not zero, no change until hysteresis reaches 0
                ioItem->changing     = ioItem->stable;
                ioItem->stabilityCnt = 0;
            }
            else
            {
                ioItem->hysteresisCnt--;
            }
        }

    } // for (int idx = params->ioCount; idx; idx--, ioItem++)

    return params->period;
}

static DEV_CPX_FUNC_INIT(debounceInit)
{
    tsDebounceParams *params = device->parameters;
    int idx;
    ADD_RCOS_VERSION("debounce","2.0.2")

    UNUSED(config); // REMOVE IF USED

    for (idx = 0; idx < params->ioCount; idx++)
    {
        devIoInit(params->ioList[idx].devIo, NULL);
        params->ioList[idx].stable = devIoGet(params->ioList[idx].devIo); // Fill initial values
    }
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

static DEV_IO_FUNC_GET(debIpGet);

const tsDevIoFuncs devIoDebIpFuncs =
    {
        NULL,
        NULL,
        debIpGet,
        NULL,
};

static DEV_IO_FUNC_GET(debIpGet)
{
    tsDebIpParams *params       = device->parameters;
    tsDebounceParams *debParams = params->debounce->parameters;

    return debParams->ioList[params->idx].stable;
}

/** @} */

/** @file       contdebounce.c
 *  @brief      Continuous debounce engine
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Burak OZAN
 *  @date       7.11.2022
 */
#define FILE_CONT_DEBOUNCE_C

/**
 *  @addtogroup CONTINUOUS DEBOUNCE
 *  @{
 */
#include "contdebounce.h"

static DEV_CPX_FUNC_INIT(contDebounceInit);
static DEV_CPX_FUNC_DEINIT(contDebounceDeinit);

const tsDevCpxFuncs devCpxContDebounceFuncs =
    {
        contDebounceInit,
        contDebounceDeinit,
};

TIMER_CALLBACK_FUNC(contDebounceCheck)
{
    const tsDevCpx *device       = parameter;
    tsContDebounceParams *params = device->parameters;
    tsContDebounceItem *ioItem   = params->ioList;

    for (int idx = params->ioCount; idx; idx--, ioItem++)
    {
        //Reset continuous debounce parameters
        ioItem->changing      = ioItem->stable;
        ioItem->stabilityCnt  = 0;
        ioItem->hysteresisCnt = 0;
        
        for (int idx2 = ioItem->limit; idx2; idx2--)
        {//Reading continuous
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
                        break;
                    }
                }
                else
                {
                    if (ioItem->hysteresisCnt == 0)
                    { //If hysteresisCnt is not zero, no change until hysteresis reaches 0
                        //Calculating hysteresis counter value
                        if (ioItem->hysteresisPer < CONT_DEB_HYS_PER_MAX_VAL)
                        {
                            ioItem->hysteresisCnt = ((uint16_t)(ioItem->limit) * (CONT_DEB_HYS_PER_MAX_VAL - (uint16_t)(ioItem->hysteresisPer))) / CONT_DEB_HYS_PER_MAX_VAL;
                        }
                        else
                        {
                            ioItem->hysteresisCnt = 0;
                        }
                        //Setting new changing value and set the counter
                        ioItem->changing     = current;
                        ioItem->stabilityCnt = ioItem->limit - ioItem->hysteresisCnt; //Stability is limit - hysteresis
                        if (ioItem->stabilityCnt)
                        {
                            --ioItem->stabilityCnt;//first read
                        }
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
                    break;
                }
                else
                {
                    ioItem->hysteresisCnt--;
                }
            }
        } // for (int idx2 = ioItem->limit; idx2; idx2--)

    } // for (int idx = params->ioCount; idx; idx--, ioItem++)

    return params->period;
}

static DEV_CPX_FUNC_INIT(contDebounceInit)
{
    tsContDebounceParams *params = device->parameters;
    int idx;
    ADD_RCOS_VERSION("contdebounce", "1.0.0")

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

static DEV_CPX_FUNC_DEINIT(contDebounceDeinit)
{
    tsContDebounceParams *params = device->parameters;

    device->sys->initialized = 0;
    timerCallBackStop(&params->timer);

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(contDebIpGet);

const tsDevIoFuncs devIoContDebIpFuncs =
    {
        NULL,
        NULL,
        contDebIpGet,
        NULL,
};

static DEV_IO_FUNC_GET(contDebIpGet)
{
    tsContDebIpParams *params           = device->parameters;
    tsContDebounceParams *contDebParams = params->contDebounce->parameters;

    return contDebParams->ioList[params->idx].stable;
}

/** @} */

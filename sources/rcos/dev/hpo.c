/** @file       hpo.c
 *  @brief      C file of HPO device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       30.01.2020
 */
#define FILE_HPO_C

#include "hpo.h"

/**
 *  @addtogroup HPO
 *  @{
 */

static DEV_IO_FUNC_INIT(hpoInit);
static DEV_IO_FUNC_DEINIT(hpoDeinit);
static DEV_IO_FUNC_GET(hpoGet);
static DEV_IO_FUNC_PUT(hpoPut);

/// @brief  Structure that defines the functions for hpo
const tsDevIoFuncs devIoHpoFuncs =
    {
        hpoInit,
        hpoDeinit,
        hpoGet,
        hpoPut,
};

static uint32_t shiftMask(uint32_t mask)
{
    mask >>= 1; // MSB to LSB
    if (mask)
    {
        return mask;
    }
    else
    {
        return BIT((sizeof(mask) * 8) - 1); // return to MSB
    }
}

static void hpioHandler(const tsDevIo *device)
{
    tsHpoParams *params = device->parameters;
    uint32_t diff       = params->expected ^ params->current;

    if (0 != diff)
    {
        while ((params->mask & diff) == 0) // If masked bit does not have a change
        {
            params->mask = shiftMask(params->mask); // Shift mask to next one
        }
        params->current &= ~params->mask;                   // Clear current bit
        params->current |= params->expected & params->mask; // Place expected bit
        devIoPut(params->target, params->current);          // Put new value to target
        params->mask = shiftMask(params->mask);             // Next time hpioHandler called check for the next bit
    }
    else
    {
        params->mask = shiftMask(0); // Default mask value
    }
}

TIMER_CALLBACK_FUNC(hpioWrapperTcb)
{
    const tsDevIo *device = parameter;

    hpioHandler(device);

    return ((tsHpoParams *)device->parameters)->timeout;
}

void hpioWrapperZc(const tsDevIo *device)
{
    tsHpoParams *params = device->parameters;
    uint32_t diff       = params->expected ^ params->current;

    if (0 != diff)
    {
        if (params->counter)
        {
            --params->counter;
        }
        if (0 == params->counter)
        {
            hpioHandler(device);
            params->counter = params->zcCnt;
        }
    }
    else
    {
        params->counter = 0;
    }
}

/// @brief  Init function for a hpo
static DEV_IO_FUNC_INIT(hpoInit)
{
    tsHpoParams *params = device->parameters;
    ADD_RCOS_VERSION("hpo", "2.0.1")

    UNUSED(config);
    devIoInit(params->target, NULL);             // Initialise target
    params->current  = devIoGet(params->target); // Get first value
    params->expected = params->current;          // First value should be expected
    if (params->timeout)
    {
        timerCallBackStart(&params->tcb, params->timeout);
    }
    else if (params->zcCnt)
    {
        devCpxInit(params->isr, (void *)params->selfIsr);
    }
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a hpo
static DEV_IO_FUNC_DEINIT(hpoDeinit)
{
    tsHpoParams *params = device->parameters;

    device->sys->initialized = 0;
    if (params->timeout)
    {
        timerCallBackStop(&params->tcb);
    }
    else if (params->zcCnt)
    {
        devCpxDeinit(params->isr);
    }
    params->current = params->expected;        // Match current with expected value
    devIoPut(params->target, params->current); // Put new value to target

    return EXIT_SUCCESS;
}

/// @brief  Get function for a hpo
static DEV_IO_FUNC_GET(hpoGet)
{
    tsHpoParams *params = device->parameters;

    return params->expected;
}

/// @brief  Put function for a hpo
static DEV_IO_FUNC_PUT(hpoPut)
{
    tsHpoParams *params = device->parameters;

    params->expected = data;

    return EXIT_SUCCESS;
}

/** @} */

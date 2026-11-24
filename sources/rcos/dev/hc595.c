/** @file       hc595.c
 *  @brief      8-bit serial-in, serial or parallel-out shift register with output latches; 3-state
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       2.04.2019
 */
#define FILE_HC595_C

/**
 *  @addtogroup HC595
 *  @{
 */
#include "hc595.h"

static DEV_IO_FUNC_INIT(hc595Init);
static DEV_IO_FUNC_DEINIT(hc595Deinit);
static DEV_IO_FUNC_GET(hc595Get);
static DEV_IO_FUNC_PUT(hc595Put);

const tsDevIoFuncs devIoHc595Funcs =
    {
        hc595Init,
        hc595Deinit,
        hc595Get,
        hc595Put,
};

static DEV_IO_FUNC_INIT(hc595Init)
{
    tsHc595Params *params       = device->parameters;
    const tsHc595Consts *consts = device->constants;
    ADD_RCOS_VERSION("hc595", "2.0.0")

    UNUSED(config); // REMOVE IF USED

    if (consts->oe) // Disable outputs
    {
        devIoInit(consts->oe, NULL);
        devIoPut(consts->oe, 0);
    }
    devIoInit(consts->shcp, NULL);
    devIoPut(consts->shcp, 0);
    devIoInit(consts->stcp, NULL);
    devIoPut(consts->stcp, 0);
    devIoInit(consts->ds, NULL);
    devIoPut(consts->ds, 0);
    if (consts->mr) // Clear buffers
    {
        devIoInit(consts->mr, NULL);
        devIoPut(consts->mr, 1);
        devIoPut(consts->mr, 0);
    }

    hc595Put(device, params->lastValue);

    if (consts->oe) // Enable outputs
    {
        devIoPut(consts->oe, 1);
    }
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(hc595Deinit)
{
    const tsHc595Consts *consts = device->constants;

    device->sys->initialized = 0;

    if (consts->oe) // Disable outputs
    {
        devIoPut(consts->oe, 0);
    }

    if (consts->mr) // Clear buffers
    {
        devIoPut(consts->mr, 1);
        devIoPut(consts->mr, 0);
    }

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(hc595Get)
{
    tsHc595Params *params       = device->parameters;

    return params->lastValue;
}

static DEV_IO_FUNC_PUT(hc595Put)
{
    tsHc595Params *params       = device->parameters;
    const tsHc595Consts *consts = device->constants;
    uint32_t mask;

    params->lastValue = data; // Remember last data placed
    data ^= consts->actLevels; // XOR active low signals
    mask = 1ul << (consts->cascade * 8 + 7);

    for (; mask; mask >>= 1)
    {
        devIoPut(consts->ds, (data & mask) ? 1 : 0); // Place a single bit into data pin
        devIoPut(consts->shcp, 1);                   // Push ds into shift register
        devIoPut(consts->shcp, 0);
    }

    devIoPut(consts->stcp, 1); // Place shift register to latch
    devIoPut(consts->stcp, 0);

    return EXIT_SUCCESS;
}

/** @} */

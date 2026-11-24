/** @file       lut.c
 *  @brief      C file of LUT device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       20.01.2020
 */
#define FILE_LUT_C

#include "lut.h"

/**
 *  @addtogroup LUT
 *  @{
 */

static DEV_IO_FUNC_INIT(lutInit);
static DEV_IO_FUNC_DEINIT(lutDeinit);
static DEV_IO_FUNC_GET(lutGet);
static DEV_IO_FUNC_PUT(lutPut);

/// @brief  Structure that defines the functions for lut
const tsDevIoFuncs devIoLutFuncs =
    {
        lutInit,
        lutDeinit,
        lutGet,
        lutPut,
};

/// @brief  Init function for a lut
static DEV_IO_FUNC_INIT(lutInit)
{
    const tsLutConsts *consts = device->constants;
    ADD_RCOS_VERSION("lut", "2.0.0")

    UNUSED(config);
    devIoInit(consts->target, NULL);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a lut
static DEV_IO_FUNC_DEINIT(lutDeinit)
{
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

/// @brief  Get function for a lut
static DEV_IO_FUNC_GET(lutGet)
{
    tsLutParams *params       = device->parameters;
    const tsLutConsts *consts = device->constants;
    uint32_t currentValue     = devIoGet(consts->target);

    for (uint32_t idx = 0; idx < consts->lutSize; idx++)
    {
        if (currentValue == consts->lut[idx])
        {
            params->lastIdx = idx;
            break;
        }
    }

    return params->lastIdx;
}

/// @brief  Put function for a lut
static DEV_IO_FUNC_PUT(lutPut)
{
    tsLutParams *params       = device->parameters;
    const tsLutConsts *consts = device->constants;

    if (data < consts->lutSize)
    {
        params->lastIdx = data;
        devIoPut(consts->target, consts->lut[params->lastIdx]);
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

/** @} */

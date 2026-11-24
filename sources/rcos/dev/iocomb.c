/** @file       iocomb.c
 *  @brief      C file of IOCOMB device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       16.11.2018
 */
#define FILE_IOCOMB_C

/**
 *  @addtogroup IOCOMB
 *  @{
 */
#include "iocomb.h"

static DEV_IO_FUNC_INIT(iocombInit);
static DEV_IO_FUNC_DEINIT(iocombDeinit);
static DEV_IO_FUNC_GET(iocombGet);
static DEV_IO_FUNC_PUT(iocombPut);

const tsDevIoFuncs devIoIocombFuncs =
    {
        iocombInit,
        iocombDeinit,
        iocombGet,
        iocombPut,
};

static DEV_IO_FUNC_INIT(iocombInit)
{
    const tsIocombConsts *consts = device->constants;
    uint8_t idx;
    ADD_RCOS_VERSION("iocomb", "2.0.0")

    UNUSED(config); // REMOVE IF USED

    for (idx = 0; idx < consts->count; idx++)
    {
        devIoInit(consts->list[idx].device, NULL);
    }
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(iocombDeinit)
{
    const tsIocombConsts *consts = device->constants;
    uint8_t idx;

    device->sys->initialized = 0;
    for (idx = 0; idx < consts->count; idx++)
    {
        devIoDeinit(consts->list[idx].device);
    }

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(iocombGet)
{
    const tsIocombConsts *consts = device->constants;
    uint32_t returnVal           = 0;

    for (int idx = consts->count - 1; idx >= 0; idx--)
    {
        returnVal <<= consts->list[idx].bitCount;
        returnVal |= devIoGet(consts->list[idx].device) & consts->list[idx].mask;
    }

    return returnVal;
}

static DEV_IO_FUNC_PUT(iocombPut)
{
    const tsIocombConsts *consts = device->constants;

    for (int idx = 0; idx < consts->count; idx++)
    {
        if ((data ^ devIoGet(consts->list[idx].device)) & consts->list[idx].mask)
        {
            devIoPut(consts->list[idx].device, data & consts->list[idx].mask);
        }
        data >>= consts->list[idx].bitCount;
    }

    return EXIT_SUCCESS;
}

/** @} */

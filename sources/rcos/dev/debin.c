/** @file       debin.c
 *  @brief      Debounce filter for input IO
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       7.01.2019
 */
#define FILE_DEBIN_C

/**
 *  @addtogroup DEBIN
 *  @{
 */
#include "debin.h"

static DEV_IO_FUNC_INIT(debinInit);
static DEV_IO_FUNC_DEINIT(debinDeinit);
static DEV_IO_FUNC_GET(debinGet);

const tsDevIoFuncs devIoDebinFuncs =
    {
        debinInit,
        debinDeinit,
        debinGet,
        NULL,
};

static DEV_IO_FUNC_INIT(debinInit)
{
    tsDebinParams *params = device->parameters;
    ADD_RCOS_VERSION("debin", "2.0.0")

    devIoInit(params->target, NULL);
    params->stable = devIoGet(params->target);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(debinDeinit)
{
    tsDebinParams *params = device->parameters;

    device->sys->initialized = 0;
    devIoDeinit(params->target);

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(debinGet)
{
    tsDebinParams *params = device->parameters;
    uint32_t current      = devIoGet(params->target);

    if (params->stable != current)
    {
        if (params->changing == current)
        {
            if (--params->count == 0)
            {
                params->stable = current;
            }
        }
        else
        {
            params->changing = current;
            params->count    = params->limit;
        }
    }

    return params->stable;
}

/** @} */

/** @file       iovirtual.c
 *  @brief      A simple device to access a data of type uint32_t
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       4.02.2019
 */
#define FILE_IOVIRTUAL_C

/**
 *  @addtogroup IOVIRTUAL
 *  @{
 */
#include "iovirtual.h"

static DEV_IO_FUNC_INIT(iovirtualInit);
static DEV_IO_FUNC_DEINIT(iovirtualDeinit);
static DEV_IO_FUNC_GET(iovirtualGet);
static DEV_IO_FUNC_PUT(iovirtualPut);

const tsDevIoFuncs devIoIovirtualFuncs =
    {
        iovirtualInit,
        iovirtualDeinit,
        iovirtualGet,
        iovirtualPut,
};

static DEV_IO_FUNC_INIT(iovirtualInit)
{
    ADD_RCOS_VERSION("iovirtual", "2.0.0")

    UNUSED(config); // REMOVE IF USED
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(iovirtualDeinit)
{
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(iovirtualGet)
{
    return *(uint32_t *)device->parameters;
}

static DEV_IO_FUNC_PUT(iovirtualPut)
{
    *(uint32_t*)device->parameters = data;

    return EXIT_SUCCESS;
}

/** @} */

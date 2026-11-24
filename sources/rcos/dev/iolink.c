/** @file       iolink.c
 *  @brief      A simple device to access a data of type uint32_t
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       4.02.2019
 */
#define FILE_IOLINK_C

/**
 *  @addtogroup IOLINK
 *  @{
 */
#include "iolink.h"
#include <string.h>

static DEV_IO_FUNC_INIT(iolinkInit);
static DEV_IO_FUNC_DEINIT(iolinkDeinit);
static DEV_IO_FUNC_GET(iolinkGet);
static DEV_IO_FUNC_PUT(iolinkPut);

const tsDevIoFuncs devIoIolinkFuncs =
    {
        iolinkInit,
        iolinkDeinit,
        iolinkGet,
        iolinkPut,
};

static DEV_IO_FUNC_INIT(iolinkInit)
{
    UNUSED(config); // REMOVE IF USED
    device->sys->initialized = 1;
    ADD_RCOS_VERSION("iolink", "2.0.0")

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(iolinkDeinit)
{
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(iolinkGet)
{
    const tsIolinkConsts *consts = device->constants;
    uint32_t retVal = 0;

    memcpy(&retVal, consts->target, consts->size);

    return retVal;
}

static DEV_IO_FUNC_PUT(iolinkPut)
{
    const tsIolinkConsts *consts = device->constants;

    memcpy(consts->target, &data, consts->size);

    return EXIT_SUCCESS;
}

/** @} */

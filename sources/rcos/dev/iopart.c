/** @file       iopart.c
 *  @brief      Io partitioning module allows access to a partition of a devIo
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       22.01.2019
 */
#define FILE_IOPART_C

/**
 *  @addtogroup IOPART
 *  @{
 */
#include "iopart.h"

static DEV_IO_FUNC_INIT(iopartInit);
static DEV_IO_FUNC_DEINIT(iopartDeinit);
static DEV_IO_FUNC_GET(iopartGet);
static DEV_IO_FUNC_PUT(iopartPut);

const tsDevIoFuncs devIoIopartFuncs =
    {
        iopartInit,
        iopartDeinit,
        iopartGet,
        iopartPut,
};

static DEV_IO_FUNC_INIT(iopartInit)
{
    const tsIopartConsts *consts = device->constants;
    ADD_RCOS_VERSION("iopart", "2.0.0")

    UNUSED(config); // REMOVE IF USED
    devIoInit(consts->target, NULL);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(iopartDeinit)
{
    const tsIopartConsts *consts = device->constants;

    device->sys->initialized = 0;
    devIoDeinit(consts->target);

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(iopartGet)
{
    const tsIopartConsts *consts = device->constants;

    return ((devIoGet(consts->target) & consts->mask) >> consts->shift);
}

static DEV_IO_FUNC_PUT(iopartPut)
{
    const tsIopartConsts *consts = device->constants;
    uint32_t value;

    value = devIoGet(consts->target);
    value &= ~consts->mask;
    value |= (data << consts->shift) & consts->mask;
    devIoPut(consts->target, value);

    return EXIT_SUCCESS;
}

/** @} */

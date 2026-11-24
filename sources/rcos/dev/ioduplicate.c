/** @file       ioduplicate.c
 *  @brief      C file of IODUPLICATE device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       3.12.2019
 */
#define FILE_IODUPLICATE_C

#include "ioduplicate.h"

/**
 *  @addtogroup IODUPLICATE
 *  @{
 */

static DEV_IO_FUNC_INIT(ioduplicateInit);
static DEV_IO_FUNC_DEINIT(ioduplicateDeinit);
static DEV_IO_FUNC_GET(ioduplicateGet);
static DEV_IO_FUNC_PUT(ioduplicatePut);

/// @brief  Structure that defines the functions for ioduplicate
const tsDevIoFuncs devIoIoduplicateFuncs =
    {
        ioduplicateInit,
        ioduplicateDeinit,
        ioduplicateGet,
        ioduplicatePut,
};

/// @brief  Init function for a ioduplicate
static DEV_IO_FUNC_INIT(ioduplicateInit)
{
    const tsIoduplicateConsts *consts = device->constants;
    uint16_t idx;
    ADD_RCOS_VERSION("ioduplicate", "2.0.0")

    UNUSED(config);
    for (idx = 0; idx < consts->itemCount; idx++)
    {
        devIoInit(consts->list[idx], NULL);
    }
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a ioduplicate
static DEV_IO_FUNC_DEINIT(ioduplicateDeinit)
{
    const tsIoduplicateConsts *consts = device->constants;
    uint16_t idx;

    device->sys->initialized = 0;
    for (idx = 0; idx < consts->itemCount; idx++)
    {
        devIoDeinit(consts->list[idx]);
    }

    return EXIT_SUCCESS;
}

/// @brief  Get function for a ioduplicate
static DEV_IO_FUNC_GET(ioduplicateGet)
{
    const tsIoduplicateConsts *consts = device->constants;

    return devIoGet(consts->list[0]);
}

/// @brief  Put function for a ioduplicate
static DEV_IO_FUNC_PUT(ioduplicatePut)
{
    const tsIoduplicateConsts *consts = device->constants;
    uint16_t idx;

    for (idx = 0; idx < consts->itemCount; idx++)
    {
        devIoPut(consts->list[idx], data);
    }

    return EXIT_SUCCESS;
}

/** @} */

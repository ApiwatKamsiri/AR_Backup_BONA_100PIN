/** @file       apos.c
 *  @brief      Analog Position - Convert specific ranges of an input devIo value to enumeration
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       11.01.2019
 */
#define FILE_APOS_C

/**
 *  @addtogroup APOS
 *  @{
 */
#include "apos.h"

static DEV_IO_FUNC_INIT(aposInit);
static DEV_IO_FUNC_GET(aposGet);

const tsDevIoFuncs devIoAposFuncs =
    {
        aposInit,
        NULL,
        aposGet,
        NULL,
};

static DEV_IO_FUNC_INIT(aposInit)
{
    const tsAposConsts *consts = device->constants;
    ADD_RCOS_VERSION("apos", "2.0.0")

    UNUSED(config); // REMOVE IF USED

    devIoInit(consts->input, NULL);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(aposGet)
{
    const tsAposConsts *consts = device->constants;
    uint32_t value             = devIoGet(consts->input);
    int idx;

    for (idx = 0; idx < consts->tableSize; idx++)
    {
        if ((value <= consts->table[idx].maximum) &&
            (value >= consts->table[idx].minimum))
        {
            return idx + 1;
        }
    }

    return 0;
}

/** @} */

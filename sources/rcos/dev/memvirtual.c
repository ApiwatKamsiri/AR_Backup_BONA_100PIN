/** @file       memvirtual.c
 *  @brief      Memory device that uses a RAM area
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       1.08.2019
 */
#define FILE_MEMVIRTUAL_C

/**
 *  @addtogroup MEMVIRTUAL
 *  @{
 */
#include "memvirtual.h"
#include <string.h>

static DEV_MEM_FUNC_INIT(memvirtualInit);
static DEV_MEM_FUNC_DEINIT(memvirtualDeinit);
static DEV_MEM_FUNC_READ(memvirtualRead);
static DEV_MEM_FUNC_WRITE(memvirtualWrite);
static DEV_MEM_FUNC_PROGRAM(memvirtualProgram);
static DEV_MEM_FUNC_ERASE(memvirtualErase);

const tsDevMemFuncs devMemMemvirtualFuncs =
    {
        memvirtualInit,
        memvirtualDeinit,
        memvirtualRead,
        memvirtualWrite,
        memvirtualProgram,
        memvirtualErase,
        NULL,
        NULL,
};

static DEV_MEM_FUNC_INIT(memvirtualInit)
{
    const tsMemvirtualConsts *consts = device->constants;
    ADD_RCOS_VERSION("memvirtual", "2.0.0")

    device->sys->initialized = 1;
    memset(consts->area, 0xff, consts->size); // Initialize target area

    return EXIT_SUCCESS;
}

static DEV_MEM_FUNC_DEINIT(memvirtualDeinit)
{
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

static DEV_MEM_FUNC_READ(memvirtualRead)
{
    const tsMemvirtualConsts *consts = device->constants;

    if (consts->size >= (address + length))
    {
        memcpy(readData, &consts->area[address], length);
        TICKET_REMOVE(device->sys->tvm);
    }
    else
    {
        length = 0;
    }

    return length;
}

static DEV_MEM_FUNC_WRITE(memvirtualWrite)
{
    const tsMemvirtualConsts *consts = device->constants;

    if (consts->size >= (address + length))
    {
        memcpy(&consts->area[address], writeData, length);
        TICKET_REMOVE(device->sys->tvm);
    }
    else
    {
        length = 0;
    }

    return length;
}

static DEV_MEM_FUNC_PROGRAM(memvirtualProgram)
{
    const tsMemvirtualConsts *consts = device->constants;

    if (consts->size >= (address + length))
    {
        uint8_t *dst = &consts->area[address];
        const uint8_t *src = progData;

        for (int cnt = length; cnt; cnt--)
        {
            *dst &= *src;
            ++dst;
            ++src;
        }
        TICKET_REMOVE(device->sys->tvm);
    }
    else
    {
        length = 0;
    }

    return length;
}

static DEV_MEM_FUNC_ERASE(memvirtualErase)
{
    const tsMemvirtualConsts *consts = device->constants;

    if (consts->size >= (address + size))
    {
        memset(&consts->area[address], 0xff, size);
        TICKET_REMOVE(device->sys->tvm);
    }
    else
    {
        size = 0;
    }

    return size;
}

/** @} */

/** @file       mem_hamming.c
 *  @brief      C file of MEM_HAMMING device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       23.01.2020
 */
#define FILE_MEM_HAMMING_C

#include <stdlib.h> // Required for malloc and free
#include "mem_hamming.h"
#include "libs/hamming.h"

/**
 *  @addtogroup MEM_HAMMING
 *  @{
 */

static DEV_MEM_FUNC_INIT(memHammingInit);
static DEV_MEM_FUNC_DEINIT(memHammingDeinit);
static DEV_MEM_FUNC_READ(memHammingRead);
static DEV_MEM_FUNC_WRITE(memHammingWrite);
// static DEV_MEM_FUNC_PROGRAM(memHammingProgram);
static DEV_MEM_FUNC_ERASE(memHammingErase);
static DEV_MEM_FUNC_TICKET_GET(memHammingTicketGet);
static DEV_MEM_FUNC_TICKET_VALID(memHammingTicketValid);

/// @brief  Structure that defines the functions for memHamming
const tsDevMemFuncs devMemMemHammingFuncs =
    {
        memHammingInit,
        memHammingDeinit,
        memHammingRead,
        memHammingWrite,
        memHammingWrite,
        memHammingErase,
        memHammingTicketGet,
        memHammingTicketValid,
};

/// @brief  Init function for a memHamming
static DEV_MEM_FUNC_INIT(memHammingInit)
{
    const tsMemHammingConsts *consts = device->constants;
    ADD_RCOS_VERSION("memhamming", "2.0.0")

    devMemInit(consts->mem);
    device->sys->initialized = TRUE;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a memHamming
static DEV_MEM_FUNC_DEINIT(memHammingDeinit)
{
    const tsMemHammingConsts *consts = device->constants;

    device->sys->initialized = FALSE;
    devMemDeinit(consts->mem);

    return EXIT_SUCCESS;
}

/// @brief  Read function for a memHamming
static DEV_MEM_FUNC_READ(memHammingRead)
{
    const tsMemHammingConsts *consts = device->constants;
    uint16_t *encBuffer;

    if (device->sys->initialized)
    {
        return 0;
    }

    encBuffer = malloc(length * sizeof(uint16_t));
    devMemRead(consts->mem, address * 2, encBuffer, length * 2);
    hammingDecodeArray(readData, encBuffer, length);
    free(encBuffer);

    return length;
}

/// @brief  Write function for a memHamming
static DEV_MEM_FUNC_WRITE(memHammingWrite)
{
    const tsMemHammingConsts *consts = device->constants;
    uint16_t *encBuffer;

    if (device->sys->initialized)
    {
        return 0;
    }

    encBuffer = malloc(length * sizeof(uint16_t));
    hammingEncodeArray(encBuffer, writeData, length);
    devMemWrite(consts->mem, address * 2, encBuffer, length * 2);
    free(encBuffer);

    return length;
}

/// @brief  Erase function for a memHamming
static DEV_MEM_FUNC_ERASE(memHammingErase)
{
    const tsMemHammingConsts *consts = device->constants;
    tsDevMemSystem *sys        = device->sys;

    if (!sys->initialized)
    {
        return 0;
    }

    return devMemErase(consts->mem, address * 2, size * 2) / 2;
}

/// @brief  Ticket get function for a memHamming
static DEV_MEM_FUNC_TICKET_GET(memHammingTicketGet)
{
    const tsMemHammingConsts *consts = device->constants;
    return devMemTicketGet(consts->mem);
}

/// @brief  Ticket valid function for a memHamming
static DEV_MEM_FUNC_TICKET_VALID(memHammingTicketValid)
{
    const tsMemHammingConsts *consts = device->constants;
    return devMemTicketValid(consts->mem, ticket);
}

/** @} */

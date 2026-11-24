/** @file       w25q.c
 *  @brief      W25Q Winbond Serial NOR Flash
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       28.11.2018
 */
#define FILE_W25Q_C

/**
 *  @addtogroup W25Q
 *  @{
 */
#include "w25q.h"

#define W25Q_READ_STATUS_1 (0x05)
#define W25Q_READ_STATUS_2 (0x35)
#define W25Q_READ_STATUS_3 (0x15)
#define W25Q_WRITE_STATUS_1 (0x01)
#define W25Q_WRITE_STATUS_2 (0x31)
#define W25Q_WRITE_STATUS_3 (0x11)

#define W25Q_STATUS_1_BUSY (0x01)

#define W25Q_READ_DEVICE_ID (0x90)
#define W25Q_DEVICE_ID (0x14ef)
#define W25Q_READ_JEDEC_ID (0x9f)
#define W25Q_JEDEC_ID (0x1540efff)

#define W25Q_READ_DATA (0x03)
#define W25Q_FAST_READ_DATA (0x0B)

#define W25Q_WRITE_ENABLE (0x06)
#define W25Q_PAGE_PROGRAM (0x02)

#define W25Q_SECTOR_ERASE (0x20)
#define W25Q_SECTOR_ERASE_SIZE (4 * 1024)
#define W25Q_32KB_BLOCK_ERASE (0x52)
#define W25Q_32KB_BLOCK_ERASE_SIZE (32 * 1024)
#define W25Q_64KB_BLOCK_ERASE (0xD8)
#define W25Q_64KB_BLOCK_ERASE_SIZE (64 * 1024)

#define W25Q_CHIP_ERASE (0xC7)

#define W25Q_PT_ENTRY()                                                \
    const tsDevMem *device     = (const tsDevMem *)process->constants; \
    tsW25qParams *params       = device->parameters;                   \
    const tsW25qConsts *consts = device->constants;

static PT_THREAD(w25qPtCheckStatus)
{
    W25Q_PT_ENTRY();

    PT_BEGIN();

    do
    {
        params->ticket = devComTicketGet(consts->spi);
        PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->spi, params->ticket), 10);

        params->command = W25Q_READ_STATUS_1;

        devComOpen(consts->spi, consts->target);
        devComSend(consts->spi, &params->command, 2); // 1 byte for command + 1 byte for reply
        PT_WAIT_WHILE(devComSend(consts->spi, NULL, 0));
        devComReceive(consts->spi, &params->command, 2); // Receive 2 bytes
        devComClose(consts->spi, consts->target);
        PT_WAIT_WHILE(devComTicketValid(consts->spi, params->ticket));

    } while (params->command & ((uint32_t)W25Q_STATUS_1_BUSY << 8));

    PT_END();
}

static PT_THREAD(w25qPtGetId)
{
    W25Q_PT_ENTRY();

    PT_BEGIN();

    PT_SPAWN(&params->child, w25qPtCheckStatus); // Wait for busy signal

    do
    {
        params->ticket = devComTicketGet(consts->spi);
        PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->spi, params->ticket), 10);

        params->command = W25Q_READ_JEDEC_ID;

        devComOpen(consts->spi, consts->target);
        devComSend(consts->spi, &params->command, sizeof(params->command)); // 1 byte for command + 3 bytes for reply
        PT_WAIT_WHILE(devComSend(consts->spi, NULL, 0));
        devComReceive(consts->spi, &params->command, sizeof(params->command)); // Receive 4 bytes
        devComClose(consts->spi, consts->target);
        PT_WAIT_WHILE(devComTicketValid(consts->spi, params->ticket));

    } while (W25Q_JEDEC_ID != params->command);

    // if (W25Q_JEDEC_ID == params->command)
    {
        /// Device is recognised as Winbond W25Q
        device->sys->initialized = TRUE;
    }
    // else
    {
        // TODO: id does not match
    }

    PT_END();
}

static PT_THREAD(w25qPtRead)
{
    W25Q_PT_ENTRY();

    PT_BEGIN();

    PT_SPAWN(&params->child, w25qPtCheckStatus); // Wait for busy signal

    params->ticket = devComTicketGet(consts->spi);
    PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->spi, params->ticket), 10);

    params->command = ((uint32_t)W25Q_READ_DATA << 24) | (params->address & (BIT(24) - 1));
    params->command = htonl(params->command);

    devComOpen(consts->spi, consts->target);

    // 1 byte for command + 3 byte for address
    devComSend(consts->spi, &params->command, sizeof(params->command));
    PT_WAIT_WHILE(devComSend(consts->spi, NULL, 0));
    devComReceive(consts->spi, NULL, sizeof(params->command)); // Get rid of reply

    // Send to receive
    devComSend(consts->spi, NULL, params->length);
    PT_WAIT_WHILE(devComSend(consts->spi, NULL, 0));
    devComReceive(consts->spi, params->buffer, params->length);

    devComClose(consts->spi, consts->target);

    PT_WAIT_WHILE(devComTicketValid(consts->spi, params->ticket));

    // Operation finished
    TICKET_REMOVE(device->sys->tvm);

    PT_END();
}

static PT_THREAD(w25qPtWriteEnable)
{
    W25Q_PT_ENTRY();

    PT_BEGIN();

    params->ticket = devComTicketGet(consts->spi);
    PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->spi, params->ticket), 10);

    params->command = W25Q_WRITE_ENABLE;

    devComOpen(consts->spi, consts->target);
    devComSend(consts->spi, &params->command, 1); // 1 byte for command
    devComClose(consts->spi, consts->target);

    PT_END();
}

static PT_THREAD(w25qPtProgram)
{
    W25Q_PT_ENTRY();

    PT_BEGIN();

    do
    {
        PT_SPAWN(&params->child, w25qPtCheckStatus); // Wait for busy signal

        PT_SPAWN(&params->child, w25qPtWriteEnable); // Enable write operations

        params->ticket = devComTicketGet(consts->spi);
        PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->spi, params->ticket), 10);

        params->command = ((uint32_t)W25Q_PAGE_PROGRAM << 24) | (params->address & (BIT(24) - 1));
        params->command = htonl(params->command);

        params->subLen = 256 - (params->address & 0xff); /// Program until end of current page
        params->subLen = MIN(params->length, params->subLen);

        devComOpen(consts->spi, consts->target);
        devComSend(consts->spi, &params->command, sizeof(params->command)); // Send command
        devComSend(consts->spi, params->buffer, params->subLen);            // Send a single operation
        devComClose(consts->spi, consts->target);

        params->address += params->subLen;                           /// Update address
        params->buffer = (uint8_t *)params->buffer + params->subLen; /// Update pointer
        params->length -= params->subLen;                            /// This amount of data is written

    } while (params->length);

    /// Operation finished
    TICKET_REMOVE(device->sys->tvm);

    PT_END();
}

static PT_THREAD(w25qPtChipErase)
{
    W25Q_PT_ENTRY();

    PT_BEGIN();

    PT_SPAWN(&params->child, w25qPtCheckStatus); // Wait for busy signal

    PT_SPAWN(&params->child, w25qPtWriteEnable); // Enable write operations

    params->ticket = devComTicketGet(consts->spi);
    PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->spi, params->ticket), 10);

    params->command = W25Q_CHIP_ERASE;

    devComOpen(consts->spi, consts->target);
    devComSend(consts->spi, &params->command, 1); // 1 byte for command
    devComClose(consts->spi, consts->target);

    /// Operation finished
    TICKET_REMOVE(device->sys->tvm);

    PT_END();
}

static PT_THREAD(w25qPtErase)
{
    W25Q_PT_ENTRY();
    uint32_t length;

    PT_BEGIN();

    do
    {
        PT_SPAWN(&params->child, w25qPtCheckStatus); // Wait for busy signal

        PT_SPAWN(&params->child, w25qPtWriteEnable); // Enable write operations

        params->ticket = devComTicketGet(consts->spi);
        PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->spi, params->ticket), 10);

        length = params->length;

        if (length >= W25Q_64KB_BLOCK_ERASE_SIZE)
        {
            params->command = W25Q_64KB_BLOCK_ERASE;
            length          = W25Q_64KB_BLOCK_ERASE_SIZE;
        }
        else if (params->length >= W25Q_32KB_BLOCK_ERASE_SIZE)
        {
            params->command = W25Q_32KB_BLOCK_ERASE;
            length          = W25Q_32KB_BLOCK_ERASE_SIZE;
        }
        else // if (params->length >= W25Q_SECTOR_ERASE_SIZE)
        {
            params->command = W25Q_SECTOR_ERASE;
            length          = W25Q_SECTOR_ERASE_SIZE;
        }

        params->command = (params->command << 24) | (params->address & (BIT(24) - 1));
        params->command = htonl(params->command);

        devComOpen(consts->spi, consts->target);
        devComSend(consts->spi, &params->command, sizeof(params->command)); // 1 byte for command + 3 bytes for address
        devComClose(consts->spi, consts->target);

        params->address += length;
        params->length -= length;
    } while (params->length);

    /// Operation finished
    TICKET_REMOVE(device->sys->tvm);

    PT_END();
}

PROCESS_INIT_PROTO(w25qProInit)
{
    threadStart(process, w25qPtGetId);
    process->initialized = TRUE;
}

PROCESS_DEINIT_PROTO(w25qProDeinit)
{
    process->initialized = FALSE;
    threadStop(process, process->threadFunction); // Whatever operation is done currently
}

static DEV_MEM_FUNC_INIT(w25qInit);
static DEV_MEM_FUNC_DEINIT(w25qDeinit);
static DEV_MEM_FUNC_READ(w25qRead);
//static DEV_MEM_FUNC_WRITE(w25qWrite);
static DEV_MEM_FUNC_PROGRAM(w25qProgram);
static DEV_MEM_FUNC_ERASE(w25qErase);

const tsDevMemFuncs devMemW25qFuncs =
    {
        w25qInit,
        w25qDeinit,
        w25qRead,
        NULL, //w25qWrite,
        w25qProgram,
        w25qErase,
        NULL,
        NULL,
};

static DEV_MEM_FUNC_INIT(w25qInit)
{
    const tsW25qConsts *consts = device->constants;
    ADD_RCOS_VERSION("w25q", "2.0.0")

    devComInit(consts->spi);
    processStart(consts->process);

    return EXIT_SUCCESS;
}

static DEV_MEM_FUNC_DEINIT(w25qDeinit)
{
    const tsW25qConsts *consts = device->constants;

    device->sys->initialized = FALSE;
    processStop(consts->process);

    return EXIT_SUCCESS;
}

static DEV_MEM_FUNC_READ(w25qRead)
{
    tsW25qParams *params       = device->parameters;
    const tsW25qConsts *consts = device->constants;

    if (!consts->process->threadFunction)
    {
        params->address = address;
        params->buffer  = readData;
        params->length  = length;
        threadStart(consts->process, w25qPtRead);

        return length;
    }
    else
    {
        return 0;
    }
}

#if 0
static DEV_MEM_FUNC_WRITE(w25qWrite)
{
    tsW25qParams *params       = device->parameters;
    const tsW25qConsts *consts = device->constants;

    UNUSED(params); // REMOVE IF USED

    return 0;
}
#endif

static DEV_MEM_FUNC_PROGRAM(w25qProgram)
{
    tsW25qParams *params       = device->parameters;
    const tsW25qConsts *consts = device->constants;

    if (!consts->process->threadFunction)
    {
        params->address = address;
        params->buffer  = (void *)progData;
        params->length  = length;
        threadStart(consts->process, w25qPtProgram);

        return length;
    }
    else
    {
        return 0;
    }
}

static DEV_MEM_FUNC_ERASE(w25qErase)
{
    tsW25qParams *params       = device->parameters;
    const tsW25qConsts *consts = device->constants;

    if (!consts->process->threadFunction)
    {
        if (size)
        {
            uint32_t temp;

            temp = address / W25Q_SECTOR_ERASE_SIZE;
            temp *= W25Q_SECTOR_ERASE_SIZE;
            params->address = temp; /// Fix address to point start of sector

            params->buffer = NULL;

            size += address - temp;
            temp = size % W25Q_SECTOR_ERASE_SIZE;
            if (temp)
            {
                size += W25Q_SECTOR_ERASE_SIZE - temp;
            }
            params->length = size; /// Fix length to cover erase area that starts and ends with a sector limit

            threadStart(consts->process, w25qPtErase);
        }
        else
        {
            // Chip erase
            threadStart(consts->process, w25qPtChipErase);
        }

        return size;
    }
    else
    {
        return 0;
    }
}

/** @} */

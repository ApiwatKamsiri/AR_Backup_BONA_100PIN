/** @file       backup.c
 *  @brief      Backup or restore a list of variables using an NVM
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     
 *  @date       7.05.2019
 */
#define FILE_BACKUP_C

/**
 *  @addtogroup BACKUP
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "backup.h"
#include <string.h>

tsProcess *rcosBackup = NULL;

/*********************************************************************
 *  LOCAL CONSTANTS
 */
#define COPY_CNT (1)     // Number of copies stored in memory
#define BURST_SIZE (128) // Maximum number of bytes that will be read or written
#define ITEM_SIZE_MAX (sizeof(uint32_t))

/// @brief  Events of a Backup process
typedef enum
{
    eBackupPeriodic,
} teBackupEvents;

/*
static void *memow(void *destination, const void *source, size_t length)
{
    uint8_t *dst = destination;
    const uint8_t *src = source;

    for (; length; length--)
    {
        *dst |= *src;
        dst++;
        src++;
    }

    return destination;
}
*/

static uint32_t itemAddress(uint8_t itemIdx)
{
    tsBackupParams *params = rcosBackup->parameters;
    uint32_t offset        = params->start + sizeof(params->header);

    for (int idx = 0; idx < itemIdx; idx++)
    {
        offset += params->itemArr[idx].length;
    }

    return offset;
}

static void devMemReadBlocking(uint32_t address, void *readData, uint16_t length)
{
    tsBackupParams *params = rcosBackup->parameters;

    params->nvmTicket = devMemTicketGet(params->nvm);
    while (FALSE == devMemTicketValid(params->nvm, params->nvmTicket))
        ;
    devMemRead(params->nvm, address, readData, length);
    while (TRUE == devMemTicketValid(params->nvm, params->nvmTicket))
        ;
}

static void devMemWriteBlocking(uint32_t address, void *writeData, uint16_t length)
{
    tsBackupParams *params = rcosBackup->parameters;

    params->nvmTicket = devMemTicketGet(params->nvm);
    while (FALSE == devMemTicketValid(params->nvm, params->nvmTicket))
        ;
    devMemWrite(params->nvm, address, writeData, length);
    while (TRUE == devMemTicketValid(params->nvm, params->nvmTicket))
        ;
}

static void itemRead(uint8_t itemIdx)
{
    tsBackupParams *params = rcosBackup->parameters;
    uint16_t encBuff[ITEM_SIZE_MAX];
    uint8_t decBuff[ITEM_SIZE_MAX];
    // uint8_t tempBuff[ITEM_SIZE_MAX] = {0};

    // memset(params->itemArr[itemIdx].address, 0, params->itemArr[itemIdx].length); // Clear data
    for (int idx = 0; idx < COPY_CNT; idx++)
    {
        devMemReadBlocking((itemAddress(itemIdx) + idx * params->header.totalSize) * 2, encBuff, params->itemArr[itemIdx].length * 2);
        if (EXIT_SUCCESS == hammingDecodeArray(decBuff, encBuff, params->itemArr[itemIdx].length))
        {
            memcpy(params->itemArr[itemIdx].address, decBuff, params->itemArr[itemIdx].length);
            return;
        }
        else
        {
            // Recover as much as we can?
            // memow(tempBuff, decBuff, params->itemArr[itemIdx].length);
            // memow(params->itemArr[itemIdx].address, decBuff, params->itemArr[itemIdx].length);
        }
    }
    // memcpy(params->itemArr[itemIdx].address, tempBuff, sizeof(tempBuff));
}

/*
static void itemWrite(uint8_t itemIdx)
{
    tsBackupParams *params = rcosBackup->parameters;
    uint16_t encBuff[ITEM_SIZE_MAX];

    hammingEncodeArray(encBuff, params->itemArr[itemIdx].address, params->itemArr[itemIdx].length);
    for (int idx = 0; idx < COPY_CNT; idx++)
    {
        devMemWriteBlocking((itemAddress(itemIdx) + idx * params->header.totalSize) * 2, encBuff, params->itemArr[itemIdx].length * 2);
    }
}
*/

static void backupReadAll(void) // IMPORTANT
{
    tsBackupParams *params = rcosBackup->parameters;
#if 0
    int idx         = 0;
    uint16_t offset = itemAddress(0);
    uint16_t encBuff[BURST_SIZE];

    while (idx < params->itemArrSize)
    {
        uint16_t total = 0;
        for (int jdx = 0; jdx < COPY_CNT; jdx++)
        {
            devMemReadBlocking((offset + jdx * params->header.totalSize) * 2, encBuff, sizeof(encBuff));
            for (; (idx < params->itemArrSize) && (total <= (ARRAY_SIZE(encBuff) - ITEM_SIZE_MAX)); idx++)
            {
                total += params->itemArr[idx].length;
                hammingEncodeArray(&encBuff[total], params->itemArr[idx].address, params->itemArr[idx].length);
            }
        }
        offset += total;
    }

#else
    for (int idx = 0; idx < params->itemArrSize; idx++)
    {
        itemRead(idx);
    }
#endif
}

static uint16_t calculateItemCrc(uint8_t itemIdx)
{
    tsBackupParams *params = rcosBackup->parameters;
    uint8_t *pointer       = (uint8_t *)params->itemArr[itemIdx].address;
    uint16_t crc;

    crc = crc16CcittInit();
    for (int idx = 0; idx < params->itemArr[itemIdx].length; idx++)
    {
        crc = crc16CcittAddData(crc, *pointer++);
    }
    crc = crc16CcittFinish(crc);

    return crc;
}

static PROCESS_HANDLER_PROTO(backupPeriodicBackup)
{
    tsBackupParams *params = process->parameters;
    uint8_t clear          = TRUE;

    if (eventCurrent.source == eventCurrent.destination)
    {
        switch (eventCurrent.event)
        {
            case eBackupPeriodic:
                for (int idx = 0; idx < params->itemArrSize; idx++)
                {
                    uint16_t check = calculateItemCrc(idx);
                    if (params->crcBuffer[idx] != check)
                    {
                        clear                  = FALSE;
                        params->crcBuffer[idx] = check;
                    }
                }
                if (!clear)
                {
                    backupWriteAll();
                }
                if (params->period)
                {
                    timerEventStart(&params->tev, params->period); // Periodic check
                }
                break;
            default:
                break;
        }
    }
}

/** INTERFACE FUNCTIONS *******************************************************/

void backupWriteAll(void) // IMPORTANT
{
    tsBackupParams *params = rcosBackup->parameters;
#if 1
    int idx         = 0;
    uint16_t offset = itemAddress(0);
    uint16_t encBuff[BURST_SIZE];

    while (idx < params->itemArrSize)
    {
        uint16_t total = 0;
        for (; (idx < params->itemArrSize) && (total <= (ARRAY_SIZE(encBuff) - ITEM_SIZE_MAX)); idx++)
        {
            hammingEncodeArray(&encBuff[total], params->itemArr[idx].address, params->itemArr[idx].length);
            total += params->itemArr[idx].length;
        }
        for (int jdx = 0; jdx < COPY_CNT; jdx++)
        {
            devMemWriteBlocking((offset + jdx * params->header.totalSize) * 2, encBuff, total * 2);
        }
        offset += total;
    }

#else
    for (int idx = 0; idx < params->itemArrSize; idx++)
    {
        itemWrite(idx);
    }
#endif
}

void backupUpdateTrigger(void)
{
    tsBackupParams *params = rcosBackup->parameters;

    // eventPost(rcosBackup->enumeration, eBackupPeriodic, NULL, 0); // Backup is closed to outside interaction
    timerEventStart(&params->tev, 1); // Periodic check is triggered after 1ms
}

PROCESS_INIT_PROTO(backupInit)
{
    tsBackupParams *params = process->parameters;
    tsBackupHeader dummyHeader;
    uint16_t encBuff[sizeof(dummyHeader)];
    ADD_RCOS_VERSION("backup", "2.0.1")

    devMemInit(params->nvm); // Did it finish?

    rcosBackup = process;

    // Calculate array identifier
    params->header.signature = BACKUP_SIGNATURE;
    params->header.itemArrId = crc16CcittInit();
    params->header.totalSize = 0;
    for (int idx = 0; idx < params->itemArrSize; idx++)
    {
        params->header.itemArrId = crc16CcittAddData(params->header.itemArrId, params->itemArr[idx].length); // Use lengths of each item for crc calculation
        params->header.totalSize += params->itemArr[idx].length;                                             // Calculate total size of items
        params->crcBuffer[idx] = calculateItemCrc(idx);                                                      // Remember CRC value of each item
    }
    params->header.itemArrId = crc16CcittFinish(params->header.itemArrId);

    // Get header from NVM
    devMemReadBlocking(params->start, encBuff, sizeof(encBuff));
    hammingDecodeArray((uint8_t *)&dummyHeader, encBuff, ARRAY_SIZE(encBuff));

    if ((dummyHeader.signature == params->header.signature) &&
        (dummyHeader.itemArrId == params->header.itemArrId) &&
        (dummyHeader.totalSize == params->header.totalSize))
    {
        backupReadAll();
    }
    else
    {
        // Backup is changed or something...
        backupWriteAll();
        // Write our header
        hammingEncodeArray(encBuff, (uint8_t *)&params->header, ARRAY_SIZE(encBuff));
        devMemWriteBlocking(params->start, encBuff, sizeof(encBuff));
    }

    PROCESS_STATE_CHANGE(process, backupPeriodicBackup);
    params->tev.post.destination = process->enumeration;
    params->tev.post.source      = process->enumeration;
    params->tev.post.event       = eBackupPeriodic;
    if (params->period)
    {
        timerEventStart(&params->tev, params->period); // Periodic check
    }
}

PROCESS_DEINIT_PROTO(backupDeinit)
{
    tsBackupParams *params = process->parameters;

    timerEventStop(&params->tev);
    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0;
}

/** @} */

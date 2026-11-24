/** @file       synchronizer.c
 *  @brief      C file of SYNCHRONIZER device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-10
 */
#define FILE_SYNCHRONIZER_C

#include "synchronizer.h"

/**
 *  @addtogroup SYNCHRONIZER
 *  @{
 */

static DEV_CPX_FUNC_INIT(synchronizerInit);
static DEV_CPX_FUNC_DEINIT(synchronizerDeinit);

static PT_THREAD(synchronizerThread);

static uint32_t GetSrcData(tsSyncItem *item);
static void PutDstData(tsSyncItem *item, uint32_t data);

/// @brief  Structure that defines the functions for synchronizer
const tsDevCpxFuncs devCpxSynchronizerFuncs =
    {
        synchronizerInit,
        synchronizerDeinit,
};

/// @brief  Init function for a synchronizer
static DEV_CPX_FUNC_INIT(synchronizerInit)
{
    tsSynchronizerParams *params = device->parameters;
    const tsSynchronizerConsts *consts = device->constants;
    UNUSED(config); // REMOVE IF USED

    if (device->sys->initialized == 0)
    {
        device->sys->initialized = 1;
        params->attachedItemCount = 0;
        processStart(consts->process);
    }

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a synchronizer
static DEV_CPX_FUNC_DEINIT(synchronizerDeinit)
{
    tsSynchronizerParams *params = device->parameters;
    const tsSynchronizerConsts *consts = device->constants;

    if (device->sys->initialized)
    {
        device->sys->initialized = 0;
        params->attachedItemCount = 0;
        processStop(consts->process);
    }

    return EXIT_SUCCESS;
}

/* Process Functions *******************************************/

PROCESS_INIT_PROTO(synchronizerProcessInit)
{
    process->initialized = 1;
    threadStart(process, synchronizerThread);
}

PROCESS_DEINIT_PROTO(synchronizerProcessDeinit)
{
    process->initialized = 0;
    threadStop(process, synchronizerThread);
}

static PT_THREAD(synchronizerThread)
{
    tsSynchronizerParams *params = process->parameters;
    const tsSynchronizerConsts *consts = process->constants;
    PT_BEGIN();

    for (;;)
    {
        for (size_t i = 0; i < params->attachedItemCount; ++i)
        {
            tsSyncItem *item = &consts->items[i];
            uint32_t srcData = GetSrcData(item);
            if (srcData != item->lastSrcData || !item->firstReadDone)
            {
                PutDstData(item, srcData);
                item->firstReadDone = true;
                item->lastSrcData = srcData;
            }
        }

        PT_YIELD_TIMEOUT(consts->cyclePeriodMs);
    }

    PT_END();
}

/// @brief returns source value of a syncItem
static uint32_t GetSrcData(tsSyncItem *item)
{
    uint32_t data = 0;
    if (item->syncType == eSyncTypeDeviceToRam)
        data = devIoGet(item->devIo);
    else if (item->ramDataSize == sizeof(uint8_t))
        data = *((uint8_t *)item->ramDataPtr);
    else if (item->ramDataSize == sizeof(uint16_t))
        data = *((uint16_t *)item->ramDataPtr);
    else if (item->ramDataSize == sizeof(uint32_t))
        data = *((uint32_t *)item->ramDataPtr);
    return data;
}

/// @brief updates destination with the given data value
static void PutDstData(tsSyncItem *item, uint32_t data)
{
    if (item->syncType == eSyncTypeRamToDevice)
        devIoPut(item->devIo, data);
    else if (item->ramDataSize == sizeof(uint8_t))
        *((uint8_t *)item->ramDataPtr) = data;
    else if (item->ramDataSize == sizeof(uint16_t))
        *((uint16_t *)item->ramDataPtr) = data;
    else if (item->ramDataSize == sizeof(uint32_t))
        *((uint32_t *)item->ramDataPtr) = data;
}

uint8_t devSyncAttachWSize(const tsDevCpx *device, teSyncType syncType, const tsDevIo *devIo, void *dataPtr, size_t dataSize)
{
    tsSynchronizerParams *params = device->parameters;
    const tsSynchronizerConsts *consts = device->constants;

    // Check that device and ram data pointer are not NULL and there is free room in synchronizer
    if (devIo && dataPtr && params->attachedItemCount < consts->maxItemCount)
    {
        tsSyncItem *item = &consts->items[params->attachedItemCount];
        item->syncType = syncType;
        item->devIo = devIo;
        item->ramDataPtr = dataPtr;
        item->ramDataSize = dataSize;
        item->firstReadDone = false;
        item->lastSrcData = 0; // no need, but for debug purposes
        params->attachedItemCount++;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/** @} */

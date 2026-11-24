/** @file       synchronizer.h
 *  @brief      Header file of SYNCHRONIZER device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-10
 *
 *  @details
 *
 * This device is used to synchronize devIos data with ram data every specific period.
 * The synchronization is unidirectional and can be from device to ram or from ram to device.
 * The synchronization job is handled using protothreads so it doesn't interrupt other
 * processes and doesn't update the target with unready source values.
 * 
 * @code
 * // Creation ...
 * #include "dev/ke14/gpio.h"
 * DEV_IO_GPIO_CREATE(light, UC_FF_LIGHT, 0, 0, 1) 
 * DEV_IO_GPIO_CREATE(switch, UC_DC_FF_DOOR, 1, 0, 1) 
 * 
 * #include "dev/synchronizer.h"
 * DEV_CPX_SYNCHRONIZER_CREATE(sync, 75, 20)
 * 
 * 
 * // initialization ...
 * devIoInit(&light, NULL);
 * devIoInit(&switch, NULL);
 * devCpxInit(&sync, NULL);
 * 
 * // Usage ...
 * uint32_t lightValue, switchValue;
 * devSyncAttach(&sync, eSyncTypeRamToDevice, &light, &lightValue);
 * devSyncAttach(&sync, eSyncTypeDeviceToRam, &switch, &switchValue);
 * 
 * // in another place
 * if (switchValue == 1) // switchValue is synchronized with switch devIo so its value represents switch devIo value
 *  lightValue = 0; // this will be synchronized with light devIo so it will be turned off
 * else
 *  lightValue = 1;
 * 
 * 
 */
#ifndef FILE_SYNCHRONIZER_H
#define FILE_SYNCHRONIZER_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_SYNCHRONIZER_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   SYNCHRONIZER SYNCHRONIZER
 *  @ingroup    DEV_CPX
 *  @brief      synchronizes ram data and device data
 *  @{
 */

/// @brief  Functions for SYNCHRONIZER devices
INTERFACE const tsDevCpxFuncs devCpxSynchronizerFuncs;

/// @brief  Device specific parameters
typedef struct
{
    size_t attachedItemCount; ///< Number of attached item that will be synchronized
} tsSynchronizerParams;

typedef enum
{
    eSyncTypeDeviceToRam, ///< used with input devices, we get data from device to ram
    eSyncTypeRamToDevice, ///< used with output devices, we put data from ram to device
} teSyncType;

typedef struct
{
    teSyncType syncType;  ///< Synchronization direction for this item
    const tsDevIo *devIo; ///< Ptr to devIo
    void *ramDataPtr;     ///< Ptr to ram data
    size_t ramDataSize;   ///< Size of ram data in bytes
    uint32_t lastSrcData; ///< Last read source data
    bool firstReadDone;   ///< Flag indicates if the synchronizer has read data before
                          ///< from the source
} tsSyncItem;

/// @brief  Device specific constants
typedef struct
{
    uint32_t cyclePeriodMs; ///< Period of the synchronization process in milliseconds
    size_t maxItemCount;    ///< Maximum number of items that can be attached to the sync
    tsSyncItem *items;      ///< Array of attached items
    tsProcess *process;     ///< Process that handles the jobs for this device
} tsSynchronizerConsts;

/** @brief  Create a devCpx for synchronizer
 *  @param  _name   Name of devCpx object
 *  @param _maxItemCount Maximum number of items that can be attached to the sync
 *  @param _cyclePeriodMs Period of the synchronization process in milliseconds
 */
#define DEV_CPX_SYNCHRONIZER_CREATE(_name, _maxItemCount, _cyclePeriodMs)                                                            \
    tsProcess _name##Process;                                                                                                        \
    tsSyncItem _name##Items[_maxItemCount];                                                                                          \
    tsSynchronizerParams _name##Params =                                                                                             \
        {                                                                                                                            \
            0,                                                                                                                       \
    };                                                                                                                               \
    const tsSynchronizerConsts _name##Consts =                                                                                       \
        {                                                                                                                            \
            .cyclePeriodMs = _cyclePeriodMs,                                                                                         \
            .maxItemCount = _maxItemCount,                                                                                           \
            .items = _name##Items,                                                                                                   \
            .process = &_name##Process,                                                                                              \
    };                                                                                                                               \
    PROCESS_CREATE(_name##Process, synchronizerProcessInit, synchronizerProcessDeinit, PROCESS_NONE, &_name##Params, &_name##Consts) \
    DEV_CPX_CREATE(_name, devCpxSynchronizerFuncs, &_name##Params, &_name##Consts)

/**
 * @brief Adds an item with the given parameters to the synchronizer list
 * 
 * @param device Ptr to synchronizer device
 * @param syncType synchronization direction can be from teSyncType enum
 * @param devIo Ptr to devIo that will be used in synchronization
 * @param dataPtr Ptr to ram data that will be used in synchronization
 * @param dataSize Size of ram data in bytes
 * @return EXIT_SUCCESS if a new item is added successfully, else EXIT_FAILURE
 */
INTERFACE uint8_t devSyncAttachWSize(const tsDevCpx *device, teSyncType syncType, const tsDevIo *devIo, void *dataPtr, size_t dataSize);

/** @brief Macro used as a shortcut to find out the ram data size and then use it with
    devSyncAttachWSize function
 */
#define devSyncAttach(_device, _syncType, _devIo, _dataPtr) \
    devSyncAttachWSize(_device, _syncType, _devIo, _dataPtr, sizeof(*(_dataPtr)))

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_SYNCHRONIZER_H

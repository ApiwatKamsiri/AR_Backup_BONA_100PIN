/** @file       h_backup.h
 *  @brief      Backup data in external eeprom after every 2 hours 
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     saqib sohail
 *  @date       01.08.2024
 */
#ifndef FILE_H_BACKUP_H
#define FILE_H_BACKUP_H

/**
 *  @defgroup   BACKUP BACKUP
 *  @ingroup    MW
 *  @brief      Backup or restore a list of variables using an NVM
 *  @details    It updates values in EEPROM every 2 hours
 *              
 *  @warning    THERE CAN BE ONLY ONE.
 *  @warning    BLOCKING OPERATION FOR EACH FIELD.
 @endcode
 *  @{
 */

/** INCLUDES ******************************************************************/
#include "rcos.h"

#include "EE_data.h"
#include "eeprom_fileds.h"
#include "math.h"
/*
typedef struct PLATFORM_PACKED
{
#define BACKUP_SIGNATURE (0x2c05)
    uint16_t signature; ///< Check if there's already a backup here
    uint16_t itemArrId; ///< Check if the itemArr is changed
    uint16_t totalSize; ///< Total size of array items
} tsBackupHeader;
*/
/// @brief  Backup item definition structure
typedef struct
{
    void *address;  ///< Address of item
    uint8_t length; ///< Length of item
} tsBackupItem;

/// @brief  Backup item initializer
#define BACKUP_ITEM(_variable)        \
    {                                 \
        &_variable, sizeof(_variable) \
    }

/// @brief  Parameters of a Backup process
typedef struct
{
    tsTimerEvent periodic100Ms;            ///< Event timer to be used for provide delay between each element update
    tsTimerEvent periodic2Hrs ;            ///< Event timer to be used to initiate backup process after every two hours
    /*** APW : Update I2C driver for add CT state ***/
    tsTimerEvent periodic1Min;             ///< Event timer to be used to initiate backup process after every one min 
    tsTimerEvent periodic20Ms;             ///< Event timer to be used to initiate backup process after every 20 ms(For force backup) 
    
    //   tsBackupHeader header;       ///< Header structure stored in NVM
    const tsDevMem *nvm;         ///< Target NVM device
 //   uint32_t start;              ///< Starting address of backup area on nvm
    const tsBackupItemEE *itemArr; ///< Data item's array
 //   uint16_t *crcBuffer;         ///< CRC buffer to check changes
    uint16_t period;             ///< Backup update period
    tTicket nvmTicket;           ///< NVM device ticket
    uint8_t itemArrSize;         ///< Data item's count
    stEEPROM_Ram *eepr;
    /*** APW : Update I2C driver CT code to global ***/
    stRefrigerator_Ram *rf;      ///< Data rf
} tsHBackupParams;

/** EXPORTED MACROS ***********************************************************/

/** @brief  Backup process object creation macro
 *  @param  _name           Name of process object
 *  @param  _mem            NVM device to be used
 *  @param  _itemArr        tsBackupItem array for selecting memory areas for backup
 *  @param  _updatePeriod   Period time in miliseconds for backup update operation
 *  @param  _eepromRam      
 *  @param  _rfRam          
 */
#define PROCESS_BACKUP_CREATE(_name, _mem, _itemArr, _updatePeriod, _eepromRam, _rfRam)                                                   \
    uint16_t _name##Crc[ARRAY_SIZE(_itemArr)];                                                                        \
    tsHBackupParams _name##Params =                                                                                   \
        {                                                                                                             \
            TIMER_EVENT_INIT(PROCESS_NONE, PROCESS_NONE, EVENT_NONE),                                                 \
            TIMER_EVENT_INIT(PROCESS_NONE, PROCESS_NONE, EVENT_NONE),                                                 \
            TIMER_EVENT_INIT(PROCESS_NONE, PROCESS_NONE, EVENT_NONE),/*** APW : Update I2C driver for add CT state ***/\
            TIMER_EVENT_INIT(PROCESS_NONE, PROCESS_NONE, EVENT_NONE),/*** APW : Update I2C driver for add CT state ***/\
              &_mem,                /* nvm */                                                                         \
            _itemArr,             /* itemArr */                                                                       \
            _updatePeriod,        /* period */                                                                        \
            0,                    /* nvmTicket */                                                                     \
            ARRAY_SIZE(_itemArr), /* itemArrSize */                                                                   \
            .eepr = _eepromRam,   /* eepr */                            					      \
            .rf = _rfRam,         /* rf */                            					              \
    };                                                                                                                \
    PROCESS_CREATE(_name, backupInit, backupDeinit, PROCESS_NONE, &_name##Params, NULL)

/** INTERFACES ****************************************************************/

#ifndef FILE_BACKUP_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/** INTERFACES: FUNCTIONS *****************************************************/

/// @brief      Emergency write function
/// @details    Write everything marked as backup data into NVM
/// @warning    BLOCKING FUNCTION
INTERFACE void backupWriteAll(void);

/// @brief      Trigger an update operation inside backup process
/// @details    Checks each item and updates if item has changed
INTERFACE void backupUpdateTrigger(void);

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_BACKUP_H

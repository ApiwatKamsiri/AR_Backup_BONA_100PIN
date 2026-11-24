/*
 *  @author     Cihan YILDIRIM
 *  @date       14.06.2023
*/

#ifndef RF_BACKUP_H
#define RF_BACKUP_H

#include "rcos.h"
//#include "mw/backup.h"
#include "libs/crc.h"   
#include "app/mw/h_backup.h" // MJI


typedef enum
{
    eBackupProcesEventsNone,
    eBackupProcessEventPeriodic,
    eBackupProcessEventWriteAll,
} teTestProcessEvents;

typedef struct
{
    tsTimerEvent tev;            ///< Event timer to be used for periodic maintenance
    const tsBackupItem *backupArr; ///< Data item's array
    uint16_t updatePeriod;             ///< Backup update period
    uint16_t backupArrSize;         ///< Data item's count
    uint8_t autoSave;
} tsE2Params;

typedef struct
{
    const tsDevMem *devEeprom;
} tsE2Consts;

//PROCESS_RF_BACKUP_CREATE(rfBackupProc, &devEeprom, backupArray)
#define PROCESS_RF_BACKUP_CREATE(_name, _enum, _devEeprom, _backupArray, _updatePeriod, _autoSave) \
    tsE2Params _name##Params =                                                   \
        {                                                                        \
            .tev = TIMER_EVENT_INIT(_enum, _enum, eBackupProcessEventPeriodic),  \
            .backupArr = _backupArray,             /* itemArr */                 \
            .updatePeriod = _updatePeriod,        /* period */                   \
            .backupArrSize = ARRAY_SIZE(_backupArray), /* itemArrSize */         \
            .autoSave = _autoSave,                                               \
    };                                                                           \
    const tsE2Consts _name##Consts =                                             \
    {                                                                            \
        .devEeprom = _devEeprom,                                                 \
    };                                                                           \
    PROCESS_CREATE(_name, backupE2Init, backupE2Deinit, _enum, &_name##Params, &_name##Consts)


#endif
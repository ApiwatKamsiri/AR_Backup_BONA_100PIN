/** @file       eeprom_fileds.h
 *  @brief      EEPROM_FILEDS application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Saqib Sohail
 *  @date       9/4/2024
 */
#ifndef FILE_EEPROM_FILEDS_H
#define FILE_EEPROM_FILEDS_H

/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "system.h"
#include "EE_data.h"
#include "app/refrigerator/rf_ram.h"
#include "app/refrigerator/rf_configs.h"
#include "dev/inverteruartcomm.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_EEPROM_FILEDS_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   EEPROM_FILEDS EEPROM_FILEDS
 *  @ingroup    APP
 *  @brief      Convert rcos variables into Hitachi required format
 *  @{
 */

/// @brief  Events of a EEPROMFileds process
#define TOTAL_ERRORS 29

typedef struct {

    uint8_t errorNumber;
    uint8_t hoursPassed;
    uint8_t daysPassed;
} tsError;

typedef enum
{
    eEEPROMEventPeriodic20Ms = eSysEventCount,
    eEEPROMEventPeriodic1Sec,
    eEEPROMEventPeriodic1Min,
    eEEPROMEventUpdateComplete,
    
} teEEPROMFiledsEvents;

typedef struct
{
    const tsDevIo *filteredAdcRSEN; 
    const tsDevIo *filteredAdcRDSEN;
    const tsDevIo *filteredAdcFSEN;
    const tsDevIo *filteredAdcFDSEN;
    const tsDevIo *filteredAdcIMSEN;
    const tsDevIo *filteredAdcTCSEN;
    const tsDevIo *filteredAdcHSEN;
    /*
    const tsDevIo *filteredAdcRML;
    const tsDevIo *filteredAdcIMFL;
    const tsDevIo *filteredAdcKML;    
    const tsDevIo *filteredAdcIMTEL;*/
} tsEEPROMFiledsDevIos;

typedef struct
{
    tsTimerEvent periodic20Ms;
    tsTimerEvent periodic1Sec;
    tsTimerEvent periodic1Min;
} tsEEPROMFiledsTimers;

/// @brief  Parameters of a EEPROMFileds process
typedef struct
{
    stEEPROM_Ram *eepr;
   
    tsEEPROMFiledsTimers te;
    
    tsEEPROMFiledsDevIos dev;
    
    tsInverterUartCommParams *comp;
    tsError errorQueue[TOTAL_ERRORS]; // store errors and corresponding time
    tsError EEpromErrorImage[4]; // 
    uint8_t errorCheckPrev[29]; // to store previous state of error
    uint8_t TimerErrHis_Old[TOTAL_ERRORS];//Counter timer failure history old
    uint8_t TimerErrHis_New[TOTAL_ERRORS];//Counter timer failure history new
    
    uint16_t FDoorOpenCntTimer1s; //STW
    uint16_t RDoorOpenCntTimer1s; //STW
    uint8_t timeCheckRouter;
    uint8_t WifiConnectimer;
    
    uint8_t FDoorOpenSum; 
    uint8_t RDoorOpenSum; 
    uint16_t CumulativeFdoorOpen; 
    uint16_t CumulativeRdoorOpen;
    
    uint16_t CumulativeFDefost;
    uint16_t CumulativeRDefost;
    
    uint16_t CumulativeWorkingDay;
    
    uint16_t CompSpeed;
    int32_t accumulated_CompFVal[4]; //for F valve
    int32_t accumulated_CompRVal[4]; //for R valve 
    
    uint16_t FValveCont1s; //for function Valve positon time per 2hr.
    uint16_t RValveCont1s; //for function Valve positon time per 2hr.
    
    uint8_t PreviousValvePosition;
    uint8_t ValeOperSum;
    uint8_t GearMoOperSum;
    uint8_t WaterDisOperSum;
    uint16_t CumulativeValveOper;
    uint16_t CumulativeGearMoOper;
    uint16_t CumulativeWaterDisOper;
    uint16_t CumulativeIceCrusheOper;
    uint16_t CumulativeIceCubeOper;
    uint8_t MODEL_SEL;
} tsEEPROMFiledsParams;

/// @brief  Constants of a EEPROMFileds process
typedef struct
{
    const stRefrigerator_Ram *rf;
    const  tsEEPROMFiledsDevIos dev;
} tsEEPROMFiledsConsts;

/** @brief  EEPROMFileds process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_EEPROM_FILEDS_CREATE(_name, _enum,_eepromRam, _compuart, _rfRam, _RSEN, _RDSEN, _FSEN, _FDSEN, _IMSEN, _TCSEN, _HSEN) \
    tsEEPROMFiledsParams _name##Params =           \
        {                               \
             .eepr =   _eepromRam,       \
             .comp =   _compuart,       \
            .te = {                                                                         \
                .periodic20Ms = TIMER_EVENT_INIT(_enum, _enum, eEEPROMEventPeriodic20Ms),     \
                .periodic1Sec = TIMER_EVENT_INIT(_enum, _enum, eEEPROMEventPeriodic1Sec),     \
                .periodic1Min = TIMER_EVENT_INIT(_enum, _enum, eEEPROMEventPeriodic1Min),     \
            },                                                                              \
           .dev = {                                                                         \
              .filteredAdcRSEN = _RSEN,                                                     \
              .filteredAdcRDSEN = _RDSEN,                                                     \
              .filteredAdcFSEN = _FSEN,                                                     \
              .filteredAdcFDSEN = _FDSEN,                                                     \
              .filteredAdcIMSEN = _IMSEN,                                                     \
              .filteredAdcTCSEN = _TCSEN,                                                     \
              .filteredAdcHSEN = _HSEN,                                                   \
           },                                                                           \
        };                                            \
    const tsEEPROMFiledsConsts _name##Consts =     \
        {                                         \
         .rf = _rfRam,                                                               \
    };                                            \
    PROCESS_CREATE(_name, EEPROMFiledsInit, EEPROMFiledsDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_EEPROM_FILEDS_H

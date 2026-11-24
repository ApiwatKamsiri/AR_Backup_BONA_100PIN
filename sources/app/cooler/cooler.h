/** @file       cooler.h
 *  @brief      COOLER application header file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-10
 */
#ifndef FILE_COOLER_H
#define FILE_COOLER_H

/** INCLUDES ******************************************************************/
#include "system.h"
#include "rcos.h"
#include "app/refrigerator/rf_configs.h"
#include "app/refrigerator/rf_ram.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_COOLER_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   COOLER COOLER
 *  @ingroup    APP
 *  @brief      Cooler main process
 *  @{
 */

/// @brief  Events of a Cooler process

extern unsigned char pumpOut;

typedef enum
{
    eCoolerEventPeriodic20Ms = eSysEventCount,
    eCoolerEventPeriodic1Min,
} teCoolerProcEvents;

typedef struct
{
    const tsDevIo *compressor;
    const tsDevIo *condenserFan;
    const tsDevIo *condenserFanError;
    const tsDevIo *ambientTempSensor;
    const tsDevIo *ambientTempSensorError;
    const tsDevIo *humiditySensor;
    const tsDevIo *humiditySensorError;
    const tsDevIo *solenoidValve[2];
    const tsDevIo *testSwitch[2];               //APW : Add CT heater software
    const tsDevIo *CommercialARM;               //APW : Add CT heater software
} tsCoolerProcDevIos;

typedef struct
{
    tsTimerEvent periodic20Ms;
    tsTimerEvent periodic1Min;
} tsCoolerProcTimers;
/// @brief  Parameters of a Cooler process
typedef struct
{
    stCooler_Ram *cooler;
    stRefrigerator_Ram *rf;
    tsCoolerProcTimers te;
    tsCoolerProcDevIos dev;
} tsCoolerProcParams;

/// @brief  Constants of a Cooler process
typedef struct
{
    const stCooler_Flash *cooler;
    const stRefrigerator_Flash *rf;
    const tsDevCpx *sync;
} tsCoolerProcConsts;

#define MIN_STAY_TIME 0
#define MAX_STAY_TIME 1

unsigned char getGasLevelStayTime(const tsCoolerProcConsts *consts, tsCoolerProcParams *params, unsigned char type);
unsigned short getGasLevelStayTimer(const tsCoolerProcConsts *consts, tsCoolerProcParams *params);
void resetGasLevelStayTimers(const tsCoolerProcConsts *consts, tsCoolerProcParams *params);


/** @brief  Cooler process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_COOLER_CREATE(_name, _enum, _coolerRam, _coolerFlash, _sync)              \
    tsCoolerProcParams _name##Params =                                                    \
        {                                                                                 \
            .cooler = _coolerRam,                                                             \
            .rf = __RF_PARAMS_OF(_coolerRam, Cooler),                                     \
            .te = {                                                                       \
                .periodic20Ms = TIMER_EVENT_INIT(_enum, _enum, eCoolerEventPeriodic20Ms), \
                .periodic1Min = TIMER_EVENT_INIT(_enum, _enum, eCoolerEventPeriodic1Min), \
            },                                                                            \
    };                                                                                    \
    const tsCoolerProcConsts _name##Consts =                                              \
        {                                                                                 \
            .cooler = _coolerFlash,                                                           \
            .rf = __RF_CONSTS_OF(_coolerFlash, Cooler),                                   \
            .sync = _sync,                                                                \
    };                                                                                    \
    PROCESS_CREATE(_name, coolerProcInit, coolerProcDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_COOLER_H

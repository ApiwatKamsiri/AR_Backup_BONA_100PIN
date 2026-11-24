/** @file       water_source.h
 *  @brief      WATER_SOURCE application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       8/21/2024
 */
#ifndef FILE_WATER_SOURCE_H
#define FILE_WATER_SOURCE_H

/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "water_source_configs.h"
#include "water_source_ram.h"
#include "system.h"
#include "app/refrigerator/rf_ram.h"
#include "app/refrigerator/rf_configs.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_WATER_SOURCE_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   WATER_SOURCE WATER_SOURCE
 *  @ingroup    APP
 *  @brief      Maria Asif
 *  @{
 */
#define _FILTER_TIME 3120 // 130day * 24hour = 3120 hour  


/// @brief  Events of a WaterSource process
typedef enum
{
    eWaterSourceEventPeriodic20Ms = eSysEventCount,
    eWaterSourceEventPeriodic1Sec,
    eWaterSourceEventPeriodic1Min,
    eWaterSourceEventPeriodic1Hour,
} teWaterSourceEvents;

typedef struct
{
    const tsDevIo  *waterIceTankEmptySensor;
    const tsDevIo  *waterDisTankEmptySensor;
    const tsDevIo  *mainPump;
    const tsDevIo  *dispenserPump;
    const tsDevIo  *iceMachinePump;
    const tsDevIo  *mainValve;
    const tsDevIo  *dispenserValve;
    const tsDevIo  *iceMachineValve;
    const tsDevIo  *waterVentilHeater;
    const tsDevIo  *waterCaseSwitch;

} tsWaterSourceProcDevIos;

typedef struct
{
    tsTimerEvent periodic20Ms;
    tsTimerEvent periodic1Sec;
    tsTimerEvent periodic1Min;
    tsTimerEvent periodic1Hour;
    
} tsWaterSourceTimers;


/// @brief  Parameters of a WaterSource process
typedef struct
{
    stWaterSource_Ram *watersource;
    stRefrigerator_Ram *rf;
    tsWaterSourceTimers te;
    tsWaterSourceProcDevIos dev;

} tsWaterSourceProcParams;

/// @brief  Constants of a WaterSource process
typedef struct
{
    const stWaterSource_Flash *watersource;
    const stRefrigerator_Flash *rf;
    const tsDevCpx *sync;
} tsWaterSourceProcConsts;

/** @brief  WaterSource process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_WATERSOURCE_CREATE(_name, _enum, _WaterSourceRam, _WaterSourceFlash, _sync)         \
    tsWaterSourceProcParams _name##Params =                                                             \
        {                                                                                           \
            .watersource = _WaterSourceRam,                                                         \
            .rf = __RF_PARAMS_OF(_WaterSourceRam, WaterSource),                                     \
            .te = {                                                                                 \
                .periodic20Ms = TIMER_EVENT_INIT(_enum, _enum, eWaterSourceEventPeriodic20Ms),      \
                .periodic1Sec = TIMER_EVENT_INIT(_enum, _enum, eWaterSourceEventPeriodic1Sec),      \
                .periodic1Min = TIMER_EVENT_INIT(_enum, _enum, eWaterSourceEventPeriodic1Min),      \
                .periodic1Hour = TIMER_EVENT_INIT(_enum, _enum, eWaterSourceEventPeriodic1Hour),    \
            },                                                                                      \
    };                                                                                              \
    const tsWaterSourceProcConsts _name##Consts =                                                       \
        {                                                                                           \
            .watersource = _WaterSourceFlash,                                                       \
            .rf = __RF_CONSTS_OF(_WaterSourceFlash, WaterSource),                                   \
            .sync = _sync,                                                                          \
     };                                                                                             \
    PROCESS_CREATE(_name, WaterSourceInit, WaterSourceDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_WATER_SOURCE_H

/** @file       ice_machine.h
 *  @brief      ICE_MACHINE application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner & Obaid Hasan Khan
 *  @date       8/2/2024
 */
#ifndef FILE_ICE_MACHINE_H
#define FILE_ICE_MACHINE_H

/** INCLUDES ******************************************************************/
#include "rcos/core/core.h"
#include "rcos.h"
#include "system.h"
#include "app/refrigerator/rf_configs.h"
#include "app/refrigerator/rf_ram.h"
#include "ice_machine_configs.h"
#include "ice_machine_ram.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_ICE_MACHINE_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   ICE_MACHINE ICE_MACHINE
 *  @ingroup    APP
 *  @brief      Creating an application for ice machine
 *  @{
 */

/// @brief  Events of a Icemachine process
typedef enum
{
    eIceMachineEventPeriodic20ms = eSysEventCount,
    eIceMachineEventPeriodic100ms,
    eIceMachineEventPeriodic1sec,
    eIceMachineEventPeriodic1min,
} teIcemachineEvents;

typedef struct
{
    tsTimerEvent periodic20Ms;
    tsTimerEvent periodic100Ms;
    tsTimerEvent periodic1sec;
    tsTimerEvent periodic1min;
}tsIceMachineProcTimers;

typedef struct
{
    const tsDevIo *caseTempSensor;
    const tsDevIo *caseTempSensorError;
    const tsDevIo *caseRotator;
    const tsDevIo *caseTactSwitch;
    const tsDevIo *fan;
    const tsDevIo *fanError;
    const tsDevIo *boxTactSwitch;
    
}tsIceMachineProcDevIos;

/// @brief  Parameters of a Icemachine process
typedef struct
{
    stIceMachine_Ram *icemachine;
    stRefrigerator_Ram *rf;
    tsIceMachineProcTimers te;
    tsIceMachineProcDevIos dev;
} tsIcemachineProcParams;

/// @brief  Constants of a Icemachine process
typedef struct
{
    const stIceMachine_Flash *icemachine;
    const stRefrigerator_Flash *rf;
    const tsDevCpx *sync;
} tsIcemachineProcConsts;

/** @brief  Icemachine process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_ICE_MACHINE_CREATE(_name, _enum, _IceMachineRam, _IceMachineFlash, _sync) \
    tsIcemachineProcParams _name##Params =                                                    \
        {                                                                                 \
        .icemachine = _IceMachineRam,                                                     \
        .rf = __RF_PARAMS_OF(_IceMachineRam, IceMachine),                                 \
        .te = {                                                                           \
            .periodic20Ms = TIMER_EVENT_INIT(_enum, _enum, eIceMachineEventPeriodic20ms), \
            .periodic100Ms = TIMER_EVENT_INIT(_enum, _enum, eIceMachineEventPeriodic100ms), \
            .periodic1sec = TIMER_EVENT_INIT(_enum, _enum, eIceMachineEventPeriodic1sec), \
            .periodic1min = TIMER_EVENT_INIT(_enum, _enum, eIceMachineEventPeriodic1min), \
        },                                                                                \
    };                                                                                    \
    const tsIcemachineProcConsts _name##Consts =                                              \
        {                                                                                 \
        .icemachine = _IceMachineFlash,                                                   \
        .rf = __RF_CONSTS_OF(_IceMachineFlash, IceMachine),                               \
        .sync = _sync,                                                                    \
    };                                                                                    \
    PROCESS_CREATE(_name, icemachineInit, icemachineDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_ICE_MACHINE_H

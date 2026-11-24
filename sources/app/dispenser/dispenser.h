
/** @file       dispenser.h
 *  @brief      DISPENSER application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Maria Asif
 *  @date       7/24/2024
 */
#ifndef FILE_DISPENSER_H
#define FILE_DISPENSER_H

/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "system.h"

#include "app/dispenser/dispenser_configs.h"
#include "app/refrigerator/rf_ram.h"
#include "app/refrigerator/rf_configs.h"

   
/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_DISPENSER_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   DISPENSER DISPENSER
 *  @ingroup    APP
 *  @brief      dispenser
 *  @{
 */

/// @brief  Events of a Dispenser process
typedef enum
{
    eDispenserEventPeriodic20Ms = eSysEventCount,
    eDispenserEventPeriodic1Sec,

} teDispenserEvents;

typedef struct
{
    const tsDevIo *tactSwitch;
    const tsDevIo *light;
    const tsDevIo *doorSwitch;
} tsDispenserProcDevIos;

typedef struct
{
    tsTimerEvent periodic20Ms;
    tsTimerEvent periodic1Sec;
} tsDispenserProcTimers;

/// @brief  Parameters of a Dispenser process

typedef struct
{

    stDispenser_Ram *dispenser;
    stRefrigerator_Ram *rf;
    tsDispenserProcTimers te;
    tsDispenserProcDevIos dev;
} tsDispenserProcParams;

/// @brief  Constants of a Dispenser process

typedef struct
{

    const stDispenser_Flash *dispenser;
    const stRefrigerator_Flash *rf;
    const tsDevCpx *sync;
    
} tsDispenserProcConsts;

/** @brief  Dispenser process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_DISPENSER_CREATE(_name, _enum, _dispenserRam, _dispenserFlash, _sync)        \
    tsDispenserProcParams _name##Params =                                                        \
        {                                                                                    \
            .dispenser = _dispenserRam,                                                      \
            .rf = __RF_PARAMS_OF(_dispenserRam, Dispenser),                                        \
            .te = {                                                                          \
                .periodic20Ms = TIMER_EVENT_INIT(_enum, _enum, eDispenserEventPeriodic20Ms), \
                .periodic1Sec = TIMER_EVENT_INIT(_enum, _enum, eDispenserEventPeriodic1Sec), \
            },                                                                               \
    };                                                                                       \
    const tsDispenserProcConsts _name##Consts =                                                  \
        {                                                                                    \
            .dispenser = _dispenserFlash,                                                    \
            .rf = __RF_CONSTS_OF(_dispenserFlash, Dispenser),                                \
            .sync = _sync,                                                                   \
    };                                                                                       \
    PROCESS_CREATE(_name, dispenserInit, dispenserDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_DISPENSER_H
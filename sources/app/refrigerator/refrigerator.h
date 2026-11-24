/** @file       refrigerator.h
 *  @brief      REFRIGERATOR application header file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-10
 */
#ifndef FILE_REFRIGERATOR_H
#define FILE_REFRIGERATOR_H

/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "system.h"
#include "rf_configs.h"
#include "rf_ram.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_REFRIGERATOR_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   REFRIGERATOR REFRIGERATOR
 *  @ingroup    APP
 *  @brief      refrigerator main process
 *  @{
 */

// DD: PING TEST
extern unsigned char pingTestS;

/// @brief  Events of a Refrigerator process
typedef enum
{
    eRfEventPeriodic20Ms = eSysEventCount,
    eRfEventPeriodic100Ms,
    eRfEventPeriodic1Sec,
    eRfEventPeriodic1Min,
    eRfEventPeriodic1Hour,
    eRfEventExitMode,
} teRefrigeratorEvents;

typedef struct
{
    tsTimerEvent periodic20Ms;
    tsTimerEvent periodic100Ms;
    tsTimerEvent periodic1Sec;
    tsTimerEvent periodic1Min;
    tsTimerEvent periodic1Hour;
    tsTimerEvent exitMode;
    tsTimerEvent startupDelay;
} tsRfProcTimers;
/// @brief  Parameters of a Refrigerator process
typedef struct
{
    stRefrigerator_Ram *rf;
    tsRfProcTimers te;
} tsRfProcParams;

/// @brief  Constants of a Refrigerator process
typedef struct
{
    const stRefrigerator_Flash *rf;
    const tsDevCpx *sync;
} tsRfProcConsts;

/** @brief  Refrigerator process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_REFRIGERATOR_CREATE(_name, _enum, _rfRam, _rfConsts, _sync)                \
    tsRfProcParams _name##Params =                                                         \
        {                                                                                  \
            .rf = _rfRam,                                                                  \
            .te = {                                                                        \
                .periodic20Ms = TIMER_EVENT_INIT(_enum, _enum, eRfEventPeriodic20Ms),      \
                .periodic100Ms = TIMER_EVENT_INIT(_enum, _enum, eRfEventPeriodic100Ms),    \
                .periodic1Sec = TIMER_EVENT_INIT(_enum, _enum, eRfEventPeriodic1Sec),      \
                .periodic1Min = TIMER_EVENT_INIT(_enum, _enum, eRfEventPeriodic1Min),      \
                .periodic1Hour = TIMER_EVENT_INIT(_enum, _enum, eRfEventPeriodic1Hour),    \
                .exitMode = TIMER_EVENT_INIT(_enum, _enum, eRfEventExitMode),              \
                .startupDelay = TIMER_EVENT_INIT(_enum, _enum, eSysEventInitAfterStartup), \
            },                                                                             \
    };                                                                                     \
    const tsRfProcConsts _name##Consts =                                                   \
        {                                                                                  \
            .rf = _rfConsts,                                                               \
            .sync = _sync,                                                                 \
    };                                                                                     \
    PROCESS_CREATE(_name, refrigeratorProcInit, refrigeratorProcDeinit, _enum, &_name##Params, &_name##Consts)

// AI Cool function prototypes (moved here to avoid circular dependency)
#if defined(AI_COOL_V1_5)
void RefrigeratorAIInit(stRefrigerator_Ram *rfP);
void RefrigeratorAIT1sec(stRefrigerator_Ram *rfP);
void RefrigeratorAIT1hour(stRefrigerator_Ram *rfP);
unsigned char RefrigeratorAIChangeCompressorSpeed(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void RefrigeratorAIDecreaseCabinSetsIfRequired(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void RefrigeratorAIChangeDefrostIntervals(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
#endif

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_REFRIGERATOR_H

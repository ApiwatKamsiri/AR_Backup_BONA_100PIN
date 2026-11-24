/** @file       ice_source.h
 *  @brief      ICE_SOURCE application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner & Obaid Hasan Khan
 *  @date       8/15/2024
 */
#ifndef FILE_ICE_SOURCE_H
#define FILE_ICE_SOURCE_H
/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "system.h"

#include "app/ice_source/ice_source_configs.h"
#include "app/refrigerator/rf_ram.h"
#include "app/refrigerator/rf_configs.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_ICE_SOURCE_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   ICE_SOURCE ICE_SOURCE
 *  @ingroup    APP
 *  @brief      Crerating ice source application
 *  @{
 */

/// @brief  Events of a Icesource process
typedef enum
{
    eIceSourceEventPeriodic20Ms = eSysEventCount,
} teIcesourceProcEvents;

typedef struct
{
    tsTimerEvent periodic20Ms;
}tsIceSourceTimers;

typedef struct
{
    const tsDevIo  *selector;
    const tsDevIo  *shutter;
    const tsDevIo  *pusher;
} tsIcesourceProcDevIos;

/// @brief  Parameters of a Icesource process
typedef struct
{
    stIceSource_Ram *icesource;
    stRefrigerator_Ram *rf;
    tsIceSourceTimers te;
    tsIcesourceProcDevIos dev;
} tsIcesourceProcParams;

/// @brief  Constants of a Icesource process
typedef struct
{
    const stIceSource_Flash *icesource;
    const stRefrigerator_Flash *rf;
    const tsDevCpx *sync;
} tsIcesourceProcConsts;

/** @brief  Icesource process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_ICE_SOURCE_CREATE(_name, _enum, _IceSourceRam, _IceSourceFlash, _sync)          \
    tsIcesourceProcParams _name##Params =                                                       \
        {                                                                                       \
            .icesource = _IceSourceRam,                                                         \
            .rf = __RF_PARAMS_OF(_IceSourceRam, IceSource),                                     \
            .te = {                                                                             \
                 .periodic20Ms = TIMER_EVENT_INIT(_enum, _enum, eIceSourceEventPeriodic20Ms),   \
                  },                                                                            \
        };                                                                                      \
    const tsIcesourceProcConsts _name##Consts =                                                 \
        {                                                                                       \
            .icesource = _IceSourceFlash,                                                       \
            .rf = __RF_CONSTS_OF(_IceSourceFlash, IceSource),                                   \
            .sync = _sync,                                                                      \
        };                                                                                      \
    PROCESS_CREATE(_name, icesourceInit, icesourceDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_ICE_SOURCE_H
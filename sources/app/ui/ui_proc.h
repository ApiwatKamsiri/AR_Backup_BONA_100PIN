/** @file       ui_proc.h
 *  @brief      UI_PROC application header file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-09-13
 */
#ifndef FILE_UI_PROC_H
#define FILE_UI_PROC_H

/** INCLUDES ******************************************************************/
#include "system.h"
#include "rcos.h"
#include "app/refrigerator/rf_configs.h"
#include "app/refrigerator/rf_ram.h"
#include "dev/display_spi.h"
#include "ui_ram.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_UI_PROC_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   UI_PROC UI_PROC
 *  @ingroup    APP
 *  @brief      User interface process
 *  @{
 */

/// @brief  Events of a UiProc process
typedef enum
{
    eUiEventEnterNewState = eSysEventCount,
    eUiEventPeriodic20Ms,
    eUiEventPeriodic1Sec,
    eUiEventPeriodic1Min,
} teUiProcEvents;

typedef struct
{
    tsTimerEvent periodic20Ms;
    tsTimerEvent periodic1Sec;
    tsTimerEvent periodic1Min;
} tsUiProcTimers;

/// @brief  Parameters of a UiProc process
typedef struct
{
    tsUiRam *ui;
    stRefrigerator_Ram *rf;
    tsUiProcTimers te;
} tsUiProcParams;

/// @brief  Constants of a UiProc process
typedef struct
{
    const stRefrigerator_Flash *rf;
    const tsDevCpx *dsp;
    const tsDevIo *buzzer;
} tsUiProcConsts;

/** @brief  UiProc process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_UI_CREATE(_name, _enum, _uiRam, _rfRam, _rfFlash, _display)           \
    tsUiProcParams _name##Params =                                                    \
        {                                                                             \
            .ui = _uiRam,                                                             \
            .rf = _rfRam,                                                             \
            .te = {                                                                   \
                .periodic20Ms = TIMER_EVENT_INIT(_enum, _enum, eUiEventPeriodic20Ms), \
                .periodic1Sec = TIMER_EVENT_INIT(_enum, _enum, eUiEventPeriodic1Sec), \
                .periodic1Min = TIMER_EVENT_INIT(_enum, _enum, eUiEventPeriodic1Min), /*** APW : Add CT heater software ***/ \
            },                                                                        \
    };                                                                                \
    const tsUiProcConsts _name##Consts =                                              \
        {                                                                             \
            .rf = _rfFlash,                                                           \
            .dsp = &display,                                                          \
    };                                                                                \
    PROCESS_CREATE(_name, uiProcInit, uiProcDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

void uiInitStates(tsUiRam *uiP);
void uiResetDisplayOffTimer(tsUiRam *uiP);
void uiResetSetChangeTimer(tsUiRam *uiP);

static void ui20MSecTask(const tsUiProcConsts *consts, tsUiProcParams *params);
static void ui1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params);
static void uiTask(const tsUiProcConsts *consts, tsUiProcParams *params);

#undef INTERFACE // Should not let this roam free

#endif // FILE_UI_PROC_H

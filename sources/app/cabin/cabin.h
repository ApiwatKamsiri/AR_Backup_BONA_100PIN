/** @file       cabin.h
 *  @brief      CABIN application header file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-10
 */
#ifndef FILE_CABIN_H
#define FILE_CABIN_H

/** INCLUDES ******************************************************************/
#include "system.h"
#include "rcos.h"
#include "app/refrigerator/rf_configs.h"
#include "app/refrigerator/rf_ram.h"
#include "procedures/cabin_ai_cool.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_CABIN_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   CABIN CABIN
 *  @ingroup    APP
 *  @brief      cabin main process
 *  @{
 */

/// @brief  Events of a Cabin process
typedef enum
{
    eCabinEventPeriodic20Ms = eSysEventCount,
    eCabinEventPeriodic1Sec,
    eCabinEventPeriodic1Min,
    eCabinEventPeriodic1Hour,
    eCabinEventStartPerformanceAndSensorsTest,
    eCabinEventIsProductionTestCondition,
    eCabinEventStartCommercialTest,                                             //APW : Add CT heater software
} teCabinProcEvents;
typedef struct
{
    const tsDevIo *fan;
    const tsDevIo *fanError;
    const tsDevIo *flap;
    const tsDevIo *heaterB;
    const tsDevIo *heaterA;
    const tsDevIo *heaterC;
    const tsDevIo *ionizer;
    const tsDevIo *innerLight;
    const tsDevIo *blueLight;
    const tsDevIo *handleLight_R;
    const tsDevIo *handleLight_G;
    const tsDevIo *handleLight_B;
    const tsDevIo *doorSwitch[2]; 
    const tsDevIo *humanSensor; 
    const tsDevIo *airTempSensor;
    const tsDevIo *airTempSensorError;
    const tsDevIo *evaTempSensor;
    const tsDevIo *evaTempSensorError;
    const tsDevIo *knob;
    const tsDevIo *knobError;
    const tsDevIo *violed;
    const tsDevIo *vacuumBox;
    const tsDevIo *vacuumBoxHeater;
    const tsDevIo *vacuumBoxOverCurrentSensor;
} tsCabinProcDevIos;

typedef struct
{
    tsTimerEvent periodic20Ms;
    tsTimerEvent periodic1Sec;
    tsTimerEvent periodic1Min;
    tsTimerEvent periodic1Hour;
} tsCabinProcTimers;

/// @brief  Parameters of a Cabin process
typedef struct
{
    stCabin_Ram *cabin;
    stRefrigerator_Ram *rf;
    tsCabinProcTimers te;
    tsCabinProcDevIos dev;
} tsCabinProcParams;

/// @brief  Constants of a Cabin process
typedef struct
{
    uint8_t cabinNo;
    const stCabin_Flash *cabin;
    const stRefrigerator_Flash *rf;
    const tsDevCpx *sync;
} tsCabinProcConsts;

/** @brief  Cabin process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */

#define PROCESS_CABIN_CREATE(_name, _enum, _cabinNo, _cabinRam, _cabinFlash, _sync)                     \
    tsCabinProcParams _name##Params =                                                                   \
        {                                                                                               \
            .cabin = _cabinRam,                                                                         \
            .rf = __RF_PARAMS_OF(_cabinRam, Cabin[_cabinNo]),                                           \
            .te = {                                                                                     \
                .periodic20Ms = TIMER_EVENT_INIT(_enum, _enum, eCabinEventPeriodic20Ms),                \
                .periodic1Sec = TIMER_EVENT_INIT(_enum, _enum, eCabinEventPeriodic1Sec),                \
                .periodic1Min = TIMER_EVENT_INIT(_enum, _enum, eCabinEventPeriodic1Min),                \
                .periodic1Hour = TIMER_EVENT_INIT(_enum, _enum, eCabinEventPeriodic1Hour),              \
            },                                                                                          \
    };                                                                                                  \
    const tsCabinProcConsts _name##Consts =                                                             \
        {                                                                                               \
            .cabinNo = _cabinNo,                                                                        \
            .cabin = _cabinFlash,                                                                       \
            .rf = __RF_CONSTS_OF(_cabinFlash, Cabin[_cabinNo]),                                         \
            .sync = _sync,                                                                              \
    };                                                                                                  \
    PROCESS_CREATE(_name, cabinProcInit, cabinProcDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

INTERFACE unsigned char cabinGetLogData(stCabin_Ram *cabinP, unsigned char *dataArrayPtr);

// AI Cool function prototypes (declared here to avoid circular dependencies)
#if defined (AI_COOL_V1)
INTERFACE void cabinDefrostUpdateInterval(tsCabinProcConsts const *consts, tsCabinProcParams *params);
INTERFACE void CabinAIResetLabel(tsCabinProcParams *params);
INTERFACE void CabinAIResetCompressorVal(tsCabinProcParams *params);
INTERFACE void CabinAIInit(tsCabinProcParams *params);
INTERFACE void CabinAIFindCompressorVal(tsCabinProcConsts const *consts, tsCabinProcParams *params);
INTERFACE void CabinAIFindLinearFunction(stPoint point1, stPoint point2, double *slope, double *yIntercept);
INTERFACE unsigned char CabinAIGetCompressorValAccordingToDistance(tsCabinProcParams *params);
INTERFACE unsigned char CabinAICheckCompressorValLimits(unsigned char speed, tsCabinProcParams *params);
INTERFACE unsigned char CabinAIGetCompressorValAccordingToLabel(tsCabinProcParams *params);
INTERFACE unsigned char CabinAIGetCompressorValAccordingToOnTimer(const stCabin_Flash *cabinC, stCabin_Ram *cabinP, const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
INTERFACE void CabinAIFindLabelWithCmpVal(tsCabinProcConsts const *consts, tsCabinProcParams *params);
INTERFACE void CabinAIDecideLabelForEachHour(tsCabinProcParams *params);
INTERFACE void CabinAILabelForEachHour(tsCabinProcConsts const *consts, tsCabinProcParams *params);
INTERFACE void CabinAIDecreaseCabinSetIfRequired(const stCabin_Flash *cabinC, stCabin_Ram *cabinP);
INTERFACE void CabinAIT1sec(tsCabinProcConsts const *consts, tsCabinProcParams *params);
INTERFACE void CabinAIT1hour(tsCabinProcConsts const *consts, tsCabinProcParams *params);
#endif // AI_COOL_V1

#undef INTERFACE // Should not let this roam free

#endif // FILE_CABIN_H

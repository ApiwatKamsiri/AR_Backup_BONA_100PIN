/** @file       energymonitoring.h
 *  @brief      ENERGYMONITORING application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Baris KARA
 *  @date       26.08.2024
 */
#ifndef FILE_ENERGYMONITORING_H
#define FILE_ENERGYMONITORING_H

/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "system.h"
#include "inverteruartcomm.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_ENERGYMONITORING_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

#define ENERGYMONITORING_TIME1SEC 1000
#define ENERGYMONITORING_TIME1MIN 60000

/**
 *  @defgroup   ENERGYMONITORING ENERGYMONITORING
 *  @ingroup    APP
 *  @brief      energy calculation
 *  @{
 */

#define _EnergyBufferSize 60

#define CONSUMPTION_ID_ENERGY 1
#define CONSUMPTION_ID_WATER  6

#define MEASUREMENT_UNIT_ENERGY 11

#define MEASUREMENT_WA_INDEX 9         // need to be compitable with wifiArrayIndex_e enumarations
#define MEASUREMENT_IS_READY 50         // consumption calculation ready
#define MEASUREMENT_IS_READ  0          // consumption read by safir or cancelled

#define ENERGY_READY_TIME_LIMIT 60
#define ENERGY_READY_MEASUREMENT_LIMIT 200*60*100

#define ENERGY_CONSUMPTION_SAVE_E2_PERIOD 5

#define STATE_TIME_IDLE 5
#define STATE_TIME_READY 55


INTERFACE unsigned char energyMonitoringMeasurement;

typedef struct
{
  unsigned char index;
}tsBuffer;

typedef struct
{
  unsigned int totalValue;
  unsigned int instantValue;
  unsigned short averageValue;
  tsBuffer Buffer;
}tsT1;

typedef struct
{
  unsigned char ID;
  unsigned int Measurement;
  unsigned int TimeInterval;
  unsigned short MeasurementUnit;
}stWifiEnergyPacket;

typedef enum{
    _ENERGY_IDLE = 0,
    _ENERGY_MEASURING = 1,
    _ENERGY_READY = 2,
}teRefrigeratorEnergyStates;

typedef struct
{
  tsT1 T1sec;
  tsT1 T1min;
  teRefrigeratorEnergyStates state;
  unsigned char stateTimerT1sec;
}tsRefrigeratorEnergy;


typedef enum
{
    eEmEventsNone,
    eEmPeriodic1Sec,
    eEmPeriodic1Min,
} teEnergyMonitoringEvents;

typedef struct
{
    tsTimerEvent periodic1Sec;
    tsTimerEvent periodic1Min;
} tsEnergyMonitoringProcTimers;

/// @brief  Events of a EnergyMonitoring process
//typedef enum
//{
//} teEnergyMonitoringEvents;

/// @brief  Parameters of a EnergyMonitoring process
typedef struct
{
    tsRefrigeratorEnergy energy;
    tsEnergyMonitoringProcTimers te;
    stCompressor *compressor;
} tsEnergyMonitoringParams;

/// @brief  Constants of a EnergyMonitoring process
typedef struct
{
  unsigned char temp;
} tsEnergyMonitoringConsts;

/** @brief  EnergyMonitoring process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object )
 */
#define PROCESS_ENERGYMONITORING_CREATE(_name, _enum,  _compressorParams) \
    tsEnergyMonitoringParams _name##Params =           \
        {                                         \
        .compressor = _compressorParams, \
        .te = {   \
            .periodic1Sec = TIMER_EVENT_INIT(_enum, _enum, eEmPeriodic1Sec),\
            .periodic1Min = TIMER_EVENT_INIT(_enum, _enum, eEmPeriodic1Min), \
        }, \
    };                                            \
    const tsEnergyMonitoringConsts _name##Consts =     \
        {                                         \
        .temp = 0   \
    };                                            \
    PROCESS_CREATE(_name, energyMonitoringInit, energyMonitoringDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_ENERGYMONITORING_H

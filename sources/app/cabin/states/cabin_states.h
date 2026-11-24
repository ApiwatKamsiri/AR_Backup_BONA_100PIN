#ifndef _Cabin_States_H
#define _Cabin_States_H

#include "cabin_states_cool.h"
#include "cabin_states_defrost.h"
#include "cabin_states_fan.h"
#include "cabin_states_flap.h"
#include "cabin_states_a_heater.h"
#include "cabin_states_b_heater.h"
#include "cabin_states_c_heater.h"

typedef enum
{
    _Sensor_AirP = 1,
    _Sensor_AirF = 2,
    _Sensor_EvaP = 4,
    _Sensor_EvaF = 8,
} enSensorStates;

typedef enum
{
    _Door_Open,
    _Door_Close,
} enDoorStates;

typedef enum
{
    _Work_Idle,
    _Work_Cool,
    //_Work_Heat,
    //_Work_PreCool,
    _Work_Defrost,
    _Work_Drainage,
    _Work_CoolWait,
    _Work_HotStart,
    _Work_PumpOut,
    _Work_FairMode,
    _Work_DistributerMode,
    _Work_ServiceTest,
    _Work_ProductionTestMode,
    _Work_PerformanceTest,
    _Work_CommercialTestMode,                                                    //APW : Add CT heater software
    _Work_CommercialDefrostTestMode,                                             //APW : Add CT heater software
} enWorkStates;

typedef struct
{
    enSensorStates Sensor;
    //enDoorStates Door;
    enWorkStates Work;
    enWorkStates WorkPrev;
    //enWorkStates WorkDesired;
    enCoolStates Cool;
    enDefrostStates Defrost;
    enFanStates Fan;
    enFlapStates Flap;
    enHeaterAStates HeaterA;
    enHeaterBStates HeaterB;
    enHeaterCStates HeaterC;
} stCabinStates;

#endif

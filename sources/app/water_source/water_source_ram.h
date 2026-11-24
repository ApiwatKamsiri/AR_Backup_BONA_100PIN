#ifndef WATER_SOURCE_RAM_H
#define WATER_SOURCE_RAM_H

#include "app/refrigerator/components_ram.h"
#include "water_source_configs.h"

#define _FILTER_TIME 3120 // 130day * 24hour = 3120 hour



typedef enum
{
    _WaterSourceValue_OFF,
    _WaterSourceValue_PUMP,
    _WaterSourceValue_VACUM,
} enWaterSourceValues;

typedef struct
{
    enWaterSourceValues iceMachineDesired : 2;
    enWaterSourceValues iceMachineApplied : 2;
    enWaterSourceValues dispenserDesired : 2;
    enWaterSourceValues dispenserApplied : 2;
    //unsigned char errFilterTime:1;
    //unsigned char rstFilterTime:1;
    unsigned char noWaterSet : 1;
    unsigned char noWaterClr : 1;
    unsigned char waterCaseOpened : 1;
    unsigned char doorsClosedState : 1;
    unsigned short noWaterClrTimer;
    unsigned short heaterTimer;

    unsigned char errFilterTime : 1;
    unsigned char errFilterEnabled : 1;
    unsigned short filterTimer;

    enWaterSourceValues iceMachineDesiredPrv : 2;
    enWaterSourceValues dispenserDesiredPrv : 2;
    unsigned char startTimer;
    unsigned short iceOnFirstOnTimer;
    unsigned short ProhibitPumpWhenIceOnFirstOnTimer;
    unsigned short ProhibitPumpAfterDoorClearEmptyTimer;
    unsigned char componentsTestDone;
    unsigned char waterTankEmpty;
    bool ProhibitionWaterTankEmpty;
    signed int waterIceTankSensorValueInC;
    signed int waterDisTankSensorValueInC;
    bool EmptyDoorOpenClear;
    bool EmptyDoorCloseClear;
    bool SkipEmptyCheck;
    
} stWaterSourceValues;

typedef struct
{
    stTempSensor_Ram waterIceTankEmptySensor;
    stTempSensor_Ram waterDisTankEmptySensor;
    stPump_Ram mainPump;
    stPump_Ram dispenserPump;
    stPump_Ram iceMachinePump;
    stValve_Ram mainValve;
    stValve_Ram dispenserValve;
    stValve_Ram iceMachineValve;
    stHeater_Ram waterVentilHeater;
    stSwitch_Ram waterCaseSwitch;
} stWaterSourceComponents_Ram;

typedef struct
{
    unsigned short filterTimer;
    unsigned char errFilterEnabled;
} stWaterSourceE2_Ram;

typedef struct
{
    stWaterSourceValues values;
    stWaterSourceComponents_Ram components;
    stWaterSourceE2_Ram E2;
} stWaterSource_Ram;

#endif


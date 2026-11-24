#ifndef WATER_SOURCE_CONFIGS_H
#define WATER_SOURCE_CONFIGS_H

#include "app/refrigerator/components_configs.h"

typedef enum
{
    _WaterSupplyType_NONE = 0,
    _WaterSupplyType_INTERNAL_IceMachine,
    _WaterSupplyType_INTERNAL_Dispenser,
    _WaterSupplyType_INTERNAL_IceMachine_Dispenser,
    _WaterSupplyType_EXTERNAL_IceMachine,
    _WaterSupplyType_EXTERNAL_Dispenser,
    _WaterSupplyType_EXTERNAL_IceMachine_Dispenser,
} enWaterSupplyTypes;

typedef enum
{
    _WaterFilterType_NONE = 0,
    _WaterFilterType_DEFAULT,
} enWaterFilterTypes;

typedef struct
{
    enWaterSupplyTypes supply : 3;
    enWaterFilterTypes filter : 1;
} stWaterSourceTypes;

typedef struct
{
    struct
    {
        unsigned short OnTimeA[5];
        unsigned short OffTimeA[5]; 
        unsigned short OnTimeB;
        unsigned short OffTimeB;
        unsigned short IceOnFirstOnTime;
        unsigned short ProhibitPumpWhenIceOnFirstOnTime;
        unsigned short ProhibitPumpAfterDoorClearEmptyTime;
    } waterVentilHeater;
    
    struct
    {
        signed short JudgmentZoneA;
        signed short JudgmentZoneB;
        signed short JudgmentZoneC;
        signed short JudgmentZoneD;
    } waterIceTankEmptySensor;
    
    struct
    {
        signed short JudgmentZoneA;
        signed short JudgmentZoneB;
        signed short JudgmentZoneC;
        signed short JudgmentZoneD;
    } waterDisTankEmptySensor;
} stWaterSourceParameters;


typedef struct
{
    stTempSensor_Flash waterIceTankEmptySensor;
    stTempSensor_Flash waterDisTankEmptySensor;
    stPump_Flash mainPump;
    stPump_Flash dispenserPump;
    stPump_Flash iceMachinePump;
    stValve_Flash mainValve;
    stValve_Flash dispenserValve;
    stValve_Flash iceMachineValve;
    stHeater_Flash waterVentilHeater;
    stSwitch_Flash waterCaseSwitch;
} stWaterSourceComponents_Flash;

typedef struct
{
    stWaterSourceTypes type;
    stWaterSourceComponents_Flash components;
    stWaterSourceParameters parameters;
} stWaterSource_Flash;

#endif


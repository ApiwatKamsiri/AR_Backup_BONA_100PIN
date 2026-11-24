#ifndef CABIN_CONFIGS_H
#define CABIN_CONFIGS_H

#include "app/refrigerator/components_configs.h"
#include "cabin_sets.h"
#include "parameters/cabin_parameters.h"

typedef enum
{
    _idle = 0,
    _checkalarmCond,
    _waitBeforeActivation,
    _alarmIsActive,
} enhighTempStatus;

extern unsigned char highTempStatus;

typedef enum
{
    _CabinType_NONE = 0,
    _CabinType_FF,
    _CabinType_FRZ,
    _CabinType_JKR,
    _CabinType_FLAP,
    //_CabinType_BioFRESH,
} enCabinTypes;


typedef struct
{
    stFan_Flash fan;
    stFlap_Flash flap;
    stHeater_Flash heaterB;
    stHeater_Flash heaterA;
    stHeater_Flash heaterC;
    stIonizer_Flash ionizer;
    stLight_Flash innerLight;
    stLight_Flash blueLight;
    stHandleRGBLight_Flash handleLight;
    stSwitch_Flash doorSwitch[2];
    stSwitch_Flash humanSensor;
    stTempSensor_Flash airTempSensor;
    stTempSensor_Flash evaTempSensor;
    stKnob_Flash knob;
    stVacuumBox_Flash vacuumBox;
    stHeater_Flash vacuumBoxHeater;
    stTempSensor_Flash vacuumBoxOverCurrentSensor;
#ifdef ALPS_H50
    stSwitch_Flash doorAssistInputSwitch;    
    stTempSensor_Flash doorAssistMotorOverCurrentSensor;
    stDoorAssistMotor_Flash doorAssistMotor;
    stDoorAssistOut_Flash doorAssistOut;
#endif    
    stVioLed_Flash violed;
} stCabinComponents_Flash;


typedef struct
{
    enCabinTypes type;
    stCabinComponents_Flash components;
    stCabinSets_Flash sets;
    stCabinParameters parameters;
} stCabin_Flash;
#endif
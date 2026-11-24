#ifndef ICE_MACHINE_CONFIGS_H
#define ICE_MACHINE_CONFIGS_H

#include "app/refrigerator/components_configs.h"

typedef enum
{
    _IceMachineType_NONE = 0,
    _IceMachineType_DEFAULT,
} enIceMachineTypes;

/*
typedef struct{
  unsigned char waterPumpTime;
}stIceMachineValues_Flash;
*/

typedef struct
{
    stTempSensor_Flash caseTempSensor;
    stRotator_Flash caseRotator;
    stSwitch_Flash caseTactSwitch;
    stFan_Flash fan;
    stSwitch_Flash boxTactSwitch;
} stIceMachineComponents_Flash;


typedef struct
{
    unsigned char waterPumpTime;        // In 20ms   ( 250 )
    signed short checkIceTempAtLongWait;  // In ADC    ( -8 C Degree )
    signed short checkIceTempAtShortWait; // In ADC    ( -12 C Degree )
    unsigned char makeIceLongWaitTime;  // In minute ( 130 )
    unsigned char makeIceShortWaitTime; // In minute ( 20 )
    unsigned char checkWaterEnabled : 1;
    unsigned short checkWaterTime;        // In second ( 300 )
    unsigned char checkWaterTempTime;   // In second ( 60 )
    signed short checkWaterTemp;          // In ADC    ( 3 C Degree kapali )
    unsigned char quickIceOnDisplay;
    signed short quickCutInTemp;          // In ADC
    signed short quickCutOutTemp;         // In ADC
    unsigned short waterPumpWaitTime;
    unsigned short iceMachineFanWaitTime;
    //unsigned char HBY_FFSet;
    unsigned short HBY_SetDecrease;       //In ADC
    signed short HBY_CaseTempThermalCutOut;
    unsigned char quickIce60WaterPumpTime;              // In 20ms   ( 250 ) <<QuickIce60 
    signed short quickIce60checkIceTempAtLongWait;  // In ADC    ( -8 C Degree )
    signed short quickIce60checkIceTempAtShortWait; // In ADC    ( -12 C Degree )
    unsigned char quickIce60makeIceLongWaitTime;  // In minute ( 130 )
    unsigned char quickIce60makeIceShortWaitTime; // In minute ( 20 )
    unsigned short quickIce60MaximumTime;                // In 20ms   ( ... ) <<QuickIce60 
    unsigned char quickIce60MaximumTimes;                // In 20ms   ( 250 ) <<QuickIce60 
} stIceMachineParameters_Flash;

typedef struct
{
    enIceMachineTypes type : 1;
    stIceMachineComponents_Flash components;
    stIceMachineParameters_Flash parameters;
} stIceMachine_Flash;

#endif
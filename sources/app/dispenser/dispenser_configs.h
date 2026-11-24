#ifndef DISPENSER_CONFIGS_H
#define DISPENSER_CONFIGS_H

#include "app/refrigerator/components_configs.h"

typedef enum
{
    _DispenserWaterType_NONE = 0,
    _DispenserWaterType_WaterPump,
    _DispenserWaterType_WaterPumpAndVacum,
} enDispenserWaterTypes;

typedef enum
{
    _DispenserIceType_NONE = 0,
    _DispenserIceType_IceCube,
    _DispenserIceType_IceCubeAndCrash,
} enDispenserIceTypes;

typedef struct
{
    stSwitch_Flash tactSwitch;
    stLight_Flash light;
    stSwitch_Flash doorSwitch;
} stDispenserComponents_Flash;

typedef struct
{
    unsigned char autofillTime_8OZ;
    unsigned char autofillTime_16OZ;
    unsigned char autofillTime_32OZ;
} stAutofillParameters_Flash;

typedef struct
{
    unsigned char OFFState : 1;
    unsigned char lightTime;
    stAutofillParameters_Flash autofill;
} stDispenserParameters_Flash;

typedef struct
{
    enDispenserWaterTypes waterType : 2;
    enDispenserIceTypes iceType : 2;
    stDispenserComponents_Flash components;
    stDispenserParameters_Flash parameters;
} stDispenser_Flash;


#endif
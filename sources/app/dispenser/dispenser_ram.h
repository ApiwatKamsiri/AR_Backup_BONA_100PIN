#ifndef DISPENSER_RAM_H
#define DISPENSER_RAM_H
#include "app/water_source/water_source_ram.h"
#include "app/ice_source/ice_source_ram.h"
#include "dispenser_configs.h"

typedef enum
{
    _DispenserState_OFF = 0,
    _DispenserState_WaterPump,
    _DispenserState_WaterVacum,
    _DispenserState_IceCube,
    _DispenserState_IceCrash,
    _DispenserState_Measured_8OZ,
    _DispenserState_Measured_16OZ,
    _DispenserState_Measured_32OZ,
    /*_DispenserState_Measured_Custom,
    _DispenserState_Measured_CalibrateCustom,*/
} enDispenserStates;

typedef struct
{
    enDispenserStates lastState;
    enDispenserStates state;
    unsigned char waterPumpTime;
    unsigned char waterVacumTime;
    unsigned char lightTime;
    unsigned char waterCalibrateTime;
    unsigned char commandIsNotAllowedF;
} stDispenserSets;

typedef struct
{
    enWaterSourceValues waterDesired : 2;
    enWaterSourceValues waterApplied : 2;
    unsigned char waterPumpTimer;
    unsigned char waterVacumTimer;

    enIceSourceValues iceDesired : 2;
    enIceSourceValues iceApplied : 2;
    unsigned char sabbathEnabled : 1;
    unsigned char iceTimer;
    unsigned char iceWaitTimer;
    unsigned short iceWaitTimer2;
    unsigned char iceContinueTimer;
    unsigned char iceContinueFactorTimer;
    unsigned short lightTimer;
    unsigned char lightEffectTimer;
    unsigned char dispenserTactFlag;
    unsigned char calibrateLockFlag;
    unsigned char tactTriggered;
    unsigned char dispenserProhibitFlag;
    unsigned char dispenserProhibitTimer;
    unsigned char dispenserProhibitBlinkFlag;
} stDispenserValues;

typedef struct
{
    stSwitch_Ram tactSwitch;
    stLight_Ram light;
    stSwitch_Ram doorSwitch;
} stDispenserComponents_Ram;

typedef struct
{
    enDispenserStates state;
    unsigned char lightTime;
    unsigned char waterCalibrateTime;
} stDispenserE2_Ram;

typedef struct
{
    stDispenserSets sets;
    stDispenserValues values;
    stDispenserComponents_Ram components;
    stDispenserE2_Ram E2;
} stDispenser_Ram;

#endif
#ifndef ICE_MACHINE_RAM_H
#define ICE_MACHINE_RAM_H


#include "app/refrigerator/components_ram.h"
#include "app/water_source/water_source_ram.h"

typedef struct
{
    unsigned char iceOFF : 1;
    unsigned char iceOFFToON : 1;
    unsigned char waterPumpTime;  
    unsigned char quickIce60WaterPumpTime;      //<<QuickIce60 
    unsigned char quickIce60MaximumTime;        //<<QuickIce60  
    unsigned char quickIce60MaximumTimes;        //<<QuickIce60 
    unsigned char quickIceOnDisplay;
} stIceMachineSets;

typedef enum
{
    _IMch_State_InitCase = 0,
    _IMch_State_FillWater_IsWaterNeeded,
    _IMch_State_FillWater_PumpingWater_Init,
    _IMch_State_FillWater_PumpingWater,
    _IMch_State_FillWater_VacumWater_Init,
    _IMch_State_FillWater_VacumWater,
    _IMch_State_CheckWater_Init,
    _IMch_State_CheckWater_IsThereWater,
    _IMch_State_NoWater,
    _IMch_State_MakeIce_LongWait_Init,
    _IMch_State_MakeIce_LongWait,
    _IMch_State_MakeIce_ShortWait_Init,
    _IMch_State_MakeIce_ShortWait,
    _IMch_State_MakeIce_ColdStartWait_Init,
    _IMch_State_MakeIce_ColdStartWait,
    _IMch_State_CheckBox,
    _IMch_State_NoBox,
    _IMch_State_BoxFull,
    _IMch_State_PureIce,
    _IMch_State_IceOff,
    _IMch_State_Error,
    _IMch_CommercialState_InitCase,                      /*** APW : Add Ice making operation test in CT ***/
    _IMch_CommercialState_ReadyToTest,                   /*** APW : Add Ice making operation test in CT ***/
    _IMch_CommercialState_IceBoxToEndTest,               /*** APW : Add Ice making operation test in CT ***/
    _IMch_CommercialState_IceBoxToStartTest,             /*** APW : Add Ice making operation test in CT ***/
    _IMch_CommercialState_WaterSuplyForwardTest,         /*** APW : Add Ice making operation test in CT ***/
    _IMch_CommercialState_WaterSuplyReverseTest,         /*** APW : Add Ice making operation test in CT ***/
    _IMch_CommercialState_GoToOriginalBeforeEndCT,       /*** APW : Add Ice making operation test in CT ***/
    _IMch_CommercialState_TestComplete,                  /*** APW : Add Ice making operation test in CT ***/
    _IMch_CommercialState_IceBoxDetected,                /*** APW : Add Ice making operation test in CT ***/
    _IMch_CommercialDefrostState_IceBoxNotOperate,      /*** APW : Add Ice making operation test in CT ***/
} enIceMachineStates;

typedef enum
{
    _IMch_Step_U = 0,
    _IMch_Step_0,
    _IMch_Step_1,
    _IMch_Step_2,
    _IMch_Step_3,
    _IMch_Step_4,
    _IMch_Step_5,
    _IMch_Step_6,
} enIceMachineSteps;


typedef struct
{
    //  unsigned char iceOFF:1;
    unsigned char firstIce : 1;
    //unsigned char block:1;
    unsigned char quickIce : 1;
    unsigned char noWaterSet : 1;
    unsigned char errCaseTactSW : 1;
    unsigned char initStep : 1;
    unsigned char noWaterClr : 1;
    unsigned char doorclosedforquickice : 1;

    enWaterSourceValues waterDesired : 2;
    enWaterSourceValues waterApplied : 2;

    //unsigned char quickIce:1;
    unsigned char block : 1;
    unsigned char sabbathEnabled : 1;
    unsigned char powerOn : 1;

    enIceMachineStates state;
    enIceMachineSteps step;

    unsigned short waterPumpTimer;
    unsigned short waterVacumTimer;
    unsigned short quickIce60WaterPumpTimer;            //<<QuickIce60
    unsigned short quickIce60MaximumTimer;              //<<QuickIce60
    unsigned short quickIce60MaximumTimesCount;         //<<QuickIce60
    unsigned short stepTimer;
    unsigned char makeIceTimer;
    unsigned char makeIceTimerBuffer;

    unsigned short checkWaterTimer;
    unsigned char checkWaterTempTimer;
    unsigned char checkBoxTimer;

    unsigned char caseTactSWErrorTimer;
    unsigned char caseTactSWErrorReTryTimer;

    unsigned char pureCounter;
    unsigned short waterPumpWaitTime;
    unsigned short iceMachineFanWaitTime;
    //unsigned char HBY_FFSet;
    unsigned short HBY_SetDecrease; //In ADC
    unsigned short HBY_CaseTempThermalCutOut;
    unsigned char boxFullActive;
    bool componentsTestDone;
    unsigned char QuickIce60;
    unsigned char IceComponentTest;             //APW : Enable component test            
    unsigned char LEDIceBlinkInCt_20ms;         //APW : Add Ice making operation test in CT
    unsigned char calibrate_IMch;
    unsigned int iceMachineLullabyCounter;
    bool SelectCalibrate_quickIce60;            //APW : Separate Calibrate function 
    signed short differential;
    unsigned char ConfirmIceNormalOrQuick60ModeTime;	//APW : For confirm Ice normal or Quick ice 
} stIceMachineValues_Ram;

typedef struct
{
    stTempSensor_Ram caseTempSensor;
    stRotator_Ram caseRotator;
    stSwitch_Ram caseTactSwitch;
    stFan_Ram fan;
    stSwitch_Ram boxTactSwitch;
} stIceMachineComponents_Ram;


typedef struct
{
    unsigned char iceOFF;
    unsigned char waterPumpTime;
    unsigned char QuickIce60;
} stIceMachineE2_Ram;


typedef struct
{
    stIceMachineSets sets;
    stIceMachineValues_Ram values;
    stIceMachineComponents_Ram components;
    stIceMachineE2_Ram E2;
} stIceMachine_Ram;
#endif
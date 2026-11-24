#ifndef CABIN_RAM_H
#define CABIN_RAM_H

#include "cabin_sets.h"
#include "states/cabin_states.h"
#include "app/refrigerator/components_ram.h"
#include "app/cooler/cooler_common.h"
#include "procedures/cabin_ai_cool_config.h"  // Only config macros, not function prototypes
#include "cooler/cooler_configs.h"

#define _DutyRaw_Size 5

typedef enum
{
  _VacuumSet_Off = 0,
  _VacuumSet_MinusOne,
  _VacuumSet_One,
}enVacuumBoxSets;

typedef enum
{
  _VacuumBox_Normal = 0,
  _VacuumBox_Lock,
  _VacuumBox_Leak,
}enVacuumBoxStatus;

typedef enum
{
    _DefrostInInteval_0 = 0,
    _DefrostInInteval_1,
    _DefrostInInteval_2,
    _DefrostInInteval_3,
    _DefrostInInteval_4,
    _DefrostInInteval_5,
} enVariableDefrostReason;

#define _Fan_ONDelayTime (5 * 50) // (sec*50) *20msec

typedef struct
{
    stFan_Ram fan;
    stFlap_Ram flap;
    stHeater_Ram heaterB;
    stHeater_Ram heaterA;
    stHeater_Ram heaterC;
    stIonizer_Ram ionizer;
    stLight_Ram innerLight;
    stLight_Ram blueLight;
    stHandleRGBLight_Ram handleLight;
    stSwitch_Ram doorSwitch[2];
    stSwitch_Ram humanSensor;
    stTempSensor_Ram vacuumBoxOverCurrentSensor;
    stTempSensor_Ram airTempSensor;
    stTempSensor_Ram evaTempSensor;
    stKnob_Ram knob;
    stVacuumBox_Ram VacuumBox;
    stHeater_Ram VacuumBoxHeater;
    stVioLed_Ram violed;
#ifdef ALPS_H50
    stSwitch_Ram doorAssistInputSwitch;   
    stTempSensor_Ram doorAssistMotorOverCurrentSensor; 
    stDoorAssistMotor_Ram doorAssistMotor;
    stDoorAssistOut_Ram doorAssistOut;
#endif   
} stCabinComponents_Ram;

typedef struct
{
    unsigned short Idle;
    unsigned short Cool;
    unsigned char PumpOut;
    // unsigned char Heat;
    // unsigned char PreCool;
    unsigned char Defrost;
    unsigned char Drainage;
    unsigned short HotStart;
    unsigned short PerformanceTest;
    // unsigned char FFNoCool;
    unsigned short CommercialTest;                 //APW : Add CT heater software
} stCabinStateTimers;


typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGBColor;

typedef struct
{
    enAmbientType ambientType;
    enHumidityType humidityType;

    stGas gasDesired;
    stGas gasApplied;

    unsigned char airSupply : 1;
    unsigned char quickIceCream : 1;
    unsigned char quickFRZ : 1;
    unsigned char quickFF : 1;
    unsigned char frzDefrost : 1;
    unsigned char quickDefrost : 1;
    unsigned char doorsClosedState : 1;
    // unsigned char errHighTemp:1;
    unsigned char errDefrostHeater : 1;
    unsigned char errSensors : 1;
    unsigned char quickBlock : 1;
    // unsigned char isHotStart:1;
    unsigned short defrostDelayTimerForIceCream;
    unsigned char defrostDelayForIceCream : 1;
    unsigned char quickIce;

    unsigned char cycleCounter;
    unsigned short workTimer;
    unsigned short realTimer;
    unsigned short defrostHighTempTimer;
    unsigned short activeDefrostHighTempTimer;
    unsigned short activeRealTimer;
    signed short activeRealTimerDiff;
    unsigned short highTempChckTimer;
    unsigned char highTempStayTimer;

    unsigned short quickTimer;
    unsigned short flapTimer;

    unsigned char errDefrostHeaterCounter;

    unsigned char fanLevel;
    unsigned char fanLevelStayTimer;
    unsigned char fanLevelBlockReduction : 1; // Bool...

    struct
    {
        unsigned char mean;
        unsigned char delay;
        unsigned char index;
        unsigned short meanCoolTime;
        unsigned short meanIdleTime;
        unsigned short coolTime[_DutyRaw_Size];
        unsigned short idleTime[_DutyRaw_Size];
    } duty;

    struct
    {
        unsigned short meanTime;
        unsigned char index;
        unsigned short rawTime[_DutyRaw_Size];
        unsigned short variableTime;
        enVariableDefrostReason variableReason;
        unsigned char delayTimer;
        unsigned char preCoolActive;
        unsigned char afterDefrostActive;
        unsigned char recoveryModeActive;
        unsigned char defrostHighTempActive;
    } defrost;

    struct
    {
        unsigned short offTimer;
        unsigned short onTimer;
        enum
        {
            _IonState_OFF = 0,
            _IonState_ON,
            _IonState_Delay,
        } state;
    } ionizer;

    struct
    {
        unsigned short offTimer;
        unsigned short onTimer;
        unsigned short delayTimer;
        unsigned short HeaterTimer;
        unsigned char airTempState;
        enum
        {
            _VacuumState_OFF = 0,
            _VacuumState_ON,
            _VacuumState_Delay,
            _VacuumState_Prohibited,
        } state;
        enVacuumBoxSets sets;
        enVacuumBoxStatus status;
    } VacuumBox;

    struct
    {
        unsigned short onVTimer;
        unsigned short offVTimer;
        unsigned char maxDelayVTimer;
        bool doorClosedModeFinish;
        enum
        {
            _VioLedState_OFF = 0,
            _VioLedState_ON,
            _VioLedState_Delay,
            _VioLedState_Active,
            _VioLedState_Deactive,
        } state;

    } violed;

    struct
    {
        unsigned char state : 1;
        unsigned char alert : 1;
        unsigned char sync : 1;
        unsigned char close : 1;
        unsigned char timer;
    } innerLight;


    unsigned char selectedColorIndex ;
    unsigned char selectedColorIndexSetTime ;
    unsigned char SelectedColorIndexAtCTMode;
    unsigned char SelectedColorIndexAtCTModeTimerChange;

    unsigned short heaterATimer;
    unsigned short heaterBTimer;
    unsigned short heaterCTimer;
    unsigned char doorClosedTimer;
    unsigned char doorOpenedTimer;
    unsigned char doorClosedDelay;
    unsigned short defrostDoorOpenedTimer;
    unsigned short doorsOpenedLevelIncreaseTimer;
    unsigned short afterDefrostTimer;
    
#ifdef ALPS_H50
    struct
    {    
        unsigned char request;
        unsigned char initRequest;
        unsigned char operationFlag;
        unsigned short procTimer;
        unsigned char delayTimer;
        unsigned char proc;
        unsigned char initProc;
//        signed short loadCurrent;
//        signed short loadCurrent2;
    } doorAssist;
#endif
    struct
    {
        unsigned char evaChecked : 1;
        unsigned char airChecked : 1;
        unsigned char allChecked : 1;
        unsigned char Fail : 1;
        signed short airTempAtStart;
    } performance;

    struct
    {
        unsigned char isactive;
        unsigned char activationTimeResetF;
        unsigned char FirstCutoutDoneF;
        unsigned char PowerUpCond;
        unsigned char defrostBlocked;
        unsigned char erasePowerUpTime;
        unsigned short activationTimer;
        unsigned short algorithmActivationTime;
        unsigned short delayTime;
        unsigned char activationSource;
        unsigned char highTempStatus;
        signed short maxFRZTempInCext;
        // signed short qFRZ
    } HighTemErr;
      signed short FanPreCoolOffset;
      signed short CoolPreCoolOffset;
    bool firstDefrostFlag;
    bool finishDefrostFlag;
    bool componentsTestDone;
    signed short ValveEvaTempCheckCommercial[6];                                //APW : Add valve record temp commercial mode
    unsigned short FlapWaitingTimeCommercial_sec;                               //APW : Add valve record temp commercial mode
} stCabinValues;

typedef struct
{
    enCabinModes Mode;
    unsigned char FF;
    unsigned char FRZ;
    unsigned char Convertible;
    unsigned char Vacation;
    unsigned char Rice;
    unsigned char TempSettingFine;
    // unsigned char errHighTemp;
    unsigned char errDefrostHeaterCounter;
    unsigned char highTempStatus;
    unsigned char HighTempIsactive;
    unsigned char FirstCutoutDoneF;
    unsigned short workTimer;
    unsigned short realTimer;
    unsigned short meanCoolTime;
    unsigned short meanIdleTime;
    signed short maxFRZTempInCext;
    unsigned short AlgorithmActivationTime;
    unsigned char selectedColorIndex;
    enVacuumBoxSets VacuumSet;
    enVacuumBoxStatus VacuumStatus;
} stCabinE2_Ram;

#if defined (AI_COOL_V1)
typedef enum
{
  _AI_LABEL_EFFICIENT= 0,
  _AI_LABEL_MODERATE,
  _AI_LABEL_POWER,
}enLabelAI;

typedef struct
{
  enLabelAI array[NUMBER_OF_AI_ANALYZE_DAYS][NUMBER_OF_AI_ANALYZE_HOURS];
  enLabelAI decisionArray[NUMBER_OF_AI_ANALYZE_HOURS];
  unsigned char indexDay:3;
  unsigned char indexHour:5;
}stLabelAI;

typedef struct
{
  unsigned char maxVal;
  unsigned char workTimer;
  unsigned char firstSpeedApplied:1;
  unsigned char speedChangeTimer;
  unsigned char cmpVal[_AmbientSize][COMPRESSOR_CMP_VAL_SIZE];
}stCompressorAI;

typedef struct
{
  stLabelAI label;
  stCompressorAI compressor;
  unsigned char isEnabled:1;
  unsigned char isActivated:1;
  unsigned char timer1hour:6;
}stCabinAI;
#endif // AI COOL

typedef struct
{
    stCabinComponents_Ram components;
    stCabinSets_Ram sets;
    stCabinStates states;
    stCabinStateTimers stateTimers;
    stCabinValues values;
    stCabinE2_Ram E2;
#if defined (AI_COOL_V1)
    stCabinAI ai;
#endif
} stCabin_Ram;

#endif

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "board/board.h"
#include "dev/fan.h"
#include "dev/stepper_motor.h"

typedef enum
{
    _CompressorType_NONE = eDeviceTypeNone,
    _CompressorType_ACOut = eDeviceTypeACOutputComp,
    _CompressorType_VCC_PANASONIC = eDeviceTypeSoftFreq,
    _CompressorType_VCC_EMBRACO_VEMX = eDeviceTypeSoftFreq,
    _CompressorType_VCC_EKI = eDeviceTypeSoftFreq,
    _CompressorType_UART = eDeviceTypeComm,
} enCompressorTypes;

typedef struct
{
    unsigned short startTime;
    unsigned short endTime;
} tsCompTestData;

typedef struct
{
    enCompressorTypes type;
    unsigned char index;
    tsCompTestData test;
} stCompressor_Flash;

typedef enum
{
    _FanType_NONE = eDeviceTypeNone,            // 0
    _FanType_AC = eDeviceTypeACOutputDelayable, // 1
    _FanType_CDC = eDeviceTypeFan,              // 2
    _FanType_VDC = eDeviceTypeFan,              // 3
    _FanType_DC = eDeviceTypeDigOutput,         // 4
} enFanTypes;

typedef struct
{
    enFanTypes type;
    unsigned char index;
    tsFanConfig configs;
    tsCompTestData test;
    unsigned int maxPower;
} stFan_Flash;

typedef enum
{
    _TempSensorType_NONE = eDeviceTypeNone,
    _TempSensorType_NTCwithADC = eDeviceTypeNTC,
    _TempSensorType_NTC_R_air_ADC = eDeviceTypeNTC_R_air,
    _TempSensorType_NTC_R_eva_ADC = eDeviceTypeNTC_R_eva,
    _TempSensorType_NTC_F_air_ADC = eDeviceTypeNTC_F_air,
    _TempSensorType_NTC_F_eva_ADC = eDeviceTypeNTC_F_eva,
    _TempSensorType_NTC_IM_case_ADC = eDeviceTypeNTC_IM_case,
    _TempSensorType_NTC_Ambient_ADC = eDeviceTypeNTC_Ambient,
    _TempSensorType_SHT3X = eDeviceTypeSHT3XTemp,
} enTempSensorTypes;

typedef struct
{
    enTempSensorTypes type;
    unsigned char index : 5;
    unsigned char hideError : 1;
    unsigned char order;
} stTempSensor_Flash;

typedef enum
{
    _HumSensorType_NONE = eDeviceTypeNone,
    _HumSensorType_SHT3X = eDeviceTypeSHT3XHumidity,
    _HumSensorType_ADC = eDeviceTypeNTC_Humidity,
} enHumSensorTypes;

typedef struct
{
    enHumSensorTypes type;
    unsigned char index;
} stHumiditySensor_Flash;


typedef enum
{
    _SolenoidValveType_NONE = eDeviceTypeNone,
    _SolenoidValveType_StepMotor = eDeviceTypeStepperMotor,
    _SolenoidValveType_AC = eDeviceTypeACOutputDelayable,
} enSolenoidValveTypes;

typedef struct
{

    enSolenoidValveTypes type;
    unsigned char index : 5;
    unsigned char positionsSize;
    unsigned char positions[5];
    tsCompTestData test;
    tsStepperMotorConfig stepperConfigs;
} stSolenoidValve_Flash;

typedef enum
{
    _FlapType_NONE = eDeviceTypeNone,
    _FlapType_StepMotor = eDeviceTypeStepperMotor,
} enFlapTypes;

typedef struct
{
    enFlapTypes type;
    unsigned char index : 5;
    tsStepperMotorConfig stepperConfigs;
} stFlap_Flash;

typedef enum
{
    _HeaterType_NONE = eDeviceTypeNone,
    _HeaterType_AC = eDeviceTypeACOutputDelayable,
} enHeaterTypes;

typedef struct
{
    enHeaterTypes type;
    unsigned char index : 5;
    tsCompTestData test;
    unsigned char commercialDefrostTestPriority;             //APW : Add CT heater software
    unsigned short resistanceForPower;
} stHeater_Flash;

typedef enum
{
    _IonizerType_NONE = eDeviceTypeNone,
    _IonizerType_AC = eDeviceTypeACOutputDelayable,
} enIonizerTypes;

typedef struct
{
    enIonizerTypes type : 3;
    unsigned char index : 5;
    tsCompTestData test;
    unsigned char maxPower;
} stIonizer_Flash;

typedef enum
{
    _LightType_NONE = eDeviceTypeNone,
    _LightType_AC = eDeviceTypeACOutputNotDelayable,
    _LightType_DC = eDeviceTypeDigOutput,
    _LightType_PWM = eDeviceTypeFadingLight,
} enLightTypes;

typedef struct
{
    enLightTypes type;
    unsigned char index : 5;
    // bool pwmDisabled;
    tsCompTestData test;
    unsigned int maxPower;
} stLight_Flash;
    
typedef struct
{
    enLightTypes type;
    unsigned char index_r : 5;
    unsigned char index_g : 5;
    unsigned char index_b : 5;
    // bool pwmDisabled;
    tsCompTestData test;
    unsigned int maxPower;
} stHandleRGBLight_Flash;    

/*** APW : Add CT heater software ***/
typedef enum
{
    _ARMType_NONE = eDeviceTypeNone,
    _ARMType_AC = eDeviceTypeACOutputNotDelayable,
    _ARMType_DC = eDeviceTypeDigOutput,
    _ARMType_PWM = eDeviceTypeFadingLight,
} enARMTypes;

typedef struct
{
    enARMTypes type;
    unsigned char index : 5;
    // bool pwmDisabled;
    tsCompTestData test;
} stARM_Flash;
/*** APW : Add CT heater software ***/

typedef enum
{
    _SwitchType_NONE = eDeviceTypeNone,
    _SwitchType_AC = eDeviceTypeFilteredReversedDigInput,
    _SwitchType_AC_Reversed = eDeviceTypeFilteredDigInput,
    _SwitchType_DC = eDeviceTypeFilteredDigInput,
    _SwitchType_DC_Reversed = eDeviceTypeFilteredReversedDigInput,
} enSwitchTypes;

typedef struct
{
    enSwitchTypes type;
    unsigned char index : 5;
} stSwitch_Flash;

typedef enum
{
    _VioLedType_NONE = eDeviceTypeNone,
    _VioLedType_DC = eDeviceTypeDigOutput,
    _VioledType_PWM = eDeviceTypeDigOutput, // in UxPure PWM implemented to set duty to 0 or 100 only, so it's like digital output
} enVioLedTypes;

typedef struct
{
    enVioLedTypes type;
    unsigned char index : 5;
    unsigned char activateOnDisplay : 1;
    tsCompTestData test;
    unsigned char maxPower;
} stVioLed_Flash;

typedef enum
{
    _RotatorType_NONE = eDeviceTypeNone,
    _RotatorType_BDCMotor = eDeviceTypeDCMotor,
} enRotatorTypes;

typedef struct
{
    enRotatorTypes type;
    unsigned char index : 5;
    struct
    {
        tsCompTestData cw;
        tsCompTestData ccw;
    } test;
} stRotator_Flash;

typedef enum
{
    _PumpType_NONE = eDeviceTypeNone,
    _PumpType_BDCMotor = eDeviceTypeDCMotor,
} enPumpTypes;
typedef enum
{
    _VacuumBoxType_NONE = eDeviceTypeNone,
    _VacuumBoxType_BDCMotor = eDeviceTypeDigOutput,
} enVacuumBoxTypes;

typedef struct
{
    enPumpTypes type;
    unsigned char index : 5;
    struct
    {
        tsCompTestData pump;
        tsCompTestData vacuum;
    } test;
} stPump_Flash;

typedef enum
{
    _ValveType_NONE = eDeviceTypeNone,
    _ValveType_ACOut = eDeviceTypeACOutputNotDelayable,
} enValveTypes;

typedef struct
{
    enValveTypes type;
    unsigned char index : 5;
    tsCompTestData test;
} stValve_Flash;

typedef enum
{
    _SelectorType_NONE = eDeviceTypeNone,
    _SelectorType_BDCMotor = eDeviceTypeDCMotor,
    _SelectorType_ACMotor = eDeviceTypeACOutputDelayable,
} enSelectorTypes;

typedef struct
{
    enSelectorTypes type;
    unsigned char index : 5;
    struct
    {
        tsCompTestData selA;
        tsCompTestData selB;
    } test;
} stSelector_Flash;

typedef enum
{
    _ShutterType_NONE = eDeviceTypeNone,
    _ShutterType_BDCMotor = eDeviceTypeDCMotor,
    _ShutterType_ACMotor = eDeviceTypeACOutputDelayable,
} enShutterTypes;

typedef struct
{
    enShutterTypes type;
    unsigned char index : 5;
    struct
    {
        tsCompTestData open;
        tsCompTestData close;
    } test;
} stShutter_Flash;

typedef enum
{
    _PusherType_NONE = eDeviceTypeNone,
    _PusherType_ACOut = eDeviceTypeACOutputDelayable,
} enPusherTypes;

typedef struct
{
    enPusherTypes type;
    unsigned char index : 5;
    tsCompTestData test;
} stPusher_Flash;

typedef enum
{
    _MixerType_NONE = eDeviceTypeNone,
    _MixerType_AC = eDeviceTypeACOutputDelayable,
} enMixerTypes;

typedef struct
{
    enMixerTypes type;
    unsigned char index : 5;
    tsCompTestData test;
} stMixer_Flash;

typedef enum
{
    _KnobType_NONE = eDeviceTypeNone,
    // TODO add knob implementation
    _KnobType_ADC_6Position = eDeviceTypeKnob,
} enKnobTypes;

typedef struct
{
    enKnobTypes type;
    unsigned char index : 5;
} stKnob_Flash;

typedef struct
{
    enVacuumBoxTypes type;
    unsigned char index : 5;
    struct
    {
        tsCompTestData pump;
        tsCompTestData vacuum;
    } test;
} stVacuumBox_Flash;


#endif
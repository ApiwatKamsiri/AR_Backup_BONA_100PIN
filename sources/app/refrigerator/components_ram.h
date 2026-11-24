#ifndef COMPONENTS_RAM_H
#define COMPONENTS_RAM_H

#include "dev/ntc.h"

typedef struct
{
  unsigned short No[3];
  unsigned char Version;
  unsigned char Revision;
  unsigned char CoreVersion;
  unsigned char CoreRevision;
} stCompressorStock_Ram;

typedef struct{
  stCompressorStock_Ram Stock;                                                  // APW add stock of compressor
  unsigned char value;
  unsigned char Readvalue;                                                      // APW add read speed compressor
  unsigned char ProductionTestDelayF:1;
  unsigned char CompressorHotstartF;
}stCompressor_Ram;

typedef struct
{
    unsigned short speedOrPWM;
    //unsigned int  calcSpeed:;
    unsigned short error;
} stFan_Ram;

typedef struct
{
    //signed int   valueInCExt;
    //signed char  valueInC;
    signed short valueInCExt;
    teNtcErrorType error;
} stTempSensor_Ram;

typedef struct
{
    unsigned char humidityValue;
    unsigned char humidityValueCal;
    unsigned char error;
} stHumiditySensor_Ram;

typedef struct
{
    unsigned char positionIndex;
    unsigned char testMode : 1;
    unsigned short position;
} stSolenoidValve_Ram;

typedef struct
{
    unsigned short value;
    unsigned short flapFeed;	//APW : add flap feedback for check position damper
} stFlap_Ram;


typedef enum{
  _HeaterValue_OFF = 0,   
  _HeaterValue_ON,
}enHeaterValues;

typedef struct{
  enHeaterValues value;
}stHeater_Ram;

typedef enum{
  _IonizerValue_OFF = 0,   
  _IonizerValue_ON,
}enIonizerValues;

typedef struct{
  enIonizerValues value;
}stIonizer_Ram;

typedef struct
{
    unsigned short value;
//    unsigned int step; // fixed in creation macro
} stLight_Ram;
    
typedef struct
{
    unsigned short R_value,G_value,B_value;
    unsigned short R_value_target,G_value_target,B_value_target;
//    unsigned int step; // fixed in creation macro
} stHandleRGBLight_Ram;


typedef enum
{
    _SwitchValue_CLOSE = 0,
    _SwitchValue_OPEN,
} enSwitchValues;

typedef struct
{
    enSwitchValues value;
} stSwitch_Ram;

/*** APW : Add CT heater software ***/
typedef struct
{
    enSwitchValues value;
} stARM_Ram;
/*** APW : Add CT heater software ***/

typedef enum
{
    _VioLedValue_OFF = 0,
    _VioLedValue_ON,
} enVioLedValues;

typedef struct
{
    /*enVioLedValues*/ unsigned short value; // ozgur
} stVioLed_Ram;

typedef enum{
  _RotatorValue_OFF = 0,
  _RotatorValue_CW,
  _RotatorValue_CCW,
}enRotatorValues;

typedef struct{
  enRotatorValues value;
}stRotator_Ram;


typedef enum{
  _PumpValue_OFF,
  _PumpValue_PUMP,
  _PumpValue_VACUM,
}enPumpValues;

typedef struct{
  enPumpValues value;
}stPump_Ram;

typedef enum{
  _VacuumValue_OFF,
  _VacuumValue_ON,
}enVacuumBoxValues;

typedef struct{
  enVacuumBoxValues value;
}stVacuumBox_Ram;

#ifdef ALPS_H50
typedef enum{
  _DoorAssistMotor_STOP,
  _DoorAssistMotor_FWD,
  _DoorAssistMotor_RVS,
}enDoorAssistMotorValues;

typedef struct{
  enDoorAssistMotorValues value;
}stDoorAssistMotor_Ram;

typedef enum{
  _DoorAssistOut_OFF,
  _DoorAssistOut_ON,
}enDoorAssistOutValues;

typedef struct{
  enDoorAssistOutValues value;
  unsigned short speedOrPWM;
}stDoorAssistOut_Ram;
#endif

typedef enum
{
    _ValveValue_CLOSE = 0,
    _ValveValue_OPEN,
} enValveValues;

typedef struct
{
    enValveValues value;
} stValve_Ram;


typedef enum
{
    _SelectorValue_OFF = 0,
    _SelectorValue_SELECTING_A,
    _SelectorValue_SELECTING_B,
} enSelectorValues;

typedef struct
{
    enSelectorValues value;
} stSelector_Ram;

typedef enum
{
    _ShutterValue_OFF = 0,
    _ShutterValue_OPENING,
    _ShutterValue_CLOSING,
} enShutterValues;

typedef struct
{
    enShutterValues value;
    bool beforeOpening;
} stShutter_Ram;

typedef enum
{
    _PusherValue_OFF = 0,
    _PusherValue_PUSH,
} enPusherValues;

typedef struct
{
    enPusherValues value;
} stPusher_Ram;

typedef enum
{
    _MixerValue_OFF = 0,
    _MixerValue_MIX,
} enMixerValues;

typedef struct
{
    enMixerValues value;
} stMixer_Ram;

typedef struct
{
    unsigned char position;
    unsigned short error;
    unsigned char positionChanged;
} stKnob_Ram;


#endif
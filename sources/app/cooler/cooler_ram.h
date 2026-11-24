#ifndef COOLER_RAM_H
#define COOLER_RAM_H

#include "app/refrigerator/components_ram.h"
#include "cooler_common.h"



typedef struct{
  unsigned char silent:1;
  unsigned char gasLevelBlockReduction:1;
  unsigned char condenserFanSpeedChecked:1;
  unsigned char condenserFanSpeedChanged:1;
  unsigned char queue:1;
  unsigned char queueState;
  
  stGas gasDesired[5];
  stGas gasApplied[5];
  stGas gasAppliedPrev[5];
  
  unsigned char gasType;
  unsigned char state;
  unsigned char priority;
 
  unsigned char gasLevel;
  unsigned short gasLevelStayTimer;
  
  unsigned short gasLevelStayTimerFF;
  unsigned short gasLevelStayTimerFRZ;
  
  unsigned short humidityCheckTimer;
  
  enAmbientType ambientType;
  enHumidityType humidityType;
  
  unsigned short compressorOnTimer;
  unsigned short compressorOffTimer;
#if defined(AI_COOL_V1) || defined(AI_COOL_V1_5)
  unsigned short compressorOnTimerPrev;
#endif
  unsigned short ValveAndCompressorCommercialOnTimer;                           //APW : Add CT heater software
  
  unsigned char maxLevelCompressorOnTimer;
  unsigned char maxLevelCompressorOnTimerPrev;
  
  unsigned short solenoidValvePositionTimer;
  unsigned short valveStateStayTimer;
  unsigned char valvePositionChanged;
  unsigned char valvePrevState;
  bool valveDefaultPosition;
  unsigned char valvePrevPosition;
  bool frzDefrost;  // Used for bug fix. Ask Salih Ozmen for details
  bool componentsTestDone;
}stCoolerParameters_Ram;

typedef struct{
  stCompressor_Ram compressor;
  stFan_Ram condenserFan;  
  stTempSensor_Ram ambientTempSensor;
  stHumiditySensor_Ram humiditySensor;
  stSolenoidValve_Ram solenoidValve[2]; 
  stSwitch_Ram testSwitch[2];               //APW : Add CT heater software
  stARM_Ram CommercialARM;                  //APW : Add CT heater software
}stCoolerComponents_Ram;

typedef struct{
  stCoolerParameters_Ram parameters;
  stCoolerComponents_Ram components;
  
}stCooler_Ram;

#endif

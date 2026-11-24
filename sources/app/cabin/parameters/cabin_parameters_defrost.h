#ifndef _CabinParameters_Defrost_H
#define _CabinParameters_Defrost_H

#define _IntervalSize 5

typedef enum{
  _DefrostType_Disabled,
  _DefrostType_Normal,
  _DefrostType_HotGas,
  _DefrostType_Variable,
}enDefrostTypes;

typedef struct{ 
  enDefrostTypes Type;
  unsigned short MaxWorkTime_DO;
  unsigned short MaxWorkTime_DC;
  unsigned short MaxRealTime_DO;
  unsigned short MaxRealTime_DC;
  unsigned short MaxRealTime_QuickFRZ;
  unsigned short MaxQuickTime;  
  unsigned short MaxDefrostHighTempTime;
  unsigned char MaxDefrostHighTempShiftCutInTemp;
  unsigned char MaxDefrostHighTempShiftCutOutTemp;
  unsigned short DefrostHighTemp_Activation;
  unsigned char MaxCycle_DO;
  unsigned char MaxCycle_DC;
  signed short  DefrostCutOutTemp;
  unsigned char MinNoCoolTime;
  unsigned char MaxNoCoolTime;  
  unsigned char MaxDefrostTime_DO;
  unsigned char MaxDefrostTime_DC;
  unsigned char ErrorMaxDefrostTime;
  unsigned short  RealTimeAfterHotStart;
  unsigned short  RealTimeAfterService;
  unsigned char ErrorMaxCycle; 
  unsigned char ErrorMaxNoCoolTime;
  unsigned short DoorOpenedIntervals[ _IntervalSize ];
  unsigned short VariableMaxRealTime[_AmbientSize][ _IntervalSize + 1 ];
  signed short VariableDefrostCutOutTemp[_AmbientSize][ _IntervalSize + 1 ];
#if defined (AI_COOL_V1)
  unsigned short DoorOpenedIntervals_AI[ _IntervalSize ];
  unsigned short VariableMaxRealTime_AI[_AmbientSize][ _IntervalSize + 1 ];
  signed int VariableDefrostCutOutTemp_AI[_AmbientSize][ _IntervalSize + 1 ];
#endif
  unsigned char DelayTime;
  unsigned short AfterDefrostTime;
  unsigned char RecoveryModeActive;
}stDefrostParameters;

#endif

#ifndef _CabinParameters_Fan_H
#define _CabinParameters_Fan_H

#define _ambientLevel 4
#define _LevelSize 6

typedef struct{  
  unsigned char SpeedLevel[ 9 ];
  unsigned char AmbientSpeedLevel[_AmbientSize][_LevelSize];
  bool CompressorOFF[_AmbientSize];
  bool CompressorON_FRZ[_AmbientSize];
  bool CompressorON_FF[_AmbientSize];
  unsigned char SpeedQuick;
  unsigned char SpeedError;
  unsigned char SpeedSilent;
  unsigned char SpeedHotStart;
  unsigned char SpeedPumpOut;
  unsigned char SpeedConvertible;
  unsigned char SpeedIdle;
  
  unsigned char MinDuty_DO;
  unsigned char MaxDuty_DO;
  unsigned char MinDuty_DC;
  unsigned char MaxDuty_DC;
  unsigned char FF_Fan_MinDutyDC_FFDefrost;
  unsigned char FF_Fan_MaxDutyDC_FFDefrost;
  signed short    CutInEvaTemp;
  unsigned char CutInTime;
  unsigned char AirSupplier;
  unsigned char CutOutHysteresis;
  unsigned char WorkTimeAfterCool;
  unsigned char HotStartDelayTime;
  
  unsigned char minStayTimeAtPresFanLevel;
  unsigned char maxStayTimeAtPresFanLevel;
  signed short Offsets[_AmbientSize];
  signed short PreCoolOffsets[_AmbientSize];
  unsigned char secOnTimeFlapFanCommercial;                                     //APW : Add CT heater software
  unsigned char secOnTimeFfFanCommercial;                                       //APW : Add CT heater software
  unsigned char secOnTimeFrzFanCommercial;                                      //APW : Add CT heater software
}stFanParameters;

#endif

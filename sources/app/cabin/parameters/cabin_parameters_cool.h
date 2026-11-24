#ifndef _CabinParameters_Cool_H
#define _CabinParameters_Cool_H

typedef enum{
  _CoolType_Disabled,
  _CoolType_Normal,
}enCoolTypes;

typedef struct{
  enCoolTypes Type;
  unsigned char SabbathMaxIdleTime;
  unsigned short MaxIdleTime;
  unsigned char ErrorMaxIdleTime;
  unsigned char ErrorMaxCoolTime;  
  signed short  ErrorCutInTemp;
  signed short  ErrorCutOutTemp;
  signed short  QuickFRZCutOutTemp;
  unsigned char MaxIdleTimeElapsedCoolTime;
  unsigned char isEvaCutInActive;
  unsigned char EvaCutInTemp;
  unsigned char pumpOutTime;
  unsigned short HotStartTime;
  unsigned short HotStart2Time;
  unsigned short PerformanceTestTime;
  unsigned short PerformanceTest2Time;
  signed short Offsets[_AmbientSize];
  signed short OffsetsShiftCutIn[_AmbientSize];
  signed short OffsetsShiftCutOut[_AmbientSize];
  signed short PreCoolOffsets[_AmbientSize];
#if defined (AI_COOL_V1)
  signed short CabinSetsAmbientOffSetAI[_AmbientSize];
  signed short FanSetsAmbientOffSetAI[_AmbientSize];
#endif
  signed short AfterDefrostOffsets[_AmbientSize];
}stCoolParameters;

#endif

#ifndef _CabinParameters_HeaterA_H
#define _CabinParameters_HeaterA_H

typedef struct{
  unsigned char MaxNoCoolTime_DO;
  unsigned char MaxNoCoolTime_DC;
  unsigned short OnTimeA;
  unsigned short OffTimeA;
  unsigned short OnTimeB;
  unsigned short OffTimeB;
    struct {
    unsigned char OnTime[_HumiditySize][_AmbientSize];
    unsigned char OffTime[_HumiditySize][_AmbientSize];
  }humidityHeater;
  signed short LTHeaterAmbientOffSet[_AmbientSize];
}stHeaterAParameters;

#endif

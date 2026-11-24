#ifndef _CabinParameters_HeaterC_H
#define _CabinParameters_HeaterC_H

//#include "HumiditySensor.h"
//#include "Cooler.h"

#define _HumiditySize 10

typedef struct{
  unsigned short OnTimeA;
  unsigned short OffTimeA;
  unsigned short OnTimeB;
  unsigned short OffTimeB;
  struct {
    unsigned char OnTime[_HumiditySize][_AmbientSize];
    unsigned char OffTime[_HumiditySize][_AmbientSize];
  }humidityHeater;
}stHeaterCParameters;

#endif

#ifndef _CabinParameters_VioLed_H
#define _CabinParameters_VioLed_H

typedef struct{
  unsigned short maxOnTime;
  unsigned short maxOffTime;
  unsigned short maxDisplayActivation;
  unsigned char maxDelayTime;
  unsigned short maxOffTimeFirstStart;
}stVioLedParameters;

#endif

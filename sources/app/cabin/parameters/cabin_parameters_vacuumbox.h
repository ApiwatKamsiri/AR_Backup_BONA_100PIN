#ifndef _CabinParameters_VacuumBox_Heater_H
#define _CabinParameters_VacuumBox_Heater_H

//#include "HumiditySensor.h"
//#include "Cooler.h"

typedef struct{
  unsigned short VacuumBoxFirstOFFTime;
  unsigned short VacuumBox_OFFTime;
  unsigned short VacuumBox_ONTime1;
  unsigned short VacuumBox_ONTime2;
  unsigned short VacuumBox_ONDelayTime;
  struct{
    unsigned short OnTime[_AmbientSize][5];
    unsigned short OffTime[_AmbientSize][5];
    signed short TemperaturesZone[5];
  }VacuumBoxHeater;
  struct
  {
    signed short JudgmentLockA;
    signed short JudgmentLockB;
    signed short JudgmentStopA;
    signed short JudgmentStopB;
    signed short JudgmentLeak;
  } VacuumBoxCurrentSensor;
}stVacuumBoxParameters;



#endif

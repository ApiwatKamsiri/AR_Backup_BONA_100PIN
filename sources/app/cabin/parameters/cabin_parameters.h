#ifndef _Cabin_Parameters_H
#define _Cabin_Parameters_H

#include "cabin_parameters_cool.h"
#include "cabin_parameters_defrost.h"
#include "cabin_parameters_drainage.h"
#include "cabin_parameters_fan.h"
#include "cabin_parameters_flap.h"
#include "cabin_parameters_a_heater.h"
#include "cabin_parameters_b_heater.h"
#include "cabin_parameters_c_heater.h"
#include "cabin_parameters_vacuumbox.h"
#include "cabin_parameters_performance.h"
#include "cabin_parameters_quick.h"
#include "cabin_parameters_level.h"
#include "cabin_parameters_violed.h"

typedef struct{
  signed short HighTempErrCutIN;
  signed short HighTempErrCutOUT;
  
  unsigned char HighTmp_Time_PowerUp;
  unsigned short HighTmp_Time_Defrost;
  unsigned short HighTmp_Time_Normal;
  unsigned short HighTmp_Algorithm_Activation;
  unsigned short HighTmp_DelayAlarm;
}stHighTempErrorParams;

typedef struct{
  unsigned char permannentHighTempError:1;
  stPerformanceParameters Performance;
  stQuickParameters Quick;
  stCoolParameters Cool;
  stDefrostParameters Defrost;
  stDrainageParameters Drainage;
  stFanParameters Fan;
  stFlapParameters Flap; 
  stHeaterAParameters HeaterA;
  stHeaterBParameters HeaterB;
  stHeaterCParameters HeaterC;
  stVacuumBoxParameters VacuumBox;
  stLevelParameters Level;
  stAmbientLevelParameters AmbientLevel;
  stHighTempErrorParams HighTempError;
  stVioLedParameters VioLed;
  unsigned char TempJudgmentValveFfCommercial;                                  //APW : Add CT heater software
  unsigned char TempJudgmentValveFrzCommercial;                                 //APW : Add CT heater software
}stCabinParameters;

#endif

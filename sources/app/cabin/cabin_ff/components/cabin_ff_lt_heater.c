#include "cabin_ff_lt_heater.h"

enHeaterAStates cabinFfLTHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;
  UNUSED(cabinC);

  switch (cabinP->states.Sensor)
  {
  case _Sensor_AirP:
  case _Sensor_AirF:
  case _Sensor_EvaP:
  case _Sensor_EvaF:
  case _Sensor_AirF | _Sensor_EvaP:
  case _Sensor_AirF | _Sensor_EvaF:
    return _HeaterAOff_FFLowTemp_Always;
    // break; if statement is not reachable comment out;
  case _Sensor_AirP | _Sensor_EvaP:
  case _Sensor_AirP | _Sensor_EvaF:
    if (cabinP->components.airTempSensor.valueInCExt >=
        cabinP->sets.datas.lowTempHeaterCutOutTemp)
      return _HeaterAOff_FFLowTemp_AirHigherThanLTHCutOut;

    if (cabinP->components.airTempSensor.valueInCExt <=
        cabinP->sets.datas.lowTempHeaterCutInTemp)
      return _HeaterAOn_FFLowTemp_AirLowerThanLTHCutIn;
    break;
  }

  return _HeaterANone_FFLowTemp;
}

enHeaterAStates cabinFfLTHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;

  switch (cabinP->states.Sensor)
  {
  case _Sensor_AirP:
  case _Sensor_EvaP:
    if (cabinP->values.doorsClosedState)
    {
      if ((cabinP->stateTimers.Idle + cabinP->stateTimers.Defrost) >=
          cabinC->parameters.HeaterA.MaxNoCoolTime_DC)
        return _HeaterAOn_FFLowTemp_MaxNoCoolTimeElapsed;
      else
        return _HeaterAOff_FFLowTemp_MaxNoCoolTimeNotElapsed;
    }
    else
    {
      if ((cabinP->stateTimers.Idle + cabinP->stateTimers.Defrost) >=
          cabinC->parameters.HeaterA.MaxNoCoolTime_DO)
        return _HeaterAOn_FFLowTemp_MaxNoCoolTimeElapsed;
      else
        return _HeaterAOff_FFLowTemp_MaxNoCoolTimeNotElapsed;
    }
    // break; if statement is not reachable comment out;
  case _Sensor_AirF:
  case _Sensor_EvaF:
  case _Sensor_AirF | _Sensor_EvaP:
  case _Sensor_AirF | _Sensor_EvaF:
    return _HeaterAOff_FFLowTemp_Always;
    // break; if statement is not reachable comment out;
  case _Sensor_AirP | _Sensor_EvaP:
  case _Sensor_AirP | _Sensor_EvaF:
    if (cabinP->values.frzDefrost)
    {
      return _HeaterAOn_FFLowTemp_ThereIsAFRZDefrost;
    }
    else
    {
      if (cabinP->components.airTempSensor.valueInCExt >=
          cabinP->sets.datas.lowTempHeaterCutOutTemp)
        return _HeaterAOff_FFLowTemp_AirHigherThanLTHCutOut;

      if (cabinP->components.airTempSensor.valueInCExt <=
          cabinP->sets.datas.lowTempHeaterCutInTemp)
        return _HeaterAOn_JKRLowTemp_AirLowerThanLTHCutIn;
    }
    break;
  }
  return _HeaterANone_FFLowTemp;
}

enHeaterAStates cabinFfLTHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;

  switch (cabinP->states.Sensor)
  {
  case _Sensor_AirP:
  case _Sensor_EvaP:
    if (cabinP->values.doorsClosedState)
    {
      if ((cabinP->stateTimers.Idle + cabinP->stateTimers.Defrost) >
          cabinC->parameters.HeaterA.MaxNoCoolTime_DC)
        return _HeaterAOn_FFLowTemp_MaxNoCoolTimeElapsed;
      else
        return _HeaterAOff_FFLowTemp_MaxNoCoolTimeNotElapsed;
    }
    else
    {
      if ((cabinP->stateTimers.Idle + cabinP->stateTimers.Defrost) >
          cabinC->parameters.HeaterA.MaxNoCoolTime_DO)
        return _HeaterAOn_FFLowTemp_MaxNoCoolTimeElapsed;
      else
        return _HeaterAOff_FFLowTemp_MaxNoCoolTimeNotElapsed;
    }
    // break; if statement is not reachable comment out;
  case _Sensor_AirF:
  case _Sensor_EvaF:
  case _Sensor_AirF | _Sensor_EvaP:
  case _Sensor_AirF | _Sensor_EvaF:
    return _HeaterAOff_FFLowTemp_Always;
    // break; if statement is not reachable comment out;
  case _Sensor_AirP | _Sensor_EvaP:
  case _Sensor_AirP | _Sensor_EvaF:
    // return _HeaterAOn_FFLowTemp_ThereIsAFRZDefrost; //FF defrost boyunca calytsyn
    if (cabinP->values.frzDefrost)
    {
      return _HeaterAOn_FFLowTemp_ThereIsAFRZDefrost;
    }
    else
    {
      if (cabinP->components.airTempSensor.valueInCExt >=
          cabinP->sets.datas.lowTempHeaterCutOutTemp)
        return _HeaterAOff_FFLowTemp_AirHigherThanLTHCutOut;

      if (cabinP->components.airTempSensor.valueInCExt <=
          cabinP->sets.datas.lowTempHeaterCutInTemp)
        return _HeaterAOn_JKRLowTemp_AirLowerThanLTHCutIn;
    }
    break;
  }

  return _HeaterANone_FFLowTemp;
}
/*** APW : Add Heater on operation in CT ***/
enHeaterAStates cabinFfHeaterACommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  return _HeaterAOff_START;
}
enHeaterAStates cabinFfHeaterACommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  
  //stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;
  
     if(cabinC->components.heaterA.commercialDefrostTestPriority == 0)
       return _HeaterAOff_START;
     else if(cabinC->components.heaterA.commercialDefrostTestPriority == 1 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_DefrostOnTest)
       return _HeaterAOn_START;
     else if(cabinC->components.heaterA.commercialDefrostTestPriority == 2 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater1OnTest)
       return _HeaterAOn_START;
     else if(cabinC->components.heaterA.commercialDefrostTestPriority == 3 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater2OnTest)
       return _HeaterAOn_START;
     else if(cabinC->components.heaterA.commercialDefrostTestPriority == 4 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater3OnTest)
       return _HeaterAOn_START;
     else if(cabinC->components.heaterA.commercialDefrostTestPriority == 5 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater4OnTest)
       return _HeaterAOn_START;
     else if(cabinC->components.heaterA.commercialDefrostTestPriority == 6 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater5OnTest)
       return _HeaterAOn_START;
     else
       return _HeaterAOff_START;
       
}
/*** APW : Add Heater on operation in CT ***/
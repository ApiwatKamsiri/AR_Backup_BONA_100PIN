#include "cabin_frz_d_heater.h"

enHeaterBStates cabinFrzDHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirF:
    case _Sensor_EvaP:
    case _Sensor_EvaF:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        return _HeaterBOff_FRZDefrost_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterBNone_FRZDefrost;
}

enHeaterBStates cabinFrzDHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirF:
    case _Sensor_EvaP:
    case _Sensor_EvaF:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        return _HeaterBOff_FRZDefrost_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterBNone_FRZDefrost;
}

enHeaterBStates cabinFrzDHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirF:
        return _HeaterBOff_FRZDefrost_Always;
        // break; if statement is not reachable comment out;
    case _Sensor_EvaP:
    case _Sensor_EvaF:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        return _HeaterBOn_FRZDefrost_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterBNone_FRZDefrost;
}

enHeaterBStates cabinFrzDHeaterDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirF:
    case _Sensor_EvaP:
    case _Sensor_EvaF:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        return _HeaterBOff_FRZDefrost_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterBNone_FRZDefrost;
}
/*** APW : Add CT heater software ***/
enHeaterBStates cabinFfzHeaterBCommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  return _HeaterBOff_START;
}
enHeaterBStates cabinFfzHeaterBCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  
  //stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;
  
     if(cabinC->components.heaterB.commercialDefrostTestPriority == 0)
       return _HeaterBOff_START;
     else if(cabinC->components.heaterB.commercialDefrostTestPriority == 1 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_DefrostOnTest)
       return _HeaterBOn_START;
     else if(cabinC->components.heaterB.commercialDefrostTestPriority == 2 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater1OnTest)
       return _HeaterBOn_START;
     else if(cabinC->components.heaterB.commercialDefrostTestPriority == 3 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater2OnTest)
       return _HeaterBOn_START;
     else if(cabinC->components.heaterB.commercialDefrostTestPriority == 4 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater3OnTest)
       return _HeaterBOn_START;
     else if(cabinC->components.heaterB.commercialDefrostTestPriority == 5 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater4OnTest)
       return _HeaterBOn_START;
     else if(cabinC->components.heaterB.commercialDefrostTestPriority == 6 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater5OnTest)
       return _HeaterBOn_START;
     else
       return _HeaterBOff_START;
       
}
/*** APW : Add CT heater software ***/
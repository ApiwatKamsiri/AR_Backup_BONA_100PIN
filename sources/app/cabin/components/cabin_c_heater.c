#include "cabin_c_heater.h"

enHeaterCStates cabinCHeater(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{ // eva kapak isitici on mu off mu olacak ona karar veren fonk.
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;

    if (cabinP->values.doorsClosedState)
    {
        OnTime = cabinC->parameters.HeaterC.OnTimeB;
        OffTime = cabinC->parameters.HeaterC.OffTimeB;
    }
    else
    {
        OnTime = cabinC->parameters.HeaterC.OnTimeA;
        OffTime = cabinC->parameters.HeaterC.OffTimeA;
    }

    if (!cabinP->values.heaterCTimer)
        cabinP->values.heaterCTimer = OnTime + OffTime;

    if (cabinP->values.heaterCTimer > OffTime)
        return _HeaterCOn_ALL_DuringOnTime;
    else
        return _HeaterCOff_ALL_DuringOffTime;
}

enHeaterCStates cabinCHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;

    if (cabinP->values.doorsClosedState)
        return _HeaterCOff_ALL_AtDoorsClosedState;

    // Door open
    OnTime = cabinC->parameters.HeaterC.OnTimeA;
    OffTime = cabinC->parameters.HeaterC.OffTimeA;

    if (!cabinP->values.heaterCTimer)
        cabinP->values.heaterCTimer = OnTime + OffTime;

    if (cabinP->values.heaterCTimer > OffTime)
        return _HeaterCOn_ALL_DuringOnTime;
    else
        return _HeaterCOff_ALL_DuringOffTime;
}

enHeaterCStates cabinCHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;

    if (cabinP->values.doorsClosedState)
        return _HeaterCOn_ALL_Always;

    // Door open
    OnTime = cabinC->parameters.HeaterC.OnTimeA;
    OffTime = cabinC->parameters.HeaterC.OffTimeA;

    if (!cabinP->values.heaterCTimer)
        cabinP->values.heaterCTimer = OnTime + OffTime;

    if (cabinP->values.heaterCTimer > OffTime)
        return _HeaterCOn_ALL_DuringOnTime;
    else
        return _HeaterCOff_ALL_DuringOffTime;
}

enHeaterCStates cabinCHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;

    if (cabinP->values.doorsClosedState)
        return _HeaterCOn_ALL_Always;

    // Door open
    OnTime = cabinC->parameters.HeaterC.OnTimeA;
    OffTime = cabinC->parameters.HeaterC.OffTimeA;

    if (!cabinP->values.heaterCTimer)
        cabinP->values.heaterCTimer = OnTime + OffTime;

    if (cabinP->values.heaterCTimer > OffTime)
        return _HeaterCOn_ALL_DuringOnTime;
    else
        return _HeaterCOff_ALL_DuringOffTime;
}

enHeaterCStates cabinCHeaterDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;

    if (cabinP->values.doorsClosedState)
        return _HeaterCOn_ALL_Always;

    // Door open
    OnTime = cabinC->parameters.HeaterC.OnTimeA;
    OffTime = cabinC->parameters.HeaterC.OffTimeA;

    if (!cabinP->values.heaterCTimer)
        cabinP->values.heaterCTimer = OnTime + OffTime;

    if (cabinP->values.heaterCTimer > OffTime)
        return _HeaterCOn_ALL_DuringOnTime;
    else
        return _HeaterCOff_ALL_DuringOffTime;
}

enHeaterCStates cabinCHeaterHumidity(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;

    enAmbientType ambient = cabinP->values.ambientType;
    enHumidityType humidity = cabinP->values.humidityType;

    OnTime = cabinC->parameters.HeaterC.humidityHeater.OnTime[humidity][ambient];
    OffTime = cabinC->parameters.HeaterC.humidityHeater.OffTime[humidity][ambient];

    if (!cabinP->values.heaterCTimer)
        cabinP->values.heaterCTimer = OnTime + OffTime;

    if (cabinP->values.heaterCTimer > OffTime)
        return _HeaterCOn_ALL_DuringOnTime;
    else
        return _HeaterCOff_ALL_DuringOffTime;
}

void cabinCHeaterControl(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinC->components.heaterC.type &&
        isHeaterCOn(cabinP->states.HeaterC))
        cabinP->components.heaterC.value = _HeaterValue_ON;
    else
        cabinP->components.heaterC.value = _HeaterValue_OFF;
}
/*** APW : Add CT heater software ***/
enHeaterCStates cabinFfHeaterCCommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  return _HeaterCOff_START;
}
enHeaterCStates cabinFfHeaterCCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  //stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;
  
     if(cabinC->components.heaterC.commercialDefrostTestPriority == 0)
       return _HeaterCOff_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 1 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_DefrostOnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 2 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater1OnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 3 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater2OnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 4 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater3OnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 5 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater4OnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 6 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater5OnTest)
       return _HeaterCOn_START;
     else
       return _HeaterCOff_START;
       
}
enHeaterCStates cabinFfzHeaterCCommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  return _HeaterCOff_START;
}
enHeaterCStates cabinFfzHeaterCCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  //stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;
  
     if(cabinC->components.heaterC.commercialDefrostTestPriority == 0)
       return _HeaterCOff_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 1 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_DefrostOnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 2 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater1OnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 3 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater2OnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 4 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater3OnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 5 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater4OnTest)
       return _HeaterCOn_START;
     else if(cabinC->components.heaterC.commercialDefrostTestPriority == 6 &&
             params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater5OnTest)
       return _HeaterCOn_START;
     else
       return _HeaterCOff_START;
       
}
/*** APW : Add CT heater software ***/
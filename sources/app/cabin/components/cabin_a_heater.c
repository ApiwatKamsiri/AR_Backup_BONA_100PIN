#include "cabin_a_heater.h"

enHeaterAStates cabinAHeater(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;

    if (cabinP->values.doorsClosedState)
    {
        OnTime = cabinC->parameters.HeaterA.OnTimeB;
        OffTime = cabinC->parameters.HeaterA.OffTimeB;
    }
    else
    {
        OnTime = cabinC->parameters.HeaterA.OnTimeA;
        OffTime = cabinC->parameters.HeaterA.OffTimeA;
    }

    if (!cabinP->values.heaterATimer)
        cabinP->values.heaterATimer = OnTime + OffTime;

    if (cabinP->values.heaterATimer > OffTime)
        return _HeaterAOn_ALL_DuringOnTime;
    else
        return _HeaterAOff_ALL_DuringOffTime;
}

void cabinAHeaterControl(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinC->components.heaterA.type &&
        isHeaterAOn(cabinP->states.HeaterA))
        cabinP->components.heaterA.value = _HeaterValue_ON;
    else
        cabinP->components.heaterA.value = _HeaterValue_OFF;
}
enHeaterAStates cabinAHeaterHumidity(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;

    enAmbientType ambient = cabinP->values.ambientType;
    enHumidityType humidity = cabinP->values.humidityType;

    OnTime = cabinC->parameters.HeaterA.humidityHeater.OnTime[humidity][ambient];
    OffTime = cabinC->parameters.HeaterA.humidityHeater.OffTime[humidity][ambient];

    if (!cabinP->values.heaterATimer)
        cabinP->values.heaterATimer = OnTime + OffTime;

    if (cabinP->values.heaterATimer > OffTime)
        return _HeaterAOn_ALL_DuringOnTime;
    else
        return _HeaterAOff_ALL_DuringOffTime;
}


enHeaterAStates cabinFfzHeaterACommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  return _HeaterAOff_START;
}
enHeaterAStates cabinFfzHeaterACommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
#include "cabin_flap_flap_heater.h"

enHeaterBStates cabinFlapFlapHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    unsigned short period;
    unsigned short onTime;
    unsigned short offTime;

    if (cabinP->sets.work.Mode == _CabinMode_FF)
    {
        onTime = cabinC->parameters.HeaterB.OnTimeA;
        offTime = cabinC->parameters.HeaterB.OffTimeA;
    }
    else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
    {
        onTime = cabinC->parameters.HeaterB.OnTimeB;
        offTime = cabinC->parameters.HeaterB.OffTimeB;
    }
    else
    {
        return _HeaterBNone_FLAPFlap;
    }

    period = onTime + offTime;

    if (cabinP->values.heaterBTimer > period)
        cabinP->values.heaterBTimer = period;

    if (cabinP->values.heaterBTimer > offTime)
        return _HeaterBOn_FLAPFlap_DuringOnTime;
    else
        return _HeaterBOff_FLAPFlap_DuringOffTime;
}

enHeaterBStates cabinFlapFlapHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    return cabinFlapFlapHeaterCool(consts, params);
}

/*** APW : Add heater on operation in CT ***/
enHeaterBStates cabinFlapFlapHeaterCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    return _HeaterBOff_START;
}

enHeaterBStates cabinFlapFlapHeaterCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
/*** APW : Add heater on operation in CT ***/
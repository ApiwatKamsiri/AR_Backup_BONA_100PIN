#include "board/board.h"
#include "cabin_flap_chl_heater.h"

enHeaterCStates cabinFlapCHLHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    unsigned short period;
    unsigned short onTime;
    unsigned short offTime;

    if (cabinP->sets.work.Mode == _CabinMode_FF)
    {
        onTime = cabinC->parameters.HeaterC.OnTimeA;
        offTime = cabinC->parameters.HeaterC.OffTimeA;
    }
    else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
    {
        onTime = cabinC->parameters.HeaterC.OnTimeB;
        offTime = cabinC->parameters.HeaterC.OffTimeB;
    }
    else
    {
        return _HeaterCNone_FLAPChannel;
    }

    period = onTime + offTime;

    if (cabinP->values.heaterCTimer > period)
        cabinP->values.heaterCTimer = period;

    if (cabinP->values.heaterCTimer > offTime)
        return _HeaterCOn_FLAPChannel_DuringOnTime;
    else
        return _HeaterCOff_FLAPChannel_DuringOffTime;
}

enHeaterCStates cabinFlapCHLHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    return cabinFlapCHLHeaterCool(consts, params);
}

/*** APW : Add heater on operation in CT ***/
enHeaterCStates cabinFlapCHLHeaterCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    return _HeaterCOff_START;
}

enHeaterCStates cabinFlapCHLHeaterCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
/*** APW : Add heater on operation in CT ***/ 
#include "cabin_flap_lt_heater.h"

enHeaterAStates cabinFlapLTHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    UNUSED(consts);
    stCabin_Ram *cabinP = params->cabin;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
        if (cabinP->sets.work.Mode == _CabinMode_FF)
        {
            if (cabinP->components.airTempSensor.valueInCExt >=
                cabinP->sets.datas.lowTempHeaterCutOutTemp)
                return _HeaterAOff_FLAPLowTemp_AirHigherThanLTHCutOut;

            if (cabinP->components.airTempSensor.valueInCExt <=
                cabinP->sets.datas.lowTempHeaterCutInTemp)
                return _HeaterAOn_FLAPLowTemp_AirLowerThanLTHCutIn;
        }
        else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
        {
            return _HeaterAOff_FLAPLowTemp_Always;
        }
        break;
    case _Sensor_AirF:
        return _HeaterAOff_FLAPLowTemp_Always;
        // break; statement is unreachable
    }
    return _HeaterANone_FLAPLowTemp;
}

enHeaterAStates cabinFlapLTHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    return cabinFlapLTHeaterCool(consts, params);
}

/*** APW : Add heater on operation in CT ***/
enHeaterAStates cabinFlapLTHeaterCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    return _HeaterAOff_START;
}

enHeaterAStates cabinFlapLTHeaterCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
/*** APW : Add heater on operation in CT ***/
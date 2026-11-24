#include "cabin_flap_fan.h"

enFanStates cabinFlapFanCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
#ifndef HOSHI_H30
    const stCabin_Flash *cabinC = consts->cabin;
#endif	//HOSHI_H30

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
        if (cabinP->sets.work.Mode == _CabinMode_FF)
        {
            if (cabinP->values.airSupply)
            {
#ifdef HOSHI_H30
            	if (cabinP->components.airTempSensor.valueInCExt >=
            		cabinP->sets.datas.coolCutInTemp)
                	return _FanLevel_FLAP_FFMode_AirHigherThanFanCutOutDuringAirSupplied;
            
            	if (cabinP->components.airTempSensor.valueInCExt <
            		cabinP->sets.datas.coolCutOutTemp)
                	return _FanOff_FLAP_AirLowerThanFanCutOutDuringAirSuppliedAtFFMode;
                
                if ((params->rf->Cabin[0].components.airTempSensor.valueInCExt >= params->rf->Cabin[0].sets.datas.coolCutOutTemp)            
                    &&(params->rf->Cabin[1].components.airTempSensor.valueInCExt >= params->rf->Cabin[1].sets.datas.coolCutInTemp)
                      &&(params->rf->Cooler.parameters.compressorOnTimer == 0)) 
                  return _FanLevel_FLAP_FFMode_AirHigherThanFanCutOutDuringAirSupplied;
#else
                if (cabinP->components.airTempSensor.valueInCExt >=
                    cabinP->sets.datas.fanCutOutTemp)
                    return _FanLevel_FLAP_FFMode_AirHigherThanFanCutOutDuringAirSupplied;
                if (cabinP->components.airTempSensor.valueInCExt <
                    (cabinP->sets.datas.fanCutOutTemp -
                     cabinC->parameters.Fan.CutOutHysteresis))
                    return _FanOff_FLAP_AirLowerThanFanCutOutDuringAirSuppliedAtFFMode;
#endif	//HOSHI_H30
            }
            else
            {
                return _FanOff_FLAP_DuringNoAirSuppliedAtFFMode;
            }
        }
        else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
        {
            return _FanLevel_FLAP_FRZMode;
        }
        break;

    case _Sensor_AirF:
        return _FanError_FLAP;
    }
    return _FanNone_FLAP;
}

enFanStates cabinFlapFanIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    UNUSED(consts);
    UNUSED(params);
    return _FanOff_FLAP_Always;
}

/*** APW : Add CT heater software ***/
enFanStates cabinFlapFanCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    switch (cabinP->states.Fan)
    {
      
    case _FanCommercialStart_FLAP:
        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
        {
            if(params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer >= 
              cabinC->parameters.Fan.secOnTimeFlapFanCommercial)
                return _FanOff_FLAP_Always;
            else
                return _FanCommercialStart_FLAP;
        }

        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest ||
            params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
        {
            if(!params->rf->workMode.values.CommercialTest.FLAP_Fan_Request)
                return _FanOff_FLAP_Always;
            else
                return _FanCommercialStart_FLAP;
        }
        break;
        
    case _FanNone_FLAP:
    case _FanOff_FLAP_Always:
        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
        {
            if(params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer < 
               cabinC->parameters.Fan.secOnTimeFlapFanCommercial)
                return _FanCommercialStart_FLAP;
            else
                return _FanOff_FLAP_Always;
        }
        
        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest ||
            params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
        {
            if(params->rf->workMode.values.CommercialTest.FLAP_Fan_Request)
                return _FanCommercialStart_FLAP;
            else
                return _FanOff_FLAP_Always;
        }
        break;
        
    default:
        return _FanOff_FLAP_Always;
    }
    return _FanNone_FLAP;
}
/*** APW : Add CT heater software ***/
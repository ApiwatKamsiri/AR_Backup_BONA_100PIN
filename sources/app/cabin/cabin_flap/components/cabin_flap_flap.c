#include "cabin_flap_flap.h"

enFlapStates cabinFlapFlapCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
#ifdef HOSHI_H30
            // if( cabinP->values.airSupply ){
            if (cabinP->components.airTempSensor.valueInCExt >=
            	cabinP->sets.datas.coolCutInTemp)
                return _FlapPositionA_FLAP_AirHigherThanFlapCutOutAtDuringAirSuppliedAtFFMode;
            if (cabinP->components.airTempSensor.valueInCExt <
            	cabinP->sets.datas.coolCutOutTemp)
                return _FlapOff_FLAP_AirLowerThanFlapCutOutAtDuringAirSuppliedAtFFMode;
            
            if ((params->rf->Cabin[0].components.airTempSensor.valueInCExt >= params->rf->Cabin[0].sets.datas.coolCutOutTemp)            
                &&(params->rf->Cabin[1].components.airTempSensor.valueInCExt >= params->rf->Cabin[1].sets.datas.coolCutInTemp)
                  &&(params->rf->Cooler.parameters.compressorOnTimer == 0)) 
                return _FlapPositionA_FLAP_AirHigherThanFlapCutOutAtDuringAirSuppliedAtFFMode;
#else
            // if( cabinP->values.airSupply ){
            if (cabinP->components.airTempSensor.valueInCExt >=
                cabinP->sets.datas.fanCutOutTemp)
                return _FlapPositionA_FLAP_AirHigherThanFlapCutOutAtDuringAirSuppliedAtFFMode;
            if (cabinP->components.airTempSensor.valueInCExt <
                (cabinP->sets.datas.fanCutOutTemp -
                 cabinC->parameters.Fan.CutOutHysteresis))
                return _FlapOff_FLAP_AirLowerThanFlapCutOutAtDuringAirSuppliedAtFFMode;
#endif	//HOSHI_H30
            /*
            }else{
                return _FlapOff_FLAP_DuringNoAirSuppliedAtFFMode;
            }
            */
        }
        else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
        {
            return _FlapPositionA_FLAP_AlwaysAtFRZMode;
        }
        break;
    case _Sensor_AirF:
        return _FlapPositionA_FLAP_Always;
    }
    return _FlapNone_FLAP;
}

enFlapStates cabinFlapFlapIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    UNUSED(consts);
    UNUSED(params);
    return _FlapOff_FLAP_Always;
}

/*** APW : Add damper on operation in CT ***/
enFlapStates cabinFlapFlapCommercialState(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;
    
    switch (cabinP->states.Flap)
    {
      case _FlapPositionA_START:
            if(params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer >= 
              (cabinC->parameters.Fan.secOnTimeFlapFanCommercial + 
                cabinC->parameters.Fan.secOnTimeFfFanCommercial + 
                  cabinC->parameters.Fan.secOnTimeFrzFanCommercial))
                //return _FlapOff_START;
            {
              if(cabinP->components.flap.flapFeed != 0)
                return _FlapPositionA_START;
              else
                return _FlapOff_START;
            }
            else
                return _FlapPositionA_START;
        break;
        
      case _FlapNone_START:
      case _FlapOff_START:
            if(params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer >= 
              (cabinC->parameters.Fan.secOnTimeFlapFanCommercial + 
                cabinC->parameters.Fan.secOnTimeFfFanCommercial + 
                  cabinC->parameters.Fan.secOnTimeFrzFanCommercial))
                //return _FlapOff_START;
            {
              if(cabinP->components.flap.flapFeed != 0)
                return _FlapPositionA_START;
              else
                return _FlapOff_START;
            }
            else
                return _FlapPositionA_START;
        break;
        
      default:
        return _FlapNone_START;
    }
}
/*** APW : Add heater on operation in CT ***/

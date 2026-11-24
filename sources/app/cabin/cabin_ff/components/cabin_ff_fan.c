#include "cabin_ff_fan.h"

enFanStates cabinFfFanCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    if ((cabinP->states.Cool == _CoolIn_FF_MaxIdleTimeElapsed) &&
        (cabinP->stateTimers.Cool < cabinC->parameters.Cool.MaxIdleTimeElapsedCoolTime))
        return _FanLevel_FF_MaxIdleTimeElapsed;

    switch (cabinP->states.Sensor)
    {
      
    case _Sensor_AirP:
    case _Sensor_AirP | _Sensor_EvaF:
       return _FanLevel_FF_AirHigherThanFanCutOut; // when eva sensor failure it should be work when cool happen
      break;
    case _Sensor_AirP | _Sensor_EvaP:
    
        if ((cabinP->components.evaTempSensor.valueInCExt <= cabinP->sets.datas.fanCutOutTemp) || 
            (cabinP->stateTimers.Cool >= cabinC->parameters.Fan.CutInTime))
            return _FanLevel_FF_AirHigherThanFanCutOut;
        if (cabinP->components.evaTempSensor.valueInCExt >= cabinP->sets.datas.fanCutInTemp)
            return _FanOff_FF_AirLowerThanFanCutOut;
        break; // if statement is not reachable comment out;

    case _Sensor_AirF:
    case _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaF:
        return _FanError_FF;

    case _Sensor_EvaP:
        return _FanLevel_FF;
        // break; if statement is not reachable comment out;
    }
    return _FanNone_FF;
}

enFanStates cabinFfFanIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirP | _Sensor_EvaF:
      return _FanOff_FF_AirLowerThanFanCutOut; // when eva sensor failure it should be stop when idle happen
       /* if (cabinP->components.airTempSensor.valueInCExt >= cabinP->sets.datas.fanCutInTemp)
            return _FanLevel_FF_AirHigherThanFanCutOut;
        if (cabinP->components.airTempSensor.valueInCExt <= cabinP->sets.datas.fanCutOutTemp)
            return _FanOff_FF_AirLowerThanFanCutOut;*/
        break;
    case _Sensor_AirP | _Sensor_EvaP:

        /*if (cabinP->stateTimers.Idle < cabinC->parameters.Fan.WorkTimeAfterCool && cabinP->states.WorkPrev == _Work_Cool)
            return _FanLevel_FF_AfterCoolState;*/

        if (cabinP->values.ambientType == _Ambient_NONE)
        {
            if (cabinP->values.doorsClosedState)
            {
                if ((cabinP->values.duty.mean > cabinC->parameters.Fan.MinDuty_DC) &&
                    (cabinP->values.duty.mean < cabinC->parameters.Fan.MaxDuty_DC))
                    return _FanOff_FF_DutyInsideInterval;
                else
                    return _FanLevel_FF_AtDC_DutyOutsideInterval;
            }
            else
            {
                if ((cabinP->values.duty.mean > cabinC->parameters.Fan.MinDuty_DO) &&
                    (cabinP->values.duty.mean < cabinC->parameters.Fan.MaxDuty_DO))
                    return _FanOff_FF_DutyInsideInterval;
                else
                    return _FanLevel_FF_AtDO_DutyOutsideInterval;
            }
        }
        else
        {
            if (!params->rf->Cooler.components.compressor.value)
            {
                if (cabinC->parameters.Fan.CompressorOFF[cabinP->values.ambientType])
                {
                  if (cabinP->components.evaTempSensor.valueInCExt <= cabinP->sets.datas.fanCutOutTemp)
                    return _FanLevel_FF_AirHigherThanFanCutOut;
                  if (cabinP->components.evaTempSensor.valueInCExt >= cabinP->sets.datas.fanCutInTemp)
                    return _FanOff_FF_AirLowerThanFanCutOut;
                  break; // if statement is not reachable comment out;
                }
                else
                    return _FanOff_FF_AtCompressorOFFAmbientLevel;
            }
            else
            {
                if (cabinC->parameters.Fan.CompressorON_FRZ[cabinP->values.ambientType])
                {                  
                  if (cabinP->components.evaTempSensor.valueInCExt <= cabinP->sets.datas.fanCutOutTemp)
                    return _FanLevel_FF_AtCompressorONFRZAmbientLevel;
                  if (cabinP->components.evaTempSensor.valueInCExt >= cabinP->sets.datas.fanCutInTemp)
                    return _FanOff_FF_AtCompressorONFRZAmbientLevel;
                  break; // if statement is not reachable comment out;
                }
                else
                    return _FanOff_FF_AtCompressorONFRZAmbientLevel;
            }
        }
        // break; if statement is not reachable comment out;
    case _Sensor_AirF | _Sensor_EvaF:
        return _FanError_FF;
        // break; if statement is not reachable comment out;
    case _Sensor_AirF:
    case _Sensor_EvaP:
    case _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaP:
        return _FanOff_FF_Always;
        // break; if statement is not reachable comment out;
    }
    return _FanNone_FF;
}

enFanStates cabinFfFanDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaP:
        if (cabinP->values.doorsClosedState)
        {
            // return _FanOff_FF_AtDC;
            if ((cabinP->values.duty.mean > cabinC->parameters.Fan.FF_Fan_MinDutyDC_FFDefrost) &&
                (cabinP->values.duty.mean < cabinC->parameters.Fan.FF_Fan_MaxDutyDC_FFDefrost))
                return _FanLevel_FF_AtDC_DutyOutsideInterval;
            else
                return _FanOff_FF_DutyInsideInterval;
        }
        else
        {
            if (cabinC->components.heaterB.type == _HeaterType_AC)
                return _FanOff_FF_Always;
            else
                return _FanLevel_FF_AtDO_DutyOutsideInterval;
        }
    // break; if statement is not reachable comment out;
    case _Sensor_EvaF:
    case _Sensor_AirF:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        return _FanLevel_FF; //_FanError_FF; ozgur
                             // break; if statement is not reachable comment out;
    }
    return _FanNone_FF;
}

/*** APW : Add Fan on operation in CT ***/
enFanStates cabinFfFanCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    switch (cabinP->states.Fan)
    {
      
    case _FanCommercialStart_FF:
     
        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
        {
            if(params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer >= 
                (cabinC->parameters.Fan.secOnTimeFlapFanCommercial + 
                  cabinC->parameters.Fan.secOnTimeFfFanCommercial))
                return _FanOff_FF_Always;
            else
                return _FanCommercialStart_FF;
        }

        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest ||
            params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
        {
            if(!params->rf->workMode.values.CommercialTest.FF_Fan_Request)
                return _FanOff_FF_Always;
            else
                return _FanCommercialStart_FF;
        }
        break;
        
    case _FanNone_FF:
    case _FanOff_FF_Always:
        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
        {
            if((params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer >= 
                  cabinC->parameters.Fan.secOnTimeFlapFanCommercial) &&
                    (params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer < 
                      (cabinC->parameters.Fan.secOnTimeFlapFanCommercial + 
                        cabinC->parameters.Fan.secOnTimeFfFanCommercial)))
                return _FanCommercialStart_FF;
            else
                return _FanOff_FF_Always;
        }
        
        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest ||
            params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
        {
            if(params->rf->workMode.values.CommercialTest.FF_Fan_Request)
                return _FanCommercialStart_FF;
            else
                return _FanOff_FF_Always;
        }
        break;
        
    default:
        return _FanOff_FF_Always;
    }
    return _FanNone_FF;
}
/*** APW : Add Fan on operation in CT ***/
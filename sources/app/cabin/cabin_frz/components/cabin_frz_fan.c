#include "cabin_frz_fan.h"

enFanStates cabinFrzFanCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if ((cabinP->states.Cool == _CoolIn_FRZ_MaxIdleTimeElapsed) &&
        (cabinP->stateTimers.Cool < cabinC->parameters.Cool.MaxIdleTimeElapsedCoolTime))
        return _FanLevel_FRZ_MaxIdleTimeElapsed;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_EvaP:
        if (cabinP->values.doorsClosedState)
            return _FanLevel_FRZ_AtDC;
        else
        {
            if (cabinP->sets.work.functions.Quick)
                return _FanQuick_FRZ;
            else
                return _FanLevel_FRZ_AtDO;
        }
        // break; if statement is not reachable comment out;
    case _Sensor_AirF:
    case _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        if (cabinP->sets.work.functions.Quick)
            return _FanQuick_FRZ;
        else
            return _FanError_FRZ;
        // break; if statement is not reachable comment out;

    case _Sensor_AirP | _Sensor_EvaP:
        if ((cabinP->components.evaTempSensor.valueInCExt <= cabinC->parameters.Fan.CutInEvaTemp) ||
            (cabinP->stateTimers.Cool >= cabinC->parameters.Fan.CutInTime))
        {
            if (cabinP->values.doorsClosedState)
                return _FanLevel_FRZ_AtDC_EvaLowerThanFanCutIn;
            else
            {
                if (cabinP->sets.work.functions.Quick)
                    return _FanQuick_FRZ;
                else
                {
                  /*if (cabinP->components.evaTempSensor.valueInCExt <= cabinP->sets.datas.fanCutInTemp)
                    return _FanLevel_FRZ_AtDO_EvaLowerThanFanCutIn;
                  if (cabinP->components.evaTempSensor.valueInCExt >= cabinP->sets.datas.fanCutOutTemp)
                    return _FanOff_FRZ_Always;
                  break; // if statement is not reachable comment out;*/
                    return _FanLevel_FRZ_AtDO_EvaLowerThanFanCutIn;
                }
            }
        }
        break;
    }
    return _FanNone_FRZ;
}

enFanStates cabinFrzFanIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

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
        if (cabinP->stateTimers.Idle < cabinC->parameters.Fan.WorkTimeAfterCool && cabinP->states.WorkPrev == _Work_Cool)
            return _FanLevel_FRZ_AfterCoolState;
        else if (cabinP->sets.user.functions.Convertible)
            return _FanLevel_FRZ_ConvertibleMode;
        else
        {
            if (cabinP->values.ambientType != _Ambient_NONE)
            {
                if (!params->rf->Cooler.components.compressor.value)
                {
                    if (cabinC->parameters.Fan.CompressorOFF[cabinP->values.ambientType])
                        return _FanLevel_FRZ_AtCompressorOFFAmbientLevel;
                     else
                        return _FanOff_FRZ_AtCompressorOFFAmbientLevel;
                }
                else
                {
                    if (cabinC->parameters.Fan.CompressorON_FF[cabinP->values.ambientType])
                        return _FanLevel_FRZ_AtCompressorONFFAmbientLevel;
                    else
                        return _FanOff_FRZ_AtCompressorONFFAmbientLevel;
                }
                // break; if statement is not reachable comment out;
            }
            else
                return _FanOff_FRZ_Always;
        }
    }
    return _FanNone_FRZ;
}

enFanStates cabinFrzFanDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
        return _FanOff_FRZ_Always;
        // break; if statement is not reachable comment out;
    }
    return _FanNone_FRZ;
}

enFanStates cabinFrzFanDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
        return _FanOff_FRZ_Always;
        // break; if statement is not reachable comment out;
    }
    return _FanNone_FRZ;
}

/*** APW : Add fan on operation in CT ***/
enFanStates cabinFrzFanCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;
    
    switch (cabinP->states.Fan)
    {

    case _FanCommercialStart_FRZ:

        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
        {
            if(params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer >=
                (cabinC->parameters.Fan.secOnTimeFlapFanCommercial + 
                  cabinC->parameters.Fan.secOnTimeFfFanCommercial + 
                    cabinC->parameters.Fan.secOnTimeFrzFanCommercial))
                return _FanOff_FRZ_Always;
            else
                return _FanCommercialStart_FRZ;
        }

        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest ||
            params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
        {
            if(!params->rf->workMode.values.CommercialTest.FRZ_Fan_Request)
                return _FanOff_FRZ_Always;
            else
                return _FanCommercialStart_FRZ;
        }
        break;
        
    case _FanNone_FRZ:
    case _FanOff_FRZ_Always:
        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
        {
            if((params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer >= (cabinC->parameters.Fan.secOnTimeFlapFanCommercial + cabinC->parameters.Fan.secOnTimeFfFanCommercial)) && 
               (params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer < (cabinC->parameters.Fan.secOnTimeFlapFanCommercial + cabinC->parameters.Fan.secOnTimeFfFanCommercial + cabinC->parameters.Fan.secOnTimeFrzFanCommercial)))
                return _FanCommercialStart_FRZ;
            else
                return _FanOff_FRZ_Always;
        }

        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest ||
            params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
        {
            if(params->rf->workMode.values.CommercialTest.FRZ_Fan_Request)
                return _FanCommercialStart_FRZ;
            else
                return _FanOff_FRZ_Always;
        }
        break;
        
    default:
        return _FanOff_FRZ_Always;
    }
    return _FanNone_FRZ;
}
/*** APW : Add fan on operation in CT ***/
#ifdef JKRavailable

#include "cabin_jkr_fan.h"

enFanStates cabinJkrFanCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        if (cabinP->sets.work.Mode == _CabinMode_FF)
        {
            if (cabinP->components.airTempSensor.valueInCExt >=
                cabinP->sets.datas.fanCutOutTemp)
            {
                if (cabinP->values.doorsClosedState)
                    return _FanLevel_JKR_FFMode_AtDC_AirHigherThanFanCutOut;
                else
                    return _FanLevel_JKR_FFMode_AtDO_AirHigherThanFanCutOut;
            }

            if (cabinP->components.airTempSensor.valueInCExt <
                (cabinP->sets.datas.fanCutOutTemp -
                 cabinC->parameters.Fan.CutOutHysteresis))
                return _FanOff_JKR_AirLowerThanFanCutOut;
        }
        else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
        {
            if (cabinP->values.doorsClosedState)
            {
                if ((cabinP->components.evaTempSensor.valueInCExt <= cabinC->parameters.Fan.CutInEvaTemp) ||
                    (cabinP->stateTimers.Cool >= 5))
                {
                    return _FanLevel_JKR_FRZMode_EvaLowerThanFanCutIn;
                }
            }
            else
            {
                return _FanLevel_JKR_FRZMode_AtDO;
            }
        }
        break;

    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaF:
        return _FanError_JKR;
        // break; if statement is not reachable comment out;
    }
    return _FanNone_JKR;
}

enFanStates cabinJkrFanIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        return _FanOff_JKR_Always;
        // break; if statement is not reachable comment out;
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaF:
        if (cabinP->sets.work.Mode == _CabinMode_FF)
        {
            return _FanError_JKR;
        }
        else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
        {
            return _FanOff_JKR_Always;
        }
        // break; if statement is not reachable comment out;
    }
    return _FanNone_JKR;
}

enFanStates cabinJkrFanDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        return _FanOff_JKR_Always;
        // break; if statement is not reachable comment out;
    }
    return _FanNone_JKR;
}

enFanStates cabinJkrFanDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        return _FanOff_JKR_Always;
        // break; if statement is not reachable comment out;
    }
    return _FanNone_JKR;
}
#endif
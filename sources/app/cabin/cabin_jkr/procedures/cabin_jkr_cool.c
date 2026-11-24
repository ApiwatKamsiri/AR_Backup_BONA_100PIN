#ifdef JKRavailable

#include "cabin_jkr_cool.h"

enCoolStates cabinJkrCoolIn(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinP->sets.work.functions.Sabbath)
    {
        if (cabinP->stateTimers.Idle >=
            cabinP->values.duty.meanIdleTime)
            return _CoolIn_JKR_MeanIdleTimeElapsed;
        if (cabinP->stateTimers.Idle >=
            cabinC->parameters.Cool.SabbathMaxIdleTime)
            return _CoolIn_JKR_SabbathMaxIdleTimeElapsed;
        return _CoolNone_JKR;
    }

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        if (cabinP->components.airTempSensor.valueInCExt >=
            cabinP->sets.datas.coolCutInTemp)
            return _CoolIn_JKR_AirHigherThanCoolCutIn;
        if (cabinP->stateTimers.Idle >=
            cabinC->parameters.Cool.MaxIdleTime)
            return _CoolIn_JKR_MaxIdleTimeElapsed;
        break;

    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaF:
        if (cabinP->stateTimers.Idle >=
            cabinC->parameters.Cool.ErrorMaxIdleTime)
            return _CoolIn_JKR_ErrorMaxIdleTimeElapsed;
        break;
    }
    return _CoolNone_JKR;
}

enCoolStates cabinJkrCoolOut(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        if (cabinP->components.airTempSensor.valueInCExt <=
            cabinP->sets.datas.coolCutOutTemp)
            return _CoolOut_JKR_AirLowerThanCoolCutOut;
        if ((cabinP->sets.work.Mode == _CabinMode_FF) &&
            (cabinP->values.quickFRZ) &&
            (cabinP->components.airTempSensor.valueInCExt <=
             cabinC->parameters.Cool.QuickFRZCutOutTemp))
            return _CoolOut_JKR_AirIsTooColdAtQuickFRZ;
        break;
    case _Sensor_AirF | _Sensor_EvaF:
        if (cabinP->sets.work.functions.Sabbath)
        {
            if (cabinP->stateTimers.Cool >=
                cabinP->values.duty.meanCoolTime)
                return _CoolOut_JKR_MeanCoolTimeElapsed;
        }
    case _Sensor_AirF | _Sensor_EvaP:
        if (cabinP->stateTimers.Cool <=
            cabinC->parameters.Cool.ErrorMaxCoolTime)
            return _CoolOut_JKR_ErrorMaxCoolTimeElapsed;
        break;
    }
    return _CoolNone_JKR;
}
#endif
#include "cabin_frz_cool.h"

enCoolStates cabinFrzCoolIn(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinP->sets.work.functions.Sabbath)
    {
        if (cabinP->stateTimers.Idle >=
            cabinP->values.duty.meanIdleTime)
            return _CoolIn_FRZ_MeanIdleTimeElapsed;
        if (cabinP->stateTimers.Idle >=
            cabinC->parameters.Cool.SabbathMaxIdleTime)
            return _CoolIn_FRZ_SabbathMaxIdleTimeElapsed;
        //return _CoolNone_FRZ; //Disable for sabbath mode after defrost start same as normal mode
    }

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        if (cabinP->components.airTempSensor.valueInCExt >=
            cabinP->sets.datas.coolCutInTemp)
            return _CoolIn_FRZ_AirHigherThanCoolCutIn;
        if (cabinP->stateTimers.Idle >=
                cabinC->parameters.Cool.MaxIdleTime &&
            !cabinP->sets.user.functions.CabinOff &&
            !cabinP->sets.user.functions.Convertible)
            return _CoolIn_FRZ_MaxIdleTimeElapsed;
        break;

    case _Sensor_AirF:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaF:
        if (cabinP->stateTimers.Idle >=
            cabinC->parameters.Cool.ErrorMaxIdleTime)
            return _CoolIn_FRZ_ErrorMaxIdleTimeElapsed;
        break;

    case _Sensor_EvaP:
        if (cabinC->parameters.Cool.Type == _CoolType_Normal)
        {
            if (cabinP->components.evaTempSensor.valueInCExt >=
                cabinP->sets.datas.coolCutInTemp)
                return _CoolIn_FRZ_EvaHigherThanCoolCutIn;
            if (cabinP->stateTimers.Idle >=
                cabinC->parameters.Cool.MaxIdleTime)
                return _CoolIn_FRZ_MaxIdleTimeElapsed;
        }
        break;

    case _Sensor_EvaF:
        if (cabinC->parameters.Cool.Type == _CoolType_Normal)
        {
            if (cabinP->stateTimers.Idle >=
                cabinC->parameters.Cool.ErrorMaxIdleTime)
                return _CoolIn_FRZ_ErrorMaxIdleTimeElapsed;
        }
        break;
    }
    return _CoolNone_FRZ;
}

enCoolStates cabinFrzCoolOut(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if ((cabinP->states.Cool == _CoolIn_FRZ_MaxIdleTimeElapsed) &&
        (cabinP->stateTimers.Cool < cabinC->parameters.Cool.MaxIdleTimeElapsedCoolTime))
        return _CoolNone_FRZ;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        if (cabinP->components.airTempSensor.valueInCExt <=
            cabinP->sets.datas.coolCutOutTemp)
            return _CoolOut_FRZ_AirLowerThanCoolCutOut;
        break;

    case _Sensor_AirF:
    case _Sensor_AirF | _Sensor_EvaF:
        if (cabinP->sets.work.functions.Sabbath)
        {
            if (cabinP->stateTimers.Cool >=
                cabinP->values.duty.meanCoolTime)
                return _CoolOut_FRZ_MeanCoolTimeElapsed;
        }
    case _Sensor_AirF | _Sensor_EvaP:
        if (cabinP->stateTimers.Cool >=
            cabinC->parameters.Cool.ErrorMaxCoolTime)
            return _CoolOut_FRZ_ErrorMaxCoolTimeElapsed;
        break;

    case _Sensor_EvaP:
        if (cabinC->parameters.Cool.Type == _CoolType_Normal)
        {
            if (cabinP->components.evaTempSensor.valueInCExt <=
                cabinP->sets.datas.coolCutOutTemp)
                return _CoolOut_FRZ_EvaLowerThanCoolCutOut;
        }
        else
          return _CoolOut_FRZ_EvaLowerThanCoolCutOut;
        break;

    case _Sensor_EvaF:
        if (cabinC->parameters.Cool.Type == _CoolType_Normal)
        {
            if (cabinP->sets.work.functions.Sabbath)
            {
                if (cabinP->stateTimers.Cool >=
                    cabinP->values.duty.meanCoolTime)
                    return _CoolOut_FRZ_MeanCoolTimeElapsed;
            }

            if (cabinP->stateTimers.Cool >=
                cabinC->parameters.Cool.ErrorMaxCoolTime)
                return _CoolOut_FRZ_ErrorMaxCoolTimeElapsed;
        }
        else
          return _CoolOut_FRZ_EvaLowerThanCoolCutOut;
        break;
    }

    return _CoolNone_FRZ;
}

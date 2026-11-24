#ifdef JKRavailable

#include "cabin_jkr_defrost.h"

enDefrostStates cabinJkrDefrostIn(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinP->values.quickFF)
        return _DefrostNone_JKR;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) ||
            (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
        {
            if ((cabinP->sets.work.Mode == _CabinMode_FRZ) &&
                cabinP->sets.work.functions.Quick &&
                !cabinP->values.quickDefrost)
            {
                if (cabinP->values.quickTimer >=
                    cabinC->parameters.Defrost.MaxQuickTime)
                    return _DefrostIn_JKR_MaxQuickTimeElapsed;
            }
            else if (cabinP->values.doorsClosedState)
            {
                if (cabinP->values.realTimer >=
                    cabinC->parameters.Defrost.MaxRealTime_DC)
                    return _DefrostIn_JKR_MaxRealTimeElapsed;
            }
            else
            {
                if (cabinP->values.realTimer >=
                    cabinC->parameters.Defrost.MaxRealTime_DO)
                    return _DefrostIn_JKR_MaxRealTimeElapsed;

                if (cabinP->values.workTimer >=
                    cabinC->parameters.Defrost.MaxWorkTime_DO)
                    return _DefrostIn_JKR_MaxWorkTimeElapsed;
            }
        }
        break;

    case _Sensor_AirF | _Sensor_EvaP:
        return _DefrostNone_JKR;
        // break; if statement is not reachable comment out;
    }
    return _DefrostNone_JKR;
}

enDefrostStates cabinJkrDefrostOut(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinP->values.defrostDelayForIceCream &&
        cabinP->values.defrostDelayTimerForIceCream)
        return _DefrostNone_JKR_ForIceCream;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
        if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) ||
            (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
        {
            if (cabinP->components.evaTempSensor.valueInCExt >=
                cabinC->parameters.Defrost.DefrostCutOutTemp)
                return _DefrostOut_JKR_EvaHigherThanDefrostCutOut;
            if (cabinP->values.doorsClosedState)
            {
                if (cabinP->stateTimers.Defrost >=
                    cabinC->parameters.Defrost.MaxDefrostTime_DC)
                    return _DefrostOut_JKR_MaxDefrostTimeElapsed_DC;
            }
            else
            {
                if (cabinP->stateTimers.Defrost >=
                    cabinC->parameters.Defrost.MaxDefrostTime_DO)
                    return _DefrostOut_JKR_MaxDefrostTimeElapsed_DO;
            }
        }
        else
        {
            return _DefrostOut_JKR_Always;
        }
        break;

    case _Sensor_AirP | _Sensor_EvaF:
        if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) ||
            (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
        {
            if (cabinP->stateTimers.Defrost >=
                cabinC->parameters.Defrost.ErrorMaxDefrostTime)
                return _DefrostOut_JKR_ErrorMaxDefrostTimeElapsed;
        }
        else
        {
            return _DefrostOut_JKR_Always;
        }
        break;

    case _Sensor_AirF | _Sensor_EvaF:
        if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) ||
            (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
        {
            if (cabinP->sets.work.functions.Sabbath)
            {
                if (cabinP->stateTimers.Defrost >=
                    cabinP->values.defrost.meanTime)
                    return _DefrostOut_JKR_MeanDefrostTimeElapsed;
            }

            if (cabinP->stateTimers.Defrost >=
                cabinC->parameters.Defrost.ErrorMaxDefrostTime)
                return _DefrostOut_JKR_ErrorMaxDefrostTimeElapsed;
        }
        else
        {
            return _DefrostOut_JKR_Always;
        }
        break;
    }
    return _DefrostNone_JKR;
}

void cabinJkrDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
        if (cabinP->components.evaTempSensor.valueInCExt >=
            cabinC->parameters.Defrost.DefrostCutOutTemp)
        {
            cabinP->values.realTimer = 0;
            cabinP->values.workTimer = 0;
        }
        break;
    }
}
#endif
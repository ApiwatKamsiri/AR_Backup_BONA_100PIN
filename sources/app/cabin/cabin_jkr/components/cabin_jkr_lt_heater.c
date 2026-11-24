#ifdef JKRavailable

#include "cabin_jkr_lt_heater.h"

enHeaterAStates cabinJkrLTHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        if (cabinP->sets.work.Mode == _CabinMode_FF)
        {
            if (cabinP->components.airTempSensor.valueInCExt >=
                cabinP->sets.datas.lowTempHeaterCutOutTemp)
                return _HeaterAOff_JKRLowTemp_AirHigherThanLTHCutOut;
            if (cabinP->components.airTempSensor.valueInCExt <=
                cabinP->sets.datas.lowTempHeaterCutInTemp)
                return _HeaterAOn_JKRLowTemp_AirLowerThanLTHCutIn;
        }
        else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
        {
            return _HeaterAOff_JKRLowTemp_Always;
        }
        break;

    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaF:
        return _HeaterAOff_JKRLowTemp_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterANone_JKRLowTemp;
}

enHeaterAStates cabinJkrLTHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        if (cabinP->sets.work.Mode == _CabinMode_FF)
        {
            if (cabinP->values.frzDefrost)
            {
                return _HeaterAOn_JKRLowTemp_ThereIsAFRZDefrost;
            }
            else
            {
                if (cabinP->components.airTempSensor.valueInCExt >=
                    cabinP->sets.datas.lowTempHeaterCutOutTemp)
                    return _HeaterAOff_JKRLowTemp_AirHigherThanLTHCutOut;
                if (cabinP->components.airTempSensor.valueInCExt <=
                    cabinP->sets.datas.lowTempHeaterCutInTemp)
                    return _HeaterAOn_JKRLowTemp_AirLowerThanLTHCutIn;
            }
        }
        else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
        {
            return _HeaterAOff_JKRLowTemp_Always;
        }
        break;

    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaF:
        return _HeaterAOff_JKRLowTemp_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterANone_JKRLowTemp;
}

enHeaterAStates cabinJkrLTHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        if (cabinP->sets.work.Mode == _CabinMode_FF)
        {
            if (cabinP->values.frzDefrost)
            {
                return _HeaterAOn_JKRLowTemp_ThereIsAFRZDefrost;
            }
            else
            {
                if (cabinP->components.airTempSensor.valueInCExt >=
                    cabinP->sets.datas.lowTempHeaterCutOutTemp)
                    return _HeaterAOff_JKRLowTemp_AirHigherThanLTHCutOut;
                if (cabinP->components.airTempSensor.valueInCExt <=
                    cabinP->sets.datas.lowTempHeaterCutInTemp)
                    return _HeaterAOn_JKRLowTemp_AirLowerThanLTHCutIn;
            }
        }
        else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
        {
            return _HeaterAOff_JKRLowTemp_Always;
        }
        break;

    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaF:
        return _HeaterAOff_JKRLowTemp_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterANone_JKRLowTemp;
}

enHeaterAStates cabinJkrLTHeaterDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaF:
        return _HeaterAOff_JKRLowTemp_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterANone_JKRLowTemp;
}
#endif
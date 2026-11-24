#ifdef JKRavailable
#include "cabin_jkr_d_heater.h"

enHeaterBStates cabinJkrDHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
        return _HeaterBOff_JKRDefrost_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterBNone_JKRDefrost;
}

enHeaterBStates cabinJkrDHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
        return _HeaterBOff_JKRDefrost_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterBNone_JKRDefrost;
}

enHeaterBStates cabinJkrDHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
        if (cabinP->sets.work.Mode == _CabinMode_FF)
        {
            return _HeaterBOff_JKRDefrost_Always;
        }
        else if (cabinP->sets.work.Mode == _CabinMode_FRZ)
        {
            return _HeaterBOn_JKRDefrost_Always;
        }
        break;
    }
    return _HeaterBNone_JKRDefrost;
}

enHeaterBStates cabinJkrDHeaterDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params)
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
        return _HeaterBOff_JKRDefrost_Always;
        // break; if statement is not reachable comment out;
    }
    return _HeaterBNone_JKRDefrost;
}
#endif
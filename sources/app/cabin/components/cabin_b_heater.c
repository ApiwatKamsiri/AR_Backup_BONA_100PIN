#include "cabin_b_heater.h"

enHeaterBStates cabinBHeater(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;

    if (cabinP->values.doorsClosedState)
    {
        OnTime = cabinC->parameters.HeaterB.OnTimeB;
        OffTime = cabinC->parameters.HeaterB.OffTimeB;
    }
    else
    {
        OnTime = cabinC->parameters.HeaterB.OnTimeA;
        OffTime = cabinC->parameters.HeaterB.OffTimeA;
    }

    if (!cabinP->values.heaterBTimer)
        cabinP->values.heaterBTimer = OnTime + OffTime;

    if (cabinP->values.heaterBTimer > OffTime)
        return _HeaterBOn_ALL_DuringOnTime;
    else
        return _HeaterBOff_ALL_DuringOffTime;
}

void cabinBHeaterControl(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinC->components.heaterB.type &&
        isHeaterBOn(cabinP->states.HeaterB))
        cabinP->components.heaterB.value = _HeaterValue_ON;
    else
        cabinP->components.heaterB.value = _HeaterValue_OFF;
}
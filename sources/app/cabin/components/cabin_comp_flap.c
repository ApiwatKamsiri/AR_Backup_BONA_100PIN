#include "cabin_comp_flap.h"

void cabinFlap1SecTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    if (cabinP->values.flapTimer)
        cabinP->values.flapTimer--;
}

enFlapStates cabinFlapState(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    unsigned short OnTime;
    unsigned short OffTime;
    enFlapStates FlapState;

    if (cabinP->values.doorsClosedState)
    {
        OnTime = cabinC->parameters.Flap.OnTimeB;
        OffTime = cabinC->parameters.Flap.OffTimeB;
        FlapState = _FlapPositionB_ALL_DuringOnTime;
    }
    else
    {
        OnTime = cabinC->parameters.Flap.OnTimeA;
        OffTime = cabinC->parameters.Flap.OffTimeA;
        FlapState = _FlapPositionA_ALL_DuringOnTime;
    }

    if (!cabinP->values.flapTimer)
        cabinP->values.flapTimer = OnTime + OffTime;

    if (cabinP->values.flapTimer > OffTime)
        return FlapState;
    else
        return _FlapOff_ALL_DuringOffTime;
}

void cabinFlapControl(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (!cabinC->components.flap.type ||
        isFlapOff(cabinP->states.Flap) ||
        isFlapNone(cabinP->states.Flap))
    {
        cabinP->components.flap.value = 0;
    }
    else if (isFlapPositionA(cabinP->states.Flap))
    {
        cabinP->components.flap.value =
            cabinC->parameters.Flap.PositionA * 10;
    }
    else if (isFlapPositionB(cabinP->states.Flap))
    {
        cabinP->components.flap.value =
            cabinC->parameters.Flap.PositionB * 10;
    }
    else if (isFlapPositionC(cabinP->states.Flap))
    {
        cabinP->components.flap.value =
            cabinC->parameters.Flap.PositionC * 10;
    }
    else
    {
        cabinP->components.flap.value = 0;
    }
}
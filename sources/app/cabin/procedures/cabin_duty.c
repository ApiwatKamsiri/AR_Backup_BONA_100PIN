#include "cabin_duty.h"

void cabinDutyInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    cabinP->values.duty.delay = 0;
    cabinP->values.duty.index = 0;

    for (unsigned char i = 0; i < _DutyRaw_Size; i++)
    {
        cabinP->values.duty.coolTime[i] = cabinP->values.duty.meanCoolTime;
        cabinP->values.duty.idleTime[i] = cabinP->values.duty.meanIdleTime;
    }

    cabinP->values.duty.delay = 1; // For initial CabinDutyTask calling
    cabinDutyTask(consts, params);
    cabinP->values.cycleCounter = 0;
}

void cabinDutyTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    unsigned long meanCoolTime = 0;
    unsigned long meanIdleTime = 0;

    if (cabinP->values.duty.index >= _DutyRaw_Size)
        cabinP->values.duty.index = 0;

    if (cabinP->values.duty.delay)
    {
        cabinP->values.duty.delay--;
    }
    else
    {
        cabinP->values.duty.coolTime[cabinP->values.duty.index] = cabinP->stateTimers.Cool;
        cabinP->values.duty.idleTime[cabinP->values.duty.index] = cabinP->stateTimers.Idle;
        cabinP->values.duty.index++;
    }

    for (unsigned char i = 0; i < _DutyRaw_Size; i++)
    {
        meanCoolTime += cabinP->values.duty.coolTime[i];
        meanIdleTime += cabinP->values.duty.idleTime[i];
    }

    meanCoolTime = meanCoolTime / _DutyRaw_Size;
    meanIdleTime = meanIdleTime / _DutyRaw_Size;

    cabinP->values.duty.mean = (meanCoolTime * 100) / (meanCoolTime + meanIdleTime);
    cabinP->values.duty.meanCoolTime = meanCoolTime;
    cabinP->values.duty.meanIdleTime = meanIdleTime;

    cabinP->stateTimers.Cool = 0;
    cabinP->stateTimers.Idle = 0;

    if (cabinP->values.cycleCounter < 0xFF)
        cabinP->values.cycleCounter++;
}

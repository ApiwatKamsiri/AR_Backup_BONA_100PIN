#include "cabin_drainage.h"

bool cabinDrainageOut(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinP->stateTimers.Drainage >=
        cabinC->parameters.Drainage.MaxDrainageTime)
    {
        // CHECKPOINT
        eventPost(eProcessRefrigerator, eSysEventCalibrateAllStepperMotors, NULL, 0);
        return true;
    }
    return false;
}

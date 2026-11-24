#include "cabin_quick.h"

void cabinQuickControl(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	
	bool disableQuick = false;
    switch (cabinP->sets.work.Mode)
    {
        case _CabinMode_FF:
            if (cabinP->values.quickTimer >
                cabinC->parameters.Quick.timeFFMode)
                disableQuick = true;
        break;

        case _CabinMode_FRZ:
            if (cabinP->values.quickTimer >
                cabinC->parameters.Quick.timeFRZMode)
                disableQuick = true;
        break;

        default:
            disableQuick = true;
        break;
    }

    if (disableQuick)
    {
        cabinP->sets.user.functions.Quick = false;
    }

    if (cabinP->sets.user.functions.Quick)
    {
        if (cabinP->values.errSensors)
            cabinP->values.quickBlock = true;
    }
    else
    {
        cabinP->values.quickBlock = false;
        cabinP->values.quickDefrost = 0;
        cabinP->values.quickTimer = 0;
    }
}
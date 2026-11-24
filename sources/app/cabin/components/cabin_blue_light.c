#include "cabin_blue_light.h"

void cabinBlueLightTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    unsigned short blueLight = 0;

    if (!cabinP->sets.user.functions.Sabbath)
    {
        if (!cabinP->values.doorClosedTimer)
        {
            blueLight = 100; // 100%
        }
        else
        {
            if (!cabinP->values.doorsClosedState)
                blueLight = 100; // 100%
        }
    }

    cabinP->components.blueLight.value = blueLight;
    // cabinP->components.blueLight.step = BLUE_LIGHT_STEP;
}

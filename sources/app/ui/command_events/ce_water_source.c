#include "ce_water_source.h"
#include "system.h"

void ceWaterSourceRstFilter(stWaterSource_Flash const *waterSourceC,
                            stWaterSource_Ram *waterSourceP,
                            bool cmdReceived)
{
    if (!cmdReceived)
        return;

    if (waterSourceC->type.filter)
    {
        if (waterSourceP->values.errFilterEnabled)
        {
            waterSourceP->values.filterTimer = _FILTER_TIME;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);

            waterSourceP->E2.filterTimer = waterSourceP->values.filterTimer;
        }
    }
}

void ceWaterSourceErrFilter(stWaterSource_Flash const *waterSourceC,
                            stWaterSource_Ram *waterSourceP,
                            bool cmdReceived)
{
    if (!cmdReceived)
        return;

    if (waterSourceC->type.filter)
    {
        if (waterSourceP->values.errFilterEnabled)
        {
            waterSourceP->values.errFilterEnabled = false;
            waterSourceP->values.filterTimer = 0;
            sysBuzzerCmd(eSysBuzzerPlayEndMelody);
        }
        else
        {
            waterSourceP->values.errFilterEnabled = true;
            waterSourceP->values.filterTimer = _FILTER_TIME;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
        }

        waterSourceP->E2.errFilterEnabled = waterSourceP->values.errFilterEnabled;
        waterSourceP->E2.filterTimer = waterSourceP->values.filterTimer;
    }
}

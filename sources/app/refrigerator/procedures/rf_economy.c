#include "rf_economy.h"

void rfEconomyTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    bool Economy = true;

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        if (rfC->Cabin[i].type != _CabinType_NONE)
        {
            if (rfP->Cabin[i].sets.user.functions.Quick)
                Economy = false;

            if (rfP->Sets.user.functions.EcoExtra)
            {
                if (!rfP->Sets.work.functions.EcoExtra)
                {
                    if (rfP->Cabin[i].sets.user.FRZ != 0)
                        Economy = false;
                }
            }
            else
            {
                if (rfP->Cabin[i].sets.user.FRZ != 0)
                    Economy = false;
            }
        }
    }

    rfP->Sets.user.functions.Economy = Economy;
}

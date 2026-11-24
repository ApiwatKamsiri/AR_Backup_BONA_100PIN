#include "rf_rice.h"

void rfRiceTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    bool Rice = false;

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        if (rfC->Cabin[i].type != _CabinType_NONE)
        {
            if (rfP->Cabin[i].sets.user.functions.Rice)
                Rice = true;
        }
    }

    rfP->Sets.user.functions.Rice = Rice;
}

void rfRiceEnable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->Sets.user.functions.Rice = true;
}

void rfRiceDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->Sets.user.functions.Rice = false;
}

void rfRiceToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->Sets.user.functions.Rice = !rfP->Sets.user.functions.Rice;
}

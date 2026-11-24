#include "rf_silent.h"

void rfSilent1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    if (rfP->Values.silentTimer)
    {
        rfP->Values.silentTimer--;
        if (!(rfP->Values.silentTimer % 60))
            rfP->E2.silentTimer = rfP->Values.silentTimer;
    }
}

void rfSilentTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (!rfC->Sets.configs.functions.Silent)
    {
        rfP->Sets.user.functions.Silent = false;
        rfP->Sets.work.functions.Silent = false;
        rfP->Values.silentTimer = 0;
    }

    if (!rfP->Values.silentTimer)
    {
        if (rfP->Sets.user.functions.Silent)
        {
            rfSilentDisable(rfC, rfP);
            rfSilentStore(rfC, rfP);
        }
        rfP->Sets.work.functions.Silent = false;
    }

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        rfP->Cabin[i].sets.work.functions.Silent = rfP->Sets.work.functions.Silent;
        rfP->Cooler.parameters.silent = rfP->Sets.work.functions.Silent;
    }
}

void rfSilentEnable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->Sets.user.functions.Silent = true;
    rfP->Values.silentTimer = rfC->Parameters.silentTime;
}

void rfSilentDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->Sets.user.functions.Silent = false;
    rfP->Values.silentTimer = 0;
}

void rfSilentToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (rfP->Sets.user.functions.Silent)
        rfSilentDisable(rfC, rfP);
    else
        rfSilentEnable(rfC, rfP);
}

void rfSilentToWork(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->Sets.work.functions.Silent = rfP->Sets.user.functions.Silent;
}

void rfSilentStore(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->E2.Silent = rfP->Sets.user.functions.Silent;
    rfP->E2.silentTimer = rfP->Values.silentTimer;
}

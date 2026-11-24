#include "rf_eco_extra.h"

void rfEcoExtraInit(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (rfP->Sets.user.functions.EcoExtra)
        rfP->Values.ecoExtraTimer =
            rfC->Parameters.ecoExtraTime;
}

void rfEcoExtra1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    if (rfP->Values.ecoExtraTimer)
        rfP->Values.ecoExtraTimer--;
}

void rfEcoExtraTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (!rfC->Sets.configs.functions.EcoExtra || rfP->Cabin[1].sets.user.functions.Convertible || rfP->Cabin[1].sets.user.functions.CabinOff)
    {
        rfP->Sets.user.functions.EcoExtra = false;
        rfP->Sets.work.functions.EcoExtra = false;
        rfP->Values.ecoExtraTimer = 0;
    }

    if (rfP->Sets.user.functions.Vacation)
        rfP->Sets.user.functions.EcoExtra = false;

    // EBI: At sensor errors, eco extra will be active on display and deactive on mainboard.
    if (rfP->Sets.user.functions.EcoExtra)
    {
        if (rfP->Values.errors.sensorErrorsNumber)
            rfEcoExtraResetTimer(rfC, rfP);
    }
    else
    {
        rfP->Values.ecoExtraTimer = 0;
    }

    if (rfP->Sets.user.functions.EcoExtra &&
        !rfP->Values.ecoExtraTimer)
    {
        rfP->Sets.work.functions.EcoExtra = true;
    }
    else
        rfP->Sets.work.functions.EcoExtra = false;

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        if (rfP->Cabin[i].values.doorOpenedTimer)
            rfEcoExtraResetTimer(rfC, rfP);

        rfP->Cabin[i].sets.work.functions.EcoExtra =
            rfP->Sets.work.functions.EcoExtra;
    }
}

void rfEcoExtraEnable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    rfP->Sets.user.functions.EcoExtra = true;
    rfP->Values.ecoExtraTimer = rfC->Parameters.ecoExtraTime;
}

void rfEcoExtraDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->Sets.user.functions.EcoExtra = false;
    rfP->Values.ecoExtraTimer = 0;
}

void rfEcoExtraToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (rfP->Sets.user.functions.EcoExtra)
        rfEcoExtraDisable(rfC, rfP);
    else
        rfEcoExtraEnable(rfC, rfP);
}

void rfEcoExtraResetTimer(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (rfP->Sets.user.functions.EcoExtra)
        rfP->Values.ecoExtraTimer =
            rfC->Parameters.ecoExtraTime;
    else
        rfP->Values.ecoExtraTimer = 0;
}

void rfEcoExtraStore(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->E2.EcoExtra = rfP->Sets.user.functions.EcoExtra;
}

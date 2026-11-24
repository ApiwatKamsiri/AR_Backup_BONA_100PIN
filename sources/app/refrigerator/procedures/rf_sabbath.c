#include "rf_sabbath.h"
#include "rf_eco_extra.h"

void rfSabbath1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    if (rfP->Values.sabbathTimer)
    {
        rfP->Values.sabbathTimer--;
        if (!(rfP->Values.sabbathTimer % 60))
            rfP->E2.sabbathTimer = rfP->Values.sabbathTimer;
    }
}

void rfSabbathTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (!rfC->Sets.configs.functions.Sabbath)
    {
        rfP->Sets.user.functions.Sabbath = false;
        rfP->Sets.work.functions.Sabbath = false;
        rfP->Values.sabbathTimer = 0;
    }

    /* EBI: At errors, sabbath will be active on display and mainboard.
    if( params->rf->Values.errors.errorsNumber ||
        !rfP->Values.sabbathTimer ){
    */
    if (!rfP->Values.sabbathTimer)
    {
        if (rfP->Sets.user.functions.Sabbath)
        {
            rfSabbathDisable(rfC, rfP);
            rfSabbathStore(rfC, rfP);
        }
        rfP->Sets.work.functions.Sabbath = false;
    }

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        rfP->Cabin[i].sets.user.functions.Sabbath =
            rfP->Sets.user.functions.Sabbath;

        rfP->Cabin[i].sets.work.functions.Sabbath =
            rfP->Sets.work.functions.Sabbath;
        rfP->Dispenser.values.sabbathEnabled =
            rfP->Sets.user.functions.Sabbath;   
        /*rfP->IceCreamMachine.values.sabbathEnabled =
            rfP->Sets.user.functions.Sabbath;*/
        rfP->IceMachine.values.sabbathEnabled =
            rfP->Sets.user.functions.Sabbath;

        if (rfP->Sets.user.functions.Sabbath)
        {
            if (rfP->Sets.user.functions.EcoExtra)
            {
                rfEcoExtraDisable(rfC, rfP);
                rfEcoExtraStore(rfC, rfP);
            }

            if (rfP->Cabin[i].sets.user.functions.Quick ||
                rfP->Cabin[i].sets.user.functions.Vacation ||
                rfP->Cabin[i].sets.user.functions.Rice)
            {
                rfP->Cabin[i].sets.user.functions.Quick = false;
                rfP->Cabin[i].sets.user.functions.Vacation = false;
                rfP->Cabin[i].sets.user.functions.Rice = false;

                rfP->Cabin[i].E2.Vacation = false;
                rfP->Cabin[i].E2.Rice = false;
            }
        }
    }
}

void rfSabbathEnable(stRefrigerator_Flash const *rfC,
                     stRefrigerator_Ram *rfP)
{
    rfP->Sets.user.functions.Sabbath = true;
    rfP->Values.sabbathTimer = rfC->Parameters.sabbathTime;
}

void rfSabbathDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->Sets.user.functions.Sabbath = false;
    rfP->Values.sabbathTimer = 0;
}

void rfSabbathToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    if (rfP->Sets.user.functions.Sabbath)
        rfSabbathDisable(rfC, rfP);
    else
        rfSabbathEnable(rfC, rfP);
}

void rfSabbathToWork(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->Sets.work.functions.Sabbath = rfP->Sets.user.functions.Sabbath;
}

void rfSabbathStore(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    UNUSED(rfC);

    rfP->E2.Sabbath = rfP->Sets.user.functions.Sabbath;
    rfP->E2.sabbathTimer = rfP->Values.sabbathTimer;
}

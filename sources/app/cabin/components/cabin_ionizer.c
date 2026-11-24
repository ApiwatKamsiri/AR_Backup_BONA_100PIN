#include "cabin_ionizer.h"


#define _Ionizer_FirstOFFTime   (16 * 60)   // (saat*60) dk
#define _Ionizer_OFFTime        ( 8 * 60)   // (saat*60) dk
#define _Ionizer_ONTime         (10 * 60)   // (dk  *60) sec
#define _Ionizer_ONDelayTime    ( 2 * 60)   // (saat*60) dk

void cabinIonizerInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    cabinP->values.ionizer.state        = _IonState_OFF;
    cabinP->values.ionizer.offTimer     = _Ionizer_FirstOFFTime;
    cabinP->components.ionizer.value    = _IonizerValue_OFF;
}

void cabinIonizer1SecTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    if (cabinP->components.ionizer.value &&
        cabinP->values.ionizer.onTimer)
        cabinP->values.ionizer.onTimer--;
}

void cabinIonizer1MinTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    if (!cabinP->components.ionizer.value &&
        cabinP->values.ionizer.offTimer)
        cabinP->values.ionizer.offTimer--;
}

void cabinIonizerTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    enIonizerValues ionizer = _IonizerValue_OFF;

    switch (cabinP->values.ionizer.state)
    {
        case _IonState_OFF:
            if (!cabinP->values.ionizer.offTimer && !cabinP->values.doorsClosedState)
            {
                cabinP->values.ionizer.state = _IonState_ON;
                if (!cabinP->values.ionizer.onTimer)
                    cabinP->values.ionizer.onTimer = _Ionizer_ONTime;
            }
            else
                cabinP->values.ionizer.state = _IonState_OFF;
        break;

        case _IonState_ON:
            ionizer = _IonizerValue_ON;

            if ((!cabinP->values.doorClosedTimer) ||
                (!cabinP->components.fan.speedOrPWM))
            {
                cabinP->values.ionizer.state = _IonState_Delay;
            }

            if (!cabinP->values.ionizer.onTimer)
            {
                cabinP->values.ionizer.state = _IonState_OFF;
                cabinP->values.ionizer.offTimer = _Ionizer_OFFTime;
            }
        break;
        case _IonState_Delay:
            if ((cabinP->values.doorClosedTimer) &&
                (cabinP->components.fan.speedOrPWM))
            {
                cabinP->values.ionizer.state = _IonState_OFF;
                cabinP->values.ionizer.offTimer = _Ionizer_ONDelayTime;
            }
        break;

        default:
            cabinIonizerInit(consts, params);
        break;
    }

    cabinP->components.ionizer.value = ionizer;
}

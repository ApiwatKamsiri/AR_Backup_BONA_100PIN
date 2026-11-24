#include "cabin_violed.h"

void cabinVioLedInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    cabinP->components.violed.value = _VioLedValue_OFF;
    cabinP->values.violed.doorClosedModeFinish = false;
}

void cabinVioLed1SecTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    // 1 sn lik timer istenirse Cabin.c den aktif edilebiilir.
    if (cabinP->values.violed.maxDelayVTimer)
        cabinP->values.violed.maxDelayVTimer--;
}

void cabinVioLed1MinTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinC->type == _CabinType_FF)
    {
        if (!cabinP->components.violed.value &&
            cabinP->values.violed.offVTimer)
        {
            if (!(cabinP->values.violed.offVTimer % 360) && cabinC->components.violed.type != _VioLedType_NONE)
                params->rf->E2.offVTimer = cabinP->values.violed.offVTimer;

            cabinP->values.violed.offVTimer--;
        }

        if (cabinP->components.violed.value &&
            cabinP->values.violed.onVTimer)
        {
            if (!(cabinP->values.violed.onVTimer % 60) && cabinC->components.violed.type != _VioLedType_NONE)
                params->rf->E2.onVTimer = cabinP->values.violed.onVTimer;

            cabinP->values.violed.onVTimer--;
        }
    }
}

void cabinVioLedTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinC->type == _CabinType_FF)
    {
        enVioLedValues violed = _VioLedValue_OFF;

        switch (cabinP->values.violed.state)
        {
        case _VioLedState_OFF:
            if (!cabinP->values.violed.offVTimer)
            {
                cabinP->values.violed.state = _VioLedState_ON;
                params->rf->E2.Violed = cabinP->values.violed.state;
                if (!cabinP->values.violed.onVTimer)
                    cabinP->values.violed.onVTimer = cabinC->parameters.VioLed.maxOnTime;
            }

            if (!cabinP->values.doorsClosedState && cabinP->values.violed.doorClosedModeFinish)
            {
                cabinP->values.violed.state = _VioLedState_ON;
                params->rf->E2.Violed = cabinP->values.violed.state;
                cabinP->values.violed.onVTimer = cabinC->parameters.VioLed.maxOnTime;
                cabinP->values.violed.offVTimer = 0;
                cabinP->values.violed.doorClosedModeFinish = false;
            }
            break;

        case _VioLedState_ON:
            violed = _VioLedValue_ON;

            if (!cabinP->values.violed.onVTimer)
            {
                cabinP->values.violed.state = _VioLedState_OFF;
                params->rf->E2.Violed = cabinP->values.violed.state;
                cabinP->values.violed.offVTimer = cabinC->parameters.VioLed.maxOffTime;
            }

            if (cabinP->values.doorsClosedState)
            {
                cabinP->values.violed.state = _VioLedState_OFF;
                params->rf->E2.Violed = cabinP->values.violed.state;
                cabinP->values.violed.offVTimer = cabinC->parameters.VioLed.maxOffTime;

                cabinP->values.violed.doorClosedModeFinish = true;
            }

            if (!params->rf->Values.doorsClosed)
            {
                cabinP->values.violed.state = _VioLedState_Delay;
            }

            break;

        case _VioLedState_Delay:
            if (!params->rf->Values.doorsClosed)
            {
                cabinP->values.violed.state = _VioLedState_Delay;
                cabinP->values.violed.maxDelayVTimer = cabinC->parameters.VioLed.maxDelayTime;
            }
            else
            {
                cabinP->values.violed.state = _VioLedState_Delay;
                if (!cabinP->values.violed.maxDelayVTimer)
                {
                    cabinP->values.violed.offVTimer = 0;
                    cabinP->values.violed.state = _VioLedState_ON;
                }
            }
            break;

        default:
            cabinVioLedInit(consts, params);
            break;
        }
        cabinP->components.violed.value = violed;
    }
}

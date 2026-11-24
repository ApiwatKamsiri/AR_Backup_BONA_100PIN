/*
#include "CE_Service.h"
#include "Definitions.h"
#include "Components.h"
#include "Sensors.h"
#include "Parameters.h"
#include "WM_Service.h"
#include "Refrigerator.h"
#include "CMDCheck.h"
*/
#include "ce_service.h"
#include "system.h"


static unsigned char IncreaseByOne(unsigned char val, unsigned char maxValue);
static unsigned char DecreaseByOne(unsigned char val, unsigned char maxValue);

void ceServiceSetState(tsUiRam *uiP, enUI_ServiceStates to, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    uiP->states.service.state = to;
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceIncreaseSlideIndex(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiP->slides.slideIndex = IncreaseByOne(uiP->slides.slideIndex, uiP->slides.slideNumber);
    uiP->states.service.timer = 50;
}

void ceServiceDecreaseSlideIndex(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiP->slides.slideIndex = DecreaseByOne(uiP->slides.slideIndex, uiP->slides.slideNumber);
    uiP->states.service.timer = 50;
}

void ceServiceIncreaseComponentIndex(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return; // burasi servis testi component degistirme fonk.
    uiP->states.service.comps.index = IncreaseByOne(uiP->states.service.comps.index, uiP->states.service.comps.number);
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceDecreaseComponentIndex(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiP->states.service.comps.index = DecreaseByOne(uiP->states.service.comps.index, uiP->states.service.comps.number);
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceIncreaseSensorIndex(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiP->states.service.sens.index = IncreaseByOne(uiP->states.service.sens.index, uiP->states.service.sens.number);
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceDecreaseSensorIndex(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiP->states.service.sens.index = DecreaseByOne(uiP->states.service.sens.index, uiP->states.service.sens.number);
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceIncreaseParameterIndex(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiP->states.service.params.index = IncreaseByOne(uiP->states.service.params.index, uiP->states.service.params.number);
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceDecreaseParameterIndex(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiP->states.service.params.index = DecreaseByOne(uiP->states.service.params.index, uiP->states.service.params.number);
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceSelectedChange(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    uiP->states.service.flags.selectedChange = 1;
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceSpecialsChange(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiP->states.service.flags.specialsChange = 1;
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceAllOff(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiP->states.service.flags.allOff = 1;
    uiP->states.service.flags.specialsOnOff = 0;
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceServiceStart(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    eventPost(eProcessRefrigerator, eSysEventStartServiceMode, NULL, 0);
    uiP->password.stayTimer = 0;
}

void ceServiceExit(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    // WM_Service_StayTimer = 0;
    //  TODO Check
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
    // exiting service mode
    eventPost(eProcessRefrigerator, eSysEventCalibrateAllStepperMotors, NULL, 0);
    eventPost(eProcessRefrigerator, eSysEventStartProductionTestMode, NULL, 0);
    uiP->states.service.timer = 50;
    uiP->slides.slideIndex = 0;
}

static unsigned char IncreaseByOne(unsigned char val, unsigned char maxValue)
{
    val++;
    if (val >= maxValue)
        val = 0;
    return val;
}

static unsigned char DecreaseByOne(unsigned char val, unsigned char maxValue)
{
    val--;
    if (val >= maxValue)
    {
        if (maxValue)
            val = maxValue - 1;
        else
            val = 0;
    }
    return val;
}
#include "ui_password.h"
#include "dev/display_spi.h"
#include "../command_events/cmd_check.h"
#include "../command_events/ce_password.h"
#include "../command_events/ce_refrigerator.h"
#include "../command_events/ce_water_source.h"
#include "../command_events/ce_buzzer.h"
#include "../command_events/ce_service.h"
#include "../command_events/ce_distributer.h"
#include "../components/ui_display.h"
#include "../../mw/connectivity/remote_control.h"
#include "../../refrigerator/refrigerator.h"

#define PasswordLength 1
#define KeyLevelUp 9
#define KeyLevelLow 0
#define _UI_Password_StayTime 60 // Enter to distributor mode up to 60second

static bool uiIsPasswordEntered(tsUiProcParams *params);

void uiPassword1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    UNUSED(consts);
    if (uiP->password.stayTimer)
        uiP->password.stayTimer--;
}

void uiPasswordInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    UNUSED(consts);

    uiP->password.passwordEntered = 0;
    uiP->password.passwordKeyCount = PasswordLength;
    uiP->password.passwordKey = KeyLevelLow;

    uiP->password.state = _UI_PasswordState_Enter;
    uiP->password.stayTimer = _UI_Password_StayTime;
}

void uiPasswordResetStayTimer(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    UNUSED(consts);

    uiP->password.stayTimer = _UI_Password_StayTime;
}

void uiPasswordTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    switch (uiP->password.state)
    {
    case _UI_PasswordState_Enter:
        /*
        if (display_F.SubType == _DisplayType_DUMMY_v0)
        {
            uiP->errors.errorShowState++;
            for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
            {
                if (i == consts->rf->Parameters.displayData)
                {
                    if (uiP->errors.errorShowState < 12)
                        uiDspCabinData(consts, params, eDisplaySpecialsPassword, i, eDisplayCabinDataTypeSpecial);
                    else
                        uiDspCabinData(consts, params, uiP->password.passwordKey, i, eDisplayCabinDataTypeNumber);
                }
                else
                    uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }

            if (uiP->errors.errorShowState > 49)    uiP->errors.errorShowState = 0;
        }
        else
        */
        {
            for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
            {
                if (i == consts->rf->Parameters.displayData)
                    uiDspCabinData(consts, params, eDisplaySpecialsPassword, i, eDisplayCabinDataTypeSpecial);
                else if (i == consts->rf->Parameters.displayValue)
                    uiDspCabinData(consts, params, uiP->password.passwordKey, i, eDisplayCabinDataTypeNumber);
                else
                    uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }
        }

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspFunctionsOff(consts, params);
        break;

    default:
        uiP->password.state = _UI_PasswordState_Enter;
        break;
    }
}

void uiPasswordKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    switch (uiP->password.state)
    {
    case _UI_PasswordState_Enter:
        /*
        if (display_F.SubType == _DisplayType_DUMMY_v0)
            cePasswordIncreaseKey(isEvent(eDisplayEventSetCabin));
        else
        */
        cePasswordIncreaseKey(params, isEvent((teDisplayEvent)(eDisplayEventC0SetDown + consts->rf->Parameters.displayValue)));

        /*
        if (display_F.SubType == _DisplayType_DUMMY_v0)
            cePasswordAcceptKey(isEvent(eDisplayEventSelectCabin));
        else
        */
        cePasswordAcceptKey(params, isEvent((teDisplayEvent)(eDisplayEventC0SetDown + consts->rf->Parameters.displayData)));

        ceServiceStart(uiP, uiIsPassword(params, 0));
        ceDistributerStart(uiP, uiIsPassword(params, 7));
        ceFairModeStart(uiP, uiIsPassword(params, 8));
        
        ceWaterSourceErrFilter(&consts->rf->WaterSource,
                                 &params->rf->WaterSource,
                                 uiIsPassword(params, 2));

        ceRfSabbath(consts, params, uiIsPassword(params, 9));
#if (CONNECTIVITY != _NON_CONNECTED)
        // DD: PING TEST
        if (uiIsPassword(params, 5))
        {
          pingTestS = 1;
          params->rf->E2.pingTestState = pingTestS;
          wifiError_B1 = 0;
          wifiError_B2 = 0;
          params->rf->E2.wifiErrors = wifiError_B2;
          WiFiVariables.TestMode = 0;
          endofbandtest = 0;
          setWifiState(WIFI_OFF);
          WiFiVariables.Situation = 0;
          rcRequest(eRcRequestCloseConnection, FALSE);
          ceServiceStart(uiP, true);
          ceServiceExit(uiP,true);
        }
#endif
        break;
    }

    if (uiIsPasswordEntered(params))
    {
        uiP->password.stayTimer = 0;
        uiPasswordInit(consts, params);
    }

    ceBuzzer(isEvent(eDisplayEventBuzzer));
}

/***********************************************************************************************/


void uiPasswordIncreaseKey(tsUiProcParams *params)
{
    tsUiPasswordData *pw = &params->ui->password;

    if (pw->passwordKey < KeyLevelUp)
        pw->passwordKey++;
    else
        pw->passwordKey = KeyLevelLow;
}

void uiPasswordDecreaseKey(tsUiProcParams *params)
{
    tsUiPasswordData *pw = &params->ui->password;

    if (pw->passwordKey < KeyLevelUp)
        pw->passwordKey++;
    else
        pw->passwordKey = KeyLevelLow;
}

void uiPasswordAcceptKey(tsUiProcParams *params)
{
    tsUiPasswordData *pw = &params->ui->password;

    if (!pw->passwordKeyCount)
        return;

    pw->passwordEntered *= 10;
    pw->passwordEntered += pw->passwordKey;
    pw->passwordKey = KeyLevelLow;
    pw->passwordKeyCount--;
}


static bool uiIsPasswordEntered(tsUiProcParams *params)
{
    tsUiPasswordData *pw = &params->ui->password;

    return pw->passwordKeyCount ? false : true;
}

bool uiIsPassword(tsUiProcParams *params, unsigned short password)
{
    tsUiPasswordData *pw = &params->ui->password;

    if (uiIsPasswordEntered(params) && (pw->passwordEntered == password))
    {
        return true;
    }
    else
    {
        return false;
    }
}

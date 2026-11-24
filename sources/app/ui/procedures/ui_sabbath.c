#include "ui_sabbath.h"
#include "../components/ui_display.h"
#include "../command_events/ce_refrigerator.h"
#include "../command_events/cmd_check.h"
#include "ui_password.h"

void uiSabbathInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    params->ui->sabbathState = _UI_SabbathState_Active;
}

void uiSabbathTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    switch (uiP->sabbathState)
    {
    case _UI_SabbathState_Active:
    default:
        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
            uiDspCabinData(consts, params, eDisplayEventSabbath, i, eDisplayCabinDataTypeSpecial);

        uiDspIceCreamData(consts, params, eDisplayEventSabbath, eDisplayCabinDataTypeSpecial);

        uiP->dspPacket.funcs.ledEconomy = false;
        uiP->dspPacket.funcs.ledEcoExtra = false;
        uiP->dspPacket.funcs.ledVacation = false;
        uiP->dspPacket.funcs.ledIceOFF = false;
        uiP->dspPacket.funcs.ledIceOn = false;                  //APW : Add Ice making operation test in CT
        uiP->dspPacket.funcs.ledFastBottleCooling = false;      //APW : Add Ice making operation test in CT
        //uiP->dspPacket.funcs.ledAlert = false;        //APW : Disable for BONA
        uiP->dspPacket.funcs.ledLock = false;
        uiP->dspPacket.funcs.ledDispOff = false;
        uiP->dspPacket.funcs.ledFilter = false;
        uiP->dspPacket.funcs.ledConverter = false;

        // uiP->dspPacket.funcs.ledCabinsQuick = 0;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
        // uiP->dspPacket.funcs.ledCabin4Quick = 0x00;

        uiP->dspPacket.funcs.ledIceCream = 0;
        uiP->dspPacket.funcs.dimLevel = params->rf->Values.dispDim;
        // uiP->dspPacket.funcs.ledSilent = false;
        break;
    }
}

void uiSabbathKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    switch (params->ui->sabbathState)
    {
    case _UI_SabbathState_Active:
    default:
        ceRfSabbath(consts, params, (uiIsPassword(params, 9) || isEvent(eDisplayEventSabbath)));
        ceRfResetSabbath(consts, params, isEvent(eDisplayEventPassword));
        break;
    }
}

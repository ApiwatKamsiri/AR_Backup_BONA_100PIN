#include "ui_distributer_state.h"
#include "../procedures/ui_password.h"
#include "../procedures/ui_sabbath.h"
#include "../components/ui_buzzer.h"
#include "../components/ui_display.h"
#include "../command_events/ce_distributer.h"
#include "../command_events/ce_display.h"
#include "../command_events/ce_buzzer.h"
#include "../command_events/cmd_check.h"
#include "../../mw/connectivity/remote_control.h"
#include "../ui_proc.h"
#include "../command_events/ce_cabin.h"
#include "../command_events/ce_ice_machine.h"
#include "../command_events/ce_dispenser.h"
#include "../command_events/ce_refrigerator.h"
#include "../command_events/ce_water_source.h"
#include "../command_events/ce_service.h"

extern tsUiProcParams uiProcParams;
extern tsUiProcConsts uiProcConsts;

void uiDistributerState20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    UNUSED(consts);

    if (uiP->states.distributer.timer)
        uiP->states.distributer.timer--;
#ifdef HOSHI_H30
    if (uiP->states.distributer.DemoDisplaytimer)	//Demo Display
      uiP->states.distributer.DemoDisplaytimer--;
#endif //HOSHI_H30       
    
    if(params->rf->Cabin[0].values.selectedColorIndexSetTime)
      params->rf->Cabin[0].values.selectedColorIndexSetTime--;
    
    if(params->rf->Cabin[1].values.selectedColorIndexSetTime)
      params->rf->Cabin[1].values.selectedColorIndexSetTime--;
}

void uiDistributerStateInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    uiP->states.distributer.state = _UI_DistributerState_AllLedsOn;
    uiP->states.distributer.timer = _UI_Distributer_AllLedsOn_Time;
    uiPasswordInit(consts, params);
    refrigerator_R.Values.doorsAlertAndDisplay = true;
}

void uiDistributerStateTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    uiP->dspPacket.funcs.displayOff = false;
    uiPasswordResetStayTimer(consts, params);

    if ((!params->rf->Values.doorsClosed) 
       || (params->rf->Dispenser.components.tactSwitch.value))
        uiResetDisplayOffTimer(uiP);

    switch (uiP->states.distributer.state)
    {
    case _UI_DistributerState_AllLedsOn_Init:
        uiP->states.distributer.timer = _UI_Distributer_AllLedsOn_Time;
        uiP->states.distributer.state = _UI_DistributerState_AllLedsOn;
    case _UI_DistributerState_AllLedsOn:
        uiDspAllOn(consts, params);
        uiP->dspPacket.funcs.dimLevel = eDisplayDimLevel100;
        if (!uiP->states.distributer.timer)
        {
            // if (display_F.Type == _DisplayType_CLEVER)
            uiP->states.distributer.state = _UI_DistributerState_DispVersionRevision_Init;
            // else
            //     uiP->states.distributer.state = _UI_DistributerState_VersionRevision_Init;
        }

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_DistributerState_DispVersionRevision_Init:
        uiP->states.distributer.state = _UI_DistributerState_DispVersionRevision;
        uiP->states.distributer.timer = _UI_Distributer_VersionRevision_Time;

    case _UI_DistributerState_DispVersionRevision: // EzgiKutlu: Distributer mode show display version revision
        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        {
            if (i == consts->rf->Parameters.displayData)
            {
                uiDspCabinData(consts, params, uiP->dspPacket.displayInfo.swVer, i, eDisplayCabinDataTypeNumber);
            }
            else if (i == consts->rf->Parameters.displayValue)
            {
                uiDspCabinData(consts, params, uiP->dspPacket.displayInfo.swRev, i, eDisplayCabinDataTypeNumber);
            }
            else
            {
                uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }
        }

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);

        uiP->dspPacket.funcs.ledEconomy = /*(FTC.tests.FTC == 0xAA) ? true :*/ false;
        uiP->dspPacket.funcs.ledEcoExtra = /*(FTC.tests.ICT == 0xAA) ? true :*/ false;
        uiP->dspPacket.funcs.ledVacation = false;
        uiP->dspPacket.funcs.ledIceOFF = false;
        uiP->dspPacket.funcs.ledIceOn = false;                  //APW : Add Ice making operation test in CT
        uiP->dspPacket.funcs.ledFastBottleCooling = false;      //APW : Add Ice making operation test in CT

        //uiP->dspPacket.funcs.ledAlert = params->rf->workMode.values.fairModeFlag ? false : true;        //APW : Disable for BONA

        /*if (!consts->rf->Parameters.fairMode)
        {
            uiP->dspPacket.funcs.ledLock = false;
            uiP->dspPacket.funcs.ledAlert = true;
        }
        else
            uiP->dspPacket.funcs.ledAlert = false;*/

        uiP->dspPacket.funcs.ledLock = false;

        uiP->dspPacket.funcs.ledDispOff = false;
        uiP->dspPacket.funcs.ledFilter = (params->rf->WaterSource.values.errFilterEnabled) ? true : false;
        uiP->dspPacket.funcs.ledConverter = false;

        // uiP->dspPacket.funcs.ledCabinsQuick = 0;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
        // uiP->dspPacket.funcs.ledCabin4Quick = 0x00;

        uiP->dspPacket.funcs.ledIceCream = 0;
        uiP->dspPacket.funcs.ledCabin0Icon = 0;
        uiP->dspPacket.funcs.ledCabin1Icon = 0;
        uiP->dspPacket.funcs.ledCabin2Icon = 0;
        uiP->dspPacket.funcs.ledCabin3Icon = 0;
        uiP->dspPacket.funcs.ledCabin4Icon = 0;

        uiP->dspPacket.funcs.ledWifi = false;
        // uiP->dspPacket.funcs.ledSilent = false;
        uiDspDispenser(consts, params);
        uiP->dspPacket.funcs.dimLevel = eDisplayDimLevel100;

        if (!uiP->states.distributer.timer)
            uiP->states.distributer.state = _UI_DistributerState_VersionRevision_Init;

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;
    case _UI_DistributerState_VersionRevision_Init:
        uiP->states.distributer.state = _UI_DistributerState_VersionRevision;
        uiP->states.distributer.timer = _UI_Distributer_VersionRevision_Time;

    case _UI_DistributerState_VersionRevision:
        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; i++)
        {
            if (i == consts->rf->Parameters.displayData)
            {
                uiDspCabinData(consts, params, consts->rf->Stock.Version, i, eDisplayCabinDataTypeNumber);
            }
            else if (i == consts->rf->Parameters.displayValue)
            {
                uiDspCabinData(consts, params, consts->rf->Stock.Revision, i, eDisplayCabinDataTypeNumber);
            }
            else
            {
                uiDspCabinData(consts, params, eDisplaySpecialsAllOff, i, eDisplayCabinDataTypeSpecial);
            }
        }

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);

        uiP->dspPacket.funcs.ledEconomy = /*(FTC.tests.FTC == 0xAA) ? true :*/ false;
        uiP->dspPacket.funcs.ledEcoExtra = /*(FTC.tests.ICT == 0xAA) ? true :*/ false;
        uiP->dspPacket.funcs.ledVacation = false;
        uiP->dspPacket.funcs.ledIceOFF = false;
        uiP->dspPacket.funcs.ledIceOn = false;                  //APW : Add Ice making operation test in CT
        uiP->dspPacket.funcs.ledFastBottleCooling = false;      //APW : Add Ice making operation test in CT             
        //uiP->dspPacket.funcs.ledAlert = params->rf->workMode.values.fairModeFlag ? false : true;      //APW : Disable for BONA
        //uiP->dspPacket.funcs.ledAlert = true;         //APW : Disable for BONA
        uiP->dspPacket.funcs.ledLock = false;
        uiP->dspPacket.funcs.ledDispOff = false;
        uiP->dspPacket.funcs.ledFilter = (params->rf->WaterSource.values.errFilterEnabled) ? true : false;
        uiP->dspPacket.funcs.ledConverter = false;
        uiP->dspPacket.funcs.ledServiceButton = false;

        // uiP->dspPacket.funcs.ledCabinsQuick = 0;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
        // uiP->dspPacket.funcs.ledCabin4Quick = 0x00;

        uiP->dspPacket.funcs.ledIceCream = 0;
        uiP->dspPacket.funcs.ledCabin0Icon = 0;
        uiP->dspPacket.funcs.ledCabin1Icon = 0;
        uiP->dspPacket.funcs.ledCabin2Icon = 0;
        uiP->dspPacket.funcs.ledCabin3Icon = 0;
        uiP->dspPacket.funcs.ledCabin4Icon = 0;

        uiP->dspPacket.funcs.ledWifi = false;
        // uiP->dspPacket.funcs.ledSilent = false;
        uiDspDispenser(consts, params);
        uiP->dspPacket.funcs.dimLevel = eDisplayDimLevel100;

        if (!uiP->states.distributer.timer)
            uiP->states.distributer.state = _UI_DistributerState_Sets_Init;

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_DistributerState_TempUnit_Init:
        uiP->states.distributer.timer = _UI_Distributer_TempUnit_Time;
        uiP->states.distributer.state = _UI_DistributerState_TempUnit;
    case _UI_DistributerState_TempUnit:
        uiDspTempUnit(consts, params);
        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);

        uiP->dspPacket.funcs.ledEconomy = false;
        uiP->dspPacket.funcs.ledEcoExtra = false;
        uiP->dspPacket.funcs.ledVacation = false;
        uiP->dspPacket.funcs.ledIceOFF = false;
        uiP->dspPacket.funcs.ledIceOn = false;                  //APW : Add Ice making operation test in CT
        uiP->dspPacket.funcs.ledFastBottleCooling = false;      //APW : Add Ice making operation test in CT
        //uiP->dspPacket.funcs.ledAlert = false;  //APW : Disable for BONA
        uiP->dspPacket.funcs.ledLock = false;
        uiP->dspPacket.funcs.ledDispOff = false;
        uiP->dspPacket.funcs.ledFilter = false;
        uiP->dspPacket.funcs.ledConverter = false;
        uiP->dspPacket.funcs.ledServiceButton = false;

        // uiP->dspPacket.funcs.ledCabinsQuick = 0;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
        // uiP->dspPacket.funcs.ledCabin4Quick = 0x00;

        uiP->dspPacket.funcs.ledIceCream = 0;
        uiP->dspPacket.funcs.ledCabin0Icon = 0;
        uiP->dspPacket.funcs.ledCabin1Icon = 0;
        uiP->dspPacket.funcs.ledCabin2Icon = 0;
        uiP->dspPacket.funcs.ledCabin3Icon = 0;
        uiP->dspPacket.funcs.ledCabin4Icon = 0;

            if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
            { // Connected functionality active?
                uiP->dspPacket.funcs.ledWifi = WiFiLed.state;
            }
            else
            {
                uiP->dspPacket.funcs.ledWifi = false;
            }

        uiDspDispenser(consts, params);
        uiP->dspPacket.funcs.dimLevel = params->rf->Values.dispDim;

        if (!uiP->states.distributer.timer)
            uiP->states.distributer.state = _UI_DistributerState_Sets_Init;

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_DistributerState_Sets_Init:
        uiResetDisplayOffTimer(uiP);
        uiP->states.distributer.timer = _UI_Distributer_Sets_Time;
        uiP->states.distributer.state = _UI_DistributerState_Sets;
    case _UI_DistributerState_Sets:
        uiDspCabinsSets(consts, params);
        //uiP->dspPacket.funcs.ledAlert = params->rf->workMode.values.fairModeFlag ? false : true;      //APW : Disable for BONA

        uiDspCabinsQuicks(consts, params);
        uiDspCabinIcon(consts, params);

        // show enabled functions
#ifdef HOSHI_H30
        if(uiP->timers.dspOffTimer) //Demo Display
        {
          uiP->dspPacket.funcs.ledEconomy = params->rf->Sets.user.functions.Economy;
          uiP->dspPacket.funcs.ledEcoExtra = params->rf->Sets.user.functions.EcoExtra;
          uiP->dspPacket.funcs.ledVacation = params->rf->Sets.user.functions.Vacation;
          uiDspVacuumBox(consts, params);
          uiDspIceOff(consts, params);
          uiDspKeyLock(consts, params);
          uiP->dspPacket.funcs.ledDispOff = false;
        }
#else
        uiP->dspPacket.funcs.ledEconomy = params->rf->Sets.user.functions.Economy;
        uiP->dspPacket.funcs.ledEcoExtra = params->rf->Sets.user.functions.EcoExtra;
        uiP->dspPacket.funcs.ledVacation = params->rf->Sets.user.functions.Vacation;
        uiDspVacuumBox(consts, params);
        uiDspIceOff(consts, params);
        uiDspKeyLock(consts, params);
        uiP->dspPacket.funcs.ledDispOff = false;
#endif
        uiP->dspPacket.funcs.ledFilter = params->rf->WaterSource.values.errFilterTime;
        uiP->dspPacket.funcs.ledConverter = params->rf->Cabin[1].sets.user.functions.Convertible || params->rf->Cabin[1].sets.user.functions.CabinOff;
        uiP->dspPacket.funcs.ledServiceButton = uiP->serviceButton.led.serviceButton;

        if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
        { // Connected functionality active?
            uiP->dspPacket.funcs.ledWifi = WiFiLed.state;
        }
        else
        {
            uiP->dspPacket.funcs.ledWifi = false;
        }

        uiDspDispenser(consts, params);
        uiP->dspPacket.funcs.dimLevel = params->rf->Values.dispDim;

        if (params->rf->Values.dispOff && !uiP->timers.dspOffTimer)
            uiP->states.distributer.state = _UI_DistributerState_DisplayOff_Init;
        if (!uiP->states.distributer.timer && consts->rf->Parameters.realTemp)
            uiP->states.distributer.state = _UI_DistributerState_RealTemp_Init;
        if (params->rf->Sets.user.functions.Sabbath)
            uiP->states.distributer.state = _UI_DistributerState_Sabbath;

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_DistributerState_RealTemp_Init:
        uiResetDisplayOffTimer(uiP);
        uiP->states.distributer.state = _UI_DistributerState_RealTemp;
    case _UI_DistributerState_RealTemp:
        uiDspCabinsRealTemps(consts, params);
        uiDspCabinsQuicks(consts, params);
        uiDspCabinIcon(consts, params);
        // show enabled functions
#ifdef HOSHI_H30
        if(uiP->timers.dspOffTimer) //Demo Display
        {
          uiP->dspPacket.funcs.ledEconomy = params->rf->Sets.user.functions.Economy;
          uiP->dspPacket.funcs.ledEcoExtra = params->rf->Sets.user.functions.EcoExtra;
          uiP->dspPacket.funcs.ledVacation = params->rf->Sets.user.functions.Vacation;
          uiDspVacuumBox(consts, params);
          uiDspIceOff(consts, params);
        }
#else
        uiP->dspPacket.funcs.ledEconomy = params->rf->Sets.user.functions.Economy;
        uiP->dspPacket.funcs.ledEcoExtra = params->rf->Sets.user.functions.EcoExtra;
        uiP->dspPacket.funcs.ledVacation = params->rf->Sets.user.functions.Vacation;
        uiDspVacuumBox(consts, params);
        uiDspIceOff(consts, params);
#endif
        /*if (!consts->rf->Parameters.fairMode)
            uiP->dspPacket.funcs.ledAlert = true;
        else
            uiP->dspPacket.funcs.ledAlert = false;*/
        //uiP->dspPacket.funcs.ledAlert = params->rf->workMode.values.fairModeFlag ? false : true;      //APW : Disable for BONA

        uiDspKeyLock(consts, params);
        uiP->dspPacket.funcs.ledDispOff = false;
        uiP->dspPacket.funcs.ledFilter = params->rf->WaterSource.values.errFilterTime;
        uiP->dspPacket.funcs.ledConverter = false;
        uiP->dspPacket.funcs.ledServiceButton = false;

        if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
        { // Connected functionality active?
            uiP->dspPacket.funcs.ledWifi = WiFiLed.state;
        }
        else
        {
            uiP->dspPacket.funcs.ledWifi = false;
        }

        uiDspDispenser(consts, params);
        uiP->dspPacket.funcs.dimLevel = params->rf->Values.dispDim;

        if (!consts->rf->Parameters.realTemp)
            uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
        if (params->rf->Values.dispOff && !uiP->timers.dspOffTimer)
            uiP->states.distributer.state = _UI_DistributerState_DisplayOff_Init;
        if (params->rf->Sets.user.functions.Sabbath)
            uiP->states.distributer.state = _UI_DistributerState_Sabbath;

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_DistributerState_DisplayOff_Init:
        uiP->states.distributer.state = _UI_DistributerState_DisplayOff;
    case _UI_DistributerState_DisplayOff:
        uiP->dspPacket.funcs.displayOff = true;
        uiDspIceCabinsSetsOff(consts, params);
        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspFunctionsAtOffMode(consts, params);
        uiDspKeyLock(consts, params);

       /* if (params->rf->IceCreamMachine.values.state ==
            _IceCreamMachineState_IceCreamIsMade)
            uiP->states.distributer.state = _UI_DistributerState_Sets_Init;*/

        uiP->dspPacket.funcs.dimLevel = params->rf->Values.dispDim;
#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)

        if (!params->rf->Values.doorsClosed)
        {
            uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
        }
        if (params->rf->Dispenser.components.tactSwitch.value)
        {
            uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
        }
        break;
    case _UI_DistributerState_Password:
        uiPasswordTask(consts, params);
#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        if (!uiP->password.stayTimer)
            uiP->states.distributer.state = _UI_DistributerState_AllLedsOn_Init;
        break;
    case _UI_DistributerState_Sabbath:
        uiSabbathTask(consts, params);

        if (!params->rf->Sets.user.functions.Sabbath)
            uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
        break;

    case _UI_DistributerState_WifiConfig:
        /*uiDspCabinData(consts, params, WA[Pass2] * 10 + WA[Pass3], 0, eDisplayCabinDataTypeNumber);
        uiDspCabinData(consts, params, WA[Pass0] * 10 + WA[Pass1], 1, eDisplayCabinDataTypeNumber);*/
        for (unsigned char i = 0; i < DSP_USED_CABINS_COUNT; ++i)
            uiDspCabinQuickLED(consts, params, i, eDisplayLedStateOff);
        uiP->dspPacket.funcs.ledEconomy = false;
        uiP->dspPacket.funcs.ledEcoExtra = false;
        uiP->dspPacket.funcs.ledVacation = false;
        //uiP->dspPacket.funcs.ledAlert = false;        //APW : Disable for BONA
        uiP->dspPacket.funcs.ledLock = false;
        uiP->dspPacket.funcs.ledFilter = false;

        // uiP->dspPacket.funcs.ledCabinsQuick = 0;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin2Quick = 0x00;
        uiP->dspPacket.funcs.ledCabin3Quick = 0x00;
        // uiP->dspPacket.funcs.ledCabin4Quick = 0x00;

        uiP->dspPacket.funcs.ledCabin0Icon = 0;
        uiP->dspPacket.funcs.ledCabin1Icon = 0;
        uiP->dspPacket.funcs.ledCabin2Icon = 0;
        uiP->dspPacket.funcs.ledCabin3Icon = 0;
        uiP->dspPacket.funcs.ledCabin4Icon = 0;

        uiP->dspPacket.funcs.ledWifi = WiFiLed.state;
        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    default:
        uiP->states.distributer.state = _UI_DistributerState_AllLedsOn_Init;
        break;
    }
}

void uiDistributerStateKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

#ifdef HOSHI_H30
    if (!params->rf->Values.keyLock)
    {
        ceServiceStart(uiP, isEvent(eDisplayEventServiceMode));
    }
#endif  //HOSHI H30 and ALPS H10/30

    /*
  if( params->rf->Values.doorsAlert &&
      ( DisplayData.DPCommand != eDisplayEventBuzzer )&&
      !isEvent( eDisplayEventNone ) ){
    DisplayData.DPCommand = eDisplayEventNone;
    params->rf->Values.doorsAlertDisable = true;
  }
  */
    if (!params->rf->Values.keyLock)
        ceDistributerExit(isEvent(eDisplayEventPassword));

    // Door Alarm
#ifdef HOSHI_H30
    if (params->rf->Values.doorsAlert && !refrigerator_R.Values.prohibitionAlertDisplay)
    {
        if (!isEvent(eDisplayEventNone) && !isEvent(eDisplayEventBuzzer))
        {
            clearEvent();
        }
    }
#else
    if (params->rf->Values.doorsAlert)
    {
        if (!isEvent(eDisplayEventNone) &&
            !isEvent(eDisplayEventBuzzer))
        {
            clearEvent();
            params->rf->Values.doorsAlertDisable = true;
        }
    }
#endif

    if (!params->rf->Sets.user.functions.Sabbath)
    {
      if (params->rf->Values.keyLock)
      {
        if (!isEvent(eDisplayEventNone) && !isEvent(eDisplayEventBuzzer) && !isEvent(eDisplayEventKeyLock) && 
            !isEvent(eDisplayEventIceCube) && !isEvent(eDisplayEventIceCrush) && !isEvent(eDisplayEventMeasuredFill) && 
              !isEvent(eDisplayEventWater) && !isEvent(eDisplayEventIceCreamMchOnOff) )
        {
          sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
          uiP->timers.keyLockWarningCounter = _UI_KeyLockWarningCount;
        }
      }
      else
      {
        ceBuzzer(isEvent(eDisplayEventBuzzer));
      }
    }
    
    if (!params->rf->Sets.user.functions.Sabbath)
    {
      if (params->rf->Values.keyLockDspr)
      {
        if (!isEvent(eDisplayEventNone) && !isEvent(eDisplayEventBuzzer) && !isEvent(eDisplayEventKeyLock) && 
            !(!isEvent(eDisplayEventIceCube) && !isEvent(eDisplayEventIceCrush) && !isEvent(eDisplayEventMeasuredFill) && 
              !isEvent(eDisplayEventWater) && !isEvent(eDisplayEventIceCreamMchOnOff)))
        {
          sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
          uiP->timers.keyLockWarningCounter = _UI_KeyLockWarningCount;
        }
      }
      else
      {
        ceBuzzer(isEvent(eDisplayEventBuzzer));
      }
    }    

    switch (uiP->states.distributer.state)
    {
    case _UI_DistributerState_AllLedsOn_Init:
    case _UI_DistributerState_AllLedsOn:
    case _UI_DistributerState_DispVersionRevision_Init:
    case _UI_DistributerState_DispVersionRevision:
    case _UI_DistributerState_VersionRevision_Init:
    case _UI_DistributerState_VersionRevision:
    case _UI_DistributerState_TempUnit_Init:
    case _UI_DistributerState_TempUnit:
        ceDisplayKeyLock(params, isEvent(eDisplayEventKeyLock));
        ceDisplayKeyLockDspr(params, isEvent(eDisplayEventIceCreamMchOnOff));
        
        if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
        { // Connected functionality active?
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            { // Swap buttons to match Connected device layout?
                if (isEvent(eDisplayEventDispOff /*wifi_OnOff*/))
                    toggleWifiState(WIFI_TOGGLE);
            }
            else
            {
                if (isEvent(eDisplayEventWifiStartStop) || params->rf->Values.WifiStatus == WIFI_CONNECTED)
                {
                    toggleWifiState(WIFI_TOGGLE);
                    params->rf->Values.WifiStatus = 0;
                }
            }
            if (isEvent(eDisplayEventWifiConfig))
                toggleWifiState(WIFI_CONFIG_TOGGLE);
        }
        if (!params->rf->Values.keyLock)
            ceDistributerSetState(uiP, _UI_DistributerState_Password, isEvent(eDisplayEventPassword));
        break;

    case _UI_DistributerState_DisplayOff_Init:
    case _UI_DistributerState_DisplayOff:
        /*
    for( unsigned char i = eDisplayEventC0_SetUp;
        i < eDisplayEventIceCreamMchOnOff;
        i++ ){
    ceDistributerSetState(uiP,  _UI_DistributerState_Sets_Init,
                                isEvent( ( teDisplayEvent ) i ) );
    }
    */
        ceDistributerSetState(uiP, _UI_DistributerState_Sets_Init,
                                 (!isEvent(eDisplayEventNone) && !isEvent(eDisplayEventBuzzer)));
        break;

    case _UI_DistributerState_RealTemp_Init:
    case _UI_DistributerState_RealTemp:
        ceDisplayKeyLock(params, isEvent(eDisplayEventKeyLock));
        ceDisplayKeyLockDspr(params, isEvent(eDisplayEventIceCreamMchOnOff));
        
        if (!params->rf->Values.keyLockDspr)
        {
          ceDispenserNextState(consts, params, &consts->rf->Dispenser, &params->rf->Dispenser, (isEvent(eDisplayEventDispencer)||isEvent(eDisplayEventWater)||isEvent(eDisplayEventIceCrush)||isEvent(eDisplayEventIceCube)|| isEvent(eDisplayEventMeasuredFill)||isEvent(eDisplayEventCalibrateCustom)), (unsigned char *)(&params->rf->ui.dspValues.receivedEvent));            // Swap buttons to match Connected device layout?
        }

        if (!params->rf->Values.keyLock)
        {
            if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
            { // Connected functionality active?
                if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
                { // Swap buttons to match Connected device layout?
                    if (isEvent(eDisplayEventDispOff /*wifi_OnOff*/))
                        toggleWifiState(WIFI_TOGGLE);
                }
                else
                {
                    if (isEvent(eDisplayEventWifiStartStop) || params->rf->Values.WifiStatus == WIFI_CONNECTED)
                    {
                        toggleWifiState(WIFI_TOGGLE);
                        params->rf->Values.WifiStatus = 0;
                    }
                }
                if (isEvent(eDisplayEventWifiConfig))
                    toggleWifiState(WIFI_CONFIG_TOGGLE);
            }
            for (unsigned char i = 0; i < _NumOfCabin; i++)
            {
                ceDistributerSetState(uiP, _UI_DistributerState_Sets_Init,
                                         isEvent((teDisplayEvent)(eDisplayEventC0SetUp + i)));

                ceDistributerSetState(uiP, _UI_DistributerState_Sets_Init,
                                         isEvent((teDisplayEvent)(eDisplayEventC0SetDown + i)));

                ceCabinQuick(&uiProcConsts,
                             &uiProcParams,
                             &consts->rf->Cabin[i],
                             &params->rf->Cabin[i],
                             isEvent((teDisplayEvent)(eDisplayEventC0QCool + i)));

                ceDistributerSetState(uiP, _UI_DistributerState_Sets_Init,
                                         isEvent((teDisplayEvent)(eDisplayEventC0Mode + i)));
            }
            ceQuickIce60OnOff(consts, params, &consts->rf->IceMachine, &params->rf->IceMachine, isEvent(eDisplayEventQuickIce60));
            ceIceMachineOnOff(&uiProcConsts,
                              &uiProcParams,
                              &consts->rf->IceMachine, &params->rf->IceMachine, isEvent(eDisplayEventIceOn));
            // Swap buttons to match Connected device layout?
            
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            {                 
              ceDisplayOff(params, isEvent(eDisplayEventVacation /*eDisplayEventDispOff*/));
            }
            else
            {
              ceDisplayOff(params, isEvent(eDisplayEventDispOff));
            }

            ceDisplaySetDim(params, isEvent(eDisplayEventDispDimmer));
            ceRfTempUnitChange(consts, params, isEvent(eDisplayEventTempUnitChange));
            ceRfEcoExtra(consts, params, isEvent(eDisplayEventEcoExtra));               //APW : For BONA/ALPS
            ceRfEcoExtra(consts, params, isEvent(eDisplayEventEcoWhirlpool));           //APW : For HOSHI
            ceCabinChangeFRZMode(consts, params, consts->rf, params->rf, isEvent(eDisplayEventConverterFRZtoFF));
#ifdef HOSHI_H30
            ceRfTempSettingFine(consts, params, isEvent(eDisplayEventTempSettingFine));
            ceRfMuteBuzzer(consts, params, isEvent(eDisplayEventMuteBuzzer));
            //ceRfDemoMode(consts, params, isEvent(eDisplayEventDemoMode));
#endif //HOSHI_H30   
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            { // Swap buttons to match Connected device layout?
                ceRfVacation(consts, params, isEvent(eDisplayEventStateMenu /*eDisplayEventVacation*/));
            }
            else
            {
                ceRfVacation(consts, params, isEvent(eDisplayEventVacation));
            }

            ceRfRice(consts, &uiProcParams, isEvent(eDisplayEventRice));
            if (!params->rf->Values.keyLock)
                ceDistributerSetState(uiP, _UI_DistributerState_Password, isEvent(eDisplayEventPassword));
            uiSabbathKeyHandler(consts, params);
            ceWaterSourceRstFilter(&consts->rf->WaterSource, &params->rf->WaterSource, isEvent(eDisplayEventFilterReset));
            //ceRfApplianceOff(consts, params, isEvent(eDisplayEventApplianceOFF));     //APW Don't need
        }
        ceDisplayServiceButton(consts, params, isEvent(eDisplayEventServiceButton));
        break;
    case _UI_DistributerState_Sets_Init:
    case _UI_DistributerState_Sets:
        ceDisplayKeyLock(params, isEvent(eDisplayEventKeyLock));
        ceDisplayKeyLockDspr(params, isEvent(eDisplayEventIceCreamMchOnOff));
        if (!params->rf->Values.keyLockDspr)
        {
          ceDispenserNextState(consts, params, &consts->rf->Dispenser, &params->rf->Dispenser, (isEvent(eDisplayEventDispencer)||isEvent(eDisplayEventWater)||isEvent(eDisplayEventIceCrush)||isEvent(eDisplayEventIceCube)|| isEvent(eDisplayEventMeasuredFill)||isEvent(eDisplayEventCalibrateCustom)), (unsigned char *)(&params->rf->ui.dspValues.receivedEvent));            // Swap buttons to match Connected device layout?
        }

        if (!params->rf->Values.keyLock)
        {

            if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
            { // Connected functionality active?
                if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
                { // Swap buttons to match Connected device layout?
                    if (isEvent(eDisplayEventDispOff /*wifi_OnOff*/))
                        toggleWifiState(WIFI_TOGGLE);
                }
                else
                {
                    if (isEvent(eDisplayEventWifiStartStop) || params->rf->Values.WifiStatus == WIFI_CONNECTED)
                    {
                        toggleWifiState(WIFI_TOGGLE);
                        params->rf->Values.WifiStatus = 0;
                    }
                }
                if (isEvent(eDisplayEventWifiConfig))
                    toggleWifiState(WIFI_CONFIG_TOGGLE);
            }

            for (unsigned char i = 0; i < _NumOfCabin; i++)
            {
                // ceSetUp has not defined yet...

                ceCabinSetDown(consts,
                               params,
                               &consts->rf->Cabin[i],
                               &params->rf->Cabin[i],
                               isEvent((teDisplayEvent)(eDisplayEventC0SetDown + i)));

                ceCabinQuick(consts,
                             params,
                             &consts->rf->Cabin[i],
                             &params->rf->Cabin[i],
                             isEvent((teDisplayEvent)(eDisplayEventC0QCool + i)));

                ceCabinMode(consts,
                            params,
                            &consts->rf->Cabin[i],
                            &params->rf->Cabin[i],
                            isEvent((teDisplayEvent)(eDisplayEventC0Mode + i)));
            }
            ceQuickIce60OnOff(consts, params, &consts->rf->IceMachine, &params->rf->IceMachine, isEvent(eDisplayEventQuickIce60));
            ceIceMachineOnOff(consts, params, &consts->rf->IceMachine, &params->rf->IceMachine, isEvent(eDisplayEventIceOn));
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            { // Swap buttons to match Connected device layout?
                ceDisplayOff(params, isEvent(eDisplayEventVacation /*eDisplayEventDispOff*/));
            }
            else
            {
                ceDisplayOff(params, isEvent(eDisplayEventDispOff));
            }

            ceDisplaySetDim(params, isEvent(eDisplayEventDispDimmer));
            ceRfTempUnitChange(consts, params, isEvent(eDisplayEventTempUnitChange));
            ceRfEcoExtra(consts, params, isEvent(eDisplayEventEcoExtra));               //APW : For BONA/ALPS
            ceRfEcoExtra(consts, params, isEvent(eDisplayEventEcoWhirlpool));           //APW : For HOSHI
            if(consts->rf->Cabin[0].components.vacuumBox.type)
              ceVacuumBox(consts, params,&consts->rf->Cabin[0], &params->rf->Cabin[0], isEvent(eDisplayEventVacuum));   
            if(consts->rf->Cabin[1].components.vacuumBox.type)
              ceVacuumBox(consts, params,&consts->rf->Cabin[1], &params->rf->Cabin[1], isEvent(eDisplayEventVacuum));
            if(consts->rf->Cabin[0].components.handleLight.type)
            {
              ceHandleLight(consts, params,&consts->rf->Cabin[0], &params->rf->Cabin[0], isEvent(eDisplayEventHandleLight));   
              ceHandleLightSet(consts, params,&consts->rf->Cabin[0], &params->rf->Cabin[0], isEvent(eDisplayEventSwitchMode)); 
              ceHandleLightOnOff(consts, params,&consts->rf->Cabin[0], &params->rf->Cabin[0], isEvent(eDisplayEventLightOff)); 
              
              if((params->rf->Cabin[0].E2.selectedColorIndex > 0) && !params->rf->Cabin[0].values.selectedColorIndexSetTime)
                uiP->dspPacket.funcs.ledUVC = true;
              else
                uiP->dspPacket.funcs.ledUVC = false;
            }
            if(consts->rf->Cabin[1].components.handleLight.type)
            {
              ceHandleLight(consts, params,&consts->rf->Cabin[1], &params->rf->Cabin[1], isEvent(eDisplayEventHandleLight));
              ceHandleLightSet(consts, params,&consts->rf->Cabin[1], &params->rf->Cabin[1], isEvent(eDisplayEventSwitchMode));
              ceHandleLightOnOff(consts, params,&consts->rf->Cabin[1], &params->rf->Cabin[1], isEvent(eDisplayEventLightOff)); 
              
              if((params->rf->Cabin[1].E2.selectedColorIndex > 0) && !params->rf->Cabin[1].values.selectedColorIndexSetTime)
                uiP->dspPacket.funcs.ledUVC = true;
              else
                uiP->dspPacket.funcs.ledUVC = false;
            }
            // ceCabinConvertible(&consts->rf->Cabin[1],&params->rf->Cabin[1], isEvent(eDisplayEventConverterFRZtoFF));
            // ceCabinFRZOff(&consts->rf->Cabin[1],&params->rf->Cabin[1], isEvent(eDisplayEventConverterFRZtoOFF));
            ceCabinChangeFRZMode(consts, params, &refrigerator_F, &refrigerator_R, isEvent(eDisplayEventConverterFRZtoFF));
#ifdef HOSHI_H30
            ceRfTempSettingFine(consts, params, isEvent(eDisplayEventTempSettingFine));
            ceRfMuteBuzzer(consts, params, isEvent(eDisplayEventMuteBuzzer));
            //ceRfDemoMode(consts, params, isEvent(eDisplayEventDemoMode));
#endif //HOSHI_H30    
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            { // Swap buttons to match Connected device layout?
                ceRfVacation(consts, params, isEvent(eDisplayEventStateMenu /*eDisplayEventVacation*/));
            }
            else
            {
                ceRfVacation(consts, params, isEvent(eDisplayEventVacation));
            }

            ceRfRice(consts, params, isEvent(eDisplayEventRice));
            if (!params->rf->Values.keyLock)
                ceDistributerSetState(uiP, _UI_DistributerState_Password, isEvent(eDisplayEventPassword));
            uiSabbathKeyHandler(consts, params);
            ceWaterSourceRstFilter(&consts->rf->WaterSource, &params->rf->WaterSource, isEvent(eDisplayEventFilterReset));
            //ceRfApplianceOff(consts, params, isEvent(eDisplayEventApplianceOFF));     //APW Don't need
        }
        ceDisplayServiceButton(consts, params, isEvent(eDisplayEventServiceButton));
#ifdef HOSHI_H30        
        ceDemoModeDisplay(uiP);     //Demo Display     
#endif //HOSHI_H30
        break;

    case _UI_DistributerState_Password:
        uiPasswordKeyHandler(consts, params);
        break;

    case _UI_DistributerState_Sabbath:
        uiSabbathKeyHandler(consts, params);
        break;

    case _UI_DistributerState_WifiConfig:

        if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
        { // Connected functionality active?
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            { // Swap buttons to match Connected device layout?
                if (isEvent(eDisplayEventDispOff /*wifi_OnOff*/))
                    toggleWifiState(WIFI_TOGGLE);
            }
            else
            {
                if (isEvent(eDisplayEventWifiStartStop) || params->rf->Values.WifiStatus == WIFI_CONNECTED)
                {
                    toggleWifiState(WIFI_TOGGLE);
                    params->rf->Values.WifiStatus = 0;
                }
            }
            if (isEvent(eDisplayEventWifiConfig))
                toggleWifiState(WIFI_CONFIG_TOGGLE);
        }
        break;

    default:
        break;
    }

    clearEvent();
}


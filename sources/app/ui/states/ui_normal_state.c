#include "ui_normal_state.h"
#include "../command_events/ce_cabin.h"
#include "../command_events/ce_buzzer.h"
#include "../command_events/ce_display.h"
#include "../command_events/ce_normal.h"
#include "../command_events/ce_refrigerator.h"
#include "../command_events/ce_service.h"
#include "../command_events/ce_water_source.h"
#include "../command_events/ce_dispenser.h"
#include "../command_events/ce_ice_machine.h"

#include "../command_events/cmd_check.h"

#include "../components/ui_display.h"
#include "../components/ui_buzzer.h"

#include "../procedures/ui_password.h"
#include "../procedures/ui_sabbath.h"

#include "app/refrigerator/procedures/rf_errors.h"

#include "../../mw/connectivity/remote_control.h"

void uiNormalStateInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    uiP->states.normal.state = _UI_NormalState_AllLedsOn;
    uiP->states.normal.generalTimer = _UI_Normal_AllLedsOn_Time;
    uiP->states.normal.errorTimer = _UI_Normal_Error_Time;
    uiP->states.normal.displayButtonPressedTimer = 0;

    //    if (endofbandtest)
    //    {
    //        WA[Situation] = 0;
    //        Wifi.TestMode = 0;
    //        endofbandtest = 0;
    //    }
    uiPasswordInit(consts, params);
    refrigerator_R.Values.doorsAlertAndDisplay = true;
}

void uiNormalState20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    // DD: DISPLAY IYILESTIRMESI
    if (!isEvent(eDisplayEventNone))
        uiP->states.normal.displayButtonPressedTimer = 2 * 50;
    if (uiP->states.normal.displayButtonPressedTimer)
        uiP->states.normal.displayButtonPressedTimer--;

    if (uiP->states.normal.generalTimer)
        uiP->states.normal.generalTimer--;
    if (uiP->states.normal.errorTimer)
        uiP->states.normal.errorTimer--;
    
    if(params->rf->Cabin[0].values.selectedColorIndexSetTime)
      params->rf->Cabin[0].values.selectedColorIndexSetTime--;
    
    if(params->rf->Cabin[1].values.selectedColorIndexSetTime)
      params->rf->Cabin[1].values.selectedColorIndexSetTime--;
}

void uiNormalStateTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    uiP->dspPacket.funcs.ledDispOff = true;
    rfErrorsCheckAll(consts->rf, params->rf);
    uiPasswordResetStayTimer(consts, params);

    /*
    EBI: At errors, sabbath will be active on display and mainboard.
    if( params->rf->Values.errors.errorsNumber && params->rf->Sets.user.functions.Sabbath ){
        rfSabbathDisable( params->rf );
        rfSabbathStore( params->rf );
    }
    */

    if ((!params->rf->Values.doorsClosed) ||  
        (params->rf->Dispenser.components.tactSwitch.value) ||
#ifndef HOSHI_H30
        (params->rf->Values.errors.errorsNumber) ||
#endif
        !isEvent(eDisplayEventNone) ||
        (uiP->serviceButton.led.state != _OFF)) // Ezgi Kutlu: Herhangi bir tusa basildiginda display off zamani telenir.
        uiResetDisplayOffTimer(uiP);

    switch (uiP->states.normal.state)
    {
    case _UI_NormalState_AllLedsOn_Init:
        uiP->states.normal.generalTimer = _UI_Normal_AllLedsOn_Time;
        uiP->states.normal.state = _UI_NormalState_AllLedsOn;
    case _UI_NormalState_AllLedsOn:
        uiDspAllOn(consts, params);

        uiP->dspPacket.funcs.dimLevel = eDisplayDimLevel100;

        if (!uiP->states.normal.generalTimer)
        {
            uiP->states.normal.state = _UI_NormalState_DispVersionRevision_Init;
        }

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_NormalState_DispVersionRevision_Init:

        uiP->states.normal.state = _UI_NormalState_DispVersionRevision;
        uiP->states.normal.generalTimer = _UI_Normal_VersionRevision_Time;
        uiP->errors.errorShowState = 0;

    case _UI_NormalState_DispVersionRevision:
        uiP->errors.errorShowState++;
#ifdef ALPS_H50
        if(uiP->states.normal.generalTimer > _UI_Normal_VersionRevision_Time / 2)
        {
          uiDspCabinData(consts, params, (unsigned char)(uiP->dspPacket.displayInfo.swVer + 100), 0, eDisplayCabinDataTypeNumber);
          uiDspCabinData(consts, params, (unsigned char)(uiP->dspPacket.displayInfo.swVer + 100), 1, eDisplayCabinDataTypeNumber);
        }
        else if(uiP->states.normal.generalTimer < _UI_Normal_VersionRevision_Time / 2)
        {
          uiDspCabinData(consts, params, (unsigned char)(uiP->dspPacket.displayInfo.swRev + 100), 0, eDisplayCabinDataTypeNumber);
          uiDspCabinData(consts, params, (unsigned char)(uiP->dspPacket.displayInfo.swRev + 100), 1, eDisplayCabinDataTypeNumber);
        }
#else
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
#endif        
        
        if (uiP->errors.errorShowState > 99)
            uiP->errors.errorShowState = 0;

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);

        uiP->dspPacket.funcs.ledEconomy = /*(FTC.tests.FTC == 0xAA) ? true :*/ false;
        uiP->dspPacket.funcs.ledEcoExtra = /*(FTC.tests.ICT == 0xAA) ? true :*/ false;
        uiP->dspPacket.funcs.ledVacation = false;
        uiP->dspPacket.funcs.ledIceOFF = false;
        uiP->dspPacket.funcs.ledIceOn = false;                  //APW : Add Ice making operation test in CT
        uiP->dspPacket.funcs.ledFastBottleCooling = false;      //APW : Add Ice making operation test in CT             
        //uiP->dspPacket.funcs.ledAlert = false;        //APW : Disable for BONA
        uiP->dspPacket.funcs.ledLock = false;
        uiP->dspPacket.funcs2.ledLockDspr = false;
        uiP->dspPacket.funcs.ledDispOff = false;
        uiP->dspPacket.funcs.ledFilter = (params->rf->WaterSource.values.errFilterEnabled) ? true : false;
        uiP->dspPacket.funcs.ledConverter = false;
        uiP->dspPacket.funcs.ledServiceButton = false;

        uiP->dspPacket.funcs.ledRice = false;
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

        if (!uiP->states.normal.generalTimer)
            uiP->states.normal.state = _UI_NormalState_VersionRevision_Init;

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_NormalState_VersionRevision_Init:
        uiP->states.normal.state = _UI_NormalState_VersionRevision;
        uiP->states.normal.generalTimer = _UI_Normal_VersionRevision_Time;
        uiP->errors.errorShowState = 0;
    case _UI_NormalState_VersionRevision:

        uiP->errors.errorShowState++;
#ifdef ALPS_H50
        if(uiP->states.normal.generalTimer > _UI_Normal_VersionRevision_Time / 2)
        {
          uiDspCabinData(consts, params, (unsigned char)(consts->rf->Stock.Version + 100), 0, eDisplayCabinDataTypeNumber);
          uiDspCabinData(consts, params, (unsigned char)(consts->rf->Stock.Version + 100), 1, eDisplayCabinDataTypeNumber);
        }
        else if(uiP->states.normal.generalTimer < _UI_Normal_VersionRevision_Time / 2)
        {
          uiDspCabinData(consts, params, (unsigned char)(consts->rf->Stock.Revision + 100), 0, eDisplayCabinDataTypeNumber);
          uiDspCabinData(consts, params, (unsigned char)(consts->rf->Stock.Revision + 100), 1, eDisplayCabinDataTypeNumber);
        }
#else
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
#endif        
        

        if (uiP->errors.errorShowState > 99)
            uiP->errors.errorShowState = 0;

        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);

        uiP->dspPacket.funcs.ledEconomy = /*(FTC.tests.FTC == 0xAA) ? true :*/ false;
        uiP->dspPacket.funcs.ledEcoExtra = /*(FTC.tests.ICT == 0xAA) ? true :*/ false;
        uiP->dspPacket.funcs.ledVacation = false;
        uiP->dspPacket.funcs.ledIceOFF = false;
        uiP->dspPacket.funcs.ledIceOn = false;                  //APW : Add Ice making operation test in CT
        uiP->dspPacket.funcs.ledFastBottleCooling = false;      //APW : Add Ice making operation test in CT
        //uiP->dspPacket.funcs.ledAlert = false;        //APW : Disable for BONA
        uiP->dspPacket.funcs.ledLock = false;
        uiP->dspPacket.funcs2.ledLockDspr = false;
        uiP->dspPacket.funcs.ledDispOff = false;
        uiP->dspPacket.funcs.ledFilter = (params->rf->WaterSource.values.errFilterEnabled) ? true : false;
        uiP->dspPacket.funcs.ledConverter = false;
        uiP->dspPacket.funcs.ledServiceButton = false;

        uiP->dspPacket.funcs.ledRice = false;
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
        uiDspDispenser(consts, params);
        uiP->dspPacket.funcs.dimLevel = eDisplayDimLevel100;

        if (!uiP->states.normal.generalTimer)
            uiP->states.normal.state = _UI_NormalState_TempUnit_Init;

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_NormalState_TempUnit_Init:
        uiP->states.normal.generalTimer = _UI_Normal_TempUnit_Time;
        uiP->states.normal.state = _UI_NormalState_TempUnit;
    case _UI_NormalState_TempUnit:
        uiDspTempUnit(consts, params);
        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);

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
        uiP->dspPacket.funcs.ledServiceButton = false;

        uiP->dspPacket.funcs.ledRice = false;
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

        if (!uiP->states.normal.generalTimer)
            uiP->states.normal.state = _UI_NormalState_Sets_Init;

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_NormalState_Sets_Init:

        uiP->errors.errorsIndex = params->rf->Values.errors.errorsNumber;
        uiResetDisplayOffTimer(uiP);
        uiP->states.normal.errorTimer = _UI_Normal_Error_Time;
        uiP->states.normal.generalTimer = _UI_Normal_Sets_Time;
        uiP->states.normal.state = _UI_NormalState_Sets;
        uiP->errors.errorShowState = 0;
    case _UI_NormalState_Sets:
        if (params->rf->Values.errors.errorsNumber)
        {
            if (uiP->states.normal.errorTimer > _UI_Normal_Error_Time / 2)
            {
                if (uiP->errors.errorsIndex < params->rf->Values.errors.errorsNumber)
                {
#ifndef HOSHI_H30
                    uiDspError(consts, params); 
                    //uiP->dspPacket.funcs.ledAlert = true;     //APW : Disable for BONA
#endif
                }
                else
                {
                    uiDspCabinsSets(consts, params);
                    //uiP->dspPacket.funcs.ledAlert = false;      //APW : Disable for BONA
                }
            }
            if (!uiP->states.normal.errorTimer)
            {
                uiP->errors.errorShowState = 0;
                uiP->errors.errorsIndex++;
                if (uiP->errors.errorsIndex > params->rf->Values.errors.errorsNumber)
                    uiP->errors.errorsIndex = 0;

                if (uiP->errors.errorsIndex == params->rf->Values.errors.errorsNumber)
                    uiP->states.normal.errorTimer = _UI_Normal_Error_Time;
                else
                    uiP->states.normal.errorTimer = _UI_Normal_Error_Time;
            }
        }
        else
        {
            //uiP->dspPacket.funcs.ledAlert = false;    //APW : Disable for BONA

            uiDspCabinsSets(consts, params);

            uiP->errors.errorsIndex = 0;
            uiP->states.normal.errorTimer = _UI_Normal_Error_Time >> 2;
        }
        
        if(refrigerator_R.WaterSource.values.waterTankEmpty)
          uiP->dspPacket.funcs.ledAlert = true;
        else
          uiP->dspPacket.funcs.ledAlert = false;
        
        uiDspCabinsQuicks(consts, params);
        uiDspCabinIcon(consts, params);

        uiP->dspPacket.funcs.ledEconomy = params->rf->Sets.user.functions.Economy;
        uiP->dspPacket.funcs.ledEcoExtra = params->rf->Sets.user.functions.EcoExtra;
        uiP->dspPacket.funcs.ledVacation = params->rf->Sets.user.functions.Vacation;
        uiDspVacuumBox(consts, params);
        uiDspIceOff(consts, params);
        uiDspKeyLock(consts, params);

        uiP->dspPacket.funcs.ledDispOff = false;
        uiP->dspPacket.funcs.ledFilter = params->rf->WaterSource.values.errFilterTime;
        uiP->dspPacket.funcs.ledConverter = params->rf->Cabin[1].sets.user.functions.Convertible || params->rf->Cabin[1].sets.user.functions.CabinOff;
        uiP->dspPacket.funcs.ledServiceButton = uiP->serviceButton.led.serviceButton;

        if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
        {
          uiP->dspPacket.funcs.ledWifi = WiFiLed.state;
        }
        else
        {
          uiP->dspPacket.funcs.ledWifi = false;
        }

        uiDspDispenser(consts, params);
        uiP->dspPacket.funcs.dimLevel = params->rf->Values.dispDim;

        if (params->rf->Values.errors.errorsNumber)
        {
#ifndef HOSHI_H30
            uiResetDisplayOffTimer(uiP);
            uiP->states.normal.generalTimer = _UI_Normal_Sets_Time;
#else
            if (params->rf->Values.dispOff && !uiP->timers.dspOffTimer)
                uiP->states.normal.state = _UI_NormalState_DisplayOff_Init;
            if (!uiP->states.normal.generalTimer && consts->rf->Parameters.realTemp)
                uiP->states.normal.state = _UI_NormalState_RealTemp_Init;
#endif
        }
        else
        {
            if (params->rf->Values.dispOff && !uiP->timers.dspOffTimer)
                uiP->states.normal.state = _UI_NormalState_DisplayOff_Init;
            if (!uiP->states.normal.generalTimer && consts->rf->Parameters.realTemp)
                uiP->states.normal.state = _UI_NormalState_RealTemp_Init;
        }

        // EBI: At errors, sabbath will be active on display and mainboard.
        if (params->rf->Sets.user.functions.Sabbath)
            uiP->states.normal.state = _UI_NormalState_Sabbath;

#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_NormalState_RealTemp_Init:
        uiResetDisplayOffTimer(uiP);
        uiP->states.normal.state = _UI_NormalState_RealTemp;
    case _UI_NormalState_RealTemp:
        uiDspCabinsRealTemps(consts, params);
        uiDspCabinsQuicks(consts, params);
        uiDspCabinIcon(consts, params);

        // show enabled functions
        uiP->dspPacket.funcs.ledEconomy = params->rf->Sets.user.functions.Economy;
        uiP->dspPacket.funcs.ledEcoExtra = params->rf->Sets.user.functions.EcoExtra;
        uiP->dspPacket.funcs.ledVacation = params->rf->Sets.user.functions.Vacation;
        uiDspVacuumBox(consts, params);
        uiDspIceOff(consts, params);
        //uiP->dspPacket.funcs.ledAlert = params->rf->Values.errHighTemp;       //APW : Disable for BONA
        uiDspKeyLock(consts, params);
        uiP->dspPacket.funcs.ledDispOff = false;
        uiP->dspPacket.funcs.ledFilter = params->rf->WaterSource.values.errFilterTime;
        uiP->dspPacket.funcs.ledConverter = false;
        uiP->dspPacket.funcs.ledServiceButton = false;

        // TODO connected
        // Connected functionality active?
        if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
        {
          uiP->dspPacket.funcs.ledWifi = WiFiLed.state;
        }
        else
        {
          uiP->dspPacket.funcs.ledWifi = false;
        }

        uiDspDispenser(consts, params);
        uiDspCabinIcon(consts, params);

        uiP->dspPacket.funcs.dimLevel = params->rf->Values.dispDim;

        if (params->rf->Values.errors.errorsNumber || !consts->rf->Parameters.realTemp)
        {
#ifndef HOSHI_H30
            uiP->states.normal.state = _UI_NormalState_Sets_Init;
#else
            if (params->rf->Values.dispOff && !uiP->timers.dspOffTimer)
                uiP->states.normal.state = _UI_NormalState_DisplayOff_Init;
            if (params->rf->Sets.user.functions.Sabbath)
                uiP->states.normal.state = _UI_NormalState_Sabbath;
#endif
        }
        else
        {
            if (params->rf->Values.dispOff && !uiP->timers.dspOffTimer)
                uiP->states.normal.state = _UI_NormalState_DisplayOff_Init;
            if (params->rf->Sets.user.functions.Sabbath)
                uiP->states.normal.state = _UI_NormalState_Sabbath;
        }
#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        break;

    case _UI_NormalState_DisplayOff_Init:
        uiP->states.normal.state = _UI_NormalState_DisplayOff;
    case _UI_NormalState_DisplayOff:
        uiP->dspPacket.funcs.ledDispOff = true;
        uiP->dspPacket.funcs.ledLock = false;
        //uiP->dspPacket.funcs2.ledLockDspr = false;
        
        uiDspIceCabinsSetsOff(consts, params);
        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspFunctionsAtOffMode(consts, params);
        // uiDspKeyLock(consts, params);

       /* if (params->rf->IceCreamMachine.values.state ==
            _IceCreamMachineState_IceCreamIsMade)
            uiP->states.normal.state = _UI_NormalState_Sets_Init;*/

        uiP->dspPacket.funcs.dimLevel = params->rf->Values.dispDim;
#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
#ifndef HOSHI_H30
        if (params->rf->Values.errors.errorsNumber)
            uiP->states.normal.state = _UI_NormalState_Sets_Init;
#endif
        if (!params->rf->Values.doorsClosed)
        {
            uiP->states.normal.state = _UI_NormalState_Sets_Init;
        }         
        if (params->rf->Dispenser.components.tactSwitch.value)
        {
            uiP->states.normal.state = _UI_NormalState_Sets_Init;
        }
        break;

    case _UI_NormalState_Password:
        uiPasswordTask(consts, params);
#ifdef HOSHI_H30
        uiBuzzerDoorAlarmAHTH(consts, params, params->rf->Values.doorsAlert);
#else
        uiBuzzerWarning(consts, params, params->rf->Values.doorsAlert);
#endif //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
        if (!uiP->password.stayTimer)
            uiP->states.normal.state = _UI_NormalState_AllLedsOn_Init;
        break;

    case _UI_NormalState_Sabbath:
        uiSabbathTask(consts, params);
        if (!params->rf->Sets.user.functions.Sabbath)
            uiP->states.normal.state = _UI_NormalState_Sets_Init;
        break;

    case _UI_NormalState_WifiConfig:
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
        uiP->states.normal.state = _UI_NormalState_AllLedsOn_Init;
        break;
    }
}

void uiNormalStateKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

#ifdef ALPS_H50
    if (!params->rf->Values.keyLock)
    {
        ceServiceStart(uiP, isEvent(eDisplayEventServiceMode));
    }
#endif

    /*
    if( params->rf->Values.doorsAlert &&
      ( DisplayData.DPCommand != eDisplayEventBuzzer )&&
        !isEvent( eDisplayEventNone ) ){
        DisplayData.DPCommand = eDisplayEventNone;
        params->rf->Values.doorsAlertDisable = true;
    }
    */


    // Door Alarm
#ifdef HOSHI_H30
#ifdef ALPS_H50
    if (params->rf->Values.doorsAlert)
    {
        if (!isEvent(eDisplayEventNone) && !isEvent(eDisplayEventBuzzer))
        {
            clearEvent();
            params->rf->Values.doorsAlertDisable = true;
        }
    }
#else    
    if (params->rf->Values.doorsAlert && !refrigerator_R.Values.prohibitionAlertDisplay)
    {
        if (!isEvent(eDisplayEventNone) && !isEvent(eDisplayEventBuzzer))
        {
            clearEvent();
        }
    }
#endif  
#else
    if (params->rf->Values.doorsAlert)
    {
        if (!isEvent(eDisplayEventNone) && !isEvent(eDisplayEventBuzzer))
        {
            clearEvent();
            params->rf->Values.doorsAlertDisable = true;
        }
    }
#endif

    if (params->rf->Cabin[1].values.HighTemErr.isactive && !params->rf->Values.keyLock)
    {     
      if (!isEvent(eDisplayEventNone) && !isEvent(eDisplayEventBuzzer) && 
          !isEvent(eDisplayEventIceCube) && !isEvent(eDisplayEventIceCrush) && 
            !isEvent(eDisplayEventMeasuredFill) && !isEvent(eDisplayEventWater) && !isEvent(eDisplayEventIceCreamMchOnOff) )
      {
        clearEvent();
        params->rf->Cabin[1].values.HighTemErr.isactive = 0;
        params->rf->Cabin[1].values.HighTemErr.delayTime = /*consts->rf->Cabin[1].parameters.HighTempError.HighTmp_DelayAlarm*/ 360 * 60; // Bilinçli olarak sabit bir deger belirlendi.
        //uiP->dspPacket.funcs.ledAlert = false; //APW : Disable for BONA
      }
    }

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

    switch (uiP->states.normal.state)
    {
    case _UI_NormalState_AllLedsOn_Init:
    case _UI_NormalState_AllLedsOn:
    case _UI_NormalState_DispVersionRevision_Init:
    case _UI_NormalState_DispVersionRevision:
    case _UI_NormalState_VersionRevision_Init:
    case _UI_NormalState_VersionRevision:
    case _UI_NormalState_TempUnit_Init:
    case _UI_NormalState_TempUnit:
        ceDisplayKeyLock(params, isEvent(eDisplayEventKeyLock));
        ceDisplayKeyLockDspr(params, isEvent(eDisplayEventIceCreamMchOnOff));

        // Connected functionality active?
        if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
        {
            // Swap buttons to match Connected device layout?
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            {
                if (isEvent(eDisplayEventDispOff))
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

            if (isEvent(eDisplayEventWifiFactoryReset))
                Connectivity_Reset();
        }

        if (!params->rf->Values.keyLock || !params->rf->Values.keyLockDspr)
        {
            ceNormalSetState(params, _UI_NormalState_Password, isEvent(eDisplayEventPassword));
        }

        break;

    case _UI_NormalState_DisplayOff_Init:
    case _UI_NormalState_DisplayOff:

#ifdef ALPS_H50    
        ceDisplayAutoDoor(params, isEvent(eDisplayEventAutoDoor));
#endif        
        if (isEvent(eDisplayEventNone) == false)
        {
            ceNormalSetState(params, _UI_NormalState_Sets_Init, !isEvent(eDisplayEventNone));
            ceBuzzer(true);
        }
        else
        {
            ceDisplayOff(params, isEvent(eDisplayEventDispOff));
            ceRfKnobsCheck(consts, params);
        }
        ceDisplayServiceButton(consts, params, isEvent(eDisplayEventServiceButton));
        break;

    case _UI_NormalState_RealTemp_Init:
    case _UI_NormalState_RealTemp:
        ceDisplayKeyLock(params, isEvent(eDisplayEventKeyLock));
        ceDisplayKeyLockDspr(params, isEvent(eDisplayEventIceCreamMchOnOff));
        
        if (!params->rf->Values.keyLockDspr)
        {
            ceDispenserNextState(consts, params, &consts->rf->Dispenser, &params->rf->Dispenser, (isEvent(eDisplayEventDispencer)||isEvent(eDisplayEventWater)||isEvent(eDisplayEventIceCrush)||isEvent(eDisplayEventIceCube)|| isEvent(eDisplayEventMeasuredFill)||isEvent(eDisplayEventCalibrateCustom)), (unsigned char *)(&params->rf->ui.dspValues.receivedEvent));            // Swap buttons to match Connected device layout?
        }
        
        if (!params->rf->Values.keyLock)
        {

            if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
            {
                if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
                {
                    if (isEvent(eDisplayEventDispOff))
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

                if (isEvent(eDisplayEventWifiFactoryReset))
                    Connectivity_Reset();
            }

            for (unsigned char i = 0; i < _NumOfCabin; i++)
            {
                ceNormalSetState(params, _UI_NormalState_Sets_Init,
                                 isEvent((teDisplayEvent)(eDisplayEventC0SetUp + i)));

                ceNormalSetState(params, _UI_NormalState_Sets_Init,
                                 isEvent((teDisplayEvent)(eDisplayEventC0SetDown + i)));

                ceCabinQuick(consts, params,
                             &consts->rf->Cabin[i], &params->rf->Cabin[i],
                             isEvent((teDisplayEvent)(eDisplayEventC0QCool + i)));

                ceNormalSetState(params, _UI_NormalState_Sets_Init,
                                 isEvent((teDisplayEvent)(eDisplayEventC0Mode + i)));
            }
            ceQuickIce60OnOff(consts, params, &consts->rf->IceMachine, &params->rf->IceMachine, isEvent(eDisplayEventQuickIce60));
            ceIceMachineOnOff(consts, params, &consts->rf->IceMachine, &params->rf->IceMachine, isEvent(eDisplayEventIceOn));  
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            {
                ceDisplayOff(params, isEvent(eDisplayEventVacation));
            }
            else
            {
                ceDisplayOff(params, isEvent(eDisplayEventDispOff));
            }

            ceDisplaySetDim(params, isEvent(eDisplayEventDispDimmer));
            ceRfTempUnitChange(consts, params, isEvent(eDisplayEventTempUnitChange));
            ceRfEcoExtra(consts, params, isEvent(eDisplayEventEcoExtra));               //APW : For BONA/ALPS
            ceRfEcoExtra(consts, params, isEvent(eDisplayEventEcoWhirlpool));	        //APW : For HOSHI
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
            ceCabinChangeFRZMode(consts, params, consts->rf, params->rf, isEvent(eDisplayEventConverterFRZtoFF));
#ifdef HOSHI_H30      
            ceRfTempSettingFine(consts, params, isEvent(eDisplayEventTempSettingFine));
            ceRfMuteBuzzer(consts, params, isEvent(eDisplayEventMuteBuzzer));
            ceRfDemoMode(consts, params, isEvent(eDisplayEventDemoMode));
#endif //HOSHI_H30   
            // Swap buttons to match Connected device layout?
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            {
                ceRfVacation(consts, params, isEvent(eDisplayEventStateMenu));
            }
            else
            {
                ceRfVacation(consts, params, isEvent(eDisplayEventVacation));
            }

            ceRfRice(consts, params, isEvent(eDisplayEventRice));
            ceNormalSetState(params, _UI_NormalState_Password, isEvent(eDisplayEventPassword));
            uiSabbathKeyHandler(consts, params);
            ceRfSilent(consts, params, isEvent(eDisplayEventSilent));
            ceWaterSourceRstFilter(&consts->rf->WaterSource, &params->rf->WaterSource, isEvent(eDisplayEventFilterReset));
            //ceRfApplianceOff(consts, params, isEvent(eDisplayEventApplianceOFF));     //APW Don't need
        }
        ceRfKnobsCheck(consts, params);
        ceDisplayServiceButton(consts, params, isEvent(eDisplayEventServiceButton));
        break;

    case _UI_NormalState_Sets_Init:
    case _UI_NormalState_Sets:
        ceDisplayKeyLock(params, isEvent(eDisplayEventKeyLock));
        ceDisplayKeyLockDspr(params, isEvent(eDisplayEventIceCreamMchOnOff));
#ifdef ALPS_H50        
        ceDisplayAutoDoor(params, isEvent(eDisplayEventAutoDoor));
#endif        
        if (!params->rf->Values.keyLockDspr)
        {
          ceDispenserNextState(consts, params, &consts->rf->Dispenser, &params->rf->Dispenser, (isEvent(eDisplayEventDispencer)||isEvent(eDisplayEventWater)||isEvent(eDisplayEventIceCrush)||isEvent(eDisplayEventIceCube)|| isEvent(eDisplayEventMeasuredFill)||isEvent(eDisplayEventCalibrateCustom)), (unsigned char *)(&params->rf->ui.dspValues.receivedEvent));            // Swap buttons to match Connected device layout?
        }
        
        if (!params->rf->Values.keyLock)
        {

            if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
            {
                if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
                {
                    if (isEvent(eDisplayEventDispOff))
                        toggleWifiState(WIFI_TOGGLE);
                }
                else
                {
                    if (isEvent(eDisplayEventWifiStartStop) || params->rf->Values.WifiStatus == WIFI_ON)
                    {
                        toggleWifiState(WIFI_TOGGLE);
                        params->rf->Values.WifiStatus = 0;
                    }
                }
                if (isEvent(eDisplayEventWifiConfig))
                    toggleWifiState(WIFI_CONFIG_TOGGLE);

                if (isEvent(eDisplayEventWifiFactoryReset))
                {
                    Connectivity_Reset();
                    clearEvent();
                }
            }


            for (unsigned char i = 0; i < _NumOfCabin; i++)
            {
                // ceSetUp has not defined yet...

                ceCabinSetUp(consts, params,
                             &consts->rf->Cabin[i], &params->rf->Cabin[i],
                             isEvent((teDisplayEvent)(eDisplayEventC0SetUp + i)));
                ceCabinSetDown(consts, params,
                               &consts->rf->Cabin[i], &params->rf->Cabin[i],
                               isEvent((teDisplayEvent)(eDisplayEventC0SetDown + i)));

                ceCabinQuick(consts, params,
                             &consts->rf->Cabin[i], &params->rf->Cabin[i],
                             isEvent((teDisplayEvent)(eDisplayEventC0QCool + i)));

                ceCabinMode(consts, params,
                            &consts->rf->Cabin[i], &params->rf->Cabin[i],
                            isEvent((teDisplayEvent)(eDisplayEventC0Mode + i)));
            }
            ceQuickIce60OnOff(consts, params, &consts->rf->IceMachine, &params->rf->IceMachine, isEvent(eDisplayEventQuickIce60));
            ceIceMachineOnOff(consts, params, &consts->rf->IceMachine, &params->rf->IceMachine, isEvent(eDisplayEventIceOn));
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            {
              
              ceDisplayOff(params, isEvent(eDisplayEventVacation));
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
            ceCabinChangeFRZMode(consts, params, consts->rf, params->rf, isEvent(eDisplayEventConverterFRZtoFF));
#ifdef HOSHI_H30
            ceRfTempSettingFine(consts, params, isEvent(eDisplayEventTempSettingFine));
            ceRfMuteBuzzer(consts, params, isEvent(eDisplayEventMuteBuzzer));
            ceRfDemoMode(consts, params, isEvent(eDisplayEventDemoMode));
#endif //HOSHI_H30       
    
            // Swap buttons to match Connected device layout?
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            {
                ceRfVacation(consts, params, isEvent(eDisplayEventStateMenu));
            }
            else
            {
                ceRfVacation(consts, params, isEvent(eDisplayEventVacation));
            }

            ceRfRice(consts, params, isEvent(eDisplayEventRice));
            ceNormalSetState(params, _UI_NormalState_Password, isEvent(eDisplayEventPassword));
            uiSabbathKeyHandler(consts, params);
            ceRfSilent(consts, params, isEvent(eDisplayEventSilent));

            ceDisplayHighTempAlarmOff(params, isEvent(eDisplayEventHighTempAlarmOff));
#if(CONNECTIVITY != NON_CONNECTED)
            if ((wifiError_B2 == 1) && isEvent(eDisplayEventWifiStartStop))
            {
                Connectivity_ResetErrors();
            }
#endif
            ceWaterSourceRstFilter(&consts->rf->WaterSource, &params->rf->WaterSource, isEvent(eDisplayEventFilterReset));

            //ceRfApplianceOff(consts, params, isEvent(eDisplayEventApplianceOFF));     //APW Don't need

            if (params->rf->Values.tempType == _TempType_Celcius)
            {
                // uiP->dspPacket.funcs.ledFahr_Cabin1 = true;
                uiP->dspPacket.funcs.ledCelsius = true;
                // uiP->dspPacket.funcs.ledFahr_Cabin0 = false;
                //uiP->dspPacket.funcs.ledFahrenheit = false;                   //APW
                uiP->dspPacket.funcs.ledFahrenheit = true;                      //Add for display enable led dregee (APW Version6)
            }
            else
            {
                // uiP->dspPacket.funcs.ledFahr_Cabin1 = false;
                uiP->dspPacket.funcs.ledCelsius = false;
                // uiP->dspPacket.funcs.ledFahr_Cabin0 = true;
                //uiP->dspPacket.funcs.ledFahrenheit = true;                    //APW              
                uiP->dspPacket.funcs.ledFahrenheit = false;                     //Add for display enable led dregee (APW Version6)
            }
        }

        ceRfKnobsCheck(consts, params);
        ceDisplayServiceButton(consts, params, isEvent(eDisplayEventServiceButton));
        break;

    case _UI_NormalState_Password:
        uiPasswordKeyHandler(consts, params);
        break;

    case _UI_NormalState_Sabbath:
        uiSabbathKeyHandler(consts, params);
        break;

    case _UI_NormalState_WifiConfig:
        if (consts->rf->Connected.funcConnectedApp.connectedAppAct == true)
        {
            if (consts->rf->Connected.funcConnectedApp.swapConnButtons == true)
            {
                if (isEvent(eDisplayEventDispOff))
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

            if (isEvent(eDisplayEventWifiFactoryReset))
                Connectivity_Reset();
        }

        break;

    default:
        break;
    }
#if (CONNECTIVITY != NON_CONNECTED)
    if (WiFiVariables.Situation != WIFI_OFF)
    {
        iswifioff = 20;
    }
#endif
    clearEvent();
}

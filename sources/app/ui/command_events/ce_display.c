#include "ce_display.h"
#include "../components/ui_service_button.h"

void ceDisplayOff(tsUiProcParams *params, bool cmdReceived)
{
    tsUiRam *uiP = params->ui;

    if (!cmdReceived)
        return;

    if (uiP->dspPacket.funcs.ledDispOff)
    {
        /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
        if(params->rf->workMode.values.workMode == _WM_CommercialTest)
        {
          uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
        }
        else
        {
          uiP->states.normal.state = _UI_NormalState_Sets_Init;
        }
        /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    }
    else
    {
        /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
        if(params->rf->workMode.values.workMode == _WM_CommercialTest)
        {
          uiP->states.commercial.state = _UI_CommercialState_DisplayOff_Init;
        }
        else
        {
          uiP->states.normal.state = _UI_NormalState_DisplayOff_Init;
        }
        /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    }
        
    /*
    if( consts->rf->Parameters.dispOff ){
    params->rf->Values.dispOff = !params->rf->Values.dispOff;
    if( params->rf->Values.dispOff ){
      uiP->timers.dspOffTimer = 0;
      sysBuzzerCmd( eSysBuzzerPlayStartMelody);
    }else{
      uiInitStates(params->ui);
      sysBuzzerCmd( eSysBuzzerPlayEndMelody);
    }

    params->rf->E2.dispOff = params->rf->Values.dispOff;
    uiResetSetChangeTimer(uiP);
    }
    else if(consts->rf->Parameters.autoDispOff){
        params->rf->Values.errHighTemp = 0;
        sysBuzzerCmd( eSysBuzzerPlayEndMelody);

        //   alarmDelay   ///
        if(params->rf->Cabin[1].values.HighTemErr.isactive){
        params->rf->Cabin[1].values.HighTemErr.isactive = 0;
        params->rf->Cabin[1].values.HighTemErr.delayTime =  consts->rf->Cabin[1].parameters.HighTempError.HighTmp_DelayAlarm*60;
        uiP->dspPacket.funcs.ledAlert = false;
        }
    ///
    }
    */
}

#ifdef ALPS_H50
void ceDisplayAutoDoor(tsUiProcParams *params, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    //This function for cabin 0      
            
    if((!params->rf->Cabin[0].values.doorAssist.operationFlag) && (params->rf->Cabin[0].components.doorSwitch[0].value == _SwitchValue_CLOSE))
    {
        params->rf->Cabin[0].values.doorAssist.request = true;
        sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
    }

    uiInitStates(params->ui);
    uiResetSetChangeTimer(params->ui);
}
#endif
void ceDisplayKeyLock(tsUiProcParams *params, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    params->rf->Values.keyLock = !params->rf->Values.keyLock;

    if (params->rf->Values.keyLock)
        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    else
        sysBuzzerCmd(eSysBuzzerPlayEndMelody);

    uiInitStates(params->ui);
    uiResetSetChangeTimer(params->ui);
    params->rf->E2.keyLock = params->rf->Values.keyLock;
}

void ceDisplayKeyLockDspr(tsUiProcParams *params, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    params->rf->Values.keyLockDspr = !params->rf->Values.keyLockDspr;

    if (params->rf->Values.keyLockDspr)
        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    else
        sysBuzzerCmd(eSysBuzzerPlayEndMelody);

    uiInitStates(params->ui);
    uiResetSetChangeTimer(params->ui);
    params->rf->E2.keyLockDspr = params->rf->Values.keyLockDspr;
}

void ceDisplaySetDim(tsUiProcParams *params, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    if (params->rf->Values.dispDim > eDisplayDimLevelStart)
        params->rf->Values.dispDim--;
    else
        params->rf->Values.dispDim = eDisplayDimLevel100;
    uiInitStates(params->ui);
    params->rf->E2.dispDim = params->rf->Values.dispDim;
}

/* AB: Yüksek sicaklik alarmi manuel kapatma */
void ceDisplayHighTempAlarmOff(tsUiProcParams *params, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    if (params->rf->Cabin[1].values.HighTemErr.isactive)
    {
        params->rf->Cabin[1].values.HighTemErr.isactive = 0;
        params->rf->Cabin[1].values.HighTemErr.delayTime = /*consts->rf->Cabin[1].parameters.HighTempError.HighTmp_DelayAlarm*/ 360 * 60; // Bilinçli olarak sabit bir değer belirlendi.
        //params->ui->dspPacket.funcs.ledAlert = false;         //APW : Disable for BONA
    }
}

void ceDisplayServiceButton(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    if (!consts->rf->Connected.funcConnectedApp.connectedAppAct)
        return;

    uiServiceButtonKeyHandler(consts, params);

    if (params->rf->Values.keyLock)
        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    else
        sysBuzzerCmd(eSysBuzzerPlayEndMelody);

    uiInitStates(params->ui);
    uiResetSetChangeTimer(params->ui);
}

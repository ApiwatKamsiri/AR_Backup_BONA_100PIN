/** @file       ui_proc.c
 *  @brief      UI_PROC application program file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-09-13
 */
#define FILE_UI_PROC_C

#include "ui_proc.h"

#include "app/refrigerator/procedures/rf_eco_extra.h"
#include "app/refrigerator/procedures/rf_sabbath.h"
#include "app/refrigerator/procedures/rf_silent.h"

#include "states/ui_states.h"
#include "states/ui_commercial_state.h"
#include "states/ui_distributer_state.h"
#include "components/ui_display.h"
#include "components/ui_service_button.h"

#include "procedures/ui_password.h"
#include "procedures/ui_wifi.h"

#include "command_events/ce_refrigerator.h"
#include "command_events/ce_cabin.h"
#include "command_events/cmd_check.h"
#include "../mw/connectivity/remote_control.h"

#define DEBUG_FILE_NAME "UI"

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup UI_PROC
 *  @{
 */
#define _UI_DisplayOffTime (1 * 60) //(dk*60) sec
#define _UI_SetChangeTimer (5 * 50) //(sec*50) *20msec

static PROCESS_HANDLER_PROTO(uiProcIdleStateHandler);
static PROCESS_HANDLER_PROTO(uiProcNormalStateHandler);
static PROCESS_HANDLER_PROTO(uiProcAppOffStateHandler);
static PROCESS_HANDLER_PROTO(uiProcProdTestStateHandler);
#ifdef SERVICEMODEavailable
static PROCESS_HANDLER_PROTO(uiProcServiceStateHandler);
#endif //SERVICEMODEavailable
static PROCESS_HANDLER_PROTO(uiProcDistributerStateHandler);
static PROCESS_HANDLER_PROTO(uiProcFTCStateHandler);
static PROCESS_HANDLER_PROTO(uiProcDefaultEventHandler);
static PROCESS_HANDLER_PROTO(uiProcSuper20MsTask);
static PROCESS_HANDLER_PROTO(uiProcSuper1SecTask);
static PROCESS_HANDLER_PROTO(uiProcSuper1MinTask);
static PROCESS_HANDLER_PROTO(uiProcCommercialTestStateHandler);                 //APW : Add CT heater software
static PROCESS_HANDLER_PROTO(uiProcCommercialDefrostTestStateHandler);          //APW : Add CT heater software

static void uiInit(const tsUiProcConsts *consts, tsUiProcParams *params);
static void ui20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params);
static void ui1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params);
static void uiTask(const tsUiProcConsts *consts, tsUiProcParams *params);

/// @brief  Initialization function of uiProc
PROCESS_INIT_PROTO(uiProcInit)
{
    const tsUiProcConsts *consts = process->constants;

    devCpxInit(consts->dsp, NULL);

    PROCESS_STATE_CHANGE(process, uiProcIdleStateHandler);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of uiProc
PROCESS_DEINIT_PROTO(uiProcDeinit)
{
    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/****************** UI EXPORTED FUNCTIONS ******************/

void uiInitStates(tsUiRam *uiP)
{
    if (uiP->states.normal.state == _UI_NormalState_DisplayOff)
        uiP->states.normal.state = _UI_NormalState_Sets_Init;
    if (uiP->states.normal.state == _UI_NormalState_Sets)
        uiP->states.normal.state = _UI_NormalState_Sets_Init;
    if (uiP->states.normal.state == _UI_NormalState_RealTemp)
        uiP->states.normal.state = _UI_NormalState_Sets_Init;
    if (uiP->states.distributer.state == _UI_DistributerState_Sets)
        uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
    if (uiP->states.distributer.state == _UI_DistributerState_RealTemp)
        uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
    // if (UI_FairModeState == _UI_FairModeState_Sets)
    //     UI_FairModeState = _UI_FairModeState_Sets_Init;
    // if (UI_FairModeState == _UI_FairModeState_RealTemp)
    //     UI_FairModeState = _UI_FairModeState_Sets_Init;
          
    /*** APW : Add Ice making operation test in CT ***/
    if (uiP->states.commercial.state == _UI_CommercialState_DisplayOff)
        uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
    if (uiP->states.commercial.state == _UI_CommercialState_Sets)
        uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
    if (uiP->states.commercial.state == _UI_CommercialState_RealTemp)
        uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
    /*** APW : Add Ice making operation test in CT ***/    
}

static void uiInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{    
    uiServiceButtonInit(consts, params);
    uiCommercialStatusInit(consts, params);                                     //APW : Add CT heater software
}

void uiResetDisplayOffTimer(tsUiRam *uiP)
{
    uiP->timers.dspOffTimer = _UI_DisplayOffTime;
}

void uiResetSetChangeTimer(tsUiRam *uiP)
{
    uiP->timers.setChangeTimer = _UI_SetChangeTimer;
}

/****************** UI GENERIC TASKS ******************/

static void ui20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;

    if (uiP->timers.warningTimer)
    uiP->timers.warningTimer--;

    if (uiP->timers.alertTimer)
    uiP->timers.alertTimer--;

    if (uiP->timers.keyLockWarningTimer)
    uiP->timers.keyLockWarningTimer--;

    if (uiP->timers.iceOffWarningTimer)
    uiP->timers.iceOffWarningTimer--;

    if (uiP->timers.setChangeTimer)
    uiP->timers.setChangeTimer--;

    if (uiP->timers.customCalibrateTimer)
    uiP->timers.customCalibrateTimer--;

    // uiServiceButton20MsTask(consts, params); // moved to super 20 MS task
}

static void ui1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    
    if (uiP->timers.dspOffTimer)
      uiP->timers.dspOffTimer--;
#if (CONNECTIVITY != _NON_CONNECTED)
    if (iswifioff)
        iswifioff--;
    
    if (resetControlTimeout)
      resetControlTimeout--;
#endif    
    uiServiceButton1SecTask(consts, params);
}

static void uiTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiRam *uiP = params->ui;
    stRefrigerator_Ram *rfP = params->rf;
    const stRefrigerator_Flash *rfC = consts->rf;
    
    /*** APW : Add Start commercial mode ***/ 
    if(rfP->workMode.values.workMode == _WM_CommercialTest &&
       uiP->states.commercial.status == _UI_Commercial_None)
    {
        // Start CT when press the switch
        eventPost(eProcessRefrigerator, eSysEventStartCommercialTestMode, NULL, 0);
        uiP->states.commercial.status = _UI_Commercial_Test;
        uiP->states.commercial.FlagCommercialEnable = 1;
    }
    /*** APW : Add Start commercial mode ***/ 
    
    if(rfP->workMode.values.workMode == _WM_FTC)
      eventPost(eProcessRefrigerator, eSysEventStartFTCMode, NULL, 0);

    if (rfC->Parameters.autoDispOff)
    {
        if (rfP->workMode.values.workMode == _WM_Distributer /*|| WorkMode == _WM_FairMode*/)
            rfP->Values.dispOff = false;
        else
            rfP->Values.dispOff = true;
        // rfP->E2.dispOff = rfP->Values.dispOff;    //auto oldu�undan dolay� E2 yaz�m�na gerek yok.
    }
    else if (rfC->Parameters.dispOff)
    {
        if (!rfP->Values.dispOff)
            uiResetDisplayOffTimer(uiP);
    }

    if (uiP->timers.setChangeTimer)
    {
        rfEcoExtraResetTimer(rfC, rfP);
    }
    else
    {
        // rfEcoExtraToWork(rfC, rfP);
        rfSabbathToWork(rfC, rfP);
        rfSilentToWork(rfC, rfP);
    }
    uiServiceButtonTask(consts, params);
}

/****************** UI PROCESS STATES HANDLERS ******************/

/// @brief  Event handler function of uiProc
static PROCESS_HANDLER_PROTO(uiProcIdleStateHandler)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventInitAfterStartup:
        uiInit(consts, params);

        timerEventStart(&params->te.periodic20Ms, 20);
        timerEventStart(&params->te.periodic1Sec, 1000);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;

    default:
        uiProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(uiProcDefaultEventHandler)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    tfProcessHandler newHandler = NULL;

    switch (eventCurrent.event)
    {
    case eUiEventPeriodic20Ms:
        uiProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    case eUiEventPeriodic1Sec:
        uiProcSuper1SecTask(process);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;

    case eUiEventPeriodic1Min:
        uiProcSuper1MinTask(process);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;

    case eSysEventReinit:
        uiInit(consts, params);
        break;

    case eSysEventStartNormalMode:
        newHandler = uiProcNormalStateHandler;
        break;
    case eSysEventStartDistributerMode:
    case eSysEventStartFairMode:
        newHandler = uiProcDistributerStateHandler;
        break;
    case eSysEventStartApplianceOffMode:
        newHandler = uiProcAppOffStateHandler;
        break;
#ifdef SERVICEMODEavailable
    case eSysEventStartServiceMode:
        newHandler = uiProcServiceStateHandler;
        break;
#endif
    case eSysEventStartFTCMode:
        newHandler = uiProcFTCStateHandler;
        break;
    case eSysEventStartProductionTestMode:
        newHandler = uiProcProdTestStateHandler;
        break;
    /*** APW : Add operation state in CT ***/ 
    case eSysEventStartCommercialTestMode:
        newHandler = uiProcCommercialTestStateHandler;
        break;
    case eSysEventStartCommercialDefrostTestMode:
        newHandler = uiProcCommercialDefrostTestStateHandler;
        break;
    /*** APW : Add operation state in CT ***/ 
    }

    if (newHandler)
        sysProcChangeState(process, newHandler);
}

static PROCESS_HANDLER_PROTO(uiProcNormalStateHandler)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        break;

    case eUiEventPeriodic20Ms:
        uiTask(consts, params);
        uiNormalStateTask(consts, params);
        uiNormalStateKeyHandler(consts, params);
        uiWifiTask(consts, params);
        uiProcSuper20MsTask(process);

        if (params->ui->timers.setChangeTimer == 0)
        {
            for (uint8_t i = 0; i < _NumOfCabin; i++)
            {
                ceCabinWorkSets(&consts->rf->Cabin[i], &params->rf->Cabin[i], &params->rf->IceMachine, consts->rf, params->rf);
                cabinWorkCompSpeedUp(&consts->rf->Cabin[i], &params->rf->Cabin[i], consts->rf, params->rf);
            }
        }
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    case eUiEventPeriodic1Min:                                                  //APW : Update I2C driver for add CT state
        uiProcSuper1MinTask(process);                                           //APW : Update I2C driver for add CT state
        timerEventStart(&params->te.periodic1Min, 60 * 1000);                   //APW : Update I2C driver for add CT state
        break;

    default:
        uiProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(uiProcAppOffStateHandler)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        uiDspAllOff(consts, params);
        params->ui->dspPacket.funcs.ledApplianceOff = true;
        params->ui->dspPacket.funcs.ledWifi = true;
        break;

    case eUiEventPeriodic20Ms:
        ceRfApplianceOff(consts, params, isEvent(eDisplayEventApplianceOFF));
        uiProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    default:
        uiProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(uiProcProdTestStateHandler)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        uiNormalStateInit(consts, params);
#ifdef SERVICEMODEavailable
        uiSmCompsInit(consts, params);
        uiSmCompsCheckAll(consts, params);
        uiSmCompsCloseAll(consts, params);
        uiSmCompsTestInit(consts, params);
#endif

        break;

    case eUiEventPeriodic20Ms:
        uiTask(consts, params);
        uiNormalStateTask(consts, params);
        uiNormalStateKeyHandler(consts, params);
         if (params->ui->timers.setChangeTimer == 0)
        {
            for (uint8_t i = 0; i < _NumOfCabin; i++)
            {
                ceCabinWorkSets(&consts->rf->Cabin[i], &params->rf->Cabin[i], &params->rf->IceMachine, consts->rf, params->rf);
                cabinWorkCompSpeedUp(&consts->rf->Cabin[i], &params->rf->Cabin[i], consts->rf, params->rf);
            }
        }
        uiWifiTask(consts, params);
        uiProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    case eUiEventPeriodic1Min:                                                  //APW : Update I2C driver for add CT state
        uiProcSuper1MinTask(process);                                           //APW : Update I2C driver for add CT state
        timerEventStart(&params->te.periodic1Min, 60 * 1000);                   //APW : Update I2C driver for add CT state
        break;

    default:
        uiProcDefaultEventHandler(process);
        break;
    }
}
#ifdef SERVICEMODEavailable
static PROCESS_HANDLER_PROTO(uiProcServiceStateHandler)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        uiServiceStateInit(consts, params);
        params->ui->states.service.comps.index = 0;
        params->ui->states.service.params.index = 0;
        params->ui->states.service.sens.index = 0;
        break;

    case eUiEventPeriodic20Ms:
        uiServiceStateTask(consts, params);
        uiServiceStateKeyHandler(consts, params);
        uiProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    default:
        uiProcDefaultEventHandler(process);
        break;
    }
}
#endif
static PROCESS_HANDLER_PROTO(uiProcDistributerStateHandler)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        uiDistributerStateInit(consts, params);
        break;

    case eUiEventPeriodic20Ms:
        uiTask(consts, params);
        uiDistributerStateTask(consts, params);
        uiDistributerStateKeyHandler(consts, params);
        uiWifiTask(consts, params);
        uiProcSuper20MsTask(process);

        if (params->ui->timers.setChangeTimer == 0)
        {
            for (uint8_t i = 0; i < _NumOfCabin; i++)
                ceCabinWorkSets(&consts->rf->Cabin[i], &params->rf->Cabin[i], &params->rf->IceMachine, consts->rf, params->rf);
        }
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    default:
        uiProcDefaultEventHandler(process);
        break;
    }
}
static PROCESS_HANDLER_PROTO(uiProcFTCStateHandler)
{
    //uiProcDefaultEventHandler(process);
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        boardInitFTC();
        break;

    case eUiEventPeriodic20Ms:
        uiFTCState20MsTask(consts, params);
        boardFTC20MsTask();
        uiFTCStateTask(consts,params);
        uiFTCStateKeyHandler(consts,params);
        uiProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    default:
        uiProcDefaultEventHandler(process);
        break;
    }
}

/*** APW : Add CT heater software ***/
static PROCESS_HANDLER_PROTO(uiProcCommercialTestStateHandler)                  
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        uiCommercialStateInit(consts, params);                                  //APW : Update command CtmodeState and display in CT and others BUG about display
        break;

    case eUiEventPeriodic20Ms:
        uiTask(consts, params);
        uiCommercialStateTask(consts, params);                                  //Buttons enable in CT mode
        uiCommercialStateKeyHandler(consts, params); 
        uiWifiTask(consts, params);
        uiProcSuper20MsTask(process);
            
         if (params->ui->timers.setChangeTimer == 0)
        {
            for (uint8_t i = 0; i < _NumOfCabin; i++)
                ceCabinWorkSets(&consts->rf->Cabin[i], &params->rf->Cabin[i], &params->rf->IceMachine, consts->rf, params->rf);
        }
          
        uiServiceButtonTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);                          // Reset timer
        break;
          
    case eUiEventPeriodic1Min:                                                  //APW : Update I2C driver for add CT state
        uiProcSuper1MinTask(process);                                           //APW : Update I2C driver for add CT state
        timerEventStart(&params->te.periodic1Min, 60 * 1000);                   //APW : Update I2C driver for add CT state
        break;
          
    default:
        uiProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(uiProcCommercialDefrostTestStateHandler) 
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        //Force control panel disable
        params->ui->timers.dspOffTimer = 0;
        devDspSyncPacket(consts->dsp, &params->ui->dspPacket);  
        params->ui->dspPacket.funcs.ledDispOff = true;        //APW : (Disable) Fix bug display not off all led
        params->ui->dspPacket.funcs.displayOff = true;        //APW : Second display off
        uiDspIceCabinsSetsOff(consts, params);                //APW : Fix bug display not off all led
        uiDspDispenser(consts, params);                       //APW
        break;

    case eUiEventPeriodic20Ms:
        uiCommercialState20MsTask(consts, params);
        //Force control panel disable
        params->ui->timers.dspOffTimer = 0;
        devDspSyncPacket(consts->dsp, &params->ui->dspPacket);  
        params->ui->dspPacket.funcs.ledDispOff = true;        //APW : (Disable) Fix bug display not off all led
        params->ui->dspPacket.funcs.displayOff = true;        //APW : Second display off
        uiDspIceCabinsSetsOff(consts, params);                //APW : Fix bug display not off all led
        uiDspDispenser(consts, params);                       //APW
        
        timerEventStart(&params->te.periodic20Ms, 20);                          // Reset timer
        break;

    default:
        uiProcDefaultEventHandler(process);
        break;
    }
}
/*** APW : Add CT heater software ***/

static PROCESS_HANDLER_PROTO(uiProcSuper20MsTask)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    getNewEvent(consts, params);
    devDspSyncPacket(consts->dsp, &params->ui->dspPacket);
    uiNormalState20MsTask(consts, params);
#ifdef SERVICEMODEavailable
    uiServiceState20MsTask(consts, params);
#endif
    uiDistributerState20MsTask(consts, params);
    // uiFTCState20Ms(consts, params);
    //uiIceCreamMachine20MsTask(consts, params);
    ui20MsTask(consts, params);
    uiServiceButton20MsTask(consts, params);
      
    uiCommercialState20MsTask(consts, params);                                  
    
    /*** APW : Add command status Ct or normal ***/
    if(params->ui->states.commercial.status == _UI_Commercial_None)             
        params->ui->dspPacket.CtModeState = 0;                                  
    else                                                                        
        params->ui->dspPacket.CtModeState = 1;                                  
    /*** APW : Add command status Ct or normal ***/
}

static PROCESS_HANDLER_PROTO(uiProcSuper1SecTask)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    //uiIceCreamMachine1SecTask(consts, params);
    ui1SecTask(consts, params);
    uiPassword1SecTask(consts, params);
      
    uiCommercialState1SecTask(consts, params);                                  //APW : Update command CtmodeState and display in CT and others BUG about display
}

static PROCESS_HANDLER_PROTO(uiProcSuper1MinTask)
{
    tsUiProcParams *params = process->parameters;
    const tsUiProcConsts *consts = process->constants;

    uiServiceButton1MinTask(consts, params);
      
    uiCommercialState1MinTask(consts, params);                                  //APW : Update I2C driver for add CT state
          
}
/** @} */

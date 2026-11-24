/** @file       dispenser.c
 *  @brief      DISPENSER application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner & Maria Asif 
 *  @date       9/21/2024
 */
#define FILE_DISPENSER_C

#include "dispenser.h"
#include "dispenser_procedures.h"
#include "dev/synchronizer.h"

static PROCESS_HANDLER_PROTO(dispenserNormalStateHandler);
static PROCESS_HANDLER_PROTO(dispenserServiceStateHandler);
static PROCESS_HANDLER_PROTO(dispenserDistributerStateHandler);
static PROCESS_HANDLER_PROTO(dispenserAppOffStateHandler);
static PROCESS_HANDLER_PROTO(dispenserDefaultEventHandler);
static PROCESS_HANDLER_PROTO(dispenserIdleEventHandler);
static PROCESS_HANDLER_PROTO(dispenserProdTestStateHandler);
static PROCESS_HANDLER_PROTO(dispenserCommercialStateHandler);                  //APW : dispensor control in CT
static PROCESS_HANDLER_PROTO(dispenserCommercialDefrostStateHandler);           //APW : dispensor control in CT
static void dispenserInitDevices(const tsDispenserProcConsts *consts, tsDispenserProcParams *params);

// static PROCESS_HANDLER_PROTO(dispenserHandler);

/// @brief  Initialization function of dispenser
PROCESS_INIT_PROTO(dispenserInit)
{
    tsDispenserProcParams *params       = process->parameters;
    const tsDispenserProcConsts *consts = process->constants;

    params->dispenser->values.waterPumpTimer = _WATER_PUMP_TIME;
    params->dispenser->values.waterVacumTimer = _WATER_VACUM_TIME;
    params->dispenser->values.iceTimer = _ICE_TIME;  
    params->dispenser->values.iceWaitTimer = 0;
    params->dispenser->values.iceContinueTimer = _ICE_TIME_CONTINUE;
    params->dispenser->values.iceContinueFactorTimer = _FACTOR_CONTINUE;
    
    DispenserCheckState(consts->dispenser, params->dispenser);
    dispenserInitDevices(consts, params);
    
    PROCESS_STATE_CHANGE(process, dispenserIdleEventHandler);
    process->initialized = 1; 
}

/// @brief  Deinitialization function of dispenser
PROCESS_DEINIT_PROTO(dispenserDeinit)
{

    tsDispenserProcParams *params       = process->parameters;
    const tsDispenserProcConsts *consts = process->constants;

    timerEventStop(&params->te.periodic20Ms);
    timerEventStop(&params->te.periodic1Sec);

    UNUSED(consts); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of dispenser
static PROCESS_HANDLER_PROTO(dispenserIdleEventHandler)
{
    tsDispenserProcParams *params       = process->parameters;


    switch (eventCurrent.event)
    {
        case eSysEventInitAfterStartup:
        timerEventStart(&params->te.periodic20Ms, 20);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;

    default:
        dispenserDefaultEventHandler(process);
        break;
    }
}


static PROCESS_HANDLER_PROTO(dispenserNormalStateHandler)
{
    tsDispenserProcParams *params = process->parameters;
    const tsDispenserProcConsts *consts = process->constants;
    
    
    switch (eventCurrent.event)
    {
      case eDispenserEventPeriodic20Ms:
        DispenserT20ms(consts->dispenser, params->dispenser);
        DispenserTask(consts,params);
        if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          dispenserClearDisplaySetting20MsTask(params->dispenser);
        
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
      case eDispenserEventPeriodic1Sec:
        DispenserT1sec(params->dispenser);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;
      default:
        dispenserDefaultEventHandler(process);
        break;
    }
}

/*** APW : Dispenser control in CT ***/
static PROCESS_HANDLER_PROTO(dispenserCommercialStateHandler)
{
    tsDispenserProcParams *params = process->parameters;
    const tsDispenserProcConsts *consts = process->constants;
    
    
    switch (eventCurrent.event)
    {
      case eDispenserEventPeriodic20Ms:
        // Waiting to check operation
        DispenserT20ms(consts->dispenser, params->dispenser);
        DispenserTask(consts,params);
        if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          dispenserClearDisplaySetting20MsTask(params->dispenser);
        
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
      case eDispenserEventPeriodic1Sec:
        // Waiting to check operation
        DispenserT1sec(params->dispenser);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;
      default:
        dispenserDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(dispenserCommercialDefrostStateHandler)
{
    tsDispenserProcParams *params = process->parameters;
    const tsDispenserProcConsts *consts = process->constants;
    
    
    switch (eventCurrent.event)
    {
      case eDispenserEventPeriodic20Ms:
        // Waiting to check operation
        DispenserT20ms(consts->dispenser, params->dispenser);
        DispenserTask(consts,params);
        DispenserCloseComponents(params->dispenser);
        if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          dispenserClearDisplaySetting20MsTask(params->dispenser);
        
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
      case eDispenserEventPeriodic1Sec:
        // Waiting to check operation
        DispenserT1sec(params->dispenser);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;
      default:
        dispenserDefaultEventHandler(process);
        break;
    }
}
/*** APW : Dispenser control in CT ***/

static PROCESS_HANDLER_PROTO(dispenserDistributerStateHandler)
{
    tsDispenserProcParams *params = process->parameters;
    const tsDispenserProcConsts *consts = process->constants;
    
    switch (eventCurrent.event)
    {
      case eDispenserEventPeriodic20Ms:
        DispenserT20ms(consts->dispenser, params->dispenser);
        DispenserTask(consts,params);
        if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          dispenserClearDisplaySetting20MsTask(params->dispenser);
        
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
      case eDispenserEventPeriodic1Sec:
        DispenserT1sec(params->dispenser);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;
      default:
        dispenserDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(dispenserProdTestStateHandler)
{
    tsDispenserProcParams *params = process->parameters;
    const tsDispenserProcConsts *consts = process->constants;
    
    
    switch (eventCurrent.event)
    {
      case eDispenserEventPeriodic20Ms:
        DispenserT20ms(consts->dispenser, params->dispenser);
        DispenserTask(consts,params);
        if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          dispenserClearDisplaySetting20MsTask(params->dispenser);
        
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
      case eDispenserEventPeriodic1Sec:
        DispenserT1sec(params->dispenser);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;
      default:
        dispenserDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(dispenserServiceStateHandler)
{
    tsDispenserProcParams *params = process->parameters;
    
    switch (eventCurrent.event)
    {
      case eSysEventEnterState:
        DispenserInitAfterService(params->dispenser);
        break;
        
      default:
        dispenserDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(dispenserAppOffStateHandler)
{
    tsDispenserProcParams *params = process->parameters;
    
    switch (eventCurrent.event)
    {
      case eSysEventEnterState:
        DispenserCloseComponents(params->dispenser);
        break;
        
      default:
        dispenserDefaultEventHandler(process);
        break;
    }
}


static PROCESS_HANDLER_PROTO(dispenserDefaultEventHandler)
{
    tsDispenserProcParams *params = process->parameters;    
    tfProcessHandler newHandler = NULL;
    
    switch (eventCurrent.event)
    {
      case eDispenserEventPeriodic20Ms:
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
        
      case eDispenserEventPeriodic1Sec:
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;
        
      case eSysEventInitAfterHotStart:
        DispenserInitAfterHotStart(params->dispenser);
          break;
      case eSysEventReinit:
        break;
        
      case eSysEventStartNormalMode:
        newHandler = dispenserNormalStateHandler;
        break;
        
      case eSysEventStartDistributerMode:
      case eSysEventStartFairMode:
        newHandler = dispenserDistributerStateHandler;
        break;
        
      case eSysEventStartApplianceOffMode:
        newHandler = dispenserAppOffStateHandler;
        break;
        
      case eSysEventStartServiceMode:
        newHandler = dispenserServiceStateHandler;
        break;
        
      case eSysEventStartFTCMode:
        //newHandler = dispenserFTCStateHandler;
        break;
      case eSysEventStartProductionTestMode:
        newHandler = dispenserProdTestStateHandler;
        break;
      /*** APW : Dispenser control in CT ***/
      case eSysEventStartCommercialTestMode:
        newHandler = dispenserCommercialStateHandler;
        break;
        
      case eSysEventStartCommercialDefrostTestMode:
        newHandler = dispenserCommercialDefrostStateHandler;
        break;
      /*** APW : Dispenser control in CT ***/
    }
    
    if (newHandler)
      sysProcChangeState(process, newHandler);
}


static void dispenserInitDevices(const tsDispenserProcConsts *consts, tsDispenserProcParams *params)
{
     const stDispenserComponents_Flash *compC = &consts->dispenser->components;
    stDispenserComponents_Ram *compP = &params->dispenser->components;
    tsDispenserProcDevIos *pDev = &params->dev;
    const tsDevIo *dev = NULL;

    dev = boardInitDevice((teBoardDeviceType)compC->doorSwitch.type, compC->doorSwitch.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->doorSwitch.value);
    pDev->doorSwitch = dev;
    dev = boardInitDevice((teBoardDeviceType)compC->light.type, compC->light.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->light.value);
    pDev->light = dev;
    dev = boardInitDevice((teBoardDeviceType)compC->tactSwitch.type, compC->tactSwitch.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->tactSwitch.value);
    pDev->tactSwitch = dev;

}
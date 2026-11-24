/** @file       ice_source.c
 *  @brief      ICE_SOURCE application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       9/21/2024
 */
#define FILE_ICE_SOURCE_C

#include "ice_source.h"
#include "ice_source_procedures.h"
#include "dev/synchronizer.h"

#define DEBUG_FILE_NAME "icesource"




static PROCESS_HANDLER_PROTO(icesourceNormalStateHandler);
static PROCESS_HANDLER_PROTO(icesourceDefaultEventHandler);
static PROCESS_HANDLER_PROTO(icesourceIdleEventHandler);
static PROCESS_HANDLER_PROTO(icesourceProdTestStateHandler);
static PROCESS_HANDLER_PROTO(icesourceServiceStateHandler);
static PROCESS_HANDLER_PROTO(icesourceAppOffStateHandler);
static PROCESS_HANDLER_PROTO(icesourceDistributerStateHandler);
static PROCESS_HANDLER_PROTO(icesourceCommercialStateHandler);                  //APW : Ice source control in CT
static PROCESS_HANDLER_PROTO(icesourceCommercialDefrostStateHandler);           //APW : Ice source control in CT
static void IcesourceInitDevices(const tsIcesourceProcConsts *consts, tsIcesourceProcParams *params);

/// @brief  Initialization function of icesource
PROCESS_INIT_PROTO(icesourceInit)
{

    tsIcesourceProcParams *params       = process->parameters;
    const tsIcesourceProcConsts *consts = process->constants;
    IcesourceInitDevices(consts, params);


    params->icesource->values.selectorState = _SelectorState_Unknown;
    params->icesource->values.shutterState = _ShutterState_Unknown;
    params->icesource->values.pusherState = _PusherState_OFF;

    params->icesource->values.selectorTimer = 0;
    params->icesource->values.shutterTimer = 0;
    params->icesource->values.startTimer = 0;
    params->icesource->values.stopTimer = 0;
    params->icesource->values.delaySelectionTimer = 0;
    params->icesource->values.delayShutterTimer = 0xFF;
    
    params->icesource->components.shutter.beforeOpening = false;

    params->icesource->values.dispenserDesired = _IceSourceValue_OFF;
    params->icesource->values.dispenserApplied = _IceSourceValue_OFF;
    PROCESS_STATE_CHANGE(process, icesourceIdleEventHandler);
}

/// @brief  Deinitialization function of icesource
PROCESS_DEINIT_PROTO(icesourceDeinit)
{
    tsIcesourceProcParams *params       = process->parameters;
    const tsIcesourceProcConsts *consts = process->constants;
    UNUSED(consts); // REMOVE IF USED
    PROCESS_STATE_CHANGE(process, NULL);

    timerEventStop(&params->te.periodic20Ms);

    process->initialized = 0; // If process needs other checks, clear this another time
}
static PROCESS_HANDLER_PROTO(icesourceIdleEventHandler)
{
    tsIcesourceProcParams *params    = process->parameters;


    switch (eventCurrent.event)
    {
        case eSysEventInitAfterStartup:
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    default:
        icesourceDefaultEventHandler(process);
        break;
    }
}
static PROCESS_HANDLER_PROTO(icesourceNormalStateHandler)
{
    tsIcesourceProcParams *params       = process->parameters;
    const tsIcesourceProcConsts *consts = process->constants;
    
    
    switch (eventCurrent.event)
    {
      case eIceSourceEventPeriodic20Ms:
        IceSourceT20ms(params->icesource);
        IceSourceTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);
        break; 
      default:
        icesourceDefaultEventHandler(process);
        break;
    }
}

/*** APW : Ice source control in CT ***/
static PROCESS_HANDLER_PROTO(icesourceCommercialStateHandler)
{
    tsIcesourceProcParams *params       = process->parameters;
    const tsIcesourceProcConsts *consts = process->constants;
    
    
    switch (eventCurrent.event)
    {
      case eIceSourceEventPeriodic20Ms:
        // Waiting to check operation
        IceSourceT20ms(params->icesource);
        IceSourceTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);
        break; 
      default:
        icesourceDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(icesourceCommercialDefrostStateHandler)
{
    tsIcesourceProcParams *params       = process->parameters;
    const tsIcesourceProcConsts *consts = process->constants;
    
    
    switch (eventCurrent.event)
    {
      case eIceSourceEventPeriodic20Ms:
        // Waiting to check operation
        IceSourceT20ms(params->icesource);
        IceSourceTask(consts, params);
        IceSourceCloseComponents(params->icesource);
        timerEventStart(&params->te.periodic20Ms, 20);
        break; 
      default:
        icesourceDefaultEventHandler(process);
        break;
    }
}

/*** APW : Ice source control in CT ***/

static PROCESS_HANDLER_PROTO(icesourceDistributerStateHandler)
{
    tsIcesourceProcParams *params = process->parameters;
    const tsIcesourceProcConsts *consts = process->constants;
    
    switch (eventCurrent.event)
    {
      case eIceSourceEventPeriodic20Ms:
        IceSourceT20ms(params->icesource);
        IceSourceTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

      default:
        icesourceDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(icesourceProdTestStateHandler)
{
    tsIcesourceProcParams *params = process->parameters;
    const tsIcesourceProcConsts *consts = process->constants;
    
    
    switch (eventCurrent.event)
    {
      case eIceSourceEventPeriodic20Ms:
        IceSourceT20ms(params->icesource);
        IceSourceTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
      default:
        icesourceDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(icesourceServiceStateHandler)
{
    icesourceDefaultEventHandler(process);
}

static PROCESS_HANDLER_PROTO(icesourceAppOffStateHandler)
{
    tsIcesourceProcParams *params = process->parameters;
    const tsIcesourceProcConsts *consts = process->constants;
    
    switch (eventCurrent.event)
    { 
      case eSysEventEnterState:
        IceSourceComponentsTest(consts,params);
        break;
        
      default:
        icesourceDefaultEventHandler(process);
        break;
    }
}


static PROCESS_HANDLER_PROTO(icesourceDefaultEventHandler)
{
    tsIcesourceProcParams *params       = process->parameters;   
    tfProcessHandler newHandler = NULL;
    
    switch (eventCurrent.event)
    {
      case eIceSourceEventPeriodic20Ms:
        timerEventStart(&params->te.periodic20Ms, 20);
        break;     
        
      case eSysEventInitAfterHotStart:
      case eSysEventReinit:
        break;
        
      case eSysEventStartNormalMode: 
        newHandler = icesourceNormalStateHandler;
        break;
        
      case eSysEventStartDistributerMode:
      case eSysEventStartFairMode:
        newHandler = icesourceDistributerStateHandler;
        break;
        
      case eSysEventStartApplianceOffMode:
        newHandler = icesourceAppOffStateHandler;
        break;
        
      case eSysEventStartServiceMode:
        newHandler = icesourceServiceStateHandler;
        break;
        
      case eSysEventStartFTCMode:
        //newHandler = icesourceFTCStateHandler;
        break;
      case eSysEventStartProductionTestMode:
        newHandler = icesourceProdTestStateHandler;
        break;
      /*** APW : Ice source control in CT ***/
      case eSysEventStartCommercialTestMode:
        newHandler = icesourceCommercialStateHandler;
        break;
        
      case eSysEventStartCommercialDefrostTestMode:
        newHandler = icesourceCommercialDefrostStateHandler;
        break;
      /*** APW : Ice source control in CT ***/
    }
    
    if (newHandler)
      sysProcChangeState(process, newHandler);
}

static void IcesourceInitDevices(const tsIcesourceProcConsts *consts, tsIcesourceProcParams *params)
{
    const stIceSourceComponents_Flash *compC = &consts->icesource->components;
    stIceSourceComponents_Ram *compP = &params->icesource->components;
    tsIcesourceProcDevIos *pDev = &params->dev;
    const tsDevIo *dev = NULL;

    dev = boardInitDevice((teBoardDeviceType)compC->pusher.type, compC->pusher.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->pusher.value);
    pDev->pusher = dev;
    dev = boardInitDevice((teBoardDeviceType)compC->selector.type, compC->selector.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->selector.value);
    pDev->selector = dev;
    dev = boardInitDevice((teBoardDeviceType)compC->shutter.type, compC->shutter.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->shutter.value);
    pDev->shutter = dev;
}

/** @} */
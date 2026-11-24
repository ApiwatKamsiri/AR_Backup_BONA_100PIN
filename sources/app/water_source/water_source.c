/** @file       water_source.c
 *  @brief      WATER_SOURCE application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       8/21/2024
 */
#define FILE_WATER_SOURCE_C

#include "dev/synchronizer.h"
#include "water_source.h"
#include "water_source_procedures.h"
#define _FILTER_TIME            3120    // 130day * 24hour = 3120 hour  
#define _HEATER_ON_TIME         2       // (dk*60) sec
#define _HEATER_OFF_TIME        100     // (dk*60) sec 


/**
 *  @addtogroup WATER_SOURCE
 *  @{
 */


static PROCESS_HANDLER_PROTO(WaterSourceIdleEventHandler);
static PROCESS_HANDLER_PROTO(WaterSourceDefaultEventHandler);
static PROCESS_HANDLER_PROTO(WaterSourceAppOffStateHandler);
static PROCESS_HANDLER_PROTO(WaterSourceDistributerStateHandler);
static PROCESS_HANDLER_PROTO(WaterSourceServiceStateHandler);
static PROCESS_HANDLER_PROTO(WaterSourceProdTestStateHandler);
static PROCESS_HANDLER_PROTO(WaterSourceNormalStateHandler);
static PROCESS_HANDLER_PROTO(WaterSourceCommercialStateHandler);                //APW : Add Ice making operation test in CT
static PROCESS_HANDLER_PROTO(WaterSourceCommercialDefrostStateHandler);         //APW : Add Ice making operation test in CT
static void watersourceInitDevices(const tsWaterSourceProcConsts *consts, tsWaterSourceProcParams *params);
/// @brief  Initialization function of watersource
PROCESS_INIT_PROTO(WaterSourceInit)
{
    tsWaterSourceProcParams *params       = process->parameters;
    const tsWaterSourceProcConsts *consts = process->constants;

    //processStart(sysProcesses[eProcessWaterSource]);
    watersourceInitDevices(consts, params);
    PROCESS_STATE_CHANGE(process, WaterSourceIdleEventHandler);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of watersource
PROCESS_DEINIT_PROTO(WaterSourceDeinit)
{
    tsWaterSourceProcParams *params       = process->parameters;
    PROCESS_STATE_CHANGE(process, NULL);

    timerEventStop(&params->te.periodic20Ms);
    timerEventStop(&params->te.periodic1Min);
    timerEventStop(&params->te.periodic1Sec);
    timerEventStop(&params->te.periodic1Hour);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of watersource

static PROCESS_HANDLER_PROTO(WaterSourceDefaultEventHandler)
{
    tsWaterSourceProcParams *params = process->parameters;
    const tsWaterSourceProcConsts *consts = process->constants;
    
    tfProcessHandler newHandler = NULL;

    switch (eventCurrent.event)
    {
    case eWaterSourceEventPeriodic20Ms:
        WaterSource_T20ms(params->watersource);
        timerEventStart(&params->te.periodic20Ms, 20);   
        break;

    case eWaterSourceEventPeriodic1Sec:
        WaterSource_T1Sec(params->watersource);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;

    case eWaterSourceEventPeriodic1Min:
        WaterSource_T1min(consts, params);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;

    case eWaterSourceEventPeriodic1Hour:
        WaterSource_T1hour(params->watersource);
        timerEventStart(&params->te.periodic1Hour, 3600 * 1000);
        break;

    case eSysEventInitAfterHotStart:
    case eSysEventReinit:
        break;

    case eSysEventStartNormalMode:
        newHandler = WaterSourceNormalStateHandler;
        break;

    case eSysEventStartDistributerMode:

    case eSysEventStartFairMode:
        newHandler = WaterSourceDistributerStateHandler;
        break;

    case eSysEventStartApplianceOffMode:
        newHandler = WaterSourceAppOffStateHandler;
        break;

    case eSysEventStartServiceMode:
        newHandler = WaterSourceServiceStateHandler;
        break;

    case eSysEventStartProductionTestMode:
        newHandler = WaterSourceProdTestStateHandler;
        break;
        
    /*** APW : Add Ice making operation test in CT ***/ 
    case eSysEventStartCommercialTestMode:
        newHandler = WaterSourceCommercialStateHandler;
        break;
        
    case eSysEventStartCommercialDefrostTestMode:
        newHandler = WaterSourceCommercialDefrostStateHandler;
        break;
    /*** APW : Add Ice making operation test in CT ***/    
        
    }

    if (newHandler)
        sysProcChangeState(process, newHandler);
        
}

static PROCESS_HANDLER_PROTO(WaterSourceIdleEventHandler)
{
    tsWaterSourceProcParams *params       = process->parameters;
    const tsWaterSourceProcConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
        case EVENT_NONE: // FILL WITH NEW EVENTS
            break;

        default:
            WaterSourceDefaultEventHandler(process);
            break;
        case eSysEventInitAfterStartup: 
          
            refrigerator_R.WaterSource.values.SkipEmptyCheck = true;
          
            timerEventStart(&params->te.periodic20Ms, 20);
            timerEventStart(&params->te.periodic1Sec, 1000);
            timerEventStart(&params->te.periodic1Min, 60 * 1000);
            timerEventStart(&params->te.periodic1Hour, 3600 * 1000);
        break;
 
    }

}

static PROCESS_HANDLER_PROTO(WaterSourceServiceStateHandler)
{
    tsWaterSourceProcParams *params       = process->parameters;
    const tsWaterSourceProcConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
      default:
            WaterSourceDefaultEventHandler(process);
            break;
    }
}

static void watersourceInitDevices(const tsWaterSourceProcConsts *consts, tsWaterSourceProcParams *params)
{
   const stWaterSourceComponents_Flash *compC = &consts->watersource->components;
    stWaterSourceComponents_Ram *compP = &params->watersource->components;
    tsWaterSourceProcDevIos *pDev = &params->dev;
    const tsDevIo *dev = NULL;
    
    dev = boardInitDevice((teBoardDeviceType)compC->waterIceTankEmptySensor.type, compC->waterIceTankEmptySensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->waterIceTankEmptySensor.valueInCExt);
    pDev->waterIceTankEmptySensor = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->waterIceTankEmptySensor.type, compC->waterIceTankEmptySensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->waterIceTankEmptySensor.valueInCExt);
    pDev->waterIceTankEmptySensor = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->waterDisTankEmptySensor.type, compC->waterDisTankEmptySensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->waterDisTankEmptySensor.valueInCExt);
    pDev->waterDisTankEmptySensor = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->mainPump.type, compC->mainPump.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->mainPump.value);
    pDev->mainPump = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->dispenserPump.type, compC->dispenserPump.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->dispenserPump.value);
    pDev->dispenserPump = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->iceMachinePump.type, compC->iceMachinePump.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->iceMachinePump.value);
    pDev->iceMachinePump = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->mainValve.type, compC->mainValve.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->mainValve.value);
    pDev->mainValve = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->dispenserValve.type, compC->dispenserValve.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->dispenserValve.value);
    pDev->dispenserValve = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->iceMachineValve.type, compC->iceMachineValve.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->iceMachineValve.value);
    pDev->iceMachineValve = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->waterVentilHeater.type, compC->waterVentilHeater.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->waterVentilHeater.value);
    pDev->waterVentilHeater = dev;
    dev = boardInitDevice((teBoardDeviceType)compC->waterCaseSwitch.type, compC->waterCaseSwitch.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->waterCaseSwitch.value);
    pDev->waterCaseSwitch = dev;
}

static PROCESS_HANDLER_PROTO(WaterSourceNormalStateHandler)
{   
    tsWaterSourceProcParams *params       = process->parameters;
    const tsWaterSourceProcConsts *consts = process->constants;

   switch (eventCurrent.event)
   {
     case eWaterSourceEventPeriodic20Ms:
       WaterSourceTask(consts, params);
       WaterSource_T20ms(params->watersource);
       timerEventStart(&params->te.periodic20Ms, 20);   
       break;
       
     case eWaterSourceEventPeriodic1Sec:
       WaterSource_T1Sec(params->watersource);
       timerEventStart(&params->te.periodic1Sec, 1000);
       break;
       
     case eWaterSourceEventPeriodic1Min:
       WaterSource_T1min(consts, params);
       timerEventStart(&params->te.periodic1Min, 60* 1000);
       break;
       
     case eWaterSourceEventPeriodic1Hour:
       WaterSource_T1hour(params->watersource);
       timerEventStart(&params->te.periodic1Hour, 3600 * 1000);
       break;
       
     default:
       WaterSourceDefaultEventHandler(process);
       break;
   }
}

static PROCESS_HANDLER_PROTO(WaterSourceProdTestStateHandler)
{
    tsWaterSourceProcParams *params       = process->parameters;
    const tsWaterSourceProcConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
      case eWaterSourceEventPeriodic20Ms:
        WaterSourceComponentsTest(consts, params);
        WaterSource_T20ms(params->watersource);
        timerEventStart(&params->te.periodic20Ms, 20);   
        break;
        
      case eWaterSourceEventPeriodic1Sec:
        WaterSource_T1Sec(params->watersource);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;
        
      case eWaterSourceEventPeriodic1Min:
        WaterSource_T1min(consts, params);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;
        
      case eWaterSourceEventPeriodic1Hour:
        WaterSource_T1hour(params->watersource);
        timerEventStart(&params->te.periodic1Hour, 3600 * 1000);
        break;
      default:
        WaterSourceDefaultEventHandler(process);
        break;
    }
}

/*** APW : Add Ice making operation test in CT ***/
static PROCESS_HANDLER_PROTO(WaterSourceCommercialStateHandler)
{   
    tsWaterSourceProcParams *params       = process->parameters;
    const tsWaterSourceProcConsts *consts = process->constants;

   switch (eventCurrent.event)
   {
     case eSysEventEnterState:
       params->watersource->values.componentsTestDone = true;
       break;
     
     case eWaterSourceEventPeriodic20Ms:
       WaterSourceTask(consts, params);
       WaterSource_T20ms(params->watersource);
       timerEventStart(&params->te.periodic20Ms, 20);   
       break;
       
     case eWaterSourceEventPeriodic1Sec:
       WaterSource_T1Sec(params->watersource);
       timerEventStart(&params->te.periodic1Sec, 1000);
       break;
       
     case eWaterSourceEventPeriodic1Min:
       WaterSource_T1min(consts, params);
       timerEventStart(&params->te.periodic1Min, 60 * 1000);
       break;
       
     case eWaterSourceEventPeriodic1Hour:
       WaterSource_T1hour(params->watersource);
       timerEventStart(&params->te.periodic1Hour, 3600 * 1000);
       break;
       
     default:
       WaterSourceDefaultEventHandler(process);
       break;
   }
}
static PROCESS_HANDLER_PROTO(WaterSourceCommercialDefrostStateHandler)
{   
    tsWaterSourceProcParams *params       = process->parameters;
    const tsWaterSourceProcConsts *consts = process->constants;

   switch (eventCurrent.event)
   {
     case eSysEventEnterState:
       params->watersource->values.componentsTestDone = true;
       break;
     
     case eWaterSourceEventPeriodic20Ms:
       WaterSourceTask(consts, params);
       WaterSource_T20ms(params->watersource);
       WaterSourceCloseComponents(params->watersource);
       timerEventStart(&params->te.periodic20Ms, 20);   
       break;
       
     /*case eWaterSourceEventPeriodic1Sec:
       WaterSource_T1Sec(params->watersource);
       timerEventStart(&params->te.periodic1Sec, 60 * 1000);
       break;
       
     case eWaterSourceEventPeriodic1Min:
       WaterSource_T1min(consts, params);
       timerEventStart(&params->te.periodic1Min, 3600 * 1000);
       break;
       
     case eWaterSourceEventPeriodic1Hour:
       WaterSource_T1hour(params->watersource);
       timerEventStart(&params->te.periodic1Hour, 3600 *60 * 1000);
       break;*/
       
     default:
       WaterSourceDefaultEventHandler(process);
       break;
   }
}
/*** APW : Add Ice making operation test in CT ***/

static PROCESS_HANDLER_PROTO(WaterSourceDistributerStateHandler)
{
tsWaterSourceProcParams *params       = process->parameters;
    const tsWaterSourceProcConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
      case eWaterSourceEventPeriodic20Ms:
        WaterSourceTask(consts, params);
        WaterSource_T20ms(params->watersource);
        timerEventStart(&params->te.periodic20Ms, 20);   
        break;
        
      case eWaterSourceEventPeriodic1Sec:
        WaterSource_T1Sec(params->watersource);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;
        
      case eWaterSourceEventPeriodic1Min:
        WaterSource_T1min(consts, params);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;
        
      case eWaterSourceEventPeriodic1Hour:
        WaterSource_T1hour(params->watersource);
        timerEventStart(&params->te.periodic1Hour, 3600 * 1000);
        break;
      default:
        WaterSourceDefaultEventHandler(process);
        break;
    }
}
static PROCESS_HANDLER_PROTO(WaterSourceAppOffStateHandler)
{
    tsWaterSourceProcParams *params       = process->parameters;
       
        switch (eventCurrent.event)
        {
          case eSysEventEnterState:
            WaterSourceCloseComponents(params->watersource);
            break;
            
          default:
            WaterSourceDefaultEventHandler(process);
            break;
        }

}


/** @} */

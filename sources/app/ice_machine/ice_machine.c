/** @file       ice_machine.c
 *  @brief      ICE_MACHINE application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner & Obaid Hasan Khan
 *  @date       8/2/2024
 */
#define FILE_ICE_MACHINE_C

#include "ice_machine.h"
#include "ice_machine_procedures.h"
#include "dev/synchronizer.h"
#define DEBUG_FILE_NAME "ice_machine"

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup ICE_MACHINE
 *  @{
 */



static PROCESS_HANDLER_PROTO(icemachineIdleHandler);
static PROCESS_HANDLER_PROTO(icemachineNormalStateHandler);
static PROCESS_HANDLER_PROTO(icemachineProdTestStateHandler);
static PROCESS_HANDLER_PROTO(icemachineServiceTestStateHandler);
static PROCESS_HANDLER_PROTO(icemachineAppOffStateHandler);
static PROCESS_HANDLER_PROTO(icemachineDefaultEventHandler);
static PROCESS_HANDLER_PROTO(icemachineCommercialStateHandler);             //APW : Add Ice making operation test in CT
static PROCESS_HANDLER_PROTO(icemachineCommercialDefrostStateHandler);      //APW : Add Ice making operation test in CT

static void IceMachineinitdevices(const tsIcemachineProcConsts *consts, tsIcemachineProcParams *params);


/// @brief  Initialization function of icemachine
PROCESS_INIT_PROTO(icemachineInit)
{
    tsIcemachineProcParams *params       = process->parameters;
    const tsIcemachineProcConsts *consts = process->constants;
    
    IceMachineinitdevices(consts, params);
    IceMachineE2Init(consts->icemachine, params->icemachine);
    
    params->icemachine->values.firstIce                    = true;
    params->icemachine->values.state                       = _IMch_State_InitCase;
    params->icemachine->values.step                        = _IMch_Step_6;
    params->icemachine->values.block                       = true;
    params->icemachine->values.errCaseTactSW               = false;
    params->icemachine->values.caseTactSWErrorTimer        = _CaseTactSWErrorTime;
    params->icemachine->values.caseTactSWErrorReTryTimer   = 0;
    params->icemachine->values.makeIceTimer                = consts->icemachine->parameters.makeIceLongWaitTime;
    params->icemachine->values.noWaterSet                  = false;
    params->icemachine->values.noWaterClr                  = false;
    params->icemachine->values.pureCounter                 = 0;
    params->icemachine->values.quickIce                    = false;
    if (!params->icemachine->sets.waterPumpTime)
        params->icemachine->sets.waterPumpTime             = consts->icemachine->parameters.waterPumpTime;
    //iceMachineP->values.HBY_FFSet = iceMachineC->parameters.HBY_FFSet;
    params->icemachine->sets.quickIce60WaterPumpTime     = consts->icemachine->parameters.quickIce60WaterPumpTime;    //<<QuickIce60
    params->icemachine->sets.quickIce60MaximumTime       = consts->icemachine->parameters.quickIce60MaximumTime;      //<<QuickIce60
    params->icemachine->sets.quickIce60MaximumTimes       = consts->icemachine->parameters.quickIce60MaximumTimes;      //<<QuickIce60
    params->icemachine->sets.quickIceOnDisplay          = consts->icemachine->parameters.quickIceOnDisplay;
    params->icemachine->values.HBY_SetDecrease             = consts->icemachine->parameters.HBY_SetDecrease;
    params->icemachine->values.HBY_CaseTempThermalCutOut   = consts->icemachine->parameters.HBY_CaseTempThermalCutOut;
    params->icemachine->values.boxFullActive = false;
    //Check power on
    params->icemachine->values.powerOn = true;
    PROCESS_STATE_CHANGE(process, icemachineIdleHandler);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of icemachine
PROCESS_DEINIT_PROTO(icemachineDeinit)
{
    tsIcemachineProcParams *params       = process->parameters;
    const tsIcemachineProcConsts *consts = process->constants;

    timerEventStop(&params->te.periodic20Ms);
    timerEventStop(&params->te.periodic100Ms);
    timerEventStop(&params->te.periodic1sec);
    timerEventStop(&params->te.periodic1min);
    UNUSED(consts); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of icemachine
static PROCESS_HANDLER_PROTO(icemachineIdleHandler)
{
    tsIcemachineProcParams *params       = process->parameters;
    //const tsIcemachineProcConsts *consts = process->constants;
    switch (eventCurrent.event)
    {
        case eSysEventInitAfterStartup: // FILL WITH NEW EVENTS
            IceMachineE2ToRam(params->icemachine); 
            timerEventStart(&params->te.periodic20Ms, 20);
            timerEventStart(&params->te.periodic100Ms, 100);
            timerEventStart(&params->te.periodic1sec, 1000);
            timerEventStart(&params->te.periodic1min, 60*1000);
            break;

        default:
        icemachineDefaultEventHandler(process);
            break;
    }
}

static void IceMachineinitdevices(const tsIcemachineProcConsts *consts, tsIcemachineProcParams *params)
{
    const stIceMachineComponents_Flash *compC = &consts->icemachine->components;
    stIceMachineComponents_Ram *compP = &params->icemachine->components;
    tsIceMachineProcDevIos *pDev = &params->dev;
    const tsDevIo *dev = NULL;
    
    dev = boardInitDevice((teBoardDeviceType)compC->caseTempSensor.type, compC->caseTempSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->caseTempSensor.valueInCExt);
    pDev->caseTempSensor = dev;
      
    dev = boardInitDevice((teBoardDeviceType)compC->caseTempSensor.type, compC->caseTempSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->caseTempSensor.valueInCExt);
    pDev->caseTempSensorError = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->caseTactSwitch.type, compC->caseTactSwitch.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->caseTactSwitch.value);
    pDev->caseTactSwitch = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->boxTactSwitch.type, compC->boxTactSwitch.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->boxTactSwitch.value);
    pDev->boxTactSwitch = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->caseRotator.type, compC->caseRotator.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->caseRotator.value);
    pDev->caseRotator = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->fan.type, compC->fan.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->fan.speedOrPWM);
    pDev->fan = dev;

    dev = boardInitDevice((teBoardDeviceType)eDeviceTypeFanErrorDetector, compC->fan.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->fan.error);
    pDev->fan = dev;

}

static PROCESS_HANDLER_PROTO(icemachineNormalStateHandler)
{
    tsIcemachineProcParams *params = process->parameters;
    const tsIcemachineProcConsts *consts = process->constants; 

   switch (eventCurrent.event)
   {
     /*** APW : Add Ice making operation test in CT ***/
     case eSysEventEnterState:
       if(params->rf->ui.states.commercial.status != _UI_Commercial_None)
            params->icemachine->values.state = _IMch_CommercialState_ReadyToTest;
       
       break;
     /*** APW : Add Ice making operation test in CT ***/
       
     case eIceMachineEventPeriodic20ms:
       IceMachineT20ms(params->icemachine);
       IceMachineTask(consts, params);
       if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
        IceMachineClearDisplaySetting20MsTask(params->icemachine);
      
       timerEventStart(&params->te.periodic20Ms, 20);
       break;
       
     case eIceMachineEventPeriodic100ms:
       IceMachineT100ms(params->icemachine);
       timerEventStart(&params->te.periodic100Ms, 100);
       break;
       
     default:
       icemachineDefaultEventHandler(process);
       break;
   }
     

}

static PROCESS_HANDLER_PROTO(icemachineProdTestStateHandler)
{
    tsIcemachineProcParams *params = process->parameters;
    const tsIcemachineProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
        case eIceMachineEventPeriodic20ms:
            IceMachineComponentsTest(consts, params);
            IceMachineT20ms(params->icemachine);
            if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
              IceMachineClearDisplaySetting20MsTask(params->icemachine);
        
            timerEventStart(&params->te.periodic20Ms, 20);
        break;
        
        case eIceMachineEventPeriodic100ms:
          IceMachineT100ms(params->icemachine);
          timerEventStart(&params->te.periodic100Ms, 100);
        break;

        case eIceMachineEventPeriodic1sec:
            IceMachineT1sec(params->icemachine);
            timerEventStart(&params->te.periodic1sec, 1000);
        break;

        case eIceMachineEventPeriodic1min:
            IceMachineT1min(params->icemachine);
            timerEventStart(&params->te.periodic1min, 60*1000);
        break;

        default:
            icemachineDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(icemachineServiceTestStateHandler)
{
    tsIcemachineProcParams *params = process->parameters;
    switch (eventCurrent.event)
    {
      case eSysEventEnterState:
        IceMachineInitAfterService(params->icemachine);
        break;
        
      default:
        icemachineDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(icemachineAppOffStateHandler)
{
    tsIcemachineProcParams *params = process->parameters;
    switch (eventCurrent.event)
    {
      case eSysEventEnterState:
        IceMachineCloseComponents(params->icemachine);
        break;
        
      default:
        icemachineDefaultEventHandler(process);
        break;
    }
}
/*** APW : Add Ice making operation test in CT ***/
static PROCESS_HANDLER_PROTO(icemachineCommercialStateHandler)
{
    tsIcemachineProcParams *params = process->parameters;
    const tsIcemachineProcConsts *consts = process->constants; 

   switch (eventCurrent.event)
   {
     case eSysEventEnterState:
       params->icemachine->values.componentsTestDone = true;
       params->icemachine->values.QuickIce60 = false;
       params->icemachine->values.state = _IMch_CommercialState_ReadyToTest;
       params->icemachine->values.LEDIceBlinkInCt_20ms = 0;      
       
       break;
       
     case eIceMachineEventPeriodic20ms:
       IceMachineT20ms(params->icemachine);
       IceMachineTask(consts, params);
       if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
        IceMachineClearDisplaySetting20MsTask(params->icemachine);
       
       timerEventStart(&params->te.periodic20Ms, 20);
       break;
       
     default:
       icemachineDefaultEventHandler(process);
       break;
   }
}

static PROCESS_HANDLER_PROTO(icemachineCommercialDefrostStateHandler)
{
    tsIcemachineProcParams *params = process->parameters;
    const tsIcemachineProcConsts *consts = process->constants; 

   switch (eventCurrent.event)
   {
     case eSysEventEnterState:
       params->icemachine->values.componentsTestDone = true;
       params->icemachine->values.state = _IMch_CommercialDefrostState_IceBoxNotOperate;
       params->icemachine->values.LEDIceBlinkInCt_20ms = 0xFF;
       break;
       
     case eIceMachineEventPeriodic20ms:
       IceMachineT20ms(params->icemachine);
       IceMachineTask(consts, params);
       if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
        IceMachineClearDisplaySetting20MsTask(params->icemachine);
       
       timerEventStart(&params->te.periodic20Ms, 20);
       break;
       
     default:
       icemachineDefaultEventHandler(process);
       break;
   }
}
/*** APW : Add Ice making operation test in CT ***/
static PROCESS_HANDLER_PROTO(icemachineDefaultEventHandler)
{
  tsIcemachineProcParams *params = process->parameters;
  //const tsIcemachineProcConsts *consts = process->constants;
  
   tfProcessHandler newHandler = NULL;

     switch (eventCurrent.event)
     {
       case eIceMachineEventPeriodic20ms:
         IceMachineT20ms(params->icemachine);
         timerEventStart(&params->te.periodic20Ms, 20);
         break;
         
       case eIceMachineEventPeriodic100ms:
         IceMachineT100ms(params->icemachine);
         timerEventStart(&params->te.periodic100Ms, 100);
         break;
         
       case eIceMachineEventPeriodic1sec:
         IceMachineT1sec(params->icemachine);
         timerEventStart(&params->te.periodic1sec, 1000);
         break;
         
       case eIceMachineEventPeriodic1min:
         IceMachineT1min(params->icemachine);
         timerEventStart(&params->te.periodic1min, 60*1000);
         break;
         
       case eSysEventInitAfterHotStart:
         IceMachineInitAfterHotStart(params->icemachine);
         break;
       case eSysEventReinit:
       case eSysEventStartDistributerMode:
       case eSysEventStartFairMode:
       case eSysEventCalibrateAllStepperMotors:
         break;
       case eSysEventStartNormalMode:
         newHandler = icemachineNormalStateHandler;
         break;
        
       case eSysEventStartApplianceOffMode:
         newHandler = icemachineAppOffStateHandler;
         break;
         
       case eSysEventStartServiceMode:
         newHandler = icemachineServiceTestStateHandler;
         break;
         
       case eSysEventStartProductionTestMode:
         newHandler = icemachineProdTestStateHandler;
         break;
         
       /*** APW : Add Ice making operation test in CT ***/  
       case eSysEventStartCommercialTestMode:
         newHandler = icemachineCommercialStateHandler;
         break;
         
       case eSysEventStartCommercialDefrostTestMode:
        newHandler = icemachineCommercialDefrostStateHandler;
        break;
       /*** APW : Add Ice making operation test in CT ***/  
       
     }
     if (newHandler)
       sysProcChangeState(process, newHandler);
}




/** @} */

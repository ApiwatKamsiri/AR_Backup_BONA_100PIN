/** @file       refrigerator.c
 *  @brief      REFRIGERATOR application program file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-10
 */
#define FILE_REFRIGERATOR_C

#include "refrigerator.h"

#include "procedures/rf_doors_alert.h"
#include "procedures/rf_doors_closed.h"
#include "procedures/rf_economy.h"
#include "procedures/rf_vacation.h"
#include "procedures/rf_rice.h"
#include "procedures/rf_sabbath.h"
#include "procedures/rf_silent.h"
#include "procedures/rf_eco_extra.h"
#include "procedures/rf_errors.h"
#include "procedures/rf_shopping_mode.h"

#include "app/libs/temp_funcs.h"
#include "dev/synchronizer.h"
#include "dev/display_spi.h"
#include "board/board.h"
//#include "mw/backup.h"
#include "app/mw/h_backup.h" // MJI
#include "app/mw/rf_backup.h"
#include "app/cabin/cabin.h"
#include "../mw/connectivity/remote_control.h"

#define DEBUG_FILE_NAME "RF"

#define _WM_Service_StayTime_MS (30UL * 60 * 1000)          // 30 min
#define _WM_Distributer_StayTime_MS (24UL * 60 * 60 * 1000) // 24 h

#define _SensorTest_Time (10) // 10 dk
#define _ErrorWait_Time (15)  // 10 dk
#define _NoHotStartWait_Time (6)

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup REFRIGERATOR
 *  @{
 */
//
   
// DD: PING TEST
unsigned char pingTestS = 0;
unsigned char pingtestminute = 18;

static PROCESS_HANDLER_PROTO(rfProcIdleStateHandler);
static PROCESS_HANDLER_PROTO(rfProcNormalStateHandler);
static PROCESS_HANDLER_PROTO(rfProcAppOffStateHandler);
static PROCESS_HANDLER_PROTO(rfProcProdTestStateHandler);
static PROCESS_HANDLER_PROTO(rfProcServiceStateHandler);
static PROCESS_HANDLER_PROTO(rfProcDistributerStateHandler);
static PROCESS_HANDLER_PROTO(rfProcFTCStateHandler);
static PROCESS_HANDLER_PROTO(rfProcDefaultEventHandler);
static PROCESS_HANDLER_PROTO(rfProcCommercialTestStateHandler);                        //APW : Add CT heater software
static PROCESS_HANDLER_PROTO(rfProcCommercialDefrostTestStateHandler);                 //APW : Add CT heater software

static void rfInit(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
static void rfInitAfterService(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
static void rfInitAfterHotStart(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
static void rfE2Init(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
static void rfClearDisplaySetting20MsTask(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);

static void rfE2ToRam(stRefrigerator_Ram *rfP);
static void rfE2AfterService(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
static void rfE2AfterHotStart(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
static void rfE2ClearDisplaySetting(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);

static PROCESS_HANDLER_PROTO(rfProcSuper1SecTask);
static PROCESS_HANDLER_PROTO(rfProcSuper1MinTask);
static void rfRegularTask(const tsRfProcConsts *consts, tsRfProcParams *params);
static void rfNormalState20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params);
static void rfProdTestState20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params);
static void rfCommercialTestState20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params);        //APW : Add CT heater software
static void rfCommercialDefrostTestState20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params); //APW : Add CT heater software
static void rfARMValue20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params);                   //APW : Add CT heater software

static bool isComponentTestDone(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
static bool isHotStart(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
static void rfPublishEvent(tEventEnum ev);

/// @brief  Initialization function of refrigerator
PROCESS_INIT_PROTO(refrigeratorProcInit)
{
    tsRfProcParams *params = process->parameters;
    const tsRfProcConsts *consts = process->constants;

    devSyncAttach(consts->sync, eSyncTypeRamToDevice, &acOutputsController, &params->rf->Values.compsEMCImprovement);
    rfE2Init(consts->rf, params->rf);

    processStart(sysProcesses[eProcessUi]);
    if (consts->rf->Cooler.type)
    {
        processStart(sysProcesses[eProcessCooler]);
        if(consts->rf->Cooler.components.compressor.type == _CompressorType_UART)
          processStart(sysProcesses[eProcessInverterCom]);
    }
    
#ifdef ENERGYMONITORING
    processStart(sysProcesses[eProcessEnergyMonitoring]);
#endif

    if (consts->rf->IceMachine.type)
    {
      processStart(sysProcesses[eProcessIceMachine]);
    }
     if (consts->rf->IceSource.type)
    {
      processStart(sysProcesses[eProcessIceSource]);
    }
    if (consts->rf->WaterSource.type.supply)
    {
      processStart(sysProcesses[eProcessWaterSource]);
    }  
    if (consts->rf->Dispenser.waterType || consts->rf->Dispenser.iceType)
    {
      processStart(sysProcesses[eProcessDispenser]);
    }
    for (uint8_t i = 0; i < _NumOfCabin; i++)
    {
        if (consts->rf->Cabin[i].type)
            processStart(sysProcesses[eProcessCabin0 + i]);
    }

    timerEventStart(&params->te.startupDelay, 500);
    PROCESS_STATE_CHANGE(process, rfProcIdleStateHandler);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of refrigerator
PROCESS_DEINIT_PROTO(refrigeratorProcDeinit)
{
    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/************************* RF State Handlers *************************/
/// @brief  Event handler function of cabin
static PROCESS_HANDLER_PROTO(rfProcIdleStateHandler)
{
    tsRfProcParams *params = process->parameters;
    const tsRfProcConsts *consts = process->constants;
    stRefrigerator_Ram *rfP = params->rf;
    stRefrigerator_Flash const *rfC = consts->rf;

    switch (eventCurrent.event)
    {
    case eSysEventInitAfterStartup:
        rfE2ToRam(rfP);
        rfInit(rfC, rfP);
        rfPublishEvent(eSysEventInitAfterStartup);

        if (isHotStart(rfC, rfP))
        {
            params->rf->Cooler.components.compressor.CompressorHotstartF = true;
            rfInitAfterHotStart(rfC, rfP);
            rfPublishEvent(eSysEventInitAfterHotStart);
        }

        teSystemEvents newMode;
        if (rfP->workMode.values.workMode == _WM_Distributer)
            newMode = eSysEventStartDistributerMode;
        else if (rfP->workMode.values.workMode == _WM_ApplianceOff)
            newMode = eSysEventStartApplianceOffMode;
        else
            newMode = eSysEventStartProductionTestMode;
        eventPost(eProcessRefrigerator, newMode, NULL, 0);

        timerEventStart(&params->te.periodic20Ms, 20);
        timerEventStart(&params->te.periodic1Sec, 1000);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        timerEventStart(&params->te.periodic1Hour, 60 * 60 * 1000);
        break;

    default:
        rfProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(rfProcNormalStateHandler)
{
    tsRfProcParams *params = process->parameters;
    const tsRfProcConsts *consts = process->constants;
    stRefrigerator_Ram *rfP = params->rf;                                       //APW : Update I2C driver for add CT state

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        params->rf->Values.compsEMCImprovement = true;
        params->rf->workMode.values.workMode = _WM_Normal;
        timerEventStart(&params->te.periodic100Ms, 100);                        //APW : Update I2C driver for add CT state

    case eRfEventPeriodic20Ms:
        rfNormalState20MsTask(consts, params);
        refreshTemperaturePointer(consts->rf, params->rf);
        rfARMValue20MsTask(consts, params);                                     //APW : Update I2C driver for add CT state
        rfARMModeErrorOutput20MsTask(rfP);      //Error output  
        boardDevForce20MsTask();                //APW : Add Dev control
        if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          rfClearDisplaySetting20MsTask(consts->rf, rfP);
        
        
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    /*** APW : Update I2C driver for add CT state ***/    
    case eRfEventPeriodic100Ms:
        if(rfP->workMode.values.ARMModeError.ARMTimer100Ms < 0xFFFF)                         //APW : Update I2C driver for add CT state
        rfP->workMode.values.ARMModeError.ARMTimer100Ms++;                                   //APW : Update I2C driver for add CT state
        
        timerEventStart(&params->te.periodic100Ms, 100);
        break;
    /*** APW : Update I2C driver for add CT state ***/   
    case eRfEventPeriodic1Sec:
#if defined(AI_COOL_V1_5)
        RefrigeratorAIT1sec(rfP);
        //RefrigeratorAIT1hour();//TEMP
#endif
        rfProcSuper1SecTask(process);	//Add solve the bug
        timerEventStart(&params->te.periodic1Sec, 1000);
      break;

    case eRfEventPeriodic1Hour:
#if defined(AI_COOL_V1_5)
		RefrigeratorAIT1hour(rfP);
#endif
		timerEventStart(&params->te.periodic1Hour, 60 * 60 * 1000);
      break;

    default:
        rfProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(rfProcAppOffStateHandler)
{
    tsRfProcParams *params = process->parameters;
    stRefrigerator_Ram *rfP = params->rf;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        rfP->workMode.values.workMode = _WM_ApplianceOff;
        rfP->workMode.E2.workMode = _WM_ApplianceOff;
        rfP->Values.compsEMCImprovement = true;
        sysBuzzerCmd(eSysBuzzerPlayEndMelody);
        break;

    default:
        rfProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(rfProcProdTestStateHandler)
{
    tsRfProcParams *params = process->parameters;
    const tsRfProcConsts *consts = process->constants;
    stRefrigerator_Ram *rfP = params->rf;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        rfP->Values.compsEMCImprovement = false;
        rfP->workMode.values.workMode = _WM_ProductionTest;
        rfP->workMode.E2.workMode = _WM_ProductionTest;
        rfP->workMode.values.prodTest.componentTestTimer100Ms = 0;
        rfP->workMode.values.prodTest.state = _WM_ProductionTest_State_CompTest;
        rfP->workMode.values.prodTest.minTimer = 0;
        timerEventStart(&params->te.periodic100Ms, 100);
        timerEventStart(&params->te.periodic1Min, 60 * 1000); // restart it

        if (consts->rf->Cooler.components.humiditySensor.type == _HumSensorType_SHT3X)
            boardInitDevice(eDeviceTypeSHT3XHumidity, 0, NULL);

        break;

    case eRfEventPeriodic100Ms:
        if (rfP->workMode.values.prodTest.componentTestTimer100Ms < 0xffff)
            rfP->workMode.values.prodTest.componentTestTimer100Ms++;

        if(rfP->workMode.values.ARMModeError.ARMTimer100Ms < 0xFFFF)                         //APW : Update I2C driver for add CT state
        rfP->workMode.values.ARMModeError.ARMTimer100Ms++;                                   //APW : Update I2C driver for add CT state
        
        timerEventStart(&params->te.periodic100Ms, 100);
        break;

    case eRfEventPeriodic20Ms:
        rfProdTestState20MsTask(consts, params);
        refreshTemperaturePointer(consts->rf, params->rf);
        rfARMValue20MsTask(consts, params);                                     //APW : Update I2C driver for add CT state
        rfARMModeErrorOutput20MsTask(rfP);      //Error output   
        boardDevForce20MsTask();                //APW : Add Dev control
        if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          rfClearDisplaySetting20MsTask(consts->rf, rfP);

        
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    case eRfEventPeriodic1Min:
       /* if (rfP->workMode.values.prodTest.minTimer < 0xffff)
            rfP->workMode.values.prodTest.minTimer++;*/
        rfProcSuper1MinTask(process);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;

    default:
        rfProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(rfProcServiceStateHandler)
{
    const tsRfProcConsts *consts = process->constants;
    tsRfProcParams *params = process->parameters;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        params->rf->Values.compsEMCImprovement = false;
        params->rf->workMode.values.workMode = _WM_Service;
        //rfInitAfterService(consts->rf, params->rf);
        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
        timerEventStart(&params->te.exitMode, _WM_Service_StayTime_MS);
        break;

    case eRfEventExitMode:
        // exiting service mode
        rfPublishEvent(eSysEventCalibrateAllStepperMotors);
        rfInitAfterService(consts->rf, params->rf);
        eventPost(eProcessRefrigerator, eSysEventStartProductionTestMode, NULL, 0);

        rfDoorsAlertInit(consts->rf, params->rf);
        break;
    default:
        rfProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(rfProcDistributerStateHandler)
{
    tsRfProcParams *params = process->parameters;
    const tsRfProcConsts *consts = process->constants;
    stRefrigerator_Ram *rfP = params->rf;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        rfP->Values.compsEMCImprovement = true;
        rfP->workMode.values.workMode = _WM_Distributer;
#ifdef HOSHI_H30
        rfP->workMode.E2.workMode = _WM_ProductionTest;
#else
        rfP->workMode.E2.workMode = _WM_Distributer;
#endif

        if (rfP->workMode.values.fairModeFlag)
            timerEventStart(&params->te.exitMode, _WM_Distributer_StayTime_MS);
        // CHECKPOINT
        //backupUpdateTrigger();
        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
        break;

    case eRfEventExitMode:
        eventPost(eProcessRefrigerator, eSysEventStartProductionTestMode, NULL, 0);
        break;

    case eRfEventPeriodic20Ms:
        rfRegularTask(consts, params);
        refreshTemperaturePointer(consts->rf, params->rf);

        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    default:
        rfProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(rfProcFTCStateHandler)
{
    rfProcDefaultEventHandler(process);
}

/*** APW : Add CT heater software ***/
static PROCESS_HANDLER_PROTO(rfProcCommercialTestStateHandler)
{
    tsRfProcParams *params = process->parameters;
    const tsRfProcConsts *consts = process->constants;
    stRefrigerator_Ram *rfP = params->rf;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        //sysBuzzerCmd(eSysBuzzerPlayEndMelody);        //APW Disable buzzer when start CT
        
        rfP->Values.compsEMCImprovement = false;
        rfP->workMode.values.CommercialTest.componentTestTimer100Ms = 0;
        rfP->workMode.values.CommercialTest.SecOnFanCommercialTimer = 0;
        rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CompTest;
        rfP->workMode.values.CommercialTest.minTimer = 0;
        rfP->workMode.values.CommercialTest.PrvPingTestInCT = false;
        rfInitARMModeError(rfP);          //APW : Initial ARM mode
        
        timerEventStart(&params->te.periodic100Ms, 100);
        timerEventStart(&params->te.periodic1Sec, 1000);
        timerEventStart(&params->te.periodic1Min, 60 * 1000); // restart it but ProtestTimer will error 1 min

        if (consts->rf->Cooler.components.humiditySensor.type == _HumSensorType_SHT3X)
            boardInitDevice(eDeviceTypeSHT3XHumidity, 0, NULL);
        
        
        // Check componant test done or not in ProdTest(After poer on)
        if(rfP->Cabin[0].values.componentsTestDone &&                           //Componant test done in ProdTest
           rfP->Cabin[1].values.componentsTestDone && 
           rfP->Cooler.parameters.componentsTestDone)
        {}
        else                                                                    //Componant test not done in ProdTest(Restart test in Commercial test) 
        {
            rfP->Cabin[0].values.componentsTestDone = true;
            rfP->Cabin[1].values.componentsTestDone = true;
            rfP->Cooler.parameters.componentsTestDone = true;
        }  
        
        break;
        
    case eRfEventPeriodic20Ms:
        rfCommercialTestState20MsTask(consts, params);
        refreshTemperaturePointer(consts->rf, params->rf);
        rfRegularTask(consts, params);                                          //APW : Update command CtmodeState and display in CT and others BUG about display
        rfARMValue20MsTask(consts, params);                                     //APW : Update I2C driver for add CT state
        rfARMModeErrorOutput20MsTask(rfP);      //Error output
        boardDevForce20MsTask();                //APW : Add Dev control
        if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          rfClearDisplaySetting20MsTask(consts->rf, rfP);
        
        timerEventStart(&params->te.periodic20Ms, 20);                          // Reset timer
        break;

    case eRfEventPeriodic100Ms:
        if (rfP->workMode.values.CommercialTest.componentTestTimer100Ms < 0xffff)
            rfP->workMode.values.CommercialTest.componentTestTimer100Ms++;
        
        if(rfP->workMode.values.ARMModeError.ARMTimer100Ms < 0xFFFF)                         //APW : Update I2C driver for add CT state
        rfP->workMode.values.ARMModeError.ARMTimer100Ms++;                                   //APW : Update I2C driver for add CT state
        
        timerEventStart(&params->te.periodic100Ms, 100);
        break;

    case eRfEventPeriodic1Min:
        if (rfP->workMode.values.CommercialTest.minTimer < 0xffff)
            rfP->workMode.values.CommercialTest.minTimer++;
        rfProcSuper1MinTask(process);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;
        
    default:
        rfProcDefaultEventHandler(process);
        break;
    }
}
static PROCESS_HANDLER_PROTO(rfProcCommercialDefrostTestStateHandler)
{
    tsRfProcParams *params = process->parameters;
    const tsRfProcConsts *consts = process->constants;
    stRefrigerator_Ram *rfP = params->rf;
    
    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        rfP->Values.compsEMCImprovement = false;
        rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
        rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_DefrostTestStanby;
        rfInitARMModeError(rfP);          //APW : Initial ARM mode
        timerEventStart(&params->te.periodic100Ms, 100);
        
        rfP->Cabin[0].values.componentsTestDone = true;
        rfP->Cabin[1].values.componentsTestDone = true;
        rfP->Cooler.parameters.componentsTestDone = true;
        
        break;
    case eRfEventPeriodic20Ms:
        rfCommercialDefrostTestState20MsTask(consts, params);
        rfARMValue20MsTask(consts, params);                                     //APW : Update I2C driver for add CT state
        rfARMModeErrorOutput20MsTask(rfP);      //Error output
        boardDevForce20MsTask();                //APW : Add Dev control
        if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          rfClearDisplaySetting20MsTask(consts->rf, rfP);
        
        timerEventStart(&params->te.periodic20Ms, 20);                          // Reset timer
        break;
        
    case eRfEventPeriodic100Ms:
        if (rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms < 0xffff)
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms++;
        
        if(rfP->workMode.values.ARMModeError.ARMTimer100Ms < 0xFFFF)                         //APW : Update I2C driver for add CT state
        rfP->workMode.values.ARMModeError.ARMTimer100Ms++;                                   //APW : Update I2C driver for add CT state
        
        /*if(rfP->workMode.values.CommercialTest.state == _WM_CommercialTest_State_DefrostTestStanby)
        {
          if(rfP->workMode.values.CompDelayInHeaterTestTimer100Ms < 0xFF)
            rfP->workMode.values.CompDelayInHeaterTestTimer100Ms++;
        }
        else
          rfP->workMode.values.CompDelayInHeaterTestTimer100Ms = 0;*/

        timerEventStart(&params->te.periodic100Ms, 100);
        break;
        
    default:
        rfProcDefaultEventHandler(process);
        break;
    }
}
/*** APW : Add CT heater software ***/

static PROCESS_HANDLER_PROTO(rfProcDefaultEventHandler)
{
    const tsRfProcConsts *consts = process->constants;
    tsRfProcParams *params = process->parameters;
    stRefrigerator_Ram *rfP = params->rf;

    // const tsRfProcConsts *consts = process->constants;

    tfProcessHandler newHandler = NULL;

    switch (eventCurrent.event)
    {
    case eRfEventPeriodic20Ms:
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    case eRfEventPeriodic1Sec:
        rfProcSuper1SecTask(process);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;

    case eRfEventPeriodic1Min:
        rfProcSuper1MinTask(process);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;
        
    case eRfEventPeriodic1Hour:
      timerEventStart(&params->te.periodic1Hour, 60 * 60 * 1000);
      break;

    case eSysEventReinit:
        rfInit(consts->rf, params->rf);
        rfPublishEvent(eventCurrent.event);
        break;

    case eSysEventCalibrateAllStepperMotors:
        rfPublishEvent(eventCurrent.event);
        break;

    case eSysEventStartNormalMode:
        newHandler = rfProcNormalStateHandler;
        break;

    case eSysEventStartDistributerMode:
        rfP->workMode.values.fairModeFlag = 0;
        rfP->workMode.E2.fairModeFlag = 0;
        newHandler = rfProcDistributerStateHandler;
        break;

    case eSysEventStartFairMode:
        rfP->workMode.values.fairModeFlag = 1;
        rfP->workMode.E2.fairModeFlag = 1;
        newHandler = rfProcDistributerStateHandler;
        break;

    case eSysEventStartApplianceOffMode:
        newHandler = rfProcAppOffStateHandler;
        break;

    case eSysEventStartServiceMode:
        newHandler = rfProcServiceStateHandler;
        break;

    case eSysEventStartFTCMode:
        newHandler = rfProcFTCStateHandler;
        break;
    case eSysEventStartProductionTestMode:
        newHandler = rfProcProdTestStateHandler;
        break;
    case eSysEventStartCommercialTestMode:                                      //APW : Add CT heater software 
        newHandler = rfProcCommercialTestStateHandler;                          //APW : Add CT heater software 
        break;
    case eSysEventStartCommercialDefrostTestMode:                               //APW : Add CT heater software 
        newHandler = rfProcCommercialDefrostTestStateHandler;                   //APW : Add CT heater software 
        break;
        
    }

    if (newHandler)
    {
        timerEventStop(&params->te.exitMode);
        timerEventStop(&params->te.periodic100Ms);
        sysProcChangeState(process, newHandler);
        rfPublishEvent(eventCurrent.event);
    }
}

/************************* RF Init Functions *************************/

static void rfInit(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
    rfP->Stock = rfC->Stock;
    rfP->Values.I2CDevices = rfC->Parameters.I2CDevices;
    rfEcoExtraInit(rfC, rfP);
    rfShoppingModeInit(rfC, rfP);
    rfDoorsClosedInit(rfC, rfP);
    rfDoorsAlertInit(rfC, rfP);
    rfCheckHighTempError(rfP);
    rfInitARMModeError(rfP);          //APW : Initial ARM mode
#if defined(AI_COOL_V1_5)
    RefrigeratorAIInit(rfP);
#endif
    
}

static void rfInitAfterService(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{

    rfE2AfterService(rfC, rfP);
    rfE2ToRam(rfP);
#if(CONNECTIVITY != NON_CONNECTED)
    // TODO connectivity
    Connectivity_ResetErrors(); //E23 hatasi servis testinden sonra unutulur.
     connectionTimer = 0;
#endif
}

static void rfInitAfterHotStart(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
    rfE2AfterHotStart(rfC, rfP);
    rfE2ToRam(rfP);
}

static void rfClearDisplaySetting20MsTask(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
    rfE2ClearDisplaySetting(rfC, rfP);
}

static void rfE2Init(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
    /*these 3 byte is for controlling wheater first write operation is done or not
    if first write is not done, don't read eeprom at powerOn
    do not change these bytes!*/
    rfP->workMode.E2.preambleBytes[0] = 0x53;
    rfP->workMode.E2.preambleBytes[1] = 0x4C;
    rfP->workMode.E2.preambleBytes[2] = 0x48;
    rfP->workMode.E2.preambleBytes[3] = 0x2A;

    rfP->workMode.E2.workMode = _WM_ProductionTest;

    rfP->E2.Sabbath = 0;
    rfP->E2.EcoExtra = 0;
    rfP->E2.keyLock = 0;
    rfP->E2.keyLockDspr = 0;
    rfP->E2.dispOff = 0;
    rfP->E2.tempType = _TempType_Celcius;
    rfP->E2.sabbathTimer = 0;
    rfP->E2.dispDim = eDisplayDimLevel100;
    rfP->E2.doorsClosedBlocked = 0;
    rfP->E2.Silent = 0;
    rfP->E2.silentTimer = 0;
    rfP->E2.Violed = 0;
    rfP->E2.onVTimer = 0;
    rfP->E2.offVTimer = rfC->Cabin[0].parameters.VioLed.maxOffTime;
    rfP->E2.Convertible = 0;
    rfP->E2.CabinOff = 0;
    // DD: PING TEST
    rfP->E2.pingTestState = 0;
    rfP->E2.FTCState = 0;
    rfP->E2.ICTState = 0;
    rfP->E2.TempSettingFine = 0;
    rfP->E2.handleLightState = 0;
}

/************************* RF E2 Functions *************************/
static void rfE2ToRam(stRefrigerator_Ram *rfP)
{
    // WorkModeE2ToRam
    rfP->workMode.values.workMode = rfP->workMode.E2.workMode;

    rfP->Sets.user.functions.Sabbath = rfP->E2.Sabbath;
    rfP->Sets.user.functions.EcoExtra = rfP->E2.EcoExtra;
    rfP->Values.keyLock = rfP->E2.keyLock;
    rfP->Values.keyLockDspr = rfP->E2.keyLockDspr;
    rfP->Values.dispOff = rfP->E2.dispOff;
    rfP->Values.tempType = rfP->E2.tempType;
    rfP->Values.sabbathTimer = rfP->E2.sabbathTimer;
    rfP->Values.dispDim = rfP->E2.dispDim;
    rfP->Values.doorsClosedBlocked = rfP->E2.doorsClosedBlocked;
    rfP->Sets.user.functions.Silent = rfP->E2.Silent;
    rfP->Values.silentTimer = rfP->E2.silentTimer;
    rfP->Cabin[1].sets.user.functions.Convertible = rfP->E2.Convertible;
    rfP->Cabin[1].sets.user.functions.CabinOff = rfP->E2.CabinOff;

    rfP->Cabin[0].values.violed.state = rfP->E2.Violed ? _VioLedState_ON : _VioLedState_OFF;
    rfP->Cabin[0].values.violed.onVTimer = rfP->E2.onVTimer;
    rfP->Cabin[0].values.violed.offVTimer = rfP->E2.offVTimer;
    FTC.Tests.FTC = rfP->E2.FTCState;
    FTC.Tests.ICT = rfP->E2.ICTState;
    rfP->Sets.user.functions.TempSettingFine = rfP->E2.TempSettingFine;
    
    if (rfP->Values.doorsClosedBlocked)
        rfP->Values.doorsClosedBlocked = 0;

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        rfP->Values.temperatureConvPointer[i] = rfP->E2.temperatureConvPointer[i];
    }
    // DD: PING TEST
    //  wifiError_B1 = refrigerator_R.E2.wifiErrors;
    //  wifiError_B2 = refrigerator_R.E2.wifiErrors;
    pingTestS = rfP->E2.pingTestState;
    rfP->Values.WifiStatus = rfP->E2.WifiStatus;
    rfP->Values.handleLightState = rfP->E2.handleLightState;
}

static void rfE2AfterService(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
    // WorkModeE2AfterService
    rfP->workMode.E2.workMode = _WM_ProductionTest;

    rfP->E2.EcoExtra = false;
    rfP->E2.keyLock = false;
    rfP->E2.keyLockDspr = false;
    rfP->E2.dispOff = false;
    rfP->E2.dispDim = eDisplayDimLevel100;
    if (rfP->E2.tempType == _TempType_Celcius)
    {
        rfP->E2.tempType = _TempType_Celcius;
    }
    else
    {
        rfP->E2.tempType = _TempType_Fahrenheit;
    }

    rfP->E2.doorsClosedBlocked = 0;
    rfP->E2.Silent = 0;
    rfP->E2.silentTimer = 0;

    rfP->E2.Violed = 0;
    rfP->E2.onVTimer = 0;
    rfP->E2.offVTimer = rfC->Cabin[0].parameters.VioLed.maxOffTime;
    rfP->E2.Convertible = rfP->Cabin[1].sets.user.functions.Convertible;
    rfP->E2.CabinOff = rfP->Cabin[1].sets.user.functions.CabinOff;

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        rfP->E2.temperatureConvPointer[i] = resetTemperaturePointerF(rfC, rfP, i);
    }

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        rfP->Cabin[i].sets.user.functions.Quick = false;
    }
    rfP->Values.handleLightState = 0;
    rfP->E2.TempSettingFine = false;
}

static void rfE2AfterHotStart(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
    //APW: Disable for reset other process
    /*rfP->E2.EcoExtra = false;
    rfP->E2.keyLock = false;
    rfP->E2.dispOff = false;
    rfP->E2.dispDim = eDisplayDimLevel100;
    rfP->E2.Violed = 0;
    rfP->E2.offVTimer = rfC->Cabin[0].parameters.VioLed.maxOffTime;
    rfP->E2.onVTimer = rfC->Cabin[0].parameters.VioLed.maxOnTime;
    rfP->E2.Convertible = rfP->Cabin[1].sets.user.functions.Convertible;
    rfP->E2.CabinOff = rfP->Cabin[1].sets.user.functions.CabinOff;
    if (rfP->E2.tempType == _TempType_Celcius)
    {
        rfP->E2.tempType = _TempType_Celcius;
    }
    else
    {
        rfP->E2.tempType = _TempType_Fahrenheit;
    }
    rfP->E2.doorsClosedBlocked = 0;
    rfP->E2.Silent = 0;
    rfP->E2.silentTimer = 0;
    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        rfP->E2.temperatureConvPointer[i] = resetTemperaturePointerF(rfC, rfP, i);
    }

    rfP->E2.Violed = 0;
    rfP->E2.onVTimer = 0;
    rfP->E2.offVTimer = rfC->Cabin[0].parameters.VioLed.maxOffTimeFirstStart;
    rfP->E2.TempSettingFine = false;*/
}

static void rfE2ClearDisplaySetting(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{

    rfP->E2.EcoExtra = false;
    rfP->E2.keyLock = false;
    rfP->E2.keyLockDspr = false;
    rfP->E2.dispOff = false;
    rfP->E2.dispDim = eDisplayDimLevel100;
    rfP->E2.Violed = 0;
    rfP->E2.offVTimer = rfC->Cabin[0].parameters.VioLed.maxOffTime;
    rfP->E2.onVTimer = rfC->Cabin[0].parameters.VioLed.maxOnTime;
    rfP->E2.Convertible = rfP->Cabin[1].sets.user.functions.Convertible;
    rfP->E2.CabinOff = rfP->Cabin[1].sets.user.functions.CabinOff;
    if (rfP->E2.tempType == _TempType_Celcius)
    {
        rfP->E2.tempType = _TempType_Celcius;
    }
    else
    {
        rfP->E2.tempType = _TempType_Fahrenheit;
    }
    rfP->E2.doorsClosedBlocked = 0;
    rfP->E2.Silent = 0;
    rfP->E2.silentTimer = 0;
    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        rfP->E2.temperatureConvPointer[i] = resetTemperaturePointerF(rfC, rfP, i);
    }

    rfP->E2.Violed = 0;
    rfP->E2.onVTimer = 0;
    rfP->E2.offVTimer = rfC->Cabin[0].parameters.VioLed.maxOffTimeFirstStart;
    rfP->E2.TempSettingFine = false;
}

/************************* RF Tasks *************************/
static PROCESS_HANDLER_PROTO(rfProcSuper1SecTask)
{
    tsRfProcParams *params = process->parameters;
    const tsRfProcConsts *consts = process->constants;
    stRefrigerator_Ram *rfP = params->rf;

    rfDoorsAlert1SecTask(consts->rf, params->rf);
    
    
        /*** APW : Add CT heater software ***/
        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
        {
          if(params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer < 0xffff)
            params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer++;
        }
        else{
            params->rf->workMode.values.CommercialTest.SecOnFanCommercialTimer = 0;
        }
        
        if(params->rf->workMode.values.ARMMode)
        {
            if (rfP->workMode.values.ARMModeError.ARMStayTimer1s < 0xFFFF)
                rfP->workMode.values.ARMModeError.ARMStayTimer1s++;
        }
        /*** APW : Add CT heater software ***/
    
    
}

static PROCESS_HANDLER_PROTO(rfProcSuper1MinTask)
{
    const tsRfProcConsts *consts = process->constants;
    tsRfProcParams *params = process->parameters;
    const stRefrigerator_Flash *rfC = consts->rf;
    stRefrigerator_Ram *rfP = params->rf;

    if (params->rf->workMode.values.prodTest.minTimer < 0xffff)
           params->rf->workMode.values.prodTest.minTimer++;
    
    rfDoorsAlert1MinTask(rfC, rfP);
    rfDoorsClosed1MinTask(rfC, rfP);
    rfEcoExtra1MinTask(rfC, rfP);
    rfSabbath1MinTask(rfC, rfP);
    rfSilent1MinTask(rfC, rfP);
    rfShoppingMode1MinTask(rfC, rfP);
    
    // DD: PING TEST
    if( pingtestminute > 0 )
      pingtestminute--;
    
    
        /*** APW : Add CT heater software ***/
        if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest ||
            params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest ||
              params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
        {
          if(params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer < 0xffff)
            params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer++;
        }
        else
        {
            params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer = 0;
        }
        
        if(params->rf->workMode.values.ARMMode)
        {
            if (rfP->workMode.values.ARMModeError.ARMStayTimer1Min < 0xFFFF)
                rfP->workMode.values.ARMModeError.ARMStayTimer1Min++;
        }
        /*** APW : Add CT heater software ***/
        
}

static void rfRegularTask(const tsRfProcConsts *consts, tsRfProcParams *params)
{
    stRefrigerator_Flash const *rfC = consts->rf;
    stRefrigerator_Ram *rfP = params->rf;

    rfEconomyTask(rfC, rfP);
    rfVacationTask(rfC, rfP);
    rfRiceTask(rfC, rfP);
    rfSabbathTask(rfC, rfP);
    rfSilentTask(rfC, rfP);
    rfEcoExtraTask(rfC, rfP);
    rfDoorsClosedTask(rfC, rfP);
    rfDoorsAlertTask(rfC, rfP);
    
#if (CONNECTIVITY != NON_CONNECTED)   
    // DD: PING TEST
    /***************** PING TEST SHORT PERFORMANCE TEST AREA *********************/
    
    if ( pingtestminute >= 10 && WiFiVariables.Situation == 0x04 && endofbandtest) /* ilk 10 dk icinde baglanti saglandiysa (SUCCESS) */
    { 
      wifiError_B1 = 0;
      wifiError_B2 = 0;
      refrigerator_R.E2.wifiErrors = wifiError_B2;
      pingTestS = 1 ;
      refrigerator_R.E2.pingTestState = pingTestS ;
      //    Wifi.TestMode = 0;
      //      WA[Situation]= 0;
      //      setWifiState(WIFI_OFF);
      endofbandtest = 0;
      sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    }
    if ( pingtestminute < 10 ) 
    {
      if( WiFiVariables.Situation != 0x04 && endofbandtest ) /* 8 dk icinde baglanti yapilamadiysa (FAIL) */
      {  
        pingTestS = 0 ;
        refrigerator_R.E2.pingTestState = pingTestS ;
        //Compressor OFF 
        refrigerator_R.Cooler.components.compressor.value = 0;
        if(!rfP->workMode.values.CommercialTest.PrvPingTestInCT)
        {
          rfP->workMode.values.CommercialTest.PrvPingTestInCT = true;
          refrigerator_R.workMode.values.ARMModeError.PingTestError = true;
        }
      }
      if (pingtestminute == 0 && endofbandtest) 
        endofbandtest = 0;
      
      if( WiFiVariables.TestMode)
      {
        setWifiState(WIFI_OFF);
        WiFiVariables.Situation = 0;  
        WiFiVariables.TestMode = 0;
      } 
    }
#endif
}

static void rfNormalState20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params)
{
    rfRegularTask(consts, params);
    rfCheckHighTempError(params->rf);
}

static void rfProdTestState20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params)
{
    const stRefrigerator_Flash *rfC = consts->rf;
    stRefrigerator_Ram *rfP = params->rf;
    rfP->Values.compsEMCImprovement = true;
#if (CONNECTIVITY != NON_CONNECTED)
    // DD: PING TEST
    if ( !pingTestS && !endofbandtest && refrigerator_F.Connected.funcConnectedApp.connectedAppAct == true &&
          rfP->workMode.values.CommercialTest.PrvPingTestInCT == false)
    {
      //    CMDFromMB( _MBCommands_Buzzer_mStart );
      WiFiVariables.Situation = 0x01;
      WiFiVariables.TestMode = 0xAA;
      setWifiState(WIFI_ON);
      toggleWifiState(WIFI_CONFIG_TOGGLE);
      endofbandtest = 1 ;
    }
#endif
    rfErrorsCheckAll(rfC, rfP);
    switch (rfP->workMode.values.prodTest.state)
    {
    case _WM_ProductionTest_State_CompTest:
        rfP->Values.compsEMCImprovement = false;
        if (isComponentTestDone(rfC, rfP))
        {
            eventPost(eProcessIceMachine, eSysEventReinit, NULL, 0);
            if (isHotStart(rfC, rfP))
            {
                rfP->workMode.values.prodTest.tempSensorOrderChecked = false;
                rfP->workMode.values.prodTest.state = _WM_ProductionTest_State_PerformanceAndSensorTests;
                for (teSystemProcesses p = eProcessCabin0; p <= eProcessCabin3; p++)
                    eventPost(p, eCabinEventStartPerformanceAndSensorsTest, NULL, 0); //work_hotstart
                for (teSystemProcesses p = eProcessCabin0; p <= eProcessCabin3; p++) 
                    eventPost(p, eCabinEventIsProductionTestCondition, NULL, 0);  //if <39C work_performance
            }
            else
            {
                rfP->workMode.values.prodTest.minTimer = 0;
                rfP->workMode.values.prodTest.state = _WM_ProductionTest_State_EndedByNoHotStart;
            }
        }
       
       if (rfP->Values.errors.errorsNumber)
         params->rf->Cooler.components.compressor.ProductionTestDelayF = true;

        break;

    case _WM_ProductionTest_State_PerformanceAndSensorTests:
        if (!rfP->workMode.values.prodTest.tempSensorOrderChecked &&
            rfP->workMode.values.prodTest.minTimer >= _SensorTest_Time)
        {
            rfP->workMode.values.prodTest.tempSensorOrderChecked = true;
            rfP->workMode.values.prodTest.state = _WM_ProductionTest_State_EndedBySensorNOK;
            rfP->workMode.values.prodTest.minTimer = 0;
        }

        if (rfP->Values.errors.errorsNumber)
          params->rf->Cooler.components.compressor.ProductionTestDelayF = true;
        else
            rfP->workMode.values.prodTest.state = _WM_ProductionTest_State_OK;

        break;

    case _WM_ProductionTest_State_EndedByError:
    case _WM_ProductionTest_State_EndedBySensorNOK:
    case _WM_ProductionTest_State_EndedByPerformanceNOK: // AAAA
    case _WM_ProductionTest_State_OK:
    default:
        //rfP->Cooler.components.compressor.value = 100; //RCOS2 if hot start exists run the compressor
        eventPost(eProcessRefrigerator, eSysEventStartNormalMode, NULL, 0);
        break;

    case _WM_ProductionTest_State_EndedByNoHotStart:
        if (rfP->workMode.values.prodTest.minTimer >= _NoHotStartWait_Time)
        {
             eventPost(eProcessRefrigerator, eSysEventStartNormalMode, NULL, 0);
             for (unsigned char i = 0; i < _NumOfCabin; i++)
                  rfP->Cabin[i].states.Work= _Work_Idle;
        }
        break;
    }

    if (rfP->workMode.values.prodTest.state != _WM_ProductionTest_State_CompTest)
        rfNormalState20MsTask(consts, params);
}

/*** APW : Add CT heater software ***/
static void rfCommercialTestState20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params)
{
    const stRefrigerator_Flash *rfC = consts->rf;
    stRefrigerator_Ram *rfP = params->rf;
    rfP->Values.compsEMCImprovement = true;
    rfErrorsCheckAll(rfC, rfP);
 
//Add for test Ping test in CT mode    
#if (CONNECTIVITY != NON_CONNECTED)
    // DD: PING TEST
    if ( !pingTestS && !endofbandtest && refrigerator_F.Connected.funcConnectedApp.connectedAppAct == true &&
          rfP->workMode.values.CommercialTest.PrvPingTestInCT == false)
    {
      //    CMDFromMB( _MBCommands_Buzzer_mStart );
      WiFiVariables.Situation = 0x01;
      WiFiVariables.TestMode = 0xAA;
      setWifiState(WIFI_ON);
      toggleWifiState(WIFI_CONFIG_TOGGLE);
      endofbandtest = 1 ;
    }
#endif
    
    switch (rfP->workMode.values.CommercialTest.state)
    {
    case _WM_CommercialTest_State_CompTest:
        rfP->Values.compsEMCImprovement = false;
        eventPost(eProcessIceMachine, eSysEventReinit, NULL, 0);
        rfP->workMode.values.CommercialTest.tempSensorOrderChecked = false;
        rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_FirstTest;    
          for (teSystemProcesses p = eProcessCabin0; p <= eProcessCabin3; p++)
            eventPost(p, eCabinEventStartCommercialTest, NULL, 0); //work_Commercial first test
        break;

    case _WM_CommercialTest_State_FirstTest:
        break;  // APW ; add break for protect goto next case by not need.
        
    case _WM_CommercialTest_State_ValveTest:
        break;  // APW ; add break for protect goto next case by not need.

    case _WM_CommercialTest_JudgmentResult:
        break;  // APW ; add break for protect goto next case by not need.

    case _WM_CommercialTest_State_CoolingTest:
        break;  // APW ; add break for protect goto next case by not need.

    case _WM_CommercialTest_State_OK:
        //When Finish
        for (unsigned char i = 0; i < _NumOfCabin; i++)
          rfP->Cabin[i].states.Work = _Work_Cool;
        
        eventPost(eProcessRefrigerator, eSysEventStartNormalMode, NULL, 0);
        break;
        
    //default:;
        //break;
    }
}
static void rfCommercialDefrostTestState20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params)
{
    const stRefrigerator_Flash *rfC = consts->rf;
    stRefrigerator_Ram *rfP = params->rf;
    //rfErrorsCheckAll(rfC, rfP);
    switch (rfP->workMode.values.CommercialTest.state)
    {
    case _WM_CommercialTest_State_DefrostTestStanby:
          /*if(!params->rf->Cooler.components.compressor.Readvalue || 
                rfP->workMode.values.CompDelayInHeaterTestTimer100Ms >= 30)*/
          if(!params->rf->Cooler.components.compressor.Readvalue)
          {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            //rfP->workMode.values.CompDelayInHeaterTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_DefrostOnTest;
          }
          else
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_DefrostTestStanby;
          
        break;
    case _WM_CommercialTest_State_DefrostOnTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialDefrostOnTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_DefrostOffTest; 
        }

      break;
    case _WM_CommercialTest_State_DefrostOffTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialDefrostOffTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater1OnTest; 
        }

      break;
    case _WM_CommercialTest_State_Heater1OnTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOnTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater1OffTest; 
        }

      break;
    case _WM_CommercialTest_State_Heater1OffTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOffTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater2OnTest; 
        }

      break;
    case _WM_CommercialTest_State_Heater2OnTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOnTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater2OffTest; 
        }

      break;
    case _WM_CommercialTest_State_Heater2OffTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOffTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater3OnTest; 
        }

      break;
    case _WM_CommercialTest_State_Heater3OnTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOnTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater3OffTest; 
        }

      break;
    case _WM_CommercialTest_State_Heater3OffTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOffTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater4OnTest; 
        }
        
      break;
    case _WM_CommercialTest_State_Heater4OnTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOnTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater4OffTest; 
        }
        
      break;
    case _WM_CommercialTest_State_Heater4OffTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOffTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater5OnTest; 
        }
        
      break;
    case _WM_CommercialTest_State_Heater5OnTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOnTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_Heater5OffTest; 
        }
      break;
    case _WM_CommercialTest_State_Heater5OffTest:
        if(rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms >= rfC->Parameters.commercialHeaterOffTimerSet)
        {
            rfP->workMode.values.CommercialTest.DefrostTestTimer100Ms = 0;
            rfP->workMode.values.CommercialTest.state = _WM_CommercialTest_State_DefrostTestComplete; 
        }
        
      break;
    case _WM_CommercialTest_State_DefrostTestComplete:
      break;
    //default:;
      //break;
    }
}
static void rfARMValue20MsTask(const tsRfProcConsts *consts, tsRfProcParams *params)
{
    stRefrigerator_Ram *rfP = params->rf;
    unsigned char Set_LightUp;
    unsigned char Set_BlinkCount;
    unsigned char Set_BlinkOnOFF;
    
    /******************************Priority ARM check******************************/
    if(rfP->Cooler.components.testSwitch[0].value == _SwitchValue_CLOSE &&
       rfP->Cooler.components.testSwitch[1].value == _SwitchValue_CLOSE)
    {
      if(rfP->workMode.values.workMode != _WM_CommercialDefrostTest)
      {
#ifdef HOSHI_H30
        //Priority1 : Freezer sensor abnormal
        if(rfP->workMode.values.ARMModeError.FRZSensorAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_FRZSensorAbnormal;
        //Priority2 : Fresh food sensor abnormal
        else if(rfP->workMode.values.ARMModeError.FFSensorAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_FFSensorAbnormal;
        //Priority3 : Ambient sensor abnormal
        else if(rfP->workMode.values.ARMModeError.AmbientSensorAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_AmbientSensorAbnormal;
        //Priority4 : Freezer evap sensor abnormal
        else if(rfP->workMode.values.ARMModeError.FRZEVASensorAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_FRZEVASensorAbnormal;
        //Priority5 : EEPROM communication error
        else if(rfP->workMode.values.ARMModeError.EEPCommunicationError)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_EEPCommunicationError;
        //Priority6 : Compressor abnormal stop (Over current or etc.)
        else if(rfP->workMode.values.ARMModeError.CompressorError)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_CompressorError;
        //Priority7 : Compressor communication error
        else if(rfP->workMode.values.ARMModeError.CompressorCommunicationError)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_CompressorCommunicationError;
        //Priority8 : Freezer fan abnormal
        else if(rfP->workMode.values.ARMModeError.FRZFanAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_FRZFanAbnormal;
        //Priority9 : Condensor fan abnormal
        else if(rfP->workMode.values.ARMModeError.CondensorFanAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_CondensorFanAbnormal;
        //Priority10 : Fresh food fan abnormal
        else if(rfP->workMode.values.ARMModeError.FFFanAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_FFFanAbnormal;
        //Priority11 : Ice making unit signal abnormal
        else if(rfP->workMode.values.ARMModeError.IMUnitAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_IMUnitAbnormal;
        //Priority12 : Ice making sensor abnormal
        else if(rfP->workMode.values.ARMModeError.IMSensorAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_IMSensorAbnormal;
        //Priority13 : Defrost doesn't finish on the time
        else if(rfP->workMode.values.ARMModeError.DefrostCircuitAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_DefrostCircuitAbnormal;
        //Priority14 : Power supply voltage abnormal (Voltage exceeds Max setting or below Low setting)
        else if(rfP->workMode.values.ARMModeError.PowerSupplyVoltageAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_PowerSupplyVoltageAbnormal;
        //Priority15 : Freezer stays at higher than -10.2 for 72Hr
        else if(rfP->workMode.values.ARMModeError.FreezerNotCool)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_FreezerNotCool;
        //Priority16 : Fresh food stays at higher than 10.2 for 72Hr
        else if(rfP->workMode.values.ARMModeError.FreshfoodNotCool)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_FreshfoodNotCool;
        //Priority17 : Huminity sensor abnormal
        else if(rfP->workMode.values.ARMModeError.HuminitySensorAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_HuminitySensorAbnormal;
        //Priority18 : Ice inspection abnormal
        else if(rfP->workMode.values.ARMModeError.IceInspectionAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_IceInspectionAbnormal;
        //Priority19 : Freezer, Fresh food, Freezer evap high temperture abnormal
        else if(rfP->workMode.values.ARMModeError.HighTemperatureAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_HighTemperatureAbnormal;
        //Priority20 : Control panel communication error
        else if(rfP->workMode.values.ARMModeError.ControlPanelCommunicationError)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_ControlPanelCommunicationError;
        //Priority21 : FS sensor abnormal
        else if(rfP->workMode.values.ARMModeError.FSSensorAbnormal)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_FSSensorAbnormal;
        //Priority22 : Ping Test Error (For ALPS)
        else if(rfP->workMode.values.ARMModeError.PingTestError)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_PingTestError;
#else
        //Priority1 : Ping Test Error (For BONA)
        if(rfP->workMode.values.ARMModeError.PingTestError)
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_On_PingTestError;
        //Priority2 : Valve ARM in commercial mode Error
        else if(rfP->workMode.values.ARMModeError.ValveARM)
          rfP->workMode.values.ARMMode = _WM_CommercialTest_ARMMode_On_ValveErrorStep;
#endif //HOSHI_H30
        //None Error
        else  
          rfP->workMode.values.ARMMode = _WM_General_ARMMode_Off;
      }
      else
        rfP->workMode.values.ARMMode = _WM_General_ARMMode_Off;
    }
    
    /********************************Setup Operation********************************/
    switch (rfP->workMode.values.ARMMode)
    {
      case _WM_General_ARMMode_Off:
              rfP->Cooler.components.CommercialARM.value = _SwitchValue_CLOSE;
              rfP->workMode.values.ARMModeError.ARMTimer100Ms = 0xFFFF;
              Set_LightUp = 0;
              Set_BlinkCount = 0;
              Set_BlinkOnOFF = 0;
          break;
          
      case _WM_General_ARMMode_On_Always:
              rfP->Cooler.components.CommercialARM.value = _SwitchValue_OPEN;
              rfP->workMode.values.ARMModeError.ARMTimer100Ms = 0xFFFF;
              Set_LightUp = 0;
              Set_BlinkCount = 0;
              Set_BlinkOnOFF = 0;
          break;
          
      case _WM_CommercialTest_ARMMode_On_ClearDataStep:
              if(rfP->workMode.values.ARMModeError.ARMTimer100Ms >= 1)
              {
                rfP->Cooler.components.CommercialARM.value ^= 1;
                rfP->workMode.values.ARMModeError.ARMTimer100Ms = 0;               
              }
              Set_LightUp = 0;
              Set_BlinkCount = 0;
              Set_BlinkOnOFF = 0;
          break;
#ifdef HOSHI_H30
      // Case priority1
      case _WM_General_ARMMode_On_FRZSensorAbnormal:
              Set_LightUp = 10;         //Light up 1s
              Set_BlinkCount = 1;       //Blink count 1 time
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority2
      case _WM_General_ARMMode_On_FFSensorAbnormal:
              Set_LightUp = 10;         //Light up 1s
              Set_BlinkCount = 2;       //Blink count 2 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority3
      case _WM_General_ARMMode_On_AmbientSensorAbnormal:
              Set_LightUp = 10;         //Light up 1s
              Set_BlinkCount = 3;       //Blink count 3 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority4
      case _WM_General_ARMMode_On_FRZEVASensorAbnormal:
              Set_LightUp = 10;         //Light up 1s
              Set_BlinkCount = 4;       //Blink count 4 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority5
      case _WM_General_ARMMode_On_EEPCommunicationError:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 2;       //Blink count 2 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority6
      case _WM_General_ARMMode_On_CompressorError:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 7;       //Blink count 7 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority7
      case _WM_General_ARMMode_On_CompressorCommunicationError:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 11;      //Blink count 11 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority8
      case _WM_General_ARMMode_On_FRZFanAbnormal:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 12;      //Blink count 12 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority9
      case _WM_General_ARMMode_On_CondensorFanAbnormal:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 13;      //Blink count 13 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority10
      case _WM_General_ARMMode_On_FFFanAbnormal:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 17;      //Blink count 17 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority11
      case _WM_General_ARMMode_On_IMUnitAbnormal:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 3;       //Blink count 3 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority12
      case _WM_General_ARMMode_On_IMSensorAbnormal:
              Set_LightUp = 10;         //Light up 1s
              Set_BlinkCount = 5;       //Blink count 5 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority13
      case _WM_General_ARMMode_On_DefrostCircuitAbnormal:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 4;       //Blink count 4 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority14
      case _WM_General_ARMMode_On_PowerSupplyVoltageAbnormal:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 14;      //Blink count 14 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority15
      case _WM_General_ARMMode_On_FreezerNotCool:
              Set_LightUp = 30;         //Light up 3s
              Set_BlinkCount = 1;       //Blink count 1 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority16
      case _WM_General_ARMMode_On_FreshfoodNotCool:
              Set_LightUp = 30;         //Light up 3s
              Set_BlinkCount = 2;       //Blink count 2 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority17
      case _WM_General_ARMMode_On_HuminitySensorAbnormal:
              Set_LightUp = 10;         //Light up 1s
              Set_BlinkCount = 7;       //Blink count 7 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority18
      case _WM_General_ARMMode_On_IceInspectionAbnormal:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 16;      //Blink count 16 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority19
      case _WM_General_ARMMode_On_HighTemperatureAbnormal:
              Set_LightUp = 30;         //Light up 3s
              Set_BlinkCount = 3;       //Blink count 3 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority20
      case _WM_General_ARMMode_On_ControlPanelCommunicationError:
              Set_LightUp = 0;          //Light up 0s or no have light up
              Set_BlinkCount = 20;      //Blink count 20 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority21
      case _WM_General_ARMMode_On_FSSensorAbnormal:
              Set_LightUp = 10;         //Light up 1s
              Set_BlinkCount = 17;      //Blink count 17 times
              Set_BlinkOnOFF = 3;       //Blink on/off time are 0.3s(Need 0.25s)
      break;
      
      // Case priority22
      case _WM_General_ARMMode_On_PingTestError:
              Set_LightUp = 50;         //Light up 5s
              Set_BlinkCount = 1;       //Blink count 1 times
              Set_BlinkOnOFF = 5;       //Blink on/off time are 0.5s
          break;
#else
      // Case priority1
      case _WM_General_ARMMode_On_PingTestError:
              Set_LightUp = 50;         //Light up 5s
              Set_BlinkCount = 1;       //Blink count 1 times
              Set_BlinkOnOFF = 5;       //Blink on/off time are 0.5s
          break;
      
      // Case priority2    
      case _WM_CommercialTest_ARMMode_On_ValveErrorStep:
              Set_LightUp = 50;         //Light up 5s
              Set_BlinkCount = 2;       //Blink count 2 times
              Set_BlinkOnOFF = 5;       //Blink on/off time are 0.5s
      break;
#endif //HOSHI_H30
    default:
      break;
    }  
    
    /****************************** Operation ARM ******************************/
    if(rfP->workMode.values.ARMMode == _WM_General_ARMMode_Off ||
        rfP->workMode.values.ARMMode == _WM_General_ARMMode_On_Always ||
          rfP->workMode.values.ARMMode == _WM_CommercialTest_ARMMode_On_ClearDataStep)
    {
      rfP->workMode.values.ARMModeError.ARMStayTimer1s = 0xFFFF;
      rfP->workMode.values.ARMModeError.ARMStayTimer1Min = 0xFFFF;
      rfP->workMode.values.ARMModeError.ARMLightUpMode = true;
      rfP->workMode.values.ARMModeError.ARMBlinkTimes = 0;
    }
    else
    {
      //Waiting ARM again
      if(rfP->workMode.values.ARMMode == _WM_General_ARMMode_On_PingTestError || 
          rfP->workMode.values.ARMMode == _WM_CommercialTest_ARMMode_On_ValveErrorStep)
      {
        if(rfP->workMode.values.ARMModeError.ARMStayTimer1s >= 10 &&
           rfP->workMode.values.ARMModeError.ARMStayTimer1s != 0xFFFF)
            rfP->workMode.values.ARMModeError.ARMStayTimer1s = 0xFFFF;
      }
      else
      {
        if(rfP->workMode.values.ARMModeError.ARMStayTimer1s >= 2 &&
           rfP->workMode.values.ARMModeError.ARMStayTimer1s != 0xFFFF)
            rfP->workMode.values.ARMModeError.ARMStayTimer1s = 0xFFFF;
      }
      
      //Patern operation
      if(rfP->workMode.values.ARMModeError.ARMStayTimer1s == 0xFFFF)
      {
        if(rfP->workMode.values.ARMModeError.ARMLightUpMode == true)
        {
          if(rfP->workMode.values.ARMModeError.ARMTimer100Ms == 0xFFFF)
            rfP->workMode.values.ARMModeError.ARMTimer100Ms = 0;
          else if(rfP->workMode.values.ARMModeError.ARMTimer100Ms >= Set_LightUp)      //Light up
          {
            rfP->Cooler.components.CommercialARM.value = _SwitchValue_CLOSE;
            rfP->workMode.values.ARMModeError.ARMLightUpMode = false;
            rfP->workMode.values.ARMModeError.ARMTimer100Ms = 0xFFFF;
          }   
          else
            rfP->Cooler.components.CommercialARM.value = _SwitchValue_OPEN; 
        }
        else
        {
          if(rfP->workMode.values.ARMModeError.ARMTimer100Ms == 0xFFFF)
            rfP->workMode.values.ARMModeError.ARMTimer100Ms = 0;
          else if(rfP->workMode.values.ARMModeError.ARMTimer100Ms >= Set_BlinkOnOFF &&     //Blink on/off time
                    rfP->workMode.values.ARMModeError.ARMBlinkTimes < Set_BlinkCount)      //Blink count times
          {
            rfP->Cooler.components.CommercialARM.value ^= _SwitchValue_OPEN;
            rfP->workMode.values.ARMModeError.ARMTimer100Ms = 0;
            
            if(rfP->Cooler.components.CommercialARM.value == _SwitchValue_OPEN)
              rfP->workMode.values.ARMModeError.ARMBlinkTimes++;
          }
          else if(rfP->workMode.values.ARMModeError.ARMTimer100Ms >= Set_BlinkOnOFF &&     //Blink on/off time
                    rfP->workMode.values.ARMModeError.ARMBlinkTimes >= Set_BlinkCount)     //Blink count times
          {
            rfP->workMode.values.ARMModeError.ARMTimer100Ms = 0xFFFF;
            rfP->workMode.values.ARMModeError.ARMStayTimer1s = 0;
            rfP->workMode.values.ARMModeError.ARMBlinkTimes = 0;
            rfP->workMode.values.ARMModeError.ARMLightUpMode = true;
            rfP->Cooler.components.CommercialARM.value = _SwitchValue_CLOSE;
          }
          else
            rfP->Cooler.components.CommercialARM.value = rfP->Cooler.components.CommercialARM.value;
        }
      }
    }
}
/*** APW : Add CT heater software ***/

/************************* RF sub functions *************************/

/// @brief Checks if all sub-processes component tests are done
static bool isComponentTestDone(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
    bool testDone = true;
    for (uint8_t i = 0; i < _NumOfCabin; i++)
    {
        if (rfC->Cabin[i].type)
            testDone &= rfP->Cabin[i].values.componentsTestDone;
    }
    if (rfC->Cooler.type)
        testDone &= rfP->Cooler.parameters.componentsTestDone;
    
    if (rfC->IceMachine.type)
        testDone &= rfP->IceMachine.values.componentsTestDone;
    
    if (rfC->WaterSource.type.supply)
        testDone &= rfP->WaterSource.values.componentsTestDone;

    return testDone;
}

/// @brief Checks if hot start condition is met by checking the ADC values of cabins' NTCs
static bool isHotStart(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
    for (uint8_t i = 0; i < _NumOfCabin; i++)
    {
        if (rfC->Cabin[i].type != _CabinType_NONE)
        {
            if (rfC->Cabin[i].components.evaTempSensor.type)
            {
                int32_t evaSensorinC = devIoGet(ntcs[rfC->Cabin[i].components.evaTempSensor.index]);
                if (!rfP->Cabin[i].components.evaTempSensor.error && evaSensorinC >= 100)
                    return true;
            }
            else if (rfC->Cabin[i].components.airTempSensor.type)
            {
                int32_t airSensorinC = devIoGet(ntcs[rfC->Cabin[i].components.airTempSensor.index]);
                if (!rfP->Cabin[i].components.airTempSensor.error && airSensorinC >= 100)
                    return true;
            }
        }
    }
    return false;
}

/// @brief Publishes the given event among all refrigerator sub-processes
static void rfPublishEvent(tEventEnum ev)
{
    for (teSystemProcesses p = eProcessCabin0; p <= eProcessCabin3; p++)
        eventPost(p, ev, NULL, 0);
    eventPost(eProcessCooler, ev, NULL, 0);
    eventPost(eProcessUi, ev, NULL, 0);
    eventPost(eProcessIceMachine, ev, NULL, 0);
    eventPost(eProcessIceSource, ev, NULL, 0);
    eventPost(eProcessWaterSource, ev, NULL, 0);
    eventPost(eProcessDispenser, ev, NULL, 0);
}

/** @} */

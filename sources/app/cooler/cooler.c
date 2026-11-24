/** @file       cooler.c
 *  @brief      COOLER application program file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-10
 */
#define FILE_COOLER_C


#include "dev/synchronizer.h"
#include "cooler.h"
#include "system.h"
#include "./types/cooler_types.h"
#include "./components/cooler_components.h"
#include "app/libs/component_test.h"

#define DEBUG_FILE_NAME "CL"

unsigned char pumpOut = 0;
// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup COOLER
 *  @{
 */

#define COMPRESSOR_PRODUCTION_TEST_DELAY (100 * 60) // service test error delay 10 dk

static PROCESS_HANDLER_PROTO(coolerProcIdleStateHandler);
static PROCESS_HANDLER_PROTO(coolerProcNormalStateHandler);
static PROCESS_HANDLER_PROTO(coolerProcAppOffStateHandler);
static PROCESS_HANDLER_PROTO(coolerProcProdTestStateHandler);
static PROCESS_HANDLER_PROTO(coolerProcServiceStateHandler);
static PROCESS_HANDLER_PROTO(coolerProcDistributerStateHandler);
static PROCESS_HANDLER_PROTO(coolerProcFTCStateHandler);
static PROCESS_HANDLER_PROTO(coolerProcDefaultEventHandler);
static PROCESS_HANDLER_PROTO(coolerProcSuper1MinTask);
static PROCESS_HANDLER_PROTO(coolerProcCommercialTestStateHandler);                     //APW : Add CT heater software
static PROCESS_HANDLER_PROTO(coolerProcCommercialDefrostTestStateHandler);              //APW : Add CT heater software

static void coolerInitDevices(const tsCoolerProcConsts *consts, tsCoolerProcParams *params);
static void coolerInit(const tsCoolerProcConsts *consts, tsCoolerProcParams *params);
static void coolerNormalState20MsTask(const tsCoolerProcConsts *consts, tsCoolerProcParams *params);

void coolerCloseAllComponents(stCooler_Ram *coolerP);

/// @brief  Initialization function of cooler
PROCESS_INIT_PROTO(coolerProcInit)
{
    tsCoolerProcParams *params = process->parameters;
    const tsCoolerProcConsts *consts = process->constants;

    coolerInitDevices(consts, params);
    PROCESS_STATE_CHANGE(process, coolerProcIdleStateHandler);

    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of cooler
PROCESS_DEINIT_PROTO(coolerProcDeinit)
{
    tsCoolerProcParams *params = process->parameters;
    const tsCoolerProcConsts *consts = process->constants;

    UNUSED(consts); // REMOVE IF USED

    timerEventStop(&params->te.periodic20Ms);
    timerEventStop(&params->te.periodic1Min);

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

static void coolerInitDevices(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
    const stCoolerComponents_Flash *compC = &consts->cooler->components;
    stCoolerComponents_Ram *compP = &params->cooler->components;
    tsCoolerProcDevIos *pDev = &params->dev;
    const tsDevIo *dev = NULL;

    dev = boardInitDevice((teBoardDeviceType)compC->compressor.type, compC->compressor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->compressor.value);
    pDev->compressor = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->condenserFan.type, compC->condenserFan.index, &compC->condenserFan.configs);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->condenserFan.speedOrPWM);
    pDev->condenserFan = dev;

    dev = boardInitDevice((teBoardDeviceType)eDeviceTypeFanErrorDetector, compC->condenserFan.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->condenserFan.error);
    pDev->condenserFanError = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->ambientTempSensor.type, compC->ambientTempSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->ambientTempSensor.valueInCExt);
    pDev->ambientTempSensor = dev;

    dev = NULL;
    if (compC->ambientTempSensor.type == _TempSensorType_NTCwithADC || compC->ambientTempSensor.type == _TempSensorType_NTC_Ambient_ADC)
      dev = boardInitDevice(eDeviceTypeNTCErrorDetector, compC->ambientTempSensor.index, NULL);
    else if (compC->ambientTempSensor.type == _TempSensorType_SHT3X)
      dev = boardInitDevice(eDeviceTypeSHT3XErrorDetector, compC->ambientTempSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->ambientTempSensor.error);
    pDev->ambientTempSensorError = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->humiditySensor.type, compC->humiditySensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->humiditySensor.humidityValue);
    pDev->humiditySensor = dev;

    dev = NULL;
    if (compC->humiditySensor.type == _HumSensorType_ADC)
      dev = boardInitDevice(eDeviceTypeNTCErrorDetector, compC->humiditySensor.index, NULL);
    else if (compC->humiditySensor.type == _HumSensorType_SHT3X)
      dev = boardInitDevice(eDeviceTypeSHT3XErrorDetector, compC->humiditySensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->humiditySensor.error);
    pDev->ambientTempSensorError = dev;
    
    
  /*  dev = boardInitDevice((teBoardDeviceType)compC->humiditySensor.type, compC->humiditySensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->humiditySensor.humidityValue);
    pDev->humiditySensor = dev;

    dev = NULL;
    if (compC->humiditySensor.type == _HumSensorType_SHT3X)
        dev = boardInitDevice(eDeviceTypeSHT3XErrorDetector, compC->humiditySensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->humiditySensor.error);
    pDev->humiditySensorError = dev;*/

    for (size_t i = 0; i < 2; i++)
    {
        dev = boardInitDevice((teBoardDeviceType)compC->solenoidValve[i].type, compC->solenoidValve[i].index, &compC->solenoidValve[i].stepperConfigs);
        devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->solenoidValve[i].position);
        devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->solenoidValve[i].positionIndex);
        pDev->solenoidValve[i] = dev;
    }
    
    /*** APW : Add CT software ***/
    dev = boardInitDevice((teBoardDeviceType)compC->testSwitch[0].type, compC->testSwitch[0].index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->testSwitch[0].value);
    pDev->testSwitch[0] = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->testSwitch[1].type, compC->testSwitch[1].index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->testSwitch[1].value);
    pDev->testSwitch[1] = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->CommercialARM.type, compC->CommercialARM.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->CommercialARM.value);
    pDev->CommercialARM = dev;
    /*** APW : Add CT software ***/
    
}

static void coolerNormalState20MsTask(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
    stCooler_Ram *coolerP = params->cooler;

    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        if (consts->rf->Cabin[i].type != _CabinType_NONE)
        {
            coolerP->parameters.gasDesired[i].type = params->rf->Cabin[i].values.gasDesired.type;
            coolerP->parameters.gasDesired[i].level = params->rf->Cabin[i].values.gasDesired.level;

            params->rf->Cabin[i].values.gasApplied.type = coolerP->parameters.gasApplied[i].type;
            params->rf->Cabin[i].values.gasApplied.level = coolerP->parameters.gasApplied[i].level;
        }
        else
        {
            coolerP->parameters.gasDesired[i].type = _GT_None;
            coolerP->parameters.gasDesired[i].level = 0;
        }
    }

    switch (consts->cooler->type)
    {
    case _CoolerType_DEFAULT:
    case _CoolerType_HOTGAS:
        coolerTypeDefault2(consts, params);
        break;

    case _CoolerType_SEMIPARALLEL:
        coolerTypeSemiParallel(consts, params);
        break;

    case _CoolerType_PARALLEL:
        coolerTypeParallel(consts, params);
        break;

    case _CoolerType_NONE:
    default:
        break;
    }
}

static void coolerProdTestState20MsTask(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
    tsCoolerProcDevIos *devs = &params->dev;
    const stCooler_Flash *coolerC = consts->cooler;
    stCooler_Ram *coolerP = params->cooler;
    const stCoolerComponents_Flash *compC = &coolerC->components;
    stCoolerComponents_Ram *compP = &coolerP->components;
    // component test timer x 100 ms
    uint32_t tt = params->rf->workMode.values.prodTest.componentTestTimer100Ms;

    coolerP->components.solenoidValve[0].testMode = 1;
    coolerP->components.solenoidValve[1].testMode = 1;
    coolerCloseAllComponents(coolerP);
    if (coolerC->type)
    {
        if (params->rf->workMode.values.prodTest.state == _WM_ProductionTest_State_CompTest)
        {
            bool done = true;
            tsCompTestData compressorTestData = compC->compressor.test;
            if (compP->compressor.ProductionTestDelayF)
            {
                compressorTestData.startTime += COMPRESSOR_PRODUCTION_TEST_DELAY;
                compressorTestData.endTime += COMPRESSOR_PRODUCTION_TEST_DELAY;
            }
            done &= compTest(devs->compressor, &compP->compressor.value, &compressorTestData, tt, 100);
            done &= compTest(devs->condenserFan, &compP->condenserFan.speedOrPWM, &compC->condenserFan.test, tt, 100);
            done &= compTest(devs->solenoidValve[0], &compP->solenoidValve[0].position, &compC->solenoidValve[0].test, tt, 1);
            done &= compTest(devs->solenoidValve[1], &compP->solenoidValve[1].position, &compC->solenoidValve[1].test, tt, 1);
            coolerP->parameters.componentsTestDone = done;

            // TODO SIM_MODE
            if (params->rf->Values.errors.errorsNumber ||
                (coolerC->components.ambientTempSensor.type != _TempSensorType_NONE && coolerP->components.ambientTempSensor.error))
            {
                coolerP->components.compressor.ProductionTestDelayF = true;
            }
        }
        else
        {
            if(params->rf->workMode.values.prodTest.state ==   _WM_ProductionTest_State_OK)
                coolerP->components.compressor.value = 100;
                coolerNormalState20MsTask(consts, params);
        }
    }
    
}

/*** APW : Add CT heater software ***/
static void coolerCommercialTestState20MsTask(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
  
    //stCooler_Ram *coolerP = params->cooler;
  
    switch (consts->cooler->type)
    {
    case _CoolerType_DEFAULT:
    case _CoolerType_HOTGAS:
        coolerTypeDefault2(consts, params);
        break;

    case _CoolerType_SEMIPARALLEL:
        //coolerTypeSemiParallel(consts, params);
        break;

    case _CoolerType_PARALLEL:
        coolerTypeParallel(consts, params);
        break;

    case _CoolerType_NONE:
    default:
        break;
    }

}

static void coolerCommercialDefrostTestState20MsTask(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
  
    stCooler_Ram *coolerP = params->cooler;
  
    switch (consts->cooler->type)
    {
    case _CoolerType_DEFAULT:
    case _CoolerType_HOTGAS:
        coolerCloseAllComponents(coolerP);   
        coolerP->components.solenoidValve[0].positionIndex = _ALL_OPEN;         //Change _ALL_CLOSE to _ALL_OPEN when comp off
        coolerTypeDefault2(consts, params);
        break;

    case _CoolerType_SEMIPARALLEL:
        coolerCloseAllComponents(coolerP);
        coolerP->components.solenoidValve[0].positionIndex = _ALL_OPEN;         //Change _ALL_CLOSE to _ALL_OPEN when comp off
        //coolerTypeSemiParallel(consts, params);
        break;

    case _CoolerType_PARALLEL:
        coolerCloseAllComponents(coolerP); 
        coolerP->components.solenoidValve[0].positionIndex = _ALL_OPEN;         //Change _ALL_CLOSE to _ALL_OPEN when comp off
        coolerTypeParallel(consts, params);
        break;

    case _CoolerType_NONE:
    default:
        break;
    }

}
/*** APW : Add CT heater software ***/

void coolerCloseAllComponents(stCooler_Ram *coolerP)
{
    coolerP->components.condenserFan.speedOrPWM = 0;
    coolerP->components.compressor.value = 0;
    coolerP->components.solenoidValve[0].position = 0;
    coolerP->components.solenoidValve[1].position = 0;
}

/// @brief  Event handler function of cooler
static PROCESS_HANDLER_PROTO(coolerProcIdleStateHandler)
{
    tsCoolerProcParams *params = process->parameters;
    const tsCoolerProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventInitAfterStartup:
        coolerInit(consts, params);
        coolerInitValveState(params->cooler);
        params->cooler->parameters.frzDefrost = false;
        params->cooler->parameters.componentsTestDone = false;

        timerEventStart(&params->te.periodic20Ms, 20);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;

    default:
        coolerProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(coolerProcNormalStateHandler)
{
    tsCoolerProcParams *params = process->parameters;
    const tsCoolerProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eCoolerEventPeriodic20Ms:
        coolerNormalState20MsTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    default:
        coolerProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(coolerProcAppOffStateHandler)
{
    tsCoolerProcParams *params = process->parameters;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        coolerCloseAllComponents(params->cooler);
        break;

    default:
        coolerProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(coolerProcProdTestStateHandler)
{
    tsCoolerProcParams *params = process->parameters;
    const tsCoolerProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eCoolerEventPeriodic20Ms:
        coolerProdTestState20MsTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    default:
        coolerProcDefaultEventHandler(process);
        break;
    }
}

/*** APW : Add CT heater software ***/
static PROCESS_HANDLER_PROTO(coolerProcCommercialTestStateHandler)
{
    tsCoolerProcParams *params = process->parameters;
    const tsCoolerProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        params->rf->workMode.values.CommercialTest.FLAP_Fan_Request = 0;
        params->rf->workMode.values.CommercialTest.FF_Fan_Request = 0;
        params->rf->workMode.values.CommercialTest.FRZ_Fan_Request = 0;
        params->rf->workMode.values.CommercialTest.FF_Valve_Request = 0;
        params->rf->workMode.values.CommercialTest.FRZ_Valve_Request = 0;
        params->rf->workMode.values.CommercialTest.LastSensorTempRecord = 0;
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;
    case eCoolerEventPeriodic20Ms:
        coolerCommercialTestState20MsTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
    default:
        coolerProcDefaultEventHandler(process);
        break;
    }
}
static PROCESS_HANDLER_PROTO(coolerProcCommercialDefrostTestStateHandler)
{
    tsCoolerProcParams *params = process->parameters;
    const tsCoolerProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        break;
    case eCoolerEventPeriodic20Ms:
        coolerCommercialDefrostTestState20MsTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
    default:
        coolerProcDefaultEventHandler(process);
        break;
    }
}
/*** APW : Add CT heater software ***/

static PROCESS_HANDLER_PROTO(coolerProcServiceStateHandler)
{
    coolerProcDefaultEventHandler(process);
}

static PROCESS_HANDLER_PROTO(coolerProcDistributerStateHandler)
{
    tsCoolerProcParams *params = process->parameters;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        coolerCloseAllComponents(params->cooler);
        break;

    default:
        coolerProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(coolerProcFTCStateHandler)
{
    coolerProcDefaultEventHandler(process);
}

static PROCESS_HANDLER_PROTO(coolerProcDefaultEventHandler)
{
    const tsCoolerProcConsts *consts = process->constants;
    tsCoolerProcParams *params = process->parameters;
    tfProcessHandler newHandler = NULL;

    switch (eventCurrent.event)
    {
    case eCoolerEventPeriodic20Ms:
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    case eCoolerEventPeriodic1Min:
        coolerProcSuper1MinTask(process);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;

    case eSysEventInitAfterHotStart:
    case eSysEventReinit:
        break;

    case eSysEventCalibrateAllStepperMotors:
        for (uint8_t i = 0; i <= 1; i++)
        {
            if (consts->cooler->components.solenoidValve[i].type == _SolenoidValveType_StepMotor)
                devIoInit(params->dev.solenoidValve[i], &consts->cooler->components.solenoidValve[i].stepperConfigs);
        }
        break;

    case eSysEventStartNormalMode:
        newHandler = coolerProcNormalStateHandler;
        break;

    case eSysEventStartDistributerMode:
    case eSysEventStartFairMode:
        newHandler = coolerProcDistributerStateHandler;
        break;

    case eSysEventStartApplianceOffMode:
        newHandler = coolerProcAppOffStateHandler;
        break;

    case eSysEventStartServiceMode:
        newHandler = coolerProcServiceStateHandler;
        break;

    case eSysEventStartFTCMode:
        newHandler = coolerProcFTCStateHandler;
        break;
    case eSysEventStartProductionTestMode:
        newHandler = coolerProcProdTestStateHandler;
        break;
    /*** APW : Add CT heater software ***/  
    case eSysEventStartCommercialTestMode:
        newHandler = coolerProcCommercialTestStateHandler;
        break;
    case eSysEventStartCommercialDefrostTestMode:
        newHandler = coolerProcCommercialDefrostTestStateHandler;
        break;
    /*** APW : Add CT heater software ***/
    }

    if (newHandler)
        sysProcChangeState(process, newHandler);
}

static PROCESS_HANDLER_PROTO(coolerProcSuper1MinTask)
{
    tsCoolerProcParams *params = process->parameters;
    stCooler_Ram *coolerP = params->cooler;

    if (coolerP->components.compressor.value)
    {
        if (coolerP->parameters.compressorOnTimer < 0xFFFF)
            coolerP->parameters.compressorOnTimer++;

        if (coolerP->parameters.ambientType == _Ambient_NONE)
        {
            if (coolerP->parameters.gasLevel == 9)
            {
                if (coolerP->parameters.maxLevelCompressorOnTimer < 0xFF)
                    coolerP->parameters.maxLevelCompressorOnTimer++;
            }
            else
                coolerP->parameters.maxLevelCompressorOnTimer = 0;
        }
        else
        {
            if (coolerP->components.ambientTempSensor.error)
            {
                if ((coolerP->parameters.gasLevel == _compressorHotStartLevel + 1) || (coolerP->parameters.gasLevel == 4))
                    if (coolerP->parameters.maxLevelCompressorOnTimer < 0xFF)
                        coolerP->parameters.maxLevelCompressorOnTimer++;
            }
            else
                coolerP->parameters.maxLevelCompressorOnTimer = 0;
        }
    }
    else
    {
#if defined(AI_COOL_V1) || defined(AI_COOL_V1_5)
        if (!coolerP->parameters.compressorOffTimer)
          coolerP->parameters.compressorOnTimerPrev = coolerP->parameters.compressorOnTimer;
#endif
        if (!coolerP->parameters.compressorOffTimer)
            coolerP->parameters.maxLevelCompressorOnTimerPrev = coolerP->parameters.maxLevelCompressorOnTimer;

        if (coolerP->parameters.compressorOffTimer < 0xFFFF)
            coolerP->parameters.compressorOffTimer++;

        coolerP->parameters.maxLevelCompressorOnTimer = 0;
    }

    if (coolerP->parameters.solenoidValvePositionTimer)
        coolerP->parameters.solenoidValvePositionTimer--;

    if (coolerP->parameters.valveStateStayTimer < 0xFFFF)
        coolerP->parameters.valveStateStayTimer++;

    if (coolerP->parameters.gasLevelStayTimer < 0xFFFF)
        coolerP->parameters.gasLevelStayTimer++;
    
    if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
    {
      if (coolerP->parameters.humidityCheckTimer < 0xFFFF)
        coolerP->parameters.humidityCheckTimer++;
    }
    else
    {
        coolerP->parameters.humidityCheckTimer = 0;
    }
    
    if (coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
    {
      if (coolerP->parameters.gasLevelStayTimerFF < 0xFFFF)
        coolerP->parameters.gasLevelStayTimerFF++;
    }
    else if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
    {
      if (coolerP->parameters.gasLevelStayTimerFRZ < 0xFFFF)
      coolerP->parameters.gasLevelStayTimerFRZ++;
    }
    else
    {
      if (coolerP->parameters.gasLevelStayTimerFF < 0xFFFF)
        coolerP->parameters.gasLevelStayTimerFF++;
      if (coolerP->parameters.gasLevelStayTimerFRZ < 0xFFFF)
        coolerP->parameters.gasLevelStayTimerFRZ++;
    }
    

    /* ozgur - there is a problem about quick ice logic.
    if( params->rf->Values.quickIceTermalCutOutTimer )
        params->rf->Values.quickIceTermalCutOutTimer--;
    */
    
    
    /*** APW : Add CT heater software ***/
    if (params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest)
    {
      if (coolerP->parameters.ValveAndCompressorCommercialOnTimer < 0xFFFF)
      coolerP->parameters.ValveAndCompressorCommercialOnTimer++;
    }
    else
    {
      coolerP->parameters.ValveAndCompressorCommercialOnTimer = 0;
    }
    /*** APW : Add CT heater software ***/
    
}

static void coolerInit(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
    const stCooler_Flash *coolerC = consts->cooler;
    stCooler_Ram *coolerP = params->cooler;

    if (coolerC->components.ambientTempSensor.type != _TempSensorType_NONE)
        coolerP->parameters.ambientType = _Ambient_0;
    else
        coolerP->parameters.ambientType = _Ambient_NONE;

    if (coolerC->components.humiditySensor.type != _HumSensorType_NONE)
        coolerP->parameters.humidityType = _Humidity_0;
    else
        coolerP->parameters.humidityType = _Humidity_NONE;
}

unsigned short getGasLevelStayTimer(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
  stCooler_Ram *coolerP = params->cooler;
 // const stCooler_Flash *coolerC = consts->cooler;
  
  unsigned short stayTimer = coolerP->parameters.gasLevelStayTimer;
  
  if (coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
    stayTimer = coolerP->parameters.gasLevelStayTimerFF;
  else if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
    stayTimer = coolerP->parameters.gasLevelStayTimerFRZ;
  
  return stayTimer;
}

unsigned char getGasLevelStayTime(const tsCoolerProcConsts *consts, tsCoolerProcParams *params, unsigned char type)
{
  stCooler_Ram *coolerP = params->cooler;
  const stCooler_Flash *coolerC = consts->cooler;
  
  unsigned char stayTime = 0;
  
  if(coolerP->parameters.ambientType != _Ambient_NONE)
  {
    if(type == MIN_STAY_TIME)
    {
      if (coolerP->parameters.gasLevel)
        stayTime = coolerC->parameters.minStayTimeAtPresGasLevel[coolerP->parameters.ambientType][coolerP->parameters.gasLevel-1];
      else
        stayTime = coolerC->parameters.minStayTimeAtPresGasLevel[coolerP->parameters.ambientType][0];
    }
    else
    {
      if(type == MAX_STAY_TIME)
      {
        if (coolerP->parameters.gasLevel)
          stayTime = coolerC->parameters.maxStayTimeAtPresGasLevel[coolerP->parameters.ambientType][coolerP->parameters.gasLevel-1];
        else
          stayTime = coolerC->parameters.maxStayTimeAtPresGasLevel[coolerP->parameters.ambientType][0];
      }
    }
  }
  else
  {
    if(type == MIN_STAY_TIME)
    {
      if (coolerP->parameters.gasLevel)
        stayTime = coolerC->parameters.minStayTimeAtPresGasLevel[0][coolerP->parameters.gasLevel-1];
      else
        stayTime = coolerC->parameters.minStayTimeAtPresGasLevel[0][0];
    }
    else
    {
      if(type == MAX_STAY_TIME)
      {
        if (coolerP->parameters.gasLevel)
          stayTime = coolerC->parameters.maxStayTimeAtPresGasLevel[0][coolerP->parameters.gasLevel-1];
        else
          stayTime = coolerC->parameters.maxStayTimeAtPresGasLevel[0][0];
      }
    }
  }
  
  return stayTime;
}

void resetGasLevelStayTimers(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
  stCooler_Ram *coolerP = params->cooler;
  const stCooler_Flash *coolerC = consts->cooler;
   
  if((coolerP->components.compressor.value == 0) || (coolerP->parameters.compressorOnTimer < coolerC->parameters.compressor.timeStartUp))
  {
    coolerP->parameters.gasLevelStayTimerFF = 0;
    coolerP->parameters.gasLevelStayTimerFRZ = 0;
  }
  else
  {
    if ((coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
        && coolerP->parameters.gasAppliedPrev[0].level != coolerP->parameters.gasLevel)
      coolerP->parameters.gasLevelStayTimerFF = 0;
    else if ((coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
      && coolerP->parameters.gasAppliedPrev[1].level != coolerP->parameters.gasLevel)
      coolerP->parameters.gasLevelStayTimerFRZ = 0;
    /*else
    {
      coolerP->parameters.gasLevelStayTimerFF = 0;
      coolerP->parameters.gasLevelStayTimerFRZ = 0;
    }*/
  }
}

/** @} */

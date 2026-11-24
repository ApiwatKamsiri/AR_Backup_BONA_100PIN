/** @file       cabin.c
 *  @brief      CABIN application program file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-10
 */
#define FILE_CABIN_C

#include "cabin.h"
#include "board/board.h"
#include "dev/synchronizer.h"
#include "cabin_ff/cabin_ff.h"
#include "cabin_frz/cabin_frz.h"
#include "cabin_flap/cabin_flap.h"
#include "components/cabin_comp_flap.h"

#include "cabin_jkr/cabin_jkr.h"

#include "components/cabin_blue_light.h"

#include "components/cabin_fan.h"
#include "components/cabin_a_heater.h"
#include "components/cabin_b_heater.h"
#include "components/cabin_c_heater.h"
#include "components/cabin_inner_light.h"
#include "components/cabin_handle_light.h"
#include "components/cabin_ionizer.h"
#include "components/cabin_vacuumbox.h"
#include "components/cabin_switch.h"
#include "components/cabin_violed.h"

#ifdef ALPS_H50
#include "components/cabin_door_assistunit.h"
#endif
#include "procedures/cabin_duty.h"
#include "procedures/cabin_defrost.h"
#include "procedures/cabin_errors.h"
#include "procedures/cabin_quick.h"
#include "procedures/cabin_shared_funcs.h"
#include "procedures/cabin_production_test.h"
#include "procedures/cabin_commercial_test.h"                                   //APW : Add CT heater software
#include "procedures/cabin_sets_proc.h"

#include "app/mw/EE_data.h" //STW
#include "app/libs/temp_funcs.h"

#define DEBUG_FILE_NAME "CB"

#define _CabinMeanCoolTime 30
#define _CabinMeanIdleTime 40

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup CABIN
 *  @{
 */

static PROCESS_HANDLER_PROTO(cabinProcIdleStateHandler);
static PROCESS_HANDLER_PROTO(cabinProcNormalStateHandler);
static PROCESS_HANDLER_PROTO(cabinProcAppOffStateHandler);
static PROCESS_HANDLER_PROTO(cabinProcProdTestStateHandler);
static PROCESS_HANDLER_PROTO(cabinProcServiceStateHandler);
static PROCESS_HANDLER_PROTO(cabinProcDistributerStateHandler);
static PROCESS_HANDLER_PROTO(cabinProcFTCStateHandler);
static PROCESS_HANDLER_PROTO(cabinProcDefaultEventHandler);
static PROCESS_HANDLER_PROTO(cabinProcSuper20MsTask);
static PROCESS_HANDLER_PROTO(cabinProcSuper1SecTask);
static PROCESS_HANDLER_PROTO(cabinProcSuper1MinTask);
static PROCESS_HANDLER_PROTO(cabinProcCommercialTestStateHandler);                                               //APW : Add CT heater software
static PROCESS_HANDLER_PROTO(cabinProcCommercialDefrostTestStateHandler);                                        //APW : Add CT heater software
static PROCESS_HANDLER_PROTO(cabinProcSuper1HourTask);

static void cabinInitDevices(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinInit(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinNormalState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinDistributerState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinProdTestState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinCommercialTestState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params);        //APW : Add CT heater software
static void cabinCommercialDefrostTestState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params); //APW : Add CT heater software
static void cabinCloseAllComponentsCommercialDefrost(const tsCabinProcConsts *consts, tsCabinProcParams *params);//APW : Add CT heater software 

static void cabinInitAfterHotStart(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinInitAfterService(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinE2Init(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinE2AfterHotStart(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinE2AfterService(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinE2ToRam(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinClearDisplaySetting20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params);
static void cabinE2ClearDisplaySetting(const tsCabinProcConsts *consts, tsCabinProcParams *params);

/// @brief  Initialization function of cabin
PROCESS_INIT_PROTO(cabinProcInit)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;

    cabinInitDevices(consts, params);
    cabinE2Init(consts, params);
    PROCESS_STATE_CHANGE(process, cabinProcIdleStateHandler);

    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of cabin
PROCESS_DEINIT_PROTO(cabinProcDeinit)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    timerEventStop(&params->te.periodic20Ms);
    timerEventStop(&params->te.periodic1Sec);
    timerEventStop(&params->te.periodic1Min);
    timerEventStop(&params->te.periodic1Hour);

    PROCESS_STATE_CHANGE(process, NULL);

    process->initialized = 0; // If process needs other checks, clear this another time
}

static void cabinInitDevices(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabinComponents_Flash *compC = &consts->cabin->components;
    stCabinComponents_Ram *compP = &params->cabin->components;
    tsCabinProcDevIos *pDev = &params->dev;
    const tsDevIo *dev = NULL;

    dev = boardInitDevice((teBoardDeviceType)compC->fan.type, compC->fan.index, &compC->fan.configs);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->fan.speedOrPWM);
    pDev->fan = dev;

    dev = boardInitDevice(eDeviceTypeFanErrorDetector, compC->fan.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->fan.error);
    pDev->fanError = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->flap.type, compC->flap.index, &compC->flap.stepperConfigs);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->flap.value);
    pDev->flap = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->heaterA.type, compC->heaterA.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->heaterA.value);
    pDev->heaterA = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->heaterB.type, compC->heaterB.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->heaterB.value);
    pDev->heaterB = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->heaterC.type, compC->heaterC.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->heaterC.value);
    pDev->heaterC = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->ionizer.type, compC->ionizer.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->ionizer.value);
    pDev->ionizer = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->innerLight.type, compC->innerLight.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->innerLight.value);
    pDev->innerLight = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->blueLight.type, compC->blueLight.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->blueLight.value);
    pDev->blueLight = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->handleLight.type, compC->handleLight.index_r, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->handleLight.R_value);
    pDev->handleLight_R = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->handleLight.type, compC->handleLight.index_g, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->handleLight.G_value);
    pDev->handleLight_G = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->handleLight.type, compC->handleLight.index_b, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->handleLight.B_value);
    pDev->handleLight_B = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->doorSwitch[0].type, compC->doorSwitch[0].index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->doorSwitch[0].value);
    pDev->doorSwitch[0] = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->doorSwitch[1].type, compC->doorSwitch[1].index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->doorSwitch[1].value);
    pDev->doorSwitch[1] = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->airTempSensor.type, compC->airTempSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->airTempSensor.valueInCExt);
    pDev->airTempSensor = dev;

    dev = boardInitDevice(eDeviceTypeNTCErrorDetector, compC->airTempSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->airTempSensor.error);
    pDev->airTempSensorError = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->vacuumBoxOverCurrentSensor.type, compC->vacuumBoxOverCurrentSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->vacuumBoxOverCurrentSensor.valueInCExt);
    pDev->vacuumBoxOverCurrentSensor = dev;
    
    dev = boardInitDevice(eDeviceTypeNTCErrorDetector, compC->airTempSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->airTempSensor.error);
    pDev->airTempSensorError = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->evaTempSensor.type, compC->evaTempSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->evaTempSensor.valueInCExt);
    pDev->evaTempSensor = dev;

    dev = boardInitDevice(eDeviceTypeNTCErrorDetector, compC->evaTempSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->evaTempSensor.error);
    pDev->evaTempSensorError = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->knob.type, compC->knob.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->knob.position);
    pDev->knob = dev;

    dev = boardInitDevice(eDeviceTypeKnobError, compC->knob.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->knob.error);
    pDev->knobError = dev;

    dev = boardInitDevice((teBoardDeviceType)compC->violed.type, compC->violed.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->violed.value);
    pDev->violed = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->vacuumBox.type, compC->vacuumBox.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->VacuumBox.value);
    pDev->vacuumBox = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->vacuumBoxHeater.type, compC->vacuumBoxHeater.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->VacuumBoxHeater.value);
    pDev->vacuumBoxHeater = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->humanSensor.type, compC->humanSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->humanSensor.value);
    pDev->humanSensor = dev;
#ifdef ALPS_H50    
    dev = boardInitDevice((teBoardDeviceType)compC->doorAssistInputSwitch.type, compC->doorAssistInputSwitch.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->doorAssistInputSwitch.value);
    pDev->doorAssistInputSwitch = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->doorAssistOut.type, compC->doorAssistOut.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->doorAssistOut.value);
    pDev->doorAssistOut = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->doorAssistOut.type, compC->doorAssistOut.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->doorAssistOut.speedOrPWM);
    pDev->doorAssistOut = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->doorAssistMotor.type, compC->doorAssistMotor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeRamToDevice, dev, &compP->doorAssistMotor.value);
    pDev->doorAssistMotor = dev;
    
    dev = boardInitDevice((teBoardDeviceType)compC->doorAssistMotorOverCurrentSensor.type, compC->doorAssistMotorOverCurrentSensor.index, NULL);
    devSyncAttach(consts->sync, eSyncTypeDeviceToRam, dev, &compP->doorAssistMotorOverCurrentSensor.valueInCExt);
    pDev->doorAssistMotorOverCurrentSensor = dev;
#endif
}

static void cabinInit(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    cabinP->sets.user.functions.Quick = false;
#ifndef HOSHI_H30       
    cabinP->sets.user.functions.TempSettingFine = true;
#endif //HOSHI_H30       
    switch (cabinC->type)
    {
    case _CabinType_FF:
        cabinFfInit(consts, params);
        break;

    case _CabinType_FRZ:
        cabinFrzInit(consts, params);
        break;
#ifdef JKRavailable
    case _CabinType_JKR:
        cabinJkrInit(consts, params);
        break;
#endif
       
    case _CabinType_FLAP:
      cabinFlapInit(consts, params);
      break;

        /*case _CabinType_BioFRESH:
            BioFresh_Init(cabinP);
        break;*/

    case _CabinType_NONE:
    default:;
        break;
    }
    
    cabinUserSetsCompSpeedUpInit(consts, params);
    cabinDutyInit(consts, params);
    cabinDefrostMeanTimeInit(consts, params);

    // CabinPerformanceInit( cabinP );
    cabinSwitchInit(consts, params);
    cabinInnerLightInit(consts, params);
    cabinIonizerInit(consts, params);
    cabinVacuumBoxInit(consts, params);
    cabinVioLedInit(consts, params);

    cabinP->values.errSensors = false;
    cabinP->values.quickBlock = false;
    cabinP->values.highTempChckTimer = (24 * 60);
    cabinP->values.highTempStayTimer = 60;
    cabinP->components.fan.speedOrPWM = 0;
    cabinP->components.flap.value = 0;
    cabinP->components.heaterA.value = _HeaterValue_OFF;
    cabinP->components.heaterB.value = _HeaterValue_OFF;
    cabinP->components.heaterC.value = _HeaterValue_OFF;
    cabinP->components.innerLight.value = 0;
    // cabinP->components.innerLight.step = INNER_LIGHT_STEP;
    cabinP->components.blueLight.value = 0;
    // cabinP->components.blueLight.step = BLUE_LIGHT_STEP;
    cabinP->components.handleLight.R_value = 0;
    cabinP->components.handleLight.G_value = 0;
    cabinP->components.handleLight.B_value = 0;
    cabinP->components.ionizer.value = _IonizerValue_OFF;
    cabinP->components.violed.value = _VioLedValue_OFF;
    cabinP->values.defrostDelayTimerForIceCream =
        //cabinC->parameters.Defrost.MaxDelayTimeForIceCream;
    cabinP->stateTimers.PumpOut = 0;
#if defined (AI_COOL_V1)
    if(cabinP->ai.isEnabled)
      cabinP->values.defrost.variableTime = cabinC->parameters.Defrost.VariableMaxRealTime_AI[cabinP->values.ambientType][_DefrostInInteval_0];
    else
#endif
      cabinP->values.defrost.variableTime = cabinC->parameters.Defrost.VariableMaxRealTime[cabinP->values.ambientType][_DefrostInInteval_0];
    cabinP->values.defrost.variableReason = _DefrostInInteval_0;

    cabinP->values.ambientType = params->rf->Cooler.parameters.ambientType;
    cabinP->values.humidityType = params->rf->Cooler.parameters.humidityType;
    cabinP->values.defrost.delayTimer = cabinC->parameters.Defrost.DelayTime;
    cabinP->values.defrost.preCoolActive = 0;
    cabinP->values.CoolPreCoolOffset = 0;
    cabinP->values.FanPreCoolOffset = 0;
    cabinP->values.defrost.recoveryModeActive = 0;
    cabinP->values.activeDefrostHighTempTimer =  cabinC->parameters.Defrost.DefrostHighTemp_Activation;
    cabinP->values.afterDefrostTimer = 0;
    cabinP->values.defrost.afterDefrostActive = 0;
    cabinP->values.firstDefrostFlag = true;
    cabinP->values.finishDefrostFlag = false;
#ifdef ALPS_H50
    if(cabinC->components.doorAssistMotor.type != _DoorAssistMotorType_NONE)
      cabinP->values.doorAssist.initRequest = true;
#endif    
}

static void cabinNormalState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    cabinUserSets(consts, params);
    cabinP->values.ambientType = params->rf->Cooler.parameters.ambientType;
    cabinP->values.humidityType = params->rf->Cooler.parameters.humidityType;

    // CabinTask start
    switch (cabinC->type)
    {
    case _CabinType_FF:
        cabinFfTask(consts, params);
        break;

    case _CabinType_FRZ:
        cabinFrzTask(consts, params);
        break;
#ifdef JKRavailable
    case _CabinType_JKR:
        cabinJkrTask(consts, params);
        break;
#endif   
 
    case _CabinType_FLAP:
        cabinFlapTask(consts, params);
        break;
        /*case _CabinType_BioFRESH:
            BioFreshTask(consts, params);
        break;*/

    case _CabinType_NONE:
    default:;
        break;
    }

    if (cabinC->type == _CabinType_FRZ &&
        !cabinP->sets.user.functions.Convertible &&
        !cabinP->sets.user.functions.CabinOff)
    {
        cabinCheckHighTempErr(consts, params);
    }
    cabinCheckDefrostHeaterErr(consts, params);

    cabinQuickControl(consts, params);
    cabinFanControl(consts, params);
    cabinFlapControl(consts, params);       
    cabinAHeaterControl(consts, params);
    cabinBHeaterControl(consts, params);
    cabinCHeaterControl(consts, params);

    cabinSwitchTask(consts, params);
    cabinVacuumBoxTask(consts, params);
    cabinBlueLightTask(consts, params);
    cabinInnerLightTask(consts, params);
    cabinHandleLightTask(consts, params);
    cabinHandleLightAnimationTask(params);
    cabinVioLedTask(consts, params);
    cabinDefrostUpdateInterval(consts, params);
    // CabinTask end

    //cabinDefrostDelayForIceCream(consts, params);
    cabinSensorsErr(consts, params);
    cabinQuickFrz(consts, params);
    cabinQuickFf(consts, params);
    cabinAirSupply(consts, params);
    //cabinQuickIceCream(consts, params);
    cabinQuickIce(consts, params);
}

static void cabinDistributerState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    cabinUserSets(consts, params);

    // CabinsDistributerTask start
    cabinSwitchTask(consts, params);

    // CHECKPOINT
    // if (params->rf->Values.dispOff) // this is better ?
    if (params->rf->ui.dspPacket.funcs.ledApplianceOff)
    {
        params->cabin->components.blueLight.value = 0;
        params->cabin->components.innerLight.value = 0;
        params->cabin->components.handleLight.R_value = 0;
        params->cabin->components.handleLight.G_value = 0;
        params->cabin->components.handleLight.B_value = 0;
        params->cabin->values.innerLight.state = 0;
        params->cabin->values.innerLight.sync = 0;
    }
    else
    {
        cabinBlueLightTask(consts, params);
        cabinInnerLightTask(consts, params);
        cabinHandleLightTask(consts, params);
    }
    // CabinsDistributerTask end
    cabinHandleLightAnimationTask(params);
    cabinSensorsErr(consts, params);
    cabinQuickFrz(consts, params);
    cabinQuickFf(consts, params);
    cabinAirSupply(consts, params);
    //cabinQuickIceCream(consts, params);
    cabinQuickIce(consts, params);
}

static void cabinProdTestState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    
    cabinUserSets(consts, params);
    switch (params->rf->workMode.values.prodTest.state)
    {
    case _WM_ProductionTest_State_CompTest:
        cabinComponentsTest(consts, params);
        break;

    case _WM_ProductionTest_State_PerformanceAndSensorTests:
        cabinPerformanceTestTask(consts, params);
        break;

    case _WM_ProductionTest_State_EndedByError:
    case _WM_ProductionTest_State_EndedBySensorNOK:
    case _WM_ProductionTest_State_EndedByPerformanceNOK: // AAAA
    case _WM_ProductionTest_State_EndedByNoHotStart:
        cabinTestsFailedTask(consts, params);
        break;
    default:
        break;
    }

    if (params->rf->workMode.values.prodTest.state != _WM_ProductionTest_State_CompTest)
    {
        cabinP->values.gasApplied.type = params->rf->Cooler.parameters.gasApplied[consts->cabinNo].type;
        cabinP->values.gasApplied.level = params->rf->Cooler.parameters.gasApplied[consts->cabinNo].level;

        cabinQuickControl(consts, params);
        cabinSwitchTask(consts, params);

        //cabinDefrostDelayForIceCream(consts, params);
        if (cabinC->type == _CabinType_FRZ)
            cabinCheckHighTempErr(consts, params);
        cabinCheckDefrostHeaterErr(consts, params);

        cabinSensorsErr(consts, params);
        cabinQuickFrz(consts, params);
        cabinQuickFf(consts, params);
        cabinAirSupply(consts, params);
        //cabinQuickIceCream(consts, params);
        cabinQuickIce(consts, params);
    }
}

/*** APW : Add CT heater software ***/
static void cabinCommercialTestState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    
    cabinUserSets(consts, params);
    
    // CabinTask start
    switch (cabinC->type)
    {
    case _CabinType_FF:
        cabinFfTask(consts, params);
        break;

    case _CabinType_FRZ:
        cabinFrzTask(consts, params);
        break;
#ifdef JKRavailable
    case _CabinType_JKR:
        //cabinJkrTask(consts, params);
        break;
#endif

    case _CabinType_FLAP:
        cabinFlapTask(consts, params);
        break;
      
    case _CabinType_NONE:
      
    default:;
        break;
    }
     
    if (params->rf->workMode.values.CommercialTest.state != _WM_CommercialTest_State_CompTest)
    {
        if (cabinC->type == _CabinType_FRZ &&
            !cabinP->sets.user.functions.Convertible &&
            !cabinP->sets.user.functions.CabinOff)
        {
            cabinCheckHighTempErr(consts, params);
        }
        cabinCheckDefrostHeaterErr(consts, params);

        cabinQuickControl(consts, params);
        cabinFanControl(consts, params);
        //cabinFlapControl(consts, params);
        cabinAHeaterControl(consts, params);
        cabinBHeaterControl(consts, params);
        cabinCHeaterControl(consts, params);

        cabinSwitchTask(consts, params);
        cabinVacuumBoxTask(consts, params);
        cabinBlueLightTask(consts, params);
        cabinInnerLightTask(consts, params);
        cabinHandleLightTask(consts, params);
        cabinHandleLightAnimationTask(params);
        cabinVioLedTask(consts, params);
        cabinDefrostUpdateInterval(consts, params);
        // CabinTask end

        //cabinDefrostDelayForIceCream(consts, params);
        cabinSensorsErr(consts, params);
        cabinQuickFrz(consts, params);
        cabinQuickFf(consts, params);
        cabinAirSupply(consts, params);
        //cabinQuickIceCream(consts, params);
        cabinQuickIce(consts, params);
    }
    
    if (params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest)
    {
        if(params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial == 0 &&
           params->rf->workMode.values.CommercialTest.SensorTempRecord == 1)
        {
               params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[0] = 
                 params->rf->Cabin[0].components.evaTempSensor.valueInCExt;
               params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[0] = 
                 params->rf->Cabin[1].components.evaTempSensor.valueInCExt;
               params->rf->workMode.values.CommercialTest.SensorTempRecord = 0;
               
               if(params->rf->workMode.values.CommercialTest.LastSensorTempRecord == 1)
                    params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial = 0;
        }
        if(params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial == 1 &&
           params->rf->workMode.values.CommercialTest.SensorTempRecord == 1)
        {
               params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[1] = 
                 params->rf->Cabin[0].components.evaTempSensor.valueInCExt;
               params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[1] = 
                 params->rf->Cabin[1].components.evaTempSensor.valueInCExt;
               params->rf->workMode.values.CommercialTest.SensorTempRecord = 0;
               
               if(params->rf->workMode.values.CommercialTest.LastSensorTempRecord == 1)
                    params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial = 0;
        }
        if(params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial == 2 &&
           params->rf->workMode.values.CommercialTest.SensorTempRecord == 1)
        {
               params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[2] = 
                 params->rf->Cabin[0].components.evaTempSensor.valueInCExt;
               params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[2] = 
                 params->rf->Cabin[1].components.evaTempSensor.valueInCExt;
               params->rf->workMode.values.CommercialTest.SensorTempRecord = 0;
               
               if(params->rf->workMode.values.CommercialTest.LastSensorTempRecord == 1)
                    params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial = 0;
        }
        if(params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial == 3 &&
           params->rf->workMode.values.CommercialTest.SensorTempRecord == 1)
        {
               params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[3] = 
                 params->rf->Cabin[0].components.evaTempSensor.valueInCExt;
               params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[3] = 
                 params->rf->Cabin[1].components.evaTempSensor.valueInCExt;
               params->rf->workMode.values.CommercialTest.SensorTempRecord = 0;
               
               if(params->rf->workMode.values.CommercialTest.LastSensorTempRecord == 1)
                    params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial = 0;
        }
        if(params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial == 4 &&
           params->rf->workMode.values.CommercialTest.SensorTempRecord == 1)
        {
               params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[4] = 
                 params->rf->Cabin[0].components.evaTempSensor.valueInCExt;
               params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[4] = 
                 params->rf->Cabin[1].components.evaTempSensor.valueInCExt;
               params->rf->workMode.values.CommercialTest.SensorTempRecord = 0;
               
               if(params->rf->workMode.values.CommercialTest.LastSensorTempRecord == 1)
                    params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial = 0;
        }
        if(params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial == 5 &&
           params->rf->workMode.values.CommercialTest.SensorTempRecord == 1)
        {
               params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[5] = 
                 params->rf->Cabin[0].components.evaTempSensor.valueInCExt;
               params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[5] = 
                 params->rf->Cabin[1].components.evaTempSensor.valueInCExt;
               params->rf->workMode.values.CommercialTest.SensorTempRecord = 0;
               
               if(params->rf->workMode.values.CommercialTest.LastSensorTempRecord == 1)
                    params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial = 0;
        }
    }
    
    // Case : 3
    if (params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_JudgmentResult)
    {     
        if(params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[1] != 0x7FFF &&
           params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[1] != 0x7FFF)
        {
            // First Valve work FF Temp check in Cabin0(FF)
            if((params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[0] - 
                params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[1]) >= 
                consts->rf->Cabin[0].parameters.TempJudgmentValveFfCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;
            }
            // First Valve work FF Temp check in Cabin1(FRZ)
            if((params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[1] - 
                params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[0]) >= 
                consts->rf->Cabin[1].parameters.TempJudgmentValveFrzCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  //Disable check first cycle in Cabin1(FRZ)
                  /*params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;*/
                  //Skip by judgment ValveARM false
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            }
        }
        
        if(params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[2] != 0x7FFF &&
           params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[2] != 0x7FFF)
        {
            // Second Valve work FRZ Temp check in Cabin0(FF)
            if((params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[2] - 
                params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[1]) >= 
                consts->rf->Cabin[0].parameters.TempJudgmentValveFfCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;
            }
            // Second Valve work FRZ Temp check in Cabin1(FRZ)
            if((params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[1] - 
                params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[2]) >= 
                consts->rf->Cabin[1].parameters.TempJudgmentValveFrzCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;
            }
        }

        if(params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[3] != 0x7FFF &&
           params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[3] != 0x7FFF)
        {
            // Third Valve work FF Temp check in Cabin0(FF)
            if((params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[2] - 
                params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[3]) >= 
                consts->rf->Cabin[0].parameters.TempJudgmentValveFfCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;
            }
            // Third Valve work FF Temp check in Cabin1(FRZ)
            if((params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[3] - 
                params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[2]) >= 
                consts->rf->Cabin[1].parameters.TempJudgmentValveFrzCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;
            }
        }
      
        if(params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[4] != 0x7FFF &&
           params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[4] != 0x7FFF)
        {
            // Fourth Valve work FRZ Temp check in Cabin0(FF)
            if((params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[4] - 
                params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[3]) >= 
                consts->rf->Cabin[0].parameters.TempJudgmentValveFfCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;
            }
            // Fourth Valve work FRZ Temp check in Cabin1(FRZ)
            if((params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[3] - 
                params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[4]) >= 
                consts->rf->Cabin[1].parameters.TempJudgmentValveFrzCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;
            }
        }
      
        if(params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[5] != 0x7FFF &&
           params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[5] != 0x7FFF)
        {
            // Five Valve work FF Temp check in Cabin0(FF)
            if((params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[4] - 
                params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[5]) >= 
                consts->rf->Cabin[0].parameters.TempJudgmentValveFfCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;
            }
            // Five Valve work FF Temp check in Cabin1(FRZ)
            if((params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[5] - 
                params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[4]) >= 
                consts->rf->Cabin[1].parameters.TempJudgmentValveFrzCommercial)
                  params->rf->workMode.values.ARMModeError.ValveARM = false;
            else
            {
                  params->rf->workMode.values.ARMModeError.ValveARM = true;
                  params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
                  return;
            }
        }
        
        // When check all Temp completed
        if(!params->rf->workMode.values.ARMModeError.ValveARM)
            params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_CoolingTest;
    }
    
    //APW : Add handle check function
    if(cabinP->values.SelectedColorIndexAtCTMode == 1)
    {
      if(cabinP->values.SelectedColorIndexAtCTModeTimerChange > 2)
      {
        cabinP->values.selectedColorIndex = 12;
        cabinP->values.SelectedColorIndexAtCTMode = 2;
        cabinP->values.SelectedColorIndexAtCTModeTimerChange = 0;
      }
      else
        cabinP->values.selectedColorIndex = 14;  //Red
    }
    else if(cabinP->values.SelectedColorIndexAtCTMode == 2)
    {
      if(cabinP->values.SelectedColorIndexAtCTModeTimerChange > 2)
      {
        cabinP->values.selectedColorIndex = 13;
        cabinP->values.SelectedColorIndexAtCTMode = 3;
        cabinP->values.SelectedColorIndexAtCTModeTimerChange = 0;
      }
      else
        cabinP->values.selectedColorIndex = 12; //Green
    }
    else if(cabinP->values.SelectedColorIndexAtCTMode == 3)
    {
      if(cabinP->values.SelectedColorIndexAtCTModeTimerChange > 2)
      {
        cabinP->values.selectedColorIndex = 1; 
        cabinP->values.SelectedColorIndexAtCTMode = 4;
        cabinP->values.SelectedColorIndexAtCTModeTimerChange = 0;
      }
      else
        cabinP->values.selectedColorIndex = 13; //Blue
    }
    else
    {
      if(cabinP->values.SelectedColorIndexAtCTModeTimerChange > 2)
      {
        cabinP->values.selectedColorIndex = 14;
        cabinP->values.SelectedColorIndexAtCTMode = 1;
        cabinP->values.SelectedColorIndexAtCTModeTimerChange = 0;
      }
      else
        cabinP->values.selectedColorIndex = 1;
    }
}
static void cabinCommercialDefrostTestState20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    //stCabin_Ram *cabinP = params->cabin;
    
    cabinUserSets(consts, params);
    
    // CabinTask start
    switch (cabinC->type)
    {
    case _CabinType_FF:
        cabinVacuumBoxTask(consts, params);
        cabinCloseAllComponentsCommercialDefrost(consts, params);
        cabinFfTask(consts, params);
        break;

    case _CabinType_FRZ:
        cabinVacuumBoxTask(consts, params);
        cabinCloseAllComponentsCommercialDefrost(consts, params);
        cabinFrzTask(consts, params);
        break;
#ifdef JKRavailable
    case _CabinType_JKR:
        //cabinJkrTask(consts, params);
        break;
#endif

    case _CabinType_FLAP:
        cabinVacuumBoxTask(consts, params);
        cabinCloseAllComponentsCommercialDefrost(consts, params);
        cabinFlapTask(consts, params);
        break;

    case _CabinType_NONE:
    default:;
        break;
    }     
}

void cabinCloseAllComponentsCommercialDefrost(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
        for(unsigned char i = 0; i <= 2; i++)
        {
        params->cabin[i].components.fan.speedOrPWM = 0;
        params->cabin[i].components.flap.value = 0;
        params->cabin[i].components.ionizer.value = _IonizerValue_OFF;
        params->cabin[i].components.violed.value = _VioLedValue_OFF;
        params->cabin[i].components.blueLight.value = 0;
        params->cabin[i].components.innerLight.value = 0;
	// params->cabin[i].components.blueLight.step   = BLUE_LIGHT_STEP;
	// params->cabin[i].components.innerLight.step  = INNER_LIGHT_STEP;
        params->cabin[i].components.handleLight.R_value = 0;
        params->cabin[i].components.handleLight.G_value = 0;
        params->cabin[i].components.handleLight.B_value = 0;
        params->cabin[i].components.VacuumBox.value = _VacuumValue_OFF;
        }
        
        //cabinP->components.heaterA.value = _HeaterValue_OFF;
        //cabinP->components.heaterB.value = _HeaterValue_OFF;
        //cabinP->components.heaterC.value = _HeaterValue_OFF;       
}

unsigned char cabinGetLogData(stCabin_Ram *cabinP, unsigned char *dataArrayPtr)
{
    unsigned char size = 0;

    dataArrayPtr[size++] =
        celciusExtToCelcius(cabinP->components.airTempSensor.valueInCExt);

    dataArrayPtr[size++] =
        celciusExtToCelcius(cabinP->components.evaTempSensor.valueInCExt);

    dataArrayPtr[size++] =
        ((cabinP->components.fan.speedOrPWM ? 0x01 : 0) << 0) |
        ((cabinP->components.flap.value ? 0x01 : 0) << 1) |     
        ((cabinP->components.heaterA.value & 0x01) << 2) |
        ((cabinP->components.heaterB.value & 0x01) << 3) |
        ((cabinP->components.heaterC.value & 0x01) << 4) |
        ((cabinP->components.ionizer.value & 0x01) << 5) |
        ((cabinP->components.innerLight.value ? 0x01 : 0) << 6) |
        ((cabinP->components.blueLight.value ? 0x01 : 0) << 7);

    return size;
}

/*******************************************/

static void cabinInitAfterHotStart(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    cabinE2AfterHotStart(consts, params);
    cabinE2ToRam(consts, params);
}

static void cabinClearDisplaySetting20MsTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    cabinE2ClearDisplaySetting(consts, params);
}

static void cabinInitAfterService(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    cabinE2AfterService(consts, params);
    cabinE2ToRam(consts, params);
}

static void cabinE2Init(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    switch (cabinC->type)
    {
    case _CabinType_FRZ:
    case _CabinType_JKR:
        cabinP->E2.Mode = _CabinMode_FRZ;
        break;

    case _CabinType_FF:
    case _CabinType_FLAP:
    // case _CabinType_BioFRESH:
    case _CabinType_NONE:
    default:
        cabinP->E2.Mode = _CabinMode_FF;
        break;
    }

    cabinP->E2.FF = 0;
    cabinP->E2.FRZ = 0;
    cabinP->E2.Convertible = 0;
    cabinP->E2.Vacation = 0;
    cabinP->E2.Rice = 0;
    cabinP->E2.TempSettingFine = 0;
    // cabinP->E2.errHighTemp = 0;
    cabinP->E2.realTimer = 0;
    cabinP->E2.workTimer = 0;
    cabinP->E2.errDefrostHeaterCounter = 0;
    cabinP->E2.meanCoolTime = _CabinMeanCoolTime;
    cabinP->E2.meanIdleTime = _CabinMeanIdleTime;

    cabinP->E2.highTempStatus = _idle;
    cabinP->E2.HighTempIsactive = 0;
    cabinP->E2.FirstCutoutDoneF = 0;
    cabinP->E2.AlgorithmActivationTime = 0;
    cabinP->E2.maxFRZTempInCext = cabinResetMaxTempInCext(consts, params);
    if(cabinC->components.vacuumBox.type)
      cabinP->E2.VacuumSet = _VacuumSet_MinusOne;
    cabinP->E2.selectedColorIndex = 1;
}

static void cabinE2AfterHotStart(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    /*** APW: Disable for reset other process(Just display) ***/
    /*cabinP->E2.FF = 0;
    cabinP->E2.FRZ = 0;
    cabinP->E2.Convertible = 0;
    cabinP->E2.Vacation = 0;
    cabinP->E2.Rice = 0;
    cabinP->components.violed.value = _VioLedValue_OFF;
    cabinP->values.violed.state = _VioLedState_OFF;
    cabinP->E2.TempSettingFine = 0;
    cabinP->E2.VacuumSet = _VacuumSet_MinusOne;
    cabinP->E2.selectedColorIndex = 1;*/
    /*** APW: Disable for reset other process(Just display) ***/
    // cabinP->E2.errHighTemp = 0;
//    cabinP->E2.realTimer =
//        cabinC->parameters.Defrost.RealTimeAfterHotStart;
    if(EEPROM_R.eep_us_Backup_RRealTimer >= 0xFFFF)
      params->rf->Cabin[0].values.realTimer = 0;
    else if(EEPROM_R.eep_us_Backup_RRealTimer >= params->rf->Cabin[0].E2.realTimer)
      params->rf->Cabin[0].values.realTimer = EEPROM_R.eep_us_Backup_RRealTimer;
    else
      params->rf->Cabin[0].values.realTimer = params->rf->Cabin[0].E2.realTimer;
    
    if(EEPROM_R.eep_us_Backup_FRealTimer >= 0xFFFF)
      params->rf->Cabin[1].values.realTimer = 0;
    else if(EEPROM_R.eep_us_Backup_FRealTimer >= params->rf->Cabin[1].E2.realTimer)
      params->rf->Cabin[1].values.realTimer = EEPROM_R.eep_us_Backup_FRealTimer;
    else
      params->rf->Cabin[1].values.realTimer = params->rf->Cabin[1].E2.realTimer;
    
    if(EEPROM_R.eep_us_Backup_RDoorOpenTimer >= 0xFFFF)
      params->rf->Cabin[0].values.defrostDoorOpenedTimer = 0;
    else
      params->rf->Cabin[0].values.defrostDoorOpenedTimer = EEPROM_R.eep_us_Backup_RDoorOpenTimer;
    
    if(EEPROM_R.eep_us_Backup_FDoorOpenTimer >= 0xFFFF)
      params->rf->Cabin[1].values.defrostDoorOpenedTimer = 0;
    else
      params->rf->Cabin[1].values.defrostDoorOpenedTimer = EEPROM_R.eep_us_Backup_FDoorOpenTimer;
    
    cabinP->E2.workTimer = 0;
    // cabinP->E2.errDefrostHeaterCounter = 0; Sicak calismada unutulur.
    cabinP->E2.meanCoolTime = _CabinMeanCoolTime;
    cabinP->E2.meanIdleTime = _CabinMeanIdleTime;
}

static void cabinE2ClearDisplaySetting(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    cabinP->E2.FF = 0;
    cabinP->E2.FRZ = 0;
    cabinP->E2.Convertible = 0;
    cabinP->E2.Vacation = 0;
    cabinP->E2.Rice = 0;
    cabinP->E2.TempSettingFine = 0;
    cabinP->components.violed.value = _VioLedValue_OFF;
    cabinP->values.violed.state = _VioLedState_OFF;
    if(cabinC->components.vacuumBox.type)
      cabinP->E2.VacuumSet = _VacuumSet_MinusOne;
    cabinP->E2.selectedColorIndex = 1;
}

static void cabinE2AfterService(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    cabinDutyInit(consts, params);
    cabinDefrostMeanTimeInit(consts, params);
    cabinP->E2.FF = 0;
    cabinP->E2.FRZ = 0;
    cabinP->E2.Convertible = 0;
    cabinP->E2.Vacation = 0;
    cabinP->E2.Rice = 0;
    cabinP->E2.TempSettingFine = 0;
    // cabinP->E2.errHighTemp = 0;
    cabinP->E2.realTimer = cabinC->parameters.Defrost.RealTimeAfterService;
    cabinP->E2.workTimer = 0;
    cabinP->E2.errDefrostHeaterCounter = 0;
    cabinP->E2.meanCoolTime = _CabinMeanCoolTime;
    cabinP->E2.meanIdleTime = _CabinMeanIdleTime;

    cabinP->E2.highTempStatus = _idle;
    cabinP->E2.HighTempIsactive = 0;
    cabinP->E2.FirstCutoutDoneF = 0;
    cabinP->E2.AlgorithmActivationTime = 0;
    cabinP->E2.maxFRZTempInCext = cabinResetMaxTempInCext(consts, params);
    if(cabinC->components.vacuumBox.type)
      cabinP->E2.VacuumSet = _VacuumSet_MinusOne;
    cabinP->E2.selectedColorIndex = 1;
}

static void cabinE2ToRam(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    cabinP->sets.user.Mode = cabinP->E2.Mode;
    cabinP->sets.user.FF = cabinP->E2.FF;
    cabinP->sets.user.FRZ = cabinP->E2.FRZ;
    cabinP->sets.user.Convertible = cabinP->E2.Convertible;
    cabinP->sets.user.functions.Vacation = cabinP->E2.Vacation;
    cabinP->sets.user.functions.Rice = cabinP->E2.Rice;
    cabinP->sets.user.functions.TempSettingFine = cabinP->E2.TempSettingFine;
    // cabinP->values.errHighTemp = cabinP->E2.errHighTemp;
    
    if(EEPROM_R.eep_us_Backup_RRealTimer >= 0xFFFF)
      params->rf->Cabin[0].values.realTimer = 0;
    else if(EEPROM_R.eep_us_Backup_RRealTimer >= params->rf->Cabin[0].E2.realTimer)
      params->rf->Cabin[0].values.realTimer = EEPROM_R.eep_us_Backup_RRealTimer;
    else
      params->rf->Cabin[0].values.realTimer = params->rf->Cabin[0].E2.realTimer;
    
    if(EEPROM_R.eep_us_Backup_FRealTimer >= 0xFFFF)
      params->rf->Cabin[1].values.realTimer = 0;
    else if(EEPROM_R.eep_us_Backup_FRealTimer >= params->rf->Cabin[1].E2.realTimer)
      params->rf->Cabin[1].values.realTimer = EEPROM_R.eep_us_Backup_FRealTimer;
    else
      params->rf->Cabin[1].values.realTimer = params->rf->Cabin[1].E2.realTimer;
    
    if(EEPROM_R.eep_us_Backup_RDoorOpenTimer >= 0xFFFF)
      params->rf->Cabin[0].values.defrostDoorOpenedTimer = 0;
    else
      params->rf->Cabin[0].values.defrostDoorOpenedTimer = EEPROM_R.eep_us_Backup_RDoorOpenTimer;
    
    if(EEPROM_R.eep_us_Backup_FDoorOpenTimer >= 0xFFFF)
      params->rf->Cabin[1].values.defrostDoorOpenedTimer = 0;
    else
      params->rf->Cabin[1].values.defrostDoorOpenedTimer = EEPROM_R.eep_us_Backup_FDoorOpenTimer;
    
    //cabinP->values.realTimer = cabinP->E2.realTimer;
    cabinP->values.workTimer = cabinP->E2.workTimer;
    cabinP->values.errDefrostHeaterCounter = cabinP->E2.errDefrostHeaterCounter;
    cabinP->values.duty.meanCoolTime = cabinP->E2.meanCoolTime;
    cabinP->values.duty.meanIdleTime = cabinP->E2.meanIdleTime;

    cabinP->values.HighTemErr.highTempStatus = cabinP->E2.highTempStatus;
    if (cabinP->values.HighTemErr.highTempStatus != _alarmIsActive)
        cabinP->values.HighTemErr.highTempStatus = _idle;

    cabinP->values.HighTemErr.isactive = cabinP->E2.HighTempIsactive;
    cabinP->values.HighTemErr.FirstCutoutDoneF = cabinP->E2.FirstCutoutDoneF;
    cabinP->values.HighTemErr.algorithmActivationTime = cabinP->E2.AlgorithmActivationTime;
    // cabinP->values.HighTemErr.maxFRZTempADC = cabinP->E2.maxFRZTempADC;
    cabinP->values.HighTemErr.maxFRZTempInCext = cabinP->E2.maxFRZTempInCext;
    cabinP->values.VacuumBox.sets = cabinP->E2.VacuumSet;
    cabinP->values.selectedColorIndex = cabinP->E2.selectedColorIndex;
}

/********************************************************************/

/// @brief  Event handler function of cabin
static PROCESS_HANDLER_PROTO(cabinProcIdleStateHandler)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventInitAfterStartup:
        cabinE2ToRam(consts, params);
        cabinInit(consts, params);
        params->cabin->values.componentsTestDone = false;

        timerEventStart(&params->te.periodic20Ms, 20);
        timerEventStart(&params->te.periodic1Sec, 1000);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        timerEventStart(&params->te.periodic1Hour, 60 * 60 * 1000);
        break;

    default:
        cabinProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(cabinProcNormalStateHandler)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eCabinEventPeriodic20Ms:
        cabinNormalState20MsTask(consts, params);
        cabinProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    default:
        cabinProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(cabinProcAppOffStateHandler)
{
    tsCabinProcParams *params = process->parameters;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        cabinCloseAllComponents(params->cabin);
        break;

    default:
        cabinProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(cabinProcProdTestStateHandler)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        if (consts->cabinNo <= 1) // needed ?
            params->cabin->states.Work = _Work_ProductionTestMode;
        break;

    case eCabinEventStartPerformanceAndSensorsTest:
        cabinPerformanceTestInit(consts->cabin, params->cabin);
        break;
        
    case eCabinEventIsProductionTestCondition:
        isProductionTestCondition(consts->cabin, params->cabin);
        break;

    case eCabinEventPeriodic20Ms:
        cabinProdTestState20MsTask(consts, params);
        cabinProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
        
    default:
        cabinProcDefaultEventHandler(process);
        break;
    }
}

/*** APW : Add CT heater software ***/
static PROCESS_HANDLER_PROTO(cabinProcCommercialTestStateHandler)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;
    
    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        if (consts->cabinNo <= 1) // needed ?
            params->cabin->states.Work = _Work_CommercialTestMode;
            params->rf->Cabin[0].values.SelectedColorIndexAtCTMode = 1;
            params->rf->Cabin[1].values.SelectedColorIndexAtCTMode = 1;
            params->rf->Cabin[0].values.SelectedColorIndexAtCTModeTimerChange = 0;
            params->rf->Cabin[1].values.SelectedColorIndexAtCTModeTimerChange = 0;
        break;

    case eCabinEventStartCommercialTest:
        cabinPerformanceCommercialTestInit(consts->cabin, params->cabin);
        
        for(unsigned char i = 0; i <= 5; i++)
        {
          params->rf->Cabin[0].values.ValveEvaTempCheckCommercial[i] = 0x7FFF;
          params->rf->Cabin[1].values.ValveEvaTempCheckCommercial[i] = 0x7FFF;
        }

        timerEventStart(&params->te.periodic1Sec, 1000);
        break;

    case eCabinEventPeriodic20Ms:
        cabinCommercialTestState20MsTask(consts, params);
        cabinProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
        
    default:
        cabinProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(cabinProcCommercialDefrostTestStateHandler)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;
    
    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        if (consts->cabinNo <= 1) // needed ?
            params->cabin->states.Work = _Work_CommercialDefrostTestMode;
        
        break;

    case eCabinEventPeriodic20Ms:
        cabinCommercialDefrostTestState20MsTask(consts, params);
        cabinProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;
        
    default:
        cabinProcDefaultEventHandler(process);
        break;
    }
}
/*** APW : Add CT heater software ***/

static PROCESS_HANDLER_PROTO(cabinProcServiceStateHandler)
{
    const tsCabinProcConsts *consts = process->constants;
    tsCabinProcParams *params = process->parameters;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        if (consts->cabinNo <= 1) // needed ?
            params->cabin->states.Work = _Work_ServiceTest;

        cabinInitAfterService(consts, params);
        break;

    default:
        cabinProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(cabinProcDistributerStateHandler)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eSysEventEnterState:
        if (params->rf->workMode.values.fairModeFlag)
            params->cabin->states.Work = _Work_FairMode;
        else
            params->cabin->states.Work = _Work_DistributerMode;
        cabinCloseAllComponents(params->cabin);
        break;

    case eCabinEventPeriodic20Ms:
        cabinDistributerState20MsTask(consts, params);
        cabinProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);

    default:
        cabinProcDefaultEventHandler(process);
        break;
    }
}

static PROCESS_HANDLER_PROTO(cabinProcFTCStateHandler)
{
    cabinProcDefaultEventHandler(process);
}

static PROCESS_HANDLER_PROTO(cabinProcDefaultEventHandler)
{
    const tsCabinProcConsts *consts = process->constants;
    tsCabinProcParams *params = process->parameters;

    tfProcessHandler newHandler = NULL;

    switch (eventCurrent.event)
    {
    case eCabinEventPeriodic20Ms:
        cabinProcSuper20MsTask(process);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    case eCabinEventPeriodic1Sec:
        cabinProcSuper1SecTask(process);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;

    case eCabinEventPeriodic1Min:
        cabinProcSuper1MinTask(process);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;
        
    case eCabinEventPeriodic1Hour:
      cabinProcSuper1HourTask(process);
      timerEventStart(&params->te.periodic1Hour, 60 * 60 * 1000);
      break;

    case eSysEventInitAfterHotStart:
        cabinInitAfterHotStart(consts, params);
        break;

    case eSysEventReinit:
        cabinInit(consts, params);
        break;

    case eSysEventCalibrateAllStepperMotors:
      if (consts->cabin->components.flap.type == _FlapType_StepMotor)
        devIoInit(params->dev.flap, &consts->cabin->components.flap.stepperConfigs);
        break;

    case eSysEventStartNormalMode:
        newHandler = cabinProcNormalStateHandler;
        break;

    case eSysEventStartDistributerMode:
    case eSysEventStartFairMode:
        newHandler = cabinProcDistributerStateHandler;
        break;

    case eSysEventStartApplianceOffMode:
        newHandler = cabinProcAppOffStateHandler;
        break;

    case eSysEventStartServiceMode:
        newHandler = cabinProcServiceStateHandler;
        break;

    case eSysEventStartFTCMode:
        newHandler = cabinProcFTCStateHandler;
        break;
    case eSysEventStartProductionTestMode:
        newHandler = cabinProcProdTestStateHandler;
        break;
    /*** APW : Add CT heater software ***/
    case eSysEventStartCommercialTestMode:
        newHandler = cabinProcCommercialTestStateHandler;
        break;  
    case eSysEventStartCommercialDefrostTestMode:
        newHandler = cabinProcCommercialDefrostTestStateHandler;
        break;  
    /*** APW : Add CT heater software ***/
    }

    if (newHandler)
        sysProcChangeState(process, newHandler);
}

static PROCESS_HANDLER_PROTO(cabinProcSuper20MsTask)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;

    cabinInnerLight20MsTask(consts, params);
    cabinSwitch20MsTask(consts, params);
#ifdef ALPS_H50    
    cabinDoorAssistUnit20MsTask(consts, params);
#endif    
    
    if(refrigerator_R.ui.states.commercial.ClearStatusCommercial)
          cabinClearDisplaySetting20MsTask(consts, params);
    
}

static PROCESS_HANDLER_PROTO(cabinProcSuper1SecTask)
{
    tsCabinProcParams *params = process->parameters;
    const tsCabinProcConsts *consts = process->constants;
    stCabin_Ram *cabinP = params->cabin;
#ifdef HOSHI_H30
    const stCabin_Flash *cabinC = consts->cabin;
#endif //HOSHI_H30   
    
    cabinHandleLightT1sec(consts, params);
    cabinFlap1SecTask(consts, params);        
    cabinIonizer1SecTask(consts, params);
    cabinVacuumBox1SecTask(consts, params);
    cabinVioLed1SecTask(consts, params);

#ifdef ALPS_H50
    cabinDoorAssistUnitT1sec(consts, params);
#endif    

    if (cabinP->values.HighTemErr.delayTime)
        cabinP->values.HighTemErr.delayTime--;
    if (cabinP->stateTimers.PumpOut)
        cabinP->stateTimers.PumpOut--;
    if ((cabinP->components.doorSwitch[0].value == _SwitchValue_OPEN) ||
        (cabinP->components.doorSwitch[1].value == _SwitchValue_OPEN))
    {
#ifdef HOSHI_H30
      if(params->rf->Cabin[1].values.defrostDoorOpenedTimer < 0xFFFF)
        params->rf->Cabin[1].values.defrostDoorOpenedTimer++;   //All opened doors will be counted only for FRZ cabin.
#else   
      if(cabinP->values.defrostDoorOpenedTimer < 0xFFFF)
        cabinP->values.defrostDoorOpenedTimer++;
#endif //HOSHI        
        cabinP->values.doorsOpenedLevelIncreaseTimer++;
             
    }
    else
    {
      cabinP->values.doorsOpenedLevelIncreaseTimer = 0;
    }
    if (cabinP->values.heaterATimer)
      cabinP->values.heaterATimer--;
    
    if (cabinP->values.heaterCTimer)
      cabinP->values.heaterCTimer--;
    
#ifdef HOSHI_H30
    /*** APW : Add flap position time in CT ***/
    if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
    {
      if(cabinP->components.flap.flapFeed == 0)
        cabinP->values.FlapWaitingTimeCommercial_sec++;
      else if(cabinP->components.flap.flapFeed == cabinC->parameters.Flap.PositionA *10)
        cabinP->values.FlapWaitingTimeCommercial_sec++;
      else
        cabinP->values.FlapWaitingTimeCommercial_sec = 0;
    }
    /*** APW : Add flap position time in CT ***/
#endif //HOSHI_H30
    
    if(refrigerator_R.ui.states.commercial.status == _UI_Commercial_Test)
    {
      if(cabinP->values.SelectedColorIndexAtCTModeTimerChange < 0xFF)
        cabinP->values.SelectedColorIndexAtCTModeTimerChange++;
    }
    else
      cabinP->values.SelectedColorIndexAtCTModeTimerChange = 0xFF;
}

static PROCESS_HANDLER_PROTO(cabinProcSuper1MinTask)
{
    const tsCabinProcConsts *consts = process->constants;
    tsCabinProcParams *params = process->parameters;
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinC->type == _CabinType_FRZ &&
        !cabinP->sets.user.functions.Convertible &&
        !cabinP->sets.user.functions.CabinOff)
    {
        cabinCheckHighTempErr1MinTask(consts, params);
    }

//Defrost by High temperature count set
    if(!cabinP->values.activeDefrostHighTempTimer)
    {
        if(cabinP->values.defrostHighTempTimer == 0)
        {
            if(cabinP->components.airTempSensor.valueInCExt > cabinP->sets.datas.coolCutInTemp + cabinC->parameters.Defrost.MaxDefrostHighTempShiftCutInTemp)
            {  
                if(cabinP->values.defrostHighTempTimer < 0xFFFF)
                    cabinP->values.defrostHighTempTimer++;
            }
        }
        else
        {
            if(cabinP->components.airTempSensor.valueInCExt < cabinP->sets.datas.coolCutInTemp + cabinC->parameters.Defrost.MaxDefrostHighTempShiftCutOutTemp)
            {
                cabinP->values.defrostHighTempTimer = 0;
            }
            else
            {
                if(cabinP->values.defrostHighTempTimer < 0xFFFF)
                    cabinP->values.defrostHighTempTimer++;
            }
        }
    }
    else
      cabinP->values.activeDefrostHighTempTimer--;
    
    switch (cabinP->states.Work)
    {
    case _Work_Idle:
    case _Work_CoolWait:
        if (cabinP->stateTimers.Idle < 0xFFFF)
            cabinP->stateTimers.Idle++;
        break;

    case _Work_Cool:
        if (cabinP->stateTimers.Cool < 0xFFFF)
            cabinP->stateTimers.Cool++;
        if (cabinP->values.workTimer < 0xFFFF)
        {
            cabinP->values.workTimer++;
            if (!(cabinP->values.workTimer % 60))
                cabinP->E2.workTimer = cabinP->values.workTimer;
        }
#if defined (AI_COOL_V1)            
        if(cabinP->ai.isActivated)
        {
          if(cabinP->ai.compressor.workTimer < 0xFF)
            cabinP->ai.compressor.workTimer++;
        }
#endif
        break;
    /*
    case _Work_Heat:
        if( cabinP->stateTimers.Heat < 0xFF )
        cabinP->stateTimers.Heat++;
    break;
    */
    case _Work_PumpOut:
      if (cabinP->values.workTimer < 0xFFFF)
      {
        cabinP->values.workTimer++;
        if (!(cabinP->values.workTimer % 60))
          cabinP->E2.workTimer = cabinP->values.workTimer;
      }
      break;
    case _Work_Defrost:
        for (uint8_t i = 0; i < _NumOfCabin; i++)
        {
            if (consts->rf->Cabin[i].type != _CabinType_NONE)
                params->rf->Cabin[i].values.defrostHighTempTimer = 0;    //Defrost by High temperature count clear
        } 
        if (cabinP->stateTimers.Defrost < 0xFF)
            cabinP->stateTimers.Defrost++;
        break;

    case _Work_Drainage:
        if (cabinP->stateTimers.Drainage < 0xFF)
            cabinP->stateTimers.Drainage++;
        break;

    case _Work_HotStart:
        if (cabinP->stateTimers.HotStart < 0xFFFF)
            cabinP->stateTimers.HotStart++;
        break;
    case _Work_PerformanceTest:
        if (cabinP->stateTimers.PerformanceTest < 0xFFFF)
          cabinP->stateTimers.PerformanceTest++;
      break;   
    /*** APW : Add CT heater software ***/
    case _Work_CommercialTestMode:
        if (cabinP->stateTimers.CommercialTest < 0xFFFF)
            cabinP->stateTimers.CommercialTest++;
        break;
    /*** APW : Add CT heater software ***/
        
    default:;
        break;
    }

    if (cabinP->values.defrostDelayForIceCream &&
        cabinP->values.defrostDelayTimerForIceCream)
        cabinP->values.defrostDelayTimerForIceCream--;

    if (cabinP->values.realTimer < 0xFFFF)
    {
        cabinP->values.realTimer++;
        if (!(cabinP->values.realTimer % 60))
            cabinP->E2.realTimer = cabinP->values.realTimer;
    }

    if (cabinP->values.quickTimer < 0xFFFF)
    {
        /*
        EBI: At sensor errors, Quick will be active on display and deactive on mainboard.
        if( cabinP->sets.work.functions.Quick )
          cabinP->values.quickTimer++;
        */
        if (cabinP->sets.user.functions.Quick)
            cabinP->values.quickTimer++;
    }

    if (cabinP->values.highTempChckTimer)
        cabinP->values.highTempChckTimer--;

    if (cabinP->values.highTempStayTimer)
        cabinP->values.highTempStayTimer--;


    if (cabinP->values.heaterBTimer)
        cabinP->values.heaterBTimer--;

    if (cabinP->values.fanLevelStayTimer < 0xFF)
        cabinP->values.fanLevelStayTimer++;

    if (cabinP->values.defrost.delayTimer && (cabinP->states.Work == _Work_Cool))
      cabinP->values.defrost.delayTimer--;
    
    if (cabinP->values.afterDefrostTimer)
      cabinP->values.afterDefrostTimer--;
    
    cabinVacuumBox1MinTask(consts, params);
    cabinVioLed1MinTask(consts, params);

}

static PROCESS_HANDLER_PROTO(cabinProcSuper1HourTask)
{
    const tsCabinProcConsts *consts = process->constants;
    tsCabinProcParams *params = process->parameters;
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    
#if defined (AI_COOL_V1)    
    CabinAIT1hour(consts, params);
#endif   
    
  
}
/** @} */

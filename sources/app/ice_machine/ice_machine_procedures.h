/** @file       ice_machine_procedures.h
 *  @brief      ice_machine_procedures application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       9/20/2024
 */
#ifndef ICE_MACHINE_PROCEDURES_H
#define ICE_MACHINE_PROCEDURES_H

#define _SEC_T20ms(_sec) ((_sec * 50))
#define _MIN_T20ms(_min) ((_min * 3000))
#define _SEC_T100ms(_sec) ((_sec * 10))
#define _SEC_T1sec(_sec) (_sec)
#define _MIN_T1sec(_min) (_min * 60)
#define _MIN_T1min(_min) (_min)
#define _CaseTactSWErrorTime _SEC_T1sec(40)   //we refresh timer when motor stop, that time defined by operation process when first pouring process takes 17 second, if somehow door open and process interrupted then nobox state will start, in the mean time motor cannot stop, that's why we need to select a parameter it should be expired the longest period 17 + 17 = 34 < 40 that i choosed 40 for BONA project.
#define _CaseTactSWErrorReTryTime _MIN_T1min(180)
#define _CheckBoxTime _MIN_T1min(20)
#define _WaterVacumTime _SEC_T100ms(4.5)
#define _CaseTactSW_ON  _SwitchValue_OPEN
#define _CaseTactSW_OFF  _SwitchValue_CLOSE
#define _BoxTactSW_ThereIs _SwitchValue_CLOSE
#define _BoxTactSW_ThereIsNot _SwitchValue_OPEN
//#define _CheckBoxTime _MIN_T1min(20)
#define _PureCount 2
#define _waterVentilHeaterON  refrigerator_F.WaterSource.parameters.waterVentilHeater.OnTimeA + refrigerator_F.WaterSource.parameters.waterVentilHeater.OffTimeA


#include "ice_machine_configs.h"
#include "ice_machine_ram.h"
#include "ice_machine.h"
   
void IceMachineTask(const tsIcemachineProcConsts * consts,  tsIcemachineProcParams* params);
void IceMachineType_DEFAULT(const tsIcemachineProcConsts* consts, tsIcemachineProcParams* params);
void IceMachineType_Commercial_DEFAULT(stIceMachine_Flash const *iceMachineC, stIceMachine_Ram *iceMachineP, const tsIcemachineProcConsts* consts, tsIcemachineProcParams* params);      //APW
void QuickIceCheck(const tsIcemachineProcConsts* consts,  tsIcemachineProcParams* params, bool quickIceCheck);
void IceMachineT20ms(stIceMachine_Ram *iceMachineP);
void IceMachineT100ms(stIceMachine_Ram *iceMachineP);
void IceMachineT1sec(stIceMachine_Ram *iceMachineP);
void IceMachineT1min(stIceMachine_Ram *iceMachineP);
void RotateToSTART(stIceMachine_Ram *iceMachineP);
void RotateStop(stIceMachine_Ram *iceMachineP);
void RotateToEND(stIceMachine_Ram *iceMachineP);
void TactSW_Tester(stIceMachine_Ram *iceMachineP);
void CalibrateWater(stIceMachine_Ram *iceMachineP );

void IceMachineComponentsTest(const tsIcemachineProcConsts* consts,  tsIcemachineProcParams* params);
void IceMachineCloseComponents(stIceMachine_Ram *iceMachineP);
void IceMachineInitAfterHotStart(stIceMachine_Ram *iceMachineP);
void IceMachineInitAfterService(stIceMachine_Ram *iceMachineP);
void IceMachineE2Init(const stIceMachine_Flash *iceMachineC, stIceMachine_Ram *iceMachineP);
void IceMachineE2AfterHotStart(stIceMachine_Ram *iceMachineP);
void IceMachineE2AfterService(stIceMachine_Ram *iceMachineP);
void IceMachineE2ToRam(stIceMachine_Ram *iceMachineP);
void IceMachineClearDisplaySetting20MsTask(stIceMachine_Ram *iceMachineP);
void IceMachineE2ClearDisplaySetting(stIceMachine_Ram *iceMachineP);

#endif







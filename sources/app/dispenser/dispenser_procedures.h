/** @file       dispenser_procedures.h
 *  @brief      dispenser_procedures header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       9/20/2024
 */
#ifndef DISPENSER_PROCEDURES_H
#define DISPENSER_PROCEDURES_H

#include "dispenser_ram.h"
#include "dispenser_configs.h"
#include "dispenser.h"

#define _WATER_PUMP_TIME 60 // 60 * 1sec = 60 sec 
#define _WATER_VACUM_TIME 60 // 60 * 1ms = 60 sec  
#define _ICE_TIME 17 // 60 * 1sec = 60sec  
#define _ICE_WAIT_TIME 5 // 60 * 1sec = 60sec
#define _ICE_WAIT_TIME2 1800
#define _ICE_TIME_CONTINUE 240//90
#define _FACTOR_CONTINUE 25
#define _WD_LED_BLINK_TIME 25 // 25*20ms = 500ms
/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_DISPENSER_PROCEDURES_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

void DispenserTask(const tsDispenserProcConsts * consts, tsDispenserProcParams* params);
void DispenserLightTask(const tsDispenserProcConsts * consts, tsDispenserProcParams* params);

void DispenserOnOff(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP);
void DispenserCheckState(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP);
void DispenserNextState(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP, bool cmdReceived, unsigned char *displayEvent );

void DispenserType_WaterPump(const tsDispenserProcConsts * consts, tsDispenserProcParams* params);
void DispenserType_WaterPumpAndVacum(const tsDispenserProcConsts * consts, tsDispenserProcParams* params);
void DispenserType_IceCube(const tsDispenserProcConsts * consts, tsDispenserProcParams* params);
void DispenserType_IceCubeAndCrash(const tsDispenserProcConsts * consts, tsDispenserProcParams* params);
void DispenserType_IceCubeAndCrash_CommercialTest(const tsDispenserProcConsts * consts, tsDispenserProcParams* params);

void DispenserT20ms(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP);
void DispenserT1sec( stDispenser_Ram *dispenserP);

unsigned char DispenserGetLogData(  stDispenser_Ram *dispenserP,  unsigned char* dataArrayPtr );

void DispenserCloseComponents( stDispenser_Ram* dispenserP );
void DispenserComponentsTest(const tsDispenserProcConsts * consts, tsDispenserProcParams* params);
bool isDispenserComponentsTestDone(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP);

void DispenserInitAfterHotStart( stDispenser_Ram* dispenserP );
void DispenserInitAfterService( stDispenser_Ram* dispenserP );
void dispenserClearDisplaySetting20MsTask( stDispenser_Ram* dispenserP );

void DispenserE2Init(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP);
void DispenserE2AfterHotStart( stDispenser_Ram* dispenserP );
void DispenserE2AfterService( stDispenser_Ram* dispenserP );
void DispenserE2ToRam( stDispenser_Ram* dispenserP );
void DispenserE2ClearDisplaySetting( stDispenser_Ram* dispenserP );

#undef INTERFACE // Should not let this roam free


#endif
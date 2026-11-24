/** @file       ice_source_procedures.h
 *  @brief      ICE_SOURCE_PROCEDURES application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       9/21/2024
 */
#ifndef FILE_ICE_SOURCE_PROCEDURES_H
#define FILE_ICE_SOURCE_PROCEDURES_H

#include "ice_source_configs.h"
#include "ice_source_ram.h"
#include "ice_source.h"


#ifndef FILE_ICE_SOURCE_PROCEDURES_H
#define INTERFACE extern
#else
#define INTERFACE
#endif
void IceSourceType_IceCube(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP);
void IceSourceType_IceCubeAndCrash(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP);
void IceSourceToShutter(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP);
void IceSourceToSelector(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP);
void IceSourceToPusher(stIceSource_Ram *iceSourceP);
void IceSourceShutterState(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP, enShutterStates desiredState);
void IceSourceSelectorState(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP, enSelectorStates desiredState);
void IceSourcePusherState(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP, enPusherStates desiredState);
void IceSourceT20ms(stIceSource_Ram *iceSourceP);
void IceSourceTask(const tsIcesourceProcConsts* consts, tsIcesourceProcParams * params);
void IceSourceCloseComponents(stIceSource_Ram *iceSourceP);
bool isIceSourceComponentsTestDone(const stIceSource_Flash  *iceSourceC, stIceSource_Ram *iceSourceP);
void IceSourceComponentsTest(const tsIcesourceProcConsts* consts, tsIcesourceProcParams * params);
unsigned char IceSourceGetLogData(stIceSource_Ram *iceSourceP, unsigned char *dataArrayPtr);
#undef INTERFACE // Should not let this roam free
#endif // FILE_ICE_SOURCE_H
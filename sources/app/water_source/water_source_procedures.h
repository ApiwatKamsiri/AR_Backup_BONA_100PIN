/** @file       water_source_procedures.h
 *  @brief      water_source_procedures application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       9/20/2024
 */
#ifndef WATER_SOURCE_PROCEDURES_H
#define WATER_SOURCE_PROCEDURES_H


#include "app/water_source/water_source_configs.h"
#include "app/water_source/water_source_ram.h"
#include "app/water_source/water_source.h"
void WaterSourceTask(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params);

void WaterSource_T20ms(stWaterSource_Ram* waterSourceP);
void WaterSource_T1Sec(stWaterSource_Ram* waterSourceP);
void WaterSource_T1min(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params);
void WaterSource_T1hour(stWaterSource_Ram* waterSourceP);
void WaterSourceCloseComponents( stWaterSource_Ram* waterSource_RPtr );


void WaterSourceComponentsTest(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params);
//bool isWaterSourceComponentsTestDone(const stWaterSource_Flash* waterSourceC, stWaterSource_Ram* waterSourceP, unsigned int testTimer);

void WaterSourceInitAfterHotStart(stWaterSource_Ram* waterSourceP);
void WaterSourceInitAfterService(stWaterSource_Ram* waterSourceP);
void WaterSourceE2ToRam(stWaterSource_Ram* waterSourceP);
void WaterSourceE2Init(const stWaterSource_Flash* waterSourceC, stWaterSource_Ram* waterSourceP);
void WaterSourceE2AfterHotStart(stWaterSource_Ram* waterSourceP);
void WaterSourceE2AfterService(stWaterSource_Ram* waterSourceP);

void WaterSupplyType_INTERNAL_IceMachine( stWaterSource_Ram* waterSourceP );
void WaterSupplyType_INTERNAL_Dispenser( stWaterSource_Ram* waterSourceP );
void WaterSupplyType_INTERNAL_IceMachine_Dispenser( stWaterSource_Ram* waterSourceP );
void WaterSupplyType_EXTERNAL_IceMachine( stWaterSource_Ram* waterSourceP );
void WaterSupplyType_EXTERNAL_Dispenser( stWaterSource_Ram* waterSourceP );
void WaterSupplyType_EXTERNAL_IceMachine_Dispenser( stWaterSource_Ram* waterSourceP );
void WaterSupply_WaterVentilHeater(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params);
void WaterSupply_WaterVentilHeater_CommercialHeaterTest(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params);       //APW
void WaterSupply_NoWaterCLR(const stWaterSource_Flash * waterSourceC ,stWaterSource_Ram* waterSourceP);



#endif // WATER_SOURCE_PROCEDURES_H

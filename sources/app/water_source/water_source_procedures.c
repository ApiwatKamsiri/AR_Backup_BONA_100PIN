/** @file       water_source_procedures.c
 *  @brief      water_source_procedures application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       9/20/2024
 */
#define WATER_SOURCE_PROCEDURES_C
#include "water_source_procedures.h"
#include "app/libs/component_test.h"

#define _NoWaterCLRTime		300

void WaterSourceTask(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params)
{
  stWaterSource_Ram * waterSourceP = params->watersource;
  const stWaterSource_Flash * waterSourceC = consts->watersource;
  
    switch( waterSourceC->type.supply )
    {
      case _WaterSupplyType_INTERNAL_IceMachine:
        WaterSupplyType_INTERNAL_IceMachine(waterSourceP);
        break;
      case _WaterSupplyType_INTERNAL_Dispenser:
        WaterSupplyType_INTERNAL_Dispenser(waterSourceP); 
        break;
      case _WaterSupplyType_INTERNAL_IceMachine_Dispenser:
        WaterSupplyType_INTERNAL_IceMachine_Dispenser(waterSourceP); 
          break;    
      case _WaterSupplyType_EXTERNAL_IceMachine:
        WaterSupplyType_EXTERNAL_IceMachine(waterSourceP);
        break;        
      case _WaterSupplyType_EXTERNAL_Dispenser:
        WaterSupplyType_EXTERNAL_Dispenser(waterSourceP);
        break;      
      case _WaterSupplyType_EXTERNAL_IceMachine_Dispenser:
        WaterSupplyType_EXTERNAL_IceMachine_Dispenser(waterSourceP);         
        break;
      case _WaterSupplyType_NONE:
      default:
        break;
    }
    
    switch( waterSourceC->type.filter )
    {
      case _WaterFilterType_DEFAULT:
        if( waterSourceP->values.errFilterEnabled &&
           ( !waterSourceP->values.filterTimer ) )
        {
          waterSourceP->values.errFilterTime = true;     
        }
        else
        {
          waterSourceP->values.errFilterTime = false;       
        }
        break;
      case _WaterFilterType_NONE:
      default:
        waterSourceP->values.filterTimer = 0;
        waterSourceP->values.errFilterTime = false; 
        break;
    }
     
    //WaterSupply_WaterVentilHeater(consts, params);
    /*** APW : Add heater operation in CT ***/
    if(refrigerator_R.ui.states.commercial.status != _UI_Commercial_None)
      WaterSupply_WaterVentilHeater_CommercialHeaterTest(consts, params);
    else
      WaterSupply_WaterVentilHeater(consts, params);
    /*** APW : Add heater operation in CT ***/
    
    WaterSupply_NoWaterCLR(waterSourceC, waterSourceP);  
    params->rf->Dispenser.values.waterApplied = waterSourceP->values.dispenserApplied;
    params->rf->IceMachine.values.waterApplied = waterSourceP->values.iceMachineApplied;  

}

void WaterSource_T20ms(stWaterSource_Ram* waterSourceP)
{
    if( waterSourceP->values.startTimer < 0xFF ) 
      waterSourceP->values.startTimer++;
}
void WaterSource_T1Sec(stWaterSource_Ram* waterSourceP)
{
   if( waterSourceP->values.noWaterClrTimer) 
    waterSourceP->values.noWaterClrTimer--;	

   if( waterSourceP->values.heaterTimer ) 
    waterSourceP->values.heaterTimer--;

}
void WaterSource_T1min(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params)
{
    stWaterSource_Ram * waterSourceP = params->watersource;
    const stWaterSource_Flash * waterSourceC = consts->watersource;

    if(waterSourceP->values.ProhibitPumpWhenIceOnFirstOnTimer < waterSourceC->parameters.waterVentilHeater.ProhibitPumpWhenIceOnFirstOnTime)
      waterSourceP->values.ProhibitPumpWhenIceOnFirstOnTimer++;

    if(waterSourceP->values.ProhibitPumpAfterDoorClearEmptyTimer < waterSourceC->parameters.waterVentilHeater.ProhibitPumpAfterDoorClearEmptyTime)
      waterSourceP->values.ProhibitPumpAfterDoorClearEmptyTimer++;
    
    if(params->rf->IceMachine.sets.iceOFF)
    {
      waterSourceP->values.iceOnFirstOnTimer = 0;
      waterSourceP->values.ProhibitPumpWhenIceOnFirstOnTimer = 0;
    }
    else if(waterSourceP->values.iceOnFirstOnTimer < waterSourceC->parameters.waterVentilHeater.IceOnFirstOnTime)
    {
      waterSourceP->values.iceOnFirstOnTimer++;
    }
}
void WaterSource_T1hour(stWaterSource_Ram* waterSourceP)
{
    if( waterSourceP->values.filterTimer ) 
      waterSourceP->values.filterTimer--;
    
    waterSourceP->E2.filterTimer = waterSourceP->values.filterTimer;
}
void WaterSourceCloseComponents(stWaterSource_Ram* waterSourceP)
{
    waterSourceP->components.mainPump.value = _PumpValue_OFF;
    waterSourceP->components.iceMachinePump.value = _PumpValue_OFF;   
    waterSourceP->components.dispenserPump.value = _PumpValue_OFF;   
    waterSourceP->components.mainValve.value = _ValveValue_CLOSE;      
    waterSourceP->components.iceMachineValve.value = _ValveValue_CLOSE;   
    waterSourceP->components.dispenserValve.value = _ValveValue_CLOSE;
    /*** APW : For Commercial and Defrost test mode don't need to control heater here ***/
    if(refrigerator_R.ui.states.commercial.status != _UI_Commercial_DefrostTest)
      waterSourceP->components.waterVentilHeater.value = _HeaterValue_OFF;
    /*** APW : For Commercial and Defrost test mode don't need to control heater here ***/
}

void WaterSourceInitAfterHotStart(stWaterSource_Ram* waterSourceP)
{
    WaterSourceE2AfterHotStart( waterSourceP );
    WaterSourceE2ToRam( waterSourceP );  
}
void WaterSourceInitAfterService(stWaterSource_Ram* waterSourceP)
{
    WaterSourceE2AfterService( waterSourceP );
    WaterSourceE2ToRam( waterSourceP ); 
}
void WaterSourceE2ToRam(stWaterSource_Ram* waterSourceP)
{
    waterSourceP->values.filterTimer = waterSourceP->E2.filterTimer;
    waterSourceP->values.errFilterEnabled = waterSourceP->E2.errFilterEnabled;    
}
void WaterSourceE2Init(const stWaterSource_Flash* waterSourceC, stWaterSource_Ram* waterSourceP)
{
    waterSourceP->E2.filterTimer = 0;//_FILTER_TIME;
    waterSourceP->E2.errFilterEnabled = false; // Default should be disabled.  waterSourceC->type.filter ? true : false; 
  
}
void WaterSourceE2AfterHotStart(stWaterSource_Ram* waterSourceP)
{
  
}
void WaterSourceE2AfterService(stWaterSource_Ram* waterSourceP)
{
  
}

void WaterSourceComponentsTest(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params)
{
    const stWaterSource_Flash * waterSourceC = consts->watersource;
    stWaterSource_Ram * waterSourceP = params->watersource;
    tsWaterSourceProcDevIos *devs = &params->dev;
    const stWaterSourceComponents_Flash *compC = &consts->watersource->components;
    stWaterSourceComponents_Ram *compP = &params->watersource->components;
    
    uint32_t tt = params->rf->workMode.values.prodTest.componentTestTimer100Ms;
    
    bool done = true;
    
    if (waterSourceP->values.iceMachineDesired ||
        waterSourceP->values.dispenserDesired)
    {
      WaterSourceTask(consts, params);
      
      waterSourceP->components.waterVentilHeater.value = _HeaterValue_OFF;
      if (waterSourceC->type.supply)
      {
        done &= compTest(devs->waterVentilHeater, &compP->waterVentilHeater, &compC->waterVentilHeater.test, tt, _HeaterValue_ON);
      }
    }
    else
    {
      WaterSourceCloseComponents(waterSourceP);

      if (waterSourceC->type.supply)
      {
        //done &= compTest(devs->mainPump, &compP->mainPump, &compC->mainPump.test.pump, tt, _PumpValue_PUMP); maybe cause unwanted intake issue ,removed
        done &= compTest(devs->mainPump, &compP->mainPump, &compC->mainPump.test.vacuum, tt, _PumpValue_VACUM);
        done &= compTest(devs->dispenserPump, &compP->dispenserPump, &compC->dispenserPump.test.vacuum, tt, _PumpValue_VACUM);
        done &= compTest(devs->iceMachinePump, &compP->iceMachinePump, &compC->iceMachinePump.test.vacuum, tt, _PumpValue_VACUM);
        done &= compTest(devs->mainValve, &compP->mainValve, &compC->mainValve.test, tt, _PumpValue_PUMP);
        
        if(compTestDone(devs->mainPump, &compC->mainPump.test.pump, tt) &&
           compTestDone(devs->dispenserPump, &compC->dispenserPump.test.pump, tt) &&
             compTestDone(devs->iceMachinePump, &compC->iceMachinePump.test.pump, tt) &&
              compTestDone(devs->mainValve, &compC->mainValve.test, tt))
        {
          done &= compTest(devs->iceMachineValve, &compP->iceMachineValve, &compC->iceMachineValve.test, tt, _ValveValue_OPEN);
          done &= compTest(devs->dispenserValve, &compP->dispenserValve, &compC->dispenserValve.test, tt, _ValveValue_OPEN);
        }
        
        done &= compTest(devs->waterVentilHeater, &compP->waterVentilHeater, &compC->waterVentilHeater.test, tt, _HeaterValue_ON);
      }
    }
    
    waterSourceP->values.componentsTestDone = done;
    waterSourceP->values.waterIceTankSensorValueInC = waterSourceP->components.waterIceTankEmptySensor.valueInCExt; //Temporary solve tank empty show on start
    waterSourceP->values.waterDisTankSensorValueInC = waterSourceP->components.waterDisTankEmptySensor.valueInCExt; //Temporary solve tank empty show on start
}


 void WaterSupplyType_INTERNAL_IceMachine(stWaterSource_Ram* waterSourceP)
{
  
    if( waterSourceP->values.iceMachineDesired != waterSourceP->values.iceMachineDesiredPrv )
      waterSourceP->values.startTimer = 0;
    waterSourceP->values.dispenserDesiredPrv = waterSourceP->values.dispenserDesired;
    waterSourceP->values.iceMachineDesiredPrv = waterSourceP->values.iceMachineDesired;  
    
    waterSourceP->components.mainPump.value = _PumpValue_OFF;
    waterSourceP->components.iceMachinePump.value = _PumpValue_OFF;
    waterSourceP->components.dispenserPump.value = _PumpValue_OFF;
    waterSourceP->components.mainValve.value = _ValveValue_CLOSE;
    waterSourceP->components.iceMachineValve.value = _ValveValue_CLOSE;
    waterSourceP->components.dispenserValve.value = _ValveValue_CLOSE;
    
    waterSourceP->values.iceMachineApplied = _WaterSourceValue_OFF;
    waterSourceP->values.dispenserApplied = _WaterSourceValue_OFF;
    
    switch( waterSourceP->values.iceMachineDesired )
    {
    case _WaterSourceValue_PUMP:      
      if( waterSourceP->values.startTimer > 25 )
      {
        waterSourceP->components.iceMachineValve.value = _ValveValue_OPEN;    
        waterSourceP->components.iceMachinePump.value = _PumpValue_PUMP;
      }
      if( waterSourceP->values.startTimer > 50 )
        waterSourceP->components.mainValve.value = _ValveValue_OPEN;
      if( waterSourceP->values.startTimer > 75 )
        waterSourceP->components.mainPump.value = _PumpValue_PUMP;
      if( waterSourceP->values.startTimer > 75 )
        waterSourceP->values.iceMachineApplied = _WaterSourceValue_PUMP;
      break;
    case _WaterSourceValue_VACUM:
      if( waterSourceP->values.startTimer > 25 )
      {
        waterSourceP->components.iceMachineValve.value = _ValveValue_OPEN; 
        waterSourceP->components.iceMachinePump.value = _PumpValue_VACUM;
      }
      if( waterSourceP->values.startTimer > 50 )
        waterSourceP->components.mainValve.value = _ValveValue_OPEN;
      if( waterSourceP->values.startTimer > 75 )
        waterSourceP->components.mainPump.value = _PumpValue_VACUM;
      if( waterSourceP->values.startTimer > 75 )
        waterSourceP->values.iceMachineApplied = _WaterSourceValue_VACUM;
      break;      
    case _WaterSourceValue_OFF:     
    default:
      break;
    } 
}
void WaterSupplyType_INTERNAL_Dispenser(stWaterSource_Ram* waterSourceP)
{
    
    if(  waterSourceP->values.dispenserDesired !=  waterSourceP->values.dispenserDesiredPrv )
      waterSourceP->values.startTimer = 0;
    
    waterSourceP->values.dispenserDesiredPrv =  waterSourceP->values.dispenserDesired;
    waterSourceP->values.iceMachineDesiredPrv =  waterSourceP->values.iceMachineDesired;  
    
    waterSourceP->components.mainPump.value = _PumpValue_OFF;
    waterSourceP->components.iceMachinePump.value = _PumpValue_OFF;
    waterSourceP->components.dispenserPump.value = _PumpValue_OFF;
    waterSourceP->components.mainValve.value = _ValveValue_CLOSE;
    waterSourceP->components.iceMachineValve.value = _ValveValue_CLOSE;
    waterSourceP->components.dispenserValve.value = _ValveValue_CLOSE;
    
    waterSourceP->values.iceMachineApplied = _WaterSourceValue_OFF;
    waterSourceP->values.dispenserApplied = _WaterSourceValue_OFF;  
    
    switch(  waterSourceP->values.dispenserDesired )
    {
    case _WaterSourceValue_PUMP:    
      if(  waterSourceP->values.startTimer > 25 )
      {
        waterSourceP->components.dispenserValve.value = _ValveValue_OPEN;  
        waterSourceP->components.dispenserPump.value = _PumpValue_PUMP;
      }
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->components.mainValve.value = _ValveValue_OPEN;    
      if(  waterSourceP->values.startTimer > 75 )
        waterSourceP->components.mainPump.value = _PumpValue_PUMP;    
      if(  waterSourceP->values.startTimer > 75 )
        waterSourceP->values.dispenserApplied = _WaterSourceValue_PUMP;
      break;
    case _WaterSourceValue_VACUM:
      if(  waterSourceP->values.startTimer > 25 )
      {
        waterSourceP->components.dispenserValve.value = _ValveValue_OPEN;
        waterSourceP->components.dispenserPump.value = _PumpValue_VACUM;
      }
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->components.mainValve.value = _ValveValue_OPEN;    
      if(  waterSourceP->values.startTimer > 75 )
        waterSourceP->components.mainPump.value = _PumpValue_VACUM;    
      if(  waterSourceP->values.startTimer > 75 )
        waterSourceP->values.dispenserApplied = _WaterSourceValue_VACUM; 
      break;
    case _WaterSourceValue_OFF:
    default: 
      break;
    }
}
void WaterSupplyType_INTERNAL_IceMachine_Dispenser(stWaterSource_Ram* waterSourceP)
{
    if( (  waterSourceP->values.dispenserDesired !=  waterSourceP->values.dispenserDesiredPrv ) ||
       (  waterSourceP->values.iceMachineDesired !=  waterSourceP->values.iceMachineDesiredPrv ) )
      waterSourceP->values.startTimer = 0;
    
    waterSourceP->values.dispenserDesiredPrv =  waterSourceP->values.dispenserDesired;
    waterSourceP->values.iceMachineDesiredPrv =  waterSourceP->values.iceMachineDesired;    
    
    waterSourceP->components.mainPump.value = _PumpValue_OFF;
    waterSourceP->components.iceMachinePump.value = _PumpValue_OFF;
    waterSourceP->components.dispenserPump.value = _PumpValue_OFF;
    waterSourceP->components.mainValve.value = _ValveValue_CLOSE;
    waterSourceP->components.iceMachineValve.value = _ValveValue_CLOSE;
    waterSourceP->components.dispenserValve.value = _ValveValue_CLOSE;
    
    waterSourceP->values.iceMachineApplied = _WaterSourceValue_OFF;
    waterSourceP->values.dispenserApplied = _WaterSourceValue_OFF;  
    
    switch(  waterSourceP->values.dispenserDesired ){
    case _WaterSourceValue_PUMP:    
      if(  waterSourceP->values.startTimer > 25 )
      {
        waterSourceP->components.dispenserValve.value = _ValveValue_OPEN;  
        waterSourceP->components.dispenserPump.value = _PumpValue_PUMP;
      }
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->components.mainValve.value = _ValveValue_OPEN;    
      if(  waterSourceP->values.startTimer > 75 )
        waterSourceP->components.mainPump.value = _PumpValue_PUMP;    
      if(  waterSourceP->values.startTimer > 75 )
      {
        waterSourceP->values.dispenserApplied = _WaterSourceValue_PUMP;
        waterSourceP->values.waterDisTankSensorValueInC = waterSourceP->components.waterDisTankEmptySensor.valueInCExt;
        waterSourceP->values.SkipEmptyCheck = false;
      }
      break;
    case _WaterSourceValue_VACUM:
      if(  waterSourceP->values.startTimer > 25 )
      {
        waterSourceP->components.dispenserValve.value = _ValveValue_OPEN;  
        waterSourceP->components.dispenserPump.value = _PumpValue_VACUM;
      }
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->components.mainValve.value = _ValveValue_OPEN;    
      if(  waterSourceP->values.startTimer > 75 )
        waterSourceP->components.mainPump.value = _PumpValue_VACUM;    
      if(  waterSourceP->values.startTimer > 75 )
        waterSourceP->values.dispenserApplied = _WaterSourceValue_VACUM; 
      break;
    case _WaterSourceValue_OFF:
    default:    
      switch(  waterSourceP->values.iceMachineDesired ){
      case _WaterSourceValue_PUMP:      
        if(  waterSourceP->values.startTimer > 25 )
        {
          waterSourceP->components.iceMachineValve.value = _ValveValue_OPEN;
          waterSourceP->components.iceMachinePump.value = _PumpValue_PUMP;
          waterSourceP->values.iceMachineApplied = _WaterSourceValue_PUMP;
        }
        if(  waterSourceP->values.startTimer > 50 )
          waterSourceP->components.mainValve.value = _ValveValue_OPEN;
        if(  waterSourceP->values.startTimer > 75 )
          waterSourceP->components.mainPump.value = _PumpValue_PUMP;
        if(  waterSourceP->values.startTimer > 75 )
        {
          
          waterSourceP->values.waterIceTankSensorValueInC = waterSourceP->components.waterIceTankEmptySensor.valueInCExt;
          waterSourceP->values.SkipEmptyCheck = false;
        }
        break;
      case _WaterSourceValue_VACUM:
        if(  waterSourceP->values.startTimer > 25 )
        {
          waterSourceP->components.iceMachineValve.value = _ValveValue_OPEN;   
          waterSourceP->components.iceMachinePump.value = _PumpValue_VACUM;
          waterSourceP->values.iceMachineApplied = _WaterSourceValue_VACUM;
        }
        if(  waterSourceP->values.startTimer > 50 )
          waterSourceP->components.mainValve.value = _ValveValue_OPEN;
        if(  waterSourceP->values.startTimer > 75 )
          waterSourceP->components.mainPump.value = _PumpValue_VACUM;     
      case _WaterSourceValue_OFF:     
      default:
        break;
      }    
      break;    
    }
    
    /* APW : Add function Empty Clear by open and Clear flag first ice for fix Ice clogged pipes */
    if(waterSourceP->values.waterTankEmpty)
    {
      //Status door open
      if(waterSourceP->values.EmptyDoorOpenClear && waterSourceP->values.EmptyDoorCloseClear)
      {
        waterSourceP->values.waterTankEmpty = false;
        refrigerator_R.IceMachine.values.firstIce = true;
        waterSourceP->values.EmptyDoorOpenClear = false;
        waterSourceP->values.EmptyDoorCloseClear = false;
        waterSourceP->values.SkipEmptyCheck = true;
        waterSourceP->values.iceOnFirstOnTimer = 0;
        waterSourceP->values.ProhibitPumpAfterDoorClearEmptyTimer = 0;
        waterSourceP->values.ProhibitionWaterTankEmpty = true;
        refrigerator_R.IceMachine.sets.iceOFFToON = false;
      }
    }
    /* APW : Add function Empty Clear by open and Clear flag first ice for fix Ice clogged pipes */
    if(!waterSourceP->values.SkipEmptyCheck)
    {
      //Variable temporary
      bool iceEmpty = false;
      bool disEmpty = false;
      
      //For Ice making
      if(refrigerator_F.WaterSource.components.waterIceTankEmptySensor.type != _TempSensorType_NONE)
      {
        if(waterSourceP->values.waterIceTankSensorValueInC > refrigerator_F.WaterSource.parameters.waterIceTankEmptySensor.JudgmentZoneC && 
            waterSourceP->values.waterIceTankSensorValueInC < refrigerator_F.WaterSource.parameters.waterIceTankEmptySensor.JudgmentZoneD)        // 1.2V < X < 0.8V  Normally 1V no water
          iceEmpty = true;
        else if(waterSourceP->values.waterIceTankSensorValueInC > refrigerator_F.WaterSource.parameters.waterIceTankEmptySensor.JudgmentZoneA && 
                  waterSourceP->values.waterIceTankSensorValueInC < refrigerator_F.WaterSource.parameters.waterIceTankEmptySensor.JudgmentZoneB)  // 3.0V < X < 2.6V  Normally 2.8V water enough
          iceEmpty = false;
        else 
          iceEmpty = false;
      }
      
      //For Water dispensor
      if(refrigerator_F.WaterSource.components.waterDisTankEmptySensor.type != _TempSensorType_NONE)
      {
        if(waterSourceP->values.waterDisTankSensorValueInC > refrigerator_F.WaterSource.parameters.waterDisTankEmptySensor.JudgmentZoneC && 
            waterSourceP->values.waterDisTankSensorValueInC < refrigerator_F.WaterSource.parameters.waterDisTankEmptySensor.JudgmentZoneD)        // 1.2V < X < 0.8V  Normally 1V no water
          disEmpty = true;
        else if(waterSourceP->values.waterDisTankSensorValueInC > refrigerator_F.WaterSource.parameters.waterDisTankEmptySensor.JudgmentZoneA && 
                  waterSourceP->values.waterDisTankSensorValueInC < refrigerator_F.WaterSource.parameters.waterDisTankEmptySensor.JudgmentZoneB)  // 3.0V < X < 2.6V  Normally 2.8V water enough
          disEmpty = false;
        else 
          disEmpty = false;
      }
      waterSourceP->values.waterTankEmpty = iceEmpty || disEmpty;
      waterSourceP->values.SkipEmptyCheck = true;
    }
}
void WaterSupplyType_EXTERNAL_IceMachine(stWaterSource_Ram* waterSourceP)
{
    if(  waterSourceP->values.iceMachineDesired !=  waterSourceP->values.iceMachineDesiredPrv )
      waterSourceP->values.startTimer = 0;
    
    waterSourceP->values.dispenserDesiredPrv =  waterSourceP->values.dispenserDesired;
    waterSourceP->values.iceMachineDesiredPrv =  waterSourceP->values.iceMachineDesired;  
    
    waterSourceP->components.mainPump.value = _PumpValue_OFF;
    waterSourceP->components.iceMachinePump.value = _PumpValue_OFF;
    waterSourceP->components.dispenserPump.value = _PumpValue_OFF;
    waterSourceP->components.mainValve.value = _ValveValue_CLOSE;
    waterSourceP->components.iceMachineValve.value = _ValveValue_CLOSE;
    waterSourceP->components.dispenserValve.value = _ValveValue_CLOSE;
    
    waterSourceP->values.iceMachineApplied = _WaterSourceValue_OFF;
    waterSourceP->values.dispenserApplied = _WaterSourceValue_OFF;
    
    switch(  waterSourceP->values.iceMachineDesired ){
    case _WaterSourceValue_PUMP:      
      if(  waterSourceP->values.startTimer > 25 )
        waterSourceP->components.iceMachineValve.value = _ValveValue_OPEN;    
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->components.mainValve.value = _ValveValue_OPEN;
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->values.iceMachineApplied = _WaterSourceValue_PUMP;
      break;
    case _WaterSourceValue_VACUM:
      waterSourceP->values.iceMachineApplied = _WaterSourceValue_VACUM;
      break;      
    case _WaterSourceValue_OFF:     
    default:
      break;
    }
}
void WaterSupplyType_EXTERNAL_Dispenser(stWaterSource_Ram* waterSourceP)
{
    if(  waterSourceP->values.dispenserDesired !=  waterSourceP->values.dispenserDesiredPrv )
      waterSourceP->values.startTimer = 0;
    
    waterSourceP->values.dispenserDesiredPrv =  waterSourceP->values.dispenserDesired;
    waterSourceP->values.iceMachineDesiredPrv =  waterSourceP->values.iceMachineDesired;  
    
    waterSourceP->components.mainPump.value = _PumpValue_OFF;
    waterSourceP->components.iceMachinePump.value = _PumpValue_OFF;
    waterSourceP->components.dispenserPump.value = _PumpValue_OFF;
    waterSourceP->components.mainValve.value = _ValveValue_CLOSE;
    waterSourceP->components.iceMachineValve.value = _ValveValue_CLOSE;
    waterSourceP->components.dispenserValve.value = _ValveValue_CLOSE;
    
    waterSourceP->values.iceMachineApplied = _WaterSourceValue_OFF;
    waterSourceP->values.dispenserApplied = _WaterSourceValue_OFF;
    
    switch(  waterSourceP->values.dispenserDesired ){
    case _WaterSourceValue_PUMP:
      if(  waterSourceP->values.startTimer > 25 )
        waterSourceP->components.dispenserValve.value = _ValveValue_OPEN;    
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->components.mainValve.value = _ValveValue_OPEN;    
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->values.dispenserApplied = _WaterSourceValue_PUMP;
      break;
    case _WaterSourceValue_VACUM:
      waterSourceP->values.dispenserApplied = _WaterSourceValue_VACUM; 
      break;
    case _WaterSourceValue_OFF:
    default:
      break;    
    }
}
void WaterSupplyType_EXTERNAL_IceMachine_Dispenser(stWaterSource_Ram* waterSourceP)
{
    if( (  waterSourceP->values.dispenserDesired !=  waterSourceP->values.dispenserDesiredPrv ) ||
       (  waterSourceP->values.iceMachineDesired !=  waterSourceP->values.iceMachineDesiredPrv ) )
      waterSourceP->values.startTimer = 0;
    
    waterSourceP->values.dispenserDesiredPrv =  waterSourceP->values.dispenserDesired;
    waterSourceP->values.iceMachineDesiredPrv =  waterSourceP->values.iceMachineDesired;     
    
    waterSourceP->components.mainPump.value = _PumpValue_OFF;
    waterSourceP->components.iceMachinePump.value = _PumpValue_OFF;
    waterSourceP->components.dispenserPump.value = _PumpValue_OFF;
    waterSourceP->components.mainValve.value = _ValveValue_CLOSE;
    waterSourceP->components.iceMachineValve.value = _ValveValue_CLOSE;
    waterSourceP->components.dispenserValve.value = _ValveValue_CLOSE;
    
    waterSourceP->values.iceMachineApplied = _WaterSourceValue_OFF;
    waterSourceP->values.dispenserApplied = _WaterSourceValue_OFF;
    
    switch(  waterSourceP->values.dispenserDesired ){
    case _WaterSourceValue_PUMP:    
      if(  waterSourceP->values.startTimer > 25 )
        waterSourceP->components.dispenserValve.value = _ValveValue_OPEN;    
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->components.mainValve.value = _ValveValue_OPEN;      
      if(  waterSourceP->values.startTimer > 50 )
        waterSourceP->values.dispenserApplied = _WaterSourceValue_PUMP;
      break;
    case _WaterSourceValue_VACUM:
      waterSourceP->values.dispenserApplied = _WaterSourceValue_VACUM; 
      break;
    case _WaterSourceValue_OFF:
    default:    
      switch(  waterSourceP->values.iceMachineDesired ){
      case _WaterSourceValue_PUMP:      
        if(  waterSourceP->values.startTimer > 25 )
          waterSourceP->components.iceMachineValve.value = _ValveValue_OPEN;    
        if(  waterSourceP->values.startTimer > 50 )
          waterSourceP->components.mainValve.value = _ValveValue_OPEN;
        if(  waterSourceP->values.startTimer > 50 )
          waterSourceP->values.iceMachineApplied = _WaterSourceValue_PUMP;
        break;
      case _WaterSourceValue_VACUM:
        waterSourceP->values.iceMachineApplied = _WaterSourceValue_VACUM;
        break;      
      case _WaterSourceValue_OFF:   
      default:
        break;
      }    
      break;    
    }
}

void WaterSupply_WaterVentilHeater(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params)
{
  stWaterSource_Ram * waterSourceP = params->watersource;
  const stWaterSource_Flash * waterSourceC = consts->watersource;
  
  unsigned short OnTime = 0; 
  unsigned short OffTime = 0;
  
    if( waterSourceC->type.supply &&
       waterSourceC->components.waterVentilHeater.type )
    { 
      if( params->rf->IceMachine.sets.iceOFF)
      { 
        if(params->rf->Cabin[1].states.Work == _Work_Defrost)
        {
          OnTime = waterSourceC->parameters.waterVentilHeater.OnTimeA[params->rf->Cabin->values.ambientType]; 
          OffTime = waterSourceC->parameters.waterVentilHeater.OffTimeA[params->rf->Cabin->values.ambientType];
        }
        else
        {
          OnTime = waterSourceC->parameters.waterVentilHeater.OnTimeB; 
          OffTime = waterSourceC->parameters.waterVentilHeater.OffTimeB;  
        }
      } 
//      else if(params->rf->IceMachine.values.boxFullActive)   // Circumvention olmamasi için : Dolabin Buzmakinesi Boxfullteyken total enerjisiyle, buz yaparken ki total enerjisi arasindaki fark  %2 enerjiyi geçmemelidir. Ondan yararlaniliyor. F. Müminoglu  
//      {
//        OnTime = waterSourceC->parameters.waterVentilHeater.OnTimeA; 
//        OffTime = waterSourceC->parameters.waterVentilHeater.OffTimeA / 2;
//      }
      else
      {
        OnTime = waterSourceC->parameters.waterVentilHeater.OnTimeA[params->rf->Cabin->values.ambientType]; 
        OffTime = waterSourceC->parameters.waterVentilHeater.OffTimeA[params->rf->Cabin->values.ambientType];
      }  
      
      if(waterSourceP->values.iceOnFirstOnTimer == 1) // Ani aç kapatta tepki vermesin diye 1 dakika beklenir.
        waterSourceP->values.heaterTimer = waterSourceC->parameters.waterVentilHeater.IceOnFirstOnTime;
      else if( (!waterSourceP->values.heaterTimer) || (params->rf->Cabin[1].states.Work == _Work_Defrost && !params->rf->Cabin[1].stateTimers.Defrost) )	
        waterSourceP->values.heaterTimer = OnTime + OffTime;
      
      if(waterSourceP->values.iceOnFirstOnTimer && waterSourceP->values.iceOnFirstOnTimer < waterSourceC->parameters.waterVentilHeater.IceOnFirstOnTime)
        waterSourceP->components.waterVentilHeater.value = _HeaterValue_ON;
      else if(waterSourceP->values.waterTankEmpty == true)
        waterSourceP->components.waterVentilHeater.value = _HeaterValue_OFF;
      else if( waterSourceP->values.heaterTimer > OffTime )
        waterSourceP->components.waterVentilHeater.value = _HeaterValue_ON;	  
      else
        waterSourceP->components.waterVentilHeater.value = _HeaterValue_OFF;
    }
    else
    {
      waterSourceP->components.waterVentilHeater.value = _HeaterValue_OFF;
    }
  
}
/*** APW : Add heater operation in CT ***/
void WaterSupply_WaterVentilHeater_CommercialHeaterTest(const tsWaterSourceProcConsts * consts, tsWaterSourceProcParams* params)
{
  stWaterSource_Ram * waterSourceP = params->watersource;
  const stWaterSource_Flash * waterSourceC = consts->watersource;

    if(refrigerator_R.ui.states.commercial.status == _UI_Commercial_Test)
        waterSourceP->components.waterVentilHeater.value = _HeaterValue_OFF;
      
    if(refrigerator_R.ui.states.commercial.status == _UI_Commercial_DefrostTest)
    {       
       if(waterSourceC->components.waterVentilHeater.commercialDefrostTestPriority == 0)
            waterSourceP->components.waterVentilHeater.value = _HeaterValue_OFF;
       else if(waterSourceC->components.waterVentilHeater.commercialDefrostTestPriority == 1 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_DefrostOnTest)
            waterSourceP->components.waterVentilHeater.value = _HeaterValue_ON;
       else if(waterSourceC->components.waterVentilHeater.commercialDefrostTestPriority == 2 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater1OnTest)
            waterSourceP->components.waterVentilHeater.value = _HeaterValue_ON;
       else if(waterSourceC->components.waterVentilHeater.commercialDefrostTestPriority == 3 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater2OnTest)
            waterSourceP->components.waterVentilHeater.value = _HeaterValue_ON;
       else if(waterSourceC->components.waterVentilHeater.commercialDefrostTestPriority == 4 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater3OnTest)
            waterSourceP->components.waterVentilHeater.value = _HeaterValue_ON;
       else if(waterSourceC->components.waterVentilHeater.commercialDefrostTestPriority == 5 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater4OnTest)
            waterSourceP->components.waterVentilHeater.value = _HeaterValue_ON;
       else if(waterSourceC->components.waterVentilHeater.commercialDefrostTestPriority == 6 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater5OnTest)
            waterSourceP->components.waterVentilHeater.value = _HeaterValue_ON;
       else
            waterSourceP->components.waterVentilHeater.value = _HeaterValue_OFF;
    }
}
/*** APW : Add heater operation in CT ***/
void WaterSupply_NoWaterCLR(const stWaterSource_Flash * waterSourceC ,stWaterSource_Ram* waterSourceP)
{
  if( waterSourceP->values.noWaterSet ){
    if( waterSourceP->components.waterCaseSwitch.value == _SwitchValue_OPEN ){    
      waterSourceP->values.noWaterClrTimer = _NoWaterCLRTime;   		   
      waterSourceP->values.waterCaseOpened	 = true;
    }	
    
    if( waterSourceP->components.waterCaseSwitch.value == _SwitchValue_CLOSE ){
      if( waterSourceP->values.waterCaseOpened ){
        if( !waterSourceP->values.noWaterClrTimer )
          waterSourceP->values.noWaterClr = true;
      }
    }
  }else{
    waterSourceP->values.noWaterClr = false;
    waterSourceP->values.waterCaseOpened = false;
  }
  
  waterSourceP->values.noWaterSet = false;

}
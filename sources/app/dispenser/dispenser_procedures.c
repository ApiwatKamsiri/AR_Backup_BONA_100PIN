/** @file       dispenser_procedures.c
 *  @brief      dispenser_procedures application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       9/20/2024
 */

#include "dispenser_procedures.h"
#include "../ui/command_events/cmd_check.h"

#define FILE_DISPENSER_PROCEDURES_C

void DispenserTask(const tsDispenserProcConsts * consts, tsDispenserProcParams* params)
{
    stDispenser_Ram *dispenserP = params->dispenser;
    const stDispenser_Flash *dispenserC = consts->dispenser;
    
    if( !dispenserP->values.tactTriggered && dispenserP->components.tactSwitch.value && !params->rf->Values.keyLockDspr &&
       dispenserP->sets.state != _DispenserState_IceCube && dispenserP->sets.state != _DispenserState_IceCrash && (!dispenserP->components.doorSwitch.value))
    {
      if(dispenserP->values.waterDesired == _WaterSourceValue_OFF)
      {
        dispenserP->values.dispenserTactFlag = 1;
      }      
      else
      {
        if(dispenserP->values.waterPumpTimer <= (_WATER_PUMP_TIME- 2))
          dispenserP->values.dispenserTactFlag = 0;
      }
      dispenserP->values.tactTriggered = true;
    }
    if(!dispenserP->components.tactSwitch.value)
      dispenserP->values.tactTriggered = false;
   
   DispenserCheckState(dispenserC, dispenserP);
    switch( consts->dispenser->waterType )
    {
    case _DispenserWaterType_WaterPump:
      DispenserType_WaterPump(consts, params);
      break;
    case _DispenserWaterType_WaterPumpAndVacum:
      DispenserType_WaterPumpAndVacum(consts, params);
      break;    
    case _DispenserWaterType_NONE:
      
    default:
      params->dispenser->values.waterDesired = _WaterSourceValue_OFF;
      break;
    }
    
    switch( consts->dispenser->iceType){
    case _DispenserIceType_IceCube:
      DispenserType_IceCube(consts, params);
      break;
    case _DispenserIceType_IceCubeAndCrash:
      //DispenserType_IceCubeAndCrash(consts, params);
      if(refrigerator_R.ui.states.commercial.status == _UI_Commercial_None)
        DispenserType_IceCubeAndCrash(consts, params);
      else
        DispenserType_IceCubeAndCrash_CommercialTest(consts, params);
      break;   
    case _DispenserIceType_NONE:
    default:
      params->dispenser->values.iceDesired = _IceSourceValue_OFF;  
      break;
    }  
    
    DispenserLightTask(consts,params); 
   
    params->rf->WaterSource.values.dispenserDesired = dispenserP->values.waterDesired;
    params->rf->IceSource.values.dispenserDesired = dispenserP->values.iceDesired;
}

void DispenserT20ms(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP)
{   
    if( dispenserP->values.lightEffectTimer < 0xFF )
      dispenserP->values.lightEffectTimer++;  
    
    if (dispenserP->values.dispenserProhibitTimer < 0xFF)
    {
      dispenserP->values.dispenserProhibitTimer++;
      
      if(dispenserP->values.dispenserProhibitTimer >= _WD_LED_BLINK_TIME * 2)
        dispenserP->values.dispenserProhibitTimer = 0;
      
      if(dispenserP->values.dispenserProhibitTimer < _WD_LED_BLINK_TIME)
        dispenserP->values.dispenserProhibitBlinkFlag = true;
      else
        dispenserP->values.dispenserProhibitBlinkFlag = false;
    }
}

void DispenserT1sec(stDispenser_Ram *dispenserP )
{
    if( dispenserP->values.waterApplied == _WaterSourceValue_PUMP ){ 
      if( dispenserP->values.waterPumpTimer ) 
        dispenserP->values.waterPumpTimer--; 
    }
    
    if( dispenserP->values.waterApplied == _WaterSourceValue_VACUM ){
      if( dispenserP->values.waterVacumTimer ) 
        dispenserP->values.waterVacumTimer--; 
    }
    
    if((dispenserP->values.iceApplied == _IceSourceValue_ICE_CUBE)||
       (dispenserP->values.iceApplied == _IceSourceValue_ICE_CRASH)){
         if( dispenserP->values.iceTimer ) 
           dispenserP->values.iceTimer--;
         
         if( dispenserP->values.iceContinueTimer )
           dispenserP->values.iceContinueTimer--;
       }
    
    if( dispenserP->values.iceApplied == _IceSourceValue_OFF ){  
      if( dispenserP->values.iceWaitTimer )
        dispenserP->values.iceWaitTimer--;
      
      if( dispenserP->values.iceWaitTimer2 )
        dispenserP->values.iceWaitTimer2--;
      
      if(( dispenserP->values.iceContinueTimer < _ICE_TIME_CONTINUE)&&
        (!dispenserP->values.iceContinueFactorTimer))
      {
        dispenserP->values.iceContinueTimer++;
        dispenserP->values.iceContinueFactorTimer = _FACTOR_CONTINUE;
      }
    }   
    
    if( dispenserP->values.lightTimer )
      dispenserP->values.lightTimer--;
    
    if( dispenserP->values.iceContinueFactorTimer )
      dispenserP->values.iceContinueFactorTimer--;
}

void DispenserCheckState(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP)
{ 
    switch (dispenserP->sets.state)
    {
    case _DispenserState_Measured_8OZ:
    case _DispenserState_Measured_16OZ:
    case _DispenserState_Measured_32OZ:
   /* case _DispenserState_Measured_Custom:
    case _DispenserState_Measured_CalibrateCustom:*/
    case _DispenserState_WaterPump:
      if ((dispenserC->waterType != _DispenserWaterType_WaterPump) &&
          (dispenserC->waterType != _DispenserWaterType_WaterPumpAndVacum))
        dispenserP->sets.state++;
      else
        return;
      
    case _DispenserState_WaterVacum:
      if (dispenserC->waterType != _DispenserWaterType_WaterPumpAndVacum)
        dispenserP->sets.state++;
      else
        return;
      
    case _DispenserState_IceCube:
      if ((dispenserC->iceType != _DispenserIceType_IceCube) &&
          (dispenserC->iceType != _DispenserIceType_IceCubeAndCrash))
        dispenserP->sets.state++;
      else
        return;
      
    case _DispenserState_IceCrash:
      if (dispenserC->iceType != _DispenserIceType_IceCubeAndCrash)
        dispenserP->sets.state++;
      else
        return;
      
    case _DispenserState_OFF:
      if (dispenserC->parameters.OFFState)
        break;
      
    default:
      if (dispenserC->waterType)
        dispenserP->sets.state = _DispenserState_WaterPump;
      else if (dispenserC->iceType)
        dispenserP->sets.state = _DispenserState_IceCube;
      else
        dispenserP->sets.state = _DispenserState_OFF;
      break;
    }
    /*
    if( dispenserC->waterType )
        dispenserP->sets.state = _DispenserState_WaterPump;
    else if( dispenserC->iceType )  
        dispenserP->sets.state = _DispenserState_IceCube;
    else
        dispenserP->sets.state = _DispenserState_OFF;
    */
}


void DispenserNextState(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP, bool cmdReceived, unsigned char *displayEvent)
{ 
  if(!cmdReceived)
    return;

    if (dispenserP->sets.state)
    {
      if(*displayEvent == (unsigned char)eDisplayEventIceCube)
      {
        dispenserP->sets.state = _DispenserState_IceCube;
      }
      else if(*displayEvent == (unsigned char)eDisplayEventIceCrush)
      {
          dispenserP->sets.state = _DispenserState_IceCrash;
      }
      else if(*displayEvent == (unsigned char)eDisplayEventWater)
      {
          dispenserP->sets.state = _DispenserState_WaterPump;
      }
      else if(*displayEvent == (unsigned char)eDisplayEventMeasuredFill)
      {
        if(dispenserP->sets.state == _DispenserState_WaterPump)
        {
          if(dispenserP->sets.lastState == _DispenserState_OFF)
            dispenserP->sets.state = _DispenserState_Measured_8OZ;
          else
            dispenserP->sets.state = dispenserP->sets.lastState;
        }
        else if(dispenserP->sets.state >= _DispenserState_Measured_8OZ)
        {
          //if(dispenserP->sets.state >= _DispenserState_Measured_Custom)
          if(dispenserP->sets.state >= _DispenserState_Measured_32OZ)
            dispenserP->sets.state = _DispenserState_Measured_8OZ;
          else
            dispenserP->sets.state++;
        }
        else if(dispenserP->sets.state == _DispenserState_IceCube || dispenserP->sets.state == _DispenserState_IceCrash)
        {
          if(dispenserP->sets.lastState == _DispenserState_OFF)
            dispenserP->sets.state = _DispenserState_Measured_8OZ;
          else
            dispenserP->sets.state = dispenserP->sets.lastState;
        }
        dispenserP->sets.lastState = dispenserP->sets.state;
      }
      /*else if(*displayEvent == (unsigned char)eDisplayEventCalibrateCustom)
      {
        //        if(!(dispenserP->sets.state == _DispenserState_IceCube || dispenserP->sets.state == _DispenserState_IceCrash))
        dispenserP->sets.state = _DispenserState_Measured_CalibrateCustom;
        //        else
        //          dispenserP->sets.commandIsNotAllowedF = true;
      }*/
    }    
    DispenserCheckState(dispenserC, dispenserP);
    dispenserP->E2.state = dispenserP->sets.state;
}

void DispenserOnOff(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP)
{
    if (dispenserC->parameters.OFFState &&
        dispenserP->sets.state)
        dispenserP->sets.state = _DispenserState_OFF;
    else
        dispenserP->sets.state = _DispenserState_WaterPump;

    DispenserCheckState(dispenserC, dispenserP);
    dispenserP->E2.state = dispenserP->sets.state;
}
void DispenserLightTask(const tsDispenserProcConsts * consts, tsDispenserProcParams* params)
{ 
  stDispenser_Ram *dispenserP = params->dispenser;
    unsigned char step;
    
    if( ( !dispenserP->values.sabbathEnabled ) &&
       dispenserP->components.tactSwitch.value )
      dispenserP->values.lightTimer = dispenserP->sets.lightTime * 6;
    
    if( dispenserP->values.lightTimer || ( dispenserP->sets.lightTime >= 99 ) || (params->rf->ui.states.normal.state != _UI_NormalState_DisplayOff))
    { 
      dispenserP->components.light.value = 100;
      // if dispenser light time >=99 full on
      /*if( dispenserP->components.light.value != 100)
      {
        dispenserP->values.lightEffectTimer = 0;
        dispenserP->components.light.value = 100;
      }
      
      if( dispenserP->values.lightEffectTimer < 5 )        step = 10;
      else if( dispenserP->values.lightEffectTimer < 10 )  step = 15;
      else if( dispenserP->values.lightEffectTimer < 15 )  step = 20;
      else if( dispenserP->values.lightEffectTimer < 20 )  step = 25;//31;
      else step = 100;*/
    }
    else
    {
      dispenserP->components.light.value = 0;
     /* if( dispenserP->components.light.value != 0){    
        dispenserP->values.lightEffectTimer = 0;      
        dispenserP->components.light.value = 0;
      }
      
      if( dispenserP->values.lightEffectTimer < 5 )       step = 100;
      else if( dispenserP->values.lightEffectTimer < 10 ) step = 25;
      else if( dispenserP->values.lightEffectTimer < 15 ) step = 20;
      else if( dispenserP->values.lightEffectTimer < 20 ) step = 15;
      else step = 10;*/
    }
    
    //dispenserP->components.light.value = step;
      
}
void DispenserType_WaterPump(const tsDispenserProcConsts * consts, tsDispenserProcParams* params)
{ 
  stDispenser_Ram *dispenserP = params->dispenser;
  const stDispenser_Flash *dispenserC = consts->dispenser;
  
  dispenserP->values.waterDesired = _WaterSourceValue_OFF;
  
  if ((!dispenserP->components.tactSwitch.value && !dispenserP->values.dispenserTactFlag) || params->rf->ui.dspPacket.funcs2.ledLockDspr)
  {
    dispenserP->values.waterPumpTimer = _WATER_PUMP_TIME;
  }
  
  if (dispenserP->values.sabbathEnabled)
  {
    dispenserP->values.waterPumpTimer = _WATER_PUMP_TIME;
    return;
  }
  
    switch (dispenserP->sets.state)
    {
    case _DispenserState_WaterPump:
      if((dispenserP->components.tactSwitch.value) && (!params->rf->Values.keyLockDspr ) && (!dispenserP->components.doorSwitch.value))
      {
        if (dispenserP->values.waterPumpTimer)
        {//Case 1: Normal water operation
          dispenserP->values.waterDesired = _WaterSourceValue_PUMP;
          dispenserP->values.dispenserProhibitFlag = false;
        }
        else
        {//Case 2: Prohibition water operation
          dispenserP->values.dispenserProhibitFlag = true;
        }
      }
      else
      {
        dispenserP->values.dispenserTactFlag = 0;
        if( dispenserP->components.tactSwitch.value )
          dispenserP->values.dispenserProhibitFlag = true;
        else
          dispenserP->values.dispenserProhibitFlag = false;
      }
      break;
      
    case _DispenserState_Measured_8OZ:
      if((dispenserP->components.tactSwitch.value || dispenserP->values.dispenserTactFlag) && (!params->rf->Values.keyLockDspr )&& (!dispenserP->components.doorSwitch.value))
        if ((dispenserP->values.waterPumpTimer > (_WATER_PUMP_TIME - dispenserC->parameters.autofill.autofillTime_8OZ)) && (dispenserP->values.waterPumpTimer)) 
          dispenserP->values.waterDesired = _WaterSourceValue_PUMP;
        else
          dispenserP->values.dispenserTactFlag = 0;
      else
      {
        if( dispenserP->components.tactSwitch.value )
          dispenserP->values.dispenserProhibitFlag = true;
        else
          dispenserP->values.dispenserProhibitFlag = false;
      }
      break;
      
    case _DispenserState_Measured_16OZ:
      if((dispenserP->components.tactSwitch.value || dispenserP->values.dispenserTactFlag) && (!params->rf->Values.keyLockDspr )&& (!dispenserP->components.doorSwitch.value))
        if ((dispenserP->values.waterPumpTimer > (_WATER_PUMP_TIME - dispenserC->parameters.autofill.autofillTime_16OZ)) && (dispenserP->values.waterPumpTimer)) 
          dispenserP->values.waterDesired = _WaterSourceValue_PUMP;
        else
          dispenserP->values.dispenserTactFlag = 0;
      else
      {
        if( dispenserP->components.tactSwitch.value )
          dispenserP->values.dispenserProhibitFlag = true;
        else
          dispenserP->values.dispenserProhibitFlag = false;
      }
      break;          
      
    case _DispenserState_Measured_32OZ:
      if((dispenserP->components.tactSwitch.value || dispenserP->values.dispenserTactFlag) && (!params->rf->Values.keyLockDspr )&& (!dispenserP->components.doorSwitch.value))
        if ((dispenserP->values.waterPumpTimer > (_WATER_PUMP_TIME - dispenserC->parameters.autofill.autofillTime_32OZ)) && (dispenserP->values.waterPumpTimer)) 
          dispenserP->values.waterDesired = _WaterSourceValue_PUMP;
        else
          dispenserP->values.dispenserTactFlag = 0;
      else
      {
        if( dispenserP->components.tactSwitch.value )
          dispenserP->values.dispenserProhibitFlag = true;
        else
          dispenserP->values.dispenserProhibitFlag = false;
      }
      break; 
      
    /*case _DispenserState_Measured_Custom:
      if((dispenserP->components.tactSwitch.value || dispenserP->values.dispenserTactFlag) && (!params->rf->Values.keyLockDspr ))
      {
        if(dispenserP->sets.waterCalibrateTime)
        {
          if (dispenserP->values.waterPumpTimer > dispenserP->sets.waterCalibrateTime)
            dispenserP->values.waterDesired = _WaterSourceValue_PUMP;
          else
            dispenserP->values.dispenserTactFlag = 0;
        }
        else
        {
          dispenserP->values.dispenserTactFlag = 0;
          dispenserP->sets.state = _DispenserState_WaterPump;
        }          
      }
      else
        dispenserP->values.dispenserTactFlag = 0;
      break;
      
    case _DispenserState_Measured_CalibrateCustom:
      if((dispenserP->components.tactSwitch.value) && (!params->rf->Values.keyLockDspr ))
      {
        if (dispenserP->values.waterPumpTimer)
        {
          dispenserP->values.waterDesired = _WaterSourceValue_PUMP;
          dispenserP->sets.waterCalibrateTime = dispenserP->values.waterPumpTimer;                  
          dispenserP->values.calibrateLockFlag = 1; 
        }
      }
      else
        if(dispenserP->values.calibrateLockFlag)
        {
          dispenserP->E2.waterCalibrateTime = dispenserP->sets.waterCalibrateTime; 
          dispenserP->sets.state = _DispenserState_Measured_Custom;
          dispenserP->values.calibrateLockFlag = 0;
          sysBuzzerCmd(eSysBuzzerPlayCloseMelody);
        }
        
                
      break;    */
  }  
}
void DispenserType_WaterPumpAndVacum(const tsDispenserProcConsts * consts, tsDispenserProcParams* params)
{
    stDispenser_Ram *dispenserP = params->dispenser;
    dispenserP->values.waterDesired = _WaterSourceValue_OFF;
    
  if ((!dispenserP->components.tactSwitch.value && !dispenserP->values.dispenserTactFlag) || params->rf->ui.dspPacket.funcs2.ledLockDspr)
  {
      dispenserP->values.waterPumpTimer = _WATER_PUMP_TIME;
      dispenserP->values.waterVacumTimer = _WATER_VACUM_TIME;
  }
    
    if( dispenserP->values.sabbathEnabled ){
      dispenserP->values.waterPumpTimer = _WATER_PUMP_TIME;
      dispenserP->values.waterVacumTimer = _WATER_VACUM_TIME;
      return;  
    } 
    
    switch( dispenserP->sets.state )
    {
    case _DispenserState_WaterPump:
      if( dispenserP->components.tactSwitch.value )
      {      
        if(dispenserP->values.waterPumpTimer)
          dispenserP->values.waterDesired = _WaterSourceValue_PUMP;      
      }    
      break;
    case _DispenserState_WaterVacum:
      if( dispenserP->components.tactSwitch.value )
      {      
        if(dispenserP->values.waterVacumTimer)
          dispenserP->values.waterDesired = _WaterSourceValue_VACUM;      
      }       
      break;
    }
  
}

void DispenserType_IceCube(const tsDispenserProcConsts * consts, tsDispenserProcParams* params)
{
    stDispenser_Ram *dispenserP = params->dispenser;
  
    dispenserP->values.iceDesired = _IceSourceValue_OFF;                           
    
    if( ( !dispenserP->components.tactSwitch.value ) ||
        ( dispenserP->components.doorSwitch.value ) || 
        ( params->rf->ui.dspPacket.funcs2.ledLockDspr ) )
      {
         dispenserP->values.iceTimer = _ICE_TIME;
      }  
    
    if( dispenserP->values.sabbathEnabled ){
      dispenserP->values.iceTimer = _ICE_TIME;
      return;  
    }  
    
    switch( dispenserP->sets.state )
    {
    case _DispenserState_IceCube:
      if( ( dispenserP->components.tactSwitch.value ) && ( !dispenserP->components.doorSwitch.value ) )
      {
        if( dispenserP->values.iceWaitTimer )
        {  
          dispenserP->values.iceTimer = _ICE_TIME;
        }
        else if( dispenserP->values.iceTimer )
        {
          dispenserP->values.iceDesired = _IceSourceValue_ICE_CUBE;
        }
        else
        {
          dispenserP->values.iceWaitTimer = _ICE_WAIT_TIME;
        }      
      }  
      break;
    }
    
}
void DispenserType_IceCubeAndCrash(const tsDispenserProcConsts * consts, tsDispenserProcParams* params)
{
    stDispenser_Ram *dispenserP = params->dispenser;
    dispenserP->values.iceDesired = _IceSourceValue_OFF;                           
    
    if( ( !dispenserP->components.tactSwitch.value ) ||
        ( dispenserP->components.doorSwitch.value ) || 
        ( params->rf->ui.dspPacket.funcs2.ledLockDspr ) )
      {
         dispenserP->values.iceTimer = _ICE_TIME;
      }
    
    if( dispenserP->values.sabbathEnabled ){
      dispenserP->values.iceTimer = _ICE_TIME;
      return;  
    }
    
    switch( dispenserP->sets.state )
    {
    case _DispenserState_IceCube:
      if( ( dispenserP->components.tactSwitch.value ) &&
         ( !dispenserP->components.doorSwitch.value ) &&
           ( !params->rf->Values.keyLockDspr ) &&
           (params->rf->Cabin[1].components.airTempSensor.valueInCExt < -50))
      {// Main case 1: Ice cube operation.
           if(( dispenserP->values.iceWaitTimer ) || (dispenserP->values.iceWaitTimer2))
           {//Sub case 1: prohibition ice cube when ice cube operation to Max Time.
             dispenserP->values.iceTimer = _ICE_TIME;
             dispenserP->values.dispenserProhibitFlag = true;
           }else if( !dispenserP->values.iceContinueTimer )
           {//Sub case 2: Enable long time prohibition
             dispenserP->values.iceWaitTimer2 = _ICE_WAIT_TIME2; 
           }else if( dispenserP->values.iceTimer )
           {//Sub case 3: Ice cube working and reset Prohibition flag.
             dispenserP->values.iceDesired = _IceSourceValue_ICE_CUBE;
             dispenserP->values.dispenserProhibitFlag = false;
           }else
           {//Sub case 4: Enable short time prohibition.
             dispenserP->values.iceWaitTimer = _ICE_WAIT_TIME;
           }
      }else
      {// Main case 2: Prohibitiob Ice cube operation.
        if( dispenserP->components.tactSwitch.value )
          dispenserP->values.dispenserProhibitFlag = true;
        else
          dispenserP->values.dispenserProhibitFlag = false;
      }
      break;
    case _DispenserState_IceCrash:
      if( ( dispenserP->components.tactSwitch.value ) &&
         ( !dispenserP->components.doorSwitch.value ) &&
           ( !params->rf->Values.keyLockDspr ) &&
           (params->rf->Cabin[1].components.airTempSensor.valueInCExt < -50))
      {// Main case 1: Ice Crash operation.
           if(( dispenserP->values.iceWaitTimer )|| (dispenserP->values.iceWaitTimer2))
           {//Sub case 1: prohibition ice Crash when ice Crash operation to Max Time.
             dispenserP->values.iceTimer = _ICE_TIME;
             dispenserP->values.dispenserProhibitFlag = true;
           }else if( !dispenserP->values.iceContinueTimer )
           {//Sub case 2: Enable long time prohibition
             dispenserP->values.iceWaitTimer2 = _ICE_WAIT_TIME2;  
           }else if( dispenserP->values.iceTimer )
           {//Sub case 3: Ice Crash working and reset Prohibition flag.
             dispenserP->values.iceDesired = _IceSourceValue_ICE_CRASH;
             dispenserP->values.dispenserProhibitFlag = false;
           }else
           {//Sub case 4: Enable short time prohibition.
             dispenserP->values.iceWaitTimer = _ICE_WAIT_TIME;
           }      
      }else
      {// Main case 2: Prohibitiob Ice Crash operation.
        if( dispenserP->components.tactSwitch.value )
          dispenserP->values.dispenserProhibitFlag = true;
        else
          dispenserP->values.dispenserProhibitFlag = false;
      }
      break;    
    }
}

void DispenserType_IceCubeAndCrash_CommercialTest(const tsDispenserProcConsts * consts, tsDispenserProcParams* params)
{
    stDispenser_Ram *dispenserP = params->dispenser;
    dispenserP->values.iceDesired = _IceSourceValue_OFF;                         
    
    if( ( !dispenserP->components.tactSwitch.value ) ||
        ( dispenserP->components.doorSwitch.value ) || 
        ( params->rf->ui.dspPacket.funcs2.ledLockDspr ) )
      {
         dispenserP->values.iceTimer = _ICE_TIME;
      }
    
    if( dispenserP->values.sabbathEnabled ){
      dispenserP->values.iceTimer = _ICE_TIME;
      return;  
    }
    
    switch( dispenserP->sets.state )
    {
    case _DispenserState_IceCube:
      if( ( dispenserP->components.tactSwitch.value ) &&
         ( !dispenserP->components.doorSwitch.value ) &&
           ( !params->rf->Values.keyLockDspr ))
      {// Main case 1: Ice cube operation.
           if(( dispenserP->values.iceWaitTimer ) || (dispenserP->values.iceWaitTimer2))
           {//Sub case 1: prohibition ice cube when ice cube operation to Max Time.
             dispenserP->values.iceTimer = _ICE_TIME;
             dispenserP->values.dispenserProhibitFlag = true;
           }else if( !dispenserP->values.iceContinueTimer )
           {//Sub case 2: Enable long time prohibition
             dispenserP->values.iceWaitTimer2 = _ICE_WAIT_TIME2; 
           }else if( dispenserP->values.iceTimer )
           {//Sub case 3: Ice cube working and reset Prohibition flag.
             dispenserP->values.iceDesired = _IceSourceValue_ICE_CUBE;
             dispenserP->values.dispenserProhibitFlag = false;
           }else
           {//Sub case 4: Enable short time prohibition.
             dispenserP->values.iceWaitTimer = _ICE_WAIT_TIME;
           }
      }else
      {// Main case 2: Prohibitiob Ice cube operation.
        if( dispenserP->components.tactSwitch.value )
          dispenserP->values.dispenserProhibitFlag = true;
        else
          dispenserP->values.dispenserProhibitFlag = false;
      }
      break;
    case _DispenserState_IceCrash:
      if( ( dispenserP->components.tactSwitch.value ) &&
         ( !dispenserP->components.doorSwitch.value ) &&
           ( !params->rf->Values.keyLockDspr ))
      {// Main case 1: Ice Crash operation.
           if(( dispenserP->values.iceWaitTimer )|| (dispenserP->values.iceWaitTimer2))
           {//Sub case 1: prohibition ice Crash when ice Crash operation to Max Time.
             dispenserP->values.iceTimer = _ICE_TIME;
             dispenserP->values.dispenserProhibitFlag = true;
           }else if( !dispenserP->values.iceContinueTimer )
           {//Sub case 2: Enable long time prohibition
             dispenserP->values.iceWaitTimer2 = _ICE_WAIT_TIME2;  
           }else if( dispenserP->values.iceTimer )
           {//Sub case 3: Ice Crash working and reset Prohibition flag.
             dispenserP->values.iceDesired = _IceSourceValue_ICE_CRASH;
             dispenserP->values.dispenserProhibitFlag = false;
           }else
           {//Sub case 4: Enable short time prohibition.
             dispenserP->values.iceWaitTimer = _ICE_WAIT_TIME;
           }      
      }else
      {// Main case 2: Prohibitiob Ice Crash operation.
        if( dispenserP->components.tactSwitch.value )
          dispenserP->values.dispenserProhibitFlag = true;
        else
          dispenserP->values.dispenserProhibitFlag = false;
      }
      break;    
    }
}

void DispenserInitAfterHotStart(stDispenser_Ram *dispenserP)
{
    DispenserE2AfterHotStart( dispenserP );
    DispenserE2ToRam( dispenserP );  
}

void dispenserClearDisplaySetting20MsTask(stDispenser_Ram *dispenserP)
{
    DispenserE2ClearDisplaySetting( dispenserP );
}

void DispenserInitAfterService( stDispenser_Ram *dispenserP )
{
    DispenserE2AfterService( dispenserP );
    DispenserE2ToRam( dispenserP );  
}

void DispenserE2Init(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP)
{
    dispenserP->E2.state = _DispenserState_OFF; 
    dispenserP->E2.lightTime = dispenserC->parameters.lightTime;
}

void DispenserE2AfterHotStart( stDispenser_Ram *dispenserP)
{
    dispenserP->E2.state = _DispenserState_OFF;
    dispenserP->E2.lightTime = refrigerator_F.Dispenser.parameters.lightTime;
}

void DispenserE2ClearDisplaySetting( stDispenser_Ram *dispenserP)
{
 
    dispenserP->E2.state = _DispenserState_OFF; 
    dispenserP->E2.lightTime = refrigerator_F.Dispenser.parameters.lightTime;
}

void DispenserE2AfterService(  stDispenser_Ram *dispenserP)
{
    dispenserP->E2.state = _DispenserState_OFF; 
    dispenserP->E2.lightTime = refrigerator_F.Dispenser.parameters.lightTime;
}

void DispenserE2ToRam( stDispenser_Ram *dispenserP)
{
    dispenserP->sets.state = dispenserP->E2.state;
    dispenserP->sets.lightTime = dispenserP->E2.lightTime;
}
void DispenserComponentsTest(const tsDispenserProcConsts * consts, tsDispenserProcParams* params)
{
    DispenserTask(consts, params);                                     
}

void DispenserCloseComponents( stDispenser_Ram *dispenserP ){

    dispenserP->components.light.value = 0;                                      
}

bool isDispenserComponentsTestDone(const stDispenser_Flash* dispenserC, stDispenser_Ram* dispenserP)
{
    return true;
}


unsigned char DispenserGetLogData( stDispenser_Ram *dispenserP,  unsigned char* dataArrayPtr )
{
    unsigned char size = 0;
    
    dataArrayPtr[ size++ ] =
      ( ( dispenserP->components.tactSwitch.value & 0x01 ) << 0 ) |
        ( ( dispenserP->components.light.value & 0x01 ) << 1 ) |
          ( ( dispenserP->components.doorSwitch.value  & 0x01 ) << 2 );
    
    return size;
}
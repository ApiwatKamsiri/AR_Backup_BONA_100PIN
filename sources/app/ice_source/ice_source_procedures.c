/** @file       ice_source.c
 *  @brief      ICE_SOURCE application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       9/21/2024
 */
   
#define FILE_ICE_SOURCE_PROCEDURES_C
#include "ice_source_procedures.h"

#define _SHUTTING_START     25  // 25 * 20ms = 0,5 .sec
#define _SELECTING_START    50  // 50 * 20ms = 1,0 .sec
#define _PUSHING_START      75  // 75 * 20ms = 1,5 .sec

#define _SELECTING_STOP     25  // 50 * 20ms = 0,5 .sec
#define _SHUTTING_STOP      50  // 50 * 20ms = 1,0 .sec

#define _SHUTTING_TIME_DC      50  // 50 * 20ms = 1sec
#define _SELECTING_TIME_DC     50  // 50 * 20ms = 1sec

#define _SHUTTING_TIME_AC      50 * 20  // 50 * 20ms = 1sec : 50*20 = 50*20*20ms = 20s
#define _SELECTING_TIME_AC     50 * 20  // 50 * 20ms = 1sec : 50*20 = 50*20*20ms = 20s
   
#define _PUSHING_TIME_AC       50 * 18  // 50 * 20ms = 1sec : 50*20 = 50*18*20ms = 18s

#define _SELECTING_TIME_DELAY_ON     50*2.5  // 50 * 20ms = 1sec : 50*20 = 50*2.5*20ms = 2.5s
#define _SHUTTER_TIME_DELAY_OFF      50*3.5  // 50 * 20ms = 1sec : 50*20 = 50*3.5*20ms = 3.5s

void IceSourceTask(const tsIcesourceProcConsts* consts, tsIcesourceProcParams * params)
{
  stIceSource_Ram* iceSourceP = params->icesource;
  const stIceSource_Flash* iceSourceC = consts->icesource;
  
    switch (iceSourceC->type)
    {
      case _IceSourceType_IceCube:
        IceSourceType_IceCube(iceSourceC, iceSourceP);
        break;
        
      case _IceSourceType_IceCubeAndCrash:
        IceSourceType_IceCubeAndCrash(iceSourceC, iceSourceP);
        break;
        
      case _IceSourceType_NONE:
      default:
        break;
    }
    
    if(params->icesource->values.flagForShutter)
    {
      params->icesource->values.timerOnOff = 250;
      params->icesource->values.flagForShutter = 0;
      params->icesource->values.iceDispenserWaitTime=100;
    }  
    
    params->rf->Dispenser.values.iceApplied = iceSourceP->values.dispenserApplied;
    
    params->rf->WaterSource.values.noWaterSet = params->rf->IceMachine.values.noWaterSet;
    params->rf->IceMachine.values.noWaterClr = params->rf->WaterSource.values.noWaterClr;
}
void IceSourceT20ms(stIceSource_Ram *iceSourceP)
{
    if (iceSourceP->values.selectorTimer)
      iceSourceP->values.selectorTimer--;
    if (iceSourceP->values.shutterTimer)
      iceSourceP->values.shutterTimer--;
    if (iceSourceP->values.pusherTimer)
      iceSourceP->values.pusherTimer--;
    
    if (iceSourceP->values.startTimer < 0xFF)
        iceSourceP->values.startTimer++;
    if (iceSourceP->values.stopTimer < 0xFF)
        iceSourceP->values.stopTimer++;
    
    if(iceSourceP->values.timerOnOff)
      iceSourceP->values.timerOnOff--;
    
    if(iceSourceP->values.iceDispenserWaitTime)
      iceSourceP->values.iceDispenserWaitTime--;
    
    if(iceSourceP->values.dispanserRepushTimer)
      iceSourceP->values.dispanserRepushTimer--;
    
    if (iceSourceP->values.delaySelectionTimer < 0xFF)
      iceSourceP->values.delaySelectionTimer++;
    if (iceSourceP->values.delayShutterTimer < 0xFF)
      iceSourceP->values.delayShutterTimer++;
}

void IceSourceShutterState(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP, enShutterStates desiredState)
{
    switch (iceSourceP->values.shutterState)
    {
        case _ShutterState_OpenInit:
          if(iceSourceC->components.shutter.type == _ShutterType_ACMotor)
          {
            iceSourceP->values.shutterTimer = _SHUTTING_TIME_AC;
            iceSourceP->values.pusherTimer = _PUSHING_TIME_AC;
          }
          else if(iceSourceC->components.shutter.type == _ShutterType_BDCMotor)
            iceSourceP->values.shutterTimer = _SHUTTING_TIME_DC;
          else
            iceSourceP->values.shutterTimer = _SHUTTING_TIME_DC;
            iceSourceP->values.shutterState = _ShutterState_Opening;
        case _ShutterState_Opening:
            if (iceSourceP->values.shutterTimer)
            {
                if (desiredState == _ShutterState_Unknown)
                    iceSourceP->values.shutterState = _ShutterState_Unknown;
            }
            else
                iceSourceP->values.shutterState = _ShutterState_Opened;
        case _ShutterState_Opened:
            if (desiredState == _ShutterState_Closed)
            {
              iceSourceP->values.flagForShutter = 1;
              iceSourceP->values.shutterState = _ShutterState_CloseInit;
            }
            iceSourceP->values.dispanserRepushTimer=250;
        break;

        case _ShutterState_CloseInit:
          if(iceSourceC->components.shutter.type == _ShutterType_ACMotor)
          {
            iceSourceP->values.shutterTimer = _SHUTTING_TIME_AC;
            iceSourceP->values.pusherTimer = _PUSHING_TIME_AC;
          }
          else if(iceSourceC->components.shutter.type == _ShutterType_BDCMotor)
            iceSourceP->values.shutterTimer = _SHUTTING_TIME_DC;
          else
            iceSourceP->values.shutterTimer = _SHUTTING_TIME_DC;
            iceSourceP->values.shutterState = _ShutterState_Closing;
        case _ShutterState_Closing:
            if (iceSourceP->values.shutterTimer)
            {
                if (desiredState == _ShutterState_Unknown)
                    iceSourceP->values.shutterState = _ShutterState_Unknown;
            }
            else
                iceSourceP->values.shutterState = _ShutterState_Closed;
        case _ShutterState_Closed:
            if (desiredState == _ShutterState_Opened)
            {
                iceSourceP->values.timerOnOff = 0;
                iceSourceP->values.iceDispenserWaitTime = 0;
                iceSourceP->values.shutterState = _ShutterState_OpenInit;
            }
        break;

        case _ShutterState_Unknown:
        default:
            if (desiredState == _ShutterState_Opened)
                iceSourceP->values.shutterState = _ShutterState_OpenInit;
            if (desiredState == _ShutterState_Closed)
                iceSourceP->values.shutterState = _ShutterState_CloseInit;
        break;
    }
}

void IceSourceSelectorState(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP, enSelectorStates desiredState)
{
    switch (iceSourceP->values.selectorState)
    {
        case _SelectorState_IceCubeInit:
          if(iceSourceC->components.selector.type == _SelectorType_ACMotor)
          {
            iceSourceP->values.selectorTimer = _SELECTING_TIME_AC;
            iceSourceP->values.pusherTimer = _PUSHING_TIME_AC;
          }
          else if(iceSourceC->components.selector.type == _SelectorType_BDCMotor)
            iceSourceP->values.selectorTimer = _SELECTING_TIME_DC;
          else
            iceSourceP->values.selectorTimer = _SELECTING_TIME_DC;
            iceSourceP->values.selectorState = _SelectorState_IceCubeSelecting;
        case _SelectorState_IceCubeSelecting:
            if (iceSourceP->values.selectorTimer)
            {
                if (desiredState == _SelectorState_Unknown)
                    iceSourceP->values.selectorState = _SelectorState_Unknown;
            }
            else
                iceSourceP->values.selectorState = _SelectorState_IceCubeSelected;
        case _SelectorState_IceCubeSelected:
            if (desiredState == _SelectorState_IceCrashSelected)
                iceSourceP->values.selectorState = _SelectorState_IceCrashInit;
        break;

        case _SelectorState_IceCrashInit:
          if(iceSourceC->components.selector.type == _SelectorType_ACMotor)
          {
            iceSourceP->values.selectorTimer = _SELECTING_TIME_AC;
            iceSourceP->values.pusherTimer = _PUSHING_TIME_AC;
          }
          else if(iceSourceC->components.selector.type == _SelectorType_BDCMotor)
            iceSourceP->values.selectorTimer = _SELECTING_TIME_DC;
          else
            iceSourceP->values.selectorTimer = _SELECTING_TIME_DC;
            iceSourceP->values.selectorState = _SelectorState_IceCrashSelecting;
        case _SelectorState_IceCrashSelecting:
            if (iceSourceP->values.selectorTimer)
            {
                if (desiredState == _SelectorState_Unknown)
                    iceSourceP->values.selectorState = _SelectorState_Unknown;
            }
            else
                iceSourceP->values.selectorState = _SelectorState_IceCrashSelected;
        case _SelectorState_IceCrashSelected:
            if (desiredState == _SelectorState_IceCubeSelected)
                iceSourceP->values.selectorState = _SelectorState_IceCubeInit;
        break;

        case _SelectorState_Unknown:
        default:
            if (desiredState == _SelectorState_IceCubeSelected)
                iceSourceP->values.selectorState = _SelectorState_IceCubeInit;
            if (desiredState == _SelectorState_IceCrashSelected)
                iceSourceP->values.selectorState = _SelectorState_IceCrashInit;
        break;
    }
}

void IceSourcePusherState(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP, enPusherStates desiredState)
{
    switch (iceSourceP->values.pusherState)
    {
    case _PusherState_Pushing:
        if (desiredState == _PusherState_OFF)
          iceSourceP->values.pusherState = _PusherState_OFF;
        if(iceSourceC->components.selector.type == _SelectorType_ACMotor)
        {
          if (!iceSourceP->values.pusherTimer)
            iceSourceP->values.pusherState = _PusherState_OFF;
        }
        break;
    case _PusherState_OFF:
    default:
        if (desiredState == _PusherState_Pushing && iceSourceP->values.pusherTimer)
            iceSourceP->values.pusherState = _PusherState_Pushing;
        break;
    }
}

void IceSourceToShutter(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP)
{
    if (iceSourceP->values.shutterState == _ShutterState_Opening)
    {
        iceSourceP->components.shutter.value = _ShutterValue_OPENING;
        iceSourceP->components.shutter.beforeOpening = true;
        iceSourceP->values.delayShutterTimer = 0;
    }
    else if (iceSourceP->values.shutterState == _ShutterState_Closing && 
          iceSourceP->values.delayShutterTimer < _SHUTTER_TIME_DELAY_OFF)
    {
      iceSourceP->components.shutter.value = _ShutterValue_OPENING;
      iceSourceP->components.shutter.beforeOpening = true;
    }
    else if (iceSourceP->values.shutterState == _ShutterState_Closing && 
          iceSourceP->values.delayShutterTimer >= _SHUTTER_TIME_DELAY_OFF)
    {
      if(iceSourceC->components.shutter.type == _ShutterType_ACMotor)
      {
        iceSourceP->components.shutter.value = _ShutterValue_OFF;
        iceSourceP->components.shutter.beforeOpening = false;
      }
      else if(iceSourceC->components.shutter.type == _ShutterType_BDCMotor)
      {
        iceSourceP->components.shutter.value = _ShutterValue_CLOSING;
        iceSourceP->components.shutter.beforeOpening = false;
      }
      else
      {
        iceSourceP->components.shutter.value = _ShutterValue_CLOSING;
        iceSourceP->components.shutter.beforeOpening = false;
      }
    }
    else
    {
      if(!iceSourceP->components.shutter.beforeOpening)
          iceSourceP->components.shutter.value = _ShutterValue_OFF;
      else
          iceSourceP->components.shutter.value = _ShutterValue_OPENING;
    }
}

void IceSourceToSelector(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP)
{
    if (iceSourceP->values.selectorState == _SelectorState_IceCubeSelecting && 
          iceSourceP->values.delaySelectionTimer >= _SELECTING_TIME_DELAY_ON)
        iceSourceP->components.selector.value = _SelectorValue_SELECTING_A;
    else if (iceSourceP->values.selectorState == _SelectorState_IceCrashSelecting)
    {
      if(iceSourceC->components.selector.type == _SelectorType_ACMotor)
        iceSourceP->components.selector.value = _SelectorValue_OFF;
      else if(iceSourceC->components.selector.type == _SelectorType_BDCMotor)
        iceSourceP->components.selector.value = _SelectorValue_SELECTING_B;
      else
        iceSourceP->components.selector.value = _SelectorValue_SELECTING_B;
    }
    else
        iceSourceP->components.selector.value = _SelectorValue_OFF;
}

void IceSourceToPusher(stIceSource_Ram *iceSourceP)
{
    if (iceSourceP->values.pusherState == _PusherState_Pushing)
      iceSourceP->components.pusher.value = _PusherValue_PUSH;
    else
    {
      iceSourceP->components.pusher.value = _PusherValue_OFF;
      iceSourceP->values.delaySelectionTimer = 0;
    }
}

void IceSourceType_IceCube(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP)
{
    if (iceSourceP->values.dispenserDesired == _IceSourceValue_OFF)
    {
        iceSourceP->values.startTimer = 0;
        IceSourcePusherState(iceSourceC, iceSourceP, _PusherState_OFF);

        iceSourceP->values.selectorState = _SelectorState_IceCubeSelected;

        if (iceSourceP->values.stopTimer > _SHUTTING_STOP)
            IceSourceShutterState(iceSourceC, iceSourceP, _ShutterState_Closed);
        else
            IceSourceShutterState(iceSourceC, iceSourceP, _ShutterState_Unknown);
    }
    else
    {
        iceSourceP->values.stopTimer = 0;

        if (iceSourceP->values.startTimer > _SHUTTING_START)
            IceSourceShutterState(iceSourceC, iceSourceP, _ShutterState_Opened);
        else
            IceSourceShutterState(iceSourceC, iceSourceP, _ShutterState_Unknown);

        iceSourceP->values.selectorState = _SelectorState_IceCubeSelected;

        if (iceSourceP->values.startTimer > _PUSHING_START)
            IceSourcePusherState(iceSourceC, iceSourceP, _PusherState_Pushing);
        else
            IceSourcePusherState(iceSourceC, iceSourceP, _PusherState_OFF);
    }

    IceSourceToShutter(iceSourceC, iceSourceP);
    IceSourceToSelector(iceSourceC, iceSourceP);
    IceSourceToPusher(iceSourceP);

    if (iceSourceP->values.pusherState == _PusherState_Pushing)
        iceSourceP->values.dispenserApplied = _IceSourceValue_ICE_CUBE;
    else
        iceSourceP->values.dispenserApplied = _IceSourceValue_OFF;
}

void IceSourceType_IceCubeAndCrash(stIceSource_Flash const *iceSourceC, stIceSource_Ram *iceSourceP)
{
    if (iceSourceP->values.dispenserDesired == _IceSourceValue_OFF)
    {
        iceSourceP->values.startTimer = 0;
        IceSourcePusherState(iceSourceC, iceSourceP, _PusherState_OFF);

        if (iceSourceP->values.stopTimer > _SELECTING_STOP)
            IceSourceSelectorState(iceSourceC, iceSourceP, _SelectorState_IceCrashSelected);
        else
            IceSourceSelectorState(iceSourceC, iceSourceP, _SelectorState_Unknown);

        if (iceSourceP->values.stopTimer > _SHUTTING_STOP)
            IceSourceShutterState(iceSourceC, iceSourceP, _ShutterState_Closed);
        else
            IceSourceShutterState(iceSourceC, iceSourceP, _ShutterState_Unknown);
    }
    else
    {
        iceSourceP->values.stopTimer = 0;

        if (iceSourceP->values.startTimer > _SHUTTING_START)
            IceSourceShutterState(iceSourceC, iceSourceP, _ShutterState_Opened);
        else
            IceSourceShutterState(iceSourceC, iceSourceP, _ShutterState_Unknown);

        if (iceSourceP->values.startTimer > _SELECTING_START)
        {
            if (iceSourceP->values.dispenserDesired == _IceSourceValue_ICE_CUBE)
                IceSourceSelectorState(iceSourceC, iceSourceP, _SelectorState_IceCubeSelected);
            if (iceSourceP->values.dispenserDesired == _IceSourceValue_ICE_CRASH)
                IceSourceSelectorState(iceSourceC, iceSourceP, _SelectorState_IceCrashSelected);
        }
        else
            IceSourceSelectorState(iceSourceC, iceSourceP, _SelectorState_Unknown);

        if (iceSourceP->values.startTimer > _PUSHING_START)
            IceSourcePusherState(iceSourceC, iceSourceP, _PusherState_Pushing);
        else
            IceSourcePusherState(iceSourceC, iceSourceP, _PusherState_OFF);
    }

    IceSourceToShutter(iceSourceC, iceSourceP);
    IceSourceToSelector(iceSourceC, iceSourceP);
    IceSourceToPusher(iceSourceP);

    if (iceSourceP->values.pusherState == _PusherState_Pushing)
        iceSourceP->values.dispenserApplied = iceSourceP->values.dispenserDesired;
    else
        iceSourceP->values.dispenserApplied = _IceSourceValue_OFF;
}

void IceSourceCloseComponents(stIceSource_Ram *iceSourceP)
{
    iceSourceP->components.selector.value = _SelectorValue_OFF;
    iceSourceP->components.shutter.value = _ShutterValue_OFF;
    iceSourceP->components.pusher.value = _PusherValue_OFF;
}

void IceSourceComponentsTest(const tsIcesourceProcConsts* consts, tsIcesourceProcParams * params)                      
{
    IceSourceTask(consts, params);
}

bool isIceSourceComponentsTestDone(const stIceSource_Flash  *iceSourceC,
                                   stIceSource_Ram *iceSourceP)
{
    /* Component Test is directly passed..                                  
  if( iceSourceC->type ){
   if( !isSelectorTestSelADone( &iceSourceC->components.selector, testTimer ) )
   	return false;
   if( !isSelectorTestSelBDone( &iceSourceC->components.selector, testTimer ) )
   	return false;
   if( !isShutterTestCloseDone( &iceSourceC->components.shutter, testTimer ) )
   	return false;
   if( !isPusherTestDone( &iceSourceC->components.pusher, testTimer ) )
   	return false;
  }
  */
    return true;
}

unsigned char IceSourceGetLogData(stIceSource_Ram *iceSourceP, unsigned char *dataArrayPtr)
{
    unsigned char size = 0;

    dataArrayPtr[size++] =
        ((iceSourceP->components.pusher.value & 0x01) << 0) |
        ((iceSourceP->components.selector.value & 0x03) << 1) |
        ((iceSourceP->components.shutter.value & 0x03) << 3);

    return size;
}
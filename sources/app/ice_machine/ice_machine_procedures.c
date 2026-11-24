/** @file       ice_machine_procedures.c
 *  @brief      ice_machine_procedures application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Omer Cihan Ekiner
 *  @date       9/20/2024
 */
#define ICE_MACHINE_PROCEDURES_C

#include "ice_machine_procedures.h"
#include "app/libs/component_test.h"

void IceMachineT20ms(stIceMachine_Ram *iceMachineP)
{
    if (iceMachineP->values.stepTimer < 0xFFFF)
        iceMachineP->values.stepTimer++;
    
    if(iceMachineP->values.calibrate_IMch)
      iceMachineP->values.iceMachineLullabyCounter++;
}

void IceMachineT100ms(stIceMachine_Ram *iceMachineP)
{
    if (iceMachineP->values.waterApplied == _WaterSourceValue_PUMP)
    {
        if (iceMachineP->values.waterPumpTimer)
            iceMachineP->values.waterPumpTimer--;
    }

    if (iceMachineP->values.waterApplied == _WaterSourceValue_VACUM)
    {
        if (iceMachineP->values.waterVacumTimer)
            iceMachineP->values.waterVacumTimer--;
    }

}

void IceMachineT1sec(stIceMachine_Ram *iceMachineP)
{
    if (iceMachineP->values.caseTactSWErrorTimer)
        iceMachineP->values.caseTactSWErrorTimer--;

    if (iceMachineP->values.checkWaterTimer)
        iceMachineP->values.checkWaterTimer--;

    if (iceMachineP->values.checkWaterTempTimer)
        iceMachineP->values.checkWaterTempTimer--;
    
    if(iceMachineP->values.state == _IMch_State_FillWater_IsWaterNeeded &&
       !iceMachineP->values.firstIce &&
         !iceMachineP->sets.iceOFF && 
           iceMachineP->values.ConfirmIceNormalOrQuick60ModeTime < 0xFF)
      iceMachineP->values.ConfirmIceNormalOrQuick60ModeTime++;
}

void IceMachineT1min(stIceMachine_Ram *iceMachineP)
{
    if (iceMachineP->values.caseTactSWErrorReTryTimer)
        iceMachineP->values.caseTactSWErrorReTryTimer--;
    if (iceMachineP->values.makeIceTimer)
        iceMachineP->values.makeIceTimer--;
    if (iceMachineP->values.checkBoxTimer)
        iceMachineP->values.checkBoxTimer--;
    if (iceMachineP->values.QuickIce60 == true)         //<<QuickIce60
    {
        if (iceMachineP->values.quickIce60MaximumTimer)
            iceMachineP->values.quickIce60MaximumTimer--;
    }
}

void IceMachineTask(const tsIcemachineProcConsts* consts,  tsIcemachineProcParams* params)
{
    const stIceMachine_Flash *iceMachineC = consts->icemachine;
    stIceMachine_Ram *iceMachineP = params->icemachine;
    
    switch (iceMachineC->type)
    {
        case _IceMachineType_DEFAULT:
            //IceMachineType_DEFAULT(iceMachineC, iceMachineP);
          if(refrigerator_R.ui.states.commercial.status == _UI_Commercial_None)
            IceMachineType_DEFAULT(consts, params);
          else
            IceMachineType_Commercial_DEFAULT(iceMachineC, iceMachineP, consts, params);
        break;

        case _IceMachineType_NONE:
        default:
            iceMachineP->values.quickIce = false;
        break;
    }
    
    params->rf->WaterSource.values.iceMachineDesired = iceMachineP->values.waterDesired;
}


void RotateToSTART(stIceMachine_Ram *iceMachineP)
{
    if (iceMachineP->values.caseTactSWErrorReTryTimer)
    {
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
        return;
    }

    if (iceMachineP->values.block)
    {
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
        if (iceMachineP->values.stepTimer >= _SEC_T20ms(1))
        {
            iceMachineP->values.block = false;
            iceMachineP->values.stepTimer = 0;
        }
        return;
    }

    switch (iceMachineP->values.step)
    {
        case _IMch_Step_6:
        case _IMch_Step_5:
            iceMachineP->components.caseRotator.value = _RotatorValue_CCW;
            if (iceMachineP->components.caseTactSwitch.value == _CaseTactSW_ON)
            {
                if (iceMachineP->values.stepTimer >= _SEC_T20ms(3))
                {
                    iceMachineP->values.step = _IMch_Step_U;
                    RotateStop(iceMachineP);
                }
            }
            else
            {
                iceMachineP->values.step = _IMch_Step_4;
                iceMachineP->values.stepTimer = 0;
            }
        break;

        case _IMch_Step_4:
            iceMachineP->components.caseRotator.value = _RotatorValue_CCW;

            if (iceMachineP->components.caseTactSwitch.value == _CaseTactSW_ON)
            {
                iceMachineP->values.step = _IMch_Step_3;
                iceMachineP->values.stepTimer = 0;
            }
        break;

        case _IMch_Step_3:
            iceMachineP->components.caseRotator.value = _RotatorValue_CCW;
            if (iceMachineP->components.caseTactSwitch.value == _CaseTactSW_ON)
            {
                if (iceMachineP->values.stepTimer >= _SEC_T20ms(3))
                {
                    iceMachineP->values.step = _IMch_Step_U;
                    RotateStop(iceMachineP);
                }
            }
            else
            {
                iceMachineP->values.step = _IMch_Step_2;
                iceMachineP->values.stepTimer = 0;
            }
        break;

        case _IMch_Step_2:
            iceMachineP->components.caseRotator.value = _RotatorValue_CCW;
            if (iceMachineP->components.caseTactSwitch.value == _CaseTactSW_ON)
            {
                iceMachineP->values.step = _IMch_Step_1;
                iceMachineP->values.stepTimer = 0;
            }
        break;

        case _IMch_Step_1:
            iceMachineP->components.caseRotator.value = _RotatorValue_CCW;
            if (iceMachineP->values.stepTimer >= _SEC_T20ms(0.5))
            {
                iceMachineP->values.step = _IMch_Step_0;
                RotateStop(iceMachineP);
            }
        break;

        case _IMch_Step_0:
            RotateStop(iceMachineP);
        break;

        case _IMch_Step_U:
            iceMachineP->components.caseRotator.value = _RotatorValue_CW;
            if (iceMachineP->components.caseTactSwitch.value == _CaseTactSW_OFF)
            {
                iceMachineP->values.step = _IMch_Step_2;
                RotateStop(iceMachineP);
            }
        break;

        default:
            iceMachineP->values.step = _IMch_Step_6;
            RotateStop(iceMachineP);
        break;
    }
}

void RotateStop(stIceMachine_Ram *iceMachineP)
{
    if (!iceMachineP->values.block)
    {
        iceMachineP->values.block = true;
        iceMachineP->values.stepTimer = 0;
    }

    iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
}

void RotateToEND(stIceMachine_Ram *iceMachineP)
{
    if (iceMachineP->values.caseTactSWErrorReTryTimer)
    {
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
        return;
    }

    if (iceMachineP->values.block)
    {
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
        if (iceMachineP->values.stepTimer >= _SEC_T20ms(1))
        {
            iceMachineP->values.stepTimer = 0;
            iceMachineP->values.block = false;
        }
        return;
    }

    switch (iceMachineP->values.step)
    {
        case _IMch_Step_U:
        case _IMch_Step_0:
        case _IMch_Step_1:
            iceMachineP->components.caseRotator.value = _RotatorValue_CW;
            if (iceMachineP->components.caseTactSwitch.value == _CaseTactSW_ON)
            {
                if (iceMachineP->values.stepTimer >= _SEC_T20ms(3))
                {
                    iceMachineP->values.step = _IMch_Step_6;
                    RotateStop(iceMachineP);
                }
            }
            else
            {
                iceMachineP->values.step = _IMch_Step_2;
                iceMachineP->values.stepTimer = 0;
            }
        break;

        case _IMch_Step_2:
            iceMachineP->components.caseRotator.value = _RotatorValue_CW;
            if (iceMachineP->components.caseTactSwitch.value == _CaseTactSW_OFF)
            {
                if (iceMachineP->values.stepTimer >= _SEC_T20ms(7))
                {
                    iceMachineP->values.step = _IMch_Step_4;
                    iceMachineP->values.stepTimer = 0;
                }
            }
            else
            {
                iceMachineP->values.step = _IMch_Step_3;
                iceMachineP->values.stepTimer = 0;
            }
        break;

        case _IMch_Step_3:
            iceMachineP->components.caseRotator.value = _RotatorValue_CW;

            if (iceMachineP->components.caseTactSwitch.value == _CaseTactSW_ON)
            {
                if (iceMachineP->values.stepTimer >= _SEC_T20ms(3))
                {
                    iceMachineP->values.step = _IMch_Step_6;
                    RotateStop(iceMachineP);
                }
            }
            else
            {
                iceMachineP->values.step = _IMch_Step_4;
                iceMachineP->values.stepTimer = 0;
            }
        break;

        case _IMch_Step_4:
            iceMachineP->components.caseRotator.value = _RotatorValue_CW;
            if (iceMachineP->components.caseTactSwitch.value == _CaseTactSW_ON)
            {
                iceMachineP->values.step = _IMch_Step_5;
                iceMachineP->values.stepTimer = 0;
            }
        break;

        case _IMch_Step_5:
            iceMachineP->components.caseRotator.value = _RotatorValue_CW;
            if (iceMachineP->values.stepTimer >= _SEC_T20ms(0.5))
            {
                iceMachineP->values.step = _IMch_Step_6;
                RotateStop(iceMachineP);
            }
        break;

        case _IMch_Step_6:
            RotateStop(iceMachineP);
        break;

        default:
            iceMachineP->values.step = _IMch_Step_U;
            RotateStop(iceMachineP);
        break;
    }
}

void TactSW_Tester(stIceMachine_Ram *iceMachineP)
{
    if (iceMachineP->components.caseRotator.value == _RotatorValue_OFF)
    { // motor is stopped?
        iceMachineP->values.caseTactSWErrorTimer = _CaseTactSWErrorTime;

        if (!iceMachineP->values.caseTactSWErrorReTryTimer)
            iceMachineP->values.errCaseTactSW = false;
    }

    if (!iceMachineP->values.caseTactSWErrorTimer)
    {
        iceMachineP->values.errCaseTactSW = true;
        iceMachineP->values.caseTactSWErrorReTryTimer = _CaseTactSWErrorReTryTime;
    }

    if (iceMachineP->values.caseTactSWErrorReTryTimer)
    {
        iceMachineP->values.caseTactSWErrorTimer = _CaseTactSWErrorTime;
    }
}

void IceMachineType_DEFAULT(const tsIcemachineProcConsts* consts, tsIcemachineProcParams* params)
{

  const stIceMachine_Flash *iceMachineC = consts->icemachine;
  stIceMachine_Ram *iceMachineP = params->icemachine;
  
    bool quickIceCheck = false;

    if((iceMachineP->values.QuickIce60 == true) && (!iceMachineP->values.quickIce60MaximumTimer))
        iceMachineP->values.QuickIce60 = false;  //<<QuickIce60
    if (iceMachineP->values.sabbathEnabled ||
        iceMachineP->components.caseTempSensor.error)
        iceMachineP->values.state = _IMch_State_Error;
    
    /******************************************************************************************
    * Keep status iceOFFToON before start switch case
    * For select the waterPump delay time
    ******************************************************************************************/
    if(iceMachineP->sets.iceOFF && !refrigerator_R.WaterSource.values.ProhibitionWaterTankEmpty)
    {
      iceMachineP->sets.iceOFFToON = false;
      iceMachineP->values.ConfirmIceNormalOrQuick60ModeTime = 0;
      
      if(refrigerator_R.WaterSource.values.ProhibitPumpAfterDoorClearEmptyTimer >= 
          refrigerator_F.WaterSource.parameters.waterVentilHeater.ProhibitPumpAfterDoorClearEmptyTime)
      {
        iceMachineP->values.powerOn = false;
      }
    }
    else if(!iceMachineP->sets.iceOFF && !refrigerator_R.WaterSource.values.ProhibitionWaterTankEmpty)
    {
      if(iceMachineP->values.powerOn)
        iceMachineP->sets.iceOFFToON = false;
      else
        iceMachineP->sets.iceOFFToON = true;
    }
    else if(iceMachineP->sets.iceOFF && refrigerator_R.WaterSource.values.ProhibitionWaterTankEmpty)
    {
      iceMachineP->values.ConfirmIceNormalOrQuick60ModeTime = 0;
      
      if(refrigerator_R.WaterSource.values.ProhibitPumpAfterDoorClearEmptyTimer >= 
          refrigerator_F.WaterSource.parameters.waterVentilHeater.ProhibitPumpAfterDoorClearEmptyTime)
      {
        refrigerator_R.WaterSource.values.ProhibitionWaterTankEmpty = false;
      }
    }
    /******************************************************************************************
    * Keep status iceOFFToON before start switch case
    * For select the waterPump delay time
    ******************************************************************************************/
    
    switch (iceMachineP->values.state)
    {
        case _IMch_State_InitCase:
            RotateToSTART(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if (iceMachineP->values.step == _IMch_Step_0)
            {
                if (iceMachineP->values.pureCounter)
                {
                    iceMachineP->values.state = _IMch_State_CheckBox;
                }
                else
                {
                    iceMachineP->values.state = _IMch_State_FillWater_IsWaterNeeded;
                }
            }
            quickIceCheck = true;
        break;

        case _IMch_State_FillWater_IsWaterNeeded:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
            if(iceMachineP->values.boxFullActive)
              iceMachineP->values.boxFullActive = false;
            if (iceMachineP->values.firstIce)
            {
              //params->rf->WaterSource.values.heaterTimer = _waterVentilHeaterON;
                if (iceMachineP->components.caseTempSensor.valueInCExt > -50)                   //Cold start check
                  iceMachineP->values.state = _IMch_State_MakeIce_LongWait_Init;
                else
                  iceMachineP->values.state = _IMch_State_MakeIce_ColdStartWait_Init;
                
                iceMachineP->values.noWaterSet = false;
                //iceMachineP->values.firstIce = false;
            }
            else
            {
                if (iceMachineP->sets.iceOFF)
                    iceMachineP->values.state = _IMch_State_IceOff;
                else
                {
                  if(iceMachineP->values.ConfirmIceNormalOrQuick60ModeTime >= 10)
                  {
                    //Priority 1
                    if(refrigerator_R.WaterSource.values.ProhibitPumpAfterDoorClearEmptyTimer >= 
                          refrigerator_F.WaterSource.parameters.waterVentilHeater.ProhibitPumpAfterDoorClearEmptyTime)
                    {
                      //Priority 1 : When Power ON and clear tank empty the pump should be delay (ProhibitPumpAfterDoorClearEmptyTime)
                      if(params->rf->IceMachine.sets.iceOFFToON == false)
                      {
                        iceMachineP->values.powerOn = false;
                        params->rf->IceMachine.sets.iceOFFToON = true;
                        refrigerator_R.WaterSource.values.ProhibitionWaterTankEmpty = false;
                        iceMachineP->values.ConfirmIceNormalOrQuick60ModeTime = 0;
                        iceMachineP->values.state = _IMch_State_FillWater_PumpingWater_Init;
                      }
                      
                      //Priority 2 : When IceOFF to IceON, and Others the pump should be delay (ProhibitPumpWhenIceOnFirstOnTime)
                      else if(params->rf->IceMachine.sets.iceOFFToON == true && 
                                (refrigerator_R.WaterSource.values.ProhibitPumpWhenIceOnFirstOnTimer >= 
                                  refrigerator_F.WaterSource.parameters.waterVentilHeater.ProhibitPumpWhenIceOnFirstOnTime))
                      {
                        iceMachineP->values.powerOn = false;
                        iceMachineP->values.ConfirmIceNormalOrQuick60ModeTime = 0;
                        iceMachineP->values.state = _IMch_State_FillWater_PumpingWater_Init;
                      }
                    }
                  }
                }
            }
            quickIceCheck = true;
        break;

        case _IMch_State_FillWater_PumpingWater_Init:    
            if(iceMachineP->values.QuickIce60)                                                          //<<QuickIce60
            {
              iceMachineP->values.waterPumpTimer = (iceMachineP->sets.quickIce60WaterPumpTime); // second to *20ms //<<QuickIce60 
              iceMachineP->values.SelectCalibrate_quickIce60 = true;
            }
            else
            {
              iceMachineP->values.waterPumpTimer = (iceMachineP->sets.waterPumpTime); // second to *20ms
              iceMachineP->values.SelectCalibrate_quickIce60 = false;
            }
                
            iceMachineP->values.state = _IMch_State_FillWater_PumpingWater;
        case _IMch_State_FillWater_PumpingWater:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_PUMP;

            if (!iceMachineP->values.waterPumpTimer)
                iceMachineP->values.state = _IMch_State_FillWater_VacumWater_Init;
            quickIceCheck = true;
        break;

        case _IMch_State_FillWater_VacumWater_Init:
            iceMachineP->values.waterVacumTimer = _WaterVacumTime;
            iceMachineP->values.state = _IMch_State_FillWater_VacumWater;
        case _IMch_State_FillWater_VacumWater:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_VACUM;

            if (!iceMachineP->values.waterVacumTimer)
            {
                if (iceMachineC->parameters.checkWaterEnabled)
                {
                    iceMachineP->values.state = _IMch_State_CheckWater_Init;
                }
                else
                {
                    iceMachineP->values.state = _IMch_State_MakeIce_LongWait_Init;
                    iceMachineP->values.noWaterSet = false;
                }
            }
            quickIceCheck = true;
        break;

        case _IMch_State_CheckWater_Init:
            iceMachineP->values.checkWaterTimer = iceMachineC->parameters.checkWaterTime;
            iceMachineP->values.checkWaterTempTimer = iceMachineC->parameters.checkWaterTempTime;
            iceMachineP->values.state = _IMch_State_CheckWater_IsThereWater;
        case _IMch_State_CheckWater_IsThereWater:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if (iceMachineP->values.checkWaterTimer)
            {
                if (iceMachineP->components.caseTempSensor.valueInCExt >= iceMachineC->parameters.checkWaterTemp)
                {
                    if (!iceMachineP->values.checkWaterTempTimer)
                    {
                        // there is water continue
                        iceMachineP->values.state = _IMch_State_MakeIce_LongWait_Init;
                        iceMachineP->values.noWaterSet = false;
                    }
                }
                else
                {
                    iceMachineP->values.checkWaterTempTimer = iceMachineC->parameters.checkWaterTempTime;
                }
            }
            else
            {
                // give water alert....
                iceMachineP->values.noWaterSet = true;
                iceMachineP->values.state = _IMch_State_NoWater;
            }
            quickIceCheck = true;
        break;

        case _IMch_State_NoWater:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if (iceMachineP->values.noWaterClr)
            {
                iceMachineP->values.state = _IMch_State_FillWater_IsWaterNeeded;
            }
            quickIceCheck = true;
        break;

        case _IMch_State_MakeIce_LongWait_Init:
          if(!iceMachineP->sets.iceOFF && 
             !params->rf->Cabin[0].components.innerLight.value && 
               !params->rf->Cabin[1].components.innerLight.value)
              iceMachineP->values.calibrate_IMch = true;
          
            if(iceMachineP->values.SelectCalibrate_quickIce60)
              iceMachineP->values.makeIceTimer = iceMachineC->parameters.quickIce60makeIceLongWaitTime;
            else
              iceMachineP->values.makeIceTimer = iceMachineC->parameters.makeIceLongWaitTime;
            
            iceMachineP->values.makeIceTimerBuffer = iceMachineP->values.makeIceTimer; //for water check
            iceMachineP->values.state = _IMch_State_MakeIce_LongWait;
            if( !iceMachineC->parameters.quickIceOnDisplay)
              iceMachineP->values.differential = iceMachineP->components.caseTempSensor.valueInCExt;
        case _IMch_State_MakeIce_LongWait:
            if(iceMachineP->values.calibrate_IMch)
              CalibrateWater ( iceMachineP );
            else
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if( !iceMachineC->parameters.quickIceOnDisplay && (iceMachineP->values.makeIceTimer  == (iceMachineP->values.makeIceTimerBuffer - 10)) )
            {
              if ( iceMachineP->components.caseTempSensor.valueInCExt > ( iceMachineP->values.differential + 50 ) ) //if ice tray get hotter, you mean intake water already
              {
                iceMachineP->values.quickIce = true;
                if(params->rf->Values.doorsClosedState == true )
                {
                  params->rf->Values.doorsClosedState = false;    
                  params->rf->Values.doorsClosedTimer = 120; 
                  params->rf->Values.doorsOpenedCounter = 0;
                  params->rf->Values.doorsClosedStateStayTimer = 0;
                }
              }
              else
              {
                iceMachineP->values.quickIce = false;
              }
              
            }
            else if(iceMachineC->parameters.quickIceOnDisplay)
            {
              iceMachineP->values.quickIce = true; // this is for pipe heater
            }
            if (!iceMachineP->values.makeIceTimer)
            {
                if(iceMachineP->values.firstIce)
                  iceMachineP->values.firstIce = false;
                
                if(iceMachineP->values.SelectCalibrate_quickIce60)
                {
                  if (iceMachineP->components.caseTempSensor.valueInCExt <=             //For normal QuickIce60
                      iceMachineC->parameters.quickIce60checkIceTempAtLongWait)         //For normal QuickIce60
                  {                                                                     //For normal QuickIce60
                    iceMachineP->values.state = _IMch_State_CheckBox;                   //For normal QuickIce60
                    iceMachineP->values.pureCounter = _PureCount;                       //For normal QuickIce60
                  }                                                                     //For normal QuickIce60
                  else                                                                  //For normal QuickIce60        
                    iceMachineP->values.state = _IMch_State_MakeIce_ShortWait_Init;     //For normal QuickIce60                  
                }
                else
                {
                  if (iceMachineP->components.caseTempSensor.valueInCExt <=             //For normal IceMaking
                      iceMachineC->parameters.checkIceTempAtLongWait)                   //For normal IceMaking        
                  {                                                                     //For normal IceMaking        
                    iceMachineP->values.state = _IMch_State_CheckBox;                   //For normal IceMaking
                    iceMachineP->values.pureCounter = _PureCount;                       //For normal IceMaking
                  }                                                                     //For normal IceMaking
                  else                                                                  //For normal IceMaking
                    iceMachineP->values.state = _IMch_State_MakeIce_ShortWait_Init;     //For normal IceMaking
                }                

            }
            quickIceCheck = true;
        break;

        case _IMch_State_MakeIce_ShortWait_Init:
            iceMachineP->values.makeIceTimer = iceMachineC->parameters.makeIceShortWaitTime;
            iceMachineP->values.state = _IMch_State_MakeIce_ShortWait;
        case _IMch_State_MakeIce_ShortWait:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if (!iceMachineP->values.makeIceTimer)
            {
              if(iceMachineP->values.SelectCalibrate_quickIce60)
              {
                if (iceMachineP->components.caseTempSensor.valueInCExt <=               //For normal QuickIce60
                    iceMachineC->parameters.quickIce60checkIceTempAtShortWait)          //For normal QuickIce60
                {                                                                       //For normal QuickIce60
                  iceMachineP->values.state = _IMch_State_CheckBox;                     //For normal QuickIce60
                  iceMachineP->values.pureCounter = _PureCount;                         //For normal QuickIce60                                
                }                                                                       //For normal QuickIce60
                else                                                                    //For normal QuickIce60
                  iceMachineP->values.state = _IMch_State_MakeIce_ShortWait_Init;       //For normal QuickIce60
              }
              else
              {
                if (iceMachineP->components.caseTempSensor.valueInCExt <=               //For normal IceMaking
                    iceMachineC->parameters.checkIceTempAtShortWait)                    //For normal IceMaking
                {                                                                       //For normal IceMaking
                  iceMachineP->values.state = _IMch_State_CheckBox;                     //For normal IceMaking
                  iceMachineP->values.pureCounter = _PureCount;                         //For normal IceMaking
                }                                                                       //For normal IceMaking
                else                                                                    //For normal IceMaking        
                  iceMachineP->values.state = _IMch_State_MakeIce_ShortWait_Init;       //For normal IceMaking
              }
            }
            quickIceCheck = true;
        break;
        
        case _IMch_State_MakeIce_ColdStartWait_Init:
            iceMachineP->values.makeIceTimer = 20;//iceMachineC->parameters.makeIceShortWaitTime;
            iceMachineP->values.state = _IMch_State_MakeIce_ColdStartWait;
        case _IMch_State_MakeIce_ColdStartWait:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if (iceMachineP->components.caseTempSensor.valueInCExt > -50)
              iceMachineP->values.state = _IMch_State_MakeIce_ColdStartWait_Init;

            if (!iceMachineP->values.makeIceTimer)
            {
              iceMachineP->values.firstIce = false;
              iceMachineP->values.state = _IMch_State_CheckBox;
              iceMachineP->values.pureCounter = _PureCount;
            }

            quickIceCheck = true;
        break;

        case _IMch_State_CheckBox:
            RotateToEND(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if (iceMachineP->components.boxTactSwitch.value == _BoxTactSW_ThereIsNot)
            {
                iceMachineP->values.state = _IMch_State_NoBox;
            }

            if (iceMachineP->values.step == _IMch_Step_3)
            {
                iceMachineP->values.checkBoxTimer = _CheckBoxTime;
                iceMachineP->values.state = _IMch_State_BoxFull;
            }

            if (iceMachineP->values.step == _IMch_Step_4)
            {
                iceMachineP->values.state = _IMch_State_PureIce;
            }
            quickIceCheck = true;
        break;

        case _IMch_State_NoBox:
            RotateToSTART(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if (iceMachineP->values.step == _IMch_Step_0)
            {
                if (iceMachineP->components.boxTactSwitch.value == _BoxTactSW_ThereIs)
                {
                    iceMachineP->values.state = _IMch_State_CheckBox;
                }
            }
            quickIceCheck = false;
            iceMachineP->values.quickIce = false;
        break;

        case _IMch_State_BoxFull:
            RotateToSTART(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
            iceMachineP->values.boxFullActive = true;
            iceMachineP->values.firstIce = true;
            if (iceMachineP->components.boxTactSwitch.value == _BoxTactSW_ThereIsNot)
            {
                iceMachineP->values.checkBoxTimer = 0;
            }

            if (iceMachineP->values.step == _IMch_Step_0)
            {
                if (!iceMachineP->values.checkBoxTimer)
                    iceMachineP->values.state = _IMch_State_CheckBox;
            }
            quickIceCheck = true;
            iceMachineP->values.quickIce = false;
        break;

        case _IMch_State_PureIce:
            RotateToEND(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if (iceMachineP->components.boxTactSwitch.value == _BoxTactSW_ThereIsNot)
            {
                iceMachineP->values.state = _IMch_State_NoBox;
            }

            if (iceMachineP->values.step == _IMch_Step_6)
            {
                iceMachineP->values.state = _IMch_State_InitCase;
                if (iceMachineP->values.pureCounter)
                    iceMachineP->values.pureCounter--;
                if(iceMachineP->values.QuickIce60)                      //<<QuickIce60
                {                                                       //<<QuickIce60
                    iceMachineP->values.quickIce60MaximumTimesCount--;         //<<QuickIce60
                    if(!iceMachineP->values.quickIce60MaximumTimesCount)
                      iceMachineP->values.QuickIce60 = false;
                }                                                       //<<QuickIce60         
            }
        break;

        case _IMch_State_IceOff:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;

            if (!iceMachineP->sets.iceOFF)
            {
                iceMachineP->values.state = _IMch_State_InitCase;
            }
            quickIceCheck = false;
            iceMachineP->values.quickIce = false;
        break;

        case _IMch_State_Error:
          
        default:
            if (!(iceMachineP->values.sabbathEnabled ||
                iceMachineP->components.caseTempSensor.error))
            {
                iceMachineP->values.firstIce = true;
                iceMachineP->values.state = _IMch_State_InitCase;
            }
        break;
    }

    TactSW_Tester(iceMachineP);

    iceMachineP->values.noWaterClr = false;

    QuickIceCheck(consts, params, quickIceCheck);   
}

/*** APW : Add Ice making operation test in CT ***/
void IceMachineType_Commercial_DEFAULT(stIceMachine_Flash const *iceMachineC, stIceMachine_Ram *iceMachineP,const tsIcemachineProcConsts* consts, tsIcemachineProcParams* params)
{
    if(iceMachineP->values.QuickIce60)
      iceMachineP->components.fan.speedOrPWM = 100;
    else
      iceMachineP->components.fan.speedOrPWM = 0;

    if (iceMachineP->values.sabbathEnabled ||
        iceMachineP->components.caseTempSensor.error)
        iceMachineP->values.state = _IMch_State_Error;

    switch (iceMachineP->values.state)
    {
        case _IMch_CommercialState_ReadyToTest:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
            
            if(iceMachineP->values.IceComponentTest || iceMachineP->values.QuickIce60)
              iceMachineP->values.state = _IMch_CommercialState_InitCase;
            
        break;
          
        case _IMch_CommercialState_InitCase:
            RotateToSTART(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
            
            if (iceMachineP->values.step == _IMch_Step_0)
              iceMachineP->values.state = _IMch_CommercialState_IceBoxToEndTest;
            
            params->rf->ui.dspPacket.funcs.ledAlert = false;
        break;

        case _IMch_CommercialState_IceBoxToEndTest:
            RotateToEND(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
            
            if(iceMachineP->values.step == _IMch_Step_3)
                iceMachineP->values.state = _IMch_CommercialState_IceBoxDetected;
            
            if(iceMachineP->values.step == _IMch_Step_6)
              iceMachineP->values.state = _IMch_CommercialState_IceBoxToStartTest;
        break;
        
        case _IMch_CommercialState_IceBoxDetected:
            RotateToSTART(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
            if (iceMachineP->values.step == _IMch_Step_0)
              iceMachineP->values.state = _IMch_CommercialState_TestComplete;
        break;
          
        case _IMch_CommercialState_IceBoxToStartTest:
            RotateToSTART(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
            
            if(iceMachineP->values.step == _IMch_Step_0)
            {
            //iceMachineP->values.waterPumpTimer = (iceMachineP->sets.waterPumpTime * 5); // second to *20ms
            iceMachineP->values.waterPumpTimer = 45;//(45 * 5);
            iceMachineP->values.state = _IMch_CommercialState_WaterSuplyForwardTest;
            }
        break;
        
        case _IMch_CommercialState_WaterSuplyForwardTest:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_PUMP;

            if (!iceMachineP->values.waterPumpTimer)
            {
              iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
              iceMachineP->values.waterVacumTimer = _WaterVacumTime;
              iceMachineP->values.state = _IMch_CommercialState_WaterSuplyReverseTest;
            }     
        break;

        case _IMch_CommercialState_WaterSuplyReverseTest:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_VACUM;

            if (!iceMachineP->values.waterVacumTimer)
               iceMachineP->values.state = _IMch_CommercialState_TestComplete;
        break;
               
        case _IMch_CommercialState_GoToOriginalBeforeEndCT:
            RotateToSTART(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
            
            if (iceMachineP->values.step == _IMch_Step_0)
              iceMachineP->values.state = _IMch_CommercialState_TestComplete;
        break;
        
        case _IMch_CommercialState_TestComplete:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
              //Reset mode when completed
              iceMachineP->values.IceComponentTest = false;     //HOSHI
              iceMachineP->values.QuickIce60 = false;           //BONA, ALPS
              
              sysBuzzerCmd(eSysBuzzerPlayEndMelody);
              params->rf->ui.dspPacket.funcs.ledAlert = true;
              iceMachineP->values.state = _IMch_CommercialState_ReadyToTest;
        break;
        
        case _IMch_CommercialDefrostState_IceBoxNotOperate:
            RotateStop(iceMachineP);
            iceMachineP->values.waterDesired = _WaterSourceValue_OFF;
        break;
          
        default:
            if (!(iceMachineP->values.sabbathEnabled ||
                iceMachineP->components.caseTempSensor.error))
            {
                iceMachineP->values.firstIce = true;
                iceMachineP->values.state = _IMch_State_InitCase;
            }
        break;
    }

    TactSW_Tester(iceMachineP);

    iceMachineP->values.noWaterClr = false;
    
    if(params->rf->ui.states.commercial.status != _UI_Commercial_None && 
        iceMachineP->values.state != _IMch_CommercialState_TestComplete && 
          iceMachineP->values.state != _IMch_CommercialState_ReadyToTest &&
            iceMachineP->values.state != _IMch_CommercialDefrostState_IceBoxNotOperate)
    {
      if(iceMachineP->values.LEDIceBlinkInCt_20ms < 0xFF)
        iceMachineP->values.LEDIceBlinkInCt_20ms++;
      else
        iceMachineP->values.LEDIceBlinkInCt_20ms = 0xFF;
      
      if(iceMachineP->values.LEDIceBlinkInCt_20ms >= 20)
      {
        params->rf->ui.dspPacket.funcs.ledIceOFF ^= 1;
        params->rf->ui.dspPacket.funcs.ledIceOn ^= 1;
        iceMachineP->values.LEDIceBlinkInCt_20ms = 0;
      }
    }
    else
      iceMachineP->values.LEDIceBlinkInCt_20ms = 0xFF;
    
    if((params->rf->ui.states.commercial.FlagCommercialDisableTimer >= 
        (consts->rf->Parameters.commercialAutoClearEEPROMMinTimer - 5)) &&
          iceMachineP->values.state != _IMch_CommercialState_TestComplete &&
            iceMachineP->values.state != _IMch_CommercialState_ReadyToTest)
              iceMachineP->values.state = _IMch_CommercialState_GoToOriginalBeforeEndCT;
}
/*** APW : Add Ice making operation test in CT ***/
void QuickIceCheck(const tsIcemachineProcConsts* consts,  tsIcemachineProcParams* params, bool quickIceCheck)
{
  const stIceMachine_Flash *iceMachineC = consts->icemachine;
  stIceMachine_Ram *iceMachineP = params->icemachine;
  
  if(iceMachineC->parameters.quickIceOnDisplay)
  {
//    if(iceMachineP->values.quickIce)
//    refrigerator_R.WaterSource.values.heaterTimer = _waterVentilHeaterON;
    
    if(iceMachineP->values.QuickIce60 && 
       !(iceMachineP->values.state == _IMch_State_IceOff) && 
       !(iceMachineP->values.state == _IMch_State_BoxFull) && 
       !(iceMachineP->values.state == _IMch_State_NoBox))
      iceMachineP->components.fan.speedOrPWM = 100;
    else
      iceMachineP->components.fan.speedOrPWM = 0;
    
    if(params->rf->Cabin[0].components.doorSwitch[0].value ||
       params->rf->Cabin[0].components.doorSwitch[1].value)
      iceMachineP->components.fan.speedOrPWM = 0;
    
    if(params->rf->Cabin[1].components.doorSwitch[0].value ||
       params->rf->Cabin[1].components.doorSwitch[1].value)
      iceMachineP->components.fan.speedOrPWM = 0;

  }
  else
  {
    if( quickIceCheck && iceMachineP->values.state != _IMch_State_IceOff )
    {
      if( iceMachineP->components.caseTempSensor.valueInCExt >= 
         iceMachineC->parameters.quickCutInTemp )
        iceMachineP->values.quickIce = true;
      if( iceMachineP->components.caseTempSensor.valueInCExt < 
         iceMachineC->parameters.quickCutOutTemp )
        iceMachineP->values.quickIce = false;
    }
    else
    {
      iceMachineP->values.quickIce = false;
    }
    
    if(!iceMachineP->sets.iceOFF && iceMachineP->values.quickIce)
    {
      iceMachineP->components.fan.speedOrPWM = 100;
      //params->rf->WaterSource.values.heaterTimer = _waterVentilHeaterON; // Timer On cycle i?erisine setlenir.
      
    if(params->rf->Cabin[0].components.doorSwitch[0].value ||
       params->rf->Cabin[0].components.doorSwitch[1].value)
      iceMachineP->components.fan.speedOrPWM = 0;
    
    if(params->rf->Cabin[1].components.doorSwitch[0].value ||
       params->rf->Cabin[1].components.doorSwitch[1].value)
      iceMachineP->components.fan.speedOrPWM = 0;
    }
    else 
    {
      iceMachineP->components.fan.speedOrPWM = 0;
    }
  }
  
  if(iceMachineP->values.firstIce)
    iceMachineP->components.fan.speedOrPWM = 0;
  
}
void IceMachineComponentsTest(const tsIcemachineProcConsts* consts,  tsIcemachineProcParams* params)
{
    const stIceMachine_Flash *iceMachineC = consts->icemachine;
    stIceMachine_Ram *iceMachineP = params->icemachine;
    tsIceMachineProcDevIos *devs = &params->dev;
    const stIceMachineComponents_Flash *compC = &consts->icemachine->components;
    stIceMachineComponents_Ram *compP = &params->icemachine->components;
    
    uint32_t tt = params->rf->workMode.values.prodTest.componentTestTimer100Ms;
    
    bool done = true;
       
    IceMachineCloseComponents(iceMachineP);
    if (iceMachineC->type)
    {
      done &= compTest(devs->caseRotator, &compP->caseRotator, &compC->caseRotator.test.cw, tt, _RotatorValue_CW);
      done &= compTest(devs->caseRotator, &compP->caseRotator, &compC->caseRotator.test.ccw, tt, _RotatorValue_CCW);
      done &= compTest(devs->fan, &compP->fan, &compC->fan.test, tt, 100);
    }
    iceMachineP->values.componentsTestDone = done;
}

void IceMachineCloseComponents(stIceMachine_Ram *iceMachineP)
{
  iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
  iceMachineP->components.fan.speedOrPWM = 0;
}

/*bool isIceMachineComponentsTestDone(stIceMachine_Flash const *consts, tsIcemachineProcParams *params, unsigned int testTimer)
{
    if (consts->type)
    {
        if (!isRotatorTestCWDone(&consts->components.caseRotator, testTimer))
            return false;
        if (!isRotatorTestCCWDone(&consts->components.caseRotator, testTimer))
            return false;
        if (!isFanTestDone(&consts->components.fan, testTimer))
            return false;
   }
    return true;
}*/
void CalibrateWater(stIceMachine_Ram *iceMachineP ){
  //Quick ice making
  if(iceMachineP->values.SelectCalibrate_quickIce60)          
  {
      if (iceMachineP->values.iceMachineLullabyCounter < 3000) // 60 sn off 
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
      else if(iceMachineP->values.iceMachineLullabyCounter < 3100) // 2second right 
        iceMachineP->components.caseRotator.value = _RotatorValue_CW;
      else if(iceMachineP->values.iceMachineLullabyCounter < 3350) // 5second wait 
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
      else if (iceMachineP->values.iceMachineLullabyCounter < 3475) //2.5 second left 
        iceMachineP->components.caseRotator.value = _RotatorValue_CCW;
      else if(iceMachineP->values.iceMachineLullabyCounter < 3575) // 2second wait 
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
      else if (iceMachineP->values.iceMachineLullabyCounter < 3600) // 0.5 second right 
        iceMachineP->components.caseRotator.value = _RotatorValue_CW;
      else if (iceMachineP->values.iceMachineLullabyCounter < 3650) // 1second wait 
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
      else
      { 
        iceMachineP->values.calibrate_IMch = false;
        iceMachineP->values.iceMachineLullabyCounter = 0;     
      }
  }
  //Normal ice making
  else                                          
  { 
      if (iceMachineP->values.iceMachineLullabyCounter < 3000) // 60 sn off 
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
      else if(iceMachineP->values.iceMachineLullabyCounter < 3050) // 1second right 
        iceMachineP->components.caseRotator.value = _RotatorValue_CW;
      else if(iceMachineP->values.iceMachineLullabyCounter < 3100) // 1second wait 
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
      else if (iceMachineP->values.iceMachineLullabyCounter < 3175) //1.5 second left 
        iceMachineP->components.caseRotator.value = _RotatorValue_CCW;
      else if(iceMachineP->values.iceMachineLullabyCounter < 3225) // 1second wait 
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
      else if (iceMachineP->values.iceMachineLullabyCounter < 3250) // 0.5 second right 
        iceMachineP->components.caseRotator.value = _RotatorValue_CW;
      else if (iceMachineP->values.iceMachineLullabyCounter < 3300) // 1second wait 
        iceMachineP->components.caseRotator.value = _RotatorValue_OFF;
      else
      { 
        iceMachineP->values.calibrate_IMch = false;
        iceMachineP->values.iceMachineLullabyCounter = 0;     
      }
  }
}

void IceMachineInitAfterHotStart(stIceMachine_Ram *iceMachineP)
{
    IceMachineE2AfterHotStart(iceMachineP);
    IceMachineE2ToRam(iceMachineP);
}

void IceMachineClearDisplaySetting20MsTask(stIceMachine_Ram *iceMachineP)
{
    IceMachineE2ClearDisplaySetting(iceMachineP);
}

void IceMachineInitAfterService(stIceMachine_Ram *iceMachineP)
{
    IceMachineE2AfterService(iceMachineP);
    IceMachineE2ToRam(iceMachineP);
}

void IceMachineE2Init(const stIceMachine_Flash *iceMachineC, stIceMachine_Ram *iceMachineP)
{
  iceMachineP->E2.iceOFF = true;
  iceMachineP->E2.QuickIce60 = false;
  iceMachineP->E2.waterPumpTime = iceMachineC->parameters.waterPumpTime;
}

void IceMachineE2AfterHotStart(stIceMachine_Ram *iceMachineP)
{
  //iceMachineP->E2.iceOFF = false;     //APW: Pohibition enable because have reset other process
}

void IceMachineE2ClearDisplaySetting(stIceMachine_Ram *iceMachineP)
{
  iceMachineP->E2.iceOFF = true;
  iceMachineP->E2.QuickIce60 = false;
  iceMachineP->E2.waterPumpTime = refrigerator_F.IceMachine.parameters.waterPumpTime;
}

void IceMachineE2AfterService(stIceMachine_Ram *iceMachineP)
{
 // iceMachineP->E2.iceOFF = false;
}

void IceMachineE2ToRam(stIceMachine_Ram *iceMachineP)
{
  iceMachineP->sets.iceOFF = iceMachineP->E2.iceOFF;
  iceMachineP->values.QuickIce60 = iceMachineP->E2.QuickIce60;
  iceMachineP->sets.waterPumpTime = iceMachineP->E2.waterPumpTime;
}




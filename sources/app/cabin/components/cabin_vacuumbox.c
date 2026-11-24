#include "cabin_vacuumbox.h"


#define _Prohibited_High_Temp_Value 450       // 45 *C

void cabinVacuumBoxInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    cabinP->values.VacuumBox.state        = _VacuumState_OFF;
    cabinP->values.VacuumBox.offTimer     = cabinC->parameters.VacuumBox.VacuumBoxFirstOFFTime;
    cabinP->values.VacuumBox.onTimer      = 0;
    cabinP->components.VacuumBox.value    = _VacuumValue_OFF;
    cabinP->values.VacuumBox.HeaterTimer  = 0;
    cabinP->values.VacuumBox.delayTimer   = 0;

}

void cabinVacuumBox1SecTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    if (cabinP->components.VacuumBox.value == _VacuumValue_ON &&
        cabinP->values.VacuumBox.onTimer)
        cabinP->values.VacuumBox.onTimer--;
    
    if(cabinP->values.VacuumBox.HeaterTimer)
      cabinP->values.VacuumBox.HeaterTimer--;
    
    if(cabinP->values.VacuumBox.delayTimer)
      cabinP->values.VacuumBox.delayTimer--;

}

void cabinVacuumBox1MinTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    if ((cabinP->values.VacuumBox.state == _VacuumState_OFF || cabinP->values.VacuumBox.state == _VacuumState_Delay || cabinP->values.VacuumBox.state == _VacuumState_Prohibited) 
        && cabinP->values.VacuumBox.offTimer)
        cabinP->values.VacuumBox.offTimer--;
}

void cabinVacuumBoxHeater(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    unsigned short OnTime;
    unsigned short OffTime;
 
    enAmbientType ambient = cabinP->values.ambientType;
    
    switch(cabinP->values.VacuumBox.airTempState)
    {
      case 0:   //Area 0
        if(cabinP->components.airTempSensor.valueInCExt >= cabinC->parameters.VacuumBox.VacuumBoxHeater.TemperaturesZone[0])
          cabinP->values.VacuumBox.airTempState = 1;
          
      break;
      
      case 1:   //Area 1
        if(cabinP->components.airTempSensor.valueInCExt < cabinC->parameters.VacuumBox.VacuumBoxHeater.TemperaturesZone[0] - 1)
          cabinP->values.VacuumBox.airTempState = 0;
        else if(cabinP->components.airTempSensor.valueInCExt >= cabinC->parameters.VacuumBox.VacuumBoxHeater.TemperaturesZone[1])
          cabinP->values.VacuumBox.airTempState = 2;
        
      break;  
      
      case 2:   //Area 2
        if(cabinP->components.airTempSensor.valueInCExt < cabinC->parameters.VacuumBox.VacuumBoxHeater.TemperaturesZone[1] - 1)
          cabinP->values.VacuumBox.airTempState = 1;
        else if(cabinP->components.airTempSensor.valueInCExt >= cabinC->parameters.VacuumBox.VacuumBoxHeater.TemperaturesZone[2])
          cabinP->values.VacuumBox.airTempState = 3;
        
      break;
      
      case 3:   //Area 3
        if(cabinP->components.airTempSensor.valueInCExt < cabinC->parameters.VacuumBox.VacuumBoxHeater.TemperaturesZone[2] - 1)
          cabinP->values.VacuumBox.airTempState = 2;
        else if(cabinP->components.airTempSensor.valueInCExt >= cabinC->parameters.VacuumBox.VacuumBoxHeater.TemperaturesZone[3])
          cabinP->values.VacuumBox.airTempState = 4;
        
      break;
      
      case 4:   //Area 4
        if(cabinP->components.airTempSensor.valueInCExt < cabinC->parameters.VacuumBox.VacuumBoxHeater.TemperaturesZone[3] - 1)
          cabinP->values.VacuumBox.airTempState = 3;

      break;
      
      default:
          cabinP->values.VacuumBox.airTempState = 0;
      break;
    }

    OnTime = cabinC->parameters.VacuumBox.VacuumBoxHeater.OnTime[ambient][cabinP->values.VacuumBox.airTempState];
    OffTime = cabinC->parameters.VacuumBox.VacuumBoxHeater.OffTime[ambient][cabinP->values.VacuumBox.airTempState];
    
    if(cabinP->values.VacuumBox.sets == _VacuumSet_One)
    {
      if (!cabinP->values.VacuumBox.HeaterTimer)
        cabinP->values.VacuumBox.HeaterTimer = OnTime + OffTime;
      
      if (cabinP->values.VacuumBox.HeaterTimer > OffTime)
        cabinP->components.VacuumBoxHeater.value = _HeaterValue_ON;
      else
        cabinP->components.VacuumBoxHeater.value = _HeaterValue_OFF;

      
    }
    else
    {
      cabinP->components.VacuumBoxHeater.value = _HeaterValue_OFF;
      cabinP->values.VacuumBox.HeaterTimer = 0;
    }
          

    
}
/*** APW : Add heater operation in CT ***/
void cabinVacuumBoxHeater_CommercialHeaterTest(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);
          
    if(refrigerator_R.ui.states.commercial.status == _UI_Commercial_Test)
        cabinP->components.VacuumBoxHeater.value = _HeaterValue_OFF;
      
    if(refrigerator_R.ui.states.commercial.status == _UI_Commercial_DefrostTest)
    {       
       if(cabinC->components.vacuumBoxHeater.commercialDefrostTestPriority == 0)
            cabinP->components.VacuumBoxHeater.value = _HeaterValue_OFF;
       else if(cabinC->components.vacuumBoxHeater.commercialDefrostTestPriority == 1 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_DefrostOnTest)
            cabinP->components.VacuumBoxHeater.value = _HeaterValue_ON;
       else if(cabinC->components.vacuumBoxHeater.commercialDefrostTestPriority == 2 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater1OnTest)
            cabinP->components.VacuumBoxHeater.value = _HeaterValue_ON;
       else if(cabinC->components.vacuumBoxHeater.commercialDefrostTestPriority == 3 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater2OnTest)
            cabinP->components.VacuumBoxHeater.value = _HeaterValue_ON;
       else if(cabinC->components.vacuumBoxHeater.commercialDefrostTestPriority == 4 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater3OnTest)
            cabinP->components.VacuumBoxHeater.value = _HeaterValue_ON;
       else if(cabinC->components.vacuumBoxHeater.commercialDefrostTestPriority == 5 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater4OnTest)
            cabinP->components.VacuumBoxHeater.value = _HeaterValue_ON;
       else if(cabinC->components.vacuumBoxHeater.commercialDefrostTestPriority == 6 &&
               params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_Heater5OnTest)
            cabinP->components.VacuumBoxHeater.value = _HeaterValue_ON;
       else
            cabinP->components.VacuumBoxHeater.value = _HeaterValue_OFF;
    }
}
/*** APW : Add heater operation in CT ***/
void cabinVacuumBoxTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);
    enVacuumBoxValues VacuumBox = _VacuumValue_OFF;
    
    if(cabinP->values.VacuumBox.sets == _VacuumSet_Off)
      cabinP->values.VacuumBox.state = _VacuumState_OFF;
    
    //cabinVacuumBoxHeater(consts,params);
    /*** APW : Add heater operation in CT ***/
    if(refrigerator_R.ui.states.commercial.status != _UI_Commercial_None)
      cabinVacuumBoxHeater_CommercialHeaterTest(consts, params);
    else
      cabinVacuumBoxHeater(consts,params);
    /*** APW : Add heater operation in CT ***/
    switch (cabinP->values.VacuumBox.state)
    {
      case _VacuumState_OFF:
        if(cabinP->values.VacuumBox.sets != _VacuumSet_Off)
        {
          if (!cabinP->values.VacuumBox.offTimer)
          {
            cabinP->values.VacuumBox.state = _VacuumState_ON;
            cabinP->values.VacuumBox.onTimer = cabinC->parameters.VacuumBox.VacuumBox_ONTime1;
          } 
          else
            cabinP->values.VacuumBox.state = _VacuumState_OFF;
          
          if (cabinP->values.doorOpenedTimer)
          {
            cabinP->values.VacuumBox.state = _VacuumState_Delay;
            cabinP->values.VacuumBox.delayTimer = cabinC->parameters.VacuumBox.VacuumBox_ONDelayTime;
          }
          
//          if(cabinP->components.vacuumBoxOverCurrentSensor.valueInCExt > cabinC->parameters.VacuumBox.VacuumBoxCurrentSensor.JudgmentLockA 
//             && cabinP->components.vacuumBoxOverCurrentSensor.valueInCExt < cabinC->parameters.VacuumBox.VacuumBoxCurrentSensor.JudgmentLockB)
//          {
//            cabinP->values.VacuumBox.state = _VacuumState_Prohibited;
//          }
        }
        else
        {
            cabinP->values.VacuumBox.offTimer = cabinC->parameters.VacuumBox.VacuumBox_OFFTime;
        }
        break;
        
      case _VacuumState_ON:
        VacuumBox = _VacuumValue_ON;
        
        if (cabinP->values.doorOpenedTimer)
        {
          cabinP->values.VacuumBox.state = _VacuumState_Delay;
          cabinP->values.VacuumBox.delayTimer = cabinC->parameters.VacuumBox.VacuumBox_ONDelayTime;
        }
        else if (!cabinP->values.VacuumBox.onTimer)
        {
          cabinP->values.VacuumBox.state = _VacuumState_OFF;
          if (!cabinP->values.VacuumBox.offTimer)
            cabinP->values.VacuumBox.offTimer = cabinC->parameters.VacuumBox.VacuumBox_OFFTime;
        }
        
        if(cabinP->components.vacuumBoxOverCurrentSensor.valueInCExt > cabinC->parameters.VacuumBox.VacuumBoxCurrentSensor.JudgmentLockA && 
           cabinP->components.vacuumBoxOverCurrentSensor.valueInCExt < cabinC->parameters.VacuumBox.VacuumBoxCurrentSensor.JudgmentLockB)
        {
          cabinP->values.VacuumBox.state = _VacuumState_Prohibited;     //Lock Check
          cabinP->values.VacuumBox.status = _VacuumBox_Lock;
        }
        else if((cabinP->values.VacuumBox.onTimer < 3)&&
                (cabinP->components.vacuumBoxOverCurrentSensor.valueInCExt > cabinC->parameters.VacuumBox.VacuumBoxCurrentSensor.JudgmentLeak))                                        
        {
          cabinP->values.VacuumBox.status = _VacuumBox_Leak;
          refrigerator_R.Values.vacuumBoxLeak = true;
        }
        else if(cabinP->components.vacuumBoxOverCurrentSensor.valueInCExt > cabinC->parameters.VacuumBox.VacuumBoxCurrentSensor.JudgmentStopA && 
                cabinP->components.vacuumBoxOverCurrentSensor.valueInCExt < cabinC->parameters.VacuumBox.VacuumBoxCurrentSensor.JudgmentStopB)
        {
          cabinP->values.VacuumBox.state = _VacuumState_Prohibited;
        }
        else
        {
          cabinP->values.VacuumBox.status = _VacuumBox_Normal;
          refrigerator_R.Values.vacuumBoxLeak = false;
        }
        
        break;
        
      case _VacuumState_Delay: 
        if (cabinP->values.doorOpenedTimer)
        {
          cabinP->values.VacuumBox.delayTimer = cabinC->parameters.VacuumBox.VacuumBox_ONDelayTime;
        }
        else if (!cabinP->values.doorOpenedTimer && !cabinP->values.VacuumBox.delayTimer)
        {
          cabinP->values.VacuumBox.state = _VacuumState_ON;
          cabinP->values.VacuumBox.onTimer = cabinC->parameters.VacuumBox.VacuumBox_ONTime2;
        }
        
        
        break;
        
      case _VacuumState_Prohibited:
        
          cabinP->values.VacuumBox.offTimer = cabinC->parameters.VacuumBox.VacuumBox_OFFTime;
          cabinP->values.VacuumBox.onTimer = 0;
          cabinP->values.VacuumBox.state = _VacuumState_OFF;

        break;
        
      default:
        cabinVacuumBoxInit(consts, params);
        break;
    }
    
    cabinP->components.VacuumBox.value = VacuumBox;
}

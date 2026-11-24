#include "cabin_ai_cool.h"
#include "../cabin.h"
#include "../../cooler/components/cooler_valve.h"

#if defined (AI_COOL_V1)
void CabinAIResetLabel(tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    
    for(unsigned char i = 0; i < NUMBER_OF_AI_ANALYZE_DAYS; i++)
      for(unsigned char j = 0; j < NUMBER_OF_AI_ANALYZE_HOURS; j++)
        cabinP->ai.label.array[i][j] = _AI_LABEL_MODERATE;
    
    for(unsigned char i = 0; i < NUMBER_OF_AI_ANALYZE_HOURS; i++)
      cabinP->ai.label.decisionArray[i] = _AI_LABEL_MODERATE;
    
    cabinP->ai.label.indexDay = 0;
    cabinP->ai.label.indexHour = 0;  
}

void CabinAIResetCompressorVal(tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
  
    cabinP->ai.compressor.maxVal = 0;
    cabinP->ai.compressor.workTimer = 0;

    cabinP->ai.compressor.cmpVal[_Ambient_0][0] = _CMP_AMB0_L1_L;
    cabinP->ai.compressor.cmpVal[_Ambient_0][1] = _CMP_AMB0_L1_H;
    cabinP->ai.compressor.cmpVal[_Ambient_0][2] = _CMP_AMB0_L2_L;
    cabinP->ai.compressor.cmpVal[_Ambient_0][3] = _CMP_AMB0_L2_H;
    cabinP->ai.compressor.cmpVal[_Ambient_0][4] = _CMP_AMB0_L3_L;
    cabinP->ai.compressor.cmpVal[_Ambient_0][5] = _CMP_AMB0_L3_H;
    
    cabinP->ai.compressor.cmpVal[_Ambient_1][0] = _CMP_AMB1_L1_L;
    cabinP->ai.compressor.cmpVal[_Ambient_1][1] = _CMP_AMB1_L1_H;
    cabinP->ai.compressor.cmpVal[_Ambient_1][2] = _CMP_AMB1_L2_L;
    cabinP->ai.compressor.cmpVal[_Ambient_1][3] = _CMP_AMB1_L2_H;
    cabinP->ai.compressor.cmpVal[_Ambient_1][4] = _CMP_AMB1_L3_L;
    cabinP->ai.compressor.cmpVal[_Ambient_1][5] = _CMP_AMB1_L3_H;

    cabinP->ai.compressor.cmpVal[_Ambient_2][0] = _CMP_AMB2_L1_L;
    cabinP->ai.compressor.cmpVal[_Ambient_2][1] = _CMP_AMB2_L1_H;
    cabinP->ai.compressor.cmpVal[_Ambient_2][2] = _CMP_AMB2_L2_L;
    cabinP->ai.compressor.cmpVal[_Ambient_2][3] = _CMP_AMB2_L2_H;
    cabinP->ai.compressor.cmpVal[_Ambient_2][4] = _CMP_AMB2_L3_L;
    cabinP->ai.compressor.cmpVal[_Ambient_2][5] = _CMP_AMB2_L3_H;

    cabinP->ai.compressor.cmpVal[_Ambient_3][0] = _CMP_AMB3_L1_L;
    cabinP->ai.compressor.cmpVal[_Ambient_3][1] = _CMP_AMB3_L1_H;
    cabinP->ai.compressor.cmpVal[_Ambient_3][2] = _CMP_AMB3_L2_L;
    cabinP->ai.compressor.cmpVal[_Ambient_3][3] = _CMP_AMB3_L2_H;
    cabinP->ai.compressor.cmpVal[_Ambient_3][4] = _CMP_AMB3_L3_L;
    cabinP->ai.compressor.cmpVal[_Ambient_3][5] = _CMP_AMB3_L3_H;

    cabinP->ai.compressor.cmpVal[_Ambient_4][0] = _CMP_AMB4_L1_L;
    cabinP->ai.compressor.cmpVal[_Ambient_4][1] = _CMP_AMB4_L1_H;
    cabinP->ai.compressor.cmpVal[_Ambient_4][2] = _CMP_AMB4_L2_L;
    cabinP->ai.compressor.cmpVal[_Ambient_4][3] = _CMP_AMB4_L2_H;
    cabinP->ai.compressor.cmpVal[_Ambient_4][4] = _CMP_AMB4_L3_L;
    cabinP->ai.compressor.cmpVal[_Ambient_4][5] = _CMP_AMB4_L3_H;

}

void CabinAIInit(tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    
    cabinP->ai.isEnabled = 0;
    cabinP->ai.isActivated = 0;
    cabinP->ai.timer1hour = 0;
    CabinAIResetLabel(params);
    CabinAIResetCompressorVal(params);
}

void CabinAIFindCompressorVal(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    
    unsigned char compSpeed = params->rf->Cooler.components.compressor.value;

    if(cabinC->type != _CabinType_NONE)
    {
      if(cabinP->states.Work == _Work_Cool)
      {
        if(cabinP->ai.compressor.maxVal < compSpeed)
          cabinP->ai.compressor.maxVal = compSpeed;
      }
      else
      {
        cabinP->ai.compressor.firstSpeedApplied = 0;
      }
    }
}

void CabinAIFindLinearFunction(stPoint point1, stPoint point2, double *slope, double *yIntercept) 
{
    // Calculate the slope (m)
    *slope = (point2.y - point1.y) / (point2.x - point1.x);
    // Calculate the y-intercept (b) using one of the points
    *yIntercept = point1.y - (*slope * point1.x);
}

unsigned char CabinAIGetCompressorValAccordingToDistance(tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    
    unsigned char compSpeed = params->rf->Cooler.components.compressor.value; // default olarak compressor h?z? verilir

    signed short sensortemp;
    signed short cutOutTemp = cabinP->sets.datas.coolCutOutTemp;
    signed short distance = 0;

    stPoint p1;
    stPoint p2;

    double slope, yIntercept;

    p1.x = _AI_CUTOUT_DISTANCE_MAX;
    p1.y = cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][cabinP->ai.label.decisionArray[cabinP->ai.label.indexHour]*2+1];
    p2.x = _AI_CUTOUT_DISTANCE_MIN;
    p2.y = cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][cabinP->ai.label.decisionArray[cabinP->ai.label.indexHour]*2];

    CabinAIFindLinearFunction(p1, p2, &slope, &yIntercept);

    switch (cabinP->states.Sensor)
    {
        case _Sensor_AirP:
        case _Sensor_AirP | _Sensor_EvaP:
        case _Sensor_AirP | _Sensor_EvaF:
            sensortemp = cabinP->components.airTempSensor.valueInCExt;
        break;

        case _Sensor_EvaP:
        case _Sensor_AirF:
        case _Sensor_EvaF:
        case _Sensor_AirF | _Sensor_EvaF:
        default:
            sensortemp = 0;
    }

    distance = sensortemp - cutOutTemp;
    compSpeed = (unsigned char)(slope * distance + yIntercept);

    /*hava sens?r?nde hata olma durumunda algoritma h?z azaltma yada artt?rma karar? alamaz*/
    if(sensortemp == 0)
    {
      cabinP->ai.compressor.firstSpeedApplied = 0;
      compSpeed = params->rf->Cooler.components.compressor.value; 
    }

    return compSpeed;
}

unsigned char CabinAICheckCompressorValLimits(unsigned char speed, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    
    unsigned char compSpeed = speed;

    switch(cabinP->ai.label.decisionArray[cabinP->ai.label.indexHour])
    {
      case _AI_LABEL_EFFICIENT:
        if(compSpeed > cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][1])
          compSpeed = cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][1];
        else
        {
          if(compSpeed < cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][0])
            compSpeed = cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][0];
        }
        break;
      case _AI_LABEL_MODERATE:
        if(compSpeed > cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][3])
          compSpeed = cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][3];
        else
        {
          if(compSpeed < cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][2])
            compSpeed = cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][2];
        }
        break;
      case _AI_LABEL_POWER:
        if(compSpeed > cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][5])
          compSpeed = cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][5];
        else
        {
          if(compSpeed < cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][4])
            compSpeed = cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][4];
        }
        break;
      default:
        break;
    }
    return compSpeed;
}

unsigned char CabinAIGetCompressorValAccordingToLabel(tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    unsigned char compSpeed = params->rf->Cooler.components.compressor.value; // default olarak compressor h?z? verilir;

    if(cabinP->ai.compressor.firstSpeedApplied == 0)
    {
        if(cabinP->ai.label.decisionArray[cabinP->ai.label.indexHour] == _AI_LABEL_EFFICIENT)
          compSpeed = (cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][0] + cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][1]) / 2;
        else if(cabinP->ai.label.decisionArray[cabinP->ai.label.indexHour] == _AI_LABEL_MODERATE)
          compSpeed = (cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][2] + cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][3]) / 2;
        else if(cabinP->ai.label.decisionArray[cabinP->ai.label.indexHour] == _AI_LABEL_POWER)
          compSpeed = (cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][4] + cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][5]) / 2;
        else
          compSpeed = cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][0];

      cabinP->ai.compressor.firstSpeedApplied = 1;
      cabinP->ai.compressor.speedChangeTimer = _AI_SPEED_CHANGE_TIME;
    }
    else
    {
        if(cabinP->ai.compressor.speedChangeTimer == 0)
        {
          cabinP->ai.compressor.speedChangeTimer = _AI_SPEED_CHANGE_TIME;
          compSpeed = CabinAIGetCompressorValAccordingToDistance(params);
        }
    }

    compSpeed = CabinAICheckCompressorValLimits(compSpeed,params);
      
    return compSpeed;
}

unsigned char CabinAIGetCompressorValAccordingToOnTimer(stCabin_Flash const *cabinC, stCabin_Ram *cabinP, stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
    
    unsigned char compSpeed = rfP->Cooler.components.compressor.value; // default olarak compressor h?z? verilir;

      if(rfP->Cooler.parameters.compressorOnTimerPrev <= _AI_COMP_ON_TIME_CMP_VAL)
      {
        if(cabinP->values.ambientType == _Ambient_0)
          compSpeed = _CMP_AMB0_CONST_AI_SPEED;
        else if(cabinP->values.ambientType == _Ambient_1)
          compSpeed = _CMP_AMB1_CONST_AI_SPEED;
        else if(cabinP->values.ambientType == _Ambient_2)
          compSpeed = _CMP_AMB2_CONST_AI_SPEED;
        else if(cabinP->values.ambientType == _Ambient_3)
          compSpeed = _CMP_AMB3_CONST_AI_SPEED;
        else if(cabinP->values.ambientType == _Ambient_4)
          compSpeed = _CMP_AMB4_CONST_AI_SPEED;
        else
          compSpeed = _CMP_AMB2_CONST_AI_SPEED;
      }
      else
      { 
        if (rfP->Cooler.components.solenoidValve[0].positionIndex == _FF_OPEN)
          compSpeed = rfC->Cooler.parameters.compressor.FF.speedLevel[cabinP->values.ambientType][rfP->Cooler.parameters.gasLevel - 1];
        else if (rfP->Cooler.components.solenoidValve[0].positionIndex == _FRZ_OPEN)
          compSpeed = rfC->Cooler.parameters.compressor.FRZ.speedLevel[cabinP->values.ambientType][rfP->Cooler.parameters.gasLevel - 1];
        else
          compSpeed = rfC->Cooler.parameters.compressor.FF.speedLevel[cabinP->values.ambientType][rfP->Cooler.parameters.gasLevel - 1];        
      }
    
    return compSpeed;
}

unsigned char udaqLabelFF = 99;
unsigned char udaqLabelFRZ = 99;

void CabinAIFindLabelWithCmpVal(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
  
    unsigned char compSpeed = cabinP->ai.compressor.maxVal;
    enLabelAI label = _AI_LABEL_MODERATE;

    if(cabinC->type != _CabinType_NONE)
    {
        if(compSpeed <= cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][1])
          label = _AI_LABEL_EFFICIENT;
        else if(compSpeed <= cabinP->ai.compressor.cmpVal[cabinP->values.ambientType][3])
          label = _AI_LABEL_MODERATE;
        else
          label = _AI_LABEL_POWER;
        
        if(cabinP->ai.isActivated == 1)
        {
          if(cabinP->ai.compressor.workTimer > _AI_LABEL_INC_WORKTIME)
          {
            if(label < _AI_LABEL_POWER)
              cabinP->ai.label.array[cabinP->ai.label.indexDay][cabinP->ai.label.indexHour] = (enLabelAI)(label+1);
          }
          else if(cabinP->ai.compressor.workTimer < _AI_LABEL_DEC_WORKTIME)
          {
            if(label > _AI_LABEL_EFFICIENT)
              cabinP->ai.label.array[cabinP->ai.label.indexDay][cabinP->ai.label.indexHour] = (enLabelAI)(label-1);
          }
          else
             cabinP->ai.label.array[cabinP->ai.label.indexDay][cabinP->ai.label.indexHour] = label;
        }
        else
          cabinP->ai.label.array[cabinP->ai.label.indexDay][cabinP->ai.label.indexHour] = label;
    }

    if(cabinC->type == _CabinType_FF)
      udaqLabelFF = label;
    if(cabinC->type == _CabinType_FRZ)
      udaqLabelFRZ = label;
}

void CabinAIDecideLabelForEachHour(tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    
    enLabelAI label = _AI_LABEL_MODERATE;
    unsigned char efficient = 0;
    unsigned char moderate = 0;
    unsigned char power = 0;

    for(unsigned char j = 0; j < NUMBER_OF_AI_ANALYZE_HOURS; j++)
    {
      for( unsigned char i = 0; i < NUMBER_OF_AI_ANALYZE_DAYS; i++)
      {
        switch(cabinP->ai.label.array[i][j])
        {
          case _AI_LABEL_EFFICIENT:
            efficient++;
            break;
          case _AI_LABEL_MODERATE:
            moderate++;
            break;
          case _AI_LABEL_POWER:
            power++;
            break;
          default:
            break;
        }
      }

      if(efficient > moderate && efficient > power)
        label = _AI_LABEL_EFFICIENT;
      else if(moderate > efficient && moderate > power)
        label = _AI_LABEL_MODERATE;
      else if(power > efficient && power > moderate)
        label = _AI_LABEL_POWER;
      else
        label = _AI_LABEL_POWER;

      efficient = 0;
      moderate = 0;
      power = 0;

      cabinP->ai.label.decisionArray[j] = label;
    }
}

void CabinAILabelForEachHour(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  stCabin_Ram *cabinP = params->cabin;
  
    CabinAIFindLabelWithCmpVal(consts, params);

    if(cabinP->ai.label.indexHour < (NUMBER_OF_AI_ANALYZE_HOURS-1))
    {
      cabinP->ai.label.indexHour++;
    }
    else
    {
      CabinAIDecideLabelForEachHour(params);
      cabinP->ai.label.indexHour = 0;
      cabinP->ai.label.indexDay++;
    }
    cabinP->ai.compressor.workTimer = 0;
}

void CabinAIDecreaseCabinSetIfRequired(const stCabin_Flash *cabinC, stCabin_Ram *cabinP)
{
 /* const stCabin_Flash *cabinC = consts->cabin;
  stCabin_Ram *cabinP = params->cabin;*/
  
  if(cabinP->ai.isEnabled == 1)
  {
      if(cabinC->type == _CabinType_FF)
      {
          if(cabinP->sets.datas.display < _AI_FF_SET)
          {
            cabinP->sets.datas.coolCutInTemp = _AI_COOL_FF_AIR_CUT_IN_TEMP + cabinC->parameters.Cool.CabinSetsAmbientOffSetAI[cabinP->values.ambientType];
            cabinP->sets.datas.coolCutOutTemp = _AI_COOL_FF_AIR_CUT_OUT_TEMP + cabinC->parameters.Cool.CabinSetsAmbientOffSetAI[cabinP->values.ambientType]; 
            cabinP->sets.datas.fanCutInTemp = _AI_COOL_FF_AIR_CUT_IN_TEMP + cabinC->parameters.Cool.FanSetsAmbientOffSetAI[cabinP->values.ambientType];
            cabinP->sets.datas.fanCutOutTemp = _AI_COOL_FF_AIR_CUT_OUT_TEMP + cabinC->parameters.Cool.FanSetsAmbientOffSetAI[cabinP->values.ambientType];
          }
      }

      if(cabinC->type == _CabinType_FRZ)
      {
          if(cabinP->sets.datas.display < _AI_FRZ_SET)
          {
            //if(_AI_COOL_FRZ_AIR_CUT_IN_TEMP > cabinP->sets.datas.coolCutInTemp)
            //{
            cabinP->sets.datas.coolCutInTemp = _AI_COOL_FRZ_AIR_CUT_IN_TEMP + cabinC->parameters.Cool.CabinSetsAmbientOffSetAI[cabinP->values.ambientType];
            cabinP->sets.datas.coolCutOutTemp = _AI_COOL_FRZ_AIR_CUT_OUT_TEMP + cabinC->parameters.Cool.CabinSetsAmbientOffSetAI[cabinP->values.ambientType];
            cabinP->sets.datas.fanCutInTemp = _AI_COOL_FRZ_AIR_CUT_IN_TEMP + cabinC->parameters.Cool.FanSetsAmbientOffSetAI[cabinP->values.ambientType];
            cabinP->sets.datas.fanCutOutTemp = _AI_COOL_FRZ_AIR_CUT_OUT_TEMP + cabinC->parameters.Cool.FanSetsAmbientOffSetAI[cabinP->values.ambientType];
            //}
          }
      }
  }
}

void CabinAIT1sec(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    
    if(cabinP->ai.isEnabled == 1)
    {
      if(params->rf->Cabin[1].sets.user.functions.Convertible || params->rf->Cabin[1].sets.user.functions.CabinOff
         || params->rf->Cabin[1].sets.user.functions.Quick)
      {
        params->rf->Cabin[0].ai.isEnabled=0;
        params->rf->Cabin[1].ai.isEnabled=0;
      }
      else
      {
      CabinAIFindCompressorVal(consts, params);
    
      if(cabinP->ai.compressor.firstSpeedApplied == 1)
        if(cabinP->ai.compressor.speedChangeTimer > 0)
          cabinP->ai.compressor.speedChangeTimer--;
      }
    }
    else
    {
      CabinAIInit(params); 
    }
}

void CabinAIT1hour(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    
    if(cabinP->ai.isEnabled == 1)
    {
      CabinAILabelForEachHour(consts, params);
      CabinAIResetCompressorVal(params);

      if(cabinP->ai.label.indexDay >= (NUMBER_OF_AI_ANALYZE_DAYS))
      {
        cabinP->ai.label.indexDay = 0;
        cabinP->ai.isActivated = 1;
      }
    }
}
#endif
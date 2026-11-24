#include "cabin_ff_defrost.h"
#include "../../defaults/cabin_defaults_defrost.h"

enDefrostStates cabinFfDefrostIn(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;

  if (cabinP->values.frzDefrost && params->rf->Cooler.parameters.frzDefrost && !cabinC->parameters.Cool.isEvaCutInActive)
  {
    return _DefrostIn_FF_MaxWorkTimeElapsed;
  }
 
  /*if (cabinP->values.frzDefrost && params->rf->Cooler.parameters.frzDefrost && !cabinC->parameters.Cool.isEvaCutInActive)
  {  
    if((!params->rf->Cooler.components.compressor.value) && (params->rf->Cooler.parameters.compressorOffTimer >= (consts->rf->Cooler.parameters.compressor.minOffTime + 5)))
    {
      return _DefrostIn_FF_MaxWorkTimeElapsed;
    }
    else
    {
      if(!cabinP->values.defrost.delayTimer || 
         (cabinP->values.defrost.preCoolActive && 
          cabinP->components.airTempSensor.valueInCExt <= cabinP->sets.datas.coolCutOutTemp))
        return _DefrostIn_FF_MaxWorkTimeElapsed;
    }
    cabinP->values.defrost.preCoolActive = 1;
  }
  else
  {
    if(!cabinP->values.defrost.preCoolActive)
      cabinP->values.defrost.delayTimer = cabinC->parameters.Defrost.DelayTime;
  }*/

  if (cabinP->values.quickFF)
    return _DefrostNone_FF;

  if (cabinP->sets.work.functions.Sabbath)
    return _DefrostNone_FF;

  switch (cabinP->states.Sensor)
  {
  case _Sensor_AirP:
    if (cabinP->values.quickFRZ)
    {
      if (cabinP->values.realTimer >= cabinC->parameters.Defrost.MaxRealTime_QuickFRZ)
        return _DefrostIn_FF_MaxRealTimeElapsed;
    }
    else if (cabinP->values.doorsClosedState)
    {
      if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DC)
        return _DefrostIn_FF_MaxWorkTimeElapsed;
      if (cabinP->values.cycleCounter >= cabinC->parameters.Defrost.MaxCycle_DC)
        return _DefrostIn_FF_MaxCycleReached;
    }
    else
    {
      if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DO)
        return _DefrostIn_FF_MaxWorkTimeElapsed;
      if (cabinP->values.cycleCounter >= cabinC->parameters.Defrost.MaxCycle_DO)
        return _DefrostIn_FF_MaxCycleReached;
    }
    break;
  case _Sensor_EvaP:
    if (cabinP->values.quickFRZ)
    {
      if (cabinP->values.realTimer >= cabinC->parameters.Defrost.MaxRealTime_QuickFRZ)
        return _DefrostIn_FF_MaxRealTimeElapsed;
    }
    else if (cabinP->values.doorsClosedState)
    {
      if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DC)
        return _DefrostIn_FF_MaxWorkTimeElapsed;
    }
    else
    {
      if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DO)
        return _DefrostIn_FF_MaxWorkTimeElapsed;
    }
    break;
  case _Sensor_AirP | _Sensor_EvaP:
    
    if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) ||
        (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
    {
      if (cabinP->values.quickFRZ)
      {
        if (cabinP->values.realTimer >= cabinC->parameters.Defrost.MaxRealTime_QuickFRZ)
          return _DefrostIn_FF_MaxRealTimeElapsed;
      }
      else if (cabinP->values.doorsClosedState)
      {
        if (cabinP->values.realTimer >= cabinC->parameters.Defrost.MaxRealTime_DC && !cabinC->parameters.Cool.isEvaCutInActive)
          return _DefrostIn_FF_MaxRealTimeElapsed;
        if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DC)
          return _DefrostIn_FF_MaxWorkTimeElapsed;
      }
      else
      {
        if (cabinP->values.realTimer >= cabinC->parameters.Defrost.MaxRealTime_DO && !cabinC->parameters.Cool.isEvaCutInActive)
          return _DefrostIn_FF_MaxRealTimeElapsed;
        if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DO)
          return _DefrostIn_FF_MaxWorkTimeElapsed;
      }
    }
    
    if (cabinC->parameters.Defrost.Type == _DefrostType_Variable)
    {
        if (cabinP->values.quickFRZ)
        {
          if (cabinP->values.realTimer >= cabinC->parameters.Defrost.MaxRealTime_QuickFRZ)
            return _DefrostIn_FF_MaxRealTimeElapsed;
        }
        
        if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DO)
          return _DefrostIn_FF_MaxWorkTimeElapsed;
        
        if (cabinP->values.realTimer >= cabinP->values.defrost.variableTime)
        {

          if((!params->rf->Cooler.components.compressor.value) && (params->rf->Cooler.parameters.compressorOffTimer >= (consts->rf->Cooler.parameters.compressor.minOffTime + 5)))
          {
            return _DefrostIn_FF_MaxRealTimeElapsed;
          }
          else if(params->rf->Cooler.parameters.compressorOnTimer >= consts->rf->Cooler.parameters.compressor.minOnTime )
          {
              if(!cabinP->values.defrost.delayTimer || 
                 (cabinP->values.defrost.preCoolActive && 
                  cabinP->components.airTempSensor.valueInCExt <= cabinP->sets.datas.coolCutOutTemp))
                return _DefrostIn_FF_MaxRealTimeElapsed;
          }
          cabinP->values.defrost.preCoolActive = 1;
        }
        else
        {
          if(!cabinP->values.defrost.preCoolActive)
            cabinP->values.defrost.delayTimer = cabinC->parameters.Defrost.DelayTime;
        }
        

    }
    break;
  case _Sensor_AirP | _Sensor_EvaF:
    if (cabinP->values.quickFRZ)
    {
      if (cabinP->values.realTimer >= cabinC->parameters.Defrost.MaxRealTime_QuickFRZ)
        return _DefrostIn_FF_MaxRealTimeElapsed;
    }
    else
    {
      if (cabinP->values.cycleCounter >= cabinC->parameters.Defrost.ErrorMaxCycle)
        return _DefrostIn_FF_ErrorMaxCycleReached;
    }
    break;
  case _Sensor_AirF:
  case _Sensor_EvaF:
  case _Sensor_AirF | _Sensor_EvaP:
  case _Sensor_AirF | _Sensor_EvaF:
    return _DefrostNone_FF;
    // break; if statement is not reachable comment out;
  }
  return _DefrostNone_FF;
}

enDefrostStates cabinFfDefrostOut(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;

  cabinP->values.defrost.preCoolActive = 0;
      

  switch (cabinP->states.Sensor)
  {
  case _Sensor_AirP:
    if (cabinP->components.airTempSensor.valueInCExt >= cabinC->parameters.Defrost.DefrostCutOutTemp)
    {
      if (cabinP->stateTimers.Defrost + cabinP->stateTimers.Idle >= cabinC->parameters.Defrost.MinNoCoolTime)
        return _DefrostOut_FF_AirHigherThanDefrostCutOutAndMinNoCoolTimeElapsed;
    }
    else
    {
      if (cabinP->stateTimers.Defrost + cabinP->stateTimers.Idle >= cabinC->parameters.Defrost.MaxNoCoolTime)
        return _DefrostOut_FF_MaxNoCoolTimeElapsed;
    }
    break;
  case _Sensor_EvaP:
  case _Sensor_AirP | _Sensor_EvaP:
  case _Sensor_AirF | _Sensor_EvaP:
    if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) || (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
    {
      if (cabinP->components.evaTempSensor.valueInCExt >= cabinC->parameters.Defrost.DefrostCutOutTemp)
      {
        params->rf->Cooler.parameters.frzDefrost = false;
        return _DefrostOut_FF_EvaHigherThanDefrostCutOut;
      }
      else
      {
        if ((cabinP->stateTimers.Defrost + cabinP->stateTimers.Idle) >= cabinC->parameters.Defrost.MaxNoCoolTime)
        {
          params->rf->Cooler.parameters.frzDefrost = false;
          return _DefrostOut_FF_MaxNoCoolTimeElapsed;
        }
      }
    }
    else if ((cabinC->parameters.Defrost.Type == _DefrostType_Variable))
    {
      enVariableDefrostReason reason = cabinP->values.defrost.variableReason;
      
      if (cabinP->states.Defrost == _DefrostIn_FF_MaxWorkTimeElapsed)
      {
        if (cabinP->components.evaTempSensor.valueInCExt >= cabinC->parameters.Defrost.DefrostCutOutTemp)
        {
          params->rf->Cooler.parameters.frzDefrost = false;
          return _DefrostOut_FF_EvaHigherThanDefrostCutOut;
        }
        
        if ((cabinP->stateTimers.Defrost + cabinP->stateTimers.Idle) >= cabinC->parameters.Defrost.MaxNoCoolTime)
        {
          params->rf->Cooler.parameters.frzDefrost = false;
          return _DefrostOut_FF_MaxNoCoolTimeElapsed;
        }
      }
      else
      {
        if ((cabinP->components.evaTempSensor.valueInCExt >= cabinC->parameters.Defrost.VariableDefrostCutOutTemp[cabinP->values.ambientType][reason]) &&
            (cabinP->components.evaTempSensor.valueInCExt >= _Defrost_FRZAirEva_MinDefrostCutOutTemp))
        {
          params->rf->Cooler.parameters.frzDefrost = false;          
          return _DefrostOut_FF_EvaHigherThanDefrostCutOut;
        }
        
        if ((cabinP->stateTimers.Defrost + cabinP->stateTimers.Idle) >= cabinC->parameters.Defrost.MaxNoCoolTime)
        {
          params->rf->Cooler.parameters.frzDefrost = false;
          return _DefrostOut_FF_MaxNoCoolTimeElapsed;
        }
      }
    }
    else
    {
      return _DefrostOut_FF_Always;
    }
    break;
  case _Sensor_AirF:
  case _Sensor_EvaF:
  case _Sensor_AirF | _Sensor_EvaF:
    if (cabinP->sets.work.functions.Sabbath)
    {
      if (cabinP->stateTimers.Defrost >= cabinP->values.defrost.meanTime)
        return _DefrostOut_FF_MeanDefrostTimeElapsed;
    }
  case _Sensor_AirP | _Sensor_EvaF:
    if ((cabinP->stateTimers.Defrost + cabinP->stateTimers.Idle) >= cabinC->parameters.Defrost.ErrorMaxNoCoolTime)
      return _DefrostOut_FF_ErrorMaxNoCoolTimeElapsed;
    break;
  }
  return _DefrostNone_FF;
}

void cabinFfDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
  stCabin_Ram *cabinP = params->cabin;
  const stCabin_Flash *cabinC = consts->cabin;
  
  switch (cabinP->states.Sensor)
  {
  case _Sensor_AirP:
  case _Sensor_AirF:
  case _Sensor_EvaF:
    if (cabinP->states.Work != _Work_Cool)
      // cabinP->values.workTimer = 0;
      break;

  case _Sensor_EvaP:
    if ((cabinP->states.Work != _Work_Cool) ||
        (cabinP->components.evaTempSensor.valueInCExt >= cabinC->parameters.Defrost.DefrostCutOutTemp))
    {
      // cabinP->values.workTimer = 0;
    }
    break;

  case _Sensor_AirP | _Sensor_EvaP:
  case _Sensor_AirF | _Sensor_EvaP:
    if (cabinP->components.evaTempSensor.valueInCExt >= cabinC->parameters.Defrost.DefrostCutOutTemp)
    {
      // cabinP->values.realTimer = 0;
    }
    break;
  }
}

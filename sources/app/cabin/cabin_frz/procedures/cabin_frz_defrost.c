#include "cabin_frz_defrost.h"
#include "../../defaults/cabin_defaults_defrost.h"

enDefrostStates cabinFrzDefrostIn(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    if (cabinP->values.defrostDelayForIceCream &&
        cabinP->values.defrostDelayTimerForIceCream)
        return _DefrostNone_FRZ_ForIceCream;

    if (cabinP->values.quickFF)
        return _DefrostNone_FRZ;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirF:
    case _Sensor_AirF | _Sensor_EvaP:
        return _DefrostNone_FRZ;
    // break; if statement is not reachable comment out;
    case _Sensor_EvaP:
    case _Sensor_EvaF:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:

        if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) ||
            (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
        {
            if ((cabinP->sets.work.Mode == _CabinMode_FRZ) &&
                cabinP->sets.work.functions.Quick &&
                !cabinP->values.quickDefrost)
            {
                if (cabinP->values.quickTimer >= cabinC->parameters.Defrost.MaxQuickTime)
                    return _DefrostIn_FRZ_MaxQuickTimeElapsed;
            }
            else if (cabinP->values.doorsClosedState)
            {
                if (cabinP->values.realTimer >= cabinC->parameters.Defrost.MaxRealTime_DC)
                    return _DefrostIn_FRZ_MaxRealTimeElapsed;
            }
            else
            {
                if (cabinP->values.realTimer >= cabinC->parameters.Defrost.MaxRealTime_DO)
                    return _DefrostIn_FRZ_MaxRealTimeElapsed;

                if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DO)
                    return _DefrostIn_FRZ_MaxWorkTimeElapsed;
                
                //Defrost by High temperature count set
                for (uint8_t i = 0; i < _NumOfCabin; i++)
                {
                    if (consts->rf->Cabin[i].type != _CabinType_NONE)
                    {
                        if(params->rf->Cabin[i].values.defrostHighTempTimer >= consts->rf->Cabin[i].parameters.Defrost.MaxDefrostHighTempTime)
                        {
                            params->rf->Cabin[i].values.defrost.defrostHighTempActive = true;
                            return _DefrostIn_FRZ_MaxDefrostHighTempTimeElapsed;
                        }
                    }
                }
            }
        }

        if (cabinC->parameters.Defrost.Type == _DefrostType_Variable)
        {
            if (consts->rf->Cooler.components.ambientTempSensor.type != _TempSensorType_NONE && cabinP->values.ambientType != _Ambient_NONE)
            {
                if ((cabinP->sets.work.Mode == _CabinMode_FRZ) &&
                    cabinP->sets.work.functions.Quick &&
                    !cabinP->values.quickDefrost)
              {
#if defined (AI_COOL_V1)
                if(cabinP->ai.isEnabled)
                {
                  if ((cabinP->values.quickTimer >= cabinC->parameters.Defrost.MaxQuickTime) &&
                      (cabinP->values.quickTimer >= cabinC->parameters.Defrost.VariableMaxRealTime_AI[cabinP->values.ambientType][_IntervalSize]))
                    return _DefrostIn_FRZ_MaxQuickTimeElapsed;
                }
                else
#endif
                {
                  if ((cabinP->values.quickTimer >= cabinC->parameters.Defrost.MaxQuickTime) &&
                      (cabinP->values.quickTimer >= cabinC->parameters.Defrost.VariableMaxRealTime[cabinP->values.ambientType][_IntervalSize]))
                    return _DefrostIn_FRZ_MaxQuickTimeElapsed;
                }
              }

                if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DO)
                    return _DefrostIn_FRZ_MaxWorkTimeElapsed;
                
                //Defrost by High temperature count set
                for (uint8_t i = 0; i < _NumOfCabin; i++)
                {
                    if (consts->rf->Cabin[i].type != _CabinType_NONE)
                    {
                        if(params->rf->Cabin[i].values.defrostHighTempTimer >= consts->rf->Cabin[i].parameters.Defrost.MaxDefrostHighTempTime)
                        {
                            params->rf->Cabin[i].values.defrost.defrostHighTempActive = true;
                            return _DefrostIn_FRZ_MaxDefrostHighTempTimeElapsed;
                        }
                    }
                }
                
                if (cabinP->values.realTimer >= cabinP->values.defrost.variableTime)
                {

                  if((!params->rf->Cooler.components.compressor.value) && (params->rf->Cooler.parameters.compressorOffTimer >= (consts->rf->Cooler.parameters.compressor.minOffTime + 5)))
                  {
                    return _DefrostIn_FRZ_MaxRealTimeElapsed;
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
            else
            {
              if ((cabinP->sets.work.Mode == _CabinMode_FRZ) &&
                  cabinP->sets.work.functions.Quick &&
                    !cabinP->values.quickDefrost)
              {
#if defined (AI_COOL_V1)
                if(cabinP->ai.isEnabled)
                {
                  if ((cabinP->values.quickTimer >= cabinC->parameters.Defrost.MaxQuickTime) &&
                      (cabinP->values.quickTimer >= cabinC->parameters.Defrost.VariableMaxRealTime_AI[cabinP->values.ambientType][_IntervalSize]))
                    return _DefrostIn_FRZ_MaxQuickTimeElapsed;
                }
                else
#endif
                {
                  
                  if ((cabinP->values.quickTimer >= cabinC->parameters.Defrost.MaxQuickTime) &&
                      (cabinP->values.quickTimer >= cabinC->parameters.Defrost.VariableMaxRealTime[_Ambient_0][_IntervalSize]))
                    return _DefrostIn_FRZ_MaxQuickTimeElapsed;
                }
              }

                if (cabinP->values.workTimer >= cabinC->parameters.Defrost.MaxWorkTime_DO)
                    return _DefrostIn_FRZ_MaxWorkTimeElapsed;

                //Defrost by High temperature count set
                for (uint8_t i = 0; i < _NumOfCabin; i++)
                {
                    if (consts->rf->Cabin[i].type != _CabinType_NONE)
                    {
                        if(params->rf->Cabin[i].values.defrostHighTempTimer >= consts->rf->Cabin[i].parameters.Defrost.MaxDefrostHighTempTime)
                        {
                            params->rf->Cabin[i].values.defrost.defrostHighTempActive = true;
                            return _DefrostIn_FRZ_MaxDefrostHighTempTimeElapsed;
                        }
                    }
                }
                
                if (cabinP->values.realTimer >= cabinP->values.defrost.variableTime)
                {
                  cabinP->values.defrost.preCoolActive = 1;
                  if((!params->rf->Cooler.components.compressor.value) && (params->rf->Cooler.parameters.compressorOffTimer >= (consts->rf->Cooler.parameters.compressor.minOffTime + 5)))
                  {
                    return _DefrostIn_FRZ_MaxRealTimeElapsed;
                  }
                  else if(params->rf->Cooler.parameters.compressorOnTimer >= consts->rf->Cooler.parameters.compressor.minOnTime )
                  {
                      if(!cabinP->values.defrost.delayTimer || 
                         (cabinP->values.defrost.preCoolActive && 
                          cabinP->components.airTempSensor.valueInCExt <= cabinP->sets.datas.coolCutOutTemp))
                        return _DefrostIn_FF_MaxRealTimeElapsed;
                  }
                }
                else
                {
                  if(!cabinP->values.defrost.preCoolActive)
                    cabinP->values.defrost.delayTimer = cabinC->parameters.Defrost.DelayTime;
                }
            }
        }

        break;
    }
    return _DefrostNone_FRZ;
}

enDefrostStates cabinFrzDefrostOut(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Flash const *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    
    cabinP->values.defrost.preCoolActive = 0;
    
    if (cabinP->values.defrostDelayForIceCream &&
        cabinP->values.defrostDelayTimerForIceCream)
        return _DefrostOut_FRZ_ForIceCream;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirF:
        return _DefrostOut_FRZ_Always;
        // break; if statement is not reachable comment out;

    case _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
        if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) || (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
        {
            if (cabinP->components.evaTempSensor.valueInCExt >= cabinC->parameters.Defrost.DefrostCutOutTemp)
            {
                return _DefrostOut_FRZ_EvaHigherThanDefrostCutOut;
            }
            if (cabinP->values.doorsClosedState)
            {
                if (cabinP->stateTimers.Defrost >= cabinC->parameters.Defrost.MaxDefrostTime_DC)
                {
                    return _DefrostOut_FRZ_MaxDefrostTimeElapsed_DC;
                }
            }
            else
            {
                if (cabinP->stateTimers.Defrost >= cabinC->parameters.Defrost.MaxDefrostTime_DO)
                {
                    return _DefrostOut_FRZ_MaxDefrostTimeElapsed_DO;
                }
            }
        }
        else if ((cabinC->parameters.Defrost.Type == _DefrostType_Variable))
        {
            enVariableDefrostReason reason = cabinP->values.defrost.variableReason;
            
            if(cabinP->values.defrost.defrostHighTempActive)
              reason = _DefrostInInteval_5;

            if (cabinP->states.Defrost == _DefrostIn_FRZ_MaxWorkTimeElapsed)
            {
                if (cabinP->components.evaTempSensor.valueInCExt >= cabinC->parameters.Defrost.DefrostCutOutTemp)
                    return _DefrostOut_FRZ_EvaHigherThanDefrostCutOut;

                if (cabinP->stateTimers.Defrost >= cabinC->parameters.Defrost.MaxDefrostTime_DO)
                    return _DefrostOut_FRZ_MaxDefrostTimeElapsed_DO;
            }
            else
            {
                if ((cabinP->components.evaTempSensor.valueInCExt >= cabinC->parameters.Defrost.VariableDefrostCutOutTemp[cabinP->values.ambientType][reason]) &&
                    (cabinP->components.evaTempSensor.valueInCExt >= _Defrost_FRZAirEva_MinDefrostCutOutTemp))
                    return _DefrostOut_FRZ_EvaHigherThanDefrostCutOut;

                if (cabinP->stateTimers.Defrost >= cabinC->parameters.Defrost.MaxDefrostTime_DO)
                    return _DefrostOut_FRZ_MaxDefrostTimeElapsed_DO;
            }
        }
        else
        {
            return _DefrostOut_FRZ_Always;
        }
        break;
    case _Sensor_EvaF:
        if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) ||
            (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
        {
            if (cabinP->sets.work.functions.Sabbath)
            {
                if (cabinP->stateTimers.Defrost >=
                    cabinP->values.defrost.meanTime)
                    return _DefrostOut_FRZ_MeanDefrostTimeElapsed;
            }

            if (cabinP->values.doorsClosedState)
            {
                if (cabinP->stateTimers.Defrost >=
                    cabinC->parameters.Defrost.MaxDefrostTime_DC)
                    return _DefrostOut_FRZ_MaxDefrostTimeElapsed_DC;
            }
            else
            {
                if (cabinP->stateTimers.Defrost >=
                    cabinC->parameters.Defrost.MaxDefrostTime_DO)
                    return _DefrostOut_FRZ_MaxDefrostTimeElapsed_DO;
            }
        }
        else
        {
            return _DefrostOut_FRZ_Always;
        }
        break;
    case _Sensor_AirP | _Sensor_EvaF:
        if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) ||
            (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
        {
            if (cabinP->stateTimers.Defrost >=
                cabinC->parameters.Defrost.ErrorMaxDefrostTime)
                return _DefrostOut_FRZ_ErrorMaxDefrostTimeElapsed;
        }
        else
        {
            return _DefrostOut_FRZ_Always;
        }
        break;
    case _Sensor_AirF | _Sensor_EvaF:
        if ((cabinC->parameters.Defrost.Type == _DefrostType_Normal) ||
            (cabinC->parameters.Defrost.Type == _DefrostType_HotGas))
        {
            if (cabinP->sets.work.functions.Sabbath)
            {
                if (cabinP->stateTimers.Defrost >=
                    cabinP->values.defrost.meanTime)
                    return _DefrostOut_FRZ_MeanDefrostTimeElapsed;
            }

            if (cabinP->stateTimers.Defrost >=
                cabinC->parameters.Defrost.ErrorMaxDefrostTime)
                return _DefrostOut_FRZ_ErrorMaxDefrostTimeElapsed;
        }
        else
        {
            return _DefrostOut_FRZ_Always;
        }
        break;
    }

    return _DefrostNone_FRZ;
}

void cabinFrzDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    switch (params->cabin->states.Sensor)
    {
    case _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirF | _Sensor_EvaP:
        if (params->cabin->components.evaTempSensor.valueInCExt >=
            consts->cabin->parameters.Defrost.DefrostCutOutTemp)
        {
            // cabinP->values.realTimer = 0;
            // cabinP->values.workTimer = 0;
        }
        break;
    }
}

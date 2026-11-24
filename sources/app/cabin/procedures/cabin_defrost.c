#include "cabin_defrost.h"
#include "app/refrigerator/refrigerator.h"

void cabinDefrostMeanTimeInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

	switch (cabinC->type)
	{
	case _CabinType_FRZ:
	case _CabinType_JKR:
		cabinP->values.defrost.meanTime =
			cabinC->parameters.Defrost.ErrorMaxDefrostTime;
		break;

	case _CabinType_FF:
	case _CabinType_FLAP:
	// case _CabinType_BioFRESH:
	case _CabinType_NONE:
	default:
		cabinP->values.defrost.meanTime =
			cabinC->parameters.Defrost.ErrorMaxNoCoolTime / 2;
		break;
	}

	for (unsigned char i = 0; i < _DutyRaw_Size; i++)
		cabinP->values.defrost.rawTime[i] = cabinP->values.defrost.meanTime;
}

void cabinDefrostMeanTimeCTRL(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);

	unsigned long meanTime = 0;

	if (cabinP->values.defrost.index >= _DutyRaw_Size)
		cabinP->values.defrost.index = 0;

	cabinP->values.defrost.rawTime[cabinP->values.defrost.index] =
		cabinP->stateTimers.Defrost;

	cabinP->values.defrost.index++;

	for (uint8_t i = 0; i < _DutyRaw_Size; i++)
	{
		meanTime += cabinP->values.defrost.rawTime[i];
	}

	meanTime = meanTime / _DutyRaw_Size;

	cabinP->values.defrost.meanTime = meanTime;

	cabinP->stateTimers.Defrost = 0;
}

void cabinDefrostUpdateInterval(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

	if (consts->rf->Cooler.components.ambientTempSensor.type && cabinP->values.ambientType != _Ambient_NONE)
	{
#if defined (AI_COOL_V1)
          if(cabinP->ai.isEnabled)
            cabinP->values.defrost.variableTime = cabinC->parameters.Defrost.VariableMaxRealTime_AI[cabinP->values.ambientType][_IntervalSize];
          else
#endif
            cabinP->values.defrost.variableTime = cabinC->parameters.Defrost.VariableMaxRealTime[cabinP->values.ambientType][_IntervalSize];
          
          cabinP->values.defrost.variableReason = _DefrostInInteval_5;

		for (unsigned char i = 0; i < _IntervalSize; i++)
		{
#if defined (AI_COOL_V1)
                  if(cabinP->ai.isEnabled)
                  {
                    if (cabinP->values.defrostDoorOpenedTimer < cabinC->parameters.Defrost.DoorOpenedIntervals_AI[i])
                    {
                      cabinP->values.defrost.variableTime = cabinC->parameters.Defrost.VariableMaxRealTime_AI[cabinP->values.ambientType][i];
                      cabinP->values.defrost.variableReason = (enVariableDefrostReason)i;
                      break;
                    }
                  }
                  else
#endif
                  {
                    if (cabinP->values.defrostDoorOpenedTimer < cabinC->parameters.Defrost.DoorOpenedIntervals[i])
                    {
                      cabinP->values.defrost.variableTime = cabinC->parameters.Defrost.VariableMaxRealTime[cabinP->values.ambientType][i];
                      cabinP->values.defrost.variableReason = (enVariableDefrostReason)i;
                      break;
                    }
                  }
		}
	}
	else
	{
		for (uint8_t i = 0; i < _IntervalSize; i++)
		{
#if defined (AI_COOL_V1)
                  if(cabinP->ai.isEnabled)
                  {
                    if (cabinP->values.defrostDoorOpenedTimer < cabinC->parameters.Defrost.DoorOpenedIntervals_AI[i])
                    {
                      cabinP->values.defrost.variableTime = cabinC->parameters.Defrost.VariableMaxRealTime_AI[_Ambient_0][i];
                      cabinP->values.defrost.variableReason = (enVariableDefrostReason)i;
                      break;
                    }
                  }
                  else
#endif
                  {
                    if (cabinP->values.defrostDoorOpenedTimer < cabinC->parameters.Defrost.DoorOpenedIntervals[i])
                    {
                      cabinP->values.defrost.variableTime = cabinC->parameters.Defrost.VariableMaxRealTime[_Ambient_0][i];
                      cabinP->values.defrost.variableReason = (enVariableDefrostReason)i;
                      break;
                    }
                  }
		}
	}
        
#if defined (AI_COOL_V1_5)
        RefrigeratorAIChangeDefrostIntervals(consts->rf, params->rf);
#endif
}


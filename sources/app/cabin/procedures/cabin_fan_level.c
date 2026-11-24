#include "cabin_fan_level.h"

void cabinUpdateFanLevel(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

	stLevel const *levelFan_FPtr;
	stDuty const *duty_FPtr;
	stDt const *dt_FPtr;
	unsigned char fanLevel = 0;

	switch (cabinP->sets.user.Mode)
	{
	case _CabinMode_FF:
		levelFan_FPtr = &cabinC->parameters.Level.levelFanFF;
		duty_FPtr = &cabinC->parameters.Level.dutyFF;
		dt_FPtr = &cabinC->parameters.Level.dtFF;
		break;

	case _CabinMode_FRZ:
		levelFan_FPtr = &cabinC->parameters.Level.levelFanFRZ;
		duty_FPtr = &cabinC->parameters.Level.dutyFRZ;
		dt_FPtr = &cabinC->parameters.Level.dtFRZ;
		break;

	default:
		cabinClearFanLevel(consts, params);
		return;
	}

	signed short sensortemp;
	signed short cutOutTemp = cabinP->sets.datas.fanCutOutTemp;

	switch (cabinP->states.Sensor)
	{
	case _Sensor_AirP:
	case _Sensor_AirP | _Sensor_EvaP:
	case _Sensor_AirP | _Sensor_EvaF:
		sensortemp = cabinP->components.airTempSensor.valueInCExt;
		break;

	case _Sensor_EvaP:
		sensortemp = cabinP->components.evaTempSensor.valueInCExt;
		break;

	case _Sensor_AirF:
	case _Sensor_EvaF:
	case _Sensor_AirF | _Sensor_EvaF:
	/*
	//case _Sensor_AirF:
	//case _Sensor_EvaF:
	//case _Sensor_AirF | _Sensor_EvaF:

	case _Sensor_EvaF:
	case _Sensor_AirF:
	case _Sensor_AirF | _Sensor_EvaP:
	//case _Sensor_AirP | _Sensor_EvaF:
	case _Sensor_AirF | _Sensor_EvaF:
	break;    // ozgur -
	*/
	default:
		cabinClearFanLevel(consts, params);
		return;
	}

	if (cabinP->values.duty.mean < duty_FPtr->low)
	{
		fanLevel = levelFan_FPtr->lowDuty[_lowDutyDT];

		for (unsigned char i = 0; i < _lowDutyDT; i++)
		{
			if (sensortemp < (cutOutTemp + dt_FPtr->lowDuty[i]))
			{
				fanLevel = levelFan_FPtr->lowDuty[i];
				break;
			}
		}
	}
	else if (cabinP->values.duty.mean > duty_FPtr->hgh)
	{
		fanLevel = levelFan_FPtr->hghDuty[_hghDutyDT];

		for (unsigned char i = 0; i < _hghDutyDT; i++)
		{
			if (sensortemp < (cutOutTemp + dt_FPtr->hghDuty[i]))
			{
				fanLevel = levelFan_FPtr->hghDuty[i];
				break;
			}
		}
	}
	else
	{
		fanLevel = levelFan_FPtr->medDuty[_medDutyDT];

		for (unsigned char i = 0; i < _medDutyDT; i++)
		{
			if (sensortemp < cutOutTemp + dt_FPtr->medDuty[i])
			{
				fanLevel = levelFan_FPtr->medDuty[i];
				break;
			}
		}
	}

	if (!cabinP->values.fanLevel)
	{
		cabinP->values.fanLevel = fanLevel;
		cabinP->values.fanLevelStayTimer = 0;
	}

	if (cabinP->values.fanLevelStayTimer >= cabinC->parameters.Fan.minStayTimeAtPresFanLevel)
	{
		if (cabinP->values.fanLevelBlockReduction)
		{
			if (fanLevel > cabinP->values.fanLevel)
			{
				cabinP->values.fanLevel = fanLevel;
				cabinP->values.fanLevelStayTimer = 0;
			}
		}
		else
		{
			if (fanLevel && (cabinP->values.fanLevel != fanLevel))
			{
				cabinP->values.fanLevel = fanLevel;
				cabinP->values.fanLevelStayTimer = 0;
			}
		}
	}

	if (cabinP->values.fanLevelStayTimer >= cabinC->parameters.Fan.maxStayTimeAtPresFanLevel)
	{
		cabinP->values.fanLevelBlockReduction = true;
		if (cabinP->values.fanLevel < 9)
		{
			cabinP->values.fanLevel++;
			cabinP->values.fanLevelStayTimer = 0;
		}
	}
}

void cabinClearFanLevel(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);

	cabinP->values.fanLevel = 0;
	cabinP->values.fanLevelStayTimer = 0;
	cabinP->values.fanLevelBlockReduction = 0;
}

void cabinUpdateFanLevelAmbient(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

	stAmbientLevel const *levelFan_FPtr;
	stAmbientDt const *dt_FPtr;
	unsigned char fanLevel = 0;

	switch (cabinP->sets.user.Mode)
	{
	case _CabinMode_FF:
		levelFan_FPtr = &cabinC->parameters.AmbientLevel.FanFF;
		dt_FPtr = &cabinC->parameters.AmbientLevel.FF;
		break;

	case _CabinMode_FRZ:
		levelFan_FPtr = &cabinC->parameters.AmbientLevel.FanFRZ;
		dt_FPtr = &cabinC->parameters.AmbientLevel.FRZ;
		break;

	default:
		cabinClearFanLevel(consts, params);
		return;
	}

	signed short sensortemp;
	signed short cutOutTemp = cabinP->sets.datas.fanCutOutTemp;

	enAmbientType ambientType = cabinP->values.ambientType;

	switch (cabinP->states.Sensor)
	{
	case _Sensor_AirP:
	case _Sensor_AirP | _Sensor_EvaP:
	case _Sensor_AirP | _Sensor_EvaF:
		sensortemp = cabinP->components.airTempSensor.valueInCExt;
		break;

	case _Sensor_EvaP:
		sensortemp = cabinP->components.evaTempSensor.valueInCExt;
		break;

	case _Sensor_AirF:
	case _Sensor_EvaF:
	case _Sensor_AirF | _Sensor_EvaF:

	default:
		cabinClearFanLevel(consts, params);
		return;
	}

	for (unsigned char i = 0; i < _ambientDt; i++)
	{
		fanLevel = levelFan_FPtr->level[ambientType][_ambientDt];
		if (sensortemp < (cutOutTemp + dt_FPtr->dt[ambientType][i]))
		{
			fanLevel = levelFan_FPtr->level[ambientType][i];
			break;
		}
	}

	if (!cabinP->values.fanLevel)
	{
		cabinP->values.fanLevel = fanLevel;
		cabinP->values.fanLevelStayTimer = 0;
	}

	if (cabinP->values.fanLevelStayTimer >= cabinC->parameters.Fan.minStayTimeAtPresFanLevel)
	{
		if (cabinP->values.fanLevelBlockReduction)
		{
			if (fanLevel > cabinP->values.fanLevel)
			{
				cabinP->values.fanLevel = fanLevel;
				cabinP->values.fanLevelStayTimer = 0;
			}
		}
		else
		{
			if (fanLevel && (cabinP->values.fanLevel != fanLevel))
			{
				cabinP->values.fanLevel = fanLevel;
				cabinP->values.fanLevelStayTimer = 0;
			}
		}
	}

	if (cabinP->values.fanLevelStayTimer >= cabinC->parameters.Fan.maxStayTimeAtPresFanLevel)
	{
		cabinP->values.fanLevelBlockReduction = true;
		if (cabinP->values.fanLevel < 4)
		{
			cabinP->values.fanLevel++;
			cabinP->values.fanLevelStayTimer = 0;
		}
	}
}

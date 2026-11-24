#include "cabin_gas_level.h"

void cabinUpdateGasLevel(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

	stLevel const *levelGas_FPtr;
	stDuty const *duty_FPtr;
	stDt const *dt_FPtr;
	unsigned char gasLevel = 0;

	switch (cabinP->sets.user.Mode)
	{
	case _CabinMode_FF:
		levelGas_FPtr = &cabinC->parameters.Level.levelGasFF;
		duty_FPtr = &cabinC->parameters.Level.dutyFF;
		dt_FPtr = &cabinC->parameters.Level.dtFF;
		break;

	case _CabinMode_FRZ:
		levelGas_FPtr = &cabinC->parameters.Level.levelGasFRZ;
		duty_FPtr = &cabinC->parameters.Level.dutyFRZ;
		dt_FPtr = &cabinC->parameters.Level.dtFRZ;
		break;

	default:
		cabinP->values.gasDesired.level = 0;
		return;
	}

	signed short sensortemp;
	signed short cutOutTemp = cabinP->sets.datas.coolCutOutTemp;

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
		cabinP->values.gasDesired.level = 5; // 0; ozgur
		return;
	}

	if (cabinP->values.duty.mean < duty_FPtr->low)
	{
		gasLevel = levelGas_FPtr->lowDuty[_lowDutyDT];

		for (unsigned char i = 0; i < _lowDutyDT; i++)
		{
			if (sensortemp < (cutOutTemp + dt_FPtr->lowDuty[i]))
			{
				gasLevel = levelGas_FPtr->lowDuty[i];
				break;
			}
		}
	}
	else if (cabinP->values.duty.mean > duty_FPtr->hgh)
	{
		gasLevel = levelGas_FPtr->hghDuty[_hghDutyDT];

		for (unsigned char i = 0; i < _hghDutyDT; i++)
		{
			if (sensortemp < (cutOutTemp + dt_FPtr->hghDuty[i]))
			{
				gasLevel = levelGas_FPtr->hghDuty[i];
				break;
			}
		}
	}
	else
	{
		gasLevel = levelGas_FPtr->medDuty[_medDutyDT];

		for (unsigned char i = 0; i < _medDutyDT; i++)
		{
			if (sensortemp < cutOutTemp + dt_FPtr->medDuty[i])
			{
				gasLevel = levelGas_FPtr->medDuty[i];
				break;
			}
		}
	}

	// DEBUG("s = %d _ c = %d _ m = %d _ gl = %d ",  sensortemp, cutOutTemp, cabinP->values.duty.mean, gasLevel );
	cabinP->values.gasDesired.level = gasLevel;
}

void cabinUpdateGasLevelAmbient(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

	stAmbientLevel const *levelGas_FPtr;
	stAmbientDt const *dt_FPtr;
	unsigned char gasLevel = 0;

	switch (cabinP->sets.user.Mode)
	{
	case _CabinMode_FF:
		levelGas_FPtr = &cabinC->parameters.AmbientLevel.GasFF;
		dt_FPtr = &cabinC->parameters.AmbientLevel.FF;
		break;

	case _CabinMode_FRZ:
		levelGas_FPtr = &cabinC->parameters.AmbientLevel.GasFRZ;
		dt_FPtr = &cabinC->parameters.AmbientLevel.FRZ;
		break;

	default:
		cabinP->values.gasDesired.level = 0;
		return;
	}

	signed short sensortemp;
	signed short cutOutTemp = cabinP->sets.datas.coolCutOutTemp;

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
		cabinP->values.gasDesired.level = 3;
		return;
	}

	for (unsigned char i = 0; i < _ambientDt; i++)
	{
		gasLevel = levelGas_FPtr->level[ambientType][_ambientDt];

		if (sensortemp < (cutOutTemp + dt_FPtr->dt[ambientType][i]))
		{
			gasLevel = levelGas_FPtr->level[ambientType][i];
			break;
		}
	}

	if (gasLevel)
	{
		if (gasLevel > cabinP->values.gasDesired.level)
			cabinP->values.gasDesired.level = gasLevel;
	}
	else
		cabinP->values.gasDesired.level = gasLevel;
}

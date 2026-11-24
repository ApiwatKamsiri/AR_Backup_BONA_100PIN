#include "cooler_types.h"
#include "../components/cooler_components.h"
#include "../system_state/cooler_system_state.h"
#include "app/cabin/cabin.h"  // For AI_COOL_V1 function prototypes
#include "app/refrigerator/refrigerator.h"

void coolerTypeSemiParallel(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
	stCooler_Ram *coolerP = params->cooler;
	const stCooler_Flash *coolerC = consts->cooler;

	enGasType desiredGasType;
	unsigned char desiredGasLevel = 0;
	enAmbientType ambientType;

	unsigned char desiredState = coolerGetSystemState(coolerC, coolerP);
	unsigned char desiredPriority = coolerGetStatePriority(coolerP, &coolerC->parameters.sysStates[desiredState]);

	/// NER
	coolerUpdateAmbientType(coolerC, coolerP);
	coolerUpdateHumidityType(coolerC, coolerP);

	ambientType = coolerP->parameters.ambientType;

	if (desiredState)
	{ // normal state
		desiredGasType = (enGasType)coolerC->parameters.sysStates[desiredState].gasSpecs[desiredPriority].type;

		if (coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
			desiredGasLevel = coolerP->parameters.gasDesired[0].level;
		else if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
			desiredGasLevel = coolerP->parameters.gasDesired[1].level;
	}
	else
	{ // default state
		desiredGasType = _GT_None;
		desiredGasLevel = 0;
	}

	if (coolerP->components.compressor.value)
	{
		if ((desiredGasType == _GT_Cold) ||
			(desiredGasType == _GT_Cold_FF) ||
			(desiredGasType == _GT_Cold_FRZ) ||
			(desiredGasType == _GT_PerformanceTest) ||
                        (desiredGasType == _GT_PerformanceTest2) ||
                        (desiredGasType == _GT_HotStart) ||
                        (desiredGasType == _GT_HotStart2) ||
			((desiredGasType == _GT_Hot) && (coolerC->type == _CoolerType_HOTGAS)))
		{
			coolerP->parameters.state = desiredState;
			coolerP->parameters.priority = desiredPriority;

			if (desiredGasType == _GT_Hot)
				coolerP->parameters.gasType = _GT_Hot;
			else
				coolerP->parameters.gasType = _GT_Cold;

			if (desiredGasLevel && !coolerP->parameters.gasLevel)
			{
				coolerP->parameters.gasLevel = desiredGasLevel;
				coolerP->parameters.gasLevelStayTimer = 0;
                                resetGasLevelStayTimers(consts, params);
			}

			if (coolerP->parameters.valvePositionChanged || getGasLevelStayTimer(consts, params) >= getGasLevelStayTime(consts, params, MIN_STAY_TIME))
			{
				if (coolerP->parameters.gasLevelBlockReduction)
				{
					if (desiredGasLevel > coolerP->parameters.gasLevel)
					{
						coolerP->parameters.gasLevel = desiredGasLevel;
						coolerP->parameters.gasLevelStayTimer = 0;
                                                //resetGasLevelStayTimers(consts, params);
						coolerP->parameters.valvePositionChanged = false;
					}
				}
				else
				{
					if (desiredGasLevel && (coolerP->parameters.gasLevel != desiredGasLevel))
					{
						coolerP->parameters.gasLevel = desiredGasLevel;
						coolerP->parameters.gasLevelStayTimer = 0;
                                                //resetGasLevelStayTimers(consts, params);
						coolerP->parameters.valvePositionChanged = false;
					}
				}
                                resetGasLevelStayTimers(consts, params);
			}
		}

		if ((getGasLevelStayTimer(consts, params) >= getGasLevelStayTime(consts, params, MAX_STAY_TIME)) 
                    || (refrigerator_R.Values.doorsAlertLevelIncrease && !refrigerator_R.Values.doorsAlertLevelIncreasePrev))
		{
                  refrigerator_R.Values.doorsAlertLevelIncreasePrev = refrigerator_R.Values.doorsAlertLevelIncrease;
                  refrigerator_R.Values.doorsAlertLevelIncrease = false;
                  
			if (coolerP->parameters.ambientType == _Ambient_NONE)
			{
				coolerP->parameters.gasLevelBlockReduction = true;
				if (coolerP->parameters.gasLevel < 9)
				{
					coolerP->parameters.gasLevel++;
					coolerP->parameters.gasLevelStayTimer = 0;
                                        resetGasLevelStayTimers(consts, params);
				}
			}
			else
			{
				coolerP->parameters.gasLevelBlockReduction = true;
				if (coolerP->parameters.gasLevel < _ambientLevel)
				{
					coolerP->parameters.gasLevel++;
					coolerP->parameters.gasLevelStayTimer = 0;
                                        resetGasLevelStayTimers(consts, params);
				}
			}
		}

		if (coolerP->parameters.compressorOnTimer < coolerC->parameters.compressor.timeStartUp)
		{
			coolerP->components.compressor.value =
				coolerC->parameters.compressor.speedStartUp;
			coolerP->parameters.gasLevelStayTimer = 0;
                        resetGasLevelStayTimers(consts, params);
		}
		else
		{
			if (coolerP->parameters.gasLevel)
			{
				if (params->rf->Values.doorsClosedState)
				{
					if (coolerP->parameters.compressorOnTimer >= coolerC->parameters.compressor.timeForMaxLevel_DC[ambientType])
					{
						if (coolerP->parameters.ambientType == _Ambient_NONE)
						{
							coolerP->parameters.gasLevelBlockReduction = true;
							coolerP->parameters.gasLevel = 9;
							coolerP->parameters.gasLevelStayTimer = 0;
                                                        resetGasLevelStayTimers(consts, params);
						}
						else
						{
							coolerP->parameters.gasLevelBlockReduction = true;
							coolerP->parameters.gasLevel = _compressorHotStartLevel + 1;
							coolerP->parameters.gasLevelStayTimer = 0;
                                                        resetGasLevelStayTimers(consts, params);
						}
					}
				}
				else
				{
					if (coolerP->parameters.compressorOnTimer >= coolerC->parameters.compressor.timeForMaxLevel_DO[ambientType] ||
						((coolerP->parameters.maxLevelCompressorOnTimerPrev >= coolerC->parameters.compressor.workTimeAtMaxLevel) &&
						 (coolerP->parameters.maxLevelCompressorOnTimerPrev >= 10)))
					{
						if (coolerP->parameters.ambientType == _Ambient_NONE)
						{
							coolerP->parameters.gasLevelBlockReduction = true;
							coolerP->parameters.gasLevel = 9;
							coolerP->parameters.gasLevelStayTimer = 0;
                                                        resetGasLevelStayTimers(consts, params);
						}
						else
						{
							coolerP->parameters.gasLevelBlockReduction = true;
							coolerP->parameters.gasLevel = _compressorHotStartLevel + 1;
							coolerP->parameters.gasLevelStayTimer = 0;
                                                        resetGasLevelStayTimers(consts, params);
						}
					}
				}

				if (((coolerC->components.ambientTempSensor.type != _TempSensorType_NONE) /*|| (params->rf->Values.I2CDevices.device.HumiditySensor)*/) && coolerP->parameters.ambientType != _Ambient_NONE)
				{
                                        if (coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
                                        {
#if defined (AI_COOL_V1)  
                                          if(params->rf->Cabin[0].ai.isEnabled/*if(refrigerator_R.Cabin[0].ai.isActivated)*/)// yeni algoritmada hemen devir degisikligi yapilsin istenildi.
                                            coolerP->components.compressor.value = CabinAIGetCompressorValAccordingToOnTimer(&consts->rf->Cabin[0], &params->rf->Cabin[0], consts->rf, params->rf); 
                                          else
#endif 
#if defined (AI_COOL_V1_5)
                                            coolerP->components.compressor.value = RefrigeratorAIChangeCompressorSpeed(consts->rf, params->rf);
#else
						coolerP->components.compressor.value =
							coolerC->parameters.compressor.FF.speedLevel[ambientType][coolerP->parameters.gasLevel - 1];
#endif                                        
                                        }
					else if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
                                        {
#if defined (AI_COOL_V1)  
                                          if(params->rf->Cabin[1].ai.isEnabled/*if(refrigerator_R.Cabin[0].ai.isActivated)*/)// yeni algoritmada hemen devir degisikligi yapilsin istenildi.
                                            coolerP->components.compressor.value = CabinAIGetCompressorValAccordingToOnTimer(&consts->rf->Cabin[1], &params->rf->Cabin[1], consts->rf, params->rf); 
                                          else
#endif 
#if defined (AI_COOL_V1_5)
                                            coolerP->components.compressor.value = RefrigeratorAIChangeCompressorSpeed(consts->rf, params->rf);
#else
                                          coolerP->components.compressor.value =
                                            coolerC->parameters.compressor.FRZ.speedLevel[ambientType][coolerP->parameters.gasLevel - 1];
#endif
                                        }
                                        else 
                                          coolerP->components.compressor.value =
							coolerC->parameters.compressor.FRZ.speedLevel[ambientType][coolerP->parameters.gasLevel - 1];                                    
				
				}
				else
				{
					if (coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
						coolerP->components.compressor.value =
							coolerC->parameters.compressor.speedLevelFF[coolerP->parameters.gasLevel - 1];
					else if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
						coolerP->components.compressor.value =
							coolerC->parameters.compressor.speedLevelFRZ[coolerP->parameters.gasLevel - 1];
				}
			}
		}

		if ((desiredGasType == _GT_No) ||
			(desiredGasType == _GT_None) ||
			(desiredGasType == _GT_Forced) ||
			(desiredGasType == _GT_Failed) ||
			((desiredGasType == _GT_Hot) && (coolerC->type != _CoolerType_HOTGAS)))
		{
			// If forced, failed or time elapsed
			if ((desiredGasType == _GT_Forced) ||
				(desiredGasType == _GT_Failed) ||
				(coolerP->parameters.compressorOnTimer >= coolerC->parameters.compressor.minOnTime))
			{
				coolerP->parameters.state = desiredState;
				coolerP->parameters.priority = desiredPriority;

				coolerP->parameters.gasType = _GT_No;
				coolerP->parameters.gasLevel = 0;

				coolerP->components.compressor.value = 0;
				coolerP->parameters.gasLevelStayTimer = 0;
                                resetGasLevelStayTimers(consts, params);
				coolerP->parameters.gasLevelBlockReduction = 0;
				coolerP->parameters.compressorOffTimer = 0;
			}
		}
	}
	else
	{
		coolerP->parameters.gasType = _GT_No;
		coolerP->parameters.gasLevel = 0;

		coolerP->components.compressor.value = 0;
		coolerP->parameters.gasLevelStayTimer = 0;
                resetGasLevelStayTimers(consts, params);               
		coolerP->parameters.valveStateStayTimer = 0;
		coolerP->parameters.gasLevelBlockReduction = 0;

		if ((desiredGasType == _GT_No) ||
			(desiredGasType == _GT_None) ||
			(desiredGasType == _GT_Forced) ||
			(desiredGasType == _GT_Failed) ||
			((desiredGasType == _GT_Hot) && (coolerC->type != _CoolerType_HOTGAS)))
		{
			coolerP->parameters.state = desiredState;
			coolerP->parameters.priority = desiredPriority;
		}

		if ((desiredGasType == _GT_Cold) ||
			(desiredGasType == _GT_Cold_FF) ||
			(desiredGasType == _GT_Cold_FRZ) ||
			(desiredGasType == _GT_PerformanceTest) ||
                        (desiredGasType == _GT_PerformanceTest2) ||
                        (desiredGasType == _GT_HotStart) ||
                        (desiredGasType == _GT_HotStart2) ||
			((desiredGasType == _GT_Hot) && (coolerC->type == _CoolerType_HOTGAS)))
		{
			// If time elapsed
			if (coolerP->parameters.compressorOffTimer >= coolerC->parameters.compressor.minOffTime)
			{
				coolerP->parameters.state = desiredState;
				coolerP->parameters.priority = desiredPriority;

				if (desiredGasType == _GT_Hot)
					coolerP->parameters.gasType = _GT_Hot;
				else
					coolerP->parameters.gasType = _GT_Cold;

				coolerP->parameters.gasLevel = desiredGasLevel;
                                resetGasLevelStayTimers(consts, params);
				coolerP->components.compressor.value = coolerC->parameters.compressor.speedStartUp;
				coolerP->parameters.gasLevelStayTimer = 0;
				coolerP->parameters.gasLevelBlockReduction = 0;
				coolerP->parameters.compressorOnTimer = 0;
			}
		}
	}

	coolerUpdateValveState(coolerC, coolerP, coolerP->parameters.state, coolerP->parameters.gasType, coolerP->parameters.gasLevel);
	coolerUpdateCondFanSpeed(coolerC, coolerP, coolerP->parameters.gasLevel);

	if (coolerP->parameters.silent)
	{
		if (coolerP->components.compressor.value > coolerC->parameters.compressor.speedSilent)
			coolerP->components.compressor.value = coolerC->parameters.compressor.speedSilent;
		if (coolerP->components.condenserFan.speedOrPWM > (coolerC->parameters.condenserFan.speedSilent))
			coolerP->components.condenserFan.speedOrPWM = coolerC->parameters.condenserFan.speedSilent;
	}
}

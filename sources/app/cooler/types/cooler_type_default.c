#include "cooler_types.h"
#include "../components/cooler_components.h"
#include "../system_state/cooler_system_state.h"
#include "app/cabin/cabin.h"  // For AI_COOL_V1 function prototypes
#include "app/refrigerator/refrigerator.h"

void coolerTypeDefault2(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
	stCooler_Ram *coolerP = params->cooler;
	const stCooler_Flash *coolerC = consts->cooler;

	enGasType desiredGasType;
	unsigned char desiredGasLevel;
	enAmbientType ambientType;

	unsigned char desiredState = coolerGetSystemState(coolerC, coolerP);
	unsigned char desiredPriority = coolerGetStatePriority(coolerP, &coolerC->parameters.sysStates[desiredState]);
	/// NER
	coolerUpdateAmbientType(coolerC, coolerP);
	coolerUpdateHumidityType(coolerC, coolerP);

	ambientType = coolerP->parameters.ambientType;

        /*** APW : Add cooler operation in CT ***/
        if (params->rf->Cabin[0].states.Work == _Work_CommercialTestMode ||
            params->rf->Cabin[1].states.Work == _Work_CommercialTestMode)
        {
          // Case : 1
          if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
          {
            if(params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer >=
                coolerC->parameters.compressor.minOntimerFirstCommercial)
            {
              params->rf->workMode.values.CommercialTest.FLAP_Fan_Request = 1;  
              params->rf->workMode.values.CommercialTest.FF_Fan_Request = 1;
              params->rf->workMode.values.CommercialTest.FRZ_Fan_Request = 1;
              params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer = 0;
              params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial = 0;
              params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_ValveTest;
            }
            else
            {
              if(refrigerator_R.workMode.values.ARMModeError.PingTestError == true)
                coolerP->components.compressor.value = 0;
              else
                coolerP->components.compressor.value = 
                  coolerC->parameters.compressor.speedFirstCommercial;
            }
          }
          // Case : 2 (Don't need for Fuji/Hoshi)
          if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest)
          {
              if(coolerC->parameters.minValveSwitchtimerCommercial == 0 &&
                  coolerC->parameters.minOntimerSecondCommercial == 0)
                params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_JudgmentResult;
          }
          
          // Case : 4 (Don't need for Fuji/Hoshi)
          if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
          {
              params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_OK;
              coolerP->parameters.compressorOnTimer = 1;        //Reset compressor on time to force skip oil and compressor on after workMode is Normal (Under _UI_Commercial_Test)
          }
          
          if(coolerP->components.compressor.value)
            coolerP->components.condenserFan.speedOrPWM = 100;
          else
            coolerP->components.condenserFan.speedOrPWM = 0;
          
          return;
        }
        
        if (params->rf->Cabin[0].states.Work == _Work_CommercialDefrostTestMode ||
            params->rf->Cabin[1].states.Work == _Work_CommercialDefrostTestMode)
              return;
        /*** APW : Add cooler operation in CT ***/
        
	if (desiredState)
	{ // normal state
		desiredGasType = (enGasType)coolerC->parameters.sysStates[desiredState].gasSpecs[desiredPriority].type;
		desiredGasLevel = coolerP->parameters.gasDesired[desiredPriority].level;
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
                                resetGasLevelStayTimers(consts, params);  //TODO 539 ???
			}

			if (coolerP->parameters.gasLevelStayTimer >= getGasLevelStayTime(consts, params, MIN_STAY_TIME))
			{
				if (coolerP->parameters.gasLevelBlockReduction)
				{
					if (desiredGasLevel > coolerP->parameters.gasLevel)
					{
						coolerP->parameters.gasLevel = desiredGasLevel;
						coolerP->parameters.gasLevelStayTimer = 0;
					}
				}
				else
				{
					if (desiredGasLevel && (coolerP->parameters.gasLevel != desiredGasLevel))
					{
						coolerP->parameters.gasLevel = desiredGasLevel;
						coolerP->parameters.gasLevelStayTimer = 0;
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
                        resetGasLevelStayTimers(consts, params); // 539 TODO
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

				if (coolerC->components.ambientTempSensor.type != _TempSensorType_NONE &&
					coolerP->parameters.ambientType != _Ambient_NONE)
				{
#if defined (AI_COOL_V1)  
                                      if(params->rf->Cabin[0].ai.isEnabled/*if(refrigerator_R.Cabin[0].ai.isActivated)*/)// yeni algoritmada hemen devir degisikligi yapilsin istenildi.
                                        coolerP->components.compressor.value = CabinAIGetCompressorValAccordingToOnTimer(&consts->rf->Cabin[0], &params->rf->Cabin[0], consts->rf, params->rf); 
                                      else
#endif 
#if defined (AI_COOL_V1_5)
                                        coolerP->components.compressor.value = RefrigeratorAIChangeCompressorSpeed(consts->rf, params->rf);
#else
                                      {
                                        if(coolerC->parameters.compressor.FF.maxDialSpeedLevelUpLH == true &&
                                            refrigerator_R.Cabin[1].sets.work.functions.CompSpeedLevelNeedUpLH == true)
                                        {
                                          coolerP->parameters.gasLevel = _compressorHotStartLevel + 1;
                                        }
                                        
					coolerP->components.compressor.value =
						coolerC->parameters.compressor.FF.speedLevel[ambientType][coolerP->parameters.gasLevel - 1];
                                      }
#endif
				}
				else
				{
					coolerP->components.compressor.value =
						coolerC->parameters.compressor.speedLevelFF[coolerP->parameters.gasLevel - 1];
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
        
        //Pulldown
        if((params->rf->Cabin[0].components.airTempSensor.valueInCExt < params->rf->Cabin[0].sets.datas.coolCutOutTemp) || 
           (params->rf->Cabin[1].components.airTempSensor.valueInCExt < params->rf->Cabin[1].sets.datas.coolCutOutTemp))
          params->rf->Cooler.components.compressor.CompressorHotstartF = false;
        
        if((params->rf->Cooler.components.compressor.CompressorHotstartF)&& (coolerP->components.compressor.value))
        {
          if(params->rf->workMode.values.prodTest.minTimer < coolerC->parameters.compressor.minOnTimePullDownStage_1)
          {
            params->rf->Cooler.components.compressor.value = coolerC->parameters.compressor.speedPullDownStage_1;
            coolerP->components.condenserFan.speedOrPWM = coolerC->parameters.condenserFan.speedPullDownStage_1;
          }
          else if (params->rf->workMode.values.prodTest.minTimer < coolerC->parameters.compressor.minOnTimePullDownStage_2)
          {
            params->rf->Cooler.components.compressor.value = coolerC->parameters.compressor.speedPullDownStage_2;
            coolerP->components.condenserFan.speedOrPWM = coolerC->parameters.condenserFan.speedPullDownStage_2;
          }
          else params->rf->Cooler.components.compressor.CompressorHotstartF = false;
        }
        //Pulldown
        
        //For default type, we force _Work_CommercialTestMode for minOntimerFirstCommercial.
        //After minOntimerFirstCommercial finish, we will goto _Work_Normal (Case default cooling control)
        if(refrigerator_R.ui.states.commercial.status != _UI_Commercial_None &&
            refrigerator_R.workMode.values.ARMModeError.PingTestError == true)
          coolerP->components.compressor.value = 0;
        //For CT mode
}
void coolerTypeDefault(const tsCoolerProcConsts *consts, tsCoolerProcParams *params)
{
	stCooler_Ram *coolerP = params->cooler;
	const stCooler_Flash *coolerC = consts->cooler;

	enGasType gasType;
	unsigned char gasLevel;

	coolerP->parameters.state = coolerGetSystemState(coolerC, coolerP);
	coolerP->parameters.priority = coolerGetStatePriority(coolerP, &coolerC->parameters.sysStates[coolerP->parameters.state]);

	if (coolerP->parameters.state == 0)
	{ // default state
		gasType = _GT_None;
		gasLevel = 0;
	}
	else
	{ // normal state
		gasType = (enGasType)coolerC->parameters.sysStates[coolerP->parameters.state].gasSpecs[coolerP->parameters.priority].type;
		gasLevel = coolerP->parameters.gasDesired[coolerP->parameters.priority].level;
	}

	// gasType and gasLevel Correction
	switch (gasType)
	{
	case _GT_Cold:
	case _GT_Cold_FF:
	case _GT_Cold_FRZ:
		gasType = _GT_Cold;
		break;

	case _GT_Hot:
		if (coolerC->type == _CoolerType_HOTGAS)
		{
			gasType = _GT_Hot;
		}
		else
		{
			gasType = _GT_No;
			gasLevel = 0;
		}
		break;

	case _GT_Failed:
	case _GT_Forced:
		gasLevel = 0;
		break;

	case _GT_No:
	case _GT_None:
	default:
		gasType = _GT_No;
		gasLevel = 0;
		break;
	}

	if ((gasType == _GT_Failed))
	{
		coolerP->components.compressor.value = 0;
	}

	if (coolerP->components.compressor.value)
	{
		if (!coolerP->parameters.gasLevel)
		{
			coolerP->parameters.gasLevel = gasLevel;
			coolerP->parameters.gasLevelStayTimer = 0;
                        resetGasLevelStayTimers(consts, params);
		}

		if (coolerP->parameters.gasLevelStayTimer >= getGasLevelStayTime(consts, params, MIN_STAY_TIME))
		{
			if (coolerP->parameters.gasLevelBlockReduction)
			{
				if (gasLevel > coolerP->parameters.gasLevel)
				{
					coolerP->parameters.gasLevel = gasLevel;
					coolerP->parameters.gasLevelStayTimer = 0;
				}
			}
			else
			{
				if (gasLevel && (coolerP->parameters.gasLevel != gasLevel))
				{
					coolerP->parameters.gasLevel = gasLevel;
					coolerP->parameters.gasLevelStayTimer = 0;
				}
			}
                        resetGasLevelStayTimers(consts, params);
		}

		if ((getGasLevelStayTimer(consts, params) >= getGasLevelStayTime(consts, params, MAX_STAY_TIME)) 
                    || (refrigerator_R.Values.doorsAlertLevelIncrease && !refrigerator_R.Values.doorsAlertLevelIncreasePrev))
		{
                  refrigerator_R.Values.doorsAlertLevelIncreasePrev = refrigerator_R.Values.doorsAlertLevelIncrease;
                  refrigerator_R.Values.doorsAlertLevelIncrease = false;
                  
			coolerP->parameters.gasLevelBlockReduction = true;
			if (coolerP->parameters.gasLevel < 9)
			{
				coolerP->parameters.gasLevel++;
				coolerP->parameters.gasLevelStayTimer = 0;
                                resetGasLevelStayTimers(consts, params);
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
			coolerP->components.compressor.value =
				coolerC->parameters.compressor.speedLevel[coolerP->parameters.gasLevel - 1];
		}

		if (!gasLevel && (coolerP->parameters.compressorOnTimer >= coolerC->parameters.compressor.minOnTime))
		{
			gasType = _GT_No;
			coolerP->parameters.gasLevel = 0;
			coolerP->components.compressor.value = 0;
			coolerP->parameters.gasLevelStayTimer = 0;
                        resetGasLevelStayTimers(consts, params);
			coolerP->parameters.gasLevelBlockReduction = 0;
			coolerP->parameters.compressorOffTimer = 0;
		}
	}
	else
	{
		gasType = _GT_No;
		coolerP->parameters.gasLevel = 0;
		coolerP->components.compressor.value = 0;
		coolerP->parameters.gasLevelStayTimer = 0;
                resetGasLevelStayTimers(consts, params);
                coolerP->parameters.gasLevelStayTimerFF = 0;
		coolerP->parameters.gasLevelStayTimerFRZ = 0;   
		coolerP->parameters.gasLevelBlockReduction = 0;
		// coolerP->parameters.compressorOnTimer = 0;

		if (gasLevel && (coolerP->parameters.compressorOffTimer >= coolerC->parameters.compressor.minOffTime))
		{
			coolerP->parameters.gasLevel = gasLevel;
                        resetGasLevelStayTimers(consts, params);
			coolerP->components.compressor.value = coolerC->parameters.compressor.speedStartUp;
			coolerP->parameters.gasLevelStayTimer = 0;
			coolerP->parameters.gasLevelBlockReduction = 0;
			coolerP->parameters.compressorOnTimer = 0;
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

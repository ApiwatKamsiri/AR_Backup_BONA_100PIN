#include "cooler_types.h"
#include "../components/cooler_components.h"
#include "../system_state/cooler_system_state.h"
#include "app/cabin/cabin.h"  // For AI_COOL_V1 function prototypes
#include "app/refrigerator/refrigerator.h"

void coolerTypeParallel(const tsCoolerProcConsts * consts, tsCoolerProcParams * params)
{
	stCooler_Ram *coolerP = params->cooler;
	const stCooler_Flash * coolerC = consts->cooler;
	
	enGasType desiredGasType;
	unsigned char desiredGasLevel = 0;
	enAmbientType ambientType;

	unsigned char desiredState = coolerGetSystemState(coolerC, coolerP);
	unsigned char desiredPriority = coolerGetStatePriority(coolerP, &coolerC->parameters.sysStates[desiredState]);
	/// NER
	coolerUpdateAmbientType(coolerC, coolerP);
	coolerUpdateHumidityType(coolerC, coolerP);

	ambientType = coolerP->parameters.ambientType;
	
        /*** APW : Add CT heater software ***/
        if (params->rf->Cabin[0].states.Work == _Work_CommercialTestMode ||
            params->rf->Cabin[1].states.Work == _Work_CommercialTestMode)
        {
           //if(coolerC->components.compressor.type == _CompressorType_UART)
           {
              // Case : 1
              if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
              {
                  if(params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer >=
                     coolerC->parameters.compressor.minOntimerFirstCommercial)
                  {
                    coolerP->components.solenoidValve[0].positionIndex = _ALL_CLOSE;    //APW : Add pump out
                    params->rf->workMode.values.CommercialTest.FF_Valve_Request = 1;    //APW : Add pump out
                    params->rf->workMode.values.CommercialTest.FRZ_Valve_Request = 0;   //APW : Add pump out
                    params->rf->workMode.values.CommercialTest.FF_Fan_Request = 1;
                    params->rf->workMode.values.CommercialTest.FRZ_Fan_Request = 1;
                    params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer = 0;
                    params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial = 0;
                    params->rf->workMode.values.CommercialTest.SensorTempRecord = 1;
                    params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_ValveTest;
                  }
                  else
                  {
                    coolerP->components.solenoidValve[0].positionIndex = _ALL_OPEN;

                    if(refrigerator_R.workMode.values.ARMModeError.PingTestError == true)
                      coolerP->components.compressor.value = 0;
                    else
                      coolerP->components.compressor.value = 
                        coolerC->parameters.compressor.speedFirstCommercial;
                  }
              }
              
              // Case : 2
              if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest)
              {
                  if(params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer >=
                     coolerC->parameters.minValveSwitchtimerCommercial &&
                     coolerP->parameters.ValveAndCompressorCommercialOnTimer <
                     coolerC->parameters.minOntimerSecondCommercial)           
                  
                  {
                     params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial += 1;
                     params->rf->workMode.values.CommercialTest.SensorTempRecord = 1;
                        if(params->rf->workMode.values.CommercialTest.FRZ_Valve_Request == 1)   //APW : Add pump out
                        {
                          params->rf->workMode.values.CommercialTest.FF_Valve_Request = 1;      //APW : Add pump out
                          params->rf->workMode.values.CommercialTest.FRZ_Valve_Request = 0;     //APW : Add pump out
                          params->rf->workMode.values.CommercialTest.FF_Fan_Request = 1;
                          params->rf->workMode.values.CommercialTest.FRZ_Fan_Request = 1;
                          params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer = 0;
                        }
                        else
                        {
                          params->rf->workMode.values.CommercialTest.FF_Valve_Request = 0;      //APW : Add pump out
                          params->rf->workMode.values.CommercialTest.FRZ_Valve_Request = 1;     //APW : Add pump out
                          params->rf->workMode.values.CommercialTest.FF_Fan_Request = 1;
                          params->rf->workMode.values.CommercialTest.FRZ_Fan_Request = 1;
                          params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer = 0;
                        }
                  }
                  else if(coolerP->parameters.ValveAndCompressorCommercialOnTimer >=
                          coolerC->parameters.minOntimerSecondCommercial)
                  {
                    params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer = 0;
                    params->rf->workMode.values.CommercialTest.FF_Fan_Request = 0;
                    params->rf->workMode.values.CommercialTest.FRZ_Fan_Request = 0;
                    //Record previous valve
                    if(params->rf->workMode.values.CommercialTest.FF_Valve_Request)
                      params->rf->workMode.values.CommercialTest.PrvValvePositionAtValveTest = 0;     //0: FF position
                    if(params->rf->workMode.values.CommercialTest.FRZ_Valve_Request)
                      params->rf->workMode.values.CommercialTest.PrvValvePositionAtValveTest = 1;     //1: FRZ position
                    //Reset valve request
                    params->rf->workMode.values.CommercialTest.FF_Valve_Request = 0;            //APW : Add pump out
                    params->rf->workMode.values.CommercialTest.FRZ_Valve_Request = 0;           //APW : Add pump out
                    
                    if(params->rf->workMode.values.CommercialTest.LastSensorTempRecord == 0)
                    {
                        params->rf->workMode.values.CommercialTest.LastSensorTempRecord = 1;
                        params->rf->workMode.values.CommercialTest.SensorTempRecord = 1;
                        params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial += 1;
                    }
                    else
                    {
                        if(params->rf->workMode.values.CommercialTest.ValveWorkStageCommercial == 0 &&
                           params->rf->workMode.values.CommercialTest.SensorTempRecord == 0)
                            params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_JudgmentResult;
                    }
                  }
                  else
                  {                                       
                        if(params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer < 1)    //APW : Add pump out
                        {
                          coolerP->components.solenoidValve[0].positionIndex = _ALL_CLOSE;              //APW : Add pump out
                          if(refrigerator_R.workMode.values.ARMModeError.PingTestError == true)
                            coolerP->components.compressor.value = 0;
                          else
                            coolerP->components.compressor.value = 
                              coolerC->parameters.compressor.speedPumpOut;
                        }

                        else                                                                            //APW : Add pump out
                        {                                                                               //APW : Add pump out
                          if(refrigerator_R.workMode.values.ARMModeError.PingTestError == true)
                            coolerP->components.compressor.value = 0;
                          else
                            coolerP->components.compressor.value =                                        
                              coolerC->parameters.compressor.speedSecondCommercialValveTest;
                          
                              if(params->rf->workMode.values.CommercialTest.FRZ_Valve_Request == 1)     //APW : Add pump out
                                  coolerP->components.solenoidValve[0].positionIndex = _FRZ_OPEN;       //APW : Add pump out
                              else                                                                      //APW : Add pump out
                                  coolerP->components.solenoidValve[0].positionIndex = _FF_OPEN;        //APW : Add pump out
                        }                                                                               //APW : Add pump out      
                  }
              }
              
              // Case : 4
              if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
              {
                  if(params->rf->workMode.values.CommercialTest.FF_Valve_Request == 0 &&
                      params->rf->workMode.values.CommercialTest.FRZ_Valve_Request == 0 &&
                        params->rf->workMode.values.prodTest.minTimer <= 70)
                  {
                    if(params->rf->workMode.values.CommercialTest.PrvValvePositionAtValveTest)  //1: Before FRZ position
                    {
                      params->rf->workMode.values.CommercialTest.FF_Valve_Request = 1;
                      params->rf->workMode.values.CommercialTest.FRZ_Valve_Request = 0;
                    }
                    else                                                                        //0: Before FF position
                    {
                      params->rf->workMode.values.CommercialTest.FF_Valve_Request = 0;
                      params->rf->workMode.values.CommercialTest.FRZ_Valve_Request = 1;
                    }
                      params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer = 0;
                  }
                  
                  else if(params->rf->workMode.values.CommercialTest.FF_Valve_Request == 1 &&
                            params->rf->workMode.values.CommercialTest.FRZ_Valve_Request == 0 &&
                              params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer >= 1 &&
                                params->rf->workMode.values.prodTest.minTimer <= 70)
                  {
                      if(params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer >= 6)
                      {
                        params->rf->workMode.values.CommercialTest.FF_Valve_Request = 0;
                        params->rf->workMode.values.CommercialTest.FRZ_Valve_Request = 1;
                        params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer = 0;
                      }
                      else
                      {
                        params->rf->workMode.values.CommercialTest.FF_Fan_Request = 1;
                        params->rf->workMode.values.CommercialTest.FRZ_Fan_Request = 1;
                        coolerP->components.solenoidValve[0].positionIndex = _FF_OPEN;
                        if(refrigerator_R.workMode.values.ARMModeError.PingTestError == true)
                          coolerP->components.compressor.value = 0;
                        else
                          coolerP->components.compressor.value =                                        
                            coolerC->parameters.compressor.speedSecondCommercialValveTest;
                      }
                  }
                  
                  else if(params->rf->workMode.values.CommercialTest.FF_Valve_Request == 0 &&
                            params->rf->workMode.values.CommercialTest.FRZ_Valve_Request == 1 &&
                              params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer >= 1 &&
                                params->rf->workMode.values.prodTest.minTimer <= 70)
                  {
                      if(params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer >= 11)
                      {
                        params->rf->workMode.values.CommercialTest.FF_Valve_Request = 1;
                        params->rf->workMode.values.CommercialTest.FRZ_Valve_Request = 0;
                        params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer = 0;
                      }
                      else
                      {
                        params->rf->workMode.values.CommercialTest.FRZ_Fan_Request = 1;
                        params->rf->workMode.values.CommercialTest.FF_Fan_Request = 1;
                        coolerP->components.solenoidValve[0].positionIndex = _FRZ_OPEN;
                        if(refrigerator_R.workMode.values.ARMModeError.PingTestError == true)
                          coolerP->components.compressor.value = 0;
                        else
                          coolerP->components.compressor.value =                                        
                            coolerC->parameters.compressor.speedSecondCommercialValveTest;
                      }
                  }
                  
                  else if(params->rf->workMode.values.CommercialTest.MinOnValveCommercialTimer < 1 &&
                            params->rf->workMode.values.prodTest.minTimer <= 70)
                  {
                      coolerP->components.solenoidValve[0].positionIndex = _ALL_CLOSE;
                      if(refrigerator_R.workMode.values.ARMModeError.PingTestError == true)
                        coolerP->components.compressor.value = 0;
                      else
                        coolerP->components.compressor.value =                                        
                          coolerC->parameters.compressor.speedPumpOut;
                  }
                  
                  else
                  {
                      params->rf->workMode.values.CommercialTest.state = _WM_CommercialTest_State_OK;
                      coolerP->parameters.compressorOnTimer = 1;        //Reset compressor on time to force skip oil and compressor on after workMode is Normal (Under _UI_Commercial_Test)
                  }
              }
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
        /*** APW : Add CT heater software ***/
        
	if (params->rf->Cabin[0].states.Work == _Work_PumpOut || params->rf->Cabin[1].states.Work == _Work_PumpOut)
	{
          
                if (consts->rf->Sets.configs.functions.PumpOut.type == _PumpOut_OpenFF)
                {
                  coolerP->components.solenoidValve[0].positionIndex = _FF_OPEN;
                  coolerP->components.condenserFan.speedOrPWM = 0;
                  coolerP->components.compressor.value = coolerC->parameters.compressor.FF.speedLevel[ambientType][coolerP->parameters.gasLevel - 1];
                }
          
                if (consts->rf->Sets.configs.functions.PumpOut.type == _PumpOut_OpenALL || pumpOut)
                {
                  coolerP->components.solenoidValve[0].positionIndex = _ALL_CLOSE;
                  coolerP->components.condenserFan.speedOrPWM = coolerC->parameters.condenserFan.speedPumpOut;
                  coolerP->components.compressor.value = coolerC->parameters.compressor.speedPumpOut;
                }
		else if (consts->rf->Sets.configs.functions.PumpOut.type == _PumpOut_Close)
		{
                  coolerP->components.solenoidValve[0].positionIndex = _ALL_CLOSE;
                  coolerP->components.condenserFan.speedOrPWM = coolerC->parameters.condenserFan.speedPumpOut;
                  coolerP->components.compressor.value = coolerC->parameters.compressor.speedPumpOut;
		}
                

		return;
	}

	if (desiredState)
	{ // normal state
		desiredGasType = (enGasType)coolerC->parameters.sysStates[desiredState].gasSpecs[desiredPriority].type;

		if (coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
			desiredGasLevel = coolerP->parameters.gasDesired[0].level;
		else if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
			desiredGasLevel = coolerP->parameters.gasDesired[1].level;
                else
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
                        
                        if (coolerP->parameters.valvePositionChanged)
                          coolerP->parameters.gasLevelBlockReduction = false;

			if (coolerP->parameters.valvePositionChanged || getGasLevelStayTimer(consts, params) >= getGasLevelStayTime(consts, params, MIN_STAY_TIME))
			{
				if (coolerP->parameters.gasLevelBlockReduction)
				{
					if (desiredGasLevel > coolerP->parameters.gasLevel)
					{
						coolerP->parameters.gasLevel = desiredGasLevel;
						coolerP->parameters.gasLevelStayTimer = 0;
						coolerP->parameters.valvePositionChanged = false;
                                                resetGasLevelStayTimers(consts, params);
					}
				}
				else
				{
					if (desiredGasLevel && (coolerP->parameters.gasLevel != desiredGasLevel))
					{
						coolerP->parameters.gasLevel = desiredGasLevel;
						coolerP->parameters.gasLevelStayTimer = 0;
						coolerP->parameters.valvePositionChanged = false;
                                                resetGasLevelStayTimers(consts, params);
					}
				}
			}
		}

		if ((getGasLevelStayTimer(consts, params) >= getGasLevelStayTime(consts, params, MAX_STAY_TIME)) 
                    || (refrigerator_R.Values.doorsAlertLevelIncrease && !refrigerator_R.Values.doorsAlertLevelIncreasePrev))
		{
                  refrigerator_R.Values.doorsAlertLevelIncreasePrev = refrigerator_R.Values.doorsAlertLevelIncrease;
                  refrigerator_R.Values.doorsAlertLevelIncrease = false;
                  
			if (coolerP->parameters.ambientType == _Ambient_NONE)
			{
				coolerP->parameters.gasLevelBlockReduction = true; //TODO 539
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
                                              ((params->rf->IceMachine.values.QuickIce60 == true)&&(params->rf->IceMachine.values.quickIce60MaximumTimer < params->rf->IceMachine.sets.quickIce60MaximumTime - 5)) ||       //<<QuickIce60
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
                                          {
                                            if(coolerC->parameters.compressor.FF.maxDialSpeedLevelUpLH == true &&
                                                refrigerator_R.Cabin[0].sets.work.functions.CompSpeedLevelNeedUpLH == true)
                                            {
                                              coolerP->parameters.gasLevel = _compressorHotStartLevel + 1;
                                            }
                                            
                                            coolerP->components.compressor.value =
                                                    coolerC->parameters.compressor.FF.speedLevel[ambientType][coolerP->parameters.gasLevel - 1];
                                          }
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
                                          {
                                            if(coolerC->parameters.compressor.FRZ.maxDialSpeedLevelUpLH == true &&
                                                refrigerator_R.Cabin[1].sets.work.functions.CompSpeedLevelNeedUpLH == true)
                                            {
                                              coolerP->parameters.gasLevel = _compressorHotStartLevel + 1;
                                            }
                                            
                                              coolerP->components.compressor.value =
                                                    coolerC->parameters.compressor.FRZ.speedLevel[ambientType][coolerP->parameters.gasLevel - 1];
                                          }
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
                                        else 
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
                          
                                pumpOut = 0;
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
                                
                              /*  if(coolerP->components.solenoidValve[0].positionIndex  == _FF_OPEN && !pumpOut)
                                {
                                  pumpOut = 1;
                                  params->rf->Cabin[0].states.Work = _Work_PumpOut;
                                  params->rf->Cabin[0].stateTimers.PumpOut = consts->rf->Cabin[0].parameters.Cool.pumpOutTime;
                                  coolerP->parameters.gasLevel = coolerP->parameters.gasDesired[0].level;
                                }
                                else if(coolerP->components.solenoidValve[0].positionIndex  == _FRZ_OPEN && !pumpOut)
                                {
                                  pumpOut = 1;
                                  params->rf->Cabin[1].states.Work = _Work_PumpOut;
                                  params->rf->Cabin[1].stateTimers.PumpOut = consts->rf->Cabin[1].parameters.Cool.pumpOutTime;
                                  coolerP->parameters.gasLevel = coolerP->parameters.gasDesired[1].level;
                                }
                                
                                if (coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
                                  desiredGasLevel = coolerP->parameters.gasDesired[0].level;
                                else if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
                                  desiredGasLevel = coolerP->parameters.gasDesired[1].level;
                                else
                                  desiredGasLevel = coolerP->parameters.gasDesired[1].level;*/

				coolerP->parameters.gasLevel = desiredGasLevel;
                                resetGasLevelStayTimers(consts, params);
                                
                                if (!(params->rf->Cabin[0].states.Work == _Work_PumpOut || 
                                    params->rf->Cabin[1].states.Work == _Work_PumpOut))
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
        
        if((params->rf->Cooler.components.compressor.CompressorHotstartF) && (coolerP->components.compressor.value))
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
}
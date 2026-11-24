#include "app/refrigerator/rf_configs.h"
#include "app/cabin/defaults/cabin_defaults.h"
#include "metadata.h"
#include "system.h"

#define _StockNoSize 10
#define _StockNo 6133102607     // Name of Project (For BONA H30 VN DPJ)
#define _StockVersion 0  	// Version      
#define _StockRevision 13 	// Revision Massprod VN model -> Update new core software and improve all heater for dew.
#define _CoreVersion UXPURE_CORE_VER    //APW : When need to change core please change in system.h                 
#define _CoreRevision UXPURE_CORE_REV   //APW : When need to change core please change in system.h  
#define _ProductStockCode 7000000000
#define _PowerVolInput 220      //110V

#pragma location = 0x00037000      //OCE OTA Changes
const stRefrigerator_Flash refrigerator_F =
{
		.Stock = { // UDAQ name.
			.No[0] = (_StockNo % (10000)),
			.No[1] = (_StockNo / 10000) % 10000,
			.No[2] = ((_StockNo / 10000) / 10000) % 10000,
			.Version = _StockVersion,
			.Revision = _StockRevision,   
			.CoreVersion = _CoreVersion,    //For record in EEPROM and display
			.CoreRevision = _CoreRevision,  //For record in EEPROM and display
		},

		.Sets = {//Enable&Disable Mode//  0 is Disable, 1 is Enable
			.configs = {
				.functions = {
					.Sabbath = 0, // If we need sabbath function should inform for change new parameter (add sabbath algorithm) There are 3 sabbath functions.
					//sabbat 0 = 1 , 0 = None sabbath = 0
					.EcoExtra = 1, // Eco function, customer don't use door 360min >> decrase time to 0 then temp FRZ setting change to Dial min(cold) 
					//it is like Energy saving function. Press Eco function on Cintrol panel.
					.DoorsClosed = 0, // For energy consumption test it is canceled. Don't use
					.Silent = 1, // Silent mode, Press on the Display.
					.PumpOut = { //Recovery function *frist start and valve change position 
						.isActive = 1, // 0 is Disable, 1 is Enable
						.type = _PumpOut_Close, // _PumpOut_AllOpen da eklendi. suresi 30sn core da//Valve position when pumpout
					},
				},
			},
			.voltageInput = _PowerVolInput,  //power voltage input
			.Model_Select_EEPROM_Record = 3, //FUJIGEN2 = 0, HOSHIGEN2_H10 = 1, HOSHIGEN2_H30 = 2, BONA = 3, ALPS78 = 4
		},

		.Parameters = {
			.I2CDevices = { //Enable&Disable Devised//  0 is Disable, 1 is Enable
				.device = {
					.rgbLEDUsage = 0,//LED Handle Lighting 
					.HumiditySensor = 0,  // 0 is Disable, 1 is Enable
				},
			},
			//.fairMode =1,
			.autoDispOff = 1, // Auto display off 1 minute, Lighting of Display will truned off after 1 minute.
			.dispOff = 0, // Manual Display off Dont use
			.realTemp = 0, // Showing Temp. on Display Dont use
			.fahrenheit = 1, // Can change Celcius to Fahrenheit
			.onBoardBuzzer = 1, //Main Bord Buzzer
			.ecoExtraTime = (6 * 60),			// 6 hour // After Press Eco function during 360 minutes *No door Open *No press display it run into EcoExtra mode
			.sabbathTime = (72 * 60),			// 72hour // Auto cencelation sabbath mode.
			.doorsClosedTime = 65000,		// 65000minute  26 hour *No door Open *No press display it run into energy consumption test mode
			.doorsClosedBlockTime = (120 * 60), // 120 hour //Mode Canceled after 120 hrs.
			.maxDoorsClosedStateStayTime = 28800,
			.doorsOpenedCount = 1, // cancel Doorclosemode
                        //APW : FUJI/HOSHI door alarm setting but BONA use doorsAlertTime 1st and 6th.
                        //               { 1st, 2nd, 3rd, 4th, 5th, 6th} alarm (note : after 3rd will alarm pattern same)
			.doorsAlertTime = {60, 120, 180, 240, 300, 90}, //Door open alert when open > 60 second. Can change to 120,180,240,300,60 by Home WIZ
			.doorsAlertingCloseLightTime = 10, // After 10 minute LED off ,buzzer continue
			.displayData = 1,
			.displayValue = 0,
			.displayUnit = 2,
			.silentTime = (12 * 60), // 12 hour Press this function on Display?
			.shoppingModeTime = (1 * 60), // to precooling when go back from Shopling , Can use This function by Home WIZ // No Use
			.doorsAlertLevelIncreaseTime = 120,
			.commercialDefrostOnTimerSet = 3*10,         // APW(Base timer 100ms ; *10) : Add CT heater software (FRZ DEF heater only)
			.commercialDefrostOffTimerSet = 1*10,        // APW(Base timer 100ms ; *10) : Add CT heater software (FRZ DEF heater only)
			.commercialHeaterOnTimerSet = 0.8*10,        // APW(Base timer 100ms ; *10) : Add CT heater software (Others heater only)
			.commercialHeaterOffTimerSet = 0*10,         // APW(Base timer 100ms ; *10) : Add CT heater software (Others heater only)
			.commercialAutoClearEEPROMMinTimer = 90,     // APW : Add timer auto clear data in CT
		},
					
		.Connected = {
			// Device Specific Number Definitons
                  .definitions = {
                      // Connected Appliances
                      .brand = 2, // Arcelik:1 , Grundig:2, Beko:3
  
                      .stockCode = _ProductStockCode,
                      .machineID = 2, // CM : 1; RF: 2; BM :3; Oven:4 ;TD:5
                      .wgVersion = _StockVersion,
                      .wgRevision = _StockRevision,
                      .productVersion = 3, // 0: ARWEN I, 1: ARWEN II, 2: ARWEN III 3: End2End
                      .dateDay = 15,
                      .dateMonth = 12,
                      .dateYear = 16,
                  },

                  // Connected Appiance Main Function availability / activation
                  .funcConnectedApp = {
                     .connectedAppAct = 0, // 1: Connected appliance,  0: non-connected
                     .swapConnButtons = 0, // use connected appliance button setup
                                                                 // please note that connectedApplianceAct is the main activation switch. To activate any other function, this connectedApplianceAct has to be set "1" too
                  },

                  // Mercek / Log Function availability / activation
                  .funcLog = {
                     .logAct = 1, // 1: Mercek logging functionaliy active 0: Mercek Array is not valid and can not be read.
                                                   // please note that connectedApplianceAct is the main activation switch. To activate any other function, this connectedApplianceAct has to be set "1" too
                  },

                  // Show High Temp Error in Application Function availability / activation
                  .funcShowHighTempError = {
                     .showHighTempErrorAct = 1, // 1: Show high temperature error temperature in WifiArray, 0: always write 0x00 to the dedicated wifi arrray address
                                                                             // please note that connectedApplianceAct is the main activation switch. To activate any other function, this connectedApplianceAct has to be set "1" too
                  },

                  // Defrost Delay Function availability / activation (ok zamanl?Tarife uyumlu; defrost'un ucuz tarifeye telenmesi)
                  .funcdelayDefrost = {
                    .delayFRZDefrostAct = 1,									   // 1: Delay Defrost to cheap time, 0: do deforst based on run times.
                    .timeOptionTableF = {0, 30, -30, 60, -60, 90, -90, 120, -120}, // delay/early options
                    .minDefrostPeriod = 60,										   // min required time to defrost take place (60 = 60)
                                                                                                                                                     // please note that connectedApplianceAct is the main activation switch. To activate any other function, this connectedApplianceAct has to be set "1" too
                          },
		},
		.IceMachine = { // For IM, not use 
			.type = _IceMachineType_DEFAULT,  //_IceMachineType_NONE,  //
			.components = {
				.caseTempSensor = {
					.type = _TempSensorType_NTC_IM_case_ADC,  //_TempSensorType_NONE,  //
					.index = 4,  //0xFF,  //
					//.order = 4,
				},
				.fan = {
					.type = _FanType_DC,  //_FanType_NONE,  // CDC = ON/OFF type,PWM = VDC //No Fan = _FanType_None//_FanType_CDC,
					.index = 4,  //0xFF,  //
					.configs = {
						.powerPinUsed = false, //POWER
						.pwmUsed = false,		//PWM PIN
						.fgPinUsed = false,		//Feedback
						.rpm = 1550, //Maximum rpm
						.ppr = 4, // paluse per rotation
					},
					.test = {
						.startTime = 0, // Prodtest //sec.
						.endTime = 0,
					},
                                    
                                 },
				.caseRotator = {
					.type = _RotatorType_BDCMotor,//_RotatorType_NONE,
					.index = 0,//0xFF,
					.test.cw = {.startTime = 0, .endTime = 0},
					.test.ccw = {.startTime = 0, .endTime = 0},
				},
				.caseTactSwitch = {
					.type = _SwitchType_DC,  //_SwitchType_NONE,  //
					.index = 2,  //0xFF,  //
				},
				.boxTactSwitch = {
					.type = _SwitchType_DC,  //_SwitchType_NONE,  //
					.index = 1,  //0xFF,  //
				},
			},
			.parameters = {                                                         // 100
				.waterPumpTime = 90, // ((waterPumpTime / 10) second ) = (waterPumpTime * 5) * 20ms = 10 second (Total water intake duration)
				.checkIceTempAtLongWait = -150,         //check temperature after ".makeIceLongWaitTime"
				.checkIceTempAtShortWait = -150,        //check temperature after ".makeIceShortWaitTime"
				.makeIceLongWaitTime = 150,             //makeIceLongWaitTime>checkIceTempAtLongWait>makeIceShortWaitTime>checkIceTempAtShortWait        
				.makeIceShortWaitTime = 1,              //makeIceLongWaitTime>checkIceTempAtLongWait>makeIceShortWaitTime>checkIceTempAtShortWait
				.checkWaterEnabled = false,             //Not use for BONA check tank empty by sensor
				.checkWaterTime = 300,                  //Not use for BONA check tank empty by sensor
				.checkWaterTempTime = 60,               //Not use for BONA check tank empty by sensor
				.checkWaterTemp = -40,                  //Not use for BONA check tank empty by sensor
				.quickCutInTemp = -50,                  //Not use for BONA auto quickIce        //normally arcelik automatic sense quick ice, when this less then this value, quickIce start
				.quickCutOutTemp = -170,                //Not use for BONA auto quickIce        // when it reaches this value, quick is finished
				.quickIceOnDisplay = true,              //BONA set true Quick Ice on display
				.HBY_SetDecrease = 20,                  //Not use for BONA auto quickIce        // when you have quick ice, your ci co will decrease 2 Celcicus
				.quickIce60WaterPumpTime  = 62,                //<<QuickIce60 water supply time (Secound)
				.quickIce60checkIceTempAtLongWait = -120,      //<<QuickIce60 check temperature after ".makeIceLongWaitTime" (C/10)
				.quickIce60checkIceTempAtShortWait = -120,     //<<QuickIce60 check temperature after ".makeIceShortWaitTime" (C/10)
				.quickIce60makeIceLongWaitTime = 58,           //<<QuickIce60 Long wait time (Minute)
				.quickIce60makeIceShortWaitTime = 1,           //<<QuickIce60 Short wait time (Minute)
				.quickIce60MaximumTime  = 240,                 //<<QuickIce60 Maximum operation time (Minute)
				.quickIce60MaximumTimes  = 5,                  //<<QuickIce60 Maximum count (Times)
			},
		},

		.IceSource = {//Ice dispensor  not use
			.type = _IceSourceType_IceCubeAndCrash, //_IceSourceType_NONE,
			.components = {
				.selector = {
					.type = _SelectorType_ACMotor, //_SelectorType_NONE,
					.index = 7, //0xFF,
					.test.selA = {
						.startTime = 0,
						.endTime = 0,
					},
					.test.selB = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.shutter = {
					.type = _ShutterType_ACMotor, //_ShutterType_NONE,
					.index = 8, //0xFF,
					.test.open = {
						.startTime = 0, // No need, not used in IceSource
						.endTime = 0,	// No need, not used in IceSource
					},
					.test.close = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.pusher = {
					.type = _PusherType_ACOut, //_PusherType_NONE,
					.index = 9, //0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
			},
		},

		.WaterSource = { // IM Pump
			.type = {
				.supply = _WaterSupplyType_INTERNAL_IceMachine_Dispenser,  //_WaterSupplyType_NONE,  //
				.filter = _WaterFilterType_NONE,
			},
			.components = {
				.waterIceTankEmptySensor = {//Ice making
				.type = _TempSensorType_NTCwithADC,  //_TempSensorType_NONE,  //
				.index = 13, //For BONA H30-50 8, //For New MCU 10, //For Old MCU
				//.order = 4,
				},
				.waterDisTankEmptySensor = {//Water dispensor
				.type = _TempSensorType_NTCwithADC,  //_TempSensorType_NONE,
				.index = 12, //For BONA H30-50	//0xFF,
				//.order = 4,
				},
				.mainPump = {
					.type = _PumpType_NONE,	//_PumpType_NONE,	//
					.index = 0xFF,	//0xFF,	//
					.test.pump = {
						.startTime = 0,
						.endTime = 0,
					},
					.test.vacuum = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.dispenserPump = {
					.type = _PumpType_BDCMotor,	//_PumpType_NONE,
					.index = 2,	//0xFF,
					.test.pump = {
						.startTime = 0,
						.endTime = 0,
					},
					.test.vacuum = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.iceMachinePump = {
					.type = _PumpType_BDCMotor, //_PumpType_NONE, //
					.index = 1, //0xFF, //
					.test.pump = {
						.startTime = 0,
						.endTime = 0,
					},
					.test.vacuum = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.mainValve = {
					.type = _ValveType_NONE,
					.index = 0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.dispenserValve = {
					.type = _ValveType_NONE,
					.index = 0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.iceMachineValve = {
					.type = _ValveType_NONE,
					.index = 0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.waterVentilHeater = {//IM heater
					.type = _HeaterType_AC,
					.index = 2, //acSPHEAT, //Hoshi_H30 3, //ALPS78
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
					.commercialDefrostTestPriority = 3,          //APW : Add CT heater software
					.resistanceForPower = 9338,
				},
				.waterCaseSwitch = { 
					.type = _SwitchType_NONE,	//_SwitchType_DC, // FF door switch index should be write
					.index =0xFF,	// 1,
				},
			},
			.parameters = {
				.waterVentilHeater = { //  Ambient {3&4=0%,other=50%}
                                  //{On/off : 0/10 is 0%, 1/9 is 10%, 2/8 is 20%,...,10/0 is 100%}
                                  .OnTimeA =  {6 ,6 ,6 ,6 ,9},          // ICEON  | BOXFULL'de OnTimeA
                                  .OffTimeA = {4 ,4 ,4 ,4 ,11},        //        | BOXFULL'de OffTimeA/2 cycle'ini yapar
                                  .OnTimeB = 0,    // ICEOFF |
                                  .OffTimeB = 50,
                                  .IceOnFirstOnTime = 3,                        //Heater ON 100% when IceOn/Empty clear
                                  .ProhibitPumpWhenIceOnFirstOnTime = 0,        //Update delay supply when IceOff to IceOn
                                  .ProhibitPumpAfterDoorClearEmptyTime = 0,     //Update delay supply when Empty/PowerOn
				},
                                .waterIceTankEmptySensor = {// Add because we separate Empty judgment each model.(Ice making)
                                  .JudgmentZoneA = -317,
                                  .JudgmentZoneB = -147,   // A->B : Normal zone (Have water in Tank)
                                  .JudgmentZoneC = -147,
                                  .JudgmentZoneD = 247,    // C->D : Empty zone (No have water in Tank)
                                },
                                .waterDisTankEmptySensor = {// Add because we separate Empty judgment each model.(Water dispensor)
                                  .JudgmentZoneA = -317,
                                  .JudgmentZoneB = -147,   // A->B : Normal zone (Have water in Tank)
                                  .JudgmentZoneC = -147,
                                  .JudgmentZoneD = 247,    // C->D : Empty zone (No have water in Tank)
                                },
			},
		},

		.Dispenser = {// For Dispensor 
			.waterType = _DispenserWaterType_WaterPump, //_DispenserWaterType_NONE,
			.iceType = _DispenserIceType_IceCubeAndCrash, //_DispenserIceType_NONE,
			.components = {
				.tactSwitch = {
					.type = _SwitchType_DC, //_SwitchType_NONE, // This is for test
					.index = 11, //0xFF,             			//index should be different in H30
				},
				.light = {
					.type = _LightType_PWM, //_LightType_NONE,
					.index = 5, //0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.doorSwitch = {
					.type = _SwitchType_DC,	//_SwitchType_NONE,
					.index = 1,	//0xFF,
				},
			},
			.parameters = {
				.OFFState = false,
				.lightTime = 50,
				.autofill = {
					.autofillTime_8OZ = 17,         //volume 200cc
					.autofillTime_16OZ = 25,        //volume 300cc
					.autofillTime_32OZ = 33,        //volume 400cc
				},
			},
		},

		/*.IceCreamMachine = {//For Ice  
			.type = _IceCreamMachineType_NONE,
			.components = {
				.mixer = {
					.type = _MixerType_NONE,
					.index = 2,
					.test = {
						.startTime = 142,
						.endTime = 160,
					},
				},
				.tempSensor = {
					.type = _TempSensorType_NTCwithADC, .index = 5,
					//.order = 0xFF,
				},
				.doorSwitch = {
					.type = _SwitchType_NONE,
					.index = 1,
				},
			},
			.sets = {
				.sizes = {
					.iceCream = 3,
				},
				.indexes = {
					.iceCream = {
						0,
						1,
						2,
					}, // iceCream[9]
				},
			},
			.parameters = {
				.relatedCabin = 1,
				.checkTemp = -50 // -5.0 C
			},
		},
*/
		.Cooler = {
			.type = _CoolerType_PARALLEL, //Full parallel  Cooling type. //
			.components = {
				.condenserFan = {
					.type = _FanType_DC,	//_FanType_NONE,//_FanType_CDC,// CDC = ON/OFF type,PWM = VDC //No Fan = _FanType_None//_FanType_CDC,
					.index = 2,	//0xFF,//
					.configs = {
						.powerPinUsed = false, //POWER
						.pwmUsed = false,		//PWM PIN
						.fgPinUsed = false,		//Feedback
						.rpm = 1550, //Maximum rpm
						.ppr = 4, // paluse per rotation
					},
					.test = {
						.startTime = 97, // Prodtest //sec.
						.endTime = 117,
					},
                                        .maxPower = 132, //divided 100 which means 1.32 watt

				},
				.compressor = {
					.type = _CompressorType_UART, //_CompressorType_VCC_EKI, //Comperssor type
					.index = 0,
					.test = {
						.startTime = 0, // Prodtest //sec.
						.endTime = 0,
					},
				},
				.ambientTempSensor = {
					.type = _TempSensorType_NTC_Ambient_ADC, // type of sensor
					.index = 5,
					.hideError = 0, //1 = Display Error off after 1 minute // 0 = continue Error Display
				},
				.humiditySensor = {
					.type = _HumSensorType_ADC, // type of sensor
					.index = 6,	//0xFF,
                    //.hideError = 0, //1 = Display Error off after 1 minute // 0 = continue Error Display
				},
				.solenoidValve[0] = {
					.type = _SolenoidValveType_StepMotor, //Prodtest after productiontest
					.index = 0,
					.positionsSize = 4,						// max 5
					.positions = {34, 195, 100, 154, 0xFF}, // allClose, COpen, BOpen, AllOpen
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
					.stepperConfigs = {
						.drivingMethod = eStepperMotorDrivingMethodHalfStep,
						.calibrateAtInit = true,
						.calibrationPosition = 220,
						.calibrationTimeH = 48,
						.movementsCountForCalibration = 200,
					},
				},
				.solenoidValve[1] = {
					.type = _SolenoidValveType_NONE, // None Valve
					.index = 0xFF,
					.positionsSize = 4,						// max 5
					.positions = {34, 195, 100, 154, 0xFF}, // allClose, COpen, BOpen, AllOpen
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
 					.stepperConfigs = {
						.drivingMethod = eStepperMotorDrivingMethodHalfStep,
						.calibrateAtInit = true,
						.calibrationPosition = 300,
						.calibrationTimeH = 1,
						.movementsCountForCalibration = 200,
 					},
				},
                                /*** APW : Add CT heater software ***/
				.testSwitch[0] = {
					.type = _SwitchType_DC,
					.index = 5,
				},
				.testSwitch[1] = {
					.type = _SwitchType_DC,
					.index = 6,
				},
				.CommercialARM = {
					.type = _ARMType_DC,
					.index = 16, // For H30-50
				},
				/*** APW : Add CT heater software ***/
			},
			.parameters = {
					//PCH UPDATE                              L1  L2  L3  L4  LD  LH  L7  L8  L9   // Have ambien sensor use L1-LH , L8-L9 For none ambien sensor
					 //.minStayTime AtPresgasLevel for protection compressor,Do not change speedLevel during .minStayTime
					.minStayTimeAtPresGasLevel[_Ambient_0] = {10, 10, 10, 10, 10, 10, 10, 10, 10},
					.minStayTimeAtPresGasLevel[_Ambient_1] = {10, 10, 10, 10, 10, 10, 10, 10, 10},
					.minStayTimeAtPresGasLevel[_Ambient_2] = {10, 10, 10, 10, 10, 10, 10, 10, 10},
					.minStayTimeAtPresGasLevel[_Ambient_3] = {10, 10, 10, 10, 10, 10, 10, 10, 10},
					.minStayTimeAtPresGasLevel[_Ambient_4] = {10, 10, 10, 10, 10, 10, 10, 10, 10},
					//maxStayTimeAtPresGasLevel maximum time each speedLevel.
					.maxStayTimeAtPresGasLevel[_Ambient_0] = {30, 30, 30, 30, 70, 80, 90, 90, 90},
					.maxStayTimeAtPresGasLevel[_Ambient_1] = {72, 72, 72, 72, 70, 80, 90, 90, 90},
					.maxStayTimeAtPresGasLevel[_Ambient_2] = {72, 72, 72, 72, 70, 80, 90, 90, 90},
					.maxStayTimeAtPresGasLevel[_Ambient_3] = {145, 72, 72, 72, 70, 80, 90, 90, 90},
					.maxStayTimeAtPresGasLevel[_Ambient_4] = {30, 30, 30, 30, 70, 80, 90, 90, 90},
					.compressor = {
						.minOnTime = 5, //minimum comp on time
						.minOffTime = 10,//minimum comp off time
						.speedLevel = { 36, 38, 38, 56, 68, 110, 122, 150, 150 }, // For none ambien sensor 
						.speedLevelFF =  { 60, 70, 75, 80, 85, 90, 110, 120, 128 }, // For none ambien sensor 
						.speedLevelFRZ = { 50, 60, 70, 75, 80, 90, 110, 120, 128 }, // For none ambien sensor 
						/// NER
						.FF = {                   /* L1, L2, L3, L4, LD, LH */ //LD is rmp after defrost , until cut out.
						                                                       //LH is frist plug in [power on] 
							.speedLevel[_Ambient_0] = {  40,  60,   80,  100,  60, 143},
							.speedLevel[_Ambient_1] = {  50,  70,   90,  110,  65, 143}, //40*30=1200rpm
							.speedLevel[_Ambient_2] = {  60,  90,  100,  143,  70, 143}, 
							.speedLevel[_Ambient_3] = {  65,  90,  120,  143,  80, 143},
							.speedLevel[_Ambient_4] = { 100,  130, 150,  143, 110, 143},
							.maxDialSpeedLevelUpLH = false,
						},//DPJ : Cannot use 2800(93), 3500(117), 4100(137), 4200(140)
						.FRZ = {                  /* L1, L2, L3, L4, LD, LH */
							.speedLevel[_Ambient_0] = {  60, 70,   90, 120,  80, 143},
							.speedLevel[_Ambient_1] = {  80, 90,  110, 130,  90, 143}, 
							.speedLevel[_Ambient_2] = {  90, 110, 130, 143, 110, 143}, 
							.speedLevel[_Ambient_3] = { 105, 120, 130, 143, 130, 143}, 
							.speedLevel[_Ambient_4] = { 110, 130, 135, 143, 130, 143},
							.maxDialSpeedLevelUpLH = true,
						},
						.speedSilent = 36, // rpm of silent , Must press on control panel
						.speedStartUp = 60, // rpm  Oil Cooling 
						.speedPumpOut = 60, // rpm of recovery 
						.timeStartUp = 1, // Time  Oil Cooling
						.timeForMaxLevel_DO = {120,120,150,480,120}, // running > Time,200 change LH level.
						.timeForMaxLevel_DC = {120,120,150,480,120}, // running > Time,200 change LH level.
						.workTimeAtMaxLevel = 255, // not using should be set to 255 // 0 = always operate high level.
						.speedPullDownStage_1 = 100,
						.speedPullDownStage_2 = 110,
						.minOnTimePullDownStage_1 = 60,
						.minOnTimePullDownStage_2 = 240,
						/*** APW : Add CT heater software ***/
						.speedFirstCommercial = 70, //(2100)    // (RPM in CT mode : Separate country) after minOntimerFirstCommercial goto speedSecondCommercialValveTest.
						.speedSecondCommercialValveTest = 144,  // (RPM in CT mode : Valve check)
						.minOntimerFirstCommercial = 5,         // (Set 2 or more)First Commercial mode time.
						/*** APW : Add CT heater software ***/
						},

				.condenserFan = {
					.speedLevel = {100, 100, 100, 100, 100, 100, 100, 100, 100}, // None ambien sensor
					.speedLevelFF = { 100, 100, 100, 100, 100, 100, 100, 100 }, // None ambien sensor
					.speedLevelFRZ = {  100, 100, 100, 100, 100, 100, 100, 100 }, // None ambien sensor
					/// NER
					.FF = {                                          /* L1,   L2,   L3,   L4,   LD,   LH */ //ON-OFF FAN = 100
						.speedLevel[_Ambient_0] = { 0,  0,  0,  0,  0,  0}, //Can set 0 , when stop fan
						.speedLevel[_Ambient_1] = { 0,  0,  0,  0,  0,  0},
						.speedLevel[_Ambient_2] = { 100,  100,  100,  100,  100,  100},
						.speedLevel[_Ambient_3] = { 100,  100,  100,  100,  100,  100},
						.speedLevel[_Ambient_4] = { 100,  100,  100,  100,  100,  100},
					},
					.FRZ = {                                          /* L1,   L2,   L3,   L4,   LD,   LH */ //ON-OFF FAN = 100
						.speedLevel[_Ambient_0] = { 0,  0,  0,  0,  0,  0},
						.speedLevel[_Ambient_1] = { 0,  0,  0,  0,  0,  0},
						.speedLevel[_Ambient_2] = { 100,  100,  100,  100,  100,  100},
						.speedLevel[_Ambient_3] = { 100,  100,  100,  100,  100,  100},
						.speedLevel[_Ambient_4] = { 100,  100,  100,  100,  100,  100},
					},
						.speedSilent = 100, //ON-OFF FAN = 100
						.speedPumpOut = 100, //ON-OFF FAN = 100
						.speedCheckCorrection = 0,
						.speedCheckTime = 15,
						.speedCheckTemp = -100, //_DegC_n10_0,
						.speedPullDownStage_1 = 100,
						.speedPullDownStage_2 = 100,
				},
				/// NER
				.ambient = {
					.temperatures = { 125, 205, 285, 355 }, // value / 10 = temperature
					.hysteresis = { 10, 10, 10, 10 },  // value / 10 = temperature
					.errorType = _Ambient_3,
					.humidity = { 
					.intervals = {10, 20, 30, 40, 50, 55, 64, 73, 82}, // 0,10,20,30,40,50,60,70 needed.
					.hysteresis = {0,  0,  0, 0, 0, 0, 0, 0, 0},
					.errorType = _Humidity_4, // Change to 7 or 8 ?
					},
				},

				.sysStates = {      
						{//0 Default position
							.queue = false,
							.valvesDefPosition = 3,
						},
						{//1
							.logic = _L_OR,//OR is gasSpecs[0] is correct "or" gasSpec[1] is correct
							.queue = false, //false:serch next status from first / true:serch next status with queue true from this status
							.gasSpecs[ 0 ] = { .type = _GT_Failed, .priority = 1, },
							.gasSpecs[ 1 ] = { .type = _GT_Failed, .priority = 2, },
							.valvesDefPosition = _ALL_CLOSE, //kompresor durdugunda ne yapalim sorulacak.
						},
						{//2
							.logic = _L_AND, //GT_PerformanceTest EVP sensor > 10*C and Air sensor (F/R) < 39*C //GT_PerformanceTest is Devise Checking(CT)
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_PerformanceTest, .priority = 2, },// Hight .priority(2) is frist
							.gasSpecs[ 1 ] = { .type = _GT_PerformanceTest, .priority = 1, },
							.valvesDefPosition = _ALL_OPEN,
							.valvesDefPositionTime = 10,
						},
						{//3
							.logic = _L_AND,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_PerformanceTest2, .priority = 2, }, // Hight .priority(2) is frist
							.gasSpecs[ 1 ] = { .type = _GT_PerformanceTest2, .priority = 1, },
							.valvesDefPosition = _FF_OPEN,
							.valvesAltPosition = _FRZ_OPEN,
							.valvesDefPositionTime = 40,
							.valvesAltPositionTime = 70,
						},
						{//4
							.logic = _L_OR,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_PerformanceTest2, .priority = 2, },
							.valvesDefPosition = _FF_OPEN,
							.valvesDefPositionTime = 25,
						},
						{//5
							.logic = _L_OR,
							.queue = false,
							.gasSpecs[ 1 ] = { .type = _GT_PerformanceTest2, .priority = 2, },
							.valvesDefPosition = _FRZ_OPEN,
							.valvesDefPositionTime = 60,
						},
						{//6
							.logic = _L_AND,  //G_GT_HotStart EVP sensor > 10*C and Air sensor (F/R) > 39*C
							.queue = false, 
							.gasSpecs[ 0 ] = { .type = _GT_HotStart, .priority = 2, }, //Pulldonw condition
							.gasSpecs[ 1 ] = { .type = _GT_HotStart, .priority = 1, },
							.valvesDefPosition = _ALL_OPEN,
							.valvesDefPositionTime = 10,
						},
						{//7
							.logic = _L_AND,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_HotStart2, .priority = 2, },
							.gasSpecs[ 1 ] = { .type = _GT_HotStart2, .priority = 1, },
							.valvesDefPosition = _FF_OPEN,
							.valvesAltPosition = _FRZ_OPEN,
							.valvesDefPositionTime = 400, // if reach Cutout in 100 ,Cange to .gasSpecs[ 1 ] Time is [400-100] + 100
							.valvesAltPositionTime = 100, // Time from .valvesDefPositionTime [400-100]+100
						},
						{//8
							.logic = _L_OR,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_HotStart2, .priority = 2, },
							.valvesDefPosition = _FF_OPEN,
							.valvesDefPositionTime = 400,
						},
						{//9
							.logic = _L_OR,
							.queue = false,
							.gasSpecs[ 1 ] = { .type = _GT_HotStart2, .priority = 2, },
							.valvesDefPosition = _FRZ_OPEN,
							.valvesDefPositionTime = 100,
						},  
						{//10		
							.logic = _L_AND,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_No, .priority = 2, },
							.gasSpecs[ 1 ] = { .type = _GT_No, .priority = 1, },
							.valvesDefPosition = _FF_OPEN,
							.valvesDefPositionTime = 40,
						},
						{//11
							.logic = _L_AND,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_Hot, .priority = 2, }, //GT_Hot is Defrosting
							.gasSpecs[ 1 ] = { .type = _GT_No,  .priority = 1, }, //GT_No is No need cooling / No Defrost
							.valvesDefPosition = _FF_OPEN,
							.valvesDefPositionTime = 40,
						},
						{//12
							.logic = _L_AND,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_No,  .priority = 1, },
							.gasSpecs[ 1 ] = { .type = _GT_Hot, .priority = 2, },
							.valvesDefPosition = _FF_OPEN,
							.valvesDefPositionTime = 40,
						},
						{//13
							.logic = _L_AND,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_Hot, .priority = 2, },
							.gasSpecs[ 1 ] = { .type = _GT_Hot, .priority = 1, },
							.valvesDefPosition = _FF_OPEN,
							.valvesDefPositionTime = 40,
						},
						{//14
							.logic = _L_AND,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_Cold_FF, .priority = 1, },//GT_Cold is Need Cooling
							.gasSpecs[ 1 ] = { .type = _GT_Hot,  .priority = 2, },//GT_Hot is Defrosting
							.valvesDefPosition = _FF_OPEN,
							.valvesDefPositionTime = 50,
						},
						{//15
							.logic = _L_AND,
							.queue = true,
							.gasSpecs[ 0 ] = { .type = _GT_Cold_FF, .priority = 1, },//GT_Cold is Need Cooling
							.gasSpecs[ 1 ] = { .type = _GT_Cold_FRZ, .priority = 2, },//GT_Cold is Need Cooling
							.valvesDefPosition = _FRZ_OPEN, //Change to _FRZ_OPEN //SZU ??? FRZ is high priority but start from FF_OPEN
							.valvesAltPosition = _FF_OPEN,
							.valvesDefPositionTime = 70,
							.valvesAltPositionTime = 40,
						},
						{//16
							.logic = _L_AND,
							.queue = true,
							.gasSpecs[ 0 ] = { .type = _GT_Hot,  .priority = 1, },
							.gasSpecs[ 1 ] = { .type = _GT_Cold_FRZ, .priority = 2, },
							.valvesDefPosition = _FRZ_OPEN,
							.valvesDefPositionTime = 40,
						},
						{//17
							.logic = _L_AND,
							.queue = true,
							.gasSpecs[ 0 ] = { .type = _GT_Cold_FF, .priority = 2, },
							.gasSpecs[ 1 ] = { .type = _GT_No, .priority = 1, },
							.valvesDefPosition = _FF_OPEN,
							.valvesDefPositionTime = 40,
						},
						{//18
							.logic = _L_AND,
							.queue = true,
							.gasSpecs[ 0 ] = { .type = _GT_Cold_FF, .priority = 2, },
							.gasSpecs[ 1 ] = { .type = _GT_Cold_FRZ, .priority = 1, },
							.valvesDefPosition = _FF_OPEN, //SZU ??? FF is high priority but start from FRZ_OPEN
							.valvesAltPosition = _FRZ_OPEN,
							.valvesDefPositionTime = 40,
							.valvesAltPositionTime = 70,
						},
						{//19
							.logic = _L_OR,
							.queue = false,
							.gasSpecs[ 0 ] = { .type = _GT_Cold_FF, .priority = 2, },
							.valvesDefPosition = _FF_OPEN,
							.valvesDefPositionTime = 75,
						}, 
						{//20
							.logic = _L_OR,
							.queue = false,
							.gasSpecs[ 1 ] = { .type = _GT_Cold_FRZ, .priority = 1, },
							.valvesDefPosition = _FRZ_OPEN,
							.valvesDefPositionTime = 40,
						},
						},
							.valveGasStates = { //Cabin 1 [FRZ]    //Cabin 0 [FF] // For  Full pararell Type 
							{ .valve0Position = 0, .valve1Position = 0, .gas = 0x00, .enough = 0x00, },
							{ .valve0Position = 1, .valve1Position = 0, .gas = 0x01, .enough = 0x01, },
							{ .valve0Position = 2, .valve1Position = 0, .gas = 0x02, .enough = 0x02, },
							{ .valve0Position = 3, .valve1Position = 0, .gas = 0x03, .enough = 0x03, },
						},
							/*** APW : Add CT heater software ***/
							.minValveSwitchtimerCommercial = 10,  // Second Commercial mode switch valve    
							.minOntimerSecondCommercial = 30,     // Second Commercial mode after finish goto normal mode.
							/*** APW : Add CT heater software ***/
							},
						},

		.Cabin[0] = { //FF
			.type = _CabinType_FF,
			.components = {
				.knob = {// Dial Type *Same Fuji model
					.type = _KnobType_NONE,
					.index = 0xFF,
				},
				.vacuumBox = {
					.type = _VacuumBoxType_NONE, //_VacuumBoxType_BDCMotor,
					.index = 0xFF, //3,
					.test.pump = {
						.startTime = 0,
						.endTime = 0,
					},
					.test.vacuum = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.vacuumBoxOverCurrentSensor = {
					.type = _TempSensorType_NONE, //_TempSensorType_NTCwithADC,   
					.index = 0xFF, //11,
                                  //.order = 4,
				},
				.vacuumBoxHeater = {
					.type = _HeaterType_NONE, //_HeaterType_AC, // HeaterType_NONE : None Heater
					.index = 0xFF, //5,
					.test = {
						.startTime = 0, //First Plug_Intial Operate
						.endTime = 0, //Off
					},
					.commercialDefrostTestPriority = 0,  //6, //For BONA H50
					.resistanceForPower = 0,  //6914,  //For BONA H50
				},
				.fan = {
					.type = _FanType_VDC,           //Prodtest
					.index = 0,
					.configs = {
						.powerPinUsed = false,
						.pwmUsed = true,
						.fgPinUsed = true, //For Mass	false, //For DEBUG   
						.rpm = 1380, // Max-Speed Turbo Fan is 1380 
						.ppr = 2, //paluse per rotation
					},
					.test = {
						.startTime = 75, // For TC
						.endTime = 95, // For TC
					},
					.maxPower = 238, //2.38 watt
				},
				.flap = {//DamperType.
					.type = _FlapType_NONE, //_FlapType_StepMotor,
					.index = 0xFF, //0,
					.stepperConfigs = {
						.drivingMethod = eStepperMotorDrivingMethodFullStep,
						.calibrateAtInit = true,
						.calibrationPosition = 1850,//2500,
						.calibrationTimeH = 24,
						.movementsCountForCalibration = 100,
					},
				},
				.heaterA = { // Wrok by Cutin_CutOut [V-Heater]
					.type = _HeaterType_AC, // acIMHeaT - BOOT HEATER - Heat Up heater
					.index = 4,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
					.commercialDefrostTestPriority = 4,          //APW : Add CT heater software
					.resistanceForPower = 12100,
				},
				.heaterB = { //Defrost Heater
					.type = _HeaterType_NONE, //None Heater _HeaterType_AC, //prodtest
					.index = 0xFF, //2, //acRdef
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
					.commercialDefrostTestPriority = 0,          //APW : Add CT heater software
					.resistanceForPower = 0,      //ohm
				},
				.heaterC = { // Dew Bootheater Handle Heater
					.type = _HeaterType_NONE, //_HeaterType_AC,
					.index = 0xFF,//6, //acRPHEAT 
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
					.commercialDefrostTestPriority = 0,  //4,  //For BONA H50
					.resistanceForPower = 0,  //8067,  //For BONA H50
				},
				.ionizer = { // None
					.type = _IonizerType_NONE,
					.index = 0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.handleLight = { //FF-Libht
					.type = _LightType_NONE, //_LightType_PWM,
					.index_r = 0xFF, //2,
					.index_g = 0xFF, //3,
					.index_b = 0xFF, //4,
					.test = {
						.startTime = 36,
						.endTime = 51,
					},
					.maxPower = 300, //3 watt
				},
				.innerLight = { //FF-Libht
					.type = _LightType_PWM,
					.index = 0,
					.test = {						
						.startTime = 19,
						.endTime = 34,
					},
					.maxPower = 300, //3 watt
				},
				.blueLight = {// None
					.type = _LightType_NONE,
					.index = 0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.doorSwitch[0] = {
					.type = _SwitchType_DC,
					.index = 0,
				},
				.doorSwitch[1] = {
					.type = _SwitchType_NONE,
					.index = 0xFF,
				},
				.humanSensor = {
					.type = _SwitchType_NONE,//_SwitchType_DC_Reversed,
					.index = 0xFF,//10,
				},
				.airTempSensor = {//R-Sensor
					.type = _TempSensorType_NTC_R_air_ADC,
					.index = 0,
					.hideError = 0,
				},
				.evaTempSensor = {//R-Defrost Sensor
					.type = _TempSensorType_NTC_R_eva_ADC,
					.index = 1,
					.hideError = 0,
				},
				.violed = {// None
					.type = _VioLedType_NONE,
					.index = 0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
			},

			.sets = {
				.configs = {
					.FF = 8,
					.FRZ = 0,
					.functions = {
						.QuickFF = 1, .QuickFRZ = 0, .Vacation = 1, .Rice = 0, .QuickIce = 0, // Quick Ice Active
						.Convertible = 0, .CabinOff = 0,
					},
				},
				.indexes = {
					.FF  = { 5, 6, 7, 8, 9, 10, 3, 4, 0xFF, 0xFF, }, // FF[10] //* 6*/  { (unsigned char) 4 is defult
					      //{ 6, 5, 4, 3, 10, 9, 8, 7, 0xFF, 0xFF, },
					.FRZ = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, }, // FRZ[10] /*17*/  { (unsigned char)-18 is defult
					.functions = {
						.QuickFF = 0, .QuickFRZ = 0xFF, .Vacation = 1, .Rice = 0xFF, .QuickIce = 6, // Quick Ice Set from table
						.Convertible = 0xFF, .CabinOff= 0xFF,
					},
				},
			},
			.parameters = { // Performance Concern  // NoneDont Use
				.Performance = {
					.timeEva1 = _Performance_TimeEva1, .timeEva2 = _Performance_TimeEva2, .tempEva1 = _Performance_TempEva1, .tempEva2 = _Performance_TempEva2, .timeAir = _Performance_TimeAir, .deltaTempAir = _Performance_DeltaTempAir,
					//.thermalCutOut = 28,
				},
				.Quick = {
					.timeFFMode = 240,//_Quick_TimeFFMode,//minute
					.timeFRZMode =_Quick_TimeFRZMode,//minute
				},
				.Cool = {
					.SabbathMaxIdleTime = _Cool_FFAirEva_SabbathMaxIdleStateTime,
					.MaxIdleTime = 1000,//>1000 minute non operating , will operate by .MaxIdleTimeElapsedCoolTime = 5,
					.ErrorMaxIdleTime = _Cool_FFAirEva_ErrorMaxIdleTime,
					.ErrorMaxCoolTime = _Cool_FFAirEva_ErrorMaxCoolTime,
					.ErrorCutInTemp = _Cool_FFAirEva_ErrorCutInTemp,
					.ErrorCutOutTemp = _Cool_FFAirEva_ErrorCutOutTemp,
					.QuickFRZCutOutTemp = _Cool_FFAirEva_QuickFRZCutOutTemp,
					.MaxIdleTimeElapsedCoolTime = 5,
					.isEvaCutInActive = 1, //For cyclic defrost 1:Have 0:No Have /Maybe BONA need
					.EvaCutInTemp = 20, //For cyclic defrost temperature 2.5C /Maybe BONA need
					.pumpOutTime = 60,      //PCH Check /Before start cooling FF side /sec.
					.HotStartTime = 15, //_Cool_HotStartTime,minutes
					.HotStart2Time = 485, //_Cool_HotStartTime,minutes
					.PerformanceTestTime = 10, //minutes
					.PerformanceTest2Time = 240,    //minutes
					.Offsets =  {5,5,0,0,-10},//Ambien shif temp.
					.PreCoolOffsets =  {0,0,0,0,0}, //  Precooling shif temp.
					.AfterDefrostOffsets =  {0,0,0,0,0}, //  After Defrost shif temp.
					.OffsetsShiftCutIn =  {0,0,0,0,0},//Ambien shif temp.
					.OffsetsShiftCutOut =  {0,0,0,0,0},//Ambien shif temp.
#if defined(AI_COOL_V1)
					.CabinSetsAmbientOffSetAI = { 11,  -1,  -10,  -19,  -19}, // FF Cool OFFSET A_COOL
					.FanSetsAmbientOffSetAI   = { 11,  -1,  -10,  -19,  -19}, // FF fan OFFSET A_COOL
#endif
				},
				.Defrost = {
					.Type = _DefrostType_Variable,
					.MaxRealTime_DO = 600, //550, //_Defrost_FFAirEva_MaxRealTime_DO,/For normal defrost, No use in variable defrost
					.MaxRealTime_DC = 600, //550, //_Defrost_FFAirEva_MaxRealTime_DC,/For normal defrost, No use in variable defrost
					.MaxRealTime_QuickFRZ = _Defrost_FFAirEva_MaxRealTime_QuickFRZ,
					.MaxWorkTime_DO = 65000,//Minute // MaxWorkTime is comp. ON time , it will reset when Comp. OFF
					.MaxWorkTime_DC = 65000,//Minute // MaxWorkTime is comp. ON time , it will reset when Comp. OFF
					.MaxDefrostHighTempTime = 720,                //Minute    (Defrost by High temperature) : set 0xFFFF for disable mode
					.MaxDefrostHighTempShiftCutInTemp = 50,       //(Defrost by High temperature)  !!If set 0, will same as cutIN
					.MaxDefrostHighTempShiftCutOutTemp = 10,      //(Defrost by High temperature)  !!If set 0, will same as cutIN
					.DefrostHighTemp_Activation = 1440,           //Activation time after power on
					.MaxCycle_DO = 100, //_Defrost_FFAirEva_MaxCycle_DO, //Max cooling cycle counting (comp on&off)
					.MaxCycle_DC = 100, //_Defrost_FFAirEva_MaxCycle_DC, //Max cooling cycle counting (comp on&off)
					.DefrostCutOutTemp = 32, //_Defrost_FFAirEva_DefrostCutOutTemp, // 4 C //For normal defrost, No use in variable defrost
					.MaxNoCoolTime = _Defrost_FFAirEva_MaxNoCoolTime, //130, defrost time + Idle time
					//.MaxDelayTimeForIceCream = (8 * 60), //we donot use
					.ErrorMaxCycle = _Defrost_FFAirEva_ErrorMaxCycle, //For Air sensor abnormal
					.ErrorMaxNoCoolTime = _Defrost_FFAirEva_ErrorMaxNoCoolTime, //For EVA sensor abnormal
					.RealTimeAfterHotStart = 0, //After start Hot start
					.RealTimeAfterService = 0, //After start Service mode
					.DelayTime = 120,        //PCH Check /Max pre cool time
					.AfterDefrostTime = 0,  // Shift cut-in cout-out time
					.RecoveryModeActive = 1,  // PCH Check before defrost pumpout/ 0:No pumpout 1:Have pumpout
					.DoorOpenedIntervals =                  { 1*5,  3*60, 4*60, 5*60, 10*60     }, // sec*60 = min //5 setting 6 area   
					.VariableMaxRealTime[_Ambient_0] =      { 24*60, 22*60, 18*60, 16*60, 14*60, 12*60 },	//min
					.VariableMaxRealTime[_Ambient_1] =      { 24*60, 22*60, 18*60, 16*60, 14*60, 12*60 },   //min
					.VariableMaxRealTime[_Ambient_2] =      { 24*60, 30*60, 18*60, 16*60, 14*60, 12*60 },   //min
					.VariableMaxRealTime[_Ambient_3] =      { 96*60, 22*60, 18*60, 16*60, 14*60, 12*60 },   //min
					.VariableMaxRealTime[_Ambient_4] =      { 24*60, 22*60, 18*60, 16*60, 14*60, 12*60 },   //min
					  //PCH Check
					.VariableDefrostCutOutTemp[_Ambient_0] = {  32,  32,  32,  32,  32,  32 }, // Celcius *10
					.VariableDefrostCutOutTemp[_Ambient_1] = {  32,  32,  32,  32,  32,  32 }, // Celcius *10
					.VariableDefrostCutOutTemp[_Ambient_2] = {  32,  32,  32,  32,  32,  32 }, // Celcius *10
					.VariableDefrostCutOutTemp[_Ambient_3] = {  32,  32,  32,  32,  32,  32 }, // Celcius *10
					.VariableDefrostCutOutTemp[_Ambient_4] = {  32,  32,  32,  32,  32,  32 }, // Celcius *10
#if defined(AI_COOL_V1)                                                       //AI_MODE ==>
					.DoorOpenedIntervals_AI =             { 1*30,  1*60,  2*60,  4*60,  7*60 }, // min
					.VariableMaxRealTime_AI[_Ambient_0] = { 6*60,   6*60,  5*60,  5*60,  4*60,   240 },
					.VariableMaxRealTime_AI[_Ambient_1] = {5*60,   5*60,  5*60,  5*60,  5*60,   300 },
					.VariableMaxRealTime_AI[_Ambient_2] = {8*60,  8*60,  7*60,  7*60,  6*60,   300 },
					.VariableMaxRealTime_AI[_Ambient_3] = {6*60,  6*60,  6*60,  6*60,  6*60,   300 },
					.VariableMaxRealTime_AI[_Ambient_4] = {10*60,  8*60,  7*60,  7*60,  6*60,   300 },
					.VariableDefrostCutOutTemp_AI[_Ambient_0] = { 55,  55,  55,  55,  65,  65 }, // Celcius *10
					.VariableDefrostCutOutTemp_AI[_Ambient_1] = { 55,  55,  55,  55,  65,  65 }, // Celcius *10
					.VariableDefrostCutOutTemp_AI[_Ambient_2] = { 55,  55,  55,  55,  65,  65 }, // Celcius *10
					.VariableDefrostCutOutTemp_AI[_Ambient_3] = { 55,  55,  55,  55,  65,  65 }, // Celcius *10
					.VariableDefrostCutOutTemp_AI[_Ambient_4] = { 55,  55,  55,  55,  65,  65 }, // Celcius *10
					//AI_MODE END
#endif
				},
				.Fan = {
					.SpeedLevel = { 100, 100, 100, 100, 100, 100, 100, 100, 100 },//For no Tc sensor model
                                        /// NER			  Fan spped level L1,  L2,  L3,  L4,  LD, LH
					.AmbientSpeedLevel[_Ambient_0] = { 50,  60,  70,  80,  90, 100},
					.AmbientSpeedLevel[_Ambient_1] = { 60,  60,  80,  90,  90, 100},
					.AmbientSpeedLevel[_Ambient_2] = { 75,  90,  90,  100,  90, 100},
					.AmbientSpeedLevel[_Ambient_3] = { 75,  90,  90,  100,  90, 100},
					.AmbientSpeedLevel[_Ambient_4] = {100, 100, 100, 100, 100, 100},
					.CompressorOFF    = {true, true, true, true, true},//We donot use
					.CompressorON_FRZ = {true, true, true, true, true},//We donot use
					.SpeedError = 80,//For abnormal
					.SpeedQuick = 100,//Quick FF
					.SpeedSilent = 0,
					.SpeedHotStart = 95,
					.SpeedPumpOut = 0, // Fan speed @ _Work_PumpOut ,set 0
					.SpeedConvertible = 80, //Variable temperature compartment
					.SpeedIdle = 60,
					.MinDuty_DO = _Fan_FFAirEva_MinDuty_DO,
					.MaxDuty_DO = _Fan_FFAirEva_MaxDuty_DO,
					.MinDuty_DC = 1,
					.MaxDuty_DC = 100,
					.CutOutHysteresis = 1, //0.1C
					.WorkTimeAfterCool = 0, //minutes  ignore
					.CutInTime = 3, // minutes /Delay time after comp on, after 
					.HotStartDelayTime = 10, //_Fan_HotStartDelay,minutes
					.minStayTimeAtPresFanLevel = 10,
					.maxStayTimeAtPresFanLevel = 75,
 					 /*** APW : Add CT heater software ***/
 					.secOnTimeFlapFanCommercial = 0,        // Add Cabin0 and Cabin1 same value
					.secOnTimeFfFanCommercial = 40,         // Add Cabin0 and Cabin1 same value
					.secOnTimeFrzFanCommercial = 40,        // Add Cabin0 and Cabin1 same value
 					 /*** APW : Add CT heater software ***/
 					 //PCH Check
					.Offsets =  {0,0,0,0,0}, //Same as air sensor setting
					.PreCoolOffsets =  {0,0,0,0,0}, //Same as air sensor setting
					.AirSupplier = 0,
				},
				.Flap = {
					.PositionA = _Flap_ALL_PositionA,
					.PositionB = _Flap_ALL_PositionB,
					.OnTimeA = _Flap_ALL_OnTimeA,
					.OffTimeA = _Flap_ALL_OffTimeA,
					.OnTimeB = _Flap_ALL_OnTimeB,
					.OffTimeB = _Flap_ALL_OffTimeB,
				},
				.VacuumBox = {
					.VacuumBoxFirstOFFTime = ( 6 * 60 ),    //6min_delay time first plug
					.VacuumBox_OFFTime = ( 6 * 60 ),        //interval time normal
					.VacuumBox_ONTime1 = 60,                //ON time interval time normal
					.VacuumBox_ONTime2 = 123,               //ON time after close door
					.VacuumBox_ONDelayTime = 60,            //Delay time after close door
					.VacuumBoxHeater = {
						.OnTime [_Ambient_0] = {  0,  0,  0,  0,  0}, // second
						.OffTime[_Ambient_0] = {  0,  0,  0,  0,  0}, // second
						.OnTime [_Ambient_1] = {  0,  0,  0,  0,  0}, // second
						.OffTime[_Ambient_1] = {  0,  0,  0,  0,  0}, // second
						.OnTime [_Ambient_2] = {  0,  0,  0,  0,  0}, // second
						.OffTime[_Ambient_2] = {  0,  0,  0,  0,  0}, // second
						.OnTime [_Ambient_3] = {  0,  0,  0,  0,  0}, // second
						.OffTime[_Ambient_3] = {  0,  0,  0,  0,  0}, // second
						.OnTime [_Ambient_4] = {  0,  0,  0,  0,  0}, // second
						.OffTime[_Ambient_4] = {  0,  0,  0,  0,  0}, // second						
						.TemperaturesZone =      { -10, 10, 30, 50},
					},
					.VacuumBoxCurrentSensor = {
						.JudgmentLockA = 800,       // Vlock : (Sensor = 950)
						.JudgmentLockB = 1000,      // Vlock : (Sensor = 950)
						.JudgmentStopA = -147,      // Vstop : 2.00V(-147)
						.JudgmentStopB = -100,      // Vstop : 1.69V(-100)
						.JudgmentLeak = 50,         // Vleak : 0.17V(50)
					},
				},
				.HeaterA = {
												//Ambient 0,1,2,3,4
					.OnTimeA = _HeaterC_ALL_OnTimeA,
					.OffTimeA = _HeaterC_ALL_OffTimeA,
					.OnTimeB = _HeaterC_ALL_OnTimeB,
					.OffTimeB = _HeaterC_ALL_OffTimeB,
					.humidityHeater = {
					//PCH Check                         125, 205, 285, 355 
						.OnTime [_Humidity_0] = {  0,   0,   0,   0,   0,},   // 1:less than 10 /Minutes
						.OffTime[_Humidity_0] = {  0,   0,   0,   0,   0,}, 	  
						.OnTime [_Humidity_1] = {  0,   0,   0,   0,   0,},   // 2:less than 10 /Minutes
						.OffTime[_Humidity_1] = {  0,   0,   0,   0,   0,}, 		  
						.OnTime [_Humidity_2] = {  1,   1,   1,   1,   1,},   // 3:less than 10 /Minutes
						.OffTime[_Humidity_2] = { 18,  18,   9,   9,   9,},		  
						.OnTime [_Humidity_3] = {  2,   2,   2,   1,   3,},   // 4:less than 10 /Minutes
						.OffTime[_Humidity_3] = { 11,   9,   8,   3,   7,}, 		  
						.OnTime [_Humidity_4] = {  2,   1,   8,   8,   8,},   // 5:less than 10 /Minutes
						.OffTime[_Humidity_4] = {  8,   3,   2,   2,   2,},
						.OnTime [_Humidity_5] = {  6,   6,   9,   9,   9,},   // 6:less than 10 /Minutes
						.OffTime[_Humidity_5] = { 11,  11,   1,   1,   1,}, 	  
						.OnTime [_Humidity_6] = {  5,   5,  10,  10,  10,},   // 7:less than 10 /Minutes
						.OffTime[_Humidity_6] = {  6,   5,   0,   0,   0,}, 		  
						.OnTime [_Humidity_7] = {  5,   4,  10,  10,  10,},   // 8:less than 10 /Minutes
						.OffTime[_Humidity_7] = {  5,   3,   0,   0,   0,}, 		  
						.OnTime [_Humidity_8] = {  6,   7,  10,  10,  10,},   // 9:less than 10 /Minutes
						.OffTime[_Humidity_8] = {  4,   4,   0,   0,   0,}, 		  
						.OnTime [_Humidity_9] = {  7,   8,  10,  10,  10,},   // 10:less than 10 /Minutes
						.OffTime[_Humidity_9] = {  3,   3,   0,   0,   0,},
					},
				},
				.HeaterB = { //defrost
					.OnTimeA = _HeaterB_ALL_OnTimeA, //DO
					.OffTimeA = _HeaterB_ALL_OffTimeA,
					.OnTimeB = _HeaterB_ALL_OnTimeB, //DC
					.OffTimeB = _HeaterB_ALL_OffTimeB,
				},
				.HeaterC = {
					.OnTimeA = _HeaterC_ALL_OnTimeA,
					.OffTimeA = _HeaterC_ALL_OffTimeA,
					.OnTimeB = _HeaterC_ALL_OnTimeB,
					.OffTimeB = _HeaterC_ALL_OffTimeB,
					.humidityHeater = {
					//Handle_heater              
						.OnTime [_Humidity_0] = { 0,  0,  0,  0,  0,},   // less than 10 /Minutes
						.OffTime[_Humidity_0] = {10, 10, 10, 10, 10,}, 	  
						.OnTime [_Humidity_1] = { 0,  0,  0,  0,  0,},  // between 10-20 
						.OffTime[_Humidity_1] = {10, 10, 10, 10, 10,}, 		  
						.OnTime [_Humidity_2] = { 0,  0,  0,  0,  0,},  // between 20-30 
						.OffTime[_Humidity_2] = {10, 10, 10, 10, 10,}, 		  
						.OnTime [_Humidity_3] = { 0,  0,  0,  0,  0,},  // between 30-40
						.OffTime[_Humidity_3] = {10, 10, 10, 10, 10,}, 		  
						.OnTime [_Humidity_4] = { 0,  0,  0,  0,  0,},  // between 40-50
						.OffTime[_Humidity_4] = {10, 10, 10, 10, 10,},
						.OnTime [_Humidity_5] = { 0,  0,  0,  0,  0,},   // between 50-60
						.OffTime[_Humidity_5] = {10, 10, 10, 10, 10,}, 	  
						.OnTime [_Humidity_6] = { 0,  0,  0,  0,  0,},  // between 60-70
						.OffTime[_Humidity_6] = {10, 10, 10, 10, 10,}, 		  
						.OnTime [_Humidity_7] = { 0,  0,  0,  0,  0,},  // between 70-80
						.OffTime[_Humidity_7] = {10, 10, 10, 10, 10,}, 		  
						.OnTime [_Humidity_8] = { 0,  0,  0,  0,  0,},  // between 80-90
						.OffTime[_Humidity_8] = {10, 10, 10, 10, 10,}, 		  
						.OnTime [_Humidity_9] = { 0,  0,  0,  0,  0,},  // between 90-100
						.OffTime[_Humidity_9] = {10, 10, 10, 10, 10,},
					},
				},
				.Level = { //No ambient sensor model
					.dutyFF  = { .low = 50, .hgh = 90, },
					.dutyFRZ = { .low = 50, .hgh = 90, },
					.dtFF  = { .lowDuty = {20, 40}, .medDuty = {20, 30, 32, 35, 38, 40}, .hghDuty = {5, 10}, },
					.dtFRZ = { .lowDuty = {20, 40}, .medDuty = {20, 30, 32, 35, 38, 40}, .hghDuty = {5, 10}, },
					.levelFanFF =  { .lowDuty = {1, 2, 3}, .medDuty = {2, 3, 4, 5, 6, 7, 8}, .hghDuty = {7, 8, 9}, },
					.levelFanFRZ = { .lowDuty = {1, 2, 3}, .medDuty = {2, 3, 4, 5, 6, 7, 8}, .hghDuty = {7, 8, 9}, },
					.levelGasFF =  { .lowDuty = {1, 2, 3}, .medDuty = {2, 3, 4, 5, 6, 7, 8}, .hghDuty = {9, 9, 9}, },
					.levelGasFRZ = { .lowDuty = {1, 2, 3}, .medDuty = {2, 3, 4, 5, 6, 7, 8}, .hghDuty = {9, 9, 9}, },
				},
					/// NER
				.AmbientLevel = {
					.FF ={ 	//FF air sensor - CO value
						.dt[_Ambient_0] = {50, 80, 120},
						.dt[_Ambient_1] = {50, 80, 120},
						.dt[_Ambient_2] = {50, 80, 120},
						.dt[_Ambient_3] = {70, 80, 120},
						.dt[_Ambient_4] = {50, 80, 120},
					},
					.FanFF = {	//Fan speed level 
				                      //<10,10-40,40-50,>50
						.level[_Ambient_0] = {1, 2, 3, 4},
						.level[_Ambient_1] = {1, 2, 3, 4},
						.level[_Ambient_2] = {1, 2, 3, 4},
						.level[_Ambient_3] = {1, 2, 3, 4},
						.level[_Ambient_4] = {1, 2, 3, 4},
					},
					.GasFF = {	//Comp speed level
						.level[_Ambient_0] = {1, 2, 3, 4},
						.level[_Ambient_1] = {1, 2, 3, 4},
						.level[_Ambient_2] = {1, 2, 3, 4},
						.level[_Ambient_3] = {1, 2, 3, 4},
						.level[_Ambient_4] = {1, 2, 3, 4},
					},
				},
				.VioLed = {//We donot have
					.maxOnTime = (2 * 60),	   // dk
					.maxOffTime = (48 * 60),   // dk
					.maxDisplayActivation = 3, // count
					.maxDelayTime = 15,		   // dk
					.maxOffTimeFirstStart = (48 * 60) , // dk ilk kalkis durma suresi
				},
					/*** APW : Add CT heater software ***/
					.TempJudgmentValveFfCommercial = 50,    // (50-> 5 degree)Add Temp for judgment valve switch
					.TempJudgmentValveFrzCommercial = 50,   // (50-> 5 degree)Add Temp for judgment valve switch
					/*** APW : Add CT heater software ***/
			},
		},

		.Cabin[1] = { //FRZ compartment
			.type = _CabinType_FRZ,
			.components = {
				.knob = {
					.type = _KnobType_NONE,
					.index = 0xFF,
				},
				.fan = {
					.type = _FanType_VDC, //_FanType_AC,
					.index = 1,
					.configs = {
						.powerPinUsed = false,
						.pwmUsed = true,
						.fgPinUsed = true, //For Mass  false, //For DEBUG 
						.rpm = 1380,
						.ppr = 2,
					},
					.test = {
						.startTime = 53,        //Prodtest
						.endTime = 73,
					},
                                        .maxPower = 238, //2.38 watt
				},
				.flap = {
					.type = _FlapType_NONE,
					.index = 0xFF,
				},
				.heaterA = {
					.type = _HeaterType_AC, //rpHeat Bootheater + ICD heater //_HeaterType_NONE,
					.index = 3, //0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
					.commercialDefrostTestPriority = 5,  //APW : Add CT heater software
					.resistanceForPower = 5378,
				},
				.heaterB = {// Alminum heater
					.type = _HeaterType_AC,
					.index = 1, //acFdef    //Prodtest
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
					.commercialDefrostTestPriority = 1,  //APW : Add CT heater software
					.resistanceForPower = 383,  //ohm
				},
				.heaterC = { //Gutter Heater
					.type = _HeaterType_AC,
					.index = 0, // acRDEF GutterHeater for BONA 0, //GutterHeater for ALPS78 // Prodtest
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
					.commercialDefrostTestPriority = 2,  //APW : Add CT heater software
					.resistanceForPower = 2361, // ohm
				},
				.ionizer = {
					.type = _IonizerType_NONE,
					.index = 0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.innerLight = {
					.type = _LightType_PWM,
					.index = 1,
					.test = {
						.startTime = 2,
						.endTime = 17,
					},
					.maxPower = 300, //3 watt
				},

				.blueLight = {
					.type = _LightType_NONE,
					.index = 0xFF,
					.test = {
						.startTime = 0,
						.endTime = 0,
					},
				},
				.doorSwitch[0] = {
					.type = _SwitchType_DC,
					.index = 1,
				},
				.doorSwitch[1] = {
					.type = _SwitchType_NONE,
					.index = 0xFF,
				},
				.humanSensor = {
					.type = _SwitchType_NONE,
					.index = 0xFF,
				},
				.airTempSensor = {
					.type = _TempSensorType_NTC_F_air_ADC,
					.index = 2,
					.hideError = 0,
				},
				.evaTempSensor = {
					.type = _TempSensorType_NTC_F_eva_ADC,
					.index = 3,
					.hideError = 0,
				},
			},
			.sets = {
				.configs = {
					.FF = 0,
					.FRZ = 7,
					.functions = {
						.QuickFF = 0, .QuickFRZ = 1, .Vacation = 0, .Rice = 0, .QuickIce = 0, // Quick Ice Active
						.Convertible = 0, .CabinOff = 0,
					},
				},
				.indexes = {
					.FF =  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, }, // FF[10]
					.FRZ = { 14, 15, 16, 17, 11, 12, 13, 0xFF, 0xFF, 0xFF,               }, // FRZ[10]
					.functions = {
						.QuickFF = 0xFF, .QuickFRZ = 2, .Vacation = 0xFF, .Rice = 0xFF, .QuickIce = 2, // Quick Ice Set from table
						.Convertible = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, },
						.CabinOff = 0xFF,
					},
				},
			},
			.parameters = {
				.permannentHighTempError = 1,
				.HighTempError = {
					.HighTempErrCutIN = -23, //FRZ AIR sensor
					.HighTempErrCutOUT = -90, //Lower value is OK
					.HighTmp_Time_PowerUp = 1, //1:Use .HighTmp_Algorithm_Activation
					.HighTmp_Time_Defrost = 1500, //minutes 
					.HighTmp_Time_Normal = 1500, //minutes
					.HighTmp_Algorithm_Activation = 1440 * 3, //After Power on, function do not operate during this time
					.HighTmp_DelayAlarm = 360, //After customer stopped alarm, delay time for re-check temperature
				},
				.Performance = {
					.timeEva1 = _Performance_TimeEva1, .timeEva2 = _Performance_TimeEva2, .tempEva1 = _Performance_TempEva1, .tempEva2 = _Performance_TempEva2, .timeAir = _Performance_TimeAir, .deltaTempAir = _Performance_DeltaTempAir,
					//.thermalCutOut = -170,
				},
				.Quick = {
					.timeFFMode = _Quick_TimeFFMode,
					.timeFRZMode = 720,//_Quick_TimeFRZMode=24hrs//minute
				},
				.Cool = {
					.SabbathMaxIdleTime = _Cool_FRZAirEva_SabbathMaxIdleTime,
					.MaxIdleTime = _Cool_FRZAirEva_MaxIdleTime,
					.ErrorMaxIdleTime = _Cool_FRZAirEva_ErrorMaxIdleTime,
					.ErrorMaxCoolTime = _Cool_FRZAirEva_ErrorMaxCoolTime,
					.MaxIdleTimeElapsedCoolTime = 15,
					.HotStartTime = 15, //_Cool_HotStartTime,
					.HotStart2Time = 485, //_Cool_HotStartTime,
					.PerformanceTestTime = 15,
					.PerformanceTest2Time = 135,
					.pumpOutTime = 60,
					.Offsets =  {-20,0,-10,0,0},
					.PreCoolOffsets =  {0,0,0,0,0},
					.AfterDefrostOffsets =  {0,0,0,0,0}, //  After Defrost shif temp.
					.OffsetsShiftCutIn =  {0,0,0,0,0},//Ambien shif temp.
					.OffsetsShiftCutOut =  {0,0,0,0,0},//Ambien shif temp.
#if defined(AI_COOL_V1)
					.CabinSetsAmbientOffSetAI = { 13,  -7,  0,  1,  8}, // FRZ Cool OFFSET A_COOL
					.FanSetsAmbientOffSetAI   = { 13,  -7,  0,  1,  8}, // FRZ fan OFFSET A_COOL
#endif
				},
				.Defrost = {
					.Type = _DefrostType_Variable, //_DefrostType_Normal,
					.MaxRealTime_DO = 1800, //2200, //_Defrost_FRZAirEva_MaxRealTime_DO,
					.MaxRealTime_DC = 1800, //2200, //_Defrost_FRZAirEva_MaxRealTime_DC,
					.MaxQuickTime = 1320, //_Defrost_FRZAirEva_MaxQuickTime,
					.MaxWorkTime_DO = 96*60, //2200, //_Defrost_FRZAirEva_MaxWorkTime_DO,
					.DefrostCutOutTemp = _Defrost_FRZAirEva_DefrostCutOutTemp, //100, //_Defrost_FRZAirEva_DefrostCutOutTemp, // ozgur
					.MaxDefrostHighTempTime = 720,                //Minute    (Defrost by High temperature) : set 0xFFFF for disable mode
					.MaxDefrostHighTempShiftCutInTemp = 90,       //(Defrost by High temperature)  !!If set 0, will same as cutIN
					.MaxDefrostHighTempShiftCutOutTemp = 40,      //(Defrost by High temperature)  !!If set 0, will same as cutIN
					.DefrostHighTemp_Activation = 1440,           //Activation time after power on
					.MaxDefrostTime_DO = 90,//_Defrost_FRZAirEva_MaxDefrostTime_DO, //60
					.MaxDefrostTime_DC = 90,//_Defrost_FRZAirEva_MaxDefrostTime_DO, //60 _Defrost_FRZAirEva_MaxDefrostTime_DC, //5
					.ErrorMaxDefrostTime = _Defrost_FRZAirEva_ErrorMaxDefrostTime,
					//.MaxDelayTimeForIceCream = (8 * 60),
					.RealTimeAfterHotStart = 0,
					.RealTimeAfterService = 840,
					.DelayTime = 120,
					.AfterDefrostTime = 0,  // Shift cut-in cout-out time
					.RecoveryModeActive = 1,
					.DoorOpenedIntervals =                      { 1*5,  3*60, 4*60, 5*60, 10*60     }, // sec
					.VariableMaxRealTime[_Ambient_0] =       { 24*60, 22*60, 18*60, 16*60, 14*60, 12*60 }, //min
					.VariableMaxRealTime[_Ambient_1] =       { 24*60, 22*60, 18*60, 16*60, 14*60, 12*60 }, //min
					.VariableMaxRealTime[_Ambient_2] =       { 24*60, 30*60, 18*60, 16*60, 14*60, 12*60 }, //min
					.VariableMaxRealTime[_Ambient_3] =       { 96*60, 22*60, 18*60, 16*60, 14*60, 12*60 }, //min
					.VariableMaxRealTime[_Ambient_4] =       { 24*60, 22*60, 18*60, 16*60, 14*60, 12*60 }, //min
					.VariableDefrostCutOutTemp[_Ambient_0] = {  150,  150,  150,  150,  150,  150 }, // Celcius *10
					.VariableDefrostCutOutTemp[_Ambient_1] = {  150,  150,  150,  150,  150,  150 }, // Celcius *10
					.VariableDefrostCutOutTemp[_Ambient_2] = {  83,  83,  150,  150,  150,  150 }, // Celcius *10
					.VariableDefrostCutOutTemp[_Ambient_3] = {  83,  83,  150,  150,  150,  150 }, // Celcius *10
					.VariableDefrostCutOutTemp[_Ambient_4] = {  150,  150,  150,  150,  150,  150 }, // Celcius *10
#if defined(AI_COOL_V1)                                                       //AI_MODE ==>
					.DoorOpenedIntervals_AI =             { 1*60, 2*60, 4*60, 6*60, 7*60 }, // min
					.VariableMaxRealTime_AI[_Ambient_0] = {32*60, 32*60, 32*60, 32*60, 32*60, 32*60 },
					.VariableMaxRealTime_AI[_Ambient_1] = {96*60, 34*60, 32*60, 32*60, 32*60, 32*60 },
					.VariableMaxRealTime_AI[_Ambient_2] = {96*60, 34*60, 32*60, 32*60, 32*60, 32*60 },
					.VariableMaxRealTime_AI[_Ambient_3] = {96*60, 84*60, 70*60, 50*60, 30*60, 12*60 },
					.VariableMaxRealTime_AI[_Ambient_4] = {96*60, 84*60, 70*60, 50*60, 30*60, 12*60 },
					.VariableDefrostCutOutTemp_AI[_Ambient_0] = {  100,  100,  100,  100,  100,  100 }, // Celcius *10
					.VariableDefrostCutOutTemp_AI[_Ambient_1] = {  100,  100,  100,  100,  100,  100 }, // Celcius *10
					.VariableDefrostCutOutTemp_AI[_Ambient_2] = {  100,  100,  100,  100,  100,  100 }, // Celcius *10
					.VariableDefrostCutOutTemp_AI[_Ambient_3] = {  100,  100,  100,  100,  100,  100 }, // Celcius *10
					.VariableDefrostCutOutTemp_AI[_Ambient_4] = {   29,  100,  100,  100,  100,  100 }, // Celcius *10
					//AI_MODE END
#endif
					//.delayTime = 60,
				},
				.Drainage = { //DEF off time
					.MaxDrainageTime = 10, // PCH Check _Drainage_FRZ_MaxDrainageTime,
				},
				.Fan = {
					.SpeedLevel = { 80, 80, 80, 80, 80, 80, 80, 80, 80 },
					/// NER                        /* L1, L2, L3, L4, LD, LH */
					.AmbientSpeedLevel[_Ambient_0] = { 75,  75,  80,  90,  90, 100},
					.AmbientSpeedLevel[_Ambient_1] = { 75,  80,  80,  100,  90, 100},
					.AmbientSpeedLevel[_Ambient_2] = { 75,  90,  90,  100,  90, 100},
					.AmbientSpeedLevel[_Ambient_3] = { 80,  90,  90,  100,  90, 100},
					.AmbientSpeedLevel[_Ambient_4] = {100, 100, 100, 100, 100, 100},
					.CompressorOFF   = {false, false, false, false, false},
					.CompressorON_FF = {false, false, true, true, true},
					.SpeedError = 80,
					.SpeedQuick = 100,//Quick FRZ
					.SpeedSilent = 0,
					.SpeedHotStart = 80,
					.SpeedPumpOut = 0, // Fan speed @ _Work_PumpOut ,set 0
					.SpeedIdle = 80,
					.CutInEvaTemp = _Fan_FRZAirEva_CutInTemp, // _Fan_FRZAirEva_CutInTemp,
					.CutInTime = 2,
					.WorkTimeAfterCool = 0,
					.HotStartDelayTime = 10, //_Fan_HotStartDelay,
					.minStayTimeAtPresFanLevel = 10,
					.maxStayTimeAtPresFanLevel = 75,
					.Offsets =  {0,0,0,0,0},
					.PreCoolOffsets =  {0,0,0,0,0},
					.AirSupplier = 0,
					/*** APW : Add CT heater software ***/
					.secOnTimeFlapFanCommercial = 0,        // Add Cabin0 and Cabin1 same value
					.secOnTimeFfFanCommercial = 40,         // Add Cabin0 and Cabin1 same value
					.secOnTimeFrzFanCommercial = 40,        // Add Cabin0 and Cabin1 same value
					/*** APW : Add CT heater software ***/
				},
				.Flap = {
					.PositionA = _Flap_ALL_PositionA,
					.PositionB = _Flap_ALL_PositionB,
					.OnTimeA = _Flap_ALL_OnTimeA,

					.OffTimeA = _Flap_ALL_OffTimeA,
					.OnTimeB = _Flap_ALL_OnTimeB,
					.OffTimeB = _Flap_ALL_OffTimeB,
				},
                                
				.HeaterA = { 
					.OnTimeA = 60,  //second  //PCH Check
					.OffTimeA = 0, //second
					.OnTimeB = _HeaterA_ALL_OnTimeB, //Do not use
					.OffTimeB = _HeaterA_ALL_OffTimeB,
					.humidityHeater = {
					//PCH Check
						.OnTime [_Humidity_0] = {  0,  0,  0,   0,  0,},   // less than 10 /Minutes
						.OffTime[_Humidity_0] = {  0,  0,  0,   0,  0,}, 	  
						.OnTime [_Humidity_1] = {  0,  0,  0,   0,  0,},   // less than 10 /Minutes
						.OffTime[_Humidity_1] = {  0,  0,  0,   0,  0,}, 		  
						.OnTime [_Humidity_2] = {  0,  0,  0,   0,  0,},   // less than 10 /Minutes
						.OffTime[_Humidity_2] = {  0,  0,  0,   0,  0,}, 		  
						.OnTime [_Humidity_3] = {  0,  0,  0,   0,  0,},   // less than 10 /Minutes
						.OffTime[_Humidity_3] = {  0,  0,  0,   0,  0,}, 		  
						.OnTime [_Humidity_4] = {  0,  0,  0,   0,  0,},   // less than 10 /Minutes
						.OffTime[_Humidity_4] = {  0,  0,  0,   0,  0,},
						.OnTime [_Humidity_5] = {  0,  0,  0,   0,  1,},   // less than 10 /Minutes
						.OffTime[_Humidity_5] = {  0,  0,  0,   0,  9,}, 	  
						.OnTime [_Humidity_6] = {  0,  0,  0,   0,  1,},   // less than 10 /Minutes
						.OffTime[_Humidity_6] = {  0,  0,  0,   0,  9,}, 		  
						.OnTime [_Humidity_7] = {  1,  1,  1,   1,  2,},   // less than 10 /Minutes
						.OffTime[_Humidity_7] = { 18, 13,  8,   8,  8,}, 		  
						.OnTime [_Humidity_8] = {  2,  1,  1,   1,  6,},   // less than 10 /Minutes
						.OffTime[_Humidity_8] = {  8,  3,  2,   2,  4,}, 		  
						.OnTime [_Humidity_9] = {  5,  6,  7,  10,  9,},   // less than 10 /Minutes
						.OffTime[_Humidity_9] = {  5,  5,  4,   3,  1,},
					},
				},

                                
				.HeaterB = {
					.OnTimeA = _HeaterB_ALL_OnTimeA,
					.OffTimeA = _HeaterB_ALL_OffTimeA,
					.OnTimeB = _HeaterB_ALL_OnTimeB,
					.OffTimeB = _HeaterB_ALL_OffTimeB,
				},
				.HeaterC = { //Gutter Heater
					.OnTimeA = 60,  //second  //PCH Check
					.OffTimeA = 0, //second
					.OnTimeB = _HeaterC_ALL_OnTimeB, //Do not use
					.OffTimeB = _HeaterC_ALL_OffTimeB,
					.humidityHeater = {
					//PCH Check
						.OnTime [_Humidity_0] = { 10,  10,  10,  10,  10,},   // less than 10 /Minutes
						.OffTime[_Humidity_0] = {  0,   0,   0,   0,   0,}, 	  
						.OnTime [_Humidity_1] = { 10,  10,  10,  10,  10,},  // between 10-20 
						.OffTime[_Humidity_1] = {  0,   0,   0,   0,   0,}, 		  
						.OnTime [_Humidity_2] = { 10,  10,  10,  10,  10,},  // between 20-30 
						.OffTime[_Humidity_2] = {  0,   0,   0,   0,   0,}, 		  
						.OnTime [_Humidity_3] = { 10,  10,  10,  10,  10,},  // between 30-40
						.OffTime[_Humidity_3] = {  0,   0,   0,   0,   0,}, 		  
						.OnTime [_Humidity_4] = { 10,  10,  10,  10,  10,},  // between 40-50
						.OffTime[_Humidity_4] = {  0,   0,   0,   0,   0,},
						.OnTime [_Humidity_5] = { 10,  10,  10,  10,  10,},   // between 50-60
						.OffTime[_Humidity_5] = {  0,   0,   0,   0,   0,}, 	  
						.OnTime [_Humidity_6] = { 10,  10,  10,  10,  10,},  // between 60-70
						.OffTime[_Humidity_6] = {  0,   0,   0,   0,   0,}, 		  
						.OnTime [_Humidity_7] = { 10,  10,  10,  10,  10,},  // between 70-80
						.OffTime[_Humidity_7] = {  0,   0,   0,   0,   0,}, 		  
						.OnTime [_Humidity_8] = { 10,  10,  10,  10,  10,},  // between 80-90
						.OffTime[_Humidity_8] = {  0,   0,   0,   0,   0,}, 		  
						.OnTime [_Humidity_9] = { 10,  10,  10,  10,  10,},  // between 90-100
						.OffTime[_Humidity_9] = {  0,   0,   0,   0,   0,},
					},
				},
				.Level = { //No ambient sensor model
					.dutyFF  = { .low = 50, .hgh = 90, },
					.dutyFRZ = { .low = 50, .hgh = 90, },
					.dtFF  = { .lowDuty = {20, 40,}, .medDuty = {20, 30, 32, 35, 38, 40}, .hghDuty = {5, 10,}, },
					.dtFRZ = { .lowDuty = {20, 40,}, .medDuty = {20, 30, 32, 35, 38, 40}, .hghDuty = {5, 10,}, },
					.levelFanFF =  { .lowDuty = {1, 2, 3}, .medDuty = {2, 3, 4, 5, 6, 7, 8}, .hghDuty = {7, 8, 9}, },
					.levelFanFRZ = { .lowDuty = {1, 2, 3}, .medDuty = {2, 3, 4, 5, 6, 7, 8}, .hghDuty = {7, 8, 9}, },
					.levelGasFF =  { .lowDuty = {1, 2, 2}, .medDuty = {2, 3, 4, 5, 6, 7, 8}, .hghDuty = {9, 9, 9}, },
					.levelGasFRZ = { .lowDuty = {3, 3, 3}, .medDuty = {2, 3, 4, 5, 6, 7, 8}, .hghDuty = {9, 9, 9}, },
				},
					/// NER 
					.AmbientLevel = {
						.FRZ ={ //Air Sensor
							.dt[_Ambient_0] = {80, 120, 180}, 
							.dt[_Ambient_1] = {80, 120, 180},
							.dt[_Ambient_2] = {50, 100, 150},
							.dt[_Ambient_3] = {100, 150, 200}, // Check at load processing test
							.dt[_Ambient_4] = {80, 120, 180},
						},
						.FanFRZ = {//Fan duty
							.level[_Ambient_0] = {1, 2, 3, 4},
							.level[_Ambient_1] = {1, 2, 3, 4},
							.level[_Ambient_2] = {1, 2, 3, 4},
							.level[_Ambient_3] = {1, 2, 3, 4},
							.level[_Ambient_4] = {1, 2, 3, 4},
						},
						.GasFRZ = {//Comp. Level
							.level[_Ambient_0] = {1, 2, 3, 4},
							.level[_Ambient_1] = {1, 2, 3, 4},
							.level[_Ambient_2] = {1, 2, 3, 4},
							.level[_Ambient_3] = {1, 2, 3, 4},
							.level[_Ambient_4] = {1, 2, 3, 4},
						},
					},
					/*** APW : Add CT heater software ***/
					.TempJudgmentValveFfCommercial = 50,    // (50-> 5 degree)Add Temp for judgment valve switch
					.TempJudgmentValveFrzCommercial = 50,   // (50-> 5 degree)Add Temp for judgment valve switch
					/*** APW : Add CT heater software ***/
			},
		},
		/*.Cabin[2] = {
			.type = _CabinType_NONE,
		},
		.Cabin[3] = {
			.type = _CabinType_NONE,
		},*/

};
/*
#pragma location=0x3FF00
__root const t_st_metadata rf_app_meta =
{
    .header = {
        .image_sign = META_IMAGE_SIGN_APPLICATION,
        .version = 1,
        .revision = 0,
        .reserved = {0, 0}
    },
    .image = {
        .product = PROD_ID_REFRIGERATOR,
        .serial = {((uint8_t)(_StockNo >> 32)), ((uint8_t)(_StockNo >> 24)), ((uint8_t)(_StockNo >> 16)), ((uint8_t)(_StockNo >> 8)), ((uint8_t)_StockNo) },
        .version = {_StockVersion, _StockRevision, 0, 0},
        .start_addr = 0x4C00,
        .length = 0x3B300,
        .crc32 = 0xAABBCCDD
    },
    .other_metadatas = {0, 0, 0},
    .reserved = {0},
    .checksum = 0x26
};*/
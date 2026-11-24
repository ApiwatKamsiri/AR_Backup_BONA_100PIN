#ifndef RF_RAM_H
#define RF_RAM_H

#include "rf_common.h"
#include "rf_sets.h"
#include "app/ice_machine/ice_machine_ram.h"
#include "app/connected/connected_ram.h"
#include "app/cooler/cooler_ram.h"
#include "app/dispenser/dispenser_ram.h"
#include "app/cabin/cabin_ram.h"
#include "app/ice_source/ice_source_ram.h"
#include "app/water_source/water_source_ram.h"
#include "dev/display_spi.h"
#include "wm_ram.h"
#include "app/ui/ui_ram.h"

#if defined(AI_COOL_V1_5)
  #include "procedures/rf_ai_cool.h"
#endif

typedef enum
{
	_TempType_START = 0,
	_TempType_Celcius = _TempType_START,
	_TempType_Fahrenheit,
	_TempType_END,
} enTempType;



typedef enum
{
    _Error_Start = 0,
    _Error_FRZ_Air = _Error_Start,      //0   E0
    _Error_FRZ_Eva,                     //1   E1
    _Error_FF_Eva,                      //2   E2
    _Error_FF_Air,                      //3   E3
    _Error_FRZ_DefrostHeater,           //4   E4   
    _Error_Cooler_Ambient,              //5   E5
    _Error_IceMachine_CaseTemp = 8,     //8   E8        skip 2 points (errorsCheckTable[6,7])
    _Error_IceMachine_CaseTactSW,       //9   E9
    _Error_JKR_Air,                     //10  E10
    _Error_JKR_Eva,                     //11  E11
    _Error_JKR_DefrostHeater,           //12  E12
    _Error_FRZ_Fan,                     //13  E13
    _Error_JKR_Fan,                     //14  E14
    _Error_Cooler_CondanserFan,         //15  E15
    _Error_FF_Fan,                      //16  E16
    _Error_FLAP_Fan,                    //17  E17
    //_Error_BioFRESH_Fan,
    _Error_FLAP_Air,                    //18  E18
    //_Error_BioFRESH_Air,
    _Error_HighTemperature,             //19  E19       skip 2 points (errorsCheckTable[20,21])
    _Error_Wifi_B1 = 22,                //22  
    _Error_Wifi_B2,                     //23  
    _Error_End = _Error_Wifi_B2,
    _Error_Size,
} enErrors;

#define _SizeOfErrorsCheckTable _Error_Size
#define _SizeOfErrorsTable 50

typedef struct
{
	uint8_t errorsNumber;
	uint8_t sensorErrorsNumber;
	uint8_t fanErrorsNumber;

	short highTempErrNumber;
	unsigned char errorsTable[_SizeOfErrorsTable];
	unsigned char errorsCheckTable[_SizeOfErrorsCheckTable];

} stRefrigeratorErrors;

typedef struct
{
	unsigned char keyLock : 1;
	unsigned char keyLockDspr: 1;
	unsigned char errHighTemp : 1;
	unsigned char doorsClosed : 1;
	unsigned char doorsClosedState : 1;
	unsigned char doorsClosedBlocked : 1;
	unsigned char doorsAlert : 1;
	unsigned char doorsAlertDisable : 1;
	unsigned char doorsAlertingCloseLights : 1;
        unsigned char doorsAlertLevelIncrease : 1;
        unsigned char doorsAlertLevelIncreasePrev : 1;
	unsigned char dispOff : 1;
        unsigned char vacuumBoxLeak : 1;
	enTempType tempType : 2;
	teDisplayDimLevels dispDim;

	struct
	{
		unsigned char sensorsChecked : 1;
		unsigned char cabinsChecked : 1;
		unsigned char Fail : 1;
		unsigned char Pass : 1;
	} performance;

	unsigned char doorsOpenedCounter;
	unsigned short doorsClosedTimer;
	unsigned short doorsClosedStateStayTimer;
	unsigned short doorsClosedBlockTimer;
	unsigned short doorsAlertTimer;
	unsigned char doorsAlertingCloseLightsTimer;
	unsigned char doorsAlertTimeSelected;          //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
	unsigned char doorsAlertCount;                 //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
	bool doorsAlertAndDisplay;                     //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
	bool prohibitionAlertDisplay;                  //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
	bool doorsAlertEcoModeFirstTime;               //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
	bool doorAlertBuzzerStart;
        
	unsigned short ecoExtraTimer;
	unsigned short shoppingModeTimer;
	unsigned short sabbathTimer;
	unsigned short silentTimer;
	unsigned char temperatureConvPointer[_NumOfCabin];
	unsigned char WifiStatus;
	unI2CDevices I2CDevices;
	stRefrigeratorErrors errors;
	bool compsEMCImprovement;
	unsigned char handleLightState;
} stRefrigeratorValues;

typedef struct
{
	// DD: PING TEST
	unsigned char pingTestState;
	unsigned char Sabbath;
	unsigned char EcoExtra;
	unsigned char keyLock;
	unsigned char keyLockDspr;
	unsigned char dispOff;
	unsigned char doorsClosedBlocked;
	unsigned char Silent;
	unsigned char Violed;
	unsigned char WifiStatus;
	unsigned char Convertible;
	unsigned char CabinOff;
	unsigned char wifiErrors;
	enTempType tempType;
	teDisplayDimLevels dispDim;
	unsigned char temperatureConvPointer[_NumOfCabin];
	unsigned short sabbathTimer;
	unsigned short silentTimer;
	unsigned short onVTimer;
	unsigned short offVTimer;
        unsigned short FTCState;
        unsigned short ICTState;
        unsigned char TempSettingFine;
        unsigned int energyMeasurement;
        unsigned int energyTimeInterval;
        unsigned char handleLightState;
#if defined(AI_COOL_V1_5)
        struct{
          unsigned char transitionMatrixCustomer[NUMBER_OF_PERSONAS*NUMBER_OF_PERSONAS][NUMBER_OF_PERSONAS];
        }AI;
#endif   
} stRefrigeratorE2_Ram;

typedef struct
{
	stStock Stock;
	stWorkMode_Ram workMode;
	stRefrigeratorSets_Ram Sets;
	stRefrigeratorValues Values;
	stIceMachine_Ram IceMachine;
	stIceSource_Ram IceSource;      
	stWaterSource_Ram WaterSource;
	stDispenser_Ram Dispenser; 
	//stIceCreamMachine_Ram IceCreamMachine;
	unConnectedAppliance_Ram ConnectedApp;
	stCooler_Ram Cooler;
	stCabin_Ram Cabin[_NumOfCabin];
	tsUiRam ui;
	stRefrigeratorE2_Ram E2;
#if defined(AI_COOL_V1_5)
        stRefrigeratorAI AI;
#endif
} stRefrigerator_Ram;

#define __RF_PARAMS_OF(_memberPtr, _member)   __RF_OF(stRefrigerator_Ram, _memberPtr, _member)

extern stRefrigerator_Ram refrigerator_R;

#endif
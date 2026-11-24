#ifndef COOLER_CONFIGS_H
#define COOLER_CONFIGS_H

#define _CoolerStateSize 25
#define _CoolerGasSize 5
#define _ValveGasStateSize 16

#define _AmbientSize 5
#define _LevelSize 6

#include "app/refrigerator/components_configs.h"
#include "cooler_common.h"
typedef enum
{
	_CoolerType_NONE = 0,
	_CoolerType_DEFAULT,
	_CoolerType_PARALLEL,
	_CoolerType_SEMIPARALLEL,
	_CoolerType_HOTGAS,
} enCoolerTypes;

typedef struct
{
	stCompressor_Flash compressor;
	stFan_Flash condenserFan;
	stTempSensor_Flash ambientTempSensor;
	stHumiditySensor_Flash humiditySensor;
	stSolenoidValve_Flash solenoidValve[2];
        stSwitch_Flash testSwitch[2];                   //APW : Add CT heater software
        stARM_Flash CommercialARM;                      //APW : Add CT heater software
} stCoolerComponents_Flash;

typedef struct
{
	unsigned char type : 4;
	unsigned char priority : 4;
	unsigned char lowLevel : 4;
	unsigned char hghLevel : 4;
} stGasSpecs;

typedef struct
{
	enLogic logic;
	bool queue;
	stGasSpecs gasSpecs[_CoolerGasSize];
	unsigned char valvesDefPosition : 4;
	unsigned char valvesAltPosition : 4;
	unsigned short valvesDefPositionTime;
	unsigned short valvesAltPositionTime;
} stCoolerSystemState;

typedef struct
{
	unsigned char valve0Position : 2;
	unsigned char valve1Position : 2;
	unsigned char gas;
	unsigned char enough;
} stCoolerValveGasState;

typedef struct
{
	unsigned char speedLevel[_AmbientSize][_LevelSize];
        bool maxDialSpeedLevelUpLH;
} stSpeedLevel;

typedef struct
{
	unsigned char intervals[_HumiditySize - 1];
	unsigned char hysteresis[_HumiditySize - 1];
	enHumidityType errorType;
} stHumidity;

typedef struct
{
	unsigned char minStayTimeAtPresGasLevel[_AmbientSize][9];
	unsigned char maxStayTimeAtPresGasLevel[_AmbientSize][9];

	struct
	{
		unsigned char minOnTime;
		unsigned char minOffTime;
		unsigned char speedLevel[9];
		unsigned char speedLevelFF[9];
		unsigned char speedLevelFRZ[9];
		unsigned char speedSilent;
		unsigned char speedStartUp;
		unsigned char speedPumpOut;
		unsigned char timeStartUp;
		unsigned short timeForMaxLevel_DO[_AmbientSize];
		unsigned short timeForMaxLevel_DC[_AmbientSize];
		unsigned char workTimeAtMaxLevel;
		stSpeedLevel FF;
		stSpeedLevel FRZ;
		unsigned char speedPullDownStage_1;
		unsigned char speedPullDownStage_2;
		unsigned char minOnTimePullDownStage_1;
		unsigned char minOnTimePullDownStage_2;
		unsigned char speedFirstCommercial;              // APW : Add CT heater software
		unsigned char speedSecondCommercialValveTest;    // APW : Add CT heater software
        unsigned char minOntimerFirstCommercial;         // APW : Add CT heater software
	} compressor;

	struct
	{
		unsigned char speedLevel[9];
		unsigned char speedLevelFF[9];
		unsigned char speedLevelFRZ[9];
		unsigned char speedSilent;
		unsigned char speedPumpOut;
		signed char speedCheckCorrection;
		unsigned char speedCheckTime;
		signed short speedCheckTemp;
		stSpeedLevel FF;
		stSpeedLevel FRZ;
		unsigned char speedPullDownStage_1;
		unsigned char speedPullDownStage_2;
	} condenserFan;

	struct
	{
		signed short temperatures[_AmbientSize - 1];
		unsigned char hysteresis[_AmbientSize - 1];
		enAmbientType errorType;
		stHumidity humidity;
	} ambient;

	stCoolerSystemState sysStates[_CoolerStateSize];
	stCoolerValveGasState valveGasStates[_ValveGasStateSize];
	unsigned char minValveSwitchtimerCommercial;     //APW : Add CT heater software
    unsigned char minOntimerSecondCommercial;        //APW : Add CT heater software
} stCoolerParameters_Flash;

typedef struct
{
	enCoolerTypes type;
	stCoolerComponents_Flash components;
	stCoolerParameters_Flash parameters;
} stCooler_Flash;

#endif
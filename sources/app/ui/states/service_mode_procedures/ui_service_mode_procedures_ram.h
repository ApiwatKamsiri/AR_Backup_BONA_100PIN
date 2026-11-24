#ifndef UI_SERVICE_MODE_PROCEDURES_RAM_h
#define UI_SERVICE_MODE_PROCEDURES_RAM_h

typedef enum
{
	_Comp_Start = 0,
	_Comp_FF_Fan = _Comp_Start,
	_Comp_FF_Flap,
	_Comp_FF_LowTempHeater,
	_Comp_FF_HeaterB,
	_Comp_FF_HeaterC,
	_Comp_FF_Ionizer,
	_Comp_FF_Violed,
	_Comp_FF_BlueLight,
	_Comp_FF_InnerLight,
	_Comp_FF_HandleLight,

	_Comp_FRZ_Fan,
	//_Comp_FRZ_Flap,
	_Comp_FRZ_HeaterA,
	_Comp_FRZ_DefrostHeater,
	_Comp_FRZ_HeaterC,
	//_Comp_FRZ_Ionizer,
	//_Comp_FRZ_BlueLight,
	_Comp_FRZ_InnerLight,

	_Comp_JKR_Fan,
	//_Comp_JKR_Flap,
	_Comp_JKR_LowTempHeater,
	_Comp_JKR_DefrostHeater,
	_Comp_JKR_HeaterC,
	//_Comp_JKR_Ionizer,
	//_Comp_JKR_BlueLight,
	_Comp_JKR_InnerLight,

	_Comp_FLAP_Fan,
	_Comp_FLAP_Flap,
	_Comp_FLAP_LowTempHeater,
	_Comp_FLAP_FlapHeater,
	_Comp_FLAP_ChannelHeater,
	//_Comp_FLAP_Ionizer,
	//_Comp_FLAP_BlueLight,
	_Comp_FLAP_InnerLight,

	_Comp_IceMachine_CaseRotator,
        _Comp_IceMachine_Fan,
	//_Comp_IceCreamMachine_Mixer,

	_Comp_WaterSource_MainPump,
	_Comp_WaterSource_MainValve,
	_Comp_WaterSource_DispenserPump,
	_Comp_WaterSource_DispenserValve,
	_Comp_WaterSource_IceMachinePump,
	_Comp_WaterSource_IceMachineValve,
	_Comp_WaterSource_WaterVentilHeater,

	_Comp_IceSource_Selector,
	_Comp_IceSource_Shutter,
	_Comp_IceSource_Pusher,

	_Comp_Cooler_CondenserFan,
	_Comp_Cooler_Compressor,
	_Comp_Cooler_SolenoidValve0,
	_Comp_Cooler_SolenoidValve1,

	_Comp_End = _Comp_Cooler_SolenoidValve1,
	_Comp_Size,
} enComponents;

#define _SizeOfComponentsCheckTable _Comp_Size
#define _SizeOfComponentsTable 70

#define NUM_OF_FANS 5
#define FAN_TABLE_SIZE 20

typedef struct
{
	signed char table[FAN_TABLE_SIZE];
	unsigned char lastIndex;
	unsigned char currentIndex;
} stFanTable;

typedef struct
{
	unsigned char index;
	unsigned char number;
	unsigned char table[_SizeOfComponentsTable][2];
	unsigned char checkTable[_SizeOfComponentsCheckTable];

	unsigned char testTimer;

	unsigned char secureRotatorTimer;
	unsigned char securePumpTimer;
	// unsigned char secureValveTimer;
	unsigned char secureSelectorTimer;
	unsigned char secureShutterTimer;

	unsigned char freqTable[30];
	unsigned char currentIndex /*= 0*/;
	unsigned char lastFreqIndex /*= 1*/; // CHECKPOINT

	stFanTable fanTables[NUM_OF_FANS]; // Fans[ 0 - 1 - 2 - 3 ] for cabins. Fans[ 4  ] for condenser
} tsUiSmCompsData;

///////////////////////////

typedef enum
{
	_Par_Start = 0,
	_Par_IceMachinePumpWaterTime = _Par_Start,
	_Par_DispenserLightTime,
	_Par_End = _Par_DispenserLightTime,
	_Par_Size,
} enParameters;

#define _SizeOfParametersCheckTable _Par_Size
#define _SizeOfParametersTable 10

typedef struct
{
	unsigned char index;
	unsigned char number;
	unsigned char table[_SizeOfParametersTable][2];
	unsigned char checkTable[_SizeOfParametersCheckTable];
} tsUiSmParametersData;

/////////////////////////

typedef enum
{
	_Sensor_Start = 0,
	_Sensor_FF_Air = _Sensor_Start,
	_Sensor_FF_Eva,
	_Sensor_FRZ_Air,
	_Sensor_FRZ_Eva,
	_Sensor_JKR_Air,
	_Sensor_JKR_Eva,
	_Sensor_FLAP_Air,
	//_Sensor_FLAP_Eva,
	//_Sensor_BioFresh_Air,
	//_Sensor_BioFresh_Eva,
	_Sensor_Cooler_AmbinetSensor,
	_Sensor_Cooler_HumiditySensor,
	_Sensor_IceMachine_CaseTemp,
	//_Sensor_IceCreamMachine_Temp,
	_Sensor_End = _Sensor_IceMachine_CaseTemp,
	_Sensor_Size,
} enSensor;

#define _SizeOfSensorsCheckTable _Sensor_Size
#define _SizeOfSensorsTable 50

typedef struct
{
	unsigned char index;
	unsigned char number;
	unsigned char table[_SizeOfSensorsTable][2];
	unsigned char checkTable[_SizeOfSensorsCheckTable];
} tsUiSmSensorsData;

#endif
#ifndef COOLER_COMMON_H
#define COOLER_COMMON_H

typedef enum
{
	_GT_None,
	_GT_No,
	_GT_Hot,
	_GT_Failed,
	_GT_Forced,
	_GT_Cold,
	_GT_Cold_FF,
	_GT_Cold_FRZ,
        _GT_HotStart,
        _GT_HotStart2,
        _GT_PerformanceTest,
        _GT_PerformanceTest2,
} enGasType;

typedef enum
{
	_L_None,
	_L_OR,
	_L_AND,
} enLogic;

typedef enum
{
	_Ambient_0 = 0,
	_Ambient_1,
	_Ambient_2,
	_Ambient_3,
	_Ambient_4,
	_Ambient_NONE = 255,
} enAmbientType;

typedef enum
{
	_Humidity_0 = 0,
	_Humidity_1,
	_Humidity_2,
	_Humidity_3,
	_Humidity_4,
        _Humidity_5,
	_Humidity_6,
	_Humidity_7,
	_Humidity_8,
	_Humidity_9,
	_Humidity_NONE = 255,
} enHumidityType;

#define _HumiditySize 10

typedef struct
{
	unsigned char type : 4;
	unsigned char level : 4;
} stGas;

#endif
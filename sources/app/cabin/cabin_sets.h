#ifndef CABIN_SETS_H
#define CABIN_SETS_H

#define LOOK_UP_TABLE_COUNT 33
typedef enum
{
    _CabinMode_NONE = 0,
    _CabinMode_FF,
    _CabinMode_FRZ,
    _CabinMode_END,
} enCabinModes;

typedef struct
{
    enCabinModes Mode;
    unsigned char FF;
    unsigned char FRZ;
    unsigned char Convertible;
    unsigned char MaxDialUserSet;
    struct
    {
        unsigned char Quick : 1;
        unsigned char Vacation : 1;
        unsigned char Rice : 1;
        unsigned char Sabbath : 1;
        unsigned char Convertible:1;
        unsigned char CabinOff:1;
        unsigned char TempSettingFine:1;
    } functions;
} stCabinSetsUser;

typedef struct
{
    enCabinModes Mode;
    unsigned char FF;
    unsigned char FRZ;
    unsigned char Convertible;
    struct
    {
        unsigned char Quick : 1;
        unsigned char Vacation : 1;
        unsigned char Rice : 1;
        unsigned char Sabbath : 1;
        unsigned char Economy : 1;
        unsigned char EcoExtra : 1;
        unsigned char Silent : 1;
        unsigned char Convertible;
        unsigned char CabinOff:1;
        unsigned char CompSpeedLevelNeedUpLH:1;
    } functions;
} stCabinSetsWork;

typedef struct
{
    unsigned char FF;
    unsigned char FRZ;
    struct
    {
        unsigned char QuickFF : 1;
        unsigned char QuickFRZ : 1;
        unsigned char Vacation : 1;
        unsigned char Rice : 1;
        unsigned char QuickIce : 1;
        unsigned char Convertible;
        unsigned char CabinOff:1;
    } functions;
} stCabinSetsConfigs;

typedef struct
{
    unsigned char FF[10];
    unsigned char FRZ[10];
    struct
    {
        unsigned char QuickFF;
        unsigned char QuickFRZ;
        unsigned char Vacation;
        unsigned char Rice;
        unsigned char QuickIce;
        unsigned char Convertible[10];
        unsigned char CabinOff;
    } functions;
} stCabinSetsIndexes;

typedef struct
{
    unsigned char QFRD_ValueCelcius;
    signed char QFRZ_ValueCelcius;
    signed char lookUpTable[LOOK_UP_TABLE_COUNT][2];
} stCabinSetsTemperature_Flash;

typedef struct
{
    unsigned char display;
    signed short coolCutInTemp;
    signed short coolCutOutTemp;
    signed short fanCutInTemp;
    signed short fanCutOutTemp;
    signed short lowTempHeaterDOCutInTemp;
    signed short lowTempHeaterDOCutOutTemp;
    signed short lowTempHeaterDCCutInTemp;
    signed short lowTempHeaterDCCutOutTemp;
} stCabinSetsDatas_Flash;

typedef struct
{
    unsigned char display;
    signed short coolCutInTemp;
    signed short coolCutOutTemp;
    signed short fanCutInTemp;
    signed short fanCutOutTemp;
    signed short lowTempHeaterCutInTemp;
    signed short lowTempHeaterCutOutTemp;
} stCabinSetsDatas_Ram;

typedef struct
{
    stCabinSetsConfigs configs;
    stCabinSetsIndexes indexes;
} stCabinSets_Flash;

typedef struct
{
    stCabinSetsUser user;
    stCabinSetsWork work;
    stCabinSetsDatas_Ram datas;
} stCabinSets_Ram;

#endif

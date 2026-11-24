#ifndef RF_SETS_H
#define RF_SETS_H


typedef enum{
  _SetType_Enable,
  _SetType_Disable,
  _SetType_Toggle,
  _SetType_ResetTimer,
  _SetType_ToWork,
  _SetType_Store,
}enSetTypes;

typedef enum{
  _PumpOut_Close,
  _PumpOut_OpenFF,
  _PumpOut_OpenFRZ,
  _PumpOut_OpenALL,
}enPumpOutType;

typedef struct{
  unsigned char isActive:1;
  enPumpOutType type;
}stPumpOut;

typedef struct{
  struct{
    unsigned char Vacation:1;
    unsigned char Rice:1; 
    unsigned char Economy:1;
    unsigned char Sabbath:1;
    unsigned char EcoExtra:1;
    unsigned char Silent:1;
    unsigned char Shopping:1;
    unsigned char TempSettingFine:1;
  }functions;
}stRefrgieratorSetsUser;

typedef struct{
  struct{
    unsigned char Sabbath:1;    
    unsigned char EcoExtra:1;
    unsigned char Silent:1;
   // unsigned char DoorsClosed:1;
  }functions;
}stRefrgieratorSetsWork;

typedef struct{
  struct{
    unsigned char Sabbath:1;
    unsigned char EcoExtra:1;  
    unsigned char DoorsClosed:1;
    unsigned char Silent:1;
    stPumpOut PumpOut;
  }functions;
}stRefrigeratorSetsConfigs;

typedef struct{
  stRefrigeratorSetsConfigs configs;
  unsigned char voltageInput; 
  unsigned char Model_Select_EEPROM_Record;
}stRefrigeratorSets_Flash;

typedef struct{
  stRefrgieratorSetsUser user;
  stRefrgieratorSetsWork work;
}stRefrigeratorSets_Ram;


#endif
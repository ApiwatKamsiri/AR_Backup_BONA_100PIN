#ifndef RF_CONFIGS_H
#define RF_CONFIGS_H
#include <stdint.h>
#include "rf_sets.h"
#include "rf_common.h"
#include "app/cooler/cooler_configs.h"
#include "app/cabin/cabin_configs.h"
#include "app/ice_machine/ice_machine_configs.h"
#include "app/water_source/water_source_configs.h"
#include "app/ice_source/ice_source_configs.h"
#include "app/dispenser/dispenser_configs.h"
#include "app/connected/connected_configs.h"



typedef struct
{
  /// SAK
  unI2CDevices I2CDevices;
  unsigned char fairMode : 1;
  unsigned char dispOff : 1;
  unsigned char autoDispOff : 1;
  unsigned char realTemp : 1;
  unsigned char fahrenheit : 1;
  unsigned char onBoardBuzzer : 1;
  unsigned short ecoExtraTime;
  unsigned short sabbathTime;
  unsigned short doorsClosedTime;
  unsigned short maxDoorsClosedStateStayTime;
  unsigned short doorsClosedBlockTime;
  unsigned char doorsOpenedCount;
  unsigned short doorsAlertTime[6];
  unsigned char doorsAlertLevelIncreaseTime;
  unsigned char doorsAlertingCloseLightTime;
  unsigned char displayData;
  unsigned char displayValue;
  unsigned char displayUnit;
  unsigned short silentTime; 
  unsigned short shoppingModeTime;
  unsigned short commercialDefrostOnTimerSet;   //APW : Add CT heater software
  unsigned char commercialDefrostOffTimerSet;   //APW : Add CT heater software
  unsigned char commercialHeaterOnTimerSet;     //APW : Add CT heater software
  unsigned char commercialHeaterOffTimerSet;    //APW : Add CT heater software
  unsigned char commercialAutoClearEEPROMMinTimer;  // APW : Add timer auto clear data in CT
} stRefrigeratorParameters_Flash;


typedef struct
{
    stStock Stock;
    stRefrigeratorSets_Flash Sets;
    stRefrigeratorParameters_Flash Parameters;
    stIceMachine_Flash IceMachine;
    stIceSource_Flash IceSource;
    stWaterSource_Flash WaterSource;
    stDispenser_Flash Dispenser;
    //stIceCreamMachine_Flash IceCreamMachine;
    stCooler_Flash Cooler;
    stCabin_Flash Cabin[_NumOfCabin];
    stConnectedAppliance_Flash Connected; 
} stRefrigerator_Flash;

#define __RF_CONSTS_OF(_memberPtr, _member)   __RF_OF(stRefrigerator_Flash, _memberPtr, _member)

extern const stRefrigerator_Flash refrigerator_F;
extern const stCabinSetsDatas_Flash cabinSetsDatasTable_F[];
extern const stCabinSetsTemperature_Flash fahrenheitCelciusTable;


#endif

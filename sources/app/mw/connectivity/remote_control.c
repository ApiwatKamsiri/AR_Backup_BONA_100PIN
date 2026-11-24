/** @file       remote_control.c
*  @brief      REMOTE_CONTROL application program file
*  @copyright  (c) 2020-Arcelik - All Rights Reserved
*              Permission to use, reproduce, copy, prepare derivative works,
*              modify, distribute, perform, display or sell this software and/or
*              its documentation for any purpose is prohibited without the express
*              written consent of Arcelik A.S.
*  @author     Umit Sahin
*  @date       26.06.2020
*/
#define FILE_REMOTE_CONTROL_C

#include "remote_control.h"
#if (CONNECTIVITY != _NON_CONNECTED)
#include "remote_control_driver.h"
#include "..\..\refrigerator\rf_ram.h"
#include "..\..\refrigerator\rf_configs.h"
#include "..\..\libs\temp_funcs.h"
#include "..\..\dispenser\dispenser_procedures.h"
#include "..\..\ui\ui_proc.h"
#include "..\..\ui\procedures\ui_wifi.h"
#include "..\..\ui\command_events\ce_cabin.h"
#include "..\..\ui\command_events\ce_refrigerator.h"
#include "..\..\dev\energymonitoring.h"
#include "..\..\..\dev\inverteruartcomm.h"

extern tsEnergyMonitoringParams energyMonitoringProcParams;
extern tsInverterUartCommParams inverterCommProcParams;

#define convertibleIsActive  0x80
#define shoppingModeIsActive 0x81 

blink_t WiFiLed = 
{
	.counter         = 0,
	.longLimit       = 25,
	.shortLimit      = 8,
	.state           = 0,
	.lastState       = 0,
	.wifiStateChange = 0
};

unsigned short WifiConfigTimeOut;
unsigned char resetControlTimeout; 
unsigned char startCntr;
unsigned char endofbandtest = 0;
unsigned char wifiError_B1;
unsigned char wifiError_B2;
unsigned char sendData;
unsigned char comingData;
unsigned char lastCabinFunc[4] = { 0 , 0 , 0 , 0 };
extern tsUiProcParams uiProcParams;
extern tsUiProcConsts uiProcConsts;
static unsigned short doorOpenTimer[MERCEK_CABIN_SIZE] = {0, 0};
static unsigned char doorStatus = 0;
static unsigned char fanStatus = 0;
static unsigned char heaterStatus = 0;
static enIceMachineStates iceMachStatus = _IMch_State_InitCase;
static unsigned short iceMachBoxFull_sec = 0;
static unsigned short iceMachFanOn_sec = 0;
static unsigned char icemachineTactSWErrorStatus = 0;
static unsigned char dispenserTactSWStatus = 0;
static unsigned char defrostStatus = 0;
static unsigned char connectionStatus = 0;
static unsigned short connectionTimerNB = 0;
static unsigned char svPosStatus = 0;
static unsigned char compressorStatus = 0;
static unsigned char humanSensorStatus = 0;  // V03R10: Human sensor edge detection
static unsigned long compPowerSum = 0;       // V03R18: Cumulative power for averaging
static unsigned char compPowerCount = 0;     // V03R18: Sample count for averaging
static unsigned short compDcbusMin = 0xFFFF; // V03R18: Min DC bus voltage
static unsigned short compDcbusMax = 0;      // V03R18: Max DC bus voltage
static unsigned long compRealSpeedSum = 0;   // V03R19: Cumulative real speed for averaging
static unsigned char compRealSpeedCount = 0; // V03R19: Sample count for averaging
static unsigned long energyWattMinuteSum = 0; // V03R21: Cumulative W-minutes for Wh calculation
static unsigned char byteNo = 0;
static unsigned char bitNo = 0;
static unsigned char mercekSendRetryTimer = 0;
static unsigned char mercekSendRetryCount = 0;
unsigned char iswifioff;
unsigned char TwentySeconds;
static bool SecureMercekSendTime;
unsigned char oneTimer = 0;

unsigned char LastSendF;
unsigned char PowerLossMASend;
stMercekDataTable MercekDataTable[] = {
  /*  0  */ { SENDING_REASON            , SENDING_REASON_SIZE           , EVERY_SECONDS  },
  /*  1  */ { MB_STOCK_NO               , MB_STOCK_NO_SIZE              , ONE_TIME       },
  /*  2  */ { MB_VERSION                , MB_VERSION_SIZE               , ONE_TIME       },
  /*  3  */ { MB_REVISION               , MB_REVISION_SIZE              , ONE_TIME       },
  /*  4  */ { WORKMODE                  , WORKMODE_SIZE                 , EVERY_MINUTES  },
  /*  5  */ { CABIN_TYPE_MODE           , CABIN_TYPE_MODE_SIZE          , ONE_TIME       },  // V03R05: Cabin modes
  /*  6  */ { CA0_CA1_SETS_IDX          , CA0_CA1_SETSIDX_SIZE          , EVERY_MINUTES  },
  /*  7  */ { CA0_CA1_DOOR_OPEN_COUNTER , CA0_CA1_DOOROPEN_COUNTER_SIZE , EVERY_SECONDS  },
  /*  8  */ { CA0_DOOR_OPEN_TIMER       , CAx_DOOROPEN_TIMER_SIZE       , EVERY_SECONDS  },
  /*  9  */ { CA1_DOOR_OPEN_TIMER       , CAx_DOOROPEN_TIMER_SIZE       , EVERY_SECONDS  },
  /* 10  */ { HUMAN_SENSOR_COUNTER      , HUMAN_SENSOR_COUNTER_SIZE     , EVERY_SECONDS  },  // V03R10
  /* 11  */ { FUNCTIONS                 , FUNCTIONS_SIZE                , EVERY_MINUTES  },
  /* 12  */ { FUNCTIONS_2               , FUNCTIONS_2_SIZE              , EVERY_MINUTES  },  // V03R07
  /* 13  */ { FUNCTIONS_3               , FUNCTIONS_3_SIZE              , EVERY_MINUTES  },  // V03R11
  /* 14  */ { ICE_MACHINE               , ICE_MACHINE_SIZE              , EVERY_SECONDS  },
  /* 15  */ { ICE_MACHINE_TEMP          , ICE_MACHINE_TEMP_SIZE         , EVERY_MINUTES  },
  /* 16  */ { ICE_MACHINE_WATER_HEATER  , ICE_MACHINE_WATER_HEATER_SIZE , EVERY_MINUTES  },  // V03R08
  /* 17  */ { DISPENSER                 , DISPENSER_SIZE                , EVERY_SECONDS  },
  /* 18  */ { CA0_FAN                   , CAx_FAN_SIZE                  , EVERY_MINUTES  },
  /* 19  */ { CA0_HEATERA               , CAx_HEATERA_SIZE              , EVERY_MINUTES  },
  /* 20  */ { CA0_HEATERB               , CAx_HEATERB_SIZE              , EVERY_MINUTES  },
  /* 21  */ { CA0_HEATERC               , CAx_HEATERC_SIZE              , EVERY_MINUTES  },
  /* 22  */ { CA1_FAN                   , CAx_FAN_SIZE                  , EVERY_MINUTES  },
  /* 23  */ { CA1_HEATERA               , CAx_HEATERA_SIZE              , EVERY_MINUTES  },
  /* 24  */ { CA1_HEATERB               , CAx_HEATERB_SIZE              , EVERY_MINUTES  },
  /* 25  */ { CA1_HEATERC               , CAx_HEATERC_SIZE              , EVERY_MINUTES  },
  /* 26  */ { VACUUM_HEATER             , VACUUM_HEATER_SIZE            , EVERY_MINUTES  },  // V03R12
  /* 27  */ { COOLER_TYPE               , COOLER_TYPE_SIZE              , EVERY_MINUTES  },  // V03R04: Cooler type bitmap
  /* 28  */ { SELENOID_VALVE            , SELENOID_VALVE_SIZE           , EVERY_MINUTES  },
  /* 29  */ { SELENOID_VALVE_DETAIL     , SELENOID_VALVE_DETAIL_SIZE    , EVERY_MINUTES  },
  /* 30  */ { ENERGY_MONITOR            , ENERGY_MONITOR_SIZE           , EVERY_MINUTES  },  // V03R09
  /* 31  */ { COMPRESSOR                , COMPRESSOR_SIZE               , EVERY_MINUTES  },
  /* 32  */ { COMPRESSOR_DETAIL         , COMPRESSOR_DETAIL_SIZE        , EVERY_MINUTES  },
  /* 33  */ { COMP_STOCK_NO             , COMP_STOCK_NO_SIZE            , ONE_TIME       },  // V03R13
  /* 34  */ { COMP_VERSION              , COMP_VERSION_SIZE             , ONE_TIME       },  // V03R14
  /* 35  */ { COMP_REVISION             , COMP_REVISION_SIZE            , ONE_TIME       },  // V03R14
  /* 36  */ { COMP_CORE_VERSION         , COMP_CORE_VERSION_SIZE        , ONE_TIME       },  // V03R22
  /* 37  */ { COMP_CORE_REVISION        , COMP_CORE_REVISION_SIZE       , ONE_TIME       },  // V03R22
  /* 38  */ { COMP_TYPE                 , COMP_TYPE_SIZE                , EVERY_MINUTES  },  // V03R15
  /* 39  */ { COMP_ERROR                , COMP_ERROR_SIZE               , EVERY_MINUTES  },  // V03R20
  /* 40  */ { COMP_POWER                , COMP_POWER_SIZE               , EVERY_MINUTES  },  // V03R18
  /* 41  */ { COMP_DCBUS                , COMP_DCBUS_SIZE               , EVERY_MINUTES  },  // V03R18
  /* 42  */ { COMP_REALSPEED            , COMP_REALSPEED_SIZE           , EVERY_MINUTES  },  // V03R19
  /* 43  */ { CA0_DEFROST_DURATION      , CAx_DEFROST_DURATION_SIZE     , EVERY_MINUTES  },
  /* 44  */ { CA0_DEFROST_REASON        , CAx_DEFROST_REASON_SIZE       , EVERY_MINUTES  },
  /* 45  */ { CA1_DEFROST_DURATION      , CAx_DEFROST_DURATION_SIZE     , EVERY_MINUTES  },
  /* 46  */ { CA1_DEFROST_REASON        , CAx_DEFROST_REASON_SIZE       , EVERY_MINUTES  },
  /* 47  */ { CA0_TEMPERATURE           , CAx_TEMPERATURE_SIZE          , EVERY_MINUTES  },
  /* 48  */ { CA1_TEMPERATURE           , CAx_TEMPERATURE_SIZE          , EVERY_MINUTES  },
  /* 49  */ { AMBIENT                   , AMBIENT_SIZE                  , EVERY_MINUTES  },
  /* 50  */ { CONNECTION                , CONNECTION_SIZE               , EVERY_SECONDS  },
  /* 51  */ { ERROR_CODE                , ERROR_CODE_SIZE               , EVERY_SECONDS  },
  /* 52  */ { DATA_COUNT                , DATA_COUNT_SIZE               , EVERY_MINUTES  },
  /* 53  */ { MERCEK_COUNT              , MERCEK_COUNT_SIZE             , EVERY_MINUTES  },
  /* 54  */ { EOF_MERCEK_DATA           ,  0                            , ONE_TIME       },
};


//#if (CONNECTIVITY != _NON_CONNECTED)
static tsTimerGeneric remoteControlResetTimer = TIMER_GENERIC_INIT();

uint8_t _Pumptimer;
uint16_t _MaxSpinRpm;
uint8_t Is_Delay_Read;
uint8_t heaterIsOpened;

#define SEND_MERCEK_COMMAND (50)
#define SEND_SECURE_MERCEK_COMMAND (55)

typedef void (*ptr_Func_Load)(uint8_t, uint8_t);
typedef void (*ptr_Func_Init)(void);
typedef void (*ptr_Func_Start)(void);
typedef void (*ptr_Func_Pause)(void);
typedef void (*ptr_Func_On)(void);
typedef void (*ptr_Func_Off)(void);

/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES 
********************************************************************************/

const uint8_t WiFiMachineStatesTop[] = {
  255,  //UI_TOP_STATE_ENTRY,
  20,	  //UI_OFF_MODE,
  20,	  //UI_STANDBY_MODE,
  50,	  //UI_ACTIVE_MODE,
  255,  //UI_TOP_STATE_EXIT,
  255}; //UI_TOP_STATE_BOUNDARY

const uint8_t WiFiMachineStatesActiveMode[] = {
  10,	  //UI_AM_NORMAL_MODE_ENTRY, // 0
  10,	  //UI_AM_NORMAL_MODE_SELECTION_MODE, // 1
  30,	  //UI_AM_NORMAL_MODE_RUN_MODE, // 2
  40,	  //UI_AM_NORMAL_MODE_PAUSE_MODE, // 3
  10,	  //UI_AM_NORMAL_MODE_STOP_MODE, // 4
  60,	  //UI_AM_NORMAL_MODE_TIME_DELAY_MODE, // 5
  10,	  //UI_AM_NORMAL_MODE_WASHING_INFO_MODE, // 6
  10,	  //UI_AM_NORMAL_MODE_PRV_WASHINFO_MODE, // 7
  70,	  //UI_AM_NORMAL_MODE_SETTINGS_MODE, // 8
  120,  //UI_AM_NORMAL_MODE_DEMO_MODE, // 9
  128,  //UI_AM_NORMAL_MODE_AUTO_AEA_MODE, // 10
  128,  //UI_AM_NORMAL_MODE_EXIT, // 11
  128}; //UI_AM_NORMAL_MODE_BOUNDARY // 12

const uint8_t WiFiMachineStatesDemoMode[] = {
  10,	  // UI_AM_NM_DEMO_MODE_ENTRY, // 0
  10,	  // UI_AM_NM_DEMO_MODE_ACTIVE, // 1
  10,	  // UI_AM_NM_DEMO_MODE_SWITCH_PROGRAMS, // 2
  30,	  // UI_AM_NM_DEMO_MODE_RUN_ACTIVE, // 3
  10,	  // UI_AM_NM_DEMO_MODE_STOP, // 4
  40,	  // UI_AM_NM_DEMO_MODE_PAUSE, // 5
  128,  // UI_AM_NM_DEMO_MODE_EXIT, // 6
  128}; // UI_AM_NM_DEMO_MODE_BOUNDARY, // 7

unsigned char resetControlTimeout; 
unsigned char startCntr;

void Connectivity_Reset(void)
{
  // factory reset algorithm
  if ((refrigerator_F.Connected.funcConnectedApp.connectedAppAct == true) &&
      /* (*wifiArray[wifiStateIndex] == WIFI_CONNECTED) && */
      (WiFiVariables.ResetControl == 0))
  {
    refrigerator_R.ui.wifi.resetStatus = _en_reset_send_1;
    resetControlTimeout = WIFI_RESET_TIMEOUT;
    startCntr = 0;
  }
}

void setWifiState(unsigned char state)
{
  wifiState = (wifi_states_t)(state);
  switch (wifiState)
  {    
  case WIFI_OFF:   
    refrigerator_R.E2.WifiStatus = WIFI_OFF;
    break;
    
  case WIFI_ON:
    if(!WiFiVariables.TestMode)
      refrigerator_R.E2.WifiStatus = WIFI_ON; 
    break;    
  case WIFI_AP_ON:
    if(!WiFiVariables.TestMode)
      refrigerator_R.E2.WifiStatus = WIFI_OFF; 
    break;    
  default:
    ;
    break;
  }
}

//-------------------------------------------------
void toggleWifiState(wifi_states_t state)
{
  //  if(serviceButton.state != _IDLE)
  //  {
  //    sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
  //    return;
  //  }
  //if (!resetControlTimeout)
  // {
  switch (state)
  {
  case WIFI_CONFIG_TOGGLE:
    switch (WiFiVariables.Situation)
    {
    case WIFI_DROPPED:
    case WIFI_OFF:
    case WIFI_NOT_CONNECTED:
    case WIFI_ON:
    case WIFI_CONNECTED:
    case WIFI_CONNECTING:
      setWifiState(WIFI_AP_ON);        
      rcRequest(eRcRequestStartOnboarding, FALSE);
      sysBuzzerCmd(eSysBuzzerPlayOpenMelody);
      break;
      
    case WIFI_AP_ON:
    case WIFI_PW_SENDING:
      setWifiState(WIFI_OFF);
      rcRequest(eRcRequestCloseConnection, FALSE);
      sysBuzzerCmd(eSysBuzzerPlayEndMelody);
      break;
    }
    break; // WIFI_CONFIG_TOGGLE'a ait
    
  case WIFI_TOGGLE:
    switch (WiFiVariables.Situation)
    {
    case WIFI_DROPPED:
    case WIFI_OFF:
      setWifiState(WIFI_ON);
      rcRequest(eRcRequestCheckConnection, TRUE);
      sysBuzzerCmd(eSysBuzzerPlayOpenMelody);
      break;
    case WIFI_NOT_CONNECTED:
    case WIFI_ON:
    case WIFI_CONNECTED:
    case WIFI_CONNECTING:
    case WIFI_AP_ON:
    case WIFI_PW_SENDING:
      setWifiState(WIFI_OFF);
      rcRequest(eRcRequestCloseConnection, FALSE);
      sysBuzzerCmd(eSysBuzzerPlayEndMelody);
      break;
    }
    break;
  }
  // }
}

void changeRice(bool val)
{
	unsigned char i;
	for (i = 0; i < _NumOfCabin; i++)
	{
		refrigerator_R.Cabin[i].sets.user.functions.Rice = val;
		if (val)
			refrigerator_R.Cabin[i].sets.user.functions.Quick = 0;
		refrigerator_R.Cabin[i].E2.Rice = val;
	}
}
//-------------------------------------------------
void changeVacation(bool val)
{
	if (refrigerator_R.Values.errors.sensorErrorsNumber)
	{
		refrigerator_R.Sets.user.functions.Vacation = false;
		sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
	}
	else
	{
		refrigerator_R.Sets.user.functions.Vacation = val;
	}
  
	for (unsigned char i = 0; i < _NumOfCabin; i++)
	{
		CabinVacation(&uiProcConsts, &uiProcParams, &refrigerator_F.Cabin[i],
                  &refrigerator_R.Cabin[i],
                  refrigerator_R.Sets.user.functions.Vacation);
	}
	uiResetSetChangeTimer(&refrigerator_R.ui);
}

void setCabinTemp(unsigned char ind, unsigned char data)
{
	unsigned char ucTemp;
	//unsigned char ucTemp1;
  
  if (ind >= _NumOfCabin)
    return;
  
	switch (refrigerator_R.Cabin[ind].sets.user.Mode)
	{
  case _CabinMode_FF:
    ucTemp = (data) % 64;
    
    if (refrigerator_R.Values.tempType == _TempType_Celcius)
    {
      if (ucTemp >= refrigerator_F.Cabin[ind].sets.configs.FF)
        return;
    }
    
    else
    {
      if (ucTemp >= 14)
        return;
    }
    
    refrigerator_R.Cabin[ind].sets.user.functions.Quick = false;
    refrigerator_R.Cabin[ind].sets.user.functions.Vacation = false;
    refrigerator_R.Cabin[ind].sets.user.functions.Rice = false;
    
    if (refrigerator_R.Values.tempType == _TempType_Celcius)
    {
      refrigerator_R.Cabin[ind].sets.user.FF = ucTemp;
      refrigerator_R.Values.temperatureConvPointer[ind] = getCurrentPointerC(cabinSetsDatasTable_F[refrigerator_F.Cabin[ind].sets.indexes.FF[refrigerator_R.Cabin[ind].sets.user.FF]].display);
    }
    else if (refrigerator_R.Values.tempType == _TempType_Fahrenheit)
    {
      refrigerator_R.Values.temperatureConvPointer[ind] = ucTemp;
      refrigerator_R.Cabin[ind].sets.user.FF = getSetValueFromPointer(&refrigerator_F, &refrigerator_R, ind);
    }
		break;
    
  case _CabinMode_FRZ:
    
    ucTemp = (data) % 64;
    if(refrigerator_R.Cabin[1].sets.user.functions.Convertible)
    {
      if (refrigerator_R.Values.tempType == _TempType_Celcius)
      {
        if (ucTemp >= refrigerator_F.Cabin[ind].sets.configs.functions.Convertible)
          return;
      }
      else
      {
        if (ucTemp < 14 || ucTemp >= 32)
          return;
      }
      
      if (refrigerator_R.Values.tempType == _TempType_Celcius)
      {
        refrigerator_R.Cabin[ind].sets.user.Convertible = ucTemp;
        refrigerator_R.Values.temperatureConvPointer[ind] = getCurrentPointerC(cabinSetsDatasTable_F[refrigerator_F.Cabin[ind].sets.indexes.functions.Convertible[refrigerator_R.Cabin[ind].sets.user.Convertible]].display);
      }
      else if (refrigerator_R.Values.tempType == _TempType_Fahrenheit)
      {
        refrigerator_R.Values.temperatureConvPointer[ind] = ucTemp; 
        refrigerator_R.Cabin[ind].sets.user.Convertible = getSetValueFromPointer(&refrigerator_F, &refrigerator_R, ind);
      }         
    }
  else if(refrigerator_R.Cabin[1].sets.user.functions.CabinOff)
    {
      /*if (refrigerator_R.Values.tempType == _TempType_Celcius)
      {
      if (ucTemp >= refrigerator_F.Cabin[ind].sets.configs.functions.CabinOff)
      return;
    }
                    else
      {
      if (ucTemp < 14 || ucTemp >= 31)
      return;
    }
      
			if (refrigerator_R.Values.tempType == _TempType_Celcius)
			{
      refrigerator_R.Cabin[ind].sets.user.FRZ = ucTemp;
      refrigerator_R.Values.temperatureConvPointer[ind] = getCurrentPointerC(cabinSetsDatasTable_F[refrigerator_F.Cabin[cabinIndex].sets.indexes.functions.Convertible[refrigerator_R.Cabin[cabinIndex].sets.user.Convertible]].display);
    }
                    else if (refrigerator_R.Values.tempType == _TempType_Fahrenheit)
      {
      refrigerator_R.Values.temperatureConvPointer[ind] = ucTemp; 
      refrigerator_R.Cabin[ind].sets.user.FRZ = getSetValueFromPointer(ind);
    } */  
    }
    else
    {
      if (refrigerator_R.Values.tempType == _TempType_Celcius)
      {
        if (ucTemp >= refrigerator_F.Cabin[ind].sets.configs.FRZ)
          return;
      }
      
      else
      {
        if (ucTemp < 14 || ucTemp >= 32)
          return;
      }
      
      if (refrigerator_R.Values.tempType == _TempType_Celcius)
      {
        refrigerator_R.Cabin[ind].sets.user.FRZ = ucTemp;
        refrigerator_R.Values.temperatureConvPointer[ind] = getCurrentPointerC(cabinSetsDatasTable_F[refrigerator_F.Cabin[ind].sets.indexes.FRZ[refrigerator_R.Cabin[ind].sets.user.FRZ]].display);
      }
      else if (refrigerator_R.Values.tempType == _TempType_Fahrenheit)
      {
        refrigerator_R.Values.temperatureConvPointer[ind] = ucTemp; 
        refrigerator_R.Cabin[ind].sets.user.FRZ = getSetValueFromPointer(&refrigerator_F, &refrigerator_R, ind);
      }
    }

    refrigerator_R.Cabin[ind].sets.user.functions.Quick = false;
    refrigerator_R.Cabin[ind].sets.user.functions.Vacation = false;
    refrigerator_R.Cabin[ind].sets.user.functions.Rice = false;
    
		break;
	}
  
	refrigerator_R.Cabin[ind].E2.FRZ = refrigerator_R.Cabin[ind].sets.user.FRZ;
	refrigerator_R.Cabin[ind].E2.FF = refrigerator_R.Cabin[ind].sets.user.FF;
	refrigerator_R.E2.temperatureConvPointer[ind] = refrigerator_R.Values.temperatureConvPointer[ind];
	refrigerator_R.Cabin[ind].E2.Rice = refrigerator_R.Cabin[ind].sets.user.functions.Rice;
	refrigerator_R.Cabin[ind].E2.Vacation = refrigerator_R.Cabin[ind].sets.user.functions.Vacation;
}


void setCabinFunc(unsigned char ind, unsigned char data)
{
	bool quick = data & 0x01;
	unsigned char mode = (data & 0x02) / 2 + 1;
  
  if (ind >= _NumOfCabin)
    return;
	
	switch (refrigerator_F.Cabin[ind].type)
	{
  case _CabinType_FLAP:
    if (mode != refrigerator_R.Cabin[ind].sets.user.Mode)
    {
      refrigerator_R.Cabin[ind].sets.user.Mode = (enCabinModes)mode;
    }
    break;
#ifdef JKRavailable		
  case _CabinType_JKR:

    if (mode != refrigerator_R.Cabin[ind].sets.user.Mode)
    {
      refrigerator_R.Cabin[ind].sets.user.Mode = (enCabinModes)mode;
    }
    
		break;
#endif    
  default:
		break;
	}
	
	if (refrigerator_F.Cabin[ind].sets.configs.functions.Vacation && (quick || mode))
		changeVacation(0);
	if (refrigerator_F.Cabin[ind].sets.configs.functions.Rice && (quick || mode))
		changeRice(0);
	if (refrigerator_F.Cabin[ind].sets.configs.functions.QuickFF)
		refrigerator_R.Cabin[ind].sets.user.functions.Quick = quick;
	if (refrigerator_F.Cabin[ind].sets.configs.functions.QuickFRZ &&
      !refrigerator_R.Cabin[ind].sets.user.functions.Convertible &&
        !refrigerator_R.Cabin[ind].sets.user.functions.CabinOff)
		refrigerator_R.Cabin[ind].sets.user.functions.Quick = quick;
}

unsigned char getCabinFunc(unsigned char index)
{
  if (index >= _NumOfCabin)
    return 0;
  
	switch (refrigerator_F.Cabin[index].type)
	{
  case _CabinType_FLAP:
    return refrigerator_R.Cabin[index].sets.user.functions.Quick + 2 * ((unsigned char)refrigerator_R.Cabin[index].sets.user.Mode) - 2; 
#ifdef JKRavailable
  case _CabinType_JKR:
    return refrigerator_R.Cabin[index].sets.user.functions.Quick + 2 * ((unsigned char)refrigerator_R.Cabin[index].sets.user.Mode) - 2;
#endif   
    default:
    return refrigerator_R.Cabin[index].sets.user.functions.Quick;
	}
}

unsigned char getCabinTemp(unsigned char index)
{
  unsigned char result = 0;
  
  if (index < _NumOfCabin)
  {
    switch (refrigerator_R.Cabin[index].sets.user.Mode)
    {
    case _CabinMode_FF:
      if (refrigerator_R.Cabin[index].sets.user.functions.Quick)
#ifdef ALPS_H10H30
        result = 14 + refrigerator_R.Values.tempType * 64;
#else
        result = 6 + refrigerator_R.Values.tempType * 64;
#endif
      else if (refrigerator_R.Cabin[index].sets.user.functions.Vacation)
        result = 0x7F;
      else
      {
        if (refrigerator_R.Values.tempType == _TempType_Celcius)
        {
          result = refrigerator_R.Cabin[index].sets.user.FF + refrigerator_R.Values.tempType * 64;
#ifdef ALPS_H10H30
          if( refrigerator_R.Cabin[0].sets.user.FF == 6)  //15 selected as base , cause of setCabin max value can be 14
            result = 15 + refrigerator_R.Cabin[0].sets.user.FF; //15+6 = 21 --> 3
          if( refrigerator_R.Cabin[0].sets.user.FF == 1)
            result = 15 + refrigerator_R.Cabin[0].sets.user.FF; //15+1 = 16 --> 5
          if( refrigerator_R.Cabin[0].sets.user.FF == 3)       
            result = 15 + refrigerator_R.Cabin[0].sets.user.FF; //15+3 = 18 --> 7
#endif          
        }
        else if (refrigerator_R.Values.tempType == _TempType_Fahrenheit)
        {
          result = refrigerator_R.Values.temperatureConvPointer[index] + refrigerator_R.Values.tempType * 64;
        }
      }
      break;
      
    case _CabinMode_FRZ:
      if (refrigerator_R.Cabin[index].sets.user.functions.Quick)
        result = 27 + refrigerator_R.Values.tempType * 64 + refrigerator_R.Values.tempType * 3; //we added new fahrenheit values thats why we add 3
      else if (refrigerator_R.Cabin[index].sets.user.functions.Vacation)
        result = 0x7F;
      else
      {
        if (refrigerator_R.Values.tempType == _TempType_Celcius)
        {
          if(refrigerator_R.Cabin[index].sets.user.functions.Convertible)
            result = refrigerator_R.Cabin[index].sets.user.Convertible + refrigerator_R.Values.tempType * 64;
          else if(refrigerator_R.Cabin[index].sets.user.functions.CabinOff)
            result = 0x7F; // burasi sonradan belki d?zenlenebilir.
          else
            result = refrigerator_R.Cabin[index].sets.user.FRZ + refrigerator_R.Values.tempType * 64;
#ifdef ALPS_H10H30
          if( refrigerator_R.Cabin[1].sets.user.FRZ == 3) //33 selected as base , cause of setCabin max value can be 32
            result = 33 + refrigerator_R.Cabin[1].sets.user.FRZ; //33+3 = 36 --> -19
          if( refrigerator_R.Cabin[1].sets.user.FRZ == 1)
            result = 33 + refrigerator_R.Cabin[1].sets.user.FRZ; //33+1 = 34 --> -21
          if( refrigerator_R.Cabin[1].sets.user.FRZ == 6)       
            result = 33 + refrigerator_R.Cabin[1].sets.user.FRZ; //33+6 = 39 --> -23
#endif
        }
        else if (refrigerator_R.Values.tempType == _TempType_Fahrenheit)
        {
          result = refrigerator_R.Values.temperatureConvPointer[index] + refrigerator_R.Values.tempType * 64;
          if(refrigerator_R.Cabin[index].sets.user.functions.CabinOff)
            result = 0x7F;
        }
      }
      break;
      
    default:
      result = refrigerator_R.Cabin[index].sets.user.FF + refrigerator_R.Cabin[index].sets.user.FRZ + 1; // ???
      break;
    }
  }
  
  return result;
}

void changeFRZCabinMode(unsigned char val)
{
  if (refrigerator_R.Cabin[1].sets.user.functions.Convertible) //ff
  {
    if(val == 0x00)
      ceCabinConvertible(&uiProcConsts, &uiProcParams, &refrigerator_F.Cabin[1], &refrigerator_R.Cabin[1], true);
    if(val == 0x02)
      ceCabinFRZOff(&uiProcConsts, &uiProcParams, &refrigerator_F.Cabin[1], &refrigerator_R.Cabin[1], true);  
  }
  else if(refrigerator_R.Cabin[1].sets.user.functions.CabinOff) //off
  {
    if(val == 0x00)
      ceCabinFRZOff(&uiProcConsts, &uiProcParams, &refrigerator_F.Cabin[1], &refrigerator_R.Cabin[1], true);  
    if(val== 0x01)
      ceCabinConvertible(&uiProcConsts, &uiProcParams, &refrigerator_F.Cabin[1], &refrigerator_R.Cabin[1], true);
  }
  else //frz
  {
    if(val == 0x01)
      ceCabinConvertible(&uiProcConsts, &uiProcParams, &refrigerator_F.Cabin[1], &refrigerator_R.Cabin[1], true);
    if(val == 0x02)
      ceCabinFRZOff(&uiProcConsts, &uiProcParams, &refrigerator_F.Cabin[1], &refrigerator_R.Cabin[1], true);  
  }
}

void changeSabbath(bool val)
{
	if (val)
	{
		refrigerator_R.Sets.user.functions.Sabbath = true;
		refrigerator_R.Values.sabbathTimer = refrigerator_F.Parameters.sabbathTime;
	}
	else
	{
		refrigerator_R.Sets.user.functions.Sabbath = false;
		refrigerator_R.Values.sabbathTimer = 0;
	}
	refrigerator_R.E2.Sabbath = refrigerator_R.Sets.user.functions.Sabbath;
	refrigerator_R.E2.sabbathTimer = refrigerator_R.Values.sabbathTimer;
}

void ecoExtra(bool val)
{
	if (val)
	{
		refrigerator_R.Sets.user.functions.EcoExtra = true;
		refrigerator_R.Values.ecoExtraTimer = refrigerator_F.Parameters.ecoExtraTime;
	}
	else
	{
		refrigerator_R.Sets.user.functions.EcoExtra = false;
		refrigerator_R.Values.ecoExtraTimer = 0;
	}
	refrigerator_R.E2.EcoExtra = refrigerator_R.Sets.user.functions.EcoExtra;
}
unsigned char fHandleSetServiceButton(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetStockCode(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetMachineID(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetServiceButtonShortTimeOut(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetServiceButtonLongTimeOut(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetMeasurementInformation(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetPassword(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetNBVersion(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetNBRevision(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetWGVersion(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDateMinute(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDateHour(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDateDay(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDateMonth(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDateYear(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetRemoteStart(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetMercekSize(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetArraySize(unsigned char rcCommand)
{
  return rcCommand;
}  
unsigned char fHandleSetSituation(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetNBErrors0(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetMercekErrorLenght(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetTestMode(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetEndMesg(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetResetControl(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetAux2(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetOTA(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetCabinTempType(unsigned char rcCommand)
{
  if(rcCommand < 2)
  {
    if(refrigerator_R.Values.tempType != (enTempType)(rcCommand))
    {
      refrigerator_R.Values.tempType = (enTempType)(rcCommand);
      refrigerator_R.E2.tempType = (enTempType)(rcCommand);
      refrigerator_R.ui.states.normal.state = _UI_NormalState_TempUnit_Init;
      refrigerator_R.ui.states.distributer.state = _UI_DistributerState_TempUnit_Init;
    }
  }
  return rcCommand;
}

unsigned char fHandleSetCabinTempSet(unsigned char rcCommand, unsigned char index)
{
  setCabinTemp(index,rcCommand);
  if(index<2)
  {
    if(refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
       refrigerator_R.ui.timers.dspOffTimer != 60)	
      refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;
  }
  return rcCommand;
}

unsigned char fHandleSetCabinFunc(unsigned char rcCommand, unsigned char index)
{
  setCabinFunc(index,rcCommand);
  if(index<2)
  {
    if(refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
       refrigerator_R.ui.timers.dspOffTimer != 60)	
      refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;      
  }
  return rcCommand;
}
unsigned char fHandleSetIceMachineSet(unsigned char rcCommand)
{
  if(refrigerator_F.IceMachine.type)
  {
    if (rcCommand != WiFiVariables.IceMachineSet)
    {

          refrigerator_R.IceMachine.sets.iceOFF = !rcCommand;
          refrigerator_R.IceMachine.E2.iceOFF = !rcCommand;
      
      if (refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
          refrigerator_R.ui.timers.dspOffTimer != 60)
        refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;
      
      if(refrigerator_R.IceMachine.sets.iceOFF)
      {
        refrigerator_R.IceMachine.values.QuickIce60 = 0;
        refrigerator_R.IceMachine.E2.QuickIce60 = 0;
      }
    }
  }
  return rcCommand;
  
}
unsigned char fHandleSetDispenser(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetIceSource(unsigned char rcCommand)
{
  switch(rcCommand)
  {
  case _DispenserState_OFF:
    refrigerator_R.Dispenser.sets.state = (enDispenserStates) _DispenserState_OFF;
    break;
    
  case _DispenserState_IceCube:
    refrigerator_R.Dispenser.sets.state = (enDispenserStates) _DispenserState_IceCube;
    break;
    
  case _DispenserState_IceCrash:
    refrigerator_R.Dispenser.sets.state = (enDispenserStates) _DispenserState_IceCrash;
    break;
    
  default:
    refrigerator_R.Dispenser.sets.state = (enDispenserStates)(rcCommand & 0x07);
    break;
  }
  DispenserCheckState( &refrigerator_F.Dispenser, &refrigerator_R.Dispenser);
  refrigerator_R.Dispenser.E2.state = refrigerator_R.Dispenser.sets.state;
  return rcCommand;
}
unsigned char fHandleSetServiceButtonCopy(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetIceMachine(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDisplaySet(unsigned char rcCommand)
{
  if( refrigerator_F.Parameters.dispOff )
  {
    refrigerator_R.Values.dispOff = (rcCommand & 0x01);
    if( refrigerator_R.Values.dispOff )
    {
      refrigerator_R.ui.timers.dspOffTimer = 0;
      refrigerator_R.ui.states.normal.state = _UI_NormalState_DisplayOff_Init; 
    }
    else
    {
      uiInitStates(&refrigerator_R.ui);
    }
    refrigerator_R.E2.dispOff = refrigerator_R.Values.dispOff;
    uiResetSetChangeTimer(&refrigerator_R.ui);
  }
  return rcCommand;
}
unsigned char fHandleSetVacation(unsigned char rcCommand)
{
  if(rcCommand != WiFiVariables.Vacation)
  {
    changeVacation(rcCommand & 0x01);
    if(refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
       refrigerator_R.ui.timers.dspOffTimer != 60)
      refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;	
  }
  return rcCommand;
}
unsigned char fHandleSetEcoFuzzy(unsigned char rcCommand)
{
  if(rcCommand != WiFiVariables.EcoFuzzy)
  {   
    ecoExtra(rcCommand & 0x01);
    if(refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
       refrigerator_R.ui.timers.dspOffTimer != 60)
      refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;	
  }
  return rcCommand;
}
unsigned char fHandleSetSabbath(unsigned char rcCommand)
{
  if(rcCommand != WiFiVariables.Sabbath)
  {
    changeSabbath(rcCommand & 0x01);
  }
  return rcCommand;
}
unsigned char fHandleSetConnectedFeatures(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetVioled(unsigned char rcCommand)
{
  if(refrigerator_F.Cabin[0].components.violed.type && refrigerator_F.Cabin[0].components.violed.activateOnDisplay)
  {
    if (rcCommand != WiFiVariables.Violed)
    {
      if (rcCommand)
      {
        refrigerator_R.Cabin[0].values.violed.state = _VioLedState_Active;
        sysBuzzerCmd(eSysBuzzerPlayOpenMelody);
      }
      else
      {
        refrigerator_R.Cabin[0].values.violed.state = _VioLedState_Deactive;
        sysBuzzerCmd(eSysBuzzerPlayEndMelody);
      }
      if (refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
          refrigerator_R.ui.timers.dspOffTimer != 60)
        refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;
    }
  }
  return rcCommand;
}
unsigned char fHandleSetWarnings(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetError(unsigned char rcCommand)
{
  if(rcCommand != WiFiVariables.Error)
  {
    refrigerator_R.Values.doorsClosed = rcCommand & 0x01;	
  }
  return rcCommand;
}
unsigned char fHandleSetFilter(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetAlarmLevel(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetRefrigeratorAux3(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetRefrigeratorAux4(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetComponents1(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetComponents2(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDailyHour(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDailyMin(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetSpecialTimes_1(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetShoppingMode(unsigned char rcCommand)
{
  if(rcCommand != WiFiVariables.ShoppingMode)
  {
    refrigerator_R.Values.shoppingModeTimer = refrigerator_F.Parameters.shoppingModeTime;
    
    if( rcCommand)
    {
      lastCabinFunc[0] = getCabinFunc(0);
      lastCabinFunc[1] = getCabinFunc(1);
      WiFiVariables.CabinFunc[0] = shoppingModeIsActive;
      setCabinFunc(0, rcCommand);
      WiFiVariables.CabinFunc[1] = shoppingModeIsActive;
      setCabinFunc(1, rcCommand);
      if(refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
         refrigerator_R.ui.timers.dspOffTimer != 60)
        refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;
    }
    else 
    {
      setCabinFunc(0,lastCabinFunc[0]);  
      setCabinFunc(1,lastCabinFunc[1]);  
    }
  }
  return rcCommand;
}
unsigned char fHandleSetSpecialTimes_3(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetSpecialFunctions(unsigned char rcCommand)
{
  if(rcCommand != WiFiVariables.SpecialFunctions)
  {
    if(rcCommand < 0x03)
    {
      changeFRZCabinMode(rcCommand);
    }
  }
  return rcCommand;
}
unsigned char fHandleSetHandleLight(unsigned char rcCommand)
{
#ifdef HANDLELIGHT
  if(rcCommand ! = WiFiVariables.HandleLight)
  {
    if(rcCommand)
      handleLightState = 1;
    else
      handleLightState = 0;
  }
#endif
  return rcCommand;
}
unsigned char fHandleSetDoorAlarmTime(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetAIMode(unsigned char rcCommand)
{ 
#if defined (AI_COOL_V1)           
  if(rcCommand != WiFiVariables.AIMode)
  {
    if(!refrigerator_R.Cabin[1].sets.user.functions.Convertible && !refrigerator_R.Cabin[1].sets.user.functions.CabinOff
       && !refrigerator_R.Cabin[1].sets.user.functions.Quick)
    {
      refrigerator_R.Cabin[0].ai.isEnabled = rcCommand;
      refrigerator_R.Cabin[1].ai.isEnabled = rcCommand;
    }
    else
    {
      sysBuzzerCmd( eSysBuzzerPlayWarningMelody); 
    }
  }               
#endif
  
#if defined (AI_COOL_V1_5)
    if(!refrigerator_R.Cabin[1].sets.user.functions.Convertible && !refrigerator_R.Cabin[1].sets.user.functions.CabinOff
       && !refrigerator_R.Cabin[1].sets.user.functions.Quick)
    {
      refrigerator_R.AI.isEnabled = rcCommand;
    }
    else
    {
      sysBuzzerCmd( eSysBuzzerPlayWarningMelody); 
    }                    
#endif
  return rcCommand;
}
unsigned char fHandleSetCheapSelectMin_3(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetCheapSelectDuration_3(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetCheapSelectFlag(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetCheapSelectConfig(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetCheapSelectCountDown(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDailyDay(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDailyMonth(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetDailyYear(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSethTE_HighestTempRead(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSethTE_WarningReason(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSeteE_EnergyEfficiencyDutyFF(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetMercekErrorShadow1(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetMercekErrorShadow2(unsigned char rcCommand)
{
  return rcCommand;
}
unsigned char fHandleSetQuickIce60(unsigned char rcCommand)
{
  
  if(refrigerator_F.IceMachine.type)
  {
    if (rcCommand != WiFiVariables.QuickIce60)
    {
      refrigerator_R.IceMachine.values.QuickIce60 = rcCommand;
      refrigerator_R.IceMachine.E2.QuickIce60 = rcCommand;
      
      if(refrigerator_R.IceMachine.values.QuickIce60)
      {
        refrigerator_R.IceMachine.values.quickIce60MaximumTimer = refrigerator_R.IceMachine.sets.quickIce60MaximumTime;
        refrigerator_R.IceMachine.values.quickIce60MaximumTimesCount = refrigerator_R.IceMachine.sets.quickIce60MaximumTimes * 2;
      }        
      else
      {
        refrigerator_R.IceMachine.values.quickIce60MaximumTimer = 0;
        refrigerator_R.IceMachine.values.quickIce60MaximumTimesCount = 0;
      }
      
      if (refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
          refrigerator_R.ui.timers.dspOffTimer != 60)
        refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;
    }
  }
  return rcCommand;
}
unsigned char fHandleSetRGBHandleLight(unsigned char rcCommand)
{
  if(refrigerator_F.Cabin[0].components.handleLight.type || refrigerator_F.Cabin[1].components.handleLight.type)
  {
    if (rcCommand)
      refrigerator_R.Values.handleLightState = 1;
    else
      refrigerator_R.Values.handleLightState = 0;
    refrigerator_R.E2.handleLightState = refrigerator_R.Values.handleLightState;
    if (rcCommand != WiFiVariables.HandleLightIndex)
    {
      refrigerator_R.Cabin[0].values.selectedColorIndex = rcCommand;
      refrigerator_R.Cabin[0].E2.selectedColorIndex = rcCommand;
      refrigerator_R.Cabin[1].values.selectedColorIndex = rcCommand;
      refrigerator_R.Cabin[1].E2.selectedColorIndex = rcCommand;
      
      if (refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
          refrigerator_R.ui.timers.dspOffTimer != 60)
        refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;
    }
  }
  return rcCommand;
}
unsigned char fHandleSetVacuumBox(unsigned char rcCommand)
{
  if(refrigerator_F.Cabin[0].components.vacuumBox.type || refrigerator_F.Cabin[1].components.vacuumBox.type)
  {
    if (rcCommand != WiFiVariables.VacuumBoxSet)
    {
      
      refrigerator_R.Cabin[0].values.VacuumBox.sets = (enVacuumBoxSets)rcCommand;
      refrigerator_R.Cabin[0].E2.VacuumSet = (enVacuumBoxSets)rcCommand;
      refrigerator_R.Cabin[1].values.VacuumBox.sets = (enVacuumBoxSets)rcCommand;
      refrigerator_R.Cabin[1].E2.VacuumSet = (enVacuumBoxSets)rcCommand;
      
      if (refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
          refrigerator_R.ui.timers.dspOffTimer != 60)
        refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;
    }
  }
  return rcCommand;
}

unsigned char fHandleSetKeylockDspr(unsigned char rcCommand)
{
  refrigerator_R.Values.keyLockDspr = rcCommand;
  refrigerator_R.E2.keyLockDspr = rcCommand;

      if (refrigerator_R.ui.states.normal.state == _UI_NormalState_DisplayOff || 
          refrigerator_R.ui.timers.dspOffTimer != 60)
        refrigerator_R.ui.states.normal.state = _UI_NormalState_Sets_Init;
  return rcCommand;
}
/* Read Yardimci Fonksiyonlar*/

/*Refrigerator fHandleRead*/
void fHandleReadServiceButton(unsigned char *param)
{
  ;
}
void fHandleReadStockCode(unsigned char *param)
{
  ;
}
void fHandleReadMachineID(unsigned char *param)
{
  ;
}
void fHandleReadServiceButtonShortTimeOut(unsigned char *param)
{
  ;
}
void fHandleReadServiceButtonLongTimeOut(unsigned char *param)
{
  ;
}
void fHandleReadMeasurementInformation(unsigned char *param)
{
  *param = energyMonitoringMeasurement;
}
//  void fHandleReadPassword[(unsigned char *param)
//  {
//    ;
//  }
void fHandleReadNBVersion(unsigned char *param)
{
  *param = refrigerator_F.Connected.definitions.nbVersion;
}
void fHandleReadNBRevision(unsigned char *param)
{
  *param = refrigerator_F.Connected.definitions.nbRevision;
}
void fHandleReadWGVersion(unsigned char *param)
{
  *param = refrigerator_F.Connected.definitions.wgVersion;
}
void fHandleReadDateMinute(unsigned char *param)
{
  ;
}
void fHandleReadDateHour(unsigned char *param)
{
  ;
}
void fHandleReadDateDay(unsigned char *param)
{
  ;
}
void fHandleReadDateMonth(unsigned char *param)
{
  ;
}
void fHandleReadDateYear(unsigned char *param)
{
  ;
}
void fHandleReadRemoteStart(unsigned char *param)
{
  ;
}
void fHandleReadMercekSize(unsigned char *param)
{
  ;
}
void fHandleReadArraySize(unsigned char *param)
{
  *param = WFA_SIZE;
}
void fHandleReadSituation(unsigned char *param)
{
  ;
}
void fHandleReadNBErrors0(unsigned char *param)
{
  ;
}
void fHandleReadMercekErrorLenght(unsigned char *param)
{
  ;
}
void fHandleReadTestMode(unsigned char *param)
{
  ;
}
void fHandleReadEndMesg(unsigned char *param)
{
  ;
}
void fHandleReadResetControl(unsigned char *param)
{
    *param = sendData;
}
void fHandleReadAux2(unsigned char *param)
{
  ;
}
void fHandleReadOTA(unsigned char *param)
{
  ;
}
void fHandleReadCabinTempType(unsigned char *param)
{
  *param = (unsigned char)refrigerator_R.Values.tempType;
}
void fHandleReadCabinTempSet(unsigned char *param)
{
  for(int j=0;j<5;j++)
  {
    unsigned char offset = 0;
    if(WiFiVariables.ShoppingMode && (j<2)) 
      offset = 128;
    
    param[j] = getCabinTemp(j) + offset;    
  }
}
void fHandleReadCabinFunc0(unsigned char *param)
{
  if(WiFiVariables.ShoppingMode)
    *param = shoppingModeIsActive;
  else
    *param  = getCabinFunc(0);
}
void fHandleReadCabinFunc1(unsigned char *param)
{
  if(refrigerator_R.Cabin[1].sets.user.functions.Convertible
     || refrigerator_R.Cabin[1].sets.user.functions.CabinOff)
    *param = convertibleIsActive;
  else if(WiFiVariables.ShoppingMode)
    *param = shoppingModeIsActive;
  else
    *param = getCabinFunc(1);
}
void fHandleReadCabinFunc2(unsigned char *param)
{
  if(WiFiVariables.ShoppingMode)
    *param  = shoppingModeIsActive;
  else
    *param  = getCabinFunc(2);
}
void fHandleReadCabinFunc3(unsigned char *param)
{
  if(WiFiVariables.ShoppingMode)
    *param  = shoppingModeIsActive;
  else
    *param  = getCabinFunc(3);
}
void fHandleReadCabinFunc4(unsigned char *param)
{
  if(WiFiVariables.ShoppingMode)
    *param  = shoppingModeIsActive;
  else
    *param  = getCabinFunc(4);
}
void fHandleReadIceMachineSet(unsigned char *param)
{
  *param= (unsigned char)(!refrigerator_R.IceMachine.sets.iceOFF);
}
void fHandleReadDispenser(unsigned char *param)
{
  *param = (unsigned char)refrigerator_R.Dispenser.sets.state;;
}
void fHandleReadIceSource(unsigned char *param)
{
  switch(refrigerator_R.Dispenser.sets.state)
  {
  case _DispenserState_OFF:
    *param = 0;
    break;    
  case _DispenserState_WaterPump:
    *param  = 0;
    break;    
  case _DispenserState_WaterVacum:
    *param = 0;
    break;    
  default:
    *param  = refrigerator_R.Dispenser.sets.state;
    break;
  }
}
void fHandleReadServiceButtonCopy(unsigned char *param)
{
  ;
}
/*void fHandleReadIceMachine(unsigned char *param)
{
  *param = (unsigned char) refrigerator_R.IceMachine.sets.iceOFF;
}*/
void fHandleReadDisplaySet(unsigned char *param)
{
  *param  = (unsigned char)refrigerator_R.Values.dispOff;
}
void fHandleReadVacation(unsigned char *param)
{
  *param  = (unsigned char)refrigerator_R.Sets.user.functions.Vacation;
}
void fHandleReadEcoFuzzy(unsigned char *param)
{
  if(WiFiVariables.Vacation == 1)
    *param  = 0x80;
  else if(refrigerator_R.Cabin[1].sets.user.functions.Convertible
          || refrigerator_R.Cabin[1].sets.user.functions.CabinOff)
    *param = 0x80;
  else
    *param = (unsigned char)refrigerator_R.Sets.user.functions.EcoExtra;
}
void fHandleReadSabbath(unsigned char *param)
{
  *param  = (unsigned char)refrigerator_R.Sets.user.functions.Sabbath;
}
void fHandleReadConnectedFeatures(unsigned char *param)
{
  *param = 0;
  *param  |= (refrigerator_F.Connected.funcConnectedApp.connectedAppAct << 0);
  *param  |= (refrigerator_F.Connected.funcLog.logAct << 1);
  *param  |= (refrigerator_F.Connected.funcShowHighTempError.showHighTempErrorAct << 2);
  *param  |= (refrigerator_F.Connected.funcdelayDefrost.delayFRZDefrostAct << 3);
}
void fHandleReadVioled(unsigned char *param)
{
  if(refrigerator_F.Cabin[0].components.violed.type && refrigerator_F.Cabin[0].components.violed.activateOnDisplay)
  {
    if(refrigerator_R.Cabin[0].values.violed.state == _VioLedState_Deactive)
      WiFiVariables.Violed = 0x00;
    else
      WiFiVariables.Violed = 0x01;
  }
  else
  {
    *param  = 0x00;
  }
}
void fHandleReadWarnings(unsigned char *param)
{            
  /* 0. bit : door 
  * 1. bit : high temp
  * 2. bit : ice machine
  * 3. bit : NOT: 4 ve 5. bit 3-4 olabilir. Kontrol ediniz!!!
  * 4. bit : buzmatikte su bitince otomatik kapaniyorum bildirimi
  * 5. bit : dispanserdeki su uzun s?redir kullanilmadi bildirimi
  */
  *param  = 0;
  *param  = ((unsigned char)refrigerator_R.Values.doorsAlert) + 2*((unsigned char)refrigerator_R.Values.errHighTemp)+ 4*((unsigned char)refrigerator_R.WaterSource.values.waterTankEmpty) + 8*((unsigned char)refrigerator_R.Values.vacuumBoxLeak);
  /*** Keep disabled: Bit 3 is used for vacuum leak detection. Enabling this causes HomeWhiz to report a vacuum leak when ice drops. ***/
  /*if (refrigerator_R.IceMachine.values.state == _IMch_State_PureIce) 
    *param  |= 0x08;*/
  
}
void fHandleReadError(unsigned char *param)
{
  *param  = (unsigned char)refrigerator_R.Values.doorsClosed;
}
void fHandleReadFilter(unsigned char *param)
{
  ;
}
void fHandleReadAlarmLevel(unsigned char *param)
{
  ;
}
void fHandleReadRefrigeratorAux3(unsigned char *param)
{
  ;
}
void fHandleReadRefrigeratorAux4(unsigned char *param)
{
  ;
}
void fHandleReadComponents1(unsigned char *param)
{
  ;
}
void fHandleReadComponents2(unsigned char *param)
{
  ;
}
void fHandleReadDailyHour(unsigned char *param)
{
  *param  = (unsigned char) refrigerator_R.ConnectedApp.dailyTime.cDailyHour;
}
void fHandleReadDailyMin(unsigned char *param)
{
  *param  = (unsigned char) refrigerator_R.ConnectedApp.dailyTime.cDailyMin;
}
void fHandleReadSpecialTimes_1(unsigned char *param)
{
  ;
}
void fHandleReadShoppingMode(unsigned char *param)
{
  if ( ( !refrigerator_R.Values.shoppingModeTimer ||
        ( refrigerator_F.Cabin[0].type != _CabinType_NONE && !refrigerator_R.Cabin[0].sets.user.functions.Quick)  ||
          ( refrigerator_F.Cabin[1].type != _CabinType_NONE && !refrigerator_R.Cabin[1].sets.user.functions.Quick &&
           !refrigerator_R.Cabin[1].sets.user.functions.Convertible && !refrigerator_R.Cabin[1].sets.user.functions.CabinOff)) 
      && WiFiVariables.ShoppingMode )
  {
    *param  = 0;
    setCabinFunc(0,lastCabinFunc[0]);  
    setCabinFunc(1,lastCabinFunc[1]);
    
    if (!refrigerator_R.Values.shoppingModeTimer)
      WiFiVariables.Warnings |= 0x80 ;
    
  }
}
void fHandleReadSpecialTimes_3(unsigned char *param)
{
  ;
}
void fHandleReadSpecialFunctions(unsigned char *param)
{       
  *param  &= 0x00;
  if (refrigerator_R.Cabin[1].sets.user.functions.Convertible)
    *param  |= 0x01;
  if(refrigerator_R.Cabin[1].sets.user.functions.CabinOff)
    *param  |= 0x02;
}
void fHandleReadHandleLight(unsigned char *param)
{
#ifdef HANDLELIGHT
  *param  = handleLightState;
#endif
}
void fHandleReadDoorAlarmTime(unsigned char *param)
{
  ;
}
void fHandleReadAIMode(unsigned char *param)
{
#if defined (AI_COOL_V1)
  if(refrigerator_R.Cabin[1].sets.user.functions.Convertible // convertible , quick ve cabinoff modelarda Acool hide olsun 
     || refrigerator_R.Cabin[1].sets.user.functions.CabinOff || refrigerator_R.Cabin[1].sets.user.functions.Quick 
       || refrigerator_R.Cabin[0].sets.user.functions.Quick)  
  {
    *param = 128; 
  }
  else if(refrigerator_R.Cabin[0].ai.isEnabled ||
          refrigerator_R.Cabin[1].ai.isEnabled )
    *param = 1;
  else
#endif
#if defined (AI_COOL_V1_5)
    if(refrigerator_R.Cabin[1].sets.user.functions.Convertible // convertible , quick ve cabinoff modelarda Acool hide olsun 
       || refrigerator_R.Cabin[1].sets.user.functions.CabinOff || refrigerator_R.Cabin[1].sets.user.functions.Quick 
         || refrigerator_R.Cabin[0].sets.user.functions.Quick )  
    {
      *param = 128; 
    }
    else if(refrigerator_R.AI.isEnabled)
      *param = 1;
    else
#endif
      *param = 0;
    
}
void fHandleReadCheapSelectMin_3(unsigned char *param)
{
  ;
}
void fHandleReadCheapSelectDuration_3(unsigned char *param)
{
  ;
}
void fHandleReadCheapSelectFlag(unsigned char *param)
{
  ;
}
void fHandleReadCheapSelectConfig(unsigned char *param)
{
  ;
}
void fHandleReadCheapSelectCountDown(unsigned char *param)
{
  ;
}
void fHandleReadDailyDay(unsigned char *param)
{
  ;
}
void fHandleReadDailyMonth(unsigned char *param)
{
  ;
}
void fHandleReadDailyYear(unsigned char *param)
{
  ;
}
void fHandleReadhTE_HighestTempRead(unsigned char *param)
{
  *param  = (unsigned char) refrigerator_R.ConnectedApp.fHighTempError.highestTempRead;
  
}
void fHandleReadhTE_WarningReason(unsigned char *param)
{
  *param  = refrigerator_R.ConnectedApp.fHighTempError.HTE_WarningReason;
  
}
void fHandleReadeE_EnergyEfficiencyDutyFF(unsigned char *param)
{
  *param  = (unsigned char) refrigerator_R.ConnectedApp.fEnergy.energyEfficiencyDutyFF;
  
}
void fHandleReadMercekErrorShadow1(unsigned char *param)
{
  ;
}
void fHandleReadMercekErrorShadow2(unsigned char *param)
{
  ;
}
void fHandleReadQuickIce60(unsigned char *param)
{
  if(WiFiVariables.IceMachineSet == 0)
    *param  = 0x80;
  else
    *param = (unsigned char)refrigerator_R.IceMachine.values.QuickIce60;
}
void fHandleReadRGBHandleLight(unsigned char *param)
{
    *param = (unsigned char)refrigerator_R.Cabin[0].values.selectedColorIndex;
}
void fHandleReadVacuumBox(unsigned char *param)
{
    *param = (unsigned char)refrigerator_R.Cabin[0].values.VacuumBox.sets;
}
void fHandleReadKeylockDspr(unsigned char *param)
{
    *param = (unsigned char)refrigerator_R.Values.keyLockDspr;
}

uint8_t fHandleEnergySavingMode(uint8_t rcCommand)
{
  //	if(app_Is_NonSelectable_Now(sp_ENERGY_SAVING_MODE) )
  //	{
  //		rcCommand = programFlag.bits.EnergySaveMode_Selected;
  //	}
  //	else
  //	{
  //		programFlag.bits.EnergySaveMode_Selected = rcCommand;	
  //	}
  
  return rcCommand;
}

/// @brief Handling service call process
uint8_t fHandleServiceCall(uint8_t rcCommand)
{
  //	rcServiceCall.newState = rcCommand;
  
  return rcCommand;
}

void setConnectivityError(uint8_t state)
{
  //	if(state)
  //	{
  //		api_SafetyInterface_SetSafetyStatus(SAFETY_CONNECTIVITY_ERROR);
  //	}
  //	else
  //	{
  //		api_SafetyInterface_ClearSafetyStatus(SAFETY_CONNECTIVITY_ERROR);
  //	}
}

void initializeSecureMercekParams(void)
{
  secureMercekVars.data2 = 3; //< Brand Code, 1 byte, enum table 1: Ar?elik 2: Grundig 3: Beko 4: Dawnlance
  secureMercekVars.data3 = 1; //< Client Type, 1 byte, enum table 1: White Good 2: Android App 3: iOS App
  secureMercekVars.data4 = 2; //< Machine Type, 1 byte, enum table 1: WM 2: Fridge 3: DW 4: Oven 5: Dryer 6: Air Conditioner
  secureMercekVars.data5 = refrigerator_R.Stock.Version; //< Software Version Major, 1 byte
  secureMercekVars.data6 = refrigerator_R.Stock.Revision; //< Software Version Minor, 1 byte
  secureMercekVars.data8 = 2; //< Operating System, 1 byte, 0: No OS 1: RCoS1 2: RCoS2 3: Linux
  secureMercekVars.data1 = 0; //< Secure Mercek Version, 1 byte
  secureMercekVars.data7 = 0; //< Software Version Build, 1 byte
  secureMercekVars.data9 = 0; //< Reset Reason 1
  secureMercekVars.data10 = 0; //< Reset Reason 2
  secureMercekVars.data11 = 0; //< Reset Reason 3
  secureMercekVars.data12 = 0; //< Reset Reason 4
  secureMercekVars.data13 = 0; //< Reset Reason 5
  secureMercekVars.data14 = 0; //< Safir Power Cycle Count first byte
  secureMercekVars.data15 = 0; //< Safir Power Cycle Count second byte
  secureMercekVars.data16 = 0; //< Factory Reset Success Count first byte
  secureMercekVars.data17 = 0; //< Factory Reset Success Count second byte
  secureMercekVars.data18 = 0; //< Factory Reset Fail Count first byte
  secureMercekVars.data19 = 0; //< Factory Reset Fail Count second byte
}

void MercekUpdateData(enMercekSamplingPeriod period)
{
  signed short calcSens = 0;
  signed char avgSens = 0;
  unsigned char avgHum = 0;
  unsigned short calcHum = 0;
  unsigned char temp = 0;
  unsigned char shift = 0;
  unsigned char idx = 0;
  unsigned short avgCompSpeed = 0;
  
  unsigned char *mercekDataPtr = (unsigned char *)&MercekData;
  
  if (SecureMercekSendTime)
  {
    TwentySeconds++;
    
    if (TwentySeconds >= 20)
    {
      TwentySeconds = 0;
      SecureMercekSendTime = false;
      MercekData.SendData = SEND_SECURE_MERCEK_COMMAND;   //send secure mercek data for testing pursose
    }
  }
  
  
  for (unsigned char i = 0; MercekDataTable[i].offset < EOF_MERCEK_DATA; i++)
  {
    if(MercekDataTable[i].period == period)
    {
      switch (MercekDataTable[i].offset)
      {
      case SENDING_REASON: // Addr 0
        MercekDataTable[i].period = EVERY_SECONDS;
        if(refrigerator_R.ui.serviceButton.sendMercekArray) //mercek data servis butonu sebebi ile
        {
          refrigerator_R.ui.serviceButton.sendMercekArray = 0;
          if ( MercekData.SendData == 0 )			// if anything to send
          {
            *(mercekDataPtr + MercekDataTable[i].offset) = MERCEK_REASON_SERVICE_BUTTON;
            *(mercekDataPtr + MercekDataTable[i].offset) |= ( refrigerator_R.Values.errors.errorsNumber ) ? MERCEK_REASON_ERROR : 0x00;
            MercekData.MercekCount++;
            MercekData.SendData = SEND_MERCEK_COMMAND;				// set send flag to send
            LastSendF = MercekData.SendData;		// set LastSendF to understand WA sended
            SecureMercekSendTime = true;
          }

        }
        if(refrigerator_R.workMode.values.prodTest.minTimer <= 1 
           && refrigerator_R.Cabin[0].components.airTempSensor.valueInCExt > 100 
             && refrigerator_R.Cabin[0].components.evaTempSensor.valueInCExt > 100
               && refrigerator_R.Cabin[1].components.airTempSensor.valueInCExt > 10
                 && refrigerator_R.Cabin[1].components.evaTempSensor.valueInCExt > 10) 
        {
          if ( MercekData.SendData == 0 && !PowerLossMASend )		//if anything to send in first min
          {
            PowerLossMASend = 1;
            *(mercekDataPtr + MercekDataTable[i].offset) = MERCEK_REASON_POWERCUT;
            *(mercekDataPtr + MercekDataTable[i].offset) |= ( refrigerator_R.Values.errors.errorsNumber) ? MERCEK_REASON_ERROR : 0x00;
            MercekData.MercekCount++;
            MercekData.SendData = SEND_MERCEK_COMMAND;				// Trigger to Safir: Mercek Data is Ready
            LastSendF = MercekData.SendData;		// Double Check for Mercek is taken by Safir
            SecureMercekSendTime = true;
          }

        }
        break;
        
      case MB_STOCK_NO: // Addr 4
        MercekDataTable[i].period = ONE_TIME;
        *(mercekDataPtr + MercekDataTable[i].offset) = refrigerator_F.Stock.No[2];
        *(mercekDataPtr + MercekDataTable[i].offset + 1) = (refrigerator_F.Stock.No[1] / 100) % 100;
        *(mercekDataPtr + MercekDataTable[i].offset + 2) = refrigerator_F.Stock.No[1] % 100;
        *(mercekDataPtr + MercekDataTable[i].offset + 3) = refrigerator_F.Stock.No[0] / 100;
        break;
        
      case MB_VERSION: // Addr 5
        MercekDataTable[i].period = ONE_TIME; 
        *(mercekDataPtr + MercekDataTable[i].offset) = refrigerator_F.Stock.Version;
        break;
        
      case MB_REVISION: // Addr 6
        MercekDataTable[i].period = ONE_TIME;
        *(mercekDataPtr + MercekDataTable[i].offset) = refrigerator_F.Stock.Revision;
        break;
                                                                                //_WM_ProductionTest --> 0000 0000 = 1
      case WORKMODE: // Addr 7  // each bit represnt 1 workmode, such as workmode _WM_Normal(1)      --> 0000 0010 = 2 
        MercekDataTable[i].period = EVERY_MINUTES;                              //_WM_Distributer(2) ---> 0000 0100 = 4 
        if ((int)refrigerator_R.workMode.values.workMode <= 7) //1 byte max can display 7
        {
          *(mercekDataPtr + MercekDataTable[i].offset) |= (1 << refrigerator_R.workMode.values.workMode);
        }
        break;
        
      case CABIN_TYPE_MODE:  // V03R05: Cabin modes bitmap (ONE_TIME)
        // bit[1:0] = Cabin[0] mode (0=NONE, 1=FF, 2=FRZ)
        // bit[3:2] = Cabin[1] mode (0=NONE, 1=FF, 2=FRZ)
        {
          unsigned char cabinTypeMode = 0;
          cabinTypeMode |= (refrigerator_R.Cabin[0].sets.work.Mode & 0x03);        // Cabin0 in bits 0-1
          cabinTypeMode |= ((refrigerator_R.Cabin[1].sets.work.Mode & 0x03) << 2); // Cabin1 in bits 2-3
          *(mercekDataPtr + MercekDataTable[i].offset) = cabinTypeMode;
        }
        break;
        
      case CA0_CA1_SETS_IDX: // Addr 9
        MercekDataTable[i].period = EVERY_MINUTES;
        *(mercekDataPtr + MercekDataTable[i].offset) = 0;
        for(unsigned char j = 0; j < 2; j++)
        {
          unsigned char temp = 0;
          
          if(refrigerator_R.Cabin[j].sets.work.Mode == _CabinMode_FF)
          {
            temp = refrigerator_R.Cabin[j].sets.datas.display;
          }
          else if(refrigerator_R.Cabin[j].sets.work.Mode == _CabinMode_FRZ)
          {
            if(refrigerator_R.Cabin[j].sets.work.functions.Convertible)
              temp = refrigerator_R.Cabin[j].sets.datas.display;
            else if(refrigerator_R.Cabin[j].sets.work.functions.CabinOff)
              temp = 0x0F;
            else
              temp = (-refrigerator_R.Cabin[j].sets.datas.display)-15;
          }
          
          *(mercekDataPtr + MercekDataTable[i].offset) |= (temp << (4-j*4));
        }
        break;
        
      case CA0_CA1_DOOR_OPEN_COUNTER: // Addr 9-10 (V03R03: 2 bytes, max 255 each)
        MercekDataTable[i].period = EVERY_SECONDS;
        for(unsigned char s = 0; s < 2; s++)  // s=0: FF (Cabin[0]), s=1: FRZ (Cabin[1])
        {
          // Rising edge detection: door just opened
          if((refrigerator_R.Cabin[s].components.doorSwitch[0].value ||
              refrigerator_R.Cabin[s].components.doorSwitch[1].value /* ||
                refrigerator_R.Cabin[s].components.manuelDoorSwitch.value*/) && (!(doorStatus & (1 << s))))
          {
            // Increment counter if not at max (255)
            if(*(mercekDataPtr + MercekDataTable[i].offset + s) < 0xFF)
            {
              (*(mercekDataPtr + MercekDataTable[i].offset + s))++;
            }
          }
          
          // Update door status for edge detection
          if(refrigerator_R.Cabin[s].components.doorSwitch[0].value ||
             refrigerator_R.Cabin[s].components.doorSwitch[1].value /* ||
               refrigerator_R.Cabin[s].components.manuelDoorSwitch.value*/)
          {
            doorStatus |= (1 << s);
          }
          else
          {
            doorStatus &= ~(1 << s);
          }
        }
        break;
        
      case CA0_DOOR_OPEN_TIMER: // Addr 10
        MercekDataTable[i].period = EVERY_SECONDS;
        if(refrigerator_R.Cabin[0].components.doorSwitch[0].value ||
           refrigerator_R.Cabin[0].components.doorSwitch[1].value /*||
             refrigerator_R.Cabin[0].components.manuelDoorSwitch.value*/)
        {
          if(doorOpenTimer[0] < 120 * 60)
            doorOpenTimer[0]++;
          
          if(doorOpenTimer[0] < 4 * 60)
            *(mercekDataPtr + MercekDataTable[i].offset) = doorOpenTimer[0];
          else
          {
            *(mercekDataPtr + MercekDataTable[i].offset) = 0x80;
            *(mercekDataPtr + MercekDataTable[i].offset) |= (doorOpenTimer[0] / 60);
          }
        }
        break;
        
      case CA1_DOOR_OPEN_TIMER: // Addr 11
        MercekDataTable[i].period = EVERY_SECONDS;
        if(refrigerator_R.Cabin[1].components.doorSwitch[0].value ||
           refrigerator_R.Cabin[1].components.doorSwitch[1].value /*||
             refrigerator_R.Cabin[1].components.manuelDoorSwitch.value*/)
        {
          if(doorOpenTimer[1] < 120 * 60)
            doorOpenTimer[1]++;
          
          if(doorOpenTimer[1] < 4 * 60)
            *(mercekDataPtr + MercekDataTable[i].offset) = doorOpenTimer[1];
          else
          {
            *(mercekDataPtr + MercekDataTable[i].offset) = 0x80;
            *(mercekDataPtr + MercekDataTable[i].offset) |= (doorOpenTimer[1] / 60);
          }
        }
        break;
        
      case HUMAN_SENSOR_COUNTER:  // V03R10: Human sensor activation count (0-255)
        MercekDataTable[i].period = EVERY_SECONDS;
        {
          // Check which cabin has human sensor configured (type != 0)
          unsigned char currentValue = 0;
          for (unsigned char c = 0; c < MERCEK_CABIN_SIZE; c++)
          {
            if (refrigerator_F.Cabin[c].components.humanSensor.type != 0)
            {
              currentValue = refrigerator_R.Cabin[c].components.humanSensor.value;
              break;  // Only one cabin uses human sensor
            }
          }
          
          // Rising edge detection: count activations
          if (currentValue && !humanSensorStatus)
          {
            if (*(mercekDataPtr + MercekDataTable[i].offset) < 255)
            {
              (*(mercekDataPtr + MercekDataTable[i].offset))++;
            }
          }
          humanSensorStatus = currentValue;
        }
        break;
        
      case FUNCTIONS: // Addr 12
        MercekDataTable[i].period = EVERY_MINUTES;
        if ( refrigerator_R.Sets.user.functions.EcoExtra )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x01;
        
        if ( refrigerator_R.Cabin[0].sets.user.functions.Vacation )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x02;
        
        if ( refrigerator_R.Cabin[0].sets.user.functions.Quick )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x04;
        
        if ( refrigerator_R.Cabin[0].sets.user.functions.Convertible )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x08;
        
        if ( refrigerator_R.Cabin[0].sets.user.functions.CabinOff )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x10;
        
        if ( refrigerator_R.Cabin[1].sets.user.functions.Quick )
          *(mercekDataPtr + MercekDataTable[i].offset)|= 0x20;
        
        if ( refrigerator_R.Cabin[1].sets.user.functions.Convertible )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x40;
        
        if ( refrigerator_R.Cabin[1].sets.user.functions.CabinOff )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x80;
        break;
        
      case FUNCTIONS_2: // V03R07: Additional functions bitmap (same logic as FUNCTIONS for now)
        MercekDataTable[i].period = EVERY_MINUTES;
        if ( !refrigerator_R.IceMachine.sets.iceOFF )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x01;
        
        if ( refrigerator_R.IceMachine.values.QuickIce60 )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x02;
#if defined (AI_COOL_V1) 
        if( refrigerator_R.Cabin[0].ai.isEnabled || refrigerator_R.Cabin[1].ai.isEnabled )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x04;
#elif defined (AI_COOL_V1_5)
        if ( refrigerator_R.AI.isEnabled )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x04;
#endif
        
      /*  if ( refrigerator_R.Cabin[0].sets.user.functions.CoD ) //Cool on Demand Reserved
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x08;*/
   if(!refrigerator_F.Cabin[0].components.vacuumBox.type || !refrigerator_F.Cabin[1].components.vacuumBox.type)  
   {    
        if ( (refrigerator_R.Cabin[0].values.VacuumBox.sets == _VacuumSet_MinusOne) || (refrigerator_R.Cabin[1].values.VacuumBox.sets == _VacuumSet_MinusOne) )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x10;
        
        if ( (refrigerator_R.Cabin[0].values.VacuumBox.sets == _VacuumSet_One) || (refrigerator_R.Cabin[1].values.VacuumBox.sets == _VacuumSet_One) )
          *(mercekDataPtr + MercekDataTable[i].offset)|= 0x20;
        
        if ( (refrigerator_R.Cabin[0].values.VacuumBox.status == _VacuumBox_Lock) || (refrigerator_R.Cabin[1].values.VacuumBox.status == _VacuumBox_Lock) )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x40;
        
        if ( (refrigerator_R.Cabin[0].values.VacuumBox.status == _VacuumBox_Leak) || (refrigerator_R.Cabin[1].values.VacuumBox.status == _VacuumBox_Leak) )
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x80;
   }
        break;
        
      case FUNCTIONS_3:  // V03R11: [3:0]=Persona bitmap, [7:4]=HandleLight color index
        MercekDataTable[i].period = EVERY_MINUTES;
        {
          unsigned char val = 0;
#if defined(AI_COOL_V1_5)      
          // LSB [3:0]: Persona bitmap (predictedPersona 0-3 maps to bit 0-3)
          unsigned char persona = refrigerator_R.AI.predictedPersona;
          if (persona <= 3)
          {
            val |= (1 << persona);  // 0?bit0, 1?bit1, 2?bit2, 3?bit3
          }
#endif          
          // LSB [7:4]: HandleLight color index (0-15)
          if (refrigerator_F.Cabin[0].components.handleLight.type != 0)
          {
            unsigned char colorIdx = refrigerator_R.Cabin[0].values.selectedColorIndex;
            if (colorIdx > 15) colorIdx = 15;  // Clamp to 4 bits max
            val |= (colorIdx << 4);
          }
          
          *(mercekDataPtr + MercekDataTable[i].offset) = val;
        }
        break;
        
      case ICE_MACHINE:
        
        MercekDataTable[i].period = EVERY_SECONDS;
        
        // --------------------------------------------------------------------
        // BYTE 0 (at offset):
        //   [7:6] -> 2-bit saturating counter, increments on rising edge to _IMch_State_PureIce (0..3)
        //   [5:0] -> minutes while state == _IMch_State_BoxFull (0..63, saturating)
        // --------------------------------------------------------------------
        {
          uint8_t *p0  = (uint8_t *)(mercekDataPtr + MercekDataTable[i].offset);
          uint8_t  val0 = *p0;
          
          // Extract current bitfields
          uint8_t top2_0 = (val0 >> 6) & 0x03;   // MSBs [7:6] as a 0..3 value
          uint8_t low6_0 =  val0       & 0x3F;   // LSBs [5:0] as a 0..63 value
          
          // Accumulate seconds in BoxFull and convert to minutes (saturate at 63)
          if (refrigerator_R.IceMachine.values.state == _IMch_State_BoxFull) {
            iceMachBoxFull_sec++;
            uint32_t minutes = iceMachBoxFull_sec / 60u;
            if (minutes > 63u) minutes = 63u;  // lower 6 bits can hold max 63
            low6_0 = (uint8_t)minutes;
          }
          // If not BoxFull, we keep the previous minutes as-is.
          // If a different behavior is desired (e.g., reset or pause), handle it here.
          
          // Rising edge detection for PureIce: increment upper 2-bit counter (saturating 0..3)
          if ((refrigerator_R.IceMachine.values.state == _IMch_State_PureIce) &&
              (iceMachStatus != refrigerator_R.IceMachine.values.state))
          {
            if (top2_0 < 3u) {
              top2_0++;
            }
          }
          
          // Recompose the byte: [7:6]=top2_0, [5:0]=low6_0 (single RMW write)
          val0 = (uint8_t)((top2_0 << 6) | (low6_0 & 0x3F));
          *p0  = val0;
          
          // Persist last state for edge detection next cycle
          iceMachStatus = refrigerator_R.IceMachine.values.state;
        }
        
        // --------------------------------------------------------------------
        // BYTE 1 (at offset+1):
        //   [7:6] -> 2-bit saturating counter, increments on rising edge of errCaseTactSW (0..3)
        //   [5:0] -> minutes while fan.speedOrPWM is true (0..63, saturating)
        // --------------------------------------------------------------------
        {
          uint8_t *p1  = (uint8_t *)(mercekDataPtr + MercekDataTable[i].offset + 1);
          uint8_t  val1 = *p1;
          
          // Extract current bitfields
          uint8_t top2_1 = (val1 >> 6) & 0x03;   // MSBs [7:6] as a 0..3 value
          uint8_t low6_1 =  val1       & 0x3F;   // LSBs [5:0] as a 0..63 value
          
          // Accumulate seconds while fan is ON and convert to minutes (saturate at 63)
          if (refrigerator_R.IceMachine.components.fan.speedOrPWM) {
            iceMachFanOn_sec++;
            uint32_t minutes = iceMachFanOn_sec / 60u;
            if (minutes > 63u) minutes = 63u;  // lower 6 bits can hold max 63
            low6_1 = (uint8_t)minutes;
          }
          // If the fan turns OFF, we keep last minutes (sticky). Adjust here if you prefer reset.
          
          // Rising edge detection for errCaseTactSW: increment upper 2-bit counter (0..3, saturating)
          bool tactNow = refrigerator_R.IceMachine.values.errCaseTactSW ? true : false;
          if (tactNow && !icemachineTactSWErrorStatus) {
            if (top2_1 < 3u) {
              top2_1++;
            }
          }
          
          // Recompose the byte: [7:6]=top2_1, [5:0]=low6_1 (single RMW write)
          val1 = (uint8_t)((top2_1 << 6) | (low6_1 & 0x3F));
          *p1  = val1;
          
          // Persist last tact switch state for edge detection next cycle
          icemachineTactSWErrorStatus = tactNow; 
        }
        
        break;
        
        
        
      case ICE_MACHINE_TEMP:
        MercekDataTable[i].period = EVERY_MINUTES;
        
        if(!(MercekData.DataCount % 6))
        {
          byteNo = MercekData.DataCount / 6;
          if(byteNo < ICE_MACHINE_TEMP_SIZE)  // Buffer overflow check (if somehow datacount keep continue, check size)
          {
            *(mercekDataPtr + MercekDataTable[i].offset + byteNo) = (refrigerator_R.IceMachine.components.caseTempSensor.valueInCExt >> 2);
          }
        }
        break;
        
      case ICE_MACHINE_WATER_HEATER:  // V03R08: Ice Machine Water Heater
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.WaterSource.components.waterVentilHeater.value && (!(heaterStatus & 0x80)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.WaterSource.components.waterVentilHeater.value)
        {
          heaterStatus |= 0x80;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          heaterStatus &= ~(0x80);
        }
        break;
        
      case DISPENSER:  // V03R06: 2 bytes - [0]=state bitmap, [1]=tactSW count (0-255)
        MercekDataTable[i].period = EVERY_SECONDS;
      
        // Byte 0: State bitmap (states 1-8 mapped to bits 0-7, state 0 ignored)
        {
          unsigned char state = refrigerator_R.Dispenser.sets.state;
          if(state >= 1 && state <= 8)  // State 1-8 valid, state 0 ignored
          {
            *(mercekDataPtr + MercekDataTable[i].offset) |= (1 << (state - 1));  // State 1?bit0, State 8?bit7
          }
        }
        
        // Byte 1: TactSwitch press count (0-255)
        if(refrigerator_R.Dispenser.components.tactSwitch.value && !dispenserTactSWStatus)
        {
          if(*(mercekDataPtr + MercekDataTable[i].offset + 1) < 0xFF)  // Max 255
          {
            (*(mercekDataPtr + MercekDataTable[i].offset + 1))++;
          }
        }
        dispenserTactSWStatus = refrigerator_R.Dispenser.components.tactSwitch.value;
        break;
        
      case CA0_FAN:
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.Cabin[0].components.fan.speedOrPWM && (!(fanStatus & 0x01)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.Cabin[0].components.fan.speedOrPWM)
        {
          fanStatus |= 0x01;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          fanStatus &= ~(0x01);
        }
        break;
        
      case CA0_HEATERA:
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.Cabin[0].components.heaterA.value && (!(heaterStatus & 0x01)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.Cabin[0].components.heaterA.value)
        {
          heaterStatus |= 0x01;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          heaterStatus &= ~(0x01);
        }
        break;
        
      case CA0_HEATERB:
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.Cabin[0].components.heaterB.value && (!(heaterStatus & 0x02)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.Cabin[0].components.heaterB.value)
        {
          heaterStatus |= 0x02;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          heaterStatus &= ~(0x02);
        }
        break;
        
      case CA0_HEATERC:
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.Cabin[0].components.heaterC.value && (!(heaterStatus & 0x04)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.Cabin[0].components.heaterC.value)
        {
          heaterStatus |= 0x04;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          heaterStatus &= ~(0x04);
        }
        break;
        
      case CA1_FAN:
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.Cabin[1].components.fan.speedOrPWM && (!(fanStatus & 0x02)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.Cabin[1].components.fan.speedOrPWM)
        {
          fanStatus |= 0x02;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          fanStatus &= ~(0x02);
        }
        break;
        
      case CA1_HEATERA:
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.Cabin[1].components.heaterA.value && (!(heaterStatus & 0x10)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.Cabin[1].components.heaterA.value)
        {
          heaterStatus |= 0x10;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          heaterStatus &= ~(0x10);
        }
        break;
        
      case CA1_HEATERB:
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.Cabin[1].components.heaterB.value && (!(heaterStatus & 0x20)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.Cabin[1].components.heaterB.value)
        {
          heaterStatus |= 0x20;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          heaterStatus &= ~(0x20);
        }
        break;
        
      case CA1_HEATERC:
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.Cabin[1].components.heaterC.value && (!(heaterStatus & 0x40)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.Cabin[1].components.heaterC.value)
        {
          heaterStatus |= 0x40;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          heaterStatus &= ~(0x40);
        }
        break;
        
      case VACUUM_HEATER:  // V03R12: Vacuum box heater (same logic as CA0_HEATERA)
        MercekDataTable[i].period = EVERY_MINUTES;
        if (refrigerator_R.Cabin[0].components.VacuumBoxHeater.value && (!(heaterStatus & 0x08)))
        {
          if ((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
          {
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
          }
        }
        
        if (refrigerator_R.Cabin[0].components.VacuumBoxHeater.value)
        {
          heaterStatus |= 0x08;
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        else
        {
          heaterStatus &= ~(0x08);
        }
        break;
        
      case COOLER_TYPE:  // V03R04: Cooler type bitmap (ONE_TIME)
        // Bitmap: bit0=NONE, bit1=DEFAULT, bit2=PARALLEL, bit3=SEMIPARALLEL, bit4=HOTGAS
        // bit5-6=reserved, bit7=waterTankEmpty
        MercekDataTable[i].period = EVERY_MINUTES;
        unsigned char coolerType = refrigerator_F.Cooler.type;
        
        if(coolerType <= 4)  // Valid range check
        {
          *(mercekDataPtr + MercekDataTable[i].offset) |= (1 << coolerType);
        }
        // bit7: Water tank empty error
        if(refrigerator_R.WaterSource.values.waterTankEmpty)
        {
          *(mercekDataPtr + MercekDataTable[i].offset) |= 0x80;
        }
        
        break;
        
      case SELENOID_VALVE:
        MercekDataTable[i].period = EVERY_MINUTES;
        
        if(refrigerator_R.Cooler.components.solenoidValve[0].positionIndex < SELENOID_VALVE_SIZE)  // Buffer overflow protection
        {
          if((*(mercekDataPtr + MercekDataTable[i].offset + refrigerator_R.Cooler.components.solenoidValve[0].positionIndex) & ~(0xC0)) < 63)
            *(mercekDataPtr + MercekDataTable[i].offset + refrigerator_R.Cooler.components.solenoidValve[0].positionIndex) += 1;
          
          if((*(mercekDataPtr + MercekDataTable[i].offset + refrigerator_R.Cooler.components.solenoidValve[0].positionIndex) & (0xC0)) < 0xC0)
          {
            if(svPosStatus != refrigerator_R.Cooler.components.solenoidValve[0].positionIndex)
              *(mercekDataPtr + MercekDataTable[i].offset + refrigerator_R.Cooler.components.solenoidValve[0].positionIndex) += 0x40;
          }
        }
        
        svPosStatus = refrigerator_R.Cooler.components.solenoidValve[0].positionIndex;
        break;
        
      case SELENOID_VALVE_DETAIL:
        MercekDataTable[i].period = EVERY_MINUTES;
        idx = (MercekData.DataCount >> 1);
        if(idx < SELENOID_VALVE_DETAIL_SIZE)  // Buffer overflow protection
        {
          shift = (MercekData.DataCount & 0x01) * 4;
          *(mercekDataPtr + MercekDataTable[i].offset + idx) |= (refrigerator_R.Cooler.components.solenoidValve[0].positionIndex << shift);
        }
        break;
        
      case ENERGY_MONITOR:  // V03R21: Energy consumption (Wh cumulative)
        MercekDataTable[i].period = EVERY_MINUTES;
        {
          // Get instant power value (in 0.01W units, e.g. 24000 = 240W)
          unsigned short instantPower = energyMonitoringProcParams.energy.T1sec.instantValue;
          unsigned short powerW = instantPower / 100;  // Convert to W
          
          // Accumulate W-minutes
          energyWattMinuteSum += powerW;
          
          // Calculate Wh (W-minutes / 60)
          unsigned long energyWh = energyWattMinuteSum / 60;
          
          // Clamp to 0-255 range
          if (energyWh > 255)
          {
            energyWh = 255;
          }
          *(mercekDataPtr + MercekDataTable[i].offset) = (unsigned char)energyWh;
        }
        break;
        
      case COMPRESSOR:
        MercekDataTable[i].period = EVERY_MINUTES;
        avgCompSpeed = 0;
        
        for (unsigned char s = 0; s < MercekDataTable[i].dataSize; s++)
        {
          switch (s)
          {
          case 0:
            if((!compressorStatus) && (refrigerator_R.Cooler.components.compressor.value))
            {
              if ((*(mercekDataPtr + MercekDataTable[i].offset + s) & 0xC0) < 0xC0)
                *(mercekDataPtr + MercekDataTable[i].offset + s) += 0x40;
            }
            
            if(refrigerator_R.Cooler.components.compressor.value)
            {
              if((*(mercekDataPtr + MercekDataTable[i].offset + s) & ~(0xC0)) < 60)
                *(mercekDataPtr + MercekDataTable[i].offset + s) += 1;
            }
            break;
            
          case 1:
            byteNo = (refrigerator_R.Cooler.parameters.gasLevel / 8);
            bitNo = (refrigerator_R.Cooler.parameters.gasLevel % 8);
            if(byteNo < 2)  // Buffer overflow protection (only byte 1 and 2 available)
            {
              *(mercekDataPtr + MercekDataTable[i].offset + s + byteNo) |= (0x01 << bitNo);
            }
            break;
            
          case 2:
            break;
            
          case 3:
            avgCompSpeed = *(mercekDataPtr + MercekDataTable[i].offset + s) * MercekData.DataCount;
            avgCompSpeed += refrigerator_R.Cooler.components.compressor.value;
            *(mercekDataPtr + MercekDataTable[i].offset + s) = avgCompSpeed / (MercekData.DataCount + 1);
            break;
            
          default:
            break;
          }
        }
        
        if(refrigerator_R.Cooler.components.compressor.value)
          compressorStatus = 1;
        else
          compressorStatus = 0;
        break;
        
      case COMPRESSOR_DETAIL:
        MercekDataTable[i].period = EVERY_MINUTES;
        byteNo = MercekData.DataCount / 8;
        if(byteNo < COMPRESSOR_DETAIL_SIZE)  // Buffer overflow protection
        {
          bitNo = MercekData.DataCount % 8;
          *(mercekDataPtr + MercekDataTable[i].offset + byteNo) |= (compressorStatus << bitNo);
        }
        break;
        
      case COMP_STOCK_NO:  // V03R13: Compressor stock number
        MercekDataTable[i].period = ONE_TIME;
        {
          // Store 3 stock numbers (0-999 each) in 6 bytes (2 bytes per number, big-endian)
          // No decode needed: No[2]=Byte[0-1], No[1]=Byte[2-3], No[0]=Byte[4-5]
          unsigned short no2 = refrigerator_R.Cooler.components.compressor.Stock.No[2];
          unsigned short no1 = refrigerator_R.Cooler.components.compressor.Stock.No[1];
          unsigned short no0 = refrigerator_R.Cooler.components.compressor.Stock.No[0];
          *(mercekDataPtr + MercekDataTable[i].offset)     = (unsigned char)(no2 >> 8);
          *(mercekDataPtr + MercekDataTable[i].offset + 1) = (unsigned char)(no2 & 0xFF);
          *(mercekDataPtr + MercekDataTable[i].offset + 2) = (unsigned char)(no1 >> 8);
          *(mercekDataPtr + MercekDataTable[i].offset + 3) = (unsigned char)(no1 & 0xFF);
          *(mercekDataPtr + MercekDataTable[i].offset + 4) = (unsigned char)(no0 >> 8);
          *(mercekDataPtr + MercekDataTable[i].offset + 5) = (unsigned char)(no0 & 0xFF);
        }
        break;
        
      case COMP_VERSION:  // V03R14: Compressor version
        MercekDataTable[i].period = ONE_TIME;
        *(mercekDataPtr + MercekDataTable[i].offset) = refrigerator_R.Cooler.components.compressor.Stock.Version;
        break;
        
      case COMP_REVISION:  // V03R14: Compressor revision
        MercekDataTable[i].period = ONE_TIME;
        *(mercekDataPtr + MercekDataTable[i].offset) = refrigerator_R.Cooler.components.compressor.Stock.Revision;
        break;
        
      case COMP_CORE_VERSION:  // V03R22: Compressor core version
        MercekDataTable[i].period = ONE_TIME;
        *(mercekDataPtr + MercekDataTable[i].offset) = refrigerator_R.Cooler.components.compressor.Stock.CoreVersion;
        break;
        
      case COMP_CORE_REVISION:  // V03R22: Compressor core revision
        MercekDataTable[i].period = ONE_TIME;
        *(mercekDataPtr + MercekDataTable[i].offset) = refrigerator_R.Cooler.components.compressor.Stock.CoreRevision;
        break;
        
      case COMP_TYPE:  // V03R15: Compressor type and onOff status
        // bit[3:0] = compressorType bitmap (bit0=EKI, bit1=JIAXIPERA, bit2=EMBRACO, bit3=TWOINONE)
        // bit[6:4] = onOffStatus bitmap (bit4=ON, bit5=OFFPREPARE, bit6=OFF)
        // bit[7] = reserved
        MercekDataTable[i].period = EVERY_MINUTES;
        {
          unsigned char compType = 0;
          unsigned char onOffStatus = 0;
          // Bitmap: value 1 -> bit0, value 2 -> bit1, etc.
          if(inverterCommProcParams.compressorComm.compressorType >= 1 && 
             inverterCommProcParams.compressorComm.compressorType <= 4)
          {
            compType = (1 << (inverterCommProcParams.compressorComm.compressorType - 1));
          }
          if(inverterCommProcParams.compressorComm.onOffStatus >= 1 && 
             inverterCommProcParams.compressorComm.onOffStatus <= 3)
          {
            onOffStatus = (1 << (inverterCommProcParams.compressorComm.onOffStatus + 3));
          }
          *(mercekDataPtr + MercekDataTable[i].offset) = compType | onOffStatus;
        }
        break;
        
      case COMP_ERROR:  // V03R20: Compressor error/status code
        MercekDataTable[i].period = EVERY_MINUTES;
        {
          unsigned short alarmCode = 0;
          
          // Get alarm/status code based on compressor type
          if(inverterCommProcParams.compressorComm.compressorType == COMPRESSOR_TYPE_EKI)
          {
            alarmCode = inverterCommProcParams.compressorComm.EKIPackets.MotorInverterAlarmCode;
          }
          else
          {
            alarmCode = inverterCommProcParams.compressorComm.Embc_Jiax_Packets.StatusData;
          }
          
          *(mercekDataPtr + MercekDataTable[i].offset) = (unsigned char)(alarmCode & 0xFF);  // Low byte
          *(mercekDataPtr + MercekDataTable[i].offset + 1) = (unsigned char)((alarmCode >> 8) & 0xFF);  // High byte
        }
        break;
        
      case COMP_POWER:  // V03R18: Compressor power average (hourly)
        MercekDataTable[i].period = EVERY_MINUTES;
        {
          unsigned short power = 0;
          
          // Get power based on compressor type
          if(inverterCommProcParams.compressorComm.compressorType == COMPRESSOR_TYPE_EKI)
          {
            power = inverterCommProcParams.compressorComm.EKIPackets.PowerDQActual;
          }
          else
          {
            power = inverterCommProcParams.compressorComm.Embc_Jiax_Packets.Power;
          }
          
          // Collect power data every minute
          compPowerSum += power;
          compPowerCount++;
          
          // Calculate and write current average
          unsigned short avgPower = (compPowerCount > 0) ? (unsigned short)(compPowerSum / compPowerCount) : 0;
          *(mercekDataPtr + MercekDataTable[i].offset) = (unsigned char)(avgPower & 0xFF);  // Low byte
          *(mercekDataPtr + MercekDataTable[i].offset + 1) = (unsigned char)((avgPower >> 8) & 0xFF);  // High byte
        }
        break;
        
      case COMP_DCBUS:  // V03R18: DC bus voltage min/max
        MercekDataTable[i].period = EVERY_MINUTES;
        {
          unsigned short dcbusVoltage = 0;
          
          // Get DC bus voltage based on compressor type
          if(inverterCommProcParams.compressorComm.compressorType == COMPRESSOR_TYPE_EKI)
          {
            dcbusVoltage = inverterCommProcParams.compressorComm.EKIPackets.InverterDCbusVoltage;
          }
          else
          {
            dcbusVoltage = inverterCommProcParams.compressorComm.Embc_Jiax_Packets.BusVoltage;
          }
          
          // Track min/max every minute
          if(dcbusVoltage < compDcbusMin)
            compDcbusMin = dcbusVoltage;
          if(dcbusVoltage > compDcbusMax)
            compDcbusMax = dcbusVoltage;
          
          // Write min/max (divided by 2 to fit in byte)
          *(mercekDataPtr + MercekDataTable[i].offset) = (unsigned char)(compDcbusMin / 2);  // Min (divided by 2)
          *(mercekDataPtr + MercekDataTable[i].offset + 1) = (unsigned char)(compDcbusMax / 2);  // Max (divided by 2)
        }
        break;
        
      case COMP_REALSPEED:  // V03R19: Compressor real speed average
        MercekDataTable[i].period = EVERY_MINUTES;
        {
          unsigned short realSpeed = 0;
          
          // Get real speed based on compressor type
          if(inverterCommProcParams.compressorComm.compressorType == COMPRESSOR_TYPE_EKI)
          {
            realSpeed = inverterCommProcParams.compressorComm.EKIPackets.MotorMechanicalSpeed;
          }
          else
          {
            realSpeed = inverterCommProcParams.compressorComm.Embc_Jiax_Packets.RealSpeed;
          }
          
          // Only accumulate if there is a valid value
          if(realSpeed > 0)
          {
            compRealSpeedSum += realSpeed;
            compRealSpeedCount++;
          }
          
          // Calculate and write average divided by 30
          if(compRealSpeedCount > 0)
          {
            unsigned short avgSpeed = (unsigned short)(compRealSpeedSum / compRealSpeedCount);
            *(mercekDataPtr + MercekDataTable[i].offset) = (unsigned char)(avgSpeed / 30);
          }
        }
        break;
        
      case CA0_DEFROST_DURATION:
        MercekDataTable[i].period = EVERY_MINUTES;
        if(refrigerator_R.Cabin[0].states.Work == _Work_Defrost && (!(defrostStatus & 0x01)))
        {
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
        }
        
        if(refrigerator_R.Cabin[0].states.Work == _Work_Defrost)
        {
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
          defrostStatus |= 0x01;
        }
        else
          defrostStatus &= ~(0x01);
        break;
        
      case CA0_DEFROST_REASON:
        MercekDataTable[i].period = EVERY_MINUTES;
        if(refrigerator_R.Cabin[0].states.Work == _Work_Defrost)
        {
          if(isDefrostIn(refrigerator_R.Cabin[0].states.Defrost))
            *(mercekDataPtr + MercekDataTable[i].offset) = refrigerator_R.Cabin[0].states.Defrost;
        }
        break;
        
      case CA1_DEFROST_DURATION:
        MercekDataTable[i].period = EVERY_MINUTES;
        if(refrigerator_R.Cabin[1].states.Work == _Work_Defrost && (!(defrostStatus & 0x02)))
        {
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0xC0) != 0xC0)
            *(mercekDataPtr + MercekDataTable[i].offset) += 0x40;
        }
        
        if(refrigerator_R.Cabin[1].states.Work == _Work_Defrost)
        {            
          if((*(mercekDataPtr + MercekDataTable[i].offset) & 0x3F)<63)
            *(mercekDataPtr + MercekDataTable[i].offset) += 1;
          defrostStatus |= 0x02;
        }
        else
          defrostStatus &= ~(0x02);
        break;
        
      case CA1_DEFROST_REASON:
        MercekDataTable[i].period = EVERY_MINUTES;
        if(refrigerator_R.Cabin[1].states.Work == _Work_Defrost)
        {
          if(isDefrostIn(refrigerator_R.Cabin[1].states.Defrost))
            *(mercekDataPtr + MercekDataTable[i].offset) = refrigerator_R.Cabin[1].states.Defrost;
        }
        break;
        
      case CA0_TEMPERATURE:
        MercekDataTable[i].period = EVERY_MINUTES;
        if(!(MercekData.DataCount % 6))
        {
          byteNo = MercekData.DataCount / 6;
          if(((byteNo << 1) + 1) < CAx_TEMPERATURE_SIZE)  // Buffer overflow protection
          {
            *(mercekDataPtr + MercekDataTable[i].offset + (byteNo << 1)) = (refrigerator_R.Cabin[0].components.airTempSensor.valueInCExt >> 2);
            *(mercekDataPtr + MercekDataTable[i].offset + ((byteNo << 1) + 1)) = (refrigerator_R.Cabin[0].components.evaTempSensor.valueInCExt >> 2);
          }
        }
        break;
        
      case CA1_TEMPERATURE:
        MercekDataTable[i].period = EVERY_MINUTES;
        if(!(MercekData.DataCount % 6))
        {
          byteNo = MercekData.DataCount / 6;
          if(((byteNo << 1) + 1) < CAx_TEMPERATURE_SIZE)  // Buffer overflow protection
          {
            *(mercekDataPtr + MercekDataTable[i].offset + (byteNo << 1)) = (refrigerator_R.Cabin[1].components.airTempSensor.valueInCExt >> 2);
            *(mercekDataPtr + MercekDataTable[i].offset + ((byteNo << 1) + 1)) = (refrigerator_R.Cabin[1].components.evaTempSensor.valueInCExt >> 2);
          }
        }
        break;
        
      case AMBIENT:
        MercekDataTable[i].period = EVERY_MINUTES;
        
        for (unsigned char s = 0; s < MercekDataTable[i].dataSize; s++)
        {
          switch (s)
          {
          case 0:
            *(mercekDataPtr + MercekDataTable[i].offset + s) = refrigerator_R.Cooler.parameters.ambientType & 0x7F;
            
            if(refrigerator_R.Cooler.components.ambientTempSensor.error)
              *(mercekDataPtr + MercekDataTable[i].offset + s) |= 0x80;
            break;
            
          case 1:
            avgSens = *(mercekDataPtr + MercekDataTable[i].offset + s);
            calcSens = (signed short)avgSens * (signed short)MercekData.DataCount;
            calcSens = calcSens + (refrigerator_R.Cooler.components.ambientTempSensor.valueInCExt >> 2);
            avgSens = (signed char)(calcSens/(MercekData.DataCount + 1));
            *(mercekDataPtr + MercekDataTable[i].offset + s) = (signed char) avgSens;
            break;
            
          case 2:
            *(mercekDataPtr + MercekDataTable[i].offset + s) = refrigerator_R.Cooler.parameters.humidityType & 0x7F;
            
            if(refrigerator_R.Cooler.components.humiditySensor.error)
              *(mercekDataPtr + MercekDataTable[i].offset + s) |= 0x80;
            break;
            
          case 3:
            avgHum = *(mercekDataPtr + MercekDataTable[i].offset + s);
            calcHum = (unsigned short)(avgHum) * MercekData.DataCount;
            calcHum = calcHum + refrigerator_R.Cooler.components.humiditySensor.humidityValue;
            avgHum = (unsigned char)(calcHum / (MercekData.DataCount + 1));
            *(mercekDataPtr + MercekDataTable[i].offset + s) = avgHum;
            break;
          }
        }
        break;
        
      case CONNECTION:
        MercekDataTable[i].period = EVERY_SECONDS;
        for (unsigned char s = 0; s < MercekDataTable[i].dataSize; s++)
        {
          switch (s)
          {
          case 0:
            if(WiFiVariables.Situation != WIFI_OFF)
            {
              if((WiFiVariables.Situation != WIFI_CONNECTED) && (connectionStatus == WIFI_CONNECTED))
              {
                if(*(mercekDataPtr + MercekDataTable[i].offset + s) < 0xFF)
                  *(mercekDataPtr + MercekDataTable[i].offset + s) += 1;
              }
            }
            
            connectionStatus = WiFiVariables.Situation;
            break;
            
          case 1:
            if(WiFiVariables.Situation == WIFI_CONNECTED)
            {
              if(connectionTimerNB < 0xFFFF)  // Overflow protection
                connectionTimerNB++;
              
              if(connectionTimerNB < 3 * 60)
              {
                *(mercekDataPtr + MercekDataTable[i].offset + s) = ((connectionTimerNB / 60) << 6) + (connectionTimerNB % 60);
              }
              else
              {
                *(mercekDataPtr + MercekDataTable[i].offset + s) = 0xC0;
                if(connectionTimerNB >= 4 * 60)
                  *(mercekDataPtr + MercekDataTable[i].offset + s) += ((connectionTimerNB / 60) - 3);
              }
            }
            else
            {
              connectionTimerNB = 0;  // Reset timer when disconnected
            }
            break;
          }
        }
        break;
        
      case ERROR_CODE:
        MercekDataTable[i].period = EVERY_SECONDS;
        memset(mercekDataPtr + MercekDataTable[i].offset, 0, ERROR_CODE_SIZE); // reset area
        if (refrigerator_R.Values.errors.errorsNumber)
        {
          for (unsigned char j = 0; j < refrigerator_R.Values.errors.errorsNumber; j++)
          {
            if(refrigerator_R.Values.errors.errorsTable[j] < ERROR_CODE_SIZE * 8)  // Buffer overflow protection
            {
              byteNo = refrigerator_R.Values.errors.errorsTable[j] / 8;
              bitNo = refrigerator_R.Values.errors.errorsTable[j] % 8;
              *(mercekDataPtr + MercekDataTable[i].offset + byteNo) |= (1 << bitNo);
            }
          }
        }
        break;
        
      case DATA_COUNT:
        MercekDataTable[i].period = EVERY_MINUTES;
        if(*(mercekDataPtr + MercekDataTable[i].offset) < 0xFF)
        {
          *(mercekDataPtr + MercekDataTable[i].offset) += 1;
        }
        
        // Retry mechanism: If NB didn't get data
        if(MercekData.SendData != 0)
        {
          mercekSendRetryTimer++;
          if(mercekSendRetryTimer >= 2)  // each 2 minute , try again
          {
            mercekSendRetryTimer = 0;
            MercekData.SendData = SEND_MERCEK_COMMAND;  // send again
            mercekSendRetryCount++;
            if(mercekSendRetryCount >= 3)  // after 3 times, break it, continue gather data
            {
              MercekData.SendData = 0;
              mercekSendRetryCount = 0;
              fResetMercekCounters();  // delete data, start to collect new cycle
            }
          }
        }
        else
        {
          mercekSendRetryTimer = 0;
          mercekSendRetryCount = 0;
        }
        
        if(*(mercekDataPtr + MercekDataTable[i].offset) >= MERCEK_SEND_PERIOD) // 1hour timer to send periodically
        {
          if (MercekData.SendData == 0) // if anything to send
          {
            *(mercekDataPtr + MercekDataTable[i].offset) = 0; // reset 1 hour timer
            MercekData.SendingReason = MERCEK_REASON_PERIODIC;
            MercekData.SendingReason |= (refrigerator_R.Values.errors.errorsNumber) ? MERCEK_REASON_ERROR : 0x00;
            MercekData.MercekCount++;
            MercekData.SendData = SEND_MERCEK_COMMAND;        // Trigger to Safir: Mercek Data is Ready
            LastSendF = MercekData.SendData; // Double Check for Mercek is taken by Safir
            SecureMercekSendTime = true;
            // NOTE: memset and counter resets moved to fResetMercekCounters(), called after mercek data is sent
          }

        }
        break;
        
      case MERCEK_COUNT:
        MercekDataTable[i].period = EVERY_MINUTES;
        break;
        
      case EOF_MERCEK_DATA:
        MercekDataTable[i].period = ONE_TIME;
        for (unsigned char s = 0; s < MercekDataTable[i].dataSize; s++)
        {
          *(mercekDataPtr + MercekDataTable[i].offset + s) = 0;
        }
      default:
        break;
      }
    }
  }
}

#define OFFLINE_MERCEK_MAXIMUM_OPEN_TIME (60000) // 60sc
#define OFFLINE_MERCEK_MAXIMUM_WRITE_TIME (15000) // 15sc
/**
* @brief Fills mercek array and handles its send sequence
* */
void fHandleMercekParameters(void)
{
  static unsigned char mercekT1MinCnt = 0;
  
  if(!oneTimer)
  {
    MercekUpdateData(ONE_TIME);
    oneTimer = 1;
  }
  
  MercekUpdateData(EVERY_SECONDS);
  mercekT1MinCnt++;
  if(mercekT1MinCnt == 60)
  {
    mercekT1MinCnt = 0;
    MercekUpdateData(EVERY_MINUTES);
  }
}

//////////////////// READ ///////////////////////////
#define RC_ACTIVATED_AND_NONSELECTABLE (2)
#define RC_SELECTABLE (6)

typedef union remoteControlWarningFlags_ {
  struct
  {
    uint8_t doorOpen : 1;
    uint8_t noWater : 1;
    uint8_t highRisk : 1;
    uint8_t addDetergent : 1;
    uint8_t addSoftener : 1;
    uint8_t cannotPause1 : 1;
    uint8_t cannotPause2 : 1;
    uint8_t changeMicroFiberFilter : 1;
  } bits;
  uint8_t all;
} rcWarnings;

typedef union remoteControlInfoFlags_ {
  struct
  {
    uint8_t programSupportsAWash : 1;
    uint8_t aWashInUse : 1;
    uint8_t reserved : 6;
  } bits;
  uint8_t all;
} rcInfo;

typedef union remoteControlWashingInfo_ {
  struct
  {
    uint8_t userCanAddGarment : 1;
    uint8_t testIsOnProcess : 1;
    uint8_t safetyIsOnProcess : 1;
    uint8_t countdownIsActive : 1;
    uint8_t reserved : 5;
  } bits;
  uint8_t all;
} rcWashingInfo;

void rcRequest(remotecontrolRequests request, uint8_t val)
{
  uint8_t sendPack[2];
  
  sendPack[0] = request;
  sendPack[1] = val;
  
  eventPost(eProcessRemoteControlDriver, eRCDEventHandleRcRequest, &sendPack, sizeof(sendPack));
}

void rcSendConsumption(uint32_t consumptionValue, uint32_t timeInMinute, teConsumptionTypes consumptionType)
{
  if (consumptionType == eConsumptionType_Energy_1)
  {
    tsConsumptionData consumptionEnergyData;
    consumptionEnergyData.type = eConsumptionType_Energy_1;
    consumptionEnergyData.totalConsumption = consumptionValue;
    consumptionEnergyData.totalDuration = timeInMinute;
    consumptionEnergyData.measurementUnit = eMeasurementUnitEnergySensorUnit1;
    eventPost(eProcessRemoteControlDriver, eRCDEventSendConsumption, &consumptionEnergyData, sizeof(consumptionEnergyData));
  }
  //	else if (consumptionType == eConsumptionType_Water_1)
  //	{
  //		tsConsumptionData consumptionWaterData;
  //		consumptionWaterData.type = eConsumptionType_Water_1;
  //		consumptionWaterData.totalConsumption = consumptionValue;
  //		consumptionWaterData.totalDuration = timeInMinute;
  //		consumptionWaterData.measurementUnit = eMeasurementUnitWaterConsuptionUnit2;
  //		eventPost(eProcessRemoteControlDriver, eRCDEventSendConsumption, &consumptionWaterData, sizeof(consumptionWaterData));
  //	}
}

/// @brief Updates the cycle count info inside the WFA
void rcUpdateCycleCountImmediately(void)
{
  //	uint8_t emptyData = 0;
  //	eventPost(eProcessRemoteControlDriver, eRCDEventImmediateCycleCountUpdate, &emptyData, sizeof(emptyData));
}

#define RC_RESET_TIMEOUT_INITIAL (10000) // 10sc
#define RC_RESET_TIMEOUT_SAFIRRESPONSE (35000) //35sc

void rcStartFactoryReset(void)
{
  remoteControlResetState = eRcFactoryResetInitiate;
  
  timerGenericStart(&remoteControlResetTimer, RC_RESET_TIMEOUT_INITIAL);
}

uint8_t fHandleFactoryReset(uint8_t rcCommand)
{
  //	if ((remoteControlResetState == eRcFactoryResetInitiate) &&
  //		(rcCommand == eRcFactoryResetSafirResponse))
  //	{
  //		remoteControlResetState = eRcFactoryResetCloud;
  //
  //		timerGenericStart(&remoteControlResetTimer, RC_RESET_TIMEOUT_SAFIRRESPONSE);
  //	}
  //	else if ((remoteControlResetState == eRcFactoryResetCloud) &&
  //			 ((rcCommand == eRcFactoryResetFinished) || (rcCommand == eRcFactoryResetCloudFailed)))
  //	{
  //		programFlag.bits.Onboarding_done = FALSE;
  //		remoteControlResetState = eRcFactoryResetNone;
  //		api_UI_Update_Buzzer_Sound_Buffer(SOUND_INACTIVE); //baska bir melodi istenir mi?
  //		rcRequest(eRcRequestStopBroadcasting, programFlag.bits.Onboarding_done);
  //		rcFlag.bits.BFSW_REMOTE_CONTROL = FALSE;
  //
  //		secureMercekVars.factoryResetSuccessCnt++;
  //
  //		resetAdaptiveWash();
  //
  //		timerGenericStop(&remoteControlResetTimer);
  //	}
  //	else if ((remoteControlResetState == eRcFactoryResetCloud) &&
  //			 ((rcCommand == eRcFactoryResetSafirFailed) || (rcCommand == eRcFactoryResetUnkownError)))
  //	{
  //		remoteControlResetState = eRcFactoryResetNone;
  //		api_UI_Update_Buzzer_Sound_Buffer(SOUND_WARNING);
  //
  //		secureMercekVars.factoryResetFailCnt++;
  //
  //		timerGenericStop(&remoteControlResetTimer);
  //	}
  //
  //	return remoteControlResetState;
  return 0;
}

void fHandleFactoryResetTimeout(void)
{
  if ((remoteControlResetTimer._cnt != 0) ||
      (remoteControlResetState == eRcFactoryResetNone))
    return;
  
  switch (remoteControlResetState)
  {
  case eRcFactoryResetInitiate:
    {
      sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
      remoteControlResetState = eRcFactoryResetNone;
      
      secureMercekVars.data5++;
      break;
    }
    
  case eRcFactoryResetCloud:
    {
      //programFlag.bits.Onboarding_done = FALSE;
      remoteControlResetState = eRcFactoryResetNone;
      //timerEventStop(&params->teFactoryResetTimeout);
      sysBuzzerCmd(eSysBuzzerPlayCloseMelody);
      //rcRequest(eRcRequestStopBroadcasting, programFlag.bits.Onboarding_done);
      
      secureMercekVars.data5++;
      break;
    }
    
  default:
    break;
  }
}

void fResetMercekCounters(void)
{
  unsigned char *mercekDataPtr = (unsigned char *)&MercekData;
  
  // Clear mercek data from WORKMODE onwards (preserve MercekCount)
  memset((mercekDataPtr + WORKMODE), 0, MERCEK_COUNT-WORKMODE);
  
  // Reset static counters for new period
  iceMachBoxFull_sec = 0;
  iceMachFanOn_sec = 0;
  doorOpenTimer[0] = 0;
  doorOpenTimer[1] = 0;
  connectionTimerNB = 0;
  compPowerSum = 0;      // V03R18: Reset power accumulator
  compPowerCount = 0;    // V03R18: Reset power count
  compDcbusMin = 0xFFFF; // V03R18: Reset DC bus min
  compDcbusMax = 0;      // V03R18: Reset DC bus max
  compRealSpeedSum = 0;  // V03R19: Reset real speed accumulator
  compRealSpeedCount = 0; // V03R19: Reset real speed count
  energyWattMinuteSum = 0; // V03R21: Reset energy accumulator
  oneTimer = 0;
}

void Connectivity_ResetErrors(void)
{
	if (wifiError_B1 || wifiError_B2)
	{
		wifiError_B1 = 0;
		wifiError_B2 = 0;
		refrigerator_R.E2.wifiErrors = wifiError_B2;
		WiFiVariables.NBErrors0 = 0;
		connectionTimerNB = 0;
		refrigerator_R.ui.dspPacket.funcs.ledAlert = false;
	}
}

#else
void Connectivity_ResetErrors(void){}
void toggleWifiState(wifi_states_t state){}
void setWifiState(unsigned char state){}
void Connectivity_Reset(void){}
void rcRequest(remotecontrolRequests request, uint8_t val){}
#endif // CONNECTIVITY
 
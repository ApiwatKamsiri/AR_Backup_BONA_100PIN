/** @file       remote_control_driver.c
*  @brief      REMOTE_CONTROL_DRIVER application program file
*  @copyright  (c) 2020-Arcelik - All Rights Reserved
*              Permission to use, reproduce, copy, prepare derivative works,
*              modify, distribute, perform, display or sell this software and/or
*              its documentation for any purpose is prohibited without the express
*              written consent of Arcelik A.S.
*  @author     Umit Sahin
*  @date       21.02.2020
*/
#define FILE_REMOTE_CONTROL_DRIVER_C

#include "remote_control_driver.h"
#include "remote_control.h"
#if (CONNECTIVITY != NON_CONNECTED)
#include "..\..\refrigerator\rf_configs.h"
#include "..\..\refrigerator\rf_ram.h"
#define DEBUG_FILE_NAME "RCD"
#define DEBUG_RCOS_RCD BIT(0)
DEBUG_START(DEBUG_RCOS_RCD)

// #define RCOS_DEBUG_PORT_

/**
*  @addtogroup REMOTE_CONTROL_DRIVER
*  @{
*/

#if (BLUETOOTH_AVAIABLE == 1)
#include "source/driver/driver_board_linker.h"
extern tsDriverBoardLinkerParams driverBoardLinkerParams; // PSoC 6  M0'a driver board bilgisi gondermek icin
#endif

static PROCESS_HANDLER_PROTO(remoteControlDriverHandler);
void fHandleFactoryResetTimeout(void);
void setConnectivityError(uint8_t state);
void Connectivity_T1sec(void);
void fHandleWiFiIdle(void);
void fHandleReadVariables(void);
void initializeRemoteControlDriver(tsProcess *process);
void fHandleConnectionState(tsProcess *process);
tsTimerGeneric connectivitiy1Sec = TIMER_GENERIC_INIT();

#define CONNECTIVITY_1000MS (1000) // 1 seconds

/// @brief  Initialization function of remoteControlDriver
PROCESS_INIT_PROTO(remoteControlDriverInit)
{
  tsRemoteControlDriverParams *params = process->parameters;  
  PROCESS_STATE_CHANGE(process, remoteControlDriverHandler);
  initializeRemoteControlDriver(process);
  process->initialized = 1; // If process needs other checks, set this another time
  timerEventStart(&params->tev, 1100);
  timerGenericStart(&connectivitiy1Sec, CONNECTIVITY_1000MS);
}

/// @brief  Deinitialization function of remoteControlDriver
PROCESS_DEINIT_PROTO(remoteControlDriverDeinit)
{
  tsRemoteControlDriverParams *params = process->parameters;  
  UNUSED(params); // REMOVE IF USED  
  PROCESS_STATE_CHANGE(process, NULL);
  timerEventStop(&params->tev);
  process->initialized = 0; // If process needs other cchecks, clear this another time
}

#define RC_TEST_MODE (0xAA)

/// @brief  Sets one-time-read remote control variables
void initializeRemoteControlDriver(tsProcess *process)
{
  tsRemoteControlDriverParams *params = process->parameters;  
  unsigned long long code = refrigerator_F.Connected.definitions.stockCode;
  for (uint8_t i = 0; i < 5; i++)
  {
    WiFiVariables.StockCode[i] = code % 256;
    code = code >> 8;
  }
  WiFiVariables.MachineID = 2;  
  // Array Sizes
  WiFiVariables.ArraySize = params->wfaSize;
  WiFiVariables.MercekSize = params->mercekSize;
  WiFiVariables.Aux2 = 64;
  WiFiVariables.DoorAlarmTime = 5;
  
  initializeSecureMercekParams();
}

/// @brief  Event handler function of remoteControlDriver
static PROCESS_HANDLER_PROTO(remoteControlDriverHandler)
{
  tsRemoteControlDriverParams *params = process->parameters;
  
  if ((eventCurrent.event != eRCDEventIdle) &&
      (eventCurrent.event != eRCDEventHandleRcRequest))
  {
    rcFlag.bits.noNewRemoteMessage = FALSE;
  }
  switch (eventCurrent.event)
  {
    
  case eRCDEventHandleRcRequest:
    {
      switch(eventData[0])
      {
      case eRcRequestStartOnboarding:
        {          
          eventPost(eProcessCcmManager, eCcmStartOnboarding, NULL, 0);
          WiFiVariables.Situation = WIFI_AP_ON;
        }
        break;
        
      case eRcRequestCheckConnection:
        {
          eventPost(eProcessCcmManager, eCcmCheckHomewhizConnected, NULL, 0);
          eventPost(eProcessCcmManager, eCcmSafirCheckCommStarted, &eventData[1], sizeof(eventData[1]));
          // TODO 24 11 04
//          if(WiFiVariables.Situation == WIFI_OFF)
//          {
//            WiFiVariables.Situation = WIFI_ON;
//          }
        }
        break;
        
      case eRcRequestStopBroadcasting:
        {
          eventPost(eProcessCcmManager, eCcmHomewhizStopBroadcasting, &eventData[1], sizeof(eventData[1]));
          if ((eventData[1]) == FALSE)
          {
            WiFiVariables.Situation = WIFI_OFF;
          }
          else
          {
            WiFiVariables.Situation = WIFI_ON;
          }
        }
        break;
        
      case eRcRequestStartTestMode:
        {
          rcFlag.bits.testModeSuccessful = FALSE;          
          WiFiVariables.TestMode = RC_TEST_MODE;
          *params->connState = CONN_SUB_STATE_UNDEFINED;
          eventPost(eProcessCcmManager, eCcmSafirCheckCommStarted, &eventData[1], sizeof(eventData[1]));
        }
        break;
        
      case eRcRequestStopTestMode:
        {
          WiFiVariables.TestMode = 0x00;
        }
        break;
        
      case eRcRequestCloseConnection:
        {
          WiFiVariables.Situation = WIFI_OFF;
          eventPost(eProcessCcmManager, eCcmHomewhizStopBroadcasting, &eventData[1], sizeof(eventData[1]));//Safir kartin statini non connected yapmak icin bu event gonderilmeli.
        }
        break;
      }
    }
    break;
  case eRCDEventOnboardingState:
    break;
  case eRCDEventSafirComState:
    {
      switch(eventData[0])
      {
      case eCommFail:
        {
          if(WiFiVariables.TestMode != RC_TEST_MODE) // test modda ccm_safir fail event'i gonderdigi icin bu modu haric tutmak zorunda kaldik.
            setConnectivityError(TRUE);
        }
        break;
      case eCommStarted:
        {
          setConnectivityError(FALSE);
        }
        break;
      }
    }
    break;
    
  case eRCDEventServiceButton:
    WiFiVariables.ServiceButton = fHandleSetServiceButton(eventData[0]);
    break;     
  case eRCDEventMachineID:
    break;     
  case eRCDEventServiceButtonShortTimeOut:
    WiFiVariables.ServiceButtonShortTimeOut = fHandleSetServiceButtonShortTimeOut(eventData[0]);
    break;     
  case eRCDEventServiceButtonLongTimeOut:
    WiFiVariables.ServiceButtonLongTimeOut = fHandleSetServiceButtonLongTimeOut(eventData[0]);
    break;     
  case eRCDEventMeasurementInformation:
    break;     
  case eRCDEventPassword:
    //fHandleSetPassword(eventData[0]);
    break;     
  case eRCDEventNBVersion:
    fHandleSetNBVersion(eventData[0]);
    break;       
  case eRCDEventNBRevision:
    fHandleSetNBRevision(eventData[0]);
    break;     
  case eRCDEventWGVersion:
    fHandleSetWGVersion(eventData[0]);
    break;     
  case eRCDEventDateMinute:
    WiFiVariables.DateMinute = fHandleSetDateMinute(eventData[0]);
    break;     
  case eRCDEventDateHour:
    WiFiVariables.DateHour = fHandleSetDateHour(eventData[0]);
    break;     
  case eRCDEventDateDay:
    WiFiVariables.DateDay = fHandleSetDateDay(eventData[0]);
    break;     
  case eRCDEventDateMonth:
    WiFiVariables.DateMonth = fHandleSetDateMonth(eventData[0]);
    break;     
  case eRCDEventDateYear:
    WiFiVariables.DateYear = fHandleSetDateYear(eventData[0]);
    break;     
  case eRCDEventMercekReadDone:
    MercekData.SendData = 0;
    fResetMercekCounters();  // Reset mercek data and counters after successful send
    break;     
  case eRCDEventMercekSize:
    //fHandleSetMercekSize(eventData[0]);
    break;     
  case eRCDEventArraySize:
    //fHandleSetArraySize(eventData[0]);  
    break;     
  case eRCDEventSituation:
    WiFiVariables.Situation = fHandleSetSituation(eventData[0]);
    break;     
  case eRCDEventNBErrors0:
    break;     
  case eRCDEventMercekErrorLenght:
    break;     
  case eRCDEventTestMode:
    break;     
  case eRCDEventEndMesg:
    break;     
  case eRCDEventResetControl:
    WiFiVariables.ResetControl = fHandleSetResetControl(eventData[0]);
    comingData = WiFiVariables.ResetControl;
    break;     
  case eRCDEventAux2:
    WiFiVariables.Aux2 = fHandleSetAux2(eventData[0]);
    break;     
  case eRCDEventOTA:
    WiFiVariables.OTA = fHandleSetOTA(eventData[0]);
    break;     
  case eRCDEventCabinTempType:
    WiFiVariables.CabinTempType = fHandleSetCabinTempType(eventData[0]);
    break;     
  case eRCDEventCabinTempSet0:
    WiFiVariables.CabinTempSet[0] = fHandleSetCabinTempSet(eventData[0],0);
    break;
  case eRCDEventCabinTempSet1:
    WiFiVariables.CabinTempSet[1] = fHandleSetCabinTempSet(eventData[0],1);
    break;     
  case eRCDEventCabinTempSet2:
    WiFiVariables.CabinTempSet[2] = fHandleSetCabinTempSet(eventData[0],2);
    break;     
  case eRCDEventCabinTempSet3:
    WiFiVariables.CabinTempSet[3] = fHandleSetCabinTempSet(eventData[0],3);
    break;     
  case eRCDEventCabinTempSet4:
    WiFiVariables.CabinTempSet[4] = fHandleSetCabinTempSet(eventData[0],4);
    break;          
  case eRCDEventCabinFunc0:
    WiFiVariables.CabinFunc[0] = fHandleSetCabinFunc(eventData[0],0);
    break;          
  case eRCDEventCabinFunc1:
    WiFiVariables.CabinFunc[1] = fHandleSetCabinFunc(eventData[0],1);
    break;         
  case eRCDEventCabinFunc2:
    WiFiVariables.CabinFunc[2] = fHandleSetCabinFunc(eventData[0],2);
    break;         
  case eRCDEventCabinFunc3:
    WiFiVariables.CabinFunc[3] = fHandleSetCabinFunc(eventData[0],3);
    break;         
  case eRCDEventCabinFunc4:
    WiFiVariables.CabinFunc[4] = fHandleSetCabinFunc(eventData[0],4);
    break;    
  case eRCDEventIceMachineSet:
    WiFiVariables.IceMachineSet = fHandleSetIceMachineSet(eventData[0]);
    break;     
  case eRCDEventDispenser:
    //fHandleSetDispenser(eventData[0]);
    break;     
  case eRCDEventIceSource:
    WiFiVariables.IceSource = fHandleSetIceSource(eventData[0]);
    break;     
  case eRCDEventServiceButtonCopy:
    //fHandleSetServiceButtonCopy(eventData[0]);
    break;     
  case eRCDEventIceMachine:
    //fHandleSetIceMachine(eventData[0]);
    break;     
  case eRCDEventDisplaySet:
    WiFiVariables.DisplaySet = fHandleSetDisplaySet(eventData[0]);
    break;     
  case eRCDEventVacation:
    WiFiVariables.Vacation = fHandleSetVacation(eventData[0]);
    break;     
  case eRCDEventEcoFuzzy:
    WiFiVariables.EcoFuzzy = fHandleSetEcoFuzzy(eventData[0]);
    break;     
  case eRCDEventSabbath:
    WiFiVariables.Sabbath = fHandleSetSabbath(eventData[0]);
    break;     
  case eRCDEventConnectedFeatures:
    //fHandleSetConnectedFeatures(eventData[0]);
    break;     
  case eRCDEventVioled:
    WiFiVariables.Violed = fHandleSetVioled(eventData[0]);
    break;     
  case eRCDEventWarnings:
    //fHandleSetWarnings(eventData[0]);
    break;     
  case eRCDEventError:
    WiFiVariables.Error = fHandleSetError(eventData[0]);
    break;     
  case eRCDEventFilter:
    //fHandleSetFilter(eventData[0]);
    break;     
  case eRCDEventAlarmLevel:
    //fHandleSetAlarmLevel(eventData[0]);
    break;     
  case eRCDEventRefrigeratorAux3:
    //fHandleSetRefrigeratorAux3(eventData[0]);
    break;     
  case eRCDEventRefrigeratorAux4:
    //fHandleSetRefrigeratorAux4(eventData[0]);
    break;     
  case eRCDEventComponents1:
    //fHandleSetComponents1(eventData[0]);
    break;     
  case eRCDEventComponents2:
    //fHandleSetComponents2(eventData[0]);
    break;      
  case eRCDEventDailyHour:
    //fHandleSetDailyHour(eventData[0]);
    break;     
  case eRCDEventDailyMin:
    //fHandleSetDailyMin(eventData[0]);
    break;     
  case eRCDEventSpecialTimes_1:
    //fHandleSetSpecialTimes_1(eventData[0]);
    break;     
  case eRCDEventShoppingMode:
    WiFiVariables.ShoppingMode = fHandleSetShoppingMode(eventData[0]);
    break;     
  case eRCDEventSpecialTimes_3:
    //fHandleSetSpecialTimes_3(eventData[0]);
    break;     
  case eRCDEventSpecialFunctions:
    WiFiVariables.SpecialFunctions = fHandleSetSpecialFunctions(eventData[0]);
    break;     
  case eRCDEventHandleLight:
#ifdef HANDLELIGHT
    WiFiVariables.HandleLight = fHandleSetHandleLight(eventData[0]);
#endif
    break;     
  case eRCDEventDoorAlarmTime:
    WiFiVariables.DoorAlarmTime = fHandleSetDoorAlarmTime(eventData[0]);
    break;     
  case eRCDEventAIMode:
    WiFiVariables.AIMode = fHandleSetAIMode(eventData[0]);
    break;     
  case eRCDEventCheapSelectMin_3:
    //fHandleSetCheapSelectMin_3(eventData[0]);
    break;     
  case eRCDEventCheapSelectDuration_3:
    //fHandleSetCheapSelectDuration_3(eventData[0]);
    break;     
  case eRCDEventCheapSelectFlag:
    //fHandleSetCheapSelectFlag(eventData[0]);
    break;     
  case eRCDEventCheapSelectConfig:
    //fHandleSetCheapSelectConfig(eventData[0]);
    break;     
  case eRCDEventCheapSelectCountDown:
    //fHandleSetCheapSelectCountDown(eventData[0]);
    break;     
  case eRCDEventDailyDay:
    //fHandleSetDailyDay(eventData[0]);
    break;     
  case eRCDEventDailyMonth:
    //fHandleSetDailyMonth(eventData[0]);
    break;     
  case eRCDEventDailyYear:
    //fHandleSetDailyYear(eventData[0]);
    break;     
  case eRCDEventhTE_HighestTempRead:
    //fHandleSethTE_HighestTempRead(eventData[0]);
    break;     
  case eRCDEventhTE_WarningReason:
    //fHandleSethTE_WarningReason(eventData[0]);
    break;     
  case eRCDEventeE_EnergyEfficiencyDutyFF:
    //fHandleSeteE_EnergyEfficiencyDutyFF(eventData[0]);
    break;     
  case eRCDEventMercekErrorShadow1:
    fHandleSetMercekErrorShadow1(eventData[0]);
    break;     
  case eRCDEventMercekErrorShadow2:
    //fHandleSetMercekErrorShadow2(eventData[0]);
    break;
  case eRCDEventQuickIce60:
    WiFiVariables.QuickIce60 = fHandleSetQuickIce60(eventData[0]);
    break;
  case eRCDEventRGBHandleLight:
    WiFiVariables.HandleLightIndex = fHandleSetRGBHandleLight(eventData[0]);
    break;
  case eRCDEventVacuumBox:
    WiFiVariables.VacuumBoxSet = fHandleSetVacuumBox(eventData[0]);
    break;
  case eRCDEventKeylockDspr:
    WiFiVariables.KeylockDspr = fHandleSetKeylockDspr(eventData[0]);
    break;
    
  case eRCDEventIdle:
    { // Yeni mesaj geldiginde 100ms araliklarla idle durumu kontrol edilir,
      // bu sure zafinda yeni mesaj gelmezse wifi idle islevleri saniyede bir calistirilir.
      
      fHandleConnectionState(process);
      
      if (rcFlag.bits.noNewRemoteMessage)
      {
        fHandleWiFiIdle();
        
        params->tev.post.event = eRCDEventIdle;
        timerEventStart(&params->tev, 1000);
      }
      else
      {
        rcFlag.bits.noNewRemoteMessage = TRUE;
        
        params->tev.post.event = eRCDEventIdle;
        timerEventStart(&params->tev, 100);
      }
    }
    break;
  }
}

void fHandleConnectionState(tsProcess *process)
{
  tsRemoteControlDriverParams *params = process->parameters;
#if (CONNECTIVITY == BLE_CONNECTED)
	switch (WiFiVariables.Situation)
	{
	case EV_BLE_MODE_DISABLE:
    {
      connectionState = EV_WIFI_MODE_NOT_CONNECTED;
    }
    break;
	case EV_BLE_MODE_CONNECTED:
    {
      connectionState = EV_WIFI_MODE_CONNECTED;
    }
    break;
	case EV_BLE_MODE_ENABLE:
	case EV_BLE_MODE_CONFIGURATION:
	case EV_BLE_MODE_GET_CONNECTING:
	case EV_BLE_MODE_NOT_CONNECTED:
    {
      connectionState = EV_WIFI_MODE_GET_CONNECTING;
    }
    break;
	}
#else
	switch (*params->connState)
	{
	case CONN_SUB_STATE_CONNECTING:
    {
      connectionState = EV_WIFI_MODE_GET_CONNECTING;
    }
    break;
	case CONN_SUB_STATE_CONNECTED:
    {
      rcFlag.bits.testModeSuccessful = TRUE;
      connectionState = EV_WIFI_MODE_CONNECTED;
    }
    break;
	case CONN_SUB_STATE_MDNS_FAIL:
	case CONN_SUB_STATE_FAILED:
	case CONN_SUB_STATE_UNDEFINED:
    {
      connectionState = EV_WIFI_MODE_NOT_CONNECTED;
    }
    break;
	}
  

switch (wifiState)
  {
    
  case WIFI_OFF:
    WiFiVariables.Situation = WIFI_OFF;    
    *params->connState = CONN_SUB_STATE_UNDEFINED;
    break;
    
  case WIFI_ON:
  case WIFI_CONNECTING:
    if(!WiFiVariables.TestMode)
    {
      WiFiVariables.Situation = WIFI_ON; 
    }
    if(connectionState == EV_WIFI_MODE_GET_CONNECTING)
    {      
      WiFiVariables.Situation = WIFI_CONNECTING;      
    }
    else if(connectionState == EV_WIFI_MODE_CONNECTED)
    {
      WiFiVariables.Situation = WIFI_CONNECTED;      
    }
    else if(connectionState == EV_WIFI_MODE_NOT_CONNECTED)
    {   
      WiFiVariables.Situation = WIFI_NOT_CONNECTED;
    }
    break;
    
  case WIFI_AP_ON:
    if(!WiFiVariables.TestMode)
    {
      WiFiVariables.Situation = WIFI_OFF; 
    }
    if(connectionState == EV_WIFI_MODE_GET_CONNECTING)
    {      
      WiFiVariables.Situation = WIFI_CONNECTING;      
    }
    else if(connectionState == EV_WIFI_MODE_CONNECTED)
    {
      WiFiVariables.Situation = WIFI_CONNECTED;      
    }
    else if(connectionState == EV_WIFI_MODE_NOT_CONNECTED)
    {   
      WiFiVariables.Situation = WIFI_NOT_CONNECTED;
    }
    break;
    
  default:
    ;
    break;
  }
  
#endif
}

void fHandleWiFiIdle(void)
{  
  fHandleFactoryResetTimeout();
  fHandleReadVariables();  
  fHandleMercekParameters();
}

/// @brief Handling read variables
void fHandleReadVariables(void)
// 1. parametre WiFi program sirasi, 2. Programdaki program sirasi
{

  fHandleReadServiceButton(&WiFiVariables.ServiceButton);
  fHandleReadStockCode(&WiFiVariables.StockCode[0]);
  fHandleReadMachineID(&WiFiVariables.MachineID);
  fHandleReadServiceButtonShortTimeOut(&WiFiVariables.ServiceButtonShortTimeOut);
  fHandleReadServiceButtonLongTimeOut(&WiFiVariables.ServiceButtonLongTimeOut);
  fHandleReadMeasurementInformation(&WiFiVariables.MeasurementInformation);
  //fHandleReadPassword[(&WiFiVariables.Password);
  fHandleReadNBVersion(&WiFiVariables.NBVersion);
  fHandleReadNBRevision(&WiFiVariables.NBRevision);
  fHandleReadWGVersion(&WiFiVariables.WGVersion);
  fHandleReadDateMinute(&WiFiVariables.DateMinute);
  fHandleReadDateHour(&WiFiVariables.DateHour);
  fHandleReadDateDay(&WiFiVariables.DateDay);
  fHandleReadDateMonth(&WiFiVariables.DateMonth);
  fHandleReadDateYear(&WiFiVariables.DateYear);
  fHandleReadRemoteStart(&WiFiVariables.RemoteStart);
  fHandleReadMercekSize(&WiFiVariables.MercekSize);
  fHandleReadArraySize(&WiFiVariables.ArraySize);
  fHandleReadSituation(&WiFiVariables.Situation);
  fHandleReadNBErrors0(&WiFiVariables.NBErrors0);
  fHandleReadMercekErrorLenght(&WiFiVariables.MercekErrorLenght);
  fHandleReadTestMode(&WiFiVariables.TestMode);
  fHandleReadEndMesg(&WiFiVariables.EndMesg);
  fHandleReadResetControl(&WiFiVariables.ResetControl);
  fHandleReadAux2(&WiFiVariables.Aux2);
  fHandleReadOTA(&WiFiVariables.OTA);
  fHandleReadCabinTempType(&WiFiVariables.CabinTempType);
  fHandleReadCabinTempSet(&WiFiVariables.CabinTempSet[0]);
  fHandleReadCabinFunc0(&WiFiVariables.CabinFunc[0]);
  fHandleReadCabinFunc1(&WiFiVariables.CabinFunc[1]);
  fHandleReadCabinFunc2(&WiFiVariables.CabinFunc[2]);
  fHandleReadCabinFunc3(&WiFiVariables.CabinFunc[3]);
  fHandleReadCabinFunc4(&WiFiVariables.CabinFunc[4]);
  fHandleReadIceMachineSet(&WiFiVariables.IceMachineSet);
  fHandleReadDispenser(&WiFiVariables.Dispenser);
  fHandleReadIceSource(&WiFiVariables.IceSource);
  fHandleReadServiceButtonCopy(&WiFiVariables.ServiceButtonCopy);
  //fHandleReadIceMachine(&WiFiVariables.IceMachine);
  fHandleReadDisplaySet(&WiFiVariables.DisplaySet);
  fHandleReadVacation(&WiFiVariables.Vacation);
  fHandleReadEcoFuzzy(&WiFiVariables.EcoFuzzy);
  fHandleReadSabbath(&WiFiVariables.Sabbath);
  fHandleReadConnectedFeatures(&WiFiVariables.ConnectedFeatures);
  fHandleReadVioled(&WiFiVariables.Violed);
  fHandleReadWarnings(&WiFiVariables.Warnings);
  fHandleReadError(&WiFiVariables.Error);
  fHandleReadFilter(&WiFiVariables.Filter);
  fHandleReadAlarmLevel(&WiFiVariables.AlarmLevel);
  fHandleReadRefrigeratorAux3(&WiFiVariables.RefrigeratorAux3);
  fHandleReadRefrigeratorAux4(&WiFiVariables.RefrigeratorAux4);
  fHandleReadComponents1(&WiFiVariables.Components1);
  fHandleReadComponents2(&WiFiVariables.Components2);
  fHandleReadDailyHour(&WiFiVariables.DailyHour);
  fHandleReadDailyMin(&WiFiVariables.DailyMin);
  fHandleReadSpecialTimes_1(&WiFiVariables.SpecialTimes_1);
  fHandleReadShoppingMode(&WiFiVariables.ShoppingMode);
  fHandleReadSpecialTimes_3(&WiFiVariables.SpecialTimes_3);
  fHandleReadSpecialFunctions(&WiFiVariables.SpecialFunctions);
  fHandleReadHandleLight(&WiFiVariables.HandleLight);
  fHandleReadDoorAlarmTime(&WiFiVariables.DoorAlarmTime);
  fHandleReadAIMode(&WiFiVariables.AIMode);
  fHandleReadCheapSelectMin_3(&WiFiVariables.CheapSelectMin_3);
  fHandleReadCheapSelectDuration_3(&WiFiVariables.CheapSelectDuration_3);
  fHandleReadCheapSelectFlag(&WiFiVariables.CheapSelectFlag);
  fHandleReadCheapSelectConfig(&WiFiVariables.CheapSelectConfig);
  fHandleReadCheapSelectCountDown(&WiFiVariables.CheapSelectCountDown);
  fHandleReadDailyDay(&WiFiVariables.DailyDay);
  fHandleReadDailyMonth(&WiFiVariables.DailyMonth);
  fHandleReadDailyYear(&WiFiVariables.DailyYear);
  fHandleReadhTE_HighestTempRead(&WiFiVariables.hTE_HighestTempRead);
  fHandleReadhTE_WarningReason(&WiFiVariables.hTE_WarningReason);
  fHandleReadeE_EnergyEfficiencyDutyFF(&WiFiVariables.eE_EnergyEfficiencyDutyFF);
  fHandleReadMercekErrorShadow1(&WiFiVariables.MercekErrorShadow1);
  fHandleReadMercekErrorShadow2(&WiFiVariables.MercekErrorShadow2);
  fHandleReadQuickIce60(&WiFiVariables.QuickIce60);
  fHandleReadRGBHandleLight(&WiFiVariables.HandleLightIndex);
  fHandleReadVacuumBox(&WiFiVariables.VacuumBoxSet);
  fHandleReadKeylockDspr(&WiFiVariables.KeylockDspr);
  Connectivity_T1sec();
}

void Connectivity_T1sec(void)
{
  if(connectivitiy1Sec._cnt == 0)
  {    
    timerGenericStart(&connectivitiy1Sec, CONNECTIVITY_1000MS);
    if ((WiFiVariables.Situation >= 1) 	&&
        (WiFiVariables.Situation != 255) &&
          (refrigerator_F.Connected.funcConnectedApp.connectedAppAct == true))
    {
      if (WiFiVariables.OTA != 0xAA)
      {
        if ((connectionTimer < 0xFF) && (!endofbandtest))
        {
          connectionTimer++;
          
          if (connectionTimer > 20)
          {
            wifiError_B1 = 1;
            WiFiVariables.NBErrors0 |= 0x01;
            //TX_timerCount = 26;
          }
          
          if (connectionTimer > 100)
          {
            WiFiVariables.NBErrors0 &= 0xFE;
            wifiError_B1 = 0;
            WiFiVariables.NBErrors0 |= 0x02;
            // wifiError_B2 = 1;
            // refrigerator_R.E2.wifiErrors = wifiError_B2;
            setWifiState(WIFI_ON);
            rcRequest(eRcRequestCheckConnection, TRUE);
          }
        }
      }
    }
    else
    {
      connectionTimer = 0;
      wifiError_B1 = 0;
    }
    
    if (startCntr < 0xFF)
      startCntr++;
    if ((WiFiVariables.Situation == WIFI_AP_ON) && (endofbandtest == 0))
      WifiConfigTimeOut++;
    
  }
}
#endif // CONNECTIVITY
/** @} */

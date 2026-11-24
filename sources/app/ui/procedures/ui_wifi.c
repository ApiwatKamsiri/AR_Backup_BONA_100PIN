
#include "ui_wifi.h"
#include "..\..\mw\connectivity\remote_control.h"
#include "RCOS\mw\Connectivity\ccm_safir.h"

#define WIFICONFIGPERIOD (5 * 60)

void uiWifiTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
#if (CONNECTIVITY != _NON_CONNECTED)
  tsUiRam *uiP = params->ui;
  UNUSED(consts);
  UNUSED(uiP);
  
  unsigned char wiFiLedState = WIFI_OFF;
  if (uiP->states.normal.state != _UI_NormalState_WifiConfig)
    WiFiLed.lastState = uiP->states.normal.state;
  if (WiFiVariables.Situation != WIFI_PW_SENDING)
    uiP->states.normal.state = (enUI_NormalStates)WiFiLed.lastState;
  
  if(connStateWifi == CONN_STATE_OFF)
    wiFiLedState = WIFI_OFF;
  else if(connStateWifi ==  CONN_STATE_STA)
  {
    if(WiFiVariables.Situation == WIFI_CONNECTED)      
      wiFiLedState = WIFI_CONNECTED; 
    else
      wiFiLedState = WIFI_CONNECTING;    
  }
  else if (connStateWifi == CONN_STATE_ONBOARDING)
    wiFiLedState = WIFI_AP_ON;    
  
  switch (wiFiLedState)
  {
  case WIFI_PW_SENDING:
    uiP->states.normal.state = _UI_NormalState_WifiConfig;
    WifiConfigTimeOut = 0;
    WiFiLed.counter++;
    if (WiFiLed.counter > WiFiLed.longLimit)
    {
      WiFiLed.state = !WiFiLed.state;
      WiFiLed.counter = 0;
    }
    break;
    
  case WIFI_AP_ON:
    if (WifiConfigTimeOut <= WIFICONFIGPERIOD) //Maximum 5 dk Wifi config toggle yapar.
    {
      WiFiLed.counter++;
      if (WiFiLed.counter > WiFiLed.longLimit)
      {
        WiFiLed.state = !WiFiLed.state;
        WiFiLed.counter = 0;
      }
    }
    else
    {
      setWifiState(WIFI_OFF);
      rcRequest(eRcRequestCloseConnection, FALSE);
      sysBuzzerCmd(eSysBuzzerPlayEndMelody);
      WiFiLed.state = 0;
      WifiConfigTimeOut = 0;
    }
    break;
    
  case WIFI_ON:
  case WIFI_CONNECTING:
    WifiConfigTimeOut = 0;
    WiFiLed.counter++;
    if (WiFiLed.counter > WiFiLed.shortLimit)
    {
      WiFiLed.state = !WiFiLed.state;
      WiFiLed.counter = 0;
    }
    break;
    
  case WIFI_OFF:
  case WIFI_NOT_CONNECTED:
  case WIFI_DROPPED:
    if(refrigerator_R.E2.WifiStatus == WIFI_ON || refrigerator_R.E2.WifiStatus == WIFI_CONNECTING)
    {
      WiFiLed.counter++;
      if (WiFiLed.counter > WiFiLed.shortLimit)
      {
        WiFiLed.state = !WiFiLed.state;
        WiFiLed.counter = 0;
      }
    }
    else
      WiFiLed.state = 0;
    
    WifiConfigTimeOut = 0;
    break;
    
  case WIFI_CONNECTED: //WA------>4
    WifiConfigTimeOut = 0;
    WiFiLed.state = 1;
    wifiError_B1 = 0;
    wifiError_B2 = 0;
    refrigerator_R.E2.wifiErrors = 0;
    break; 
    
  default:
    break;
  }
  if (!resetControlTimeout)
  {
    if (sendData == 3)
      uiP->wifi.notConnectedResetFlag = 1;
    uiP->wifi.resetStatus = _en_reset_default;
  }  
  else
  {
    if (startCntr <= 3)
    {
      // sysBuzzerCmd( eSysBuzzerPlayStartMelody);
      uiP->dspPacket.funcs.ledWifi = true;
      
      if (startCntr < 3)
        uiP->dspPacket.funcs.ledAlert = true;
      else
        uiP->dspPacket.funcs.ledAlert = false;
    }
    else
    {
      
      if (startCntr % 2)
        uiP->dspPacket.funcs.ledWifi = false;
      else
        uiP->dspPacket.funcs.ledWifi = true;
    }
  }
  switch (uiP->wifi.resetStatus)
  {
  case _en_reset_default:
    comingData = 0;
    sendData = 0;
    resetControlTimeout = 0;
    if (uiP->wifi.notConnectedResetFlag == 1)
    {
      setWifiState(WIFI_OFF);
      rcRequest(eRcRequestCloseConnection, FALSE);
      sysBuzzerCmd(eSysBuzzerPlayStartMelody);
      uiP->wifi.notConnectedResetFlag = 0;
      uiP->wifi.resetStatus = _en_reset_default;
    }
    break;
    
  case _en_reset_send_1:
    if (comingData == 0)
    {
      sendData = 1;
      resetControlTimeout = WIFI_RESET_TIMEOUT;
      uiP->wifi.resetStatus = _en_reset_wait_2;
    }
    break;
    
  case _en_reset_wait_2:
    if (comingData == 2)
    {
      sendData = 3;
      resetControlTimeout = WIFI_RESET_TIMEOUT;
      uiP->wifi.resetStatus = _en_reset_wait_4;
    }
    break;
    
  case _en_reset_wait_4:
    if (comingData == 4)
    {
      setWifiState(WIFI_OFF);
      rcRequest(eRcRequestCloseConnection, FALSE);
      sendData = 0;
      uiP->wifi.resetStatus = _en_reset_default;
      sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    }
    else if (comingData == 10 || comingData == 11)
    {
      //setWifiState( WIFI_OFF );
      sendData = 0;
      uiP->wifi.resetStatus = _en_reset_default;
      uiP->wifi.notConnectedResetFlag = 1;
    }
    break;
    
  case _en_reset_cancel:
    uiP->wifi.resetStatus = _en_reset_default;
    break;
    
  default:
    sendData = 0;
    resetControlTimeout = 0;
    break;
  }
#endif
}

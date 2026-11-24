/**
 ******************************************************************************
 * @file    sb->c
 * @author  ECM Software Team - EBI
 * @version V01R00
 * @date    27/04/2020
 * @brief   This file contains all functions for the ServiceButton Project.
 *****************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "ui_service_button.h"
#include "..\..\mw\connectivity\remote_control.h"

/**
 * @brief Default Time Variables.
 */
#define SERVICE_BUTTON_SHORT_TIME_OUT 15    // min
#define SERVICE_BUTTON_LONG_TIME_OUT 60     // min
#define SERVICE_BUTTON_MIN_LED_STATE_TIME 3 // sec
#define BLINK_TIME 50                       // 50*20  ms

static void serviceButtonSendMercekArray(const tsUiProcConsts *consts, tsUiProcParams *params);
static void serviceButtonOpenWifi(const tsUiProcConsts *consts, tsUiProcParams *params);
static void serviceButtonWakeUpDisplay(const tsUiProcConsts *consts, tsUiProcParams *params);

static void serviceButtonStateCheck(const tsUiProcConsts *consts, tsUiProcParams *params);
static void serviceButtonLedStateCheck(const tsUiProcConsts *consts, tsUiProcParams *params);
static void serviceButtonLedDriver(const tsUiProcConsts *consts, tsUiProcParams *params);
static void serviceButtonSetWifiArray(const tsUiProcConsts *consts, tsUiProcParams *params);

static void serviceButtonErrorCheck(const tsUiProcConsts *consts, tsUiProcParams *params, enServiceButtonStates state);
static void serviceButtonSetTimeOut(const tsUiProcConsts *consts, tsUiProcParams *params, enServiceButtonStates state);
static void serviceButtonMelodies(const tsUiProcConsts *consts, tsUiProcParams *params, enServiceButtonStates state);
static bool isServiceButtonStateChanged(const tsUiProcConsts *consts, tsUiProcParams *params, enServiceButtonStates state);

/**
 * @brief  Initialization function.
 *         Initializes the tsUiServiceButtonData values.
 * @param  None
 * @retval None
 */
void uiServiceButtonInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  UNUSED(consts);
  tsUiServiceButtonData *sb = &params->ui->serviceButton;

  sb->state = _IDLE;
  sb->statePrev = _IDLE;
  sb->timeOut.timer = SERVICE_BUTTON_SHORT_TIME_OUT;
  sb->led.state = _OFF;
  sb->led.serviceButton = _OFF;
  sb->led.blink.timer = 0;
  sb->led.stateChange.timer = 0;
  sb->sendMercekArray = false;
}

/**
 * @brief  20ms timer function.
 *         Controls the blink timer used for serviceButton Led
 * @param  None
 * @retval None
 */
void uiServiceButton20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  UNUSED(consts);
  tsUiServiceButtonData *sb = &params->ui->serviceButton;

  if (sb->led.blink.timer)
    sb->led.blink.timer--;
  else
  {
    switch (sb->led.state)
    {
    case _SLOW_BLINK:
      sb->led.blink.timer = BLINK_TIME;
      break;
    case _FAST_BLINK:
      sb->led.blink.timer = BLINK_TIME / 2;
      break;
    default:
      sb->led.blink.timer = 0;
      break;
    }
  }
}

/**
 * @brief  1sec timer function.
 *         Controls the stateChange timer
 * @param  None
 * @retval None
 */
void uiServiceButton1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  UNUSED(consts);
  tsUiServiceButtonData *sb = &params->ui->serviceButton;

  if (sb->led.stateChange.timer)
    sb->led.stateChange.timer--;
}

/**
 * @brief  1min timer function.
 *         Controls the timeOut timer
 * @param  None
 * @retval None
 */
void uiServiceButton1MinTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  UNUSED(consts);
  tsUiServiceButtonData *sb = &params->ui->serviceButton;

  if (sb->timeOut.timer)
    sb->timeOut.timer--;
}

/**
 * @brief  serviceButton Task.
 *         Controls the all serviceButton operations.
 * @param  None
 * @retval None
 */
void uiServiceButtonTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  serviceButtonStateCheck(consts, params);

  serviceButtonSetWifiArray(consts, params);

  serviceButtonLedStateCheck(consts, params);

  serviceButtonLedDriver(consts, params);
}

/**
 * @brief  serviceButton key handler function.
 *         Control state transition and call required functions
 *         when the serviceButton pressed.
 *         ServiceButton can be made state change only in state _IDLE and
 *         _REQUEST_CREATED in Normal work mode.
 * @param  *sb @ref tsUiServiceButtonData
 * @retval None
 */
void uiServiceButtonKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  tsUiServiceButtonData *sb = &params->ui->serviceButton;

  enServiceButtonStates state = sb->state;

  switch (state)
  {
  case _IDLE: // 0
    state = _PRESSED_3SEC;
    serviceButtonOpenWifi(consts, params);
    serviceButtonSendMercekArray(consts, params);
    break;

  case _PRESSED_3SEC:     // 1
  case _REQUEST_CREATING: // 2
    if (params->rf->workMode.values.workMode == _WM_Distributer)
      state = _IDLE;
    break;

  case _REQUEST_CREATED: // 3
    state = _REQUEST_CANCELLING;
    break;

  case _REQUEST_SUCCEED:               // 4
  case _DEVICE_NO_BOARDING:            // 5
  case _HOMEWHIZ_ASSOC_NO:             // 6
  case _HOMEWHIZ_CLOUD_NOT_ACCESSIBLE: // 7
  case _IT_SERVICE_CONNECT_TIMEOUT:    // 8
  case _ERROR_BLACK_LIST:              // 9
  case _NO_REGISTERED_PHONE_NUMBER:    // 10
  case _NO_ANSWER_REGISTERED_PHONE:    // 11
  case _NO_ANSWER_FOR_T_MINUTE:        // 12
  case _THERE_IS_NO_REQUEST:           // 13
  case _THERE_IS_OPEN_REQUEST:         // 14
  case _OTHER_REASONS:                 // 15
  case _REQUEST_CANCELLING:            // 16
  case _REQUEST_CANCELLING_IS_OK:      // 17
  case _REQUEST_CANCELLING_IS_NOK:     // 18
    break;

  default:
    state = _IDLE;
    break;
  }

  if (isServiceButtonStateChanged(consts, params, state))
    sb->statePrev = sb->state;

  sb->state = state;
}

/**
 * @brief  serviceButton state check function.
 *         Controls the serviceButton states and state trasitions.
 * @param  *sb @ref tsUiServiceButtonData
 * @retval None
 */
static void serviceButtonStateCheck(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  tsUiServiceButtonData *sb = &params->ui->serviceButton;

  enServiceButtonStates state = sb->state;

  switch (state)
  {
  case _IDLE: // 0
    break;

  case _PRESSED_3SEC: // 1
    if (WiFiVariables.ServiceButton == _REQUEST_CREATING)
      state = _REQUEST_CREATING;
    else
    {
      if (!sb->timeOut.timer)
        state = _NO_ANSWER_FOR_T_MINUTE;
      else
        state = _PRESSED_3SEC;
    }
    break;

  case _REQUEST_CREATING: // 2

    if (WiFiVariables.ServiceButton == _REQUEST_CREATED)
      state = _REQUEST_CREATED; // 3
    else if (WiFiVariables.ServiceButton == _DEVICE_NO_BOARDING)
      state = _DEVICE_NO_BOARDING; // 5
    else if (WiFiVariables.ServiceButton == _HOMEWHIZ_ASSOC_NO)
      state = _HOMEWHIZ_ASSOC_NO; // 6
    else if (WiFiVariables.ServiceButton == _HOMEWHIZ_CLOUD_NOT_ACCESSIBLE)
      state = _HOMEWHIZ_CLOUD_NOT_ACCESSIBLE; // 7
    else if (WiFiVariables.ServiceButton == _IT_SERVICE_CONNECT_TIMEOUT)
      state = _IT_SERVICE_CONNECT_TIMEOUT; // 8
    else if (WiFiVariables.ServiceButton == _ERROR_BLACK_LIST)
      state = _ERROR_BLACK_LIST; // 9
    else if (WiFiVariables.ServiceButton == _NO_REGISTERED_PHONE_NUMBER)
      state = _NO_REGISTERED_PHONE_NUMBER; // 10
    else if (WiFiVariables.ServiceButton == _NO_ANSWER_REGISTERED_PHONE)
      state = _NO_ANSWER_REGISTERED_PHONE; // 11
    else if (WiFiVariables.ServiceButton == _OTHER_REASONS)
      state = _OTHER_REASONS; // 15
    else
    {
      if (!sb->timeOut.timer)
        state = _NO_ANSWER_FOR_T_MINUTE;
      else
        state = _REQUEST_CREATING;
    }

    break;

  case _REQUEST_CREATED: // 3
    if (WiFiVariables.ServiceButton == _REQUEST_SUCCEED)
      state = _REQUEST_SUCCEED;
    else if (WiFiVariables.ServiceButton == _DEVICE_NO_BOARDING)
      state = _DEVICE_NO_BOARDING; // 5
    else if (WiFiVariables.ServiceButton == _HOMEWHIZ_ASSOC_NO)
      state = _HOMEWHIZ_ASSOC_NO; // 6
    else if (WiFiVariables.ServiceButton == _HOMEWHIZ_CLOUD_NOT_ACCESSIBLE)
      state = _HOMEWHIZ_CLOUD_NOT_ACCESSIBLE; // 7
    else if (WiFiVariables.ServiceButton == _IT_SERVICE_CONNECT_TIMEOUT)
      state = _IT_SERVICE_CONNECT_TIMEOUT; // 8
    else if (WiFiVariables.ServiceButton == _ERROR_BLACK_LIST)
      state = _ERROR_BLACK_LIST; // 9
    else if (WiFiVariables.ServiceButton == _NO_REGISTERED_PHONE_NUMBER)
      state = _NO_REGISTERED_PHONE_NUMBER; // 10
    else if (WiFiVariables.ServiceButton == _NO_ANSWER_REGISTERED_PHONE)
      state = _NO_ANSWER_REGISTERED_PHONE; // 11
    else if (WiFiVariables.ServiceButton == _OTHER_REASONS)
      state = _OTHER_REASONS; // 15
    else
    {
      if (!sb->timeOut.timer)
        state = _NO_ANSWER_FOR_T_MINUTE;
      else
        state = _REQUEST_CREATED;
    }

    break;

  case _REQUEST_SUCCEED:               // 4
  case _DEVICE_NO_BOARDING:            // 5
  case _HOMEWHIZ_ASSOC_NO:             // 6
  case _HOMEWHIZ_CLOUD_NOT_ACCESSIBLE: // 7
  case _IT_SERVICE_CONNECT_TIMEOUT:    // 8
  case _ERROR_BLACK_LIST:              // 9
  case _NO_REGISTERED_PHONE_NUMBER:    // 10
  case _NO_ANSWER_REGISTERED_PHONE:    // 11
  case _THERE_IS_NO_REQUEST:           // 13
  case _THERE_IS_OPEN_REQUEST:         // 14
  case _OTHER_REASONS:                 // 15
    state = _IDLE;
    break;

  case _NO_ANSWER_FOR_T_MINUTE: // 12
    if (WiFiVariables.ServiceButton == _IDLE)
      state = _IDLE;
    else
    {
      if (!sb->timeOut.timer)
        state = _IDLE;
      else
        state = _NO_ANSWER_FOR_T_MINUTE;
    }
    break;

  case _REQUEST_CANCELLING: // 16
    if (WiFiVariables.ServiceButton == _REQUEST_CANCELLING_IS_OK)
      state = _REQUEST_CANCELLING_IS_OK;
    else if (WiFiVariables.ServiceButton == _REQUEST_CANCELLING_IS_NOK)
      state = _REQUEST_CANCELLING_IS_NOK;
    else
    {
      if (!sb->timeOut.timer)
        state = _NO_ANSWER_FOR_T_MINUTE;
      else
        state = _REQUEST_CANCELLING;
    }
    break;

  case _REQUEST_CANCELLING_IS_OK: // 17
    state = _IDLE;
    break;

  case _REQUEST_CANCELLING_IS_NOK: // 18
    state = _REQUEST_CREATED;
    break;

  default:
    state = _IDLE;
    break;
  }

  if (isServiceButtonStateChanged(consts, params, state))
  {
    sb->statePrev = sb->state;
    serviceButtonMelodies(consts, params, state);
    serviceButtonErrorCheck(consts, params, state);
  }

  sb->state = state;
}

/**
 * @brief  serviceButton led state check function.
 *         Controls the serviceButton led state according to serviceButton
 *         state.
 * @param  *sb @ref tsUiServiceButtonData
 * @retval None
 */
static void serviceButtonLedStateCheck(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  tsUiServiceButtonData *sb = &params->ui->serviceButton;

  enServiceButtonStates state = sb->state;
  enServiceButtonLedStates ledState = _OFF;

  switch (state)
  {
  case _IDLE:                          // 0
  case _REQUEST_SUCCEED:               // 4
  case _DEVICE_NO_BOARDING:            // 5
  case _HOMEWHIZ_ASSOC_NO:             // 6
  case _HOMEWHIZ_CLOUD_NOT_ACCESSIBLE: // 7
  case _IT_SERVICE_CONNECT_TIMEOUT:    // 8
  case _ERROR_BLACK_LIST:              // 9
  case _NO_REGISTERED_PHONE_NUMBER:    // 10
  case _NO_ANSWER_REGISTERED_PHONE:    // 11
  case _NO_ANSWER_FOR_T_MINUTE:        // 12
  case _THERE_IS_NO_REQUEST:           // 13
  case _THERE_IS_OPEN_REQUEST:         // 14
  case _OTHER_REASONS:                 // 15
  case _REQUEST_CANCELLING_IS_OK:      // 17
    ledState = _OFF;
    break;

  case _PRESSED_3SEC:     // 1
  case _REQUEST_CREATING: // 2
    ledState = _FAST_BLINK;
    break;

  case _REQUEST_CREATED:           // 3
  case _REQUEST_CANCELLING_IS_NOK: // 18
    ledState = _ON;
    break;

  case _REQUEST_CANCELLING: // 16
    ledState = _SLOW_BLINK;
    break;

  default:
    ledState = _OFF;
    break;
  }

  if (!sb->led.stateChange.timer ||
      sb->led.state == _OFF)
    sb->led.state = ledState;
}

/**
 * @brief  serviceButton led driver function.
 *         Drives the serviceButton led according to serviceButton
 *         led state.
 * @param  *sb @ref tsUiServiceButtonData
 * @retval None
 */
static void serviceButtonLedDriver(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  tsUiServiceButtonData *sb = &params->ui->serviceButton;

  enServiceButtonLedStates state = sb->led.state;

  switch (state)
  {
  case _OFF:
    sb->led.serviceButton = _OFF;
    sb->led.blink.timer = 0;
    break;

  case _SLOW_BLINK:
    if (sb->led.blink.timer > BLINK_TIME / 2)
      sb->led.serviceButton = _ON;
    else
      sb->led.serviceButton = _OFF;
    break;

  case _FAST_BLINK:
    if (sb->led.blink.timer > BLINK_TIME / 4)
      sb->led.serviceButton = _ON;
    else
      sb->led.serviceButton = _OFF;
    break;

  case _ON:
    sb->led.serviceButton = _ON;
    sb->led.blink.timer = 0;
    break;

  default:
    sb->led.serviceButton = _OFF;
    sb->led.blink.timer = 0;
    break;
  }
}

/**
 * @brief  serviceButton melodies function.
 *         Controls the buzzer melodies according the serviceButton
 *         state transition.
 * @param  state @ref enServiceButtonStates
 * @retval None
 */
static void serviceButtonMelodies(const tsUiProcConsts *consts, tsUiProcParams *params, enServiceButtonStates state)
{
  switch (state)
  {
  case _IDLE:               // 0
  case _PRESSED_3SEC:       // 1
  case _REQUEST_CREATING:   // 2
  case _REQUEST_SUCCEED:    // 4
  case _REQUEST_CANCELLING: // 16
    break;

  case _REQUEST_CREATED:          // 3
  case _REQUEST_CANCELLING_IS_OK: // 17
    sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    break;

  case _DEVICE_NO_BOARDING:            // 5
  case _HOMEWHIZ_ASSOC_NO:             // 6
  case _HOMEWHIZ_CLOUD_NOT_ACCESSIBLE: // 7
  case _IT_SERVICE_CONNECT_TIMEOUT:    // 8
  case _ERROR_BLACK_LIST:              // 9
  case _NO_REGISTERED_PHONE_NUMBER:    // 10
  case _NO_ANSWER_REGISTERED_PHONE:    // 11
  case _NO_ANSWER_FOR_T_MINUTE:        // 12
  case _THERE_IS_NO_REQUEST:           // 13
  case _THERE_IS_OPEN_REQUEST:         // 14
  case _OTHER_REASONS:                 // 15
  case _REQUEST_CANCELLING_IS_NOK:     // 18
    sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
    break;

  default:
    sysBuzzerCmd(eSysBuzzerNone);
    break;
  }
}

/**
 * @brief  serviceButton errorCheck function.
 *         Decide whether serviceButton error state occurs.
 * @param  state @ref enServiceButtonStates
 * @retval None
 */
static void serviceButtonErrorCheck(const tsUiProcConsts *consts, tsUiProcParams *params, enServiceButtonStates state)
{
  UNUSED(consts);
  UNUSED(params);
  UNUSED(state);
  /* Since this is option is not required.
     this function does not implemented yet. */
}

/**
 * @brief  serviceButton set time out function.
 *         This funciton sets the timeout and led state change timer
 *         according to state transition.
 *         If timeout intervals updated by serviceButton IT service,
 *         updated values (WA[7] and WA [8]) can be use.
 * @param  state @ref enServiceButtonStates
 * @retval None
 */
static void serviceButtonSetTimeOut(const tsUiProcConsts *consts, tsUiProcParams *params, enServiceButtonStates state)
{
  UNUSED(consts);
  UNUSED(params);
  UNUSED(state);

  tsUiServiceButtonData *sb = &params->ui->serviceButton;
  if (state != _REQUEST_CREATED)
  {
    if (WiFiVariables.ServiceButtonShortTimeOut)                   // WA[7]
      sb->timeOut.timer = WiFiVariables.ServiceButtonShortTimeOut; // SERVICE_BUTTON_SHORT_TIME_OUT;
    else
      sb->timeOut.timer = SERVICE_BUTTON_SHORT_TIME_OUT;
  }
  else
  {
    if (WiFiVariables.ServiceButtonLongTimeOut)                   // WA[8]
      sb->timeOut.timer = WiFiVariables.ServiceButtonLongTimeOut; // SERVICE_BUTTON_LONG_TIME_OUT;
    else
      sb->timeOut.timer = SERVICE_BUTTON_LONG_TIME_OUT;
  }

  sb->led.stateChange.timer = SERVICE_BUTTON_MIN_LED_STATE_TIME;
}

/**
 * @brief  serviceButton state change function.
 *         Decide wheater serviceButton state change or not.
 *         Calls the required functions if the state changed.
 * @param  state @ref enServiceButtonStates
 * @retval bool
 */
static bool isServiceButtonStateChanged(const tsUiProcConsts *consts, tsUiProcParams *params, enServiceButtonStates state)
{
  tsUiServiceButtonData *sb = &params->ui->serviceButton;

  if (sb->statePrev != state)
  {
    sb->statePrev = state;
    serviceButtonSetTimeOut(consts, params, state);
    serviceButtonWakeUpDisplay(consts, params);
    return true;
  }
  else
    return false;
}

/**
 * @brief  serviceButton set wifi array funciton.
 *         This function set serviceButton state to the WA[0]
 *         and WA[48](ServiceButtonCopy). WA[48] can be change
 *         according to appliance. Copy has to placed after WA[26]
 * @param  sb @ref tsUiServiceButtonData
 * @retval None
 */
static void serviceButtonSetWifiArray(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  tsUiServiceButtonData *sb = &params->ui->serviceButton;
  if ((params->rf->workMode.values.workMode == _WM_Normal) ||
      (params->rf->workMode.values.workMode == _WM_ProductionTest))
  {
    WiFiVariables.ServiceButton = sb->state;     // Write instant state to WifiArray[0]
    WiFiVariables.ServiceButtonCopy = sb->state; // Write replica of instant state to WifiArray[48]
  }
}

/**
 * @brief  serviceButton send Mercek array
 *         Sets the mercekArray send flag.
 *         Since it is not need to send mercek array in Distributer Mode.
 *         It is prevented.
 * @param  None
 * @retval None
 */
static void serviceButtonSendMercekArray(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  UNUSED(consts);
  if ((params->rf->workMode.values.workMode == _WM_Normal) ||
      (params->rf->workMode.values.workMode == _WM_ProductionTest))
  {
    /*MercekData.SendingReason = MERCEK_REASON_SERVICE_BUTTON;
    WA[RemoteStart]=50;
    LastSendF = WA[RemoteStart];
    MercekData.MercekID++;*/
    params->ui->serviceButton.sendMercekArray = true;
  }
}

/**
 * @brief  serviceButton open wifi funciton.
 *         When the serviceButton is pressed. If it is closed, oped wifi.
 * @param  None
 * @retval None
 */
void serviceButtonOpenWifi(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  UNUSED(params);
  UNUSED(consts);
  if (WiFiVariables.Situation != WIFI_CONNECTED)
  {
    setWifiState(WIFI_ON);
    rcRequest(eRcRequestCheckConnection, TRUE);
  }
}

/**
 * @brief  serviceButton wake up display funciton.
 *         If the display in the off mode. This function wake up the display
 * @param  None
 * @retval None
 */
void serviceButtonWakeUpDisplay(const tsUiProcConsts *consts, tsUiProcParams *params)
{
   if (params->ui->states.normal.state == _UI_NormalState_DisplayOff || params->ui->timers.dspOffTimer != 60)
     params->ui->states.normal.state = _UI_NormalState_Sets_Init;
}
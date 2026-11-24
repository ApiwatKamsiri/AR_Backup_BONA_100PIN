#ifndef UI_COMPONENTS_RAM_H
#define UI_COMPONENTS_RAM_H

/*
#define _UI_IceCreamMachine_BlinkSet_Time 50					// 25 * 20ms = 0,5 sec
#define _UI_IceCreamMachine_StateErrTemp_Time 30				// second
#define _UI_IceCreamMachine_StateIceCreamIsMade_Time (5.0 * 60) //(dk*60 sec)
#define _UI_IceCreamMachine_StateIdle_Time 30					// second
*/
typedef struct
{
	unsigned char timer;
	unsigned short stateTimer;
} tsUiIceMachineData;


/***********************************/

/**
 * @brief serviceButton state enumarations.
 */
typedef enum
{
  _IDLE = 0,                          // 0
  _PRESSED_3SEC = 1,                  // 1
  _REQUEST_CREATING = 2,              // 2
  _REQUEST_CREATED = 3,               // 3
  _REQUEST_SUCCEED = 4,               // 4
  _DEVICE_NO_BOARDING = 5,            // 5
  _HOMEWHIZ_ASSOC_NO = 6,             // 6
  _HOMEWHIZ_CLOUD_NOT_ACCESSIBLE = 7, // 7
  _IT_SERVICE_CONNECT_TIMEOUT = 8,    // 8
  _ERROR_BLACK_LIST = 9,              // 9
  _NO_REGISTERED_PHONE_NUMBER = 10,   // 10
  _NO_ANSWER_REGISTERED_PHONE = 11,   // 11
  _NO_ANSWER_FOR_T_MINUTE = 12,       // 12
  _THERE_IS_NO_REQUEST = 13,          // 13
  _THERE_IS_OPEN_REQUEST = 14,        // 14
  _OTHER_REASONS = 15,                // 15
  _REQUEST_CANCELLING = 16,           // 16
  _REQUEST_CANCELLING_IS_OK = 17,     // 17
  _REQUEST_CANCELLING_IS_NOK = 18,    // 18
} enServiceButtonStates;

/**
 * @brief serviceButton led state enumerations.
 */
typedef enum
{
  _OFF = 0,
  _SLOW_BLINK,
  _FAST_BLINK,
  _ON,
} enServiceButtonLedStates;

/**
 * @brief timer struct variables.
 */
typedef struct
{
  unsigned char timer;
  // bool isActive;
} stTimer;

/**
 * @brief led struct variables.
 */
typedef struct
{
  enServiceButtonLedStates state;
  enServiceButtonLedStates serviceButton;
  stTimer blink;
  stTimer stateChange;
} stLED;

/**
 * @brief serviceButton struct variables.
 */
typedef struct
{
  enServiceButtonStates state;
  enServiceButtonStates statePrev;
  stLED led;
  stTimer timeOut;
  bool sendMercekArray;
} tsUiServiceButtonData;

#endif
/** @file       remote_control_driver.h
 *  @brief      REMOTE_CONTROL_DRIVER application header file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Umit Sahin
 *  @date       21.02.2020
 */
#ifndef FILE_REMOTE_CONTROL_DRIVER_H
#define FILE_REMOTE_CONTROL_DRIVER_H

/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "system.h"
#if (CONNECTIVITY != _NON_CONNECTED)
#include "../../rcos/mw/Connectivity/wfa.h"
#include "../../rcos/mw/Connectivity/ccm.h"
#include "../../rcos/mw/Connectivity/ccm_safir.h"
//#include "app/protb_gdm_control.h"

#define PACK_TYPE_IPC_LOCAL (0xFF)

typedef enum
{
    IPC_LOCAL_M4_IS_ALIVE = 0x01,
} teIpcLocalCommands;

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_REMOTE_CONTROL_DRIVER_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   REMOTE_CONTROL_DRIVER REMOTE_CONTROL_DRIVER
 *  @ingroup    APP
 *  @brief      Handling  remote control
 *  @{
 */

/// @brief  Events of a RemoteControlDriver process
typedef enum
{
  eRCDEventIdle = 0,  
  eRCDEventHandleRcRequest,
  eRCDEventOnboardingState,
  eRCDEventSafirComState,
  
  eRCDEventSendConsumption,
  eRCDEventConsumptionSent,
  
  eRCDEventServiceButton,        
  eRCDEventMachineID,        
  eRCDEventServiceButtonShortTimeOut,
  eRCDEventServiceButtonLongTimeOut,
  eRCDEventMeasurementInformation,
  eRCDEventPassword,
  eRCDEventNBVersion,
  
  eRCDEventNBRevision,
  eRCDEventWGVersion,
  eRCDEventDateMinute,
  eRCDEventDateHour,
  eRCDEventDateDay,
  eRCDEventDateMonth,
  //eRCDNewCommandLowerBorder, // events that do not trigger a wake-up from standby should be placed above.
  eRCDEventDateYear,
  eRCDEventMercekReadDone,
  eRCDEventMercekSize,
  eRCDEventArraySize,
  eRCDEventSituation,
  eRCDEventNBErrors0,
  eRCDEventMercekErrorLenght,
  eRCDEventTestMode,
  eRCDEventEndMesg,
  eRCDEventResetControl,
  eRCDEventAux2,
  eRCDEventOTA,
  eRCDEventCabinTempType,
  eRCDEventCabinTempSet0,
  eRCDEventCabinTempSet1,
  eRCDEventCabinTempSet2,
  eRCDEventCabinTempSet3,
  eRCDEventCabinTempSet4,
  eRCDEventCabinFunc0,
  eRCDEventCabinFunc1,
  eRCDEventCabinFunc2,
  eRCDEventCabinFunc3,
  eRCDEventCabinFunc4,
  eRCDEventIceMachineSet,
  eRCDEventDispenser,
  eRCDEventIceSource,
  eRCDEventServiceButtonCopy,
  eRCDEventIceMachine,
  eRCDEventDisplaySet,
  eRCDEventVacation,
  eRCDEventEcoFuzzy,
  eRCDEventSabbath,
  eRCDEventConnectedFeatures,
  eRCDEventVioled,
  eRCDEventWarnings,
  eRCDEventError,
  eRCDEventFilter,
  eRCDEventAlarmLevel,
  eRCDEventRefrigeratorAux3,
  eRCDEventRefrigeratorAux4,
  eRCDEventComponents1,
  eRCDEventComponents2,
  
  eRCDEventDailyHour,
  eRCDEventDailyMin,
  eRCDEventSpecialTimes_1,
  eRCDEventShoppingMode,
  eRCDEventSpecialTimes_3,
  eRCDEventSpecialFunctions,
  eRCDEventHandleLight,
  eRCDEventDoorAlarmTime,
  eRCDEventAIMode,
  eRCDEventCheapSelectMin_3,
  eRCDEventCheapSelectDuration_3,
  eRCDEventCheapSelectFlag,
  eRCDEventCheapSelectConfig,
  eRCDEventCheapSelectCountDown,
  
  eRCDEventDailyDay,
  eRCDEventDailyMonth,
  eRCDEventDailyYear,
  eRCDEventhTE_HighestTempRead,
  eRCDEventhTE_WarningReason,
  eRCDEventeE_EnergyEfficiencyDutyFF,
  eRCDEventMercekErrorShadow1,
  eRCDEventMercekErrorShadow2,
  eRCDEventQuickIce60,
  eRCDEventRGBHandleLight,
  eRCDEventVacuumBox,
  eRCDEventKeylockDspr,
  
  eRCDEventSize,
} teRemoteControlDriverEvents;

/// @brief  Parameters of a RemoteControlDriver process
typedef struct
{
	tsTimerEvent tev;
	enum_connectionSubStateWifi *connState;
	uint8_t wfaSize;
	uint8_t mercekSize;
} tsRemoteControlDriverParams;

/** @brief  RemoteControlDriver process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_REMOTE_CONTROL_DRIVER_CREATE(_name, _enum, connState, _wfaSize, _mercekSize) \
	tsRemoteControlDriverParams _name##Params =                                              \
		{                                                                                    \
			.tev = TIMER_EVENT_INIT(_enum, _enum, eRCDEventIdle),                            \
			&connState,                                                                      \
			_wfaSize,                                                                        \
			_mercekSize,                                                                     \
	};                                                                                       \
	PROCESS_CREATE(_name, remoteControlDriverInit, remoteControlDriverDeinit, _enum, &_name##Params, NULL)

/** @} */
#undef INTERFACE // Should not let this roam free
#endif // CONNECTIVITY
#endif // FILE_REMOTE_CONTROL_DRIVER_H

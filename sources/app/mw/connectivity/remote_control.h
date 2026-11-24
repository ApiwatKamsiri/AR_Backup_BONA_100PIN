/** @file       remote_control.h
*  @brief      REMOTE_CONTROL application header file
*  @copyright  (c) 2020-Arcelik - All Rights Reserved
*              Permission to use, reproduce, copy, prepare derivative works,
*              modify, distribute, perform, display or sell this software and/or
*              its documentation for any purpose is prohibited without the express
*              written consent of Arcelik A.S.
*  @author     Umit Sahin
*  @date       26.06.2020
*/
#ifndef FILE_REMOTE_CONTROL_H
#define FILE_REMOTE_CONTROL_H

#include "rcos.h"
#include "system.h"
#if (CONNECTIVITY != _NON_CONNECTED)
#include "..\..\..\RCOS\mw\Connectivity\consumption.h"
#endif

#ifndef FILE_REMOTE_CONTROL_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

#define MERCEK_CABIN_SIZE       2
#define WFA_SIZE 86

/* ------- WIFI STATES ----------------
State                  State Variable
Wi-Fi is Closed             0
Wi-Fi is Open               1
Wi-Fi Setup is Open         2
Connecting                  3
Connected                   4
Failed to Connect           5
Password is Sending         6
Dropped from Modem          8
-------------------------------------*/
typedef enum
{ 
    WIFI_OFF = 0,
    WIFI_ON,
    WIFI_AP_ON,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_NOT_CONNECTED,
    WIFI_PW_SENDING,
    WIFI_DROPPED = 8,
    WIFI_CONFIG_TOGGLE,
    WIFI_TOGGLE
} wifi_states_t;

typedef enum
{
  eRcRequestIdle = 0,
  eRcRequestStartOnboarding,
  eRcRequestCheckConnection,
  eRcRequestStopBroadcasting,
  eRcRequestStartTestMode,
  eRcRequestStopTestMode,
  eRcRequestCloseConnection
} remotecontrolRequests;
INTERFACE void rcRequest(remotecontrolRequests request, uint8_t val);

INTERFACE unsigned char connectionTimer;

typedef struct
{
    char counter;
    char longLimit;
    char shortLimit;
    bool state;
    char lastState;
    char wifiStateChange;
} blink_t;
INTERFACE blink_t WiFiLed;
typedef struct _typeWifiVariables
{
  unsigned char ServiceButton;
  unsigned char StockCode[5];
  unsigned char MachineID;
  unsigned char ServiceButtonShortTimeOut;
  unsigned char ServiceButtonLongTimeOut;
  unsigned char MeasurementInformation;
  unsigned char Password[10];
  unsigned char NBVersion;
  unsigned char NBRevision;
  unsigned char WGVersion;
  unsigned char DateMinute;
  unsigned char DateHour;
  unsigned char DateDay;
  unsigned char DateMonth;
  unsigned char DateYear;
  unsigned char RemoteStart;
  unsigned char MercekSize;
  unsigned char ArraySize;

  unsigned char Situation;
  unsigned char NBErrors0;
  unsigned char MercekErrorLenght;
  unsigned char TestMode;
  unsigned char EndMesg;
  unsigned char ResetControl;
  unsigned char Aux2;
  unsigned char OTA;
  unsigned char CabinTempType;
  unsigned char CabinTempSet[5];
  unsigned char CabinFunc[5];
  unsigned char IceMachineSet;
  unsigned char Dispenser;
  unsigned char IceSource;
  unsigned char ServiceButtonCopy;
  unsigned char IceMachine;
  unsigned char DisplaySet;
  unsigned char Vacation;
  unsigned char EcoFuzzy;
  unsigned char Sabbath;
  unsigned char ConnectedFeatures;
  unsigned char Violed;
  unsigned char Warnings;
  unsigned char Error;
  unsigned char Filter;
  unsigned char AlarmLevel;
  unsigned char RefrigeratorAux3;
  unsigned char RefrigeratorAux4;
  unsigned char Components1;
  unsigned char Components2;

  // --> Order is important
  unsigned char DailyHour;
  unsigned char DailyMin;
  unsigned char SpecialTimes_1;
  unsigned char ShoppingMode;
  unsigned char SpecialTimes_3;
  unsigned char SpecialFunctions;
  unsigned char HandleLight;
  unsigned char DoorAlarmTime;
  unsigned char AIMode;
  //unsigned char AIPersonaSelect;
  unsigned char CheapSelectMin_3;
  unsigned char CheapSelectDuration_3;
  unsigned char CheapSelectFlag;
  unsigned char CheapSelectConfig;
  unsigned char CheapSelectCountDown;
  // <-- order is important

  unsigned char DailyDay;
  unsigned char DailyMonth;
  unsigned char DailyYear;
  unsigned char hTE_HighestTempRead;
  unsigned char hTE_WarningReason;
  unsigned char eE_EnergyEfficiencyDutyFF;
  unsigned char MercekErrorShadow1;
  unsigned char MercekErrorShadow2;
  unsigned char QuickIce60;
  unsigned char HandleLightIndex;
  unsigned char VacuumBoxSet;
  unsigned char KeylockDspr;
} typeWifiVariables;

INTERFACE typeWifiVariables WiFiVariables;
#if (CONNECTIVITY != _NON_CONNECTED)

void Connectivity_Reset(void);
void toggleWifiState(wifi_states_t state);
void setWifiState(unsigned char state);
void fHandleMercekParameters(void);
void Connectivity_ResetErrors(void);
void initializeSecureMercekParams(void);

INTERFACE unsigned char resetControlTimeout;
INTERFACE unsigned char startCntr;
INTERFACE unsigned char endofbandtest;
INTERFACE unsigned char wifiError_B1;
INTERFACE unsigned char wifiError_B2;
INTERFACE unsigned short WifiConfigTimeOut;
INTERFACE unsigned char sendData;
INTERFACE unsigned char comingData;
INTERFACE unsigned char iswifioff;

typedef union remoteControlFlags_ {
  struct
  {
    uint8_t BFSW_REMOTE_CONTROL : 1;
    uint8_t BFSW_SEND_MERCEK_DATA : 1;
    uint8_t BFSW_RECEIVE_MERCEK_DATA : 1;
    uint8_t ResetStandByTimer : 1;
    uint8_t StandBy_OFF : 1;
    uint8_t wifiWokeUpForMercek : 1; // NUI
    uint8_t noNewRemoteMessage : 1;
    uint8_t testModeSuccessful : 1;
  } bits;
  uint8_t all;
} rcFlags;
INTERFACE rcFlags rcFlag;

/// @brief  Events of a Service Call Function
typedef enum
{
  eServiceCallIdle = 0,
  eServiceCallPressed,
  eServiceCallCreatingRequest,
  eServiceCallPositiveAnswer,
  eServiceCallMissionCompleted,
  eServiceCallNeverOnboarded,
  eServiceCallNoHomewhizAssociation,
  eServiceCallCloudNotAccessible,
  eServiceCallITNotAccessible, // Information Technology services is not accessible
  eServiceCallBlacklist,
  eServiceCallNoPhoneRecord, // There is no recorded phone number on system.
  eServiceCallNoAnswerFromCustomer,
  eServiceCallTimeout,
  eServiceCallRequestCannotBeCancelled, // There might not be an active request.
  eServiceCallCannotCreateRequest, // There might already be an active request.
  eServiceCallOtherNegativeReasons,
  eServiceCallCancellingRequest,
  eServiceCallCancelled,
  eServiceCallCannotBeCancelled,
  eServiceCallMax
} eServiceCallEvents;
typedef struct SERVICE_CALL_PARAMS
{
  uint16_t timeout;
  eServiceCallEvents state;
  eServiceCallEvents newState;
  eServiceCallEvents previousState;
  uint8_t wakeServiceLed;
} serviceCallParams;
INTERFACE serviceCallParams rcServiceCall;

typedef enum
{
  EV_WIFI_MODE_GET_CONNECTING = 1, // 1
  EV_WIFI_MODE_CONNECTED,		 // 2
  EV_WIFI_MODE_NOT_CONNECTED,	 // 3
}connectionModes;
INTERFACE connectionModes connectionState;
INTERFACE wifi_states_t wifiState;

typedef enum
{
  eSmartLoadAmount = 0,
  eSmartTemperature,	   //  1
  eSmartWashingTime,	   //  2
  eSmartTextileType,	   //  3
  eSmartDetergentDosage, //  4
  eSmartSoftenerDosage,  //  5
  eSmartDirtLevel,	   //  6
  eSmartSpin,			   //  7
} teSmartTypes;

typedef enum
{
  eRcFactoryResetNone,
  eRcFactoryResetInitiate,
  eRcFactoryResetSafirResponse,
  eRcFactoryResetCloud,
  eRcFactoryResetFinished,
  eRcFactoryResetSafirFailed = 10,
  eRcFactoryResetCloudFailed = 11,
  eRcFactoryResetUnkownError = 60,
} teRcFactoryResetStates;

INTERFACE teRcFactoryResetStates remoteControlResetState;

#define MERCEK_SEND_PERIOD             60

/*MERCEK ARRAY SENDING REASONS*/
#define MERCEK_REASON_PERIODIC          0x01
#define MERCEK_REASON_ERROR             0x02
#define MERCEK_REASON_REQUEST           0x04
#define MERCEK_REASON_SERVICE_BUTTON    0x08
#define MERCEK_REASON_POWERCUT          0x10

#define SENDING_REASON_SIZE             1
#define MB_STOCK_NO_SIZE                4
#define MB_VERSION_SIZE                 1
#define MB_REVISION_SIZE                1
#define WORKMODE_SIZE                   1
#define CABIN_TYPE_MODE_SIZE            1   // V03R05: Cabin modes bitmap [1:0]=Cabin0, [3:2]=Cabin1
#define CA0_CA1_SETSIDX_SIZE            1
#define CA0_CA1_DOOROPEN_COUNTER_SIZE   2   // V03R03: Changed from 1 to 2 (each cabin 0-255)
#define CAx_DOOROPEN_TIMER_SIZE         1
#define HUMAN_SENSOR_COUNTER_SIZE       1   // V03R10: Human sensor activation count (0-255)
#define FUNCTIONS_SIZE                  1
#define FUNCTIONS_2_SIZE                1   // V03R07: Additional functions bitmap
#define FUNCTIONS_3_SIZE                1   // V03R11: [3:0]=Persona bitmap, [7:4]=HandleLight color index
#define ICE_MACHINE_SIZE                2
#define ICE_MACHINE_TEMP_SIZE           10
#define ICE_MACHINE_WATER_HEATER_SIZE   1   // V03R08: Ice Machine Water Heater
#define DISPENSER_SIZE                  2   // V03R06: [0]=state bitmap (8 states), [1]=tactSW count (0-255)
#define CAx_FAN_SIZE                    1
#define CAx_HEATERA_SIZE                1
#define CAx_HEATERB_SIZE                1
#define CAx_HEATERC_SIZE                1
#define VACUUM_HEATER_SIZE              1   // V03R12: Vacuum box heater
#define COOLER_TYPE_SIZE                1   // V03R04: CoolerType bitmap
#define SELENOID_VALVE_SIZE             4
#define SELENOID_VALVE_DETAIL_SIZE      30
#define ENERGY_MONITOR_SIZE             1   // V03R09: Energy monitoring (0-255 W)
#define COMPRESSOR_SIZE                 4
#define COMPRESSOR_DETAIL_SIZE          8
#define COMP_STOCK_NO_SIZE              6   // V03R13: Compressor stock number (3x 2bytes)
#define COMP_VERSION_SIZE               1   // V03R14: Compressor version
#define COMP_REVISION_SIZE              1   // V03R14: Compressor revision
#define COMP_CORE_VERSION_SIZE          1   // V03R22: Compressor core version
#define COMP_CORE_REVISION_SIZE         1   // V03R22: Compressor core revision
#define COMP_TYPE_SIZE                  1   // V03R15: type bitmap [3:0], onOff bitmap [6:4]
#define COMP_ERROR_SIZE                 2   // V03R20: Compressor error/status code (EKI or Others)
#define COMP_POWER_SIZE                 2   // V03R18: Compressor power average
#define COMP_DCBUS_SIZE                 2   // V03R18: DC bus voltage min/max
#define COMP_REALSPEED_SIZE             1   // V03R19: Compressor real speed average
#define CAx_DEFROST_DURATION_SIZE       1
#define CAx_DEFROST_REASON_SIZE         1
#define CAx_TEMPERATURE_SIZE            20
#define AMBIENT_SIZE                    4
#define CONNECTION_SIZE                 2
#define ERROR_CODE_SIZE                 8
#define DATA_COUNT_SIZE                 1
#define MERCEK_COUNT_SIZE               1

typedef enum
{
  SENDING_REASON = 0,
  MB_STOCK_NO = SENDING_REASON + SENDING_REASON_SIZE,
  MB_VERSION = MB_STOCK_NO + MB_STOCK_NO_SIZE,
  MB_REVISION = MB_VERSION + MB_VERSION_SIZE,
  WORKMODE = MB_REVISION + MB_REVISION_SIZE,
  CABIN_TYPE_MODE = WORKMODE + WORKMODE_SIZE,   // V03R05: Cabin modes bitmap
  CA0_CA1_SETS_IDX = CABIN_TYPE_MODE + CABIN_TYPE_MODE_SIZE,
  CA0_CA1_DOOR_OPEN_COUNTER = CA0_CA1_SETS_IDX + CA0_CA1_SETSIDX_SIZE,
  CA0_DOOR_OPEN_TIMER = CA0_CA1_DOOR_OPEN_COUNTER + CA0_CA1_DOOROPEN_COUNTER_SIZE,
  CA1_DOOR_OPEN_TIMER = CA0_DOOR_OPEN_TIMER + CAx_DOOROPEN_TIMER_SIZE,
  HUMAN_SENSOR_COUNTER = CA1_DOOR_OPEN_TIMER + CAx_DOOROPEN_TIMER_SIZE,  // V03R10: Human sensor count
  FUNCTIONS = HUMAN_SENSOR_COUNTER + HUMAN_SENSOR_COUNTER_SIZE,
  FUNCTIONS_2 = FUNCTIONS + FUNCTIONS_SIZE,   // V03R07: Additional functions bitmap
  FUNCTIONS_3 = FUNCTIONS_2 + FUNCTIONS_2_SIZE,   // V03R11: Persona + HandleLight color
  ICE_MACHINE = FUNCTIONS_3 + FUNCTIONS_3_SIZE,
  ICE_MACHINE_TEMP = ICE_MACHINE + ICE_MACHINE_SIZE,
  ICE_MACHINE_WATER_HEATER = ICE_MACHINE_TEMP + ICE_MACHINE_TEMP_SIZE,  // V03R08: Ice Machine Water Heater
  DISPENSER = ICE_MACHINE_WATER_HEATER + ICE_MACHINE_WATER_HEATER_SIZE,
  CA0_FAN = DISPENSER + DISPENSER_SIZE,
  CA0_HEATERA = CA0_FAN + CAx_FAN_SIZE,
  CA0_HEATERB = CA0_HEATERA + CAx_HEATERA_SIZE,
  CA0_HEATERC = CA0_HEATERB + CAx_HEATERB_SIZE,
  CA1_FAN = CA0_HEATERC + CAx_HEATERC_SIZE,
  CA1_HEATERA = CA1_FAN + CAx_FAN_SIZE,
  CA1_HEATERB = CA1_HEATERA + CAx_HEATERA_SIZE,
  CA1_HEATERC = CA1_HEATERB + CAx_HEATERB_SIZE,
  VACUUM_HEATER = CA1_HEATERC + CAx_HEATERC_SIZE,  // V03R12: Vacuum box heater
  COOLER_TYPE = VACUUM_HEATER + VACUUM_HEATER_SIZE,   // V03R04: CoolerType bitmap
  SELENOID_VALVE = COOLER_TYPE + COOLER_TYPE_SIZE,
  SELENOID_VALVE_DETAIL = SELENOID_VALVE + SELENOID_VALVE_SIZE,
  ENERGY_MONITOR = SELENOID_VALVE_DETAIL + SELENOID_VALVE_DETAIL_SIZE,  // V03R09: Energy monitoring
  COMPRESSOR = ENERGY_MONITOR + ENERGY_MONITOR_SIZE,
  COMPRESSOR_DETAIL = COMPRESSOR + COMPRESSOR_SIZE,
  COMP_STOCK_NO = COMPRESSOR_DETAIL + COMPRESSOR_DETAIL_SIZE,  // V03R13: Compressor stock number
  COMP_VERSION = COMP_STOCK_NO + COMP_STOCK_NO_SIZE,            // V03R14: Compressor version
  COMP_REVISION = COMP_VERSION + COMP_VERSION_SIZE,             // V03R14: Compressor revision
  COMP_CORE_VERSION = COMP_REVISION + COMP_REVISION_SIZE,       // V03R22: Compressor core version
  COMP_CORE_REVISION = COMP_CORE_VERSION + COMP_CORE_VERSION_SIZE, // V03R22: Compressor core revision
  COMP_TYPE = COMP_CORE_REVISION + COMP_CORE_REVISION_SIZE,     // V03R15: Compressor type + onOff
  COMP_ERROR = COMP_TYPE + COMP_TYPE_SIZE,                       // V03R20: Error/status code
  COMP_POWER = COMP_ERROR + COMP_ERROR_SIZE,                     // V03R18: Power average
  COMP_DCBUS = COMP_POWER + COMP_POWER_SIZE,                    // V03R18: DC bus min/max
  COMP_REALSPEED = COMP_DCBUS + COMP_DCBUS_SIZE,                // V03R19: Real speed average
  CA0_DEFROST_DURATION = COMP_REALSPEED + COMP_REALSPEED_SIZE,
  CA0_DEFROST_REASON = CA0_DEFROST_DURATION + CAx_DEFROST_DURATION_SIZE,
  CA1_DEFROST_DURATION = CA0_DEFROST_REASON + CAx_DEFROST_REASON_SIZE,
  CA1_DEFROST_REASON = CA1_DEFROST_DURATION + CAx_DEFROST_DURATION_SIZE,
  CA0_TEMPERATURE = CA1_DEFROST_REASON + CAx_DEFROST_REASON_SIZE,
  CA1_TEMPERATURE = CA0_TEMPERATURE + CAx_TEMPERATURE_SIZE,
  AMBIENT = CA1_TEMPERATURE + CAx_TEMPERATURE_SIZE,
  CONNECTION = AMBIENT + AMBIENT_SIZE,
  ERROR_CODE = CONNECTION + CONNECTION_SIZE,
  DATA_COUNT = ERROR_CODE + ERROR_CODE_SIZE,
  MERCEK_COUNT = DATA_COUNT + DATA_COUNT_SIZE,
  EOF_MERCEK_DATA = MERCEK_COUNT + MERCEK_COUNT_SIZE,
} enMercekAddrOffset;

typedef struct _typeMercekVariables
{
  unsigned char SendingReason;                   // 0
  unsigned char MBStock[MB_STOCK_NO_SIZE];       // 1
  unsigned char MBVersion;                       // 5
  unsigned char MBRevision;                      // 6
  unsigned char WorkMode;                        // 7
  unsigned char CabinTypeMode;                   // 8 V03R05: [1:0]=Cabin0, [3:2]=Cabin1 mode
  unsigned char CabinSets;                       // 9
  unsigned char DoorOpenCounter[CA0_CA1_DOOROPEN_COUNTER_SIZE]; // 10-11 V03R03: [0]=FF, [1]=FRZ (0-255 each)
  unsigned char CabinDoorOpenTimer[MERCEK_CABIN_SIZE * CAx_DOOROPEN_TIMER_SIZE]; // 12
  unsigned char HumanSensorCounter;  // 14 V03R10: Human sensor activation count
  unsigned char Functions;  // 15
  unsigned char Functions2; // 16 V03R07: Additional functions bitmap
  unsigned char Functions3; // 17 V03R11: [3:0]=Persona, [7:4]=HandleLight color
  unsigned char IceMachine[ICE_MACHINE_SIZE];          // 18
  signed char IceMachineTemp[ICE_MACHINE_TEMP_SIZE]; // 20
  unsigned char IceMachineWaterHeater;                 // 30 V03R08: Ice Machine Water Heater
  unsigned char Dispenser[DISPENSER_SIZE];             // 31-32 V03R06: [0]=state bitmap, [1]=tactSW count
  unsigned char Cabin0Components[CAx_FAN_SIZE + CAx_HEATERA_SIZE + CAx_HEATERB_SIZE + CAx_HEATERC_SIZE]; // 33
  unsigned char Cabin1Components[CAx_FAN_SIZE + CAx_HEATERA_SIZE + CAx_HEATERB_SIZE + CAx_HEATERC_SIZE]; // 37
  unsigned char VacuumHeater;                                       // 41 V03R12: Vacuum box heater
  unsigned char CoolerType;                                         // 42 V03R04: Cooler type bitmap
  unsigned char SelenoidValve[SELENOID_VALVE_SIZE];                 // 43
  unsigned char SelenoidValveDetailed[SELENOID_VALVE_DETAIL_SIZE];  // 47
  unsigned char EnergyMonitor;                                     // 77 V03R09: Energy (W/100)
  unsigned char Compresor[COMPRESSOR_SIZE];                         // 78
  unsigned char CompresorDetailed[COMPRESSOR_DETAIL_SIZE];          // 82
  unsigned char CompStock[COMP_STOCK_NO_SIZE];                      // 90 V03R13: Compressor stock number
  unsigned char CompVersion;                                         // 96 V03R14: Compressor version
  unsigned char CompRevision;                                        // 97 V03R14: Compressor revision
  unsigned char CompCoreVersion;                                     // 98 V03R22: Compressor core version
  unsigned char CompCoreRevision;                                    // 99 V03R22: Compressor core revision
  unsigned char CompType;                                            // 100 V03R15: type[3:0]=EKI/JIAX/EMBC/2IN1, onOff[6:4]=ON/OFFPREP/OFF
  unsigned char CompError[COMP_ERROR_SIZE];                          // 101 V03R20: Error/status code
  unsigned char CompPower[COMP_POWER_SIZE];                          // 103 V03R18: Power average (hourly)
  unsigned char CompDcbus[COMP_DCBUS_SIZE];                          // 105 V03R18: DC bus min/max
  unsigned char CompRealSpeed[COMP_REALSPEED_SIZE];                  // 107 V03R19: Real speed avg/30
  unsigned char Cabin0Defrost[CAx_DEFROST_DURATION_SIZE + CAx_DEFROST_REASON_SIZE]; // 108
  unsigned char Cabin1Defrost[CAx_DEFROST_DURATION_SIZE + CAx_DEFROST_REASON_SIZE]; // 110
  signed char Cabin0TempSensor[CAx_TEMPERATURE_SIZE];             // 112
  signed char Cabin1TempSensor[CAx_TEMPERATURE_SIZE];             // 132
  unsigned char Ambient[AMBIENT_SIZE];                              // 152
  unsigned char Connection[CONNECTION_SIZE];                        // 156
  unsigned char ErrorCode[ERROR_CODE_SIZE];                         // 158
  unsigned char DataCount;                                          // 166
  unsigned char MercekCount;                                        // 167
  unsigned char SendData;                                           // internal
} typeMercekVariables;

typedef struct _typeSecureMercekVariables
{
  unsigned char data1;
  unsigned char data2;
  unsigned char data3;
  unsigned char data4;
  unsigned char data5;
  unsigned char data6;
  unsigned char data7;
  unsigned char data8;
  unsigned char data9;
  unsigned char data10;
  unsigned char data11;
  unsigned char data12;
  unsigned char data13;
  unsigned char data14;
  unsigned char data15;
  unsigned char data16;
  unsigned char data17;
  unsigned char data18;
  unsigned char data19;
} typeSecureMercekVariables;

INTERFACE typeMercekVariables MercekData;
INTERFACE typeSecureMercekVariables secureMercekVars;

typedef enum {
  ONE_TIME = 0,
  EVERY_SECONDS,
  EVERY_MINUTES,
} enMercekSamplingPeriod;

typedef struct {
  const enMercekAddrOffset offset;
  const unsigned char dataSize;
  enMercekSamplingPeriod period;
} stMercekDataTable;





INTERFACE void rcSendConsumption(uint32_t consumptionValue, uint32_t timeInMinute, teConsumptionTypes consumptionType);

typedef enum WIFI_VOLUME_SETTINGS
{
  VOLUME_LOW,	 // 0
  VOLUME_HIGH, // 1
  VOLUME_NONE	 // 2
} Volume_Levels;

unsigned char fHandleSetServiceButton(unsigned char rcCommand);
unsigned char fHandleSetStockCode(unsigned char rcCommand);
unsigned char fHandleSetMachineID(unsigned char rcCommand);
unsigned char fHandleSetServiceButtonShortTimeOut(unsigned char rcCommand);
unsigned char fHandleSetServiceButtonLongTimeOut(unsigned char rcCommand);
unsigned char fHandleSetMeasurementInformation(unsigned char rcCommand);
unsigned char fHandleSetPassword(unsigned char rcCommand);
unsigned char fHandleSetNBVersion(unsigned char rcCommand);
unsigned char fHandleSetNBRevision(unsigned char rcCommand);
unsigned char fHandleSetWGVersion(unsigned char rcCommand);
unsigned char fHandleSetDateMinute(unsigned char rcCommand);
unsigned char fHandleSetDateHour(unsigned char rcCommand);
unsigned char fHandleSetDateDay(unsigned char rcCommand);
unsigned char fHandleSetDateMonth(unsigned char rcCommand);
unsigned char fHandleSetDateYear(unsigned char rcCommand);
unsigned char fHandleSetRemoteStart(unsigned char rcCommand);
unsigned char fHandleSetMercekSize(unsigned char rcCommand);
unsigned char fHandleSetArraySize(unsigned char rcCommand);
void fResetMercekCounters(void);
unsigned char fHandleSetSituation(unsigned char rcCommand);
unsigned char fHandleSetNBErrors0(unsigned char rcCommand);
unsigned char fHandleSetMercekErrorLenght(unsigned char rcCommand);
unsigned char fHandleSetTestMode(unsigned char rcCommand);
unsigned char fHandleSetEndMesg(unsigned char rcCommand);
unsigned char fHandleSetResetControl(unsigned char rcCommand);
unsigned char fHandleSetAux2(unsigned char rcCommand);
unsigned char fHandleSetOTA(unsigned char rcCommand);
unsigned char fHandleSetCabinTempType(unsigned char rcCommand);
unsigned char fHandleSetCabinTempSet(unsigned char rcCommand, unsigned char index);
unsigned char fHandleSetCabinFunc(unsigned char rcCommand, unsigned char index);
unsigned char fHandleSetIceMachineSet(unsigned char rcCommand);
unsigned char fHandleSetDispenser(unsigned char rcCommand);
unsigned char fHandleSetIceSource(unsigned char rcCommand);
unsigned char fHandleSetServiceButtonCopy(unsigned char rcCommand);
unsigned char fHandleSetIceMachine(unsigned char rcCommand);
unsigned char fHandleSetDisplaySet(unsigned char rcCommand);
unsigned char fHandleSetVacation(unsigned char rcCommand);
unsigned char fHandleSetEcoFuzzy(unsigned char rcCommand);
unsigned char fHandleSetSabbath(unsigned char rcCommand);
unsigned char fHandleSetConnectedFeatures(unsigned char rcCommand);
unsigned char fHandleSetVioled(unsigned char rcCommand);
unsigned char fHandleSetWarnings(unsigned char rcCommand);
unsigned char fHandleSetError(unsigned char rcCommand);
unsigned char fHandleSetFilter(unsigned char rcCommand);
unsigned char fHandleSetAlarmLevel(unsigned char rcCommand);
unsigned char fHandleSetRefrigeratorAux3(unsigned char rcCommand);
unsigned char fHandleSetRefrigeratorAux4(unsigned char rcCommand);
unsigned char fHandleSetComponents1(unsigned char rcCommand);
unsigned char fHandleSetComponents2(unsigned char rcCommand);
unsigned char fHandleSetDailyHour(unsigned char rcCommand);
unsigned char fHandleSetDailyMin(unsigned char rcCommand);
unsigned char fHandleSetSpecialTimes_1(unsigned char rcCommand);
unsigned char fHandleSetShoppingMode(unsigned char rcCommand);
unsigned char fHandleSetSpecialTimes_3(unsigned char rcCommand);
unsigned char fHandleSetSpecialFunctions(unsigned char rcCommand);
unsigned char fHandleSetHandleLight(unsigned char rcCommand);
unsigned char fHandleSetDoorAlarmTime(unsigned char rcCommand);
unsigned char fHandleSetAIMode(unsigned char rcCommand);
unsigned char fHandleSetCheapSelectMin_3(unsigned char rcCommand);
unsigned char fHandleSetCheapSelectDuration_3(unsigned char rcCommand);
unsigned char fHandleSetCheapSelectFlag(unsigned char rcCommand);
unsigned char fHandleSetCheapSelectConfig(unsigned char rcCommand);
unsigned char fHandleSetCheapSelectCountDown(unsigned char rcCommand);
unsigned char fHandleSetDailyDay(unsigned char rcCommand);
unsigned char fHandleSetDailyMonth(unsigned char rcCommand);
unsigned char fHandleSetDailyYear(unsigned char rcCommand);
unsigned char fHandleSethTE_HighestTempRead(unsigned char rcCommand);
unsigned char fHandleSethTE_WarningReason(unsigned char rcCommand);
unsigned char fHandleSeteE_EnergyEfficiencyDutyFF(unsigned char rcCommand);
unsigned char fHandleSetMercekErrorShadow1(unsigned char rcCommand);
unsigned char fHandleSetMercekErrorShadow2(unsigned char rcCommand);
unsigned char fHandleSetQuickIce60(unsigned char rcCommand);
unsigned char fHandleSetRGBHandleLight(unsigned char rcCommand);
unsigned char fHandleSetVacuumBox(unsigned char rcCommand);
unsigned char fHandleSetKeylockDspr(unsigned char rcCommand);


void fHandleReadServiceButton(unsigned char *param);
void fHandleReadStockCode(unsigned char *param);
void fHandleReadMachineID(unsigned char *param);
void fHandleReadServiceButtonShortTimeOut(unsigned char *param);
void fHandleReadServiceButtonLongTimeOut(unsigned char *param);
void fHandleReadMeasurementInformation(unsigned char *param);


void fHandleReadDateMinute(unsigned char *param);
void fHandleReadDateHour(unsigned char *param);
void fHandleReadDateDay(unsigned char *param);
void fHandleReadDateMonth(unsigned char *param);
void fHandleReadDateYear(unsigned char *param);
void fHandleReadRemoteStart(unsigned char *param);
void fHandleReadMercekSize(unsigned char *param);
void fHandleReadArraySize(unsigned char *param);
void fHandleReadSituation(unsigned char *param);
void fHandleReadNBErrors0(unsigned char *param);
void fHandleReadMercekErrorLenght(unsigned char *param);
void fHandleReadTestMode(unsigned char *param);
void fHandleReadEndMesg(unsigned char *param);
void fHandleReadResetControl(unsigned char *param);
void fHandleReadAux2(unsigned char *param);
void fHandleReadOTA(unsigned char *param);


void fHandleReadNBVersion(unsigned char *param);
void fHandleReadNBRevision(unsigned char *param);
void fHandleReadWGVersion(unsigned char *param);
void fHandleReadCabinTempType(unsigned char *param);
void fHandleReadCabinTempSet(unsigned char *param);
void fHandleReadCabinFunc0(unsigned char *param);
void fHandleReadCabinFunc1(unsigned char *param);
void fHandleReadCabinFunc2(unsigned char *param);
void fHandleReadCabinFunc3(unsigned char *param);
void fHandleReadCabinFunc4(unsigned char *param);
void fHandleReadIceMachineSet(unsigned char *param);
void fHandleReadDispenser(unsigned char *param);
void fHandleReadIceSource(unsigned char *param);
void fHandleReadServiceButtonCopy(unsigned char *param);
void fHandleReadIceMachine(unsigned char *param);
void fHandleReadDisplaySet(unsigned char *param);
void fHandleReadVacation(unsigned char *param);
void fHandleReadEcoFuzzy(unsigned char *param);
void fHandleReadSabbath(unsigned char *param);
void fHandleReadConnectedFeatures(unsigned char *param);
void fHandleReadVioled(unsigned char *param);
void fHandleReadWarnings(unsigned char *param);
void fHandleReadError(unsigned char *param);

void fHandleReadFilter(unsigned char *param);
void fHandleReadAlarmLevel(unsigned char *param);
void fHandleReadRefrigeratorAux3(unsigned char *param);
void fHandleReadRefrigeratorAux4(unsigned char *param);
void fHandleReadComponents1(unsigned char *param);
void fHandleReadComponents2(unsigned char *param);
void fHandleReadDailyHour(unsigned char *param);
void fHandleReadDailyMin(unsigned char *param);
void fHandleReadSpecialTimes_1(unsigned char *param);
void fHandleReadShoppingMode(unsigned char *param);
void fHandleReadSpecialTimes_3(unsigned char *param);
void fHandleReadSpecialFunctions(unsigned char *param);
void fHandleReadHandleLight(unsigned char *param);
void fHandleReadDoorAlarmTime(unsigned char *param);
void fHandleReadAIMode(unsigned char *param);
void fHandleReadCheapSelectMin_3(unsigned char *param);
void fHandleReadCheapSelectDuration_3(unsigned char *param);
void fHandleReadCheapSelectFlag(unsigned char *param);
void fHandleReadCheapSelectConfig(unsigned char *param);
void fHandleReadCheapSelectCountDown(unsigned char *param);
void fHandleReadDailyDay(unsigned char *param);
void fHandleReadDailyMonth(unsigned char *param);
void fHandleReadDailyYear(unsigned char *param);
void fHandleReadhTE_HighestTempRead(unsigned char *param);
void fHandleReadhTE_WarningReason(unsigned char *param);
void fHandleReadeE_EnergyEfficiencyDutyFF(unsigned char *param);
void fHandleReadMercekErrorShadow1(unsigned char *param);
void fHandleReadMercekErrorShadow2(unsigned char *param);
void fHandleReadQuickIce60(unsigned char *param);
void fHandleReadRGBHandleLight(unsigned char *param);
void fHandleReadVacuumBox(unsigned char *param);
void fHandleReadKeylockDspr(unsigned char *param);
#else
void Connectivity_ResetErrors(void);
void toggleWifiState(wifi_states_t state);
void setWifiState(unsigned char state);
void Connectivity_Reset(void);
void rcRequest(remotecontrolRequests request, uint8_t val);
#endif // CONNECTIVITY
/** @} */

#elif defined(FILE_REMOTE_CONTROL_C) // CONNECTIVITY
void rcUpdateCycleCountImmediately(void) {;}


#undef INTERFACE // Should not let this roam free
#endif // FILE_REMOTE_CONTROL_H

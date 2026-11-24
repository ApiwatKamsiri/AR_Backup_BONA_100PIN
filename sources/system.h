#ifndef SYSTEM_H
#define SYSTEM_H

#include "core/core.h"
#include "board/board.h"
// the next definations are used in autobuild script also
#define UXPURE_CORE_VER 1
#define UXPURE_CORE_REV 15	//Add reset record

/* Connectivity - Start */
#define _NON_CONNECTED (0)
#define _WIFI_CONNECTED (1)
#define _BLE_CONNECTED (2)

#define  NONE_AVAILABLE (0)
#define  DEF_AVAILABLE (1)


#if (SAFIR_AVAILABILITY == COMP_AVAILABLE)
#define CONNECTIVITY _WIFI_CONNECTED
#define ENERGY_MONITORING DEF_AVAILABLE
#endif

/// @brief type of pointer to process handler function
typedef void (*tfProcessHandler)(tsProcess *);

/// @brief system processes' enumeration values (ID's)
typedef enum
{
    eProcessRefrigerator = 1,
    eProcessUi,
    eProcessBackup,
    eProcessInverterCom,
#ifdef ENERGYMONITORING
    eProcessEnergyMonitoring, 
#endif
    eProcessCooler,
    eProcessIceMachine,
    eProcessIceSource,
    eProcessWaterSource,
    eProcessDispenser,
    eProcessCabin0,
    eProcessCabin1,
    eProcessCabin2,
    eProcessCabin3,
    eProcessDisplay,
    eProcessEEprom,
    eProcessGdm,
    eProcessCcm,
    eProcessCcmManager,
    eProcessWifiArray,
    eProcessOta,
    eProcessConsumption,
    eProcessRemoteControlDriver,
    eProcessFeature,
} teSystemProcesses;

extern tsProcess *sysProcesses[];

/// @brief Common events shared between all system processes
typedef enum
{

    eSysEventInitAfterStartup = 1,
    eSysEventInitAfterHotStart,
    eSysEventReinit,
    eSysEventEnterState,

    eSysEventStartProductionTestMode,
    eSysEventStartNormalMode,
    eSysEventStartDistributerMode,
    eSysEventStartApplianceOffMode,
    eSysEventStartServiceMode,
    eSysEventStartFTCMode,
    eSysEventStartFairMode,
    eSysEventStartCommercialTestMode,                                            //APW : Add CT heater software
    eSysEventStartCommercialDefrostTestMode,                                     //APW : Add CT heater software
    
    eSysEventCalibrateAllStepperMotors, // separate them for service & defrost ?
    eSysEventDisplayCommunication,

    // Dont' touch this event
    eSysEventCount,
} teSystemEvents;

typedef enum
{
    eSysBuzzerNone = 0,
    eSysBuzzerPlayOpenMelody,
    eSysBuzzerPlayCloseMelody,
    eSysBuzzerPlayStartMelody,
    eSysBuzzerPlayEndMelody,
    eSysBuzzerPlayButtonMelody,
    eSysBuzzerPlayWarningMelody,
    eSysBuzzerPlayDoorsAlertAHTH,               //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
    eSysBuzzerPlayDoorsAlertMelody = 10,        //OCE  if buzzer on the display , it should be 10
} teSystemBuzzerCommands;

void sysStart(void);
void sysProcChangeState(tsProcess *process, tfProcessHandler stateHandler);
void sysBuzzerCmd(teSystemBuzzerCommands cmd);

#endif
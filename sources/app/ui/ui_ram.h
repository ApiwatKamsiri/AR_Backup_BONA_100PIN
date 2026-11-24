#ifndef UI_RAM_H
#define UI_RAM_H

#include "dev/display_spi.h"
#include "components/ui_components_ram.h"
#include "procedures/ui_procedures_ram.h"
#include "states/ui_states_ram.h"
typedef enum
{
  _UI_FTCState_BlinkLed,
}enUI_FTCStates;


typedef struct 
{
	uint32_t dspOffTimer;
	uint8_t warningTimer;
	uint8_t alertTimer;
	uint8_t keyLockWarningCounter;
	uint8_t keyLockWarningTimer;
	uint8_t iceOffWarningTimer;
	uint8_t setChangeTimer;
	uint8_t customCalibrateTimer;
	uint8_t commercialTimer;                                                //APW : Add CT heater software
}tsUiTimers;

typedef struct
{
	uint8_t dspSelector;
	teDisplayEvent receivedEvent;
}tsDspValues;

typedef struct
{
  uint8_t timer;
  enUI_FTCStates state;
}tsUiFTCStateData;

typedef struct 
{
	tsUiNormalStateRam normal;
	tsUiDistributerStateRam  distributer;
	tsUiFTCStateData ftc;
	tsUiServiceStateRam service;
    tsUiCommercialStateRam commercial;                                      //APW : Add CT heater software
}tsUiStatesRam;

typedef struct 
{
    tsDisplayPacket dspPacket;
    tsUiTimers timers;
	tsUiServiceButtonData serviceButton;
	tsSlides slides;
	tsUiErrors errors;
	tsDspValues dspValues;
	tsUiPasswordData password;
	tsUiIceMachineData iceMachine;
	teUiSabbathState sabbathState;
	tsUiStatesRam states;
	tsUiWifiData wifi;
}tsUiRam;


#endif
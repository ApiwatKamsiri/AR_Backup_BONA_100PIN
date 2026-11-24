#ifndef ICE_SOURCE_RAM_H
#define ICE_SOURCE_RAM_H

#include "app/refrigerator/components_ram.h"

typedef enum{
  _IceSourceValue_OFF,
  _IceSourceValue_ICE_CUBE,
  _IceSourceValue_ICE_CRASH,
}enIceSourceValues;

typedef enum{
  _SelectorState_Unknown,
  _SelectorState_IceCubeInit,
  _SelectorState_IceCubeSelecting,
  _SelectorState_IceCubeSelected, 
  _SelectorState_IceCrashInit,
  _SelectorState_IceCrashSelecting,
  _SelectorState_IceCrashSelected,
}enSelectorStates;

typedef enum{
  _ShutterState_Unknown,
  
  _ShutterState_OpenInit,  
  _ShutterState_Opening,
  _ShutterState_Opened,
  
  _ShutterState_CloseInit,
  _ShutterState_Closing,
  _ShutterState_Closed,
}enShutterStates;

typedef enum{ 
  _PusherState_OFF,
  _PusherState_Pushing,
}enPusherStates;

typedef struct{   
  enIceSourceValues dispenserDesired:2;
  enIceSourceValues dispenserApplied:2;
  
  enSelectorStates selectorState:3;
  enShutterStates  shutterState:3;
  enPusherStates   pusherState:1;
    
  unsigned int selectorTimer;
  unsigned int shutterTimer;
  unsigned int pusherTimer;
  
  unsigned char startTimer;
  unsigned char stopTimer;
  
  unsigned char delaySelectionTimer;
  unsigned char delayShutterTimer;
  
  unsigned char flagForShutter;
  unsigned char timerOnOff;
  unsigned char iceDispenserWaitTime;
  unsigned char dispanserRepushTimer;
}stIceSourceValues;

typedef struct{
  stSelector_Ram selector;
  stShutter_Ram shutter;
  stPusher_Ram pusher;
}stIceSourceComponents_Ram;

typedef struct{
  stIceSourceValues values;
  stIceSourceComponents_Ram components;
}stIceSource_Ram;

#endif
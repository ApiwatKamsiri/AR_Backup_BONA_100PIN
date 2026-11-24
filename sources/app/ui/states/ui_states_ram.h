#ifndef UI_STATES_RAM_H
#define UI_STATES_RAM_H

#include "service_mode_procedures/ui_service_mode_procedures_ram.h"

typedef enum
{
    _UI_NormalState_AllLedsOn_Init,
    _UI_NormalState_AllLedsOn,
    _UI_NormalState_DispVersionRevision_Init,
    _UI_NormalState_DispVersionRevision,
    _UI_NormalState_VersionRevision_Init,
    _UI_NormalState_VersionRevision,
    _UI_NormalState_TempUnit_Init,
    _UI_NormalState_TempUnit,
    _UI_NormalState_Sets_Init,
    _UI_NormalState_Sets,
    _UI_NormalState_RealTemp_Init,
    _UI_NormalState_RealTemp,
    _UI_NormalState_Error_Init,
    _UI_NormalState_Error,
    _UI_NormalState_DisplayOff_Init,
    _UI_NormalState_DisplayOff,
    _UI_NormalState_Password,
    _UI_NormalState_Sabbath,
    _UI_NormalState_WifiConfig,
} enUI_NormalStates;

typedef struct
{
    enUI_NormalStates state;    // state of ui in normal state
    unsigned short displayButtonPressedTimer;
    unsigned char generalTimer;
    unsigned char errorTimer;
} tsUiNormalStateRam;

/*** APW : Add define display state in CT ***/
typedef enum
{
    _UI_CommercialState_AllLedsOn_Init,                 
    _UI_CommercialState_AllLedsOn,                      
    _UI_CommercialState_DispVersionRevision_Init,       
    _UI_CommercialState_DispVersionRevision,           
    _UI_CommercialState_VersionRevision_Init,          
    _UI_CommercialState_VersionRevision,               
    _UI_CommercialState_TempUnit_Init,                  
    _UI_CommercialState_TempUnit,                       
    _UI_CommercialState_Sets_Init,                      
    _UI_CommercialState_Sets,                           
    _UI_CommercialState_RealTemp_Init,                 
    _UI_CommercialState_RealTemp,                       
    _UI_CommercialState_Error_Init,                     
    _UI_CommercialState_Error,                          
    _UI_CommercialState_DisplayOff_Init,                 
    _UI_CommercialState_DisplayOff,                    
    _UI_CommercialState_Password,                       
    _UI_CommercialState_Sabbath,                        
    _UI_CommercialState_WifiConfig,                     
} enUI_CommercialStates;

typedef enum
{
    _UI_Commercial_None = 0,
    _UI_Commercial_Test,
    _UI_Commercial_DefrostTest,
} enUI_CommercialStatus;

typedef struct
{
    enUI_CommercialStates state;                        
    unsigned short displayButtonPressedTimer;           
    unsigned char generalTimer;                        
    unsigned char errorTimer;                           
    enUI_CommercialStatus status;
    unsigned char testSwitchRelease :1;
    unsigned char FlagCommercialEnable:1;               
    unsigned char ForceWriteFlageCommercial:1;          
    unsigned char ClearStatusCommercial:1;              
    unsigned char ProhibitionCommercialUpdateEEP:1;
    unsigned char FlagCommercialDisableTimer;           
} tsUiCommercialStateRam;
/*** APW : Add define display state in CT ***/

///////////////////////////////

#define _UI_Distributer_Sets_Time               250 // 250 * 20ms = 5 sec


/*
typedef enum
{
    _UI_DistributerState_AllLedsOn,
    _UI_DistributerState_VersionRevision,
    _UI_DistributerState_RealTemp,
    _UI_DistributerState_Sets,
    _UI_DistributerState_Password,
    _UI_DistributerState_Sabbath,  
}enUI_DistributerStates;
*/

typedef enum
{
    _UI_DistributerState_AllLedsOn_Init,
    _UI_DistributerState_AllLedsOn,
    _UI_DistributerState_DispVersionRevision_Init,
    _UI_DistributerState_DispVersionRevision,
    _UI_DistributerState_VersionRevision_Init,
    _UI_DistributerState_VersionRevision,
    _UI_DistributerState_TempUnit_Init,
    _UI_DistributerState_TempUnit,
    _UI_DistributerState_Sets_Init,
    _UI_DistributerState_Sets,
    _UI_DistributerState_RealTemp_Init,
    _UI_DistributerState_RealTemp,
    // _UI_Distributer_Error_Init,
    // _UI_Distributer_Error,
    _UI_DistributerState_DisplayOff_Init,
    _UI_DistributerState_DisplayOff,
    _UI_DistributerState_Password,
    _UI_DistributerState_Sabbath,
    _UI_DistributerState_WifiConfig,
} enUI_DistributerStates;

typedef struct 
{
    unsigned char timer;
#ifdef HOSHI_H30    
    unsigned char DemoDisplaytimer;        //Demo Display
    unsigned char DemoDisplayStep;        //Demo Display
#endif //HOSHI_H30       
    enUI_DistributerStates state;
}tsUiDistributerStateRam ;

//////////////////////////////////


typedef enum
{
    _UI_ServiceState_BlinkLed,
    _UI_ServiceState_DispVersionRevision,
    _UI_ServiceState_VersionRevision,
    _UI_ServiceState_ShowNTC,
    _UI_ServiceState_AllOnAllOff,
    _UI_ServiceState_ShowComponent,
    _UI_ServiceState_ShowParameter,
} enUI_ServiceStates;

typedef union {
    struct
    {
        unsigned char selectedChange : 1;
        unsigned char specialsChange : 1;
        unsigned char specialsOnOff : 1;
        unsigned char allOff : 1;
    };
    unsigned char alls;
} unUI_ServiceFlags;

typedef struct
{
    enUI_ServiceStates state;
    unUI_ServiceFlags flags;
    unsigned char timer;
    tsUiSmCompsData comps;
    tsUiSmParametersData params;
    tsUiSmSensorsData sens;
} tsUiServiceStateRam;

////////////////////////////////////////

#if 0
typedef enum
{
  _UI_FTCState_BlinkLed,
} enUI_FTCStates;

typedef struct
{
  enUI_FTCStates state;
  unsigned char timer;
} tsUiFTCStateData;
#endif
#endif
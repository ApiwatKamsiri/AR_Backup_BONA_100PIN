#ifndef WM_RAM_H
#define WM_RAM_h


typedef enum
{
    _WM_ProductionTest = 0,
    _WM_Normal,
    _WM_Distributer,
    _WM_Service,
    _WM_ApplianceOff, 
    _WM_CommercialTest,         //APW : Add operation state in CT
    _WM_CommercialDefrostTest,  //APW : Add operation state in CT

    //#if _DispTest_ENABLE	//Changed by Kyle 2017,07,20
    //_WM_DispTest,
    //#endif	//Changed by Kyle 2017,07,20
    //#if defined( _CompTest_ENABLE )	//Changed by Kyle 2017,07,20
    //_WM_CompTest,
    //#endif	//Changed by Kyle 2017,07,20
    //_WM_FairMode,
    _WM_FTC = 100,

} enWorkModes;


typedef enum
{
    _WM_ProductionTest_State_CompTest,
    _WM_ProductionTest_State_PerformanceAndSensorTests,
    _WM_ProductionTest_State_EndedByError,
    _WM_ProductionTest_State_EndedBySensorNOK,
    _WM_ProductionTest_State_EndedByPerformanceNOK,
    _WM_ProductionTest_State_EndedByNoHotStart,
    _WM_ProductionTest_State_OK,
} enWM_ProductionTest_States;

typedef struct
{
    unsigned char preambleBytes[4]; //to check eeprom wheater first write operation is done or not
    enWorkModes workMode;
    unsigned char fairModeFlag;
} stWorkModeE2_Ram;

typedef struct 
{
    enWM_ProductionTest_States state;
    unsigned short minTimer;
    unsigned short componentTestTimer100Ms;
    bool tempSensorOrderChecked;
}tsWorkModeProductionTestRam;

/*** APW : Add operation state in CT ***/
typedef enum
{
    _WM_CommercialTest_State_CompTest,
    _WM_CommercialTest_State_FirstTest,
    _WM_CommercialTest_State_OK,
    _WM_CommercialTest_JudgmentResult,
    _WM_CommercialTest_State_ValveTest,
    _WM_CommercialTest_State_CoolingTest,
    _WM_CommercialTest_State_DefrostTestStanby,
    _WM_CommercialTest_State_DefrostOnTest,
    _WM_CommercialTest_State_DefrostOffTest,
    _WM_CommercialTest_State_Heater1OnTest,
    _WM_CommercialTest_State_Heater1OffTest,
    _WM_CommercialTest_State_Heater2OnTest,
    _WM_CommercialTest_State_Heater2OffTest,
    _WM_CommercialTest_State_Heater3OnTest,
    _WM_CommercialTest_State_Heater3OffTest,
    _WM_CommercialTest_State_Heater4OnTest,
    _WM_CommercialTest_State_Heater4OffTest,
    _WM_CommercialTest_State_Heater5OnTest,
    _WM_CommercialTest_State_Heater5OffTest,
    _WM_CommercialTest_State_DefrostTestComplete,
} enWM_CommercialTest_States;

typedef enum
{
    _WM_General_ARMMode_Off,
    _WM_General_ARMMode_On_Always,
    _WM_CommercialTest_ARMMode_On_ClearDataStep,
#ifdef HOSHI_H30
    _WM_General_ARMMode_On_FRZSensorAbnormal,                   //For HOSHI
    _WM_General_ARMMode_On_FFSensorAbnormal,                    //For HOSHI
    _WM_General_ARMMode_On_AmbientSensorAbnormal,               //For HOSHI
    _WM_General_ARMMode_On_FRZEVASensorAbnormal,                //For HOSHI
    _WM_General_ARMMode_On_EEPCommunicationError,               //For HOSHI
    _WM_General_ARMMode_On_CompressorError,                     //For HOSHI
    _WM_General_ARMMode_On_CompressorCommunicationError,        //For HOSHI
    _WM_General_ARMMode_On_FRZFanAbnormal,                      //For HOSHI
    _WM_General_ARMMode_On_CondensorFanAbnormal,                //For HOSHI
    _WM_General_ARMMode_On_FFFanAbnormal,                       //For HOSHI
    _WM_General_ARMMode_On_IMUnitAbnormal,                      //For HOSHI
    _WM_General_ARMMode_On_IMSensorAbnormal,                    //For HOSHI
    _WM_General_ARMMode_On_DefrostCircuitAbnormal,              //For HOSHI
    _WM_General_ARMMode_On_PowerSupplyVoltageAbnormal,          //For HOSHI
    _WM_General_ARMMode_On_FreezerNotCool,                      //For HOSHI
    _WM_General_ARMMode_On_FreshfoodNotCool,                    //For HOSHI
    _WM_General_ARMMode_On_HuminitySensorAbnormal,              //For HOSHI
    _WM_General_ARMMode_On_IceInspectionAbnormal,               //For HOSHI
    _WM_General_ARMMode_On_HighTemperatureAbnormal,             //For HOSHI
    _WM_General_ARMMode_On_ControlPanelCommunicationError,      //For HOSHI
    _WM_General_ARMMode_On_FSSensorAbnormal,                    //For HOSHI
    _WM_General_ARMMode_On_PingTestError,                       //For BONA
    _WM_CommercialTest_ARMMode_On_ValveErrorStep,               //For BONA
#else
    _WM_General_ARMMode_On_PingTestError,                       //For BONA
    _WM_CommercialTest_ARMMode_On_ValveErrorStep,               //For BONA
#endif //HOSHI_H30
} enWM_workMode_ARMMode;

typedef struct 
{
    enWM_CommercialTest_States state;
    unsigned short minTimer;
    unsigned short SecOnFanCommercialTimer;
    unsigned short MinOnValveCommercialTimer;
    unsigned short componentTestTimer100Ms;
    unsigned short DefrostTestTimer100Ms;
    unsigned char ValveWorkStageCommercial;
    bool tempSensorOrderChecked;
    bool FLAP_Fan_Request;
    bool FF_Fan_Request;
    bool FRZ_Fan_Request;
    bool FF_Valve_Request;
    bool FRZ_Valve_Request;
    bool PrvValvePositionAtValveTest;
    bool SensorTempRecord;
    bool LastSensorTempRecord;
    bool PrvPingTestInCT;
}tsWorkModeCommercialTestRam;

typedef struct 
{
    bool ARMLightUpMode;
    unsigned char ARMBlinkTimes;
    unsigned short ARMTimer100Ms;                                               //APW : Update I2C driver for add CT state
    unsigned short ARMStayTimer1s;                                              //APW : Update for CT process
    unsigned short ARMStayTimer1Min;                                            //APW : Update I2C driver for add CT state
#ifdef HOSHI_H30
    bool FRZSensorAbnormal;                                                     //For HOSHI
    bool FFSensorAbnormal;                                                      //For HOSHI
    bool AmbientSensorAbnormal;                                                 //For HOSHI
    bool FRZEVASensorAbnormal;                                                  //For HOSHI
    bool EEPCommunicationError;                                                 //For HOSHI
    bool CompressorError;                                                       //For HOSHI
    bool CompressorCommunicationError;                                          //For HOSHI
    bool FRZFanAbnormal;                                                        //For HOSHI
    bool CondensorFanAbnormal;                                                  //For HOSHI
    bool FFFanAbnormal;                                                         //For HOSHI
    bool IMUnitAbnormal;                                                        //For HOSHI
    bool IMSensorAbnormal;                                                      //For HOSHI
    bool DefrostCircuitAbnormal;                                                //For HOSHI
    bool PowerSupplyVoltageAbnormal;                                            //For HOSHI
    bool FreezerNotCool;                                                        //For HOSHI
    bool FreshfoodNotCool;                                                      //For HOSHI
    bool HuminitySensorAbnormal;                                                //For HOSHI
    bool IceInspectionAbnormal;                                                 //For HOSHI
    bool HighTemperatureAbnormal;                                               //For HOSHI
    bool ControlPanelCommunicationError;                                        //For HOSHI
    bool FSSensorAbnormal;                                                      //For HOSHI
    bool PingTestError;                                                         //For BONA
    bool ValveARM;                                                              //For BONA
#else
    bool PingTestError;                                                         //For BONA
    bool ValveARM;                                                              //For BONA
#endif //HOSHI_H30
}tsWorkARMModeErrorRam;
/*** APW : Add CT heater software ***/

typedef struct
{
    enWorkModes workMode;
    unsigned short workModeTimer;
    unsigned char fairModeFlag;
    enWM_workMode_ARMMode ARMMode;                                              //APW : Update I2C driver for add CT state
    //unsigned char CompDelayInHeaterTestTimer100Ms;                              //APW : Update for CT process
    tsWorkModeProductionTestRam prodTest;
    tsWorkModeCommercialTestRam CommercialTest;                                 //APW : Add CT heater software
    tsWorkARMModeErrorRam ARMModeError;                                         //APW : Add ARM mode for HOSHI/BONA
} stWorkModeValues;

typedef struct
{
    stWorkModeE2_Ram E2;
    stWorkModeValues values;
} stWorkMode_Ram;

//extern enWorkModes WorkMode;
//extern stWorkMode_Ram workMode_R;

void WorkModeE2ToRam(stWorkMode_Ram *workMode_RPtr);
void WorkModeE2Init(stWorkMode_Ram *workMode_RPtr);
void WorkModeE2AfterHotStart(stWorkMode_Ram *workMode_RPtr);
void WorkModeE2AfterService(stWorkMode_Ram *workMode_RPtr);

#endif
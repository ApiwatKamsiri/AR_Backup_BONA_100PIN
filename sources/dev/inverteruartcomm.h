/** @file       inverteruartcomm.h
 *  @brief      INVERTERUARTCOMM application header file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     baris kara
 *  @date       6.05.2024
 */
#ifndef FILE_INVERTERUARTCOMM_H
#define FILE_INVERTERUARTCOMM_H

/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "system.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_INVERTERUARTCOMM_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   INVERTERUARTCOMM INVERTERUARTCOMM
 *  @ingroup    APP
 *  @brief      inverter commpressor communication
 *  @{
 */
#define INVERTER_REGUEST_TIMEMS 250
#define INVERTER_TIMEOUT_MS 1000
#define INVERTER_REGUEST_FASTLOOP_TIMEMS 2000
#define INVERTER_REGUEST_LOOP_TIMEMS 5000
#define INVERTER_OFFTIME 10000
typedef enum
{
    TX_ID = 0,         // 0
    TX_COMMAND,        // 1
    TX_DATA_L,         // 2
    TX_DATA_H,         // 3
    TX_CHKSM,          // 4
    TX_MAX,            // 5
}enTXIndex;



typedef enum
{
    __IDLE = 0,
    _SENDING_DATA,
    _RECEIVING_HANDSHAKE_0,
    _RECEIVING_HANDSHAKE_1,
    _RECEIVING_SIZE,
    _RECEIVING_DATA,
    _RECEIVING_FINISH,
}enProtocolStates;



typedef enum
{
    RX_HANDSHAKE_0,                     // 0  0XAA  //170
    RX_HANDSHAKE_1,                 // 1  0X55  //85
    RX_PACKET_SIZE,                 // 2
    RX_BOARD_SKU_ID1_0,                 // 3
    RX_BOARD_SKU_ID1_1,                 // 4
    RX_BOARD_SKU_ID2_0,                 // 5
    RX_BOARD_SKU_ID2_1,                 // 6
    RX_BOARD_SKU_ID3_0,                 // 7
    RX_BOARD_SKU_ID3_1,                 // 8
    RX_BOARD_SW_VER_REV_0,          // 9
    RX_BOARD_SW_VER_REV_1,          // 10
    RX_BOARD_SW_VER_REV_2,          // 11
    RX_BOARD_SW_VER_REV_3,          // 12
    RX_INPUT_DC_BUS_VOLTAGE_0,          // 13
    RX_INPUT_DC_BUS_VOLTAGE_1,          // 14
    RX_REFERANCE_SPEED_0,               // 15
    RX_REFERANCE_SPEED_1,               // 16
    RX_MECHANICAL_SPEED_0,              // 17
    RX_MECHANICAL_SPEED_1,              // 18
    RX_REC_REFERANCE_SPEED_0,           // 19
    RX_REC_REFERANCE_SPEED_1,           // 20
    RX_ALARM_CODE_0,                    // 21
    RX_ALARM_CODE_1,                    // 22
    RX_ALARM_TRIAL_COUNTER,             // 23
    RX_ALARM_CYCLE_COUNTER,             // 24
    RX_3_PHASE_CURRENT_SQUARE_0,        // 25
    RX_3_PHASE_CURRENT_SQUARE_1,        // 26
    RX_WORKING_STATUS,                  // 27
    //RX_THERMOSTAT_STATE,                // 28
    RX_POWER_DQ_ACTUAL_0,               //28
    RX_POWER_DQ_ACTUAL_1,               //29
    RX_MOTOR_ON_TIME_0,                 // 30
    RX_MOTOR_ON_TIME_1,                 // 31
    RX_MOTOR_OFF_TIME_0,                // 32
    RX_MOTOR_OFF_TIME_1,                // 33
    RX_CRC,                         //34
    //RX_CTMODE_STATUS,             //35 	APW : Disable because Inverter don't send to Main   
    RX_END,                         //36    //35
    RX_MAX,                         //37    //36
    RX_ID = RX_HANDSHAKE_0,
    RX_COMMAND = RX_HANDSHAKE_1,
    RX_DATA_L = RX_PACKET_SIZE,
    RX_DATA_H = RX_BOARD_SKU_ID1_0,
    RX_CHKSM = RX_BOARD_SKU_ID1_1,
}enRXIndex;

typedef enum
{
    HANDLER_IDLE = 0,
    HANDLER_SENT,
    HANDLER_RECEIVING,
    HANDLER_BUSY,
    HANDLER_ERROR,
}enHandlerResult;

typedef enum
{
    TRANSFER_FINISHED = 0,
    RECEIVE_FINISHED,
}enUartStatus;

typedef enum
{
    REQUEST_NONE = 0,
    REQUEST_ID,
    REQUEST_SET_SPEED,
    REQUEST_READ_SPEED,
    REQUEST_READ_POWER,
    REQUEST_READ_BUSVOLTAGE,
    REQUEST_READ_REALSPEED,
    REQUEST_STATUS,
    REQUEST_SET_CTMODE,         // APW : Now we don't have inverter for support this
    REQUEST_END,
}enRequestType;

typedef enum
{
    ERROR_NONE                                  = 0x0000,
    ERROR_OVERLOAD_PROTECTION                   = 0x0002,
    ERROR_SPEED_OUT_OF_RANGE                    = 0x0080,
    ERROR_OVER_TEMP_FAIL_SPEED_REDUCED          = 0x0020,
    ERROR_COMPRESSOR_STOPPED                    = 0xFF00,
    ERROR_START_FAIL                            = 0xFF01,
    ERROR_OVERLOAD_CONDITION                    = 0xFF02,
    ERROR_UNDER_SPEED                           = 0xFF04,
    ERROR_WRONG_ROTOR_POSITION                  = 0xFF08,
    ERROR_MOTOR_CABLE_INVERTER_FAIL             = 0xFF10,
    ERROR_OVER_TEMP_FAIL_COMPRESSOR_STOPPED     = 0xFF20,
    ERROR_END                                   = 0xFFFF,
}enErrorType;

typedef enum
{
    ERROR_CODE_NONE                               = 0x00,
    ERROR_CODE_BYTE_4                             = 0xF0,
    ERROR_CODE_CHCKSUM_BYTE_5                     = 0xF2,
    ERROR_CODE_COMMAND_BYTE_2                     = 0xF4,
    ERROR_CODE_BYTE_3                             = 0xF8,
    ERROR_CODE_END                                = 0xFF,
}enErrorCode;

typedef enum
{
    COMPRESSOR_TYPE_NONE = 0,
    COMPRESSOR_TYPE_EKI,
    COMPRESSOR_TYPE_JIAXIPERA,
    COMPRESSOR_TYPE_EMBRACO,
    COMPRESSOR_TYPE_TWOINONE,
    COMPRESSOR_TYPE_END,
}enCompressorType;

typedef enum
{
    COMP_ONOFFSTATUS_NONE = 0,
    COMP_ONOFFSTATUS_ON,
    COMP_ONOFFSTATUS_OFFPREPARE,
    COMP_ONOFFSTATUS_OFF,
    COMP_ONOFFSTATUS_END,
}enCompOnOffStatusType;


typedef struct
{     /*EKI PACKETS*/
     unsigned short      BoardSkuID1;
     unsigned short      BoardSkuID2;
     unsigned short      BoardSkuID3;
     unsigned char     BoardVerRev0;
     unsigned char     BoardVerRev1;
     unsigned char     BoardVerRev2;
     unsigned char     BoardVerRev3;
     unsigned short      InverterDCbusVoltage;
     unsigned short      MotorSpeedReferance;
     unsigned short      MotorMechanicalSpeed;
     unsigned short      MotorExtSpeedReferance;
     unsigned short      MotorInverterAlarmCode;
     unsigned char     TrialCounter;
     unsigned char     CycleCounter;
     unsigned short      MotorCurrentVector;
     unsigned char     WorkingStatus;
     unsigned short      PowerDQActual;
     unsigned short      MotorOnTime;
     unsigned short      MotorOffTime;
     //unsigned char       ReadCTStatus;   //APW : Disable because Inverter don't send to Main   
}stEKIPackets;

typedef struct
{
    unsigned short  Power;
    unsigned short  BusVoltage;
    unsigned short  RealSpeed;
    unsigned short  StatusData;
}stEMBC_JIAXPackets;

typedef struct
{
    enCompressorType compressorType;
    enRequestType    requestType;
    enErrorType      errorType;
    enHandlerResult  handlerResult;

    stEKIPackets     EKIPackets;
    stEMBC_JIAXPackets Embc_Jiax_Packets;

    unsigned short sendedSpeed;
    unsigned short readSpeed;
    enCompOnOffStatusType onOffStatus;
    bool sendedCTStatus;        //APW : Now we don't have inverter for support this
    bool PrvCTStatus;           //APW : Now we don't have inverter for support this
    unsigned char CountOfCTPackets;     //APW : Now we don't have inverter for support this

    struct
    {
       bool isRequestActive;

       bool isCompressorSpeedChanged;
       bool isCompressorSpeedValidated;

       bool isPowerNeeded;
       bool isBusVoltageNeeded;
       bool isRealSpeedNeeded;
       bool isStatusNeeded;

       bool isErrorActive;
       bool isCommunicationErrorActive;
       
       bool isCTMODEStatusChanged;   //APW : Now we don't have inverter for support this
       enErrorCode communicationErrorType;
    }status;

}stCompressor;
/// @brief  Events of a InverterUartComm process
typedef enum
{
    eInverterEventsNone,
    eInverterEventPeriodic250Ms,
    eInverterEventPeriodic1Sec,
    eInverterEventPeriodic2Sec,
    eInverterEventPeriodic5Sec,
    eInverterEventInvOffTime
} teInverterUartCommEvents;

typedef struct
{
    tsTimerEvent periodic250Ms;
    tsTimerEvent periodic1Sec;
    tsTimerEvent periodic2Sec;
    tsTimerEvent periodic5Sec;
    tsTimerEvent invOffTimer;
} tsInverterUartCommProcTimers;

/// @brief  Parameters of a InverterUartComm process
typedef struct
{
    uint8_t txBuffer[TX_MAX];
    uint8_t rxBuffer[RX_MAX];
    tsInverterUartCommProcTimers te;
    stCompressor compressorComm;
} tsInverterUartCommParams;

/// @brief  Constants of a InverterUartComm process
typedef struct
{
    const tsDevCom *com;         ///< Communication device that will be used for data transfer
     //const tsDevIo *devio;    //APW : Disable for new MCU
} tsInverterUartCommConsts;



/** @brief  InverterUartComm process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_INVERTER_UART_COMM_CREATE(_name, _enum, _com/*, _dev*/) \
    tsInverterUartCommParams _name##Params =           \
        {                                         \
          .txBuffer = {0,0,0,0,0},  \
          .rxBuffer = {0,0,0,0,0},  \
          .te = {                                                                       \
             .periodic250Ms = TIMER_EVENT_INIT(_enum, _enum, eInverterEventPeriodic250Ms), \
             .periodic1Sec = TIMER_EVENT_INIT(_enum, _enum, eInverterEventPeriodic1Sec), \
             .periodic2Sec = TIMER_EVENT_INIT(_enum, _enum, eInverterEventPeriodic2Sec), \
             .periodic5Sec = TIMER_EVENT_INIT(_enum, _enum, eInverterEventPeriodic5Sec), \
             .invOffTimer = TIMER_EVENT_INIT(_enum, _enum, eInverterEventInvOffTime), \
          },  \
        };                                            \
    const tsInverterUartCommConsts _name##Consts =     \
        {                                         \
        .com             = &(_com),     \
/*        .devio             =&( _dev),*/     \
    };                                            \
    PROCESS_CREATE(_name, inverterUartCommInit, inverterUartCommDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_INVERTERUARTCOMM_H

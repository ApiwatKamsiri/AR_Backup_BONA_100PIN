/** @file       inverteruartcomm.c
 *  @brief      INVERTERUARTCOMM application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     baris kara
 *  @date       6.05.2024
 */
#define FILE_INVERTERUARTCOMM_C
#include "app/refrigerator/rf_ram.h"
#include "inverteruartcomm.h"
#include "..\RCOS\libs\crc.h"
#include <string.h>
uint16_t speedCompGlobal = 0;
#define DEBUG_FILE_NAME "invcom"
/*********************************************************************
 * TYPEDEFS
 **********************************************************************/
#define HANDSHAKE_CHAR                          0x3F    // '?'
#define HANDSHAKE_ANSW_0                        0xAA
#define HANDSHAKE_ANSW_1                        0x55
#define HANDSHAKE_EMBRACO                       0x5A

#define CMD_TX_ID                               0xA5
#define CMD_TX_SET                              0xC3
#define CMD_TX_READ                             0x3C

#ifdef New_algorithm       //New algorithm (APW)
#define CMD_TX_SET_CTMODE                       0x99    //lucky number
#endif 

#define CMD_TX_READ_DATA_L_SPEED                0x80
#define CMD_TX_READ_DATA_L_STATUS               0x83
#define CMD_TX_READ_DATA_L_POWER                0x82
#define CMD_TX_READ_DATA_L_BUSVOLTAGE           0x84
#define CMD_TX_READ_DATA_L_REALSPEED            0x86
#define CMD_TX_READ_DATA_H                      0x39

#define CMD_TX_READ_ID_L                        0x99
#define CMD_TX_READ_ID_H                        0x66

#define CMD_RX_RESPONSE_ID                      0x5A
#define CMD_RX_RESPONSE_SET_SPEED               CMD_TX_READ_DATA_L_STATUS
#define CMD_RX_RESPONSE_READ_SPEED              CMD_TX_READ_DATA_L_SPEED
#define CMD_RX_RESPONSE_READ_POWER              CMD_TX_READ_DATA_L_POWER
#define CMD_RX_RESPONSE_READ_BUSVOLTAGE         CMD_TX_READ_DATA_L_BUSVOLTAGE
#define CMD_RX_RESPONSE_READ_REALSPEED          CMD_TX_READ_DATA_L_REALSPEED
#define CMD_RX_RESPONSE_READ_STATUS             CMD_RX_RESPONSE_SET_SPEED
#define CMD_RX_RESPONSE_READ_ID                 0x31

#define CMD_DATA_EKI_L                          0xA0
#define CMD_DATA_EKI_H                          0x50

#define CMD_DATA_JIAXIPERA_L                    0x0A
#define CMD_DATA_JIAXIPERA_H                    0x05

#define CMD_DATA_TWOINONE_L                     0x0B
#define CMD_DATA_TWOINONE_H                     0x05

#define EMBRACO_PACKET_SIZE        5


#define isRequestExists(_val) ((_val > REQUEST_NONE) && (_val < REQUEST_END))

#define isCommunicationErrorExists(_val) ((_val == ERROR_CODE_BYTE_4)||(_val == ERROR_CODE_CHCKSUM_BYTE_5)||(_val == ERROR_CODE_COMMAND_BYTE_2)||(_val == ERROR_CODE_BYTE_3))

#define TIMEOUT_TIME 10 // TIMEOUT_TIME*100ms
#define STATUS_REQUEST_TIME 60 // STATUS_REQUEST_TIME*1sec
unsigned char TimeoutTimer = TIMEOUT_TIME;


static uint8_t TX_Packet[TX_MAX] = {0}; // Update This TX Packet

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup INVERTERUARTCOMM
 *  @{
 */

// TODO fonk. degiskenler static olarak tanimlanacak

uint8_t compComm_calculateCheckSum(uint8_t *v)
{
    uint8_t i, chksm = 0;

    for (i = 0; i < 4; i++)
    {
        chksm += v[i];
    }

    chksm = 0x100 - (chksm & 0xFF);

    return chksm;
}

uint8_t compComm_CalCRC(uint8_t n, uint8_t *v)
{
    uint8_t i, crc = 0;

    for (i = 0; i < n; i++)
    {
        crc = crc8MaximTable[crc ^ v[i]];
    }
    return crc;
}
void compComm_requestDeviceID(void)
{
    TX_Packet[TX_ID]            = CMD_TX_ID;
    TX_Packet[TX_COMMAND]       = CMD_TX_READ;
    TX_Packet[TX_DATA_L]        = CMD_TX_READ_ID_L;
    TX_Packet[TX_DATA_H]        = CMD_TX_READ_ID_H;
    TX_Packet[TX_CHKSM]         = compComm_calculateCheckSum(TX_Packet);
}

void compComm_setDesiredSpeedPacket(unsigned short speed)
{
    TX_Packet[TX_ID]            = CMD_TX_ID;
    TX_Packet[TX_COMMAND]       = CMD_TX_SET;//0xC3;//
    TX_Packet[TX_DATA_L]        = speed & 0x00FF; //0xA4;//
    TX_Packet[TX_DATA_H]        = ((speed & 0xFF00)>>8);//0x06;//
    TX_Packet[TX_CHKSM]         = compComm_calculateCheckSum(TX_Packet);//0x20;//
}

void compComm_requestSpeedPacket(void)
{
    TX_Packet[TX_ID]            = CMD_TX_ID;
    TX_Packet[TX_COMMAND]       = CMD_TX_READ;
    TX_Packet[TX_DATA_L]        = CMD_TX_READ_DATA_L_SPEED;
    TX_Packet[TX_DATA_H]        = CMD_TX_READ_DATA_H;
    TX_Packet[TX_CHKSM]         = compComm_calculateCheckSum(TX_Packet);
}

void compComm_requestStatusPacket(void)
{
    TX_Packet[TX_ID]            = CMD_TX_ID;
    TX_Packet[TX_COMMAND]       = CMD_TX_READ;
    TX_Packet[TX_DATA_L]        = CMD_TX_READ_DATA_L_STATUS;
    TX_Packet[TX_DATA_H]        = CMD_TX_READ_DATA_H;
    TX_Packet[TX_CHKSM]         = compComm_calculateCheckSum(TX_Packet);
}

void compComm_requestPower(void)
{
    TX_Packet[TX_ID]            = CMD_TX_ID;
    TX_Packet[TX_COMMAND]       = CMD_TX_READ;
    TX_Packet[TX_DATA_L]        = CMD_TX_READ_DATA_L_POWER;
    TX_Packet[TX_DATA_H]        = CMD_TX_READ_DATA_H;
    TX_Packet[TX_CHKSM]         = compComm_calculateCheckSum(TX_Packet);
}

void compComm_requestBusVoltage(void)
{
    TX_Packet[TX_ID]            = CMD_TX_ID;
    TX_Packet[TX_COMMAND]       = CMD_TX_READ;
    TX_Packet[TX_DATA_L]        = CMD_TX_READ_DATA_L_BUSVOLTAGE;
    TX_Packet[TX_DATA_H]        = CMD_TX_READ_DATA_H;
    TX_Packet[TX_CHKSM]         = compComm_calculateCheckSum(TX_Packet);
}

void compComm_requestRealSpeed(void)
{
    TX_Packet[TX_ID]            = CMD_TX_ID;
    TX_Packet[TX_COMMAND]       = CMD_TX_READ;
    TX_Packet[TX_DATA_L]        = CMD_TX_READ_DATA_L_REALSPEED;
    TX_Packet[TX_DATA_H]        = CMD_TX_READ_DATA_H;
    TX_Packet[TX_CHKSM]         = compComm_calculateCheckSum(TX_Packet);
}

void compComm_isReceiveSucceed(bool isSucceed, tsInverterUartCommParams *params)
{
    if(isSucceed)
    {
        memset(params->rxBuffer, 0, sizeof(params->rxBuffer));

        params->compressorComm.status.isRequestActive = false;
        params->compressorComm.status.isCommunicationErrorActive = false;
        params->compressorComm.status.communicationErrorType = ERROR_CODE_NONE;
        params->compressorComm.requestType = REQUEST_NONE;
        
#ifdef New_algorithm       //New algorithm (APW)
        if(params->compressorComm.CountOfCTPackets == 0xFF)
          timerEventStop(&params->te.periodic1Sec);
        else
          timerEventStart(&params->te.periodic1Sec , INVERTER_TIMEOUT_MS);
#else   // Old algorithm
        timerEventStop(&params->te.periodic1Sec);       
#endif 
        
        
        
        params->compressorComm.handlerResult = HANDLER_IDLE;
    }
    else
    {
        if(isCommunicationErrorExists(params->rxBuffer[RX_COMMAND]))
        {
            params->compressorComm.status.isCommunicationErrorActive = true;
            params->compressorComm.status.communicationErrorType = (enErrorCode)params->rxBuffer[RX_COMMAND];
            params->compressorComm.handlerResult = HANDLER_ERROR;
        }

        memset(params->rxBuffer, 0, sizeof(params->rxBuffer));

        params->compressorComm.status.isRequestActive = true;
    }
}

#ifdef New_algorithm       //New algorithm (APW)
void compComm_setDesiredCTMODEPacket(bool CTstatus)
{
    TX_Packet[TX_ID]            = CMD_TX_ID;
    TX_Packet[TX_COMMAND]       = CMD_TX_SET_CTMODE;
    TX_Packet[TX_DATA_L]        = (uint8_t)CTstatus;
    TX_Packet[TX_DATA_H]        = 0x00;
    TX_Packet[TX_CHKSM]         = compComm_calculateCheckSum(TX_Packet);
}
void compComm_setTXPacket(enRequestType request, unsigned short speed, bool CTstatus)
#else   //Old algorithm
void compComm_setTXPacket(enRequestType request, unsigned short speed)  
#endif 

{   // buraya !isRequestActive korumas? koyabilirim. Bu sayede s?rekli olarak chcksum hesaplatmam?? olurum.
    if(request == REQUEST_ID)
        compComm_requestDeviceID();
    else if(request == REQUEST_SET_SPEED)
        compComm_setDesiredSpeedPacket(speed);
    else if(request == REQUEST_READ_SPEED)
        compComm_requestSpeedPacket();
    else if(request ==  REQUEST_READ_POWER)
        compComm_requestPower();
    else if(request ==  REQUEST_READ_BUSVOLTAGE)
        compComm_requestBusVoltage();
    else if(request ==  REQUEST_READ_REALSPEED)
        compComm_requestRealSpeed();
    else if(request == REQUEST_STATUS)
        compComm_requestStatusPacket();
    
#ifdef New_algorithm       //New algorithm (APW)
    else if(request == REQUEST_SET_CTMODE)
        compComm_setDesiredCTMODEPacket(CTstatus);
#endif 
    
    else
        memset(TX_Packet, 0, sizeof TX_Packet);

}

void compComm_determineCompressorType(tsInverterUartCommParams *params)
{

    if((params->rxBuffer[RX_ID]==CMD_RX_RESPONSE_ID) &&
    (params->rxBuffer[RX_COMMAND]==CMD_RX_RESPONSE_READ_ID) &&
    (params->rxBuffer[RX_DATA_L]==CMD_DATA_EKI_L) &&
    (params->rxBuffer[RX_DATA_H]==CMD_DATA_EKI_H))
    {
        uint8_t crc = compComm_calculateCheckSum(params->rxBuffer);
        uint8_t receivedCRC = params->rxBuffer[RX_CHKSM];
        if (crc == receivedCRC)
        {
            compComm_isReceiveSucceed(true, params);
            params->compressorComm.compressorType = COMPRESSOR_TYPE_EKI;
        }
    }
    else if((params->rxBuffer[RX_ID]==CMD_RX_RESPONSE_ID) &&
    (params->rxBuffer[RX_COMMAND]==CMD_RX_RESPONSE_READ_ID) &&
    (params->rxBuffer[RX_DATA_L]==CMD_DATA_JIAXIPERA_L) &&
    (params->rxBuffer[RX_DATA_H]==CMD_DATA_JIAXIPERA_H))
    {
        uint8_t crc = compComm_calculateCheckSum(params->rxBuffer);
        uint8_t receivedCRC = params->rxBuffer[RX_CHKSM];
        if (crc == receivedCRC)
        {
            compComm_isReceiveSucceed(true, params);
            params->compressorComm.compressorType = COMPRESSOR_TYPE_JIAXIPERA;
        }
    }
    else if((params->rxBuffer[RX_ID]==CMD_RX_RESPONSE_ID) &&
    (params->rxBuffer[RX_COMMAND]==CMD_RX_RESPONSE_READ_ID) &&
    (params->rxBuffer[RX_DATA_L]==CMD_DATA_TWOINONE_L) &&
    (params->rxBuffer[RX_DATA_H]==CMD_DATA_TWOINONE_H))
    {
        uint8_t crc = compComm_calculateCheckSum(params->rxBuffer);
        uint8_t receivedCRC = params->rxBuffer[RX_CHKSM];
        if (crc == receivedCRC)
        {
            compComm_isReceiveSucceed(true, params);
            params->compressorComm.compressorType = COMPRESSOR_TYPE_TWOINONE;
        }
    }
    else
    {
        compComm_isReceiveSucceed(true, params);
        params->compressorComm.compressorType = COMPRESSOR_TYPE_EMBRACO;
    }

}

void compComm_readSetSpeedAndStatus(tsInverterUartCommParams *params)
{
    if((params->rxBuffer[RX_ID]==CMD_RX_RESPONSE_ID) &&
    (params->rxBuffer[RX_COMMAND]==CMD_RX_RESPONSE_SET_SPEED))
    {
        uint8_t crc = compComm_calculateCheckSum(params->rxBuffer);
        uint8_t receivedCRC = params->rxBuffer[RX_CHKSM];
        if (crc == receivedCRC)
        {
            params->compressorComm.Embc_Jiax_Packets.StatusData = params->rxBuffer[RX_DATA_L] + (params->rxBuffer[RX_DATA_H] << 8);
            params->compressorComm.status.isCompressorSpeedChanged = false;
            params->compressorComm.status.isStatusNeeded = false;
            compComm_isReceiveSucceed(true, params);
        }
        else
        {
            compComm_isReceiveSucceed(false, params);
        }
    }
    else
    {
        compComm_isReceiveSucceed(false, params);
    }
}

void compComm_readSpeed(tsInverterUartCommParams *params)
{

    if((params->rxBuffer[RX_ID]==CMD_RX_RESPONSE_ID) &&
    (params->rxBuffer[RX_COMMAND]==CMD_RX_RESPONSE_READ_SPEED))
    {
        uint8_t crc = compComm_calculateCheckSum(params->rxBuffer);
        uint8_t receivedCRC = params->rxBuffer[RX_CHKSM];
        if (crc == receivedCRC)
        {

            params->compressorComm.EKIPackets.MotorSpeedReferance = params->rxBuffer[RX_DATA_L] + (params->rxBuffer[RX_DATA_H] << 8);
            params->compressorComm.readSpeed = params->rxBuffer[RX_DATA_L] + (params->rxBuffer[RX_DATA_H] << 8);

            if(params->compressorComm.readSpeed  == params->compressorComm.sendedSpeed)
            {
                params->compressorComm.status.isCompressorSpeedValidated = true;
                params->compressorComm.status.isCompressorSpeedChanged = false;
            }
            else
            {
                params->compressorComm.status.isCompressorSpeedChanged = true;
            }
            /*** APW : Add continues send command CT for Type TWOINONE and request real speed needed ***/
            if(params->compressorComm.compressorType == COMPRESSOR_TYPE_TWOINONE)
            {
                params->compressorComm.status.isRealSpeedNeeded = true;
                params->compressorComm.status.isCTMODEStatusChanged = false;
            }
            /*** APW : Add continues send command CT for Type TWOINONE and request real speed needed ***/
            compComm_isReceiveSucceed(true, params);

        }
        else
        {
            compComm_isReceiveSucceed(false, params);
        }
    }
    else
    {
        compComm_isReceiveSucceed(false, params);
    }
}

void compComm_readPower(tsInverterUartCommParams *params)
{
    if((params->rxBuffer[RX_ID]==CMD_RX_RESPONSE_ID) &&
    (params->rxBuffer[RX_COMMAND]==CMD_RX_RESPONSE_READ_POWER))
    {
        uint8_t crc = compComm_calculateCheckSum(params->rxBuffer);
        uint8_t receivedCRC = params->rxBuffer[RX_CHKSM];
        if (crc == receivedCRC)
        {

            params->compressorComm.Embc_Jiax_Packets.Power = params->rxBuffer[RX_DATA_L] + (params->rxBuffer[RX_DATA_H] << 8);
            params->compressorComm.status.isPowerNeeded = false;
            compComm_isReceiveSucceed(true, params);
        }
        else
        {
            compComm_isReceiveSucceed(false, params);
        }
    }
    else
    {
        compComm_isReceiveSucceed(false, params);
    }
}

void compComm_readBusVoltage(tsInverterUartCommParams *params)
{
    if((params->rxBuffer[RX_ID]==CMD_RX_RESPONSE_ID) &&
    (params->rxBuffer[RX_COMMAND]==CMD_RX_RESPONSE_READ_BUSVOLTAGE))
    {
        uint8_t crc = compComm_calculateCheckSum(params->rxBuffer);
        uint8_t receivedCRC = params->rxBuffer[RX_CHKSM];
        if (crc == receivedCRC)
        {

            params->compressorComm.Embc_Jiax_Packets.BusVoltage = params->rxBuffer[RX_DATA_L] + (params->rxBuffer[RX_DATA_H] << 8);
            params->compressorComm.status.isBusVoltageNeeded = false;
            compComm_isReceiveSucceed(true, params);
        }
        else
        {
            compComm_isReceiveSucceed(false, params);
        }
    }
    else
    {
        compComm_isReceiveSucceed(false, params);
    }
}

void compComm_readRealSpeed(tsInverterUartCommParams *params)
{
    if((params->rxBuffer[RX_ID]==CMD_RX_RESPONSE_ID) &&
    (params->rxBuffer[RX_COMMAND]==CMD_RX_RESPONSE_READ_REALSPEED))
    {
        uint8_t crc = compComm_calculateCheckSum(params->rxBuffer);
        uint8_t receivedCRC = params->rxBuffer[RX_CHKSM];
        if (crc == receivedCRC)
        {
            params->compressorComm.Embc_Jiax_Packets.RealSpeed = params->rxBuffer[RX_DATA_L] + (params->rxBuffer[RX_DATA_H] << 8);
            params->compressorComm.status.isRealSpeedNeeded = false;
            compComm_isReceiveSucceed(true, params);
        }
        else
        {
            compComm_isReceiveSucceed(false, params);
        }
    }
    else
    {
        compComm_isReceiveSucceed(false, params);
    }
}

void compComm_Receiver(tsInverterUartCommParams *params)
{

    if (params->rxBuffer[RX_ID] == (CMD_RX_RESPONSE_ID))
    {
        if(params->compressorComm.requestType == REQUEST_ID)
            compComm_determineCompressorType(params);
        else if(params->compressorComm.requestType  == REQUEST_SET_SPEED)
            compComm_readSetSpeedAndStatus(params);
        else if(params->compressorComm.requestType  == REQUEST_READ_SPEED)
            compComm_readSpeed(params);
        else if(params->compressorComm.requestType  == REQUEST_READ_POWER)
            compComm_readPower(params);
        else if(params->compressorComm.requestType  == REQUEST_READ_BUSVOLTAGE)
            compComm_readBusVoltage(params);
        else if(params->compressorComm.requestType  == REQUEST_READ_REALSPEED)
            compComm_readRealSpeed(params);
        else if(params->compressorComm.requestType  == REQUEST_STATUS)
            compComm_readSetSpeedAndStatus(params);
        else
            ;
    }

}


unsigned short  getBoardSkuID1(uint8_t *buf)
{
    return ((buf[RX_BOARD_SKU_ID1_1] << 8) + buf[RX_BOARD_SKU_ID1_0]);
}

unsigned short  getBoardSkuID2(uint8_t *buf)
{
    return ((buf[RX_BOARD_SKU_ID2_1] << 8) + buf[RX_BOARD_SKU_ID2_0]);
}

unsigned short  getBoardSkuID3(uint8_t *buf)
{
    return ((buf[RX_BOARD_SKU_ID3_1] << 8) + buf[RX_BOARD_SKU_ID3_0]);
}

unsigned char getBoardVerRev0(uint8_t *buf)
{
    return (buf[RX_BOARD_SW_VER_REV_0]);
}

unsigned char getBoardVerRev1(uint8_t *buf)
{
    return (buf[RX_BOARD_SW_VER_REV_1]);
}

unsigned char getBoardVerRev2(uint8_t *buf)
{
    return (buf[RX_BOARD_SW_VER_REV_2]);
}

unsigned char getBoardVerRev3(uint8_t *buf)
{
    return (buf[RX_BOARD_SW_VER_REV_3]);
}

unsigned short  getInverterDCbusVoltage(uint8_t *buf)
{
    return ((buf[RX_INPUT_DC_BUS_VOLTAGE_1] << 8) + buf[RX_INPUT_DC_BUS_VOLTAGE_0]);
}

unsigned short  getMotorSpeedReferance(uint8_t *buf)
{
    return ((buf[RX_REFERANCE_SPEED_1] << 8) + buf[RX_REFERANCE_SPEED_0]);
}

unsigned short  getMotorMechanicalSpeed(uint8_t *buf)
{
    return ((buf[RX_MECHANICAL_SPEED_1] << 8) + buf[RX_MECHANICAL_SPEED_0]);
}

unsigned short  getMotorExtSpeedReferance(uint8_t *buf)
{
    return ((buf[RX_REC_REFERANCE_SPEED_1] << 8) + buf[RX_REC_REFERANCE_SPEED_0]);
}

unsigned short  getMotorInverterAlarmCode(uint8_t *buf)
{
    return ((buf[RX_ALARM_CODE_1] << 8) + buf[RX_ALARM_CODE_0]);
}

unsigned char getTrialCounter(uint8_t *buf)
{
    return (buf[RX_ALARM_TRIAL_COUNTER]);
}

unsigned char getCycleCounter(uint8_t *buf)
{
    return (buf[RX_ALARM_CYCLE_COUNTER]);
}

unsigned short  getMotorCurrentVector(uint8_t *buf)
{
    return ((buf[RX_3_PHASE_CURRENT_SQUARE_1] << 8) + buf[RX_3_PHASE_CURRENT_SQUARE_0]);
}

unsigned char getCompWorkingStatus(uint8_t *buf)
{
    return (buf[RX_WORKING_STATUS]);
}

unsigned short getPowerDQActual(uint8_t *buf)
{
    return ((buf[RX_POWER_DQ_ACTUAL_1] << 8) + (buf[RX_POWER_DQ_ACTUAL_0]));
}

unsigned short  getMotorOnTime(uint8_t *buf)
{
    return ((buf[RX_MOTOR_ON_TIME_1] << 8) + buf[RX_MOTOR_ON_TIME_0]);
}

unsigned short  getMotorOffTime(uint8_t *buf)
{
    return ((buf[RX_MOTOR_OFF_TIME_1] << 8) + buf[RX_MOTOR_OFF_TIME_0]);
}

#if 0
unsigned char getCompCTMODEStatus(uint8_t *buf)
{
    return (buf[RX_CTMODE_STATUS]);           // Disable because Inverter don't send to Main  
}
#endif //New algorithm (APW) : Disable because Inverter don't send to Main  

void drvrCompCommGetEKIPackets(tsInverterUartCommParams *params)
{
      params->compressorComm.EKIPackets.BoardSkuID1 =  getBoardSkuID1(params->rxBuffer);
      params->compressorComm.EKIPackets.BoardSkuID2 =  getBoardSkuID2(params->rxBuffer);
      params->compressorComm.EKIPackets.BoardSkuID3 =  getBoardSkuID3(params->rxBuffer);
      params->compressorComm.EKIPackets.BoardVerRev0 = getBoardVerRev0(params->rxBuffer);
      params->compressorComm.EKIPackets.BoardVerRev1 = getBoardVerRev1(params->rxBuffer);
      params->compressorComm.EKIPackets.BoardVerRev2 = getBoardVerRev2(params->rxBuffer);
      params->compressorComm.EKIPackets.BoardVerRev3 = getBoardVerRev3(params->rxBuffer);
      params->compressorComm.EKIPackets.InverterDCbusVoltage = getInverterDCbusVoltage(params->rxBuffer);
      params->compressorComm.EKIPackets.MotorSpeedReferance = getMotorSpeedReferance(params->rxBuffer);
      params->compressorComm.EKIPackets.MotorMechanicalSpeed =  getMotorMechanicalSpeed(params->rxBuffer);
      params->compressorComm.EKIPackets.MotorExtSpeedReferance =  getMotorExtSpeedReferance(params->rxBuffer);
      params->compressorComm.EKIPackets.MotorInverterAlarmCode = getMotorInverterAlarmCode(params->rxBuffer);
      params->compressorComm.EKIPackets.TrialCounter = getTrialCounter(params->rxBuffer);
      params->compressorComm.EKIPackets.CycleCounter = getCycleCounter(params->rxBuffer);
      params->compressorComm.EKIPackets.MotorCurrentVector = getMotorCurrentVector(params->rxBuffer);
      params->compressorComm.EKIPackets.WorkingStatus = getCompWorkingStatus(params->rxBuffer);
      params->compressorComm.EKIPackets.PowerDQActual = getPowerDQActual(params->rxBuffer);
      params->compressorComm.EKIPackets.MotorOnTime  = getMotorOnTime(params->rxBuffer);
      params->compressorComm.EKIPackets.MotorOffTime = getMotorOffTime(params->rxBuffer);

#if 0      
      params->compressorComm.EKIPackets.ReadCTStatus = getCompCTMODEStatus(params->rxBuffer);   // Disable because Inverter don't send to Main  
#endif //New algorithm (APW) : Disable because Inverter don't send to Main      
      
#ifdef New_algorithm //New algorithm (APW)
      params->compressorComm.readSpeed = params->compressorComm.EKIPackets.MotorSpeedReferance;
      
      if(params->compressorComm.readSpeed == params->compressorComm.sendedSpeed)
      {
          params->compressorComm.status.isCompressorSpeedValidated = true;
          params->compressorComm.status.isCompressorSpeedChanged = false;
      }
      else
          params->compressorComm.status.isCompressorSpeedChanged = true;
      
      params->compressorComm.status.isCTMODEStatusChanged = false;
#else   // Old algorithm      
      params->compressorComm.readSpeed = params->compressorComm.EKIPackets.MotorExtSpeedReferance;

      if(params->compressorComm.readSpeed == (params->compressorComm.sendedSpeed/30))
          params->compressorComm.status.isCompressorSpeedValidated = true;
      params->compressorComm.status.isCompressorSpeedChanged = false;
#endif 
      
      params->compressorComm.status.isPowerNeeded = false;
      params->compressorComm.status.isBusVoltageNeeded = false;
      params->compressorComm.status.isRealSpeedNeeded = false;
      params->compressorComm.status.isStatusNeeded = false;

}

void compComm_receiveDone(tsInverterUartCommParams *params)
{
    uint8_t crc = 0;
    if(/*(receiveCount+1)*/ RX_MAX == params->rxBuffer[RX_PACKET_SIZE])
    {
        crc = compComm_CalCRC(params->rxBuffer[RX_PACKET_SIZE]- 2, params->rxBuffer);
        uint8_t receivedCRC = params->rxBuffer[RX_CRC];
        if (crc == receivedCRC)
        {
            /*** APW : add read speed compressor ***/
            drvrCompCommGetEKIPackets(params);
            /*** APW : add read speed compressor ***/
            compComm_isReceiveSucceed(true, params);
        }
        else
        {
            compComm_isReceiveSucceed(false, params);
        }
    }
    else
    {
        compComm_isReceiveSucceed(false, params);
    }
}

void compComm_Receiver_EKI(tsInverterUartCommParams *params)
{
    if (params->rxBuffer[RX_HANDSHAKE_0] == (HANDSHAKE_ANSW_0) &&
    params->rxBuffer[RX_HANDSHAKE_1] == (HANDSHAKE_ANSW_1))
    {
        if (params->rxBuffer[RX_PACKET_SIZE] == (RX_MAX))
        {
            compComm_receiveDone(params);
        }
    }
}

void InverterUartCommDataPrepare(tsInverterUartCommParams *params)
{
    //tsInverterUartCommParams *params       = process->parameters;
    if(params->compressorComm.requestType == REQUEST_NONE)
    {
        if(!params->compressorComm.compressorType)
            params->compressorComm.requestType = REQUEST_ID;
        else if(params->compressorComm.status.isCompressorSpeedChanged)
        {
            params->compressorComm.requestType = REQUEST_SET_SPEED;
            params->compressorComm.sendedSpeed = (unsigned short)(refrigerator_R.Cooler.components.compressor.value*30); // speedCompGlobal;//
            params->compressorComm.status.isCompressorSpeedValidated = false;
        }
        else if(!params->compressorComm.status.isCompressorSpeedValidated)
            params->compressorComm.requestType = REQUEST_READ_SPEED;
        else if(params->compressorComm.status.isPowerNeeded)
            params->compressorComm.requestType = REQUEST_READ_POWER;
        else if(params->compressorComm.status.isBusVoltageNeeded)
            params->compressorComm.requestType = REQUEST_READ_BUSVOLTAGE;
        else if(params->compressorComm.status.isRealSpeedNeeded)
            params->compressorComm.requestType = REQUEST_READ_REALSPEED;
        else if(params->compressorComm.status.isStatusNeeded)
            params->compressorComm.requestType = REQUEST_STATUS;
        else
            params->compressorComm.requestType = REQUEST_NONE;

#ifdef New_algorithm       //New algorithm (APW)
        //APW : For type EKI compressor
        if(params->compressorComm.compressorType == COMPRESSOR_TYPE_EKI) //Comment : Need to add other comp or not(All comp of AHTH will use Eki only or other)
            if(params->compressorComm.requestType != REQUEST_SET_SPEED)
            {
                if(params->compressorComm.status.isCTMODEStatusChanged)
                {
                  params->compressorComm.requestType = REQUEST_SET_CTMODE;
                  params->compressorComm.sendedCTStatus = (bool)refrigerator_R.ui.dspPacket.CtModeState;
                }
                else  
                  params->compressorComm.requestType = REQUEST_NONE;
            }
        //APW : For type TWOINONE compressor
        if(params->compressorComm.compressorType == COMPRESSOR_TYPE_TWOINONE) //Comment : Need to add other comp or not(All comp of AHTH will use Eki only or other)
            if(!params->compressorComm.requestType)
            {
                if(params->compressorComm.status.isCTMODEStatusChanged)
                {
                  params->compressorComm.requestType = REQUEST_SET_CTMODE;
                  params->compressorComm.sendedCTStatus = (bool)refrigerator_R.ui.dspPacket.CtModeState;
                }
                else  
                  params->compressorComm.requestType = REQUEST_NONE;
            }
        compComm_setTXPacket(params->compressorComm.requestType, params->compressorComm.sendedSpeed, params->compressorComm.sendedCTStatus);
#else   // Old algorithm
        if(params->compressorComm.compressorType == COMPRESSOR_TYPE_EKI)
            if(params->compressorComm.requestType != REQUEST_SET_SPEED)
                params->compressorComm.requestType = REQUEST_NONE;
        
        compComm_setTXPacket(params->compressorComm.requestType, params->compressorComm.sendedSpeed);
#endif 

        memcpy(params->txBuffer, TX_Packet, TX_MAX);
    
    }
}

static PT_THREAD(inverterUartCommThread)
{
    tsInverterUartCommParams *params       = process->parameters;
    const tsInverterUartCommConsts *consts = process->constants;

    uint8_t failed=0;

    PT_BEGIN();

    for (;;)
    {
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT((devComReceive(consts->com, &params->rxBuffer[0], 1) == 1), 10, 25)
        {
            failed = 1;
        }
        if(!failed)
        {
            if(((params->rxBuffer[0] == CMD_RX_RESPONSE_ID) && (params->compressorComm.compressorType != COMPRESSOR_TYPE_EKI))/* || (params->compressorComm.requestType == REQUEST_ID)*/)
            {
                params->compressorComm.handlerResult = HANDLER_RECEIVING;
                PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT((devComReceive(consts->com, &params->rxBuffer[1], 4) == 4), 10, 100)
                {
                    failed = 1;
                }
                if(!failed)
                {
                    //timerEventStop(&params->te.periodic1Sec); // stop timeout
                    if(params->compressorComm.compressorType == COMPRESSOR_TYPE_EKI)
                       compComm_Receiver_EKI(params);
                    else if((params->compressorComm.compressorType == COMPRESSOR_TYPE_TWOINONE) ||
                    (params->compressorComm.compressorType == COMPRESSOR_TYPE_EMBRACO) ||
                    (params->compressorComm.compressorType == COMPRESSOR_TYPE_JIAXIPERA))
                        compComm_Receiver(params);
                    else if(isRequestExists(params->compressorComm.requestType))
                        compComm_Receiver(params);
                    else
                        ;
                }
            }
            else if(params->rxBuffer[0] == HANDSHAKE_ANSW_0 && params->compressorComm.compressorType == COMPRESSOR_TYPE_EKI)
            {
                PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT((devComReceive(consts->com, &params->rxBuffer[1], 1) == 1), 10, 25)
                {
                         failed = 1;
                }
                if(!failed)
                {
                    if(params->rxBuffer[1] == HANDSHAKE_ANSW_1)
                    {
                        params->compressorComm.handlerResult = HANDLER_RECEIVING;
                        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT((devComReceive(consts->com, &params->rxBuffer[2], 34) == 34), 30, 600)
                        //PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT((devComReceive(consts->com, &params->rxBuffer[2], 35) == 35), 30, 600)      //APW : Disable because Inverter don't send to Main  
                        {
                            failed = 1;
                        }
                        if(!failed)
                        {
                            if(params->compressorComm.compressorType == COMPRESSOR_TYPE_EKI)
                                compComm_Receiver_EKI(params);
                        }

                    }
                }
            }
        }
        //        PT_WHILE();
        //        PT_YIELD();
    }

    PT_END();
}

static PROCESS_HANDLER_PROTO(inverterUartCommHandler);

/// @brief  Initialization function of inverterUartComm
PROCESS_INIT_PROTO(inverterUartCommInit)
{
    tsInverterUartCommParams *params       = process->parameters;
    const tsInverterUartCommConsts *consts = process->constants;
    ADD_RCOS_VERSION("inverterCommProc", "1.0.0")

    PROCESS_STATE_CHANGE(process, inverterUartCommHandler);

    devComInit(consts->com);
//    devIoInit(consts->devio,NULL);    //APW : Disable for new MCU
//    devIoPut(consts->devio,1);  //APW : Disable for new MCU

    timerEventStart(&params->te.periodic250Ms , INVERTER_REGUEST_TIMEMS);
    timerEventStart(&params->te.periodic2Sec , INVERTER_REGUEST_FASTLOOP_TIMEMS);
    timerEventStart(&params->te.periodic5Sec , INVERTER_REGUEST_LOOP_TIMEMS);
    threadStart(process, inverterUartCommThread);

    params->compressorComm.status.isCompressorSpeedValidated = true;
    params->compressorComm.onOffStatus = COMP_ONOFFSTATUS_ON;
    params->compressorComm.CountOfCTPackets = 0xFF;     //Initial count of send CT packets

    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of inverterUartComm
PROCESS_DEINIT_PROTO(inverterUartCommDeinit)
{
    tsInverterUartCommParams *params       = process->parameters;
//    const tsInverterUartCommConsts *consts = process->constants;

    timerEventStop(&params->te.periodic250Ms);
    timerEventStop(&params->te.periodic1Sec);
    timerEventStop(&params->te.periodic2Sec);
    timerEventStop(&params->te.periodic5Sec);

//    devIoPut(consts->devio,0);  //APW : Disable for new MCU
//    devIoDeinit(consts->devio); //APW : Disable for new MCU

    PROCESS_STATE_CHANGE(process, NULL);
    params->compressorComm.onOffStatus = COMP_ONOFFSTATUS_OFF;
    threadStop(process, process->threadFunction); // inverterUartCommThread
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of inverterUartComm
static PROCESS_HANDLER_PROTO(inverterUartCommHandler)
{
    tsInverterUartCommParams *params       = process->parameters;
    const tsInverterUartCommConsts *consts = process->constants;

    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
        case  eInverterEventPeriodic250Ms:
            timerEventStart(&params->te.periodic250Ms, INVERTER_REGUEST_TIMEMS);
            
            if(params->compressorComm.sendedSpeed == 0 && params->compressorComm.readSpeed == 0)
            {
              if(params->compressorComm.onOffStatus == COMP_ONOFFSTATUS_ON)
              {
                params->compressorComm.onOffStatus = COMP_ONOFFSTATUS_OFFPREPARE;
                timerEventStart(&params->te.invOffTimer, INVERTER_OFFTIME);
              }
            }
            else
            {
              if(params->compressorComm.onOffStatus != COMP_ONOFFSTATUS_ON)
              {
                /* Open VCC En*/
//                devIoPut(consts->devio,1);      //APW : Disable for new MCU
                params->compressorComm.onOffStatus = COMP_ONOFFSTATUS_ON;
              }
            }
            
            InverterUartCommDataPrepare(params);
            if(params->compressorComm.requestType && !params->compressorComm.status.isRequestActive)
            {
                params->compressorComm.status.isRequestActive = true;
                params->compressorComm.handlerResult = HANDLER_SENT;
                devComSend(consts->com, params->txBuffer, TX_MAX);
                timerEventStart(&params->te.periodic1Sec, INVERTER_TIMEOUT_MS);
                params->compressorComm.handlerResult = HANDLER_RECEIVING;
            }
            
            /*** APW : Add command send status in CT ***/
            //Add read speed and stock/ver compressor 
            if(params->compressorComm.compressorType == COMPRESSOR_TYPE_EKI)
            {
              refrigerator_R.Cooler.components.compressor.Readvalue = 
                (unsigned char)(params->compressorComm.EKIPackets.MotorMechanicalSpeed/30);
              //Remark : EKI Packets send Stock 10 digits by
              //BoardSkuID1 3 digits, BoardSkuID2 3 digits, BoardSkuID3 4 digits
              //Please don't change pack process because it effect to PWRs/FTC function.
              refrigerator_R.Cooler.components.compressor.Stock.No[0] = 
                params->compressorComm.EKIPackets.BoardSkuID3;
              refrigerator_R.Cooler.components.compressor.Stock.No[1] = 
                params->compressorComm.EKIPackets.BoardSkuID2;
              refrigerator_R.Cooler.components.compressor.Stock.No[2] = 
                params->compressorComm.EKIPackets.BoardSkuID1;
              refrigerator_R.Cooler.components.compressor.Stock.Version =
                params->compressorComm.EKIPackets.BoardVerRev0;
              refrigerator_R.Cooler.components.compressor.Stock.Revision =
                params->compressorComm.EKIPackets.BoardVerRev1;
              refrigerator_R.Cooler.components.compressor.Stock.CoreVersion =
                params->compressorComm.EKIPackets.BoardVerRev2;
              refrigerator_R.Cooler.components.compressor.Stock.CoreRevision =
                params->compressorComm.EKIPackets.BoardVerRev3;
            }
            else
            {
              refrigerator_R.Cooler.components.compressor.Readvalue = 
                (unsigned char)(params->compressorComm.Embc_Jiax_Packets.RealSpeed/30);
              //Remark : EKI Packets send Stock 10 digits by
              //BoardSkuID1 3 digits, BoardSkuID2 3 digits, BoardSkuID3 4 digits
              //Please don't change pack process because it effect to PWRs/FTC function.
              refrigerator_R.Cooler.components.compressor.Stock.No[0] = 0;
              refrigerator_R.Cooler.components.compressor.Stock.No[1] = 0;
              refrigerator_R.Cooler.components.compressor.Stock.No[2] = 0;
              refrigerator_R.Cooler.components.compressor.Stock.Version = 0;
              refrigerator_R.Cooler.components.compressor.Stock.Revision = 0;
              refrigerator_R.Cooler.components.compressor.Stock.CoreVersion = 0;
              refrigerator_R.Cooler.components.compressor.Stock.CoreRevision = 0;
            }
#ifdef New_algorithm       //New algorithm (APW)          
            //Add force send speed when speed not zero in Ct mode
            if(refrigerator_R.ui.states.commercial.status != _UI_Commercial_None && 
                !refrigerator_R.Cooler.components.compressor.value &&
                  refrigerator_R.Cooler.components.compressor.value != (params->compressorComm.sendedSpeed/30))
            {
              params->compressorComm.status.isCompressorSpeedChanged = true;
              InverterUartCommDataPrepare(params);
            }
            
            //Add send command setting when we have change the state
            if(params->compressorComm.PrvCTStatus != (bool)refrigerator_R.ui.dspPacket.CtModeState)
            {
               timerEventStart(&params->te.periodic1Sec, INVERTER_TIMEOUT_MS);
               params->compressorComm.CountOfCTPackets = 0;
               params->compressorComm.status.isCTMODEStatusChanged = true;       
               params->compressorComm.PrvCTStatus = (bool)refrigerator_R.ui.dspPacket.CtModeState;
               params->compressorComm.status.isCompressorSpeedChanged = true;
               InverterUartCommDataPrepare(params);
            }
#endif 
            /*** APW : Add command send status in CT ***/
            
            break;
        case  eInverterEventPeriodic1Sec:
              
#ifdef New_algorithm       //New algorithm (APW)
            if(params->compressorComm.requestType && 
                params->compressorComm.status.isRequestActive)
            {
              memset(params->rxBuffer, 0, sizeof(params->rxBuffer));
              memset(TX_Packet, 0, sizeof TX_Packet);
              params->compressorComm.status.isRequestActive = false;
              params->compressorComm.status.isPowerNeeded = false;
              params->compressorComm.status.isBusVoltageNeeded = false;
              params->compressorComm.status.isRealSpeedNeeded = false;
              params->compressorComm.status.isStatusNeeded = false;
              params->compressorComm.requestType = REQUEST_NONE;
              params->compressorComm.handlerResult = HANDLER_IDLE;
            }
            
            if(params->compressorComm.CountOfCTPackets < 9)
            {
              if(refrigerator_R.Cooler.components.compressor.value != (params->compressorComm.sendedSpeed/30))
                params->compressorComm.status.isCompressorSpeedChanged = true;
              
              if(!params->compressorComm.status.isCTMODEStatusChanged)
              {
                params->compressorComm.status.isCTMODEStatusChanged = true;
                params->compressorComm.CountOfCTPackets += 1;
              }
            }
            else
              params->compressorComm.CountOfCTPackets = 0xFF;
#else   // Old algorithm
              memset(params->rxBuffer, 0, sizeof(params->rxBuffer));
              memset(TX_Packet, 0, sizeof TX_Packet);
              params->compressorComm.status.isRequestActive = false;
              params->compressorComm.status.isPowerNeeded = false;
              params->compressorComm.status.isBusVoltageNeeded = false;
              params->compressorComm.status.isRealSpeedNeeded = false;
              params->compressorComm.status.isStatusNeeded = false;
              params->compressorComm.requestType = REQUEST_NONE;
              params->compressorComm.handlerResult = HANDLER_IDLE;
#endif 
            
            break;
        case  eInverterEventPeriodic2Sec:
            timerEventStart(&params->te.periodic2Sec, INVERTER_REGUEST_FASTLOOP_TIMEMS);
            params->compressorComm.status.isPowerNeeded = true;

            break;
        case  eInverterEventPeriodic5Sec:
            timerEventStart(&params->te.periodic5Sec, INVERTER_REGUEST_LOOP_TIMEMS);

            params->compressorComm.status.isCompressorSpeedChanged = true;
            params->compressorComm.status.isRealSpeedNeeded = true;
            params->compressorComm.status.isBusVoltageNeeded = true;
            

            break;
        case eInverterEventInvOffTime:
          /* Close VCC En*/
          if(params->compressorComm.onOffStatus == COMP_ONOFFSTATUS_OFFPREPARE)
          {
//            devIoPut(consts->devio,0);  //APW : Disable for new MCU
            params->compressorComm.onOffStatus = COMP_ONOFFSTATUS_OFF;
          }
          break;

        default:
            break;
    }
}



/** @} */

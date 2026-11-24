/** @file       spi_slave.h
 *  @brief      Header file of SPI device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       May 2021
 */
#ifndef FILE_SPI_SLAVE_H
#define FILE_SPI_SLAVE_H

/// Includes
#include "rcos.h"
#include "spi_slave.h"
#include "libs/crc.h"
#include "r_spi.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_SPI_SLAVE_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   RA2_SPI_SLAVE SPI_SLAVE
 *  @ingroup    RA2_DEV_COM
 *  @brief      SPI slave device for Renesas RA2
 *  @{
 */

#define DISPLAY_PACK_SIZE 26
#define SPI_ISR_FUNCTION(_name) void _name(spi_callback_args_t *p_args)
 void displayComProcess(const tsDevCom *device);
 void isReadyToConversion(void);
/// @brief  Functions for SPI devices
INTERFACE const tsDevComFuncs devComSpiSlaveFuncs;

/// @brief  Device specific parameters
//typedef struct
//{
//    tsDeque *rxQue;          ///< Receive queue
//    tsDeque *txQue;          ///< Transmit queue
//    uint8_t bytesToSend;     ///< Number of bytes left to send
//    uint8_t sendingLastByte; ///<Last byte needs to be sent from TX DTC
//} tsSpiSlaveParams;

 enum teResponse_Bytes{
   eResponse_Byte_0 = 0,
   eResponse_Byte_1,
   eResponse_Byte_2,
   eResponse_Byte_3, 
 };
 
 typedef enum{
  _SPIState_Start,
  _SPIState_Command,
  _SPIState_Adress,
  _SPIState_Size,
  _SPIState_Command_RX_Data,
  _SPIState_Command_TX_Data,
  _SPIState_Command_RX_Checksum,
  _SPIState_Command_TX_Checksum,

}enSPIState;

typedef struct
{
    enSPIState eSPIState;  
    uint16_t sendSize;                                           ///< Used to be sure of completion of last transmission
    volatile   uint8_t txbuffer[DISPLAY_PACK_SIZE + 1/*CRC*/];   ///< Holds  outgoing data with display
    volatile   uint8_t rxbuffer[DISPLAY_PACK_SIZE + 1 /*CRC*/];   ///< Holds ingoing  data with display
    tsDeque *commandsQueue;                                        ///< holds the commands that will be sent from mb to display
    tsDeque *eventsQueue;                                          ///< holds the events that are received from display to mb
    uint8_t completed;                                           ///< Transmission complete flag
    uint8_t sendingLastByte; ///<Last byte needs to be sent from TX DTC
    uint8_t timeOut;
    uint8_t timeOutComError;
    uint16_t errorDisplayComTimes;      //Add monitor reset completed
    bool startErrorDisplayComTimes;     //Add monitor reset completed
} tsSpiSlaveParams;

/// @brief  Device specific constants
typedef struct
{
    const spi_instance_t *spi; ///< SPI stack pointer
} tsSpiSlaveConsts;


/** @brief  Create a devCom
 *  @param  _name           Name of devCom object
 *  @param  _spi            Name of the SPI stack
 *  @param  _rxBufferSize   Size of receive buffer
 *  @param  _txQueueSize    Size of transmit queue
 *  @attention              Rename all SPI ISR function names to "spi_isr" from FSP Configuration
 */
#define DEV_COM_SPI_SLAVE_CREATE(_name, _spi, _callback, _rxQueueSize, _txQueueSize) \
    const tsDevCom _name;                                                 \
      SPI_ISR_FUNCTION(_callback)                                         \
    {                                                                     \
        spiIsr(&_name, p_args);                                           \
    }                                                                     \
    FIFO_CREATE(_name##Cmd, _rxQueueSize)                                 \
    FIFO_CREATE(_name##Evnt, _txQueueSize)                                \
                                                                          \
      tsSpiSlaveParams _name##Params =                                    \
        {                                                                 \
            .commandsQueue = &_name##Cmd,                                 \
            .eventsQueue = &_name##Evnt,                                  \
            .completed = TRUE,                                            \
            .eSPIState = _SPIState_Start,                                 \
    };                                                                    \
    const tsSpiSlaveConsts _name##Consts =                                \
        {                                                                 \
            &_spi,                                                        \
    };                                                                    \
    DEV_COM_CREATE(_name, devComSpiSlaveFuncs, &_name##Params, &_name##Consts)

/// @brief  Functions to handle SPI receive and transmit complete interrupts. DO NOT CALL OUTSIDE THE TARGET ISR
INTERFACE void spiIsr(const tsDevCom *device, spi_callback_args_t *p_args);

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_SPI_H

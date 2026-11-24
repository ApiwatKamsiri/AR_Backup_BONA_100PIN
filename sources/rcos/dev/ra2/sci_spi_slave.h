/** @file       sci_spi_slave.h
 *  @brief      Header file of SCI_SPI device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       May 2021
 */
#ifndef FILE_SCI_SPI_SLAVE_H
#define FILE_SCI_SPI_SLAVE_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_SCI_SPI_SLAVE_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   RA2_SCI_SPI_SLAVE SCI_SPI_SLAVE
 *  @ingroup    RA2_DEV_COM
 *  @brief      SCI_SPI slave device for Renesas RA2
 *  @{
 */

/// @brief  Functions for SCI_SPI devices
INTERFACE const tsDevComFuncs devComSciSpiSlaveFuncs;

/// @brief  Device specific parameters
typedef struct
{
    tsDeque *rxQue;           ///< Receive queue
    tsDeque *txQue;           ///< Transmit queue
    uint8_t bytesToSend;      ///< Number of bytes left to send
    uint8_t sendingLastByte;  ///<Last byte needs to be sent from TX DTC
    uint8_t lastReceivedByte; ///<Last read byte
} tsSciSpiSlaveParams;

/// @brief  Device specific constants
typedef struct
{
    const spi_instance_t *spi; ///< SCI_SPI stack pointer
} tsSciSpiSlaveConsts;

/** @brief  Create a devCom
 *  @param  _name           Name of devCom object
 *  @param  _spi            Name of the SCI_SPI stack
 *  @param  _rxBufferSize   Size of receive buffer
 *  @param  _txQueueSize    Size of transmit queue
 *  @attention              Rename all SCI SPI ISR function names to "sci_spi_isr" from FSP Configuration
 *  @attention              Let the name of the callback function stay as is (sci_spi_callback)
 */
#define DEV_COM_SCI_SPI_SLAVE_CREATE(_name, _spi, _rxQueueSize, _txQueueSize) \
    const tsDevCom _name;                                                     \
    void sci_spi_callback(spi_callback_args_t *p_args)                        \
    {                                                                         \
        return;                                                               \
    }                                                                         \
    void sci_spi_isr(void)                                                    \
    {                                                                         \
        sciSpiIsr(&_name);                                                    \
    }                                                                         \
    FIFO_CREATE(_name##RxQ, _rxQueueSize)                                     \
    FIFO_CREATE(_name##TxQ, _txQueueSize)                                     \
    tsSciSpiSlaveParams _name##Params =                                       \
        {                                                                     \
            &_name##RxQ,                                                      \
            &_name##TxQ,                                                      \
            0,                                                                \
            FALSE,                                                            \
            0,                                                                \
    };                                                                        \
    const tsSciSpiSlaveConsts _name##Consts =                                 \
        {                                                                     \
            &_spi,                                                            \
    };                                                                        \
    DEV_COM_CREATE(_name, devComSciSpiSlaveFuncs, &_name##Params, &_name##Consts)

/// @brief  Functions to handle SCI SPI receive and transmit complete interrupts. DO NOT CALL OUTSIDE THE TARGET ISR
INTERFACE void sciSpiIsr(const tsDevCom *device);

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_SCI_SPI_H

/** @file       uart.h
 *  @brief      Header file of UART device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       4/14/2021
 *  @author     Vehbi Umur CABUK / Burak OZAN
 *  @date       09/09/2022
 *  @warning    FIFO Should be DISABLE!
 *              If our data pointer points odd address to fifo the hardfault occur!
 */
#ifndef FILE_UART_H
#define FILE_UART_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_UART_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   RA2_UART UART
 *  @ingroup    RA2_DEV_COM
 *  @brief      UART device for Renesas RA2
 *  @{
 */

#define UART_ISR_FUNCTION(_name) void _name(uart_callback_args_t *p_args)

/// @brief  Functions for UART devices
INTERFACE const tsDevComFuncs devComUartFuncs;

/// @brief  Device specific parameters
typedef struct
{
    tsDeque *rxQue;           ///< Receive queue
    tsDeque *txQue;           ///< Transmit queue
    uint8_t lastTransferSize; ///< Size of the latest transfer
    uint8_t txComplete;
} tsUartParams;

/// @brief  Device specific constants
typedef struct
{
    const uart_instance_t *uart; ///< UART stack pointer
} tsUartConsts;

/** @brief  Create a devCom
 *  @param  _name           Name of devCom object
 *  @param  _uart           Name of the UART stack
 *  @param  _callback       Name of the UART interrupt callback function
 *  @param  _rxBufferSize   Size of receive buffer
 *  @param  _txQueueSize    Size of transmit queue
 */
#define DEV_COM_UART_CREATE(_name, _uart, _callback, _rxQueueSize, _txQueueSize) \
    const tsDevCom _name;                                                        \
    UART_ISR_FUNCTION(_callback)                                                 \
    {                                                                            \
        uartIsr(&_name, p_args);                                                 \
    }                                                                            \
    FIFO_CREATE(_name##RxQ, _rxQueueSize)                                        \
    FIFO_CREATE(_name##TxQ, _txQueueSize)                                        \
    tsUartParams _name##Params =                                                 \
        {                                                                        \
            &_name##RxQ,                                                         \
            &_name##TxQ,                                                         \
            0,                                                                   \
            .txComplete = TRUE,                                                  \
    };                                                                           \
    const tsUartConsts _name##Consts =                                           \
        {                                                                        \
            &_uart,                                                              \
    };                                                                           \
    DEV_COM_CREATE(_name, devComUartFuncs, &_name##Params, &_name##Consts)

/// @brief  Functions to handle UART receive and transmit complete interrupts. DO NOT CALL OUTSIDE THE TARGET ISR
INTERFACE void uartIsr(const tsDevCom *device, uart_callback_args_t *p_args);

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_UART_H

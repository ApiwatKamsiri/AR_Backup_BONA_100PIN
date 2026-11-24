/** @file       uart.h
 *  @brief      Header file of UART device
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     halil
 *  @date       16.02.2023
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
 *  @defgroup   UART UART
 *  @ingroup    DEV_COM
 *  @brief      uart
 *  @{
 */
#define UART_ISR_FUNCTION(_name) void _name(void)
#define None UART_Parity_None
#define Even UART_Parity_Even
#define Odd  UART_Parity_Odd
/**
  * @brief   Uart Remap navigation map array
  * 
  * Uart0 = { Num_0 = Tx :Port A-->Pin 3, Rx :Port A-->Pin 4        | Uart1 = { Num_0 = Tx :Port A-->Pin 5, Rx :Port A-->Pin 6        
  *           Num_1 = Tx :Port B-->Pin 6, Rx :Port B-->Pin 7        |           Num_1 = Tx :Port B-->Pin 8, Rx :Port B-->Pin 9
  *           Num_2 = Tx :Port C-->Pin 11, Rx :Port C-->Pin 10      |           Num_2 = Tx :Port C-->Pin 7, Rx :Port C-->Pin 8
  *           Num_3 = Tx :Port D-->Pin 14, Rx :Port D-->Pin 13}     |           Num_3 = Tx :Port C-->Pin 12, Rx :Port C-->Pin 13
  *                                                                 |           Num_4 = Tx :Port D-->Pin 15, Rx :Port E-->Pin 0}
  * .......
  * Uart2 = { Num_0 = Tx :Port A-->Pin 7, Rx :Port A-->Pin 8        | Uart3 = { Num_0 = Tx :Port A-->Pin 0, Rx :Port A-->Pin 1
  *           Num_1 = Tx :Port B-->Pin 10, Rx :Port B-->Pin 11      |           Num_1 = Tx :Port B-->Pin 0, Rx :Port B-->Pin 1
  *           Num_2 = Tx :Port C-->Pin 14, Rx :Port C-->Pin 15      |           Num_2 = Tx :Port C-->Pin 0, Rx :Port C-->Pin 1
  *           Num_3 = Tx :Port E-->Pin 1, Rx :Port E-->Pin 2}       |           Num_3 = Tx :Port D-->Pin 0, Rx :Port D-->Pin 1}
  *           Num_4 = Tx :Port E-->Pin 5, Rx :Port E-->Pin 6}
  * 
  * Uart4 = { Num_0 = Tx :Port A-->Pin 9, Rx :Port A-->Pin 10       | Uart5 = { Num_0 = Tx :Port A-->Pin 11, Rx :Port A-->Pin 12
  *           Num_1 = Tx :Port B-->Pin 2, Rx :Port B-->Pin 3        |           Num_1 = Tx :Port B-->Pin 4, Rx :Port B-->Pin 5
  *           Num_2 = Tx :Port C-->Pin 2, Rx :Port C-->Pin 3        |           Num_2 = Tx :Port C-->Pin 4, Rx :Port C-->Pin 5
  *           Num_3 = Tx :Port D-->Pin 2, Rx :Port D-->Pin 3}       |           Num_3 = Tx :Port D-->Pin 4, Rx :Port D-->Pin 5}
  * 
  */

 typedef enum{
    Num_0,
    Num_1,
    Num_2,
    Num_3,
    Num_4,
    Num_5,
 }RemapNum;

/// @brief  Functions for UART devices
INTERFACE const tsDevComFuncs devComUartFuncs;

/// @brief  Device specific parameters
typedef struct
{
    UART_TypeDef *channel;
    tsDeque *rxQue; ///< Receive queue
    tsDeque *txQue; ///< Transmit queue
    uint8_t completed;
} tsUartParams;

/// @brief  Device specific constants
typedef struct
{
    const RemapNum remapnum;
    IRQn_Type UARTx_IRQn;
    const uint32_t baud;
    const uint32_t stop;
    const uint32_t parity;
} tsUartConsts;

/** @brief  Create a devCom
 *  @param  _name     Name of devCom object
 *  @param  _channel  Uart Channel (UART0 ... UART5)
 *  @param  _remapnum Remap channel number,it can be Num_0,Num_1,Num_2,Num_3,Num_4,Num_5
 *  @param  _stop     Stop bit count (1 or 2)
 *  @param  _parity   Parity (None, Even, Odd)
 *  @param  _rxBufferSize   Size of receive buffer
 *  @param  _txQueueSize    Size of transmit queue
 */
#define DEV_COM_UART_CREATE(_name, _channel, _remapnum, _baud, _stop, _parity, _rxQueueSize, _txQueueSize)      \
        const tsDevCom _name;                   \
    UART_ISR_FUNCTION(_channel##_Handler)       \
    {                                           \
        uartIsr(&_name);                        \
    }                                           \
    FIFO_CREATE(_name##RxQ, _rxQueueSize)       \
    FIFO_CREATE(_name##TxQ, _txQueueSize)       \
    tsUartParams _name##Params =                \
    {                                           \
        .channel =_channel,                     \
        .rxQue = &_name##RxQ,                   \
        .txQue = &_name##TxQ,                   \
        .completed = TRUE,          \
    };                                          \
    const tsUartConsts _name##Consts =          \
    {                                           \
        .remapnum =_remapnum,                   \
        .baud =_baud,                           \
        .stop =_stop,                           \
        .parity =_parity,                       \
        .UARTx_IRQn = _channel##_IRQn,      \
    };                                          \
    DEV_COM_CREATE(_name, devComUartFuncs, &_name##Params, &_name##Consts)
INTERFACE void uartIsr(const tsDevCom *device);
/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_UART_H

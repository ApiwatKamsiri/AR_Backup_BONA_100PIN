/** @file       uart.c
 *  @brief      C file of UART device
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     halil
 *  @date       16.02.2023
 */
#define FILE_UART_C

#include "uart.h"

#include "sh32f9061.h"
#include "sh32f9xx_sb0_retarget.h"
#include "sh32f9xx_sb0_libcfg.h"
#include "sh32f9xx_sb0_uart.h"

/**
 *  @addtogroup UART
 *  @{
 */

static DEV_COM_FUNC_INIT(uartInit);
static DEV_COM_FUNC_DEINIT(uartDeinit);
static DEV_COM_FUNC_OPEN(uartOpen);
static DEV_COM_FUNC_CLOSE(uartClose);
static DEV_COM_FUNC_SEND(uartSend);
static DEV_COM_FUNC_RECEIVE(uartReceive);
// static DEV_COM_FUNC_TICKET_GET(uartTicketGet);
// static DEV_COM_FUNC_TICKET_VALID(uartTicketValid);

/// @brief  Structure that defines the functions for uart
const tsDevComFuncs devComUartFuncs =
    {
        uartInit,
        uartDeinit,
        uartOpen,
        uartClose,
        uartSend,
        uartReceive,
        NULL,
        NULL,
};

/// @brief  Generic interrupt subroutine function for a uart
void uartIsr(const tsDevCom *device)
{
    tsUartParams *params = device->parameters;
    const tsUartConsts *consts = device->constants;
    tsDevComSystem *sys = device->sys;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    if (sys->closed)
    {
        // Close function called before ISR
        sys->closed = NULL;
        TICKET_REMOVE(sys->tvm);
    }
    else
    {
        if (UART_GetFlagStatus(params->channel, UART_FLAG_TC) == SH_SET)
        {
            UART_ClearFlag(params->channel, UART_FLAG_TC);
            
            if (FIFO_LOAD(params->txQue) != 0)
            {
                // Remove the sent bytes from transmit buffer
                dequeRemFront(params->txQue, 1);
            }

            // eger tx fifo bos degilse yeni datayi gonder sonra fifo'dan sil.
            if (FIFO_LOAD(params->txQue) != 0)
            {
                UART_SendData(params->channel, params->txQue->buffer[params->txQue->front]);
            }
            else // gönderme işlemi bitti. fifo artık bomboş.
            {
                params->completed = TRUE;
            }
        }

        if (UART_GetFlagStatus(params->channel, UART_FLAG_RI) == SH_SET)
        {
            UART_ClearFlag(params->channel, UART_FLAG_RI);

            uint8_t temp = (uint8_t)UART_ReceiveData(params->channel);
            FIFO_PUSH(params->rxQue, &temp, 1);
        }
        // ISR arrived but there's no close request yet
        sys->closed = sys->opened;
    }
}

/// @brief  Init function for a uart
static DEV_COM_FUNC_INIT(uartInit)
{
    tsUartParams *params = device->parameters;
    const tsUartConsts *consts = device->constants;

    RCC_AHBPeriphClockOnOff(RCC_AHB_IOCLK, SH_ON);

    UART_InitAF(params->channel, consts->remapnum);

    UART_InitTypeDef UART_InitStructure;

    UART_ClockOnOff(params->channel, SH_ON);
    // RCC_APB1PeriphClockOnOff(RCC_APB1_UART5, SH_ON);

    UART_InitStructure.UART_Mode = UART_Mode_1;
    UART_InitStructure.UART_BaudRate = consts->baud;
    UART_InitStructure.UART_DataLength = UART_DataLength_8Bit;
    UART_InitStructure.UART_StopBits = consts->stop;
    UART_InitStructure.UART_Parity = consts->parity;
    UART_InitStructure.UART_Enable = UART_Enable_Tx | UART_Enable_Rx;
    UART_Init(params->channel, &UART_InitStructure);

    UART_INTConfig(params->channel, UART_INT_RX | UART_INT_TC, SH_ENABLE);
    NVIC_EnableIRQ(consts->UARTx_IRQn);

    // UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    device->sys->initialized = TRUE;

    // params->completed = TRUE;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a uart
static DEV_COM_FUNC_DEINIT(uartDeinit)
{
    tsUartParams *params = device->parameters;
    const tsUartConsts *consts = device->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    device->sys->initialized = FALSE;

    return EXIT_SUCCESS;
}

/// @brief  Open function for a uart
static DEV_COM_FUNC_OPEN(uartOpen)
{
    tsUartParams *params = device->parameters;
    const tsUartConsts *consts = device->constants;
    tsDevComSystem *sys = device->sys;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    if (!sys->initialized)
    {
        return EXIT_FAILURE;
    }

    if (target)
    {
        if (sys->opened || sys->closed)
        {
            // Previous session did not closed yet
            return EXIT_FAILURE;
        }
        else
        {
            sys->opened = target;
            sys->closed = NULL;
            return EXIT_SUCCESS;
        }
    }
    else
    {
        return EXIT_FAILURE;
    }
}

/// @brief  Close function for a uart
static DEV_COM_FUNC_CLOSE(uartClose)
{
    tsUartParams *params = device->parameters;
    const tsUartConsts *consts = device->constants;
    tsDevComSystem *sys = device->sys;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    if (!sys->initialized)
    {
        return EXIT_FAILURE;
    }

    if (sys->opened == target)
    {
        globalIntDisable();
        if (sys->closed)
        {
            // Last operation is already finished in ISR
            sys->closed = NULL;
            TICKET_REMOVE(sys->tvm);
        }
        else
        {
            // Close after ISR
            sys->closed = sys->opened;
        }
        sys->opened = NULL;
        globalIntEnable();

        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

/// @brief  Send function for a uart
static DEV_COM_FUNC_SEND(uartSend)
{
    tsUartParams *params = device->parameters;
    const tsUartConsts *consts = device->constants;
    tsDevComSystem *sys = device->sys;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    if (!sys->initialized)
    {
        return 0;
    }

    if (!length)
    {
        // Return data count in transmit buffer
        length = FIFO_LOAD(params->txQue);
    }
    else // if (!length)
    {
        if (txb == NULL)
        {
            // No operation
            length = 0;
        }
        else // if (txb == NULL)
        {
            if (FIFO_FREE(params->txQue) >= length)
            {

                // globalIntDisable();

                // Place data into transmit buffer
                FIFO_PUSH(params->txQue, txb, length);
                // GÖNDERME İŞLEMİ HALA DEVAM EDİYORSA FİFO'DAN GÖNDERMEYE DEVAM ETMELİ. ZATEN HANDLER İÇİNDE GÖNDERMEYE DEVAM ETMEKTE.
                // AŞAĞIDAKİ SATIRDA true OLUNCA FİFO BOŞKEN YENİ PAKEDİ FİFOYA KOYMUŞ VE GÖNDERME İŞLEMİNİ BAŞLATMIŞTIR.
                if (params->completed == TRUE)
                {
                    UART_SendData(params->channel, params->txQue->buffer[params->txQue->front]);
                    params->completed = FALSE;
                }

                sys->closed = NULL;
                // globalIntEnable();
            }
            else
            {
                length = 0;
            }
        } // else // if (txb == NULL)
    }     // else // if (!length)

    return length;
}

/// @brief  Receive function for a uart
static DEV_COM_FUNC_RECEIVE(uartReceive)
{
    tsUartParams *params = device->parameters;
    const tsUartConsts *consts = device->constants;
    tsDevComSystem *sys = device->sys;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    if (!sys->initialized)
    {
        return 0;
    }

    if (!length)
    {
        // Return data count in receive buffer
        length = FIFO_LOAD(params->rxQue);
    }
    else
    {
        if (rxb == NULL)
        {
            // Dismiss received data
            FIFO_FLUSH(params->rxQue);
        }
        else
        {
            if (FIFO_LOAD(params->rxQue) >= length)
            {
                // Receive complete
                globalIntDisable();
                FIFO_POP(params->rxQue, rxb, length);
                globalIntEnable();
                if (sys->closed)
                {
                    // Close function called before ISR
                    sys->closed = 0;
                    TICKET_REMOVE(sys->tvm);
                }
                else
                {
                    // ISR arrived but there's no close request yet
                    sys->closed = sys->opened;
                }
            }
            else
            {
                length = 0;
            }
        } // else // if (rxb == NULL)
    }     // else // if (!length)

    return length;
}

#if 0 // Enable if uart requires another functionality other than generic ticket functions

/// @brief  Ticket get function for a uart
static DEV_COM_FUNC_TICKET_GET(uartTicketGet)
{
    tsUartParams *params       = device->parameters;
    const tsUartConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;
}

/// @brief  Ticket valid function for a uart
static DEV_COM_FUNC_TICKET_VALID(uartTicketValid)
{
    tsUartParams *params       = device->parameters;
    const tsUartConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;
}

#endif

/** @} */

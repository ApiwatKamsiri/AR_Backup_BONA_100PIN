/** @file       ra2/uart.c
 *  @brief      RA2 Universal Asynchronous Receiver-Transmitter
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       4/14/2021
 *  @author     Vehbi Umur CABUK / Burak OZAN
 *  @date       09/09/2022
 */
#define FILE_UART_C

/**
 *  @addtogroup RA2_UART
 *  @{
 */
#include "uart.h"

static DEV_COM_FUNC_INIT(uartInit);
static DEV_COM_FUNC_DEINIT(uartDeinit);
static DEV_COM_FUNC_OPEN(uartOpen);
static DEV_COM_FUNC_CLOSE(uartClose);
static DEV_COM_FUNC_SEND(uartSend);
static DEV_COM_FUNC_RECEIVE(uartReceive);

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

void uartIsr(const tsDevCom *device, uart_callback_args_t *p_args)
{
    tsUartParams *params       = device->parameters;
    const tsUartConsts *consts = device->constants;
    tsDevComSystem *sys        = device->sys;

    if (UART_EVENT_TX_COMPLETE == p_args->event)
    {
        // Remove the sent bytes from transmit buffer
        dequeRemFront(params->txQue, params->lastTransferSize);

        // There is still data in fifo
        if (FIFO_LOAD(params->txQue))
        {
            fsp_err_t err = FSP_SUCCESS;

            params->lastTransferSize = MIN((params->txQue->size - params->txQue->front), params->txQue->count);
            err = consts->uart->p_api->write(consts->uart->p_ctrl, &params->txQue->buffer[params->txQue->front], params->lastTransferSize);
            RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
        }
        else
        {
            params->txComplete = TRUE;
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
    }

    if (UART_EVENT_RX_CHAR == p_args->event)
    {
        FIFO_PUSH(params->rxQue, (uint8_t *)&p_args->data, 1);
    }

}

/// @brief  Init function for a uart
static DEV_COM_FUNC_INIT(uartInit)
{
    const tsUartConsts *consts = device->constants;
    tsUartParams *params = device->parameters;
    fsp_err_t err = FSP_SUCCESS;

    ADD_RCOS_VERSION("ra2/uart", "1.0.1")

    err = consts->uart->p_api->open(consts->uart->p_ctrl, consts->uart->p_cfg);
    RCOS_ASSERT((FSP_SUCCESS == err) || (FSP_ERR_ALREADY_OPEN == err), RASSERT_PRI_LOW);
    params->txComplete = TRUE;
    device->sys->initialized = TRUE;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a uart
static DEV_COM_FUNC_DEINIT(uartDeinit)
{
    const tsUartConsts *consts = device->constants;
    tsUartParams *params       = device->parameters;
    fsp_err_t err = FSP_SUCCESS;

    err = consts->uart->p_api->communicationAbort(consts->uart->p_ctrl, UART_DIR_RX_TX);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

    err = consts->uart->p_api->close(consts->uart->p_ctrl);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

    params->txComplete = TRUE;
    FIFO_FLUSH(params->txQue);
    FIFO_FLUSH(params->rxQue);

    device->sys->initialized = FALSE;

    return EXIT_SUCCESS;
}

/// @brief  Open function for a uart
static DEV_COM_FUNC_OPEN(uartOpen)
{
    tsDevComSystem *sys = device->sys;

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
    tsDevComSystem *sys = device->sys;

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
    tsUartParams *params       = device->parameters;
    const tsUartConsts *consts = device->constants;

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
            // RA2 FSP HARD FAULT ONLEMI!
            // EGER VERILEN DATA'NIN ADRESI IKININ KATI DEGILSE HARDFAULT URETIYOR.
            // BU SEBEPLE HER DATA KOYUSUMUZDA QUEUE TEK ADRESTE ISE CIFT ADRESE ILERLETILMELI!
            
            if (FIFO_FREE(params->txQue) >= length)
            {
                globalIntDisable();

                // Place data into transmit buffer and activate interrupt
                FIFO_PUSH(params->txQue, txb, length);

                // If Tx not completed then data will send in interrupt.
                if (params->txComplete == TRUE)
                {
                    fsp_err_t err = FSP_SUCCESS;

                    params->txComplete = FALSE;
                    params->lastTransferSize = MIN((params->txQue->size - params->txQue->front), params->txQue->count);

                    err = consts->uart->p_api->write(consts->uart->p_ctrl, &params->txQue->buffer[params->txQue->front], params->lastTransferSize);
                    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
                }
                device->sys->closed = NULL;

                globalIntEnable();
            }
            else
            {
                length = 0;
            }
        } // else // if (txb == NULL)
    }     // else // if (!length)

    return length;
}

static DEV_COM_FUNC_RECEIVE(uartReceive)
{
    tsDevComSystem *sys        = device->sys;
    tsUartParams *params       = device->parameters;
    const tsUartConsts *consts = device->constants;

    if (!length)
    {
        // Return data count in receive buffer
        length = FIFO_LOAD(params->rxQue);
    }
    else // if (!length)
    {
        if (rxb == NULL)
        {
            fsp_err_t err = FSP_SUCCESS;

            // Dismiss received data
            FIFO_FLUSH(params->rxQue);
            err = consts->uart->p_api->communicationAbort(consts->uart->p_ctrl, UART_DIR_RX);
            RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
        }
        else // if (rxb == NULL)
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

/** @} */

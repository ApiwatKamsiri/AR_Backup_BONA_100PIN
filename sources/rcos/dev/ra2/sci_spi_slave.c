/** @file       ra2/sci_spi_slave.c
 *  @brief      RA2 Serial Peripheral Interface
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       6/21/2021
 */
#define FILE_SCI_SPI_C

/**
 *  @addtogroup RA2_SPI
 *  @{
 */
#include "sci_spi_slave.h"

#define SPI_READ(buffer, size) consts->spi->p_api->read(consts->spi->p_ctrl, buffer, size, SPI_BIT_WIDTH_8_BITS)

static DEV_COM_FUNC_INIT(sciSpiInit);
static DEV_COM_FUNC_DEINIT(sciSpiDeinit);
static DEV_COM_FUNC_OPEN(sciSpiOpen);
static DEV_COM_FUNC_CLOSE(sciSpiClose);
static DEV_COM_FUNC_SEND(sciSpiSend);
static DEV_COM_FUNC_RECEIVE(sciSpiReceive);

const tsDevComFuncs devComSciSpiSlaveFuncs =
    {
        sciSpiInit,
        sciSpiDeinit,
        sciSpiOpen,
        sciSpiClose,
        sciSpiSend,
        sciSpiReceive,
        NULL,
        NULL,
};

void sciSpiIsr(const tsDevCom *device)
{
    tsSciSpiSlaveParams *params       = device->parameters;
    const tsSciSpiSlaveConsts *consts = device->constants;
    IRQn_Type irq                     = R_FSP_CurrentIrqGet();
    sci_spi_instance_ctrl_t *ctrl     = (sci_spi_instance_ctrl_t *)R_FSP_IsrContextGet(irq);

    R_BSP_IrqStatusClear(irq);

    if (irq == ctrl->p_cfg->rxi_irq) // SPI RXI interrupt
    {
        uint8_t data = ctrl->p_reg->RDR;

        if (FIFO_IS_EMPTY(params->txQue) && params->bytesToSend) // Sending is finished
        {
            params->bytesToSend     = 0;
            params->sendingLastByte = FALSE;
        }
        else if (FIFO_IS_EMPTY(params->txQue)) // Don't fill receive buffer while sending
        {
            FIFO_PUSH(params->rxQue, (const void *)&data, 1);
        }
    }
    else if (irq == ctrl->p_cfg->txi_irq) // SPI TXI interrupt
    {
        if (FIFO_LOAD(params->txQue))
        {
            FIFO_POP(params->txQue, (void *)&ctrl->p_reg->TDR, 1);
            params->bytesToSend == 1 ? 1 : params->bytesToSend--;

            if (FIFO_IS_EMPTY(params->txQue) && !params->sendingLastByte) // FIFO is empty but there is still data in transmit register
            {
                uint16_t data           = 0;
                params->sendingLastByte = TRUE;
                FIFO_PUSH(params->txQue, &data, sizeof(data));
            }
        }
        else
        {
            ctrl->p_reg->TDR = 0;
        }
    }
    else if (irq == ctrl->p_cfg->eri_irq)
    {
        ctrl->p_reg->SSR;
        ctrl->p_reg->SSR = 0;
    }
}

/// @brief  Init function for a spi
static DEV_COM_FUNC_INIT(sciSpiInit)
{
    ADD_RCOS_VERSION("ra2/sci_spi_slave", "1.0.0")

    device->sys->initialized = TRUE;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a spi
static DEV_COM_FUNC_DEINIT(sciSpiDeinit)
{
    const tsSciSpiSlaveConsts *consts = device->constants;
    tsSciSpiSlaveParams *params       = device->parameters;

    // Close the SPI interface
    consts->spi->p_api->close(consts->spi->p_ctrl);

    // Clear RX and TX queues
    FIFO_CLEAR(params->txQue);
    FIFO_CLEAR(params->rxQue);

    device->sys->initialized = FALSE;

    return EXIT_SUCCESS;
}

/// @brief  Open function for a spi
static DEV_COM_FUNC_OPEN(sciSpiOpen)
{
    tsSciSpiSlaveParams *params       = device->parameters;
    const tsSciSpiSlaveConsts *consts = device->constants;

    if (!device->sys->initialized)
    {
        return EXIT_FAILURE;
    }

    // Clear RX and TX queues
    FIFO_FLUSH(params->rxQue);
    FIFO_FLUSH(params->txQue);

    // Open SPI interface
    consts->spi->p_api->open(consts->spi->p_ctrl, consts->spi->p_cfg);

    // Start receiving data
    SPI_READ(&params->lastReceivedByte, 1);

    return EXIT_SUCCESS;
}
/// @brief  Close function for a spi
static DEV_COM_FUNC_CLOSE(sciSpiClose)
{
    const tsSciSpiSlaveConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;

    if (!sys->initialized)
    {
        return EXIT_FAILURE;
    }

    // Close the SPI interface
    consts->spi->p_api->close(consts->spi->p_ctrl);

    return EXIT_SUCCESS;
}

/// @brief  Send function for a spi
static DEV_COM_FUNC_SEND(sciSpiSend)
{
    tsSciSpiSlaveParams *params       = device->parameters;
    const tsSciSpiSlaveConsts *consts = device->constants;

    if (!device->sys->initialized)
    {
        return EXIT_FAILURE;
    }

    if (!length)
    {
        // Return data count in transmit buffer
        length = params->bytesToSend;
    }
    else // if (!length)
    {
        if (txb == NULL)
        {
            // No operation
        }
        else // if (txb == NULL)
        {
            if (FIFO_FREE(params->txQue) >= length)
            {
                globalIntDisable();
                // Place data into transmit buffer
                FIFO_PUSH(params->txQue, txb, length);
                params->bytesToSend = FIFO_LOAD(params->txQue);
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

static DEV_COM_FUNC_RECEIVE(sciSpiReceive)
{
    tsDevComSystem *sys         = device->sys;
    tsSciSpiSlaveParams *params = device->parameters;

    if (!sys->initialized)
    {
        EXIT_FAILURE;
    }

    if (!length)
    {
        length = FIFO_LOAD(params->rxQue);
    }
    else // if (!length)
    {
        if (rxb == NULL)
        {
            // Dismiss received data
            FIFO_FLUSH(params->rxQue); // Empty RX buffer
        }
        else // if (rxb == NULL)
        {
            if (FIFO_LOAD(params->rxQue) >= length)
            {
                globalIntDisable();
                FIFO_POP(params->rxQue, rxb, length);
                globalIntEnable();
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

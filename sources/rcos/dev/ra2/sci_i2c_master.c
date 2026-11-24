/** @file       sci_i2c_master.c
 *  @brief      C file of I2C device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Enes Inanc
 *  @date       20.10.2021
 */
#define FILE_I2C_C
#define RESET_VALUE             (0x00)

#include "sci_i2c_master.h"

/**
 *  @addtogroup I2C
 *  @{
 */
/* Callback function */
void i2c_Callback(i2c_master_callback_args_t *p_args, const tsDevCom *device)
{
    tsI2cParams *params = device->parameters;

    ///< Call back event capture variable is assigned as below;
    ///< "I2C_MASTER_EVENT_ABORTED" or "I2C_MASTER_EVENT_RX_COMPLETE" or "I2C_MASTER_EVENT_TX_COMPLETE"
    params->flag = p_args->event;
}


/// @brief  Validate whether send or receive process was done
///         If timeout, return abort info
static i2c_master_event_t validate_i2c_event(tsI2cParams *params)
{
    uint16_t local_time_out = UINT16_MAX;

    /* resetting call back event capture variable */
    params->flag = (i2c_master_event_t)RESET_VALUE;

    do
    {
        /* This is to avoid infinite loop */
        --local_time_out;

        if(RESET_VALUE == local_time_out)
        {
            return I2C_MASTER_EVENT_ABORTED;
        }

    }while(params->flag == RESET_VALUE);

    if(params->flag != I2C_MASTER_EVENT_ABORTED)
    {
        params->flag = (i2c_master_event_t)RESET_VALUE;  // Make sure this is always Reset before return
        return FSP_SUCCESS;
    }

    params->flag = (i2c_master_event_t)RESET_VALUE; // Make sure this is always Reset before return
    return I2C_MASTER_EVENT_ABORTED;
}

static DEV_COM_FUNC_INIT(i2cInit);
static DEV_COM_FUNC_DEINIT(i2cDeinit);
static DEV_COM_FUNC_OPEN(i2cOpen);
static DEV_COM_FUNC_CLOSE(i2cClose);
static DEV_COM_FUNC_SEND(i2cSend);
static DEV_COM_FUNC_RECEIVE(i2cReceive);
// static DEV_COM_FUNC_TICKET_GET(i2cTicketGet);
// static DEV_COM_FUNC_TICKET_VALID(i2cTicketValid);

/// @brief  Structure that defines the functions for i2c
const tsDevComFuncs devComI2cFuncs =
    {
        i2cInit,
        i2cDeinit,
        i2cOpen,
        i2cClose,
        i2cSend,
        i2cReceive,
        NULL,
        NULL,
};

/// @brief  Init function for a i2c
static DEV_COM_FUNC_INIT(i2cInit)
{
    tsI2cParams *params = device->parameters;
    const tsI2cConsts *consts = device->constants;
    ADD_RCOS_VERSION("ra2/sci_i2c_master", "1.0.1")

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    device->sys->initialized = TRUE;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a i2c
static DEV_COM_FUNC_DEINIT(i2cDeinit)
{
    tsI2cParams *params = device->parameters;
    const tsI2cConsts *consts = device->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    device->sys->initialized = FALSE;

    return EXIT_SUCCESS;
}

/// @brief  Open function for a i2c
static DEV_COM_FUNC_OPEN(i2cOpen)
{
    tsI2cParams *params = device->parameters;
    const tsI2cConsts *consts = device->constants;
    tsDevComSystem *sys = device->sys;

    fsp_err_t err = FSP_SUCCESS;

    if (!sys->initialized)
    {
        return EXIT_FAILURE;
    }

    if (target)
    {
        if (sys->opened || sys->closed) 
        {
            ///< Previous session did not closed yet
            return EXIT_FAILURE;
        }
        else
        {
            ///< Open I2C bus, Renesas internal function, take contol&config structure variables
            err = consts->i2c_master_on_sci->open(params->i2c_master_ctrl, params->i2c_master_cfg);
            if (FSP_SUCCESS != err)
            {
                ///< I2C bus did not opened
                return EXIT_FAILURE;
            }

            ///< I2C device slave address given by the user is set
            err = consts->i2c_master_on_sci->slaveAddressSet(params->i2c_master_ctrl, target->slave, I2C_MASTER_ADDR_MODE_7BIT);
            if (FSP_SUCCESS != err)
            {
                ///< I2C device slave address did not set
                return EXIT_FAILURE;
            }
            else
            {
                ///< I2C device slave address is set
                sys->opened = target;
                sys->closed = NULL;
            }

            ///< Write on I2C bus, Renesas internal function, take contol variable structure, dont send restart info
            err = consts->i2c_master_on_sci->write(params->i2c_master_ctrl, NULL, 0x00, false);
            if (FSP_SUCCESS != err)
            {
                return EXIT_FAILURE;
            }
            else
            {
                err = validate_i2c_event(device->parameters);
                /* handle error */
                if(FSP_SUCCESS != err)
                {
                    return EXIT_FAILURE;
                }
                else
                {
                    return EXIT_SUCCESS;
                }
            }
        }
    }
    else
    {
        return EXIT_FAILURE;
    }
}

/// @brief  Close function for a i2c
static DEV_COM_FUNC_CLOSE(i2cClose)
{
    tsI2cParams *params = device->parameters;
    const tsI2cConsts *consts = device->constants;
    tsDevComSystem *sys = device->sys;

    fsp_err_t err = FSP_SUCCESS;

    if (!sys->initialized)
    {
        return EXIT_FAILURE;
    }

    if (sys->opened == target)
    {
        //globalIntDisable();

        ///< Close I2C bus, Renesas internal function, take contol structure variable
        err = consts->i2c_master_on_sci->close(params->i2c_master_ctrl);
        if (FSP_SUCCESS != err)
        {
            return EXIT_FAILURE;
        }
        else
        {
            sys->opened = NULL;
            sys->closed = NULL;
            TICKET_REMOVE(sys->tvm);
            return EXIT_SUCCESS;
        }

        sys->opened = NULL;
        //globalIntEnable();

        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

/// @brief  Send function for a i2c
static DEV_COM_FUNC_SEND(i2cSend)
{
    tsI2cParams *params = device->parameters;
    const tsI2cConsts *consts = device->constants;
    tsDevComSystem *sys = device->sys;

    fsp_err_t err = FSP_SUCCESS;

    if (!sys->initialized)
    {
        return 0;
    }

    if (!length)
    {
        length = 0;
    }
    else // if (!length)
    {
        if (txb == NULL)
        {
            length = 0;
        }
        else // if (txb == NULL)
        {   
            ///< Write on I2C bus, Renesas internal function, take contol variable structure, dont send restart info
            err = consts->i2c_master_on_sci->write(params->i2c_master_ctrl, (uint8_t *)(txb), length, false);

            if (FSP_SUCCESS != err)
            {
                return EXIT_FAILURE;
            }
            else
            {
                err = validate_i2c_event(device->parameters);
                /* handle error */
                if(FSP_SUCCESS != err)
                {
                    return EXIT_FAILURE;
                }
                else
                {
                    return EXIT_SUCCESS;
                }
            }

        } // else // if (txb == NULL)
    }     // else // if (!length)

    return length;
}

/// @brief  Receive function for a i2c
static DEV_COM_FUNC_RECEIVE(i2cReceive)
{
    tsI2cParams *params = device->parameters;
    const tsI2cConsts *consts = device->constants;
    tsDevComSystem *sys = device->sys;

    fsp_err_t err = FSP_SUCCESS;

    if (!sys->initialized)
    {
        return 0;
    }

    if (!length)
    {
        length = 0;
    }
    else // if (!length)
    {
        if (rxb == NULL)
        {
            // Dismiss received data
        }
        else // if (rxb == NULL)
        {
            params->flag = 0;

            ///< Write on I2C bus, Renesas internal function, take contol variable structure, dont send restart info
            err = consts->i2c_master_on_sci->read(params->i2c_master_ctrl, rxb, length, false);
            if (FSP_SUCCESS != err)
            {
                return EXIT_FAILURE;
            }
            else
            {
                err = validate_i2c_event(device->parameters);
                /* handle error */
                if(FSP_SUCCESS != err)
                {
                    return EXIT_FAILURE;
                }
                else
                {
                    sys->closed = NULL;
                    return EXIT_SUCCESS;
                }
            }

        } // else // if (rxb == NULL)
    }     // else // if (!length)

    return length;
}

#if 0 // Enable if i2c requires another functionality other than generic ticket functions

/// @brief  Ticket get function for a i2c
static DEV_COM_FUNC_TICKET_GET(i2cTicketGet)
{
    tsI2cParams *params       = device->parameters;
    const tsI2cConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;
}

/// @brief  Ticket valid function for a i2c
static DEV_COM_FUNC_TICKET_VALID(i2cTicketValid)
{
    tsI2cParams *params       = device->parameters;
    const tsI2cConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;
}

#endif

/** @} */

/** @file       dev.c
 *  @brief      RCoS+ abstract device driver structures and definitions
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       5.11.2018
 */
#define FILE_DEV_C

#include <stdio.h>  ///< included for vsprintf in devComPrint
#include "dev.h"

/**
 *  @addtogroup DEV_IO
 *  @{
 */
void addDevVersion()
{
    ADD_RCOS_VERSION("dev", "2.0.0")
}

DEV_IO_FUNC_INIT(devIoInit)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->init)
            {
                return device->functions->init(device, config);
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_IO_FUNC_DEINIT(devIoDeinit)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->deinit)
            {
                return device->functions->deinit(device);
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_IO_FUNC_GET(devIoGet)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->get)
            {
                return device->functions->get(device);
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_IO_FUNC_PUT(devIoPut)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->put)
            {
                return device->functions->put(device, data);
            }
        }
    }
    return EXIT_FAILURE;
}
/** @} */

/**
 *  @addtogroup DEV_COM
 *  @{
 */
DEV_COM_FUNC_INIT(devComInit)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->init)
            {
                return device->functions->init(device);
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_COM_FUNC_DEINIT(devComDeinit)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->deinit)
            {
                return device->functions->deinit(device);
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_COM_FUNC_OPEN(devComOpen)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->open)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->open(device, target);
                }
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_COM_FUNC_CLOSE(devComClose)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->close)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->close(device, target);
                }
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_COM_FUNC_SEND(devComSend)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->send)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->send(device, txb, length);
                }
            }
        }
    }
    return 0;
}

DEV_COM_FUNC_RECEIVE(devComReceive)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->receive)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->receive(device, rxb, length);
                }
            }
        }
    }
    return 0;
}

DEV_COM_FUNC_TICKET_GET(devComTicketGet)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->ticketGet)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->ticketGet(device); // Device specific ticket get function
                }
            }
            else
            {
                // Generic ticket get
                DEV_COM_FUNC_TICKET_GET_GENERIC(device);
            }
        }
    }
    return 0;
}

DEV_COM_FUNC_TICKET_VALID(devComTicketValid)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->ticketValid)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->ticketValid(device, ticket); // Device specific ticket validation function
                }
            }
            else
            {
                // Generic ticket validation
                DEV_COM_FUNC_TICKET_VALID_GENERIC(device);
            }
        }
    }
    return FALSE;
}

uint16_t _devComVPrint(const tsDevCom *device, const char *format, va_list args)
{
    char array[256]; // warning! long text may overflow
    int retVal = 0;

    retVal = vsnprintf(array, sizeof(array), format, args);

    if (retVal)
    {
        retVal = devComSend(device, array, retVal);
    }
    else
    {
        retVal = 0;
    }

    return retVal;
}

uint16_t devComPrint(const tsDevCom *device, const char *format, ...)
{
    int length = 0;

    if (device != NULL)
    {
        if (device->sys->initialized)
        {
            va_list args;

            va_start(args, format);

            length = _devComVPrint(device, format, args);

            va_end(args);
        }
    }

    return length;
}
/** @} */

/**
 *  @addtogroup DEV_MEM
 *  @{
 */
DEV_MEM_FUNC_INIT(devMemInit)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->init)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->init(device);
                }
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_MEM_FUNC_DEINIT(devMemDeinit)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->deinit)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->deinit(device);
                }
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_MEM_FUNC_READ(devMemRead)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->read)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->read(device, address, readData, length);
                }
            }
        }
    }
    return 0;
}

DEV_MEM_FUNC_WRITE(devMemWrite)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->write)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->write(device, address, writeData, length);
                }
            }
        }
    }
    return 0;
}

DEV_MEM_FUNC_PROGRAM(devMemProgram)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->program)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->program(device, address, progData, length);
                }
            }
        }
    }
    return 0;
}

DEV_MEM_FUNC_ERASE(devMemErase)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->erase)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->erase(device, address, size);
                }
            }
        }
    }
    return 0;
}

DEV_MEM_FUNC_TICKET_GET(devMemTicketGet)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->ticketGet)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->ticketGet(device); // Device specific ticket get function
                }
            }
            else
            {
                // Generic ticket get
                DEV_MEM_FUNC_TICKET_GET_GENERIC(device);
            }
        }
    }

    return 0;
}

DEV_MEM_FUNC_TICKET_VALID(devMemTicketValid)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->ticketValid)
            {
                // if (device->sys->initialized)
                {
                    return device->functions->ticketValid(device, ticket); // Device specific ticket validation function
                }
            }
            else
            {
                // Generic ticket validation
                DEV_MEM_FUNC_TICKET_VALID_GENERIC(device);
            }
        }
    }

    return FALSE;
}
/** @} */

/**
 *  @addtogroup DEV_CPX
 *  @{
 */
DEV_CPX_FUNC_INIT(devCpxInit)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->init)
            {
                return device->functions->init(device, config);
            }
        }
    }
    return EXIT_FAILURE;
}

DEV_CPX_FUNC_DEINIT(devCpxDeinit)
{
    if (device)
    {
        if (device->functions)
        {
            if (device->functions->deinit)
            {
                return device->functions->deinit(device);
            }
        }
    }
    return EXIT_FAILURE;
}
/** @} */

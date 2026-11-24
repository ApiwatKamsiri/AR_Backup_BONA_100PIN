/** @file       segger_debug_com.c
 *  @brief      C file of SEGGER_DEBUG_COM device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Vehbi Umur Cabuk
 *  @date       18.03.2021
 */
#define FILE_SEGGER_DEBUG_COM_C

#include "segger_debug_com.h"

/**
 *  @addtogroup SEGGER_DEBUG_COM
 *  @{
 */

static DEV_COM_FUNC_INIT(seggerDebugComInit);
static DEV_COM_FUNC_DEINIT(seggerDebugComDeinit);
static DEV_COM_FUNC_OPEN(seggerDebugComOpen);
static DEV_COM_FUNC_CLOSE(seggerDebugComClose);
static DEV_COM_FUNC_SEND(seggerDebugComSend);
static DEV_COM_FUNC_RECEIVE(seggerDebugComReceive);
// static DEV_COM_FUNC_TICKET_GET(seggerDebugComTicketGet);
// static DEV_COM_FUNC_TICKET_VALID(seggerDebugComTicketValid);

/// @brief  Structure that defines the functions for seggerDebugCom
const tsDevComFuncs devComSeggerDebugComFuncs =
    {
        seggerDebugComInit,
        seggerDebugComDeinit,
        seggerDebugComOpen,
        seggerDebugComClose,
        seggerDebugComSend,
        seggerDebugComReceive,
        NULL,
        NULL,
};

/// @brief  Generic interrupt subroutine function for a seggerDebugCom
static void seggerDebugComIsr(const tsDevCom *device)
{
    tsSeggerDebugComParams *params       = device->parameters;
    const tsSeggerDebugComConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;

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
        // ISR arrived but there's no close request yet
        sys->closed = sys->opened;
    }
}

/// @brief  Init function for a seggerDebugCom
static DEV_COM_FUNC_INIT(seggerDebugComInit)
{
    tsSeggerDebugComParams *params       = device->parameters;
    const tsSeggerDebugComConsts *consts = device->constants;

    SEGGER_RTT_Init();
    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    device->sys->initialized = TRUE;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a seggerDebugCom
static DEV_COM_FUNC_DEINIT(seggerDebugComDeinit)
{
    tsSeggerDebugComParams *params       = device->parameters;
    const tsSeggerDebugComConsts *consts = device->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    device->sys->initialized = FALSE;

    return EXIT_SUCCESS;
}

/// @brief  Open function for a seggerDebugCom
static DEV_COM_FUNC_OPEN(seggerDebugComOpen)
{
    tsSeggerDebugComParams *params       = device->parameters;
    const tsSeggerDebugComConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;

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

/// @brief  Close function for a seggerDebugCom
static DEV_COM_FUNC_CLOSE(seggerDebugComClose)
{
    tsSeggerDebugComParams *params       = device->parameters;
    const tsSeggerDebugComConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;

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

/// @brief  Send function for a seggerDebugCom
static DEV_COM_FUNC_SEND(seggerDebugComSend)
{
    tsSeggerDebugComParams *params       = device->parameters;
    const tsSeggerDebugComConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    if (!sys->initialized)
    {
        return 0;
    }

    if (!length)
    {
        return 0;
    }
    else // if (!length)
    {
        if (txb == NULL)
        {
            // No operation
        }
        else // if (txb == NULL)
        {
            SEGGER_RTT_WriteString(0, txb);
            sys->closed = NULL;
        } // else // if (txb == NULL)
    } // else // if (!length)

    return length;
}

/// @brief  Receive function for a seggerDebugCom
static DEV_COM_FUNC_RECEIVE(seggerDebugComReceive)
{
    tsSeggerDebugComParams *params       = device->parameters;
    const tsSeggerDebugComConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    if (!sys->initialized)
    {
        return 0;
    }

    if (!length)
    {
        return 0;
    }
    else // if (!length)
    {
        if (rxb == NULL)
        {
            // Dismiss received data
        }
        else // if (rxb == NULL)
        {
            // Place received data to rxb
            sys->closed = NULL;
        } // else // if (rxb == NULL)
    }     // else // if (!length)

    return length;
}

#if 0 // Enable if seggerDebugCom requires another functionality other than generic ticket functions

/// @brief  Ticket get function for a seggerDebugCom
static DEV_COM_FUNC_TICKET_GET(seggerDebugComTicketGet)
{
    tsSeggerDebugComParams *params       = device->parameters;
    const tsSeggerDebugComConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;
}

/// @brief  Ticket valid function for a seggerDebugCom
static DEV_COM_FUNC_TICKET_VALID(seggerDebugComTicketValid)
{
    tsSeggerDebugComParams *params       = device->parameters;
    const tsSeggerDebugComConsts *consts = device->constants;
    tsDevComSystem *sys               = device->sys;
}

#endif

/** @} */

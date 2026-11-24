/** @file       uartecho.c
 *  @brief      Get data from uart and echo back
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     DF
 *  @date       25.01.2019
 */
#define FILE_UARTECHO_C

#include "uartecho.h"

/**
 *  @addtogroup UARTECHO
 *  @{
 */

PT_THREAD(uartechoThread)
{
    tsUartechoParams *params = process->parameters;

    PT_BEGIN();

    devComPrint(params->uart, "hello world!\r\n");

    for (;;)
    {
        uint8_t length = devComReceive(params->uart, NULL, 0);

        if (length)
        {
            uint8_t array[16];

            length = MIN(length, sizeof(array));
            devComReceive(params->uart, array, length);
            devComSend(params->uart, array, length);
        }

        PT_YIELD_TIMEOUT(100);
    }

    PT_END();
}

PROCESS_HANDLER_PROTO(uartechoHandler);

PROCESS_INIT_PROTO(uartechoInit)
{
    ADD_RCOS_VERSION("uartecho","2.0.0")

    PROCESS_STATE_CHANGE(process, uartechoHandler);
    threadStart(process, uartechoThread);
    process->initialized = 1;
}

PROCESS_DEINIT_PROTO(uartechoDeinit)
{
    process->initialized = 0;
    PROCESS_STATE_CHANGE(process, NULL);
    threadStop(process, uartechoThread);
}

PROCESS_HANDLER_PROTO(uartechoHandler)
{
    UNUSED(process); // REMOVE IF USED
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

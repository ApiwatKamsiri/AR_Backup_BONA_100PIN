/** @file       log_csv.c
 *  @brief      LOG_CSV middleware program file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       20/04/2020
 */
#define FILE_LOG_CSV_C

#include "log_csv.h"
#include <string.h>

#define DEBUG_FILE_NAME "csv"

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup LOG_CSV
 *  @{
 */

PT_THREAD(logCsvThread)
{
    tsLogCsvParams *params       = process->parameters;
    const tsLogCsvConsts *consts = process->constants;

    PT_BEGIN();

    params->ticket = devComTicketGet(consts->uart);
    PT_WAIT_UNTIL(devComTicketValid(consts->uart, params->ticket));
    devComOpen(consts->uart, consts->target);

    for (;;)
    {
        if (params->lineCnt == 0)
        {
            // Header line
            for (int idx = 0; idx < consts->listCnt; idx++)
            {
                if (idx != 0)
                {
                    devComPrint(consts->uart, "%s", consts->delimiter);
                }
                devComPrint(consts->uart, "\"%s\"", consts->list[idx].label);
            }
            devComPrint(consts->uart, consts->lineBreak);
        }

        // New line
        for (int idx = 0; idx < consts->listCnt; idx++)
        {
            uint32_t data = 0;

            if (consts->list[idx].length <= sizeof(data))
            {
                memcpy(&data, consts->list[idx].address, consts->list[idx].length);
            }
            if (idx != 0)
            {
                devComPrint(consts->uart, "%s", consts->delimiter);
            }
            devComPrint(consts->uart, consts->list[idx].format, data);
        }
        devComPrint(consts->uart, consts->lineBreak);

        // Catch window end
        if (consts->window)
        {
            if (++params->lineCnt >= consts->window)
            {
                params->lineCnt = 0;
            }
        }
        else
        {
            params->lineCnt = 1;
        }
        PT_YIELD_TIMEOUT(consts->period);
    }

    devComClose(consts->uart, consts->target);

    PT_END();
}

static PROCESS_HANDLER_PROTO(logCsvHandler);

/// @brief  Initialization function of logCsv
PROCESS_INIT_PROTO(logCsvInit)
{
    ADD_RCOS_VERSION("logcsv", "2.0.0")

    PROCESS_STATE_CHANGE(process, logCsvHandler);
    threadStart(process, logCsvThread);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of logCsv
PROCESS_DEINIT_PROTO(logCsvDeinit)
{
    process->initialized = 0; // If process needs other checks, clear this another time
    PROCESS_STATE_CHANGE(process, NULL);
}

/// @brief  Event handler function of logCsv
static PROCESS_HANDLER_PROTO(logCsvHandler)
{
    UNUSED(process); // REMOVE IF USED
}

/** @} */

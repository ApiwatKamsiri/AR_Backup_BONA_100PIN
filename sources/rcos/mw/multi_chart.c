/** @file       multi_chart.c
 *  @brief      Send data over serial channel to be logged by MultiChart program
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     
 *  @date       21.03.2019
 */
#define FILE_MULTI_CHART_C

/**
 *  @addtogroup MULTI_CHART
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "multi_chart.h"

/*********************************************************************
 *  LOCAL CONSTANTS
 */

/*********************************************************************
 *  LOCAL TYPEDEFS
 */

/*********************************************************************
 *  LOCAL MACROS
 */

/*********************************************************************
 *  LOCAL VARIABLES
 */

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */
PROCESS_HANDLER_PROTO(multiChartHandler);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(multiChartInit)
{
    tsMultiChartParams *params       = process->parameters;
    const tsMultiChartConsts *consts = process->constants;
    ADD_RCOS_VERSION("multichart", "2.0.0")

    PROCESS_STATE_CHANGE(process, multiChartHandler);

    devComInit(consts->uart);
    params->tev.post.source      = process->enumeration;
    params->tev.post.destination = process->enumeration;
    timerEventStart(&params->tev, consts->period);
}

PROCESS_DEINIT_PROTO(multiChartDeinit)
{
    tsMultiChartParams *params = process->parameters;

    PROCESS_STATE_CHANGE(process, NULL);

    timerEventStop(&params->tev);
}

PROCESS_HANDLER_PROTO(multiChartHandler)
{
    tsMultiChartParams *params       = process->parameters;
    const tsMultiChartConsts *consts = process->constants;

    if (eventCurrent.source == process->enumeration)
    {
        uint16_t idx;
        uint16_t shortData;
        uint32_t longData;

        devComOpen(consts->uart, consts->target);

        // Header
        shortData = 0x0A0D; // Assumes little endian
        devComSend(consts->uart, &shortData, sizeof(shortData));

        // Data Raw
        for (idx = 0; idx < consts->arraySize; idx++)
        {
            shortData = *consts->array[idx].raw;
            shortData = htons(shortData);
            devComSend(consts->uart, &shortData, sizeof(shortData));
        }

        // Data Baseline
        for (idx = 0; idx < consts->arraySize; idx++)
        {
            shortData = *consts->array[idx].baseline;
            shortData = htons(shortData);
            devComSend(consts->uart, &shortData, sizeof(shortData));
        }

        // Data Signal
        for (idx = 0; idx < consts->arraySize; idx++)
        {
            shortData = *consts->array[idx].signal;
            shortData = htons(shortData);
            devComSend(consts->uart, &shortData, sizeof(shortData));
        }

        // Tail
        longData = 0xffff00; // Assumes little endian
        devComSend(consts->uart, &longData, 3);

        devComClose(consts->uart, consts->target);

        timerEventStart(&params->tev, consts->period);
    }
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

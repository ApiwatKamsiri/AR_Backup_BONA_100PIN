/** @file       csd_tuner.c
 *  @brief      CYPRESS PSoC Creator Sense Tuner
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       29.04.2019
 */
#define FILE_CSD_TUNER_C

/**
 *  @addtogroup CSD_TUNER
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "csd_tuner.h"

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
PROCESS_HANDLER_PROTO(csdTunerHandler);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(csdTunerInit)
{
    tsCsdTunerParams *params = process->parameters;
    ADD_RCOS_VERSION("csdtuner", "2.0.0")

    devComInit(params->uart);
    PROCESS_STATE_CHANGE(process, csdTunerHandler);
    params->tev.post.source      = process->enumeration;
    params->tev.post.destination = process->enumeration;

    timerEventStart(&params->tev, params->period); // 115200 8N1
}

PROCESS_DEINIT_PROTO(csdTunerDeinit)
{
    tsCsdTunerParams *params = process->parameters;

    PROCESS_STATE_CHANGE(process, NULL);
    timerEventStop(&params->tev);
}

PROCESS_HANDLER_PROTO(csdTunerHandler)
{
    tsCsdTunerParams *params = process->parameters;
    uint32_t tempData;

    switch (eventCurrent.event)
    {
        case eCsdTunerLoop:
            devComOpen(params->uart, &params->target);
            tempData = 0x0a0d;
            devComSend(params->uart, &tempData, 2); // header
            globalIntDisable();
            devComSend(params->uart, params->dsRam, params->dsRamSize); // CapSense data
            globalIntEnable();
            tempData = 0xffff00;
            devComSend(params->uart, &tempData, 3); // tail
            devComClose(params->uart, &params->target);
            timerEventStart(&params->tev, params->period); // 115200 8N1
            break;

        default:
            break;
    }
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

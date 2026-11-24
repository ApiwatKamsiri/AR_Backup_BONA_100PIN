/** @file       event_mapper.c
 *  @brief      Event mapper process will translator an event from a source into another event for another destination
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       16.09.2019
 */
#define FILE_EVENT_MAPPER_C

/**
 *  @addtogroup EVENT_MAPPER
 *  @{
 */

#include "event_mapper.h"

static PROCESS_HANDLER_PROTO(eventMapperHandler);

PROCESS_INIT_PROTO(eventMapperInit)
{
    ADD_RCOS_VERSION("eventmapper", "2.0.0")

    PROCESS_STATE_CHANGE(process, eventMapperHandler);
    process->initialized = 1; // If process needs other checks, set this another time
}

PROCESS_DEINIT_PROTO(eventMapperDeinit)
{
    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

static PROCESS_HANDLER_PROTO(eventMapperHandler)
{
    tsEventMapperParams *params = process->parameters;
    uint16_t idx;

    for (idx = 0; idx < params->arraySize; idx++)
    {
        if (eventCurrent.source == params->array[idx].srcProcess)
        {
            if (eventCurrent.event == params->array[idx].srcEvent)
            {
                eventPost(params->array[idx].dstProcess, params->array[idx].dstEvent, eventData, eventCurrent.length);
#if EVENT_MAPPER_MULTI_EVENTPOSTS
#else
                break;
#endif
            }
        }
    }
}

/** @} */

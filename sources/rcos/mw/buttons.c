/** @file       buttons.c
 *  @brief      Buttons application, checks buttons and creates button pressed events
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ozkan AKGUL (ozkan.akgul@arcelik.com)
 *  @author     Deniz Fer
 *  @date       01.04.2016
 */
#define FILE_BUTTONS_C

#include "buttons.h"

#define BUTTONS_RESOLUTION (100) ///< in milliseconds

PT_THREAD(buttonsThread)
{
    tsButtonsParams *params       = process->parameters;
    const tsButtonsConsts *consts = process->constants;

    PT_BEGIN();

    for (;;)
    {
        uint32_t buttonsValue = devIoGet(consts->buttonsDevIo);
        uint32_t changed      = buttonsValue ^ params->lastValue; // Changed bits

        if (changed)
        {
            if (changed & buttonsValue)
            {
                // Something pressed
                uint8_t idx;

                params->lastIdx = consts->mapSize;
                params->time    = 0;
                for (idx = 0; idx < consts->mapSize; idx++)
                {
                    if (buttonsValue == consts->map[idx].buttonsValue)
                    {
                        params->lastIdx = idx;
                        if (EVENT_NONE != consts->map[params->lastIdx].pressed)
                        {
                            eventPost(params->targetProcess, consts->map[params->lastIdx].pressed, &params->time, sizeof(params->time));
                        }
                        break;
                    }
                }
            }
            else // if (changed & buttonsValue)
            {
                // Something released
                if (params->lastIdx != consts->mapSize)
                {
                    // Top layer button is being released
                    if (EVENT_NONE != consts->map[params->lastIdx].released)
                    {
                        eventPost(params->targetProcess, consts->map[params->lastIdx].released, &params->time, sizeof(params->time));
                    }
                    params->lastIdx = consts->mapSize;
                    params->time    = 0;
                }
                else
                {
                    // Top layer button is already released,
                    // other button releases are ignored
                }
            } // else // if (changed & buttonsValue)

            params->lastValue = buttonsValue;
        }
        else // if (changed)
        {
            // No change since last event
            if (params->lastIdx != consts->mapSize)
            {
                // Pressed time increses
                params->time += consts->resolution;
                if (EVENT_NONE != consts->map[params->lastIdx].pressed)
                {
                    eventPost(params->targetProcess, consts->map[params->lastIdx].pressed, &params->time, sizeof(params->time));
                }
            }
        }
        PT_YIELD_TIMEOUT(consts->resolution);
    }
    
    PT_END();
}

PROCESS_HANDLER_PROTO(buttonsHandler);

PROCESS_INIT_PROTO(buttonsInit)
{
    tsButtonsParams *params       = process->parameters;
    const tsButtonsConsts *consts = process->constants;
    ADD_RCOS_VERSION("buttons", "2.0.0")

    PROCESS_STATE_CHANGE(process, buttonsHandler);
    devIoInit(consts->buttonsDevIo, NULL);
    params->lastValue = devIoGet(consts->buttonsDevIo);
    threadStart(process, buttonsThread);
    process->initialized = 1;
}

PROCESS_DEINIT_PROTO(buttonsDeinit)
{
    threadStop(process, buttonsThread);
    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0;
}

PROCESS_HANDLER_PROTO(buttonsHandler)
{
    UNUSED(process);
}

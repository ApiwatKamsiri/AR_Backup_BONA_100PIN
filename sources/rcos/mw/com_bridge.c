/** @file       com_bridge.c
 *  @brief      COM_BRIDGE middleware program file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       09/06/2020
 */
#define FILE_COM_BRIDGE_C

#include "com_bridge.h"

#define DEBUG_FILE_NAME "cbrdg"

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup COM_BRIDGE
 *  @{
 */

static PT_THREAD(comBridgeThread)
{
    tsComBridgeParams *params       = process->parameters;
    const tsComBridgeConsts *consts = process->constants;

    PT_BEGIN();

    devComInit(consts->com1);
    devComInit(consts->com2);
    process->initialized = 1; // If process needs other checks, set this another time

    params->ticket1 = devComTicketGet(consts->com1);
    PT_WAIT_UNTIL(devComTicketValid(consts->com1, params->ticket1));
    params->ticket2 = devComTicketGet(consts->com2);
    PT_WAIT_UNTIL(devComTicketValid(consts->com2, params->ticket2));

    devComOpen(consts->com1, consts->target1);
    devComOpen(consts->com2, consts->target2);

    for (;;)
    {
        uint8_t amount;
        uint8_t stackBuffer[16];

        amount = devComReceive(consts->com1, NULL, 0);
        if (amount)
        {
            amount = MIN(sizeof(stackBuffer), amount);
            devComReceive(consts->com1, stackBuffer, amount);
            devComSend(consts->com2, stackBuffer, amount);
        }

        amount = devComReceive(consts->com2, NULL, 0);
        if (amount)
        {
            amount = MIN(sizeof(stackBuffer), amount);
            devComReceive(consts->com2, stackBuffer, amount);
            devComSend(consts->com1, stackBuffer, amount);
        }

        PT_YIELD_TIMEOUT(10);
    }

    PT_END();

    devComClose(consts->com1, consts->target1);
    devComClose(consts->com2, consts->target2);

    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Initialization function of comBridge
PROCESS_INIT_PROTO(comBridgeInit)
{
    ADD_RCOS_VERSION("combridge","2.0.0")
    threadStart(process, comBridgeThread);
}

/// @brief  Deinitialization function of comBridge
PROCESS_DEINIT_PROTO(comBridgeDeinit)
{
    threadStop(process, process->threadFunction);
}

/** @} */

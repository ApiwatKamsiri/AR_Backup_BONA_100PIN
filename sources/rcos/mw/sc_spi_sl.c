/** @file       sc_spi_sl.c
 *  @brief      Serial Communication Protocol over SPI Slave
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     
 *  @date       24.07.2019
 */
#define FILE_SC_SPI_SL_C

/**
 *  @addtogroup SC_SPI_SL
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "sc_spi_sl.h"
#include "libs/crc.h"
#include <string.h>

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
static PROCESS_HANDLER_PROTO(scSpiIdle);
static uint8_t scSpiCalcCrc(tsFrame *frame);
static void defaultEventProcess(tsProcess *process);
static void stateEnterSend(tsProcess *process);
static void stateEnterReceive(tsProcess *process);
static void stateEnterIdle(tsProcess *process);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(scSpiSlInit)
{
    tsScSpiSlParams *params = process->parameters;
    ADD_RCOS_VERSION("scspisl", "2.0.0")

    devComInit(params->com);
    process->initialized = 1; // If process needs other checks, set this another time
    stateEnterReceive(process);
}

PROCESS_DEINIT_PROTO(scSpiSlDeinit)
{
    tsScSpiSlParams *params = process->parameters;

    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler for send state
static PROCESS_HANDLER_PROTO(scSpiSend);

static void stateEnterSend(tsProcess *process)
{
    tsScSpiSlParams *params = process->parameters;

    params->comTicket = devComTicketGet(params->com);

    PROCESS_STATE_CHANGE(process, scSpiSend);
    eventPost(process->enumeration, eScSpiLoop, NULL, 0);
    // timerEventStart(&params->tev, 10);
}

static PROCESS_HANDLER_PROTO(scSpiSend)
{
    tsScSpiSlParams *params = process->parameters;

    if (eventCurrent.source == eventCurrent.destination)
    // if (eScSpiLoop == eventCurrent.event)
    {
        if (!devComTicketValid(params->com, params->comTicket))
        {
            eventPostPone();
        }
        else
        {
            devComOpen(params->com, params->comTarget);
            devComSend(params->com, &params->sending, offsetof(tsFrame, data));
            if (params->sending.length)
            {
                devComSend(params->com, params->sending.data, params->sending.length);
            }
            devComSend(params->com, &params->sending.crc, sizeof(params->sending.crc));
            devComClose(params->com, params->comTarget);

            stateEnterReceive(process); // Enter receiving state right after sending the package
        }
    }
    else
    {
        defaultEventProcess(process);
    }
}

/// @brief  Event handler for receive state
static PROCESS_HANDLER_PROTO(scSpiReceive);

static void stateEnterReceive(tsProcess *process)
{
    tsScSpiSlParams *params = process->parameters;

    params->comTicket = devComTicketGet(params->com);
    params->recState  = eScSpiOpen; // eScSpiHeader
    memset(&params->receiving, 0, sizeof(params->receiving));

    PROCESS_STATE_CHANGE(process, scSpiReceive);
    eventPost(process->enumeration, eScSpiLoop, NULL, 0);
    // timerEventStart(&params->tev, 10);
}

static PROCESS_HANDLER_PROTO(scSpiReceive)
{
    tsScSpiSlParams *params = process->parameters;

    // if (eventCurrent.source == eventCurrent.destination)
    if (eScSpiLoop == eventCurrent.event)
    {
        if (!devComTicketValid(params->com, params->comTicket))
        {
            eventPostPone();
        }
        else
        {
            switch (params->recState)
            {
                case eScSpiOpen:
                    devComOpen(params->com, params->comTarget);
                    params->recState = eScSpiPreamble;
                    // break; // Fall-through intentional

                case eScSpiPreamble:
                    if (0 == devComReceive(params->com, NULL, 0))
                    {
                        eventPostPone();
                        break; // case eScSpiPreamble:
                    }
                    else
                    {
                        do
                        {
                            devComReceive(params->com, &params->receiving, sizeof(params->receiving.preamble));
                        } while ((START_OF_FRAME != params->receiving.preamble) && devComReceive(params->com, NULL, 0));

                        if (START_OF_FRAME != params->receiving.preamble)
                        {
                            // No response
                            eventPostPone(); // devComClose(params->com, params->comTarget);
                            break;           // case eScSpiPreamble:
                        }
                        else
                        {
                            // Get header
                            params->recState = eScSpiHeader;
                        }
                    }
                    // break; // case eScSpiPreamble: // Fall-through intentional

                case eScSpiHeader:
#define HEADER_REMAIN (offsetof(tsFrame, data) - sizeof(params->receiving.preamble))
                    if (HEADER_REMAIN > devComReceive(params->com, NULL, 0))
                    {
                        eventPostPone();
                        break; // case eScSpiHeader:
                    }
                    else
                    {
                        devComReceive(params->com, &params->receiving.identifier, HEADER_REMAIN);
                        params->recState   = eScSpiData;
                        params->recDataIdx = 0;
                    }
                    // break; // case eScSpiHeader: // Fall-through intentional

                case eScSpiData:
                    if (params->receiving.length > params->recDataIdx)
                    {
                        // We need to gather the data section
                        uint16_t received = devComReceive(params->com, NULL, 0);

                        if (received)
                        {
                            received = MIN(received, (params->receiving.length - params->recDataIdx));
                            devComReceive(params->com, &params->receiving.data[params->recDataIdx], received);
                            params->recDataIdx += received;
                        }

                        eventPostPone();
                        break; // case eScSpiData:
                    }
                    else
                    {
                        // We have everything we need
#if SC_OLD_BUG_FIX
                        if (params->receiving.length)
                        {
                            uint8_t *low, *high;
                            low  = params->receiving.data;
                            high = &params->receiving.data[params->receiving.length - 1];

                            while (low < high)
                            {
                                uint8_t temp = *low;
                                *low         = *high;
                                *high        = temp;
                                low++;
                                high--;
                            }
                        }
#endif
                        params->recState = eScSpiCrc;
                    }
                    // break; // case eScSpiData: // Fall-through intentional

                case eScSpiCrc:
                    if (devComReceive(params->com, NULL, 0) >= sizeof(params->receiving.crc)) // SPI wait until complete
                    {
                        devComReceive(params->com, &params->receiving.crc, sizeof(params->receiving.crc));
                        devComClose(params->com, params->comTarget);

                        if (params->receiving.crc == scSpiCalcCrc(&params->receiving))
                        {
                            uint8_t idx;

                            for (idx = 0; idx < params->mapSize; idx++)
                            {
                                if (params->receiving.identifier == params->mapArr[idx].identifier)
                                {
                                    eventPost(params->mapArr[idx].process, params->mapArr[idx].event, params->receiving.data, params->receiving.length);
                                    break; // for
                                }
                            }

                            if (idx < params->mapSize)
                            {
                                if (params->receiving.identifier & SC_ACK_NACK_REQ)
                                {
                                    params->sending.preamble   = START_OF_FRAME;
                                    params->sending.identifier = SC_SET_ACK;
                                    params->sending.length     = 0;
                                    params->sending.crc        = scSpiCalcCrc(&params->sending);
                                    stateEnterSend(process);
                                }
                                else
                                {
                                    // Someone else will prepare a reply
                                    stateEnterIdle(process);
                                }
                                break; // case eScSpiCrc:
                            }
                            // Not present in the mapArr
                        }
                        else
                        {
                            // CRC error
                        }

                        params->sending.preamble   = START_OF_FRAME;
                        params->sending.identifier = SC_SET_NACK;
                        params->sending.length     = 0;
                        params->sending.crc        = scSpiCalcCrc(&params->sending);
                        stateEnterSend(process);
                    }
                    else // if (devComReceive(params->com, NULL, 0) >= sizeof(params->receiving.crc))
                    {
                        eventPostPone();
                    }
                    break;

                default:
                    params->recState = eScSpiOpen; // eScSpiHeader;
                    break;
            }
        }
    }
    else
    {
        defaultEventProcess(process);
    }
}

/// @brief  Event handler for idle state
static PROCESS_HANDLER_PROTO(scSpiIdle);

static void stateEnterIdle(tsProcess *process)
{
    tsScSpiSlParams *params = process->parameters;

    UNUSED(params);

    PROCESS_STATE_CHANGE(process, scSpiIdle);
    // timerEventStart(&params->tev, 10); // trigger it to check the queue after a timeout
    eventPost(process->enumeration, eScSpiLoop, NULL, 0);
}

static PROCESS_HANDLER_PROTO(scSpiIdle)
{
    tsScSpiSlParams *params = process->parameters;

    if (eventCurrent.source == eventCurrent.destination)
    // if (eScSpiLoop == eventCurrent.event)
    {
        if (DEQUE_LOAD(params->queue) >= sizeof(tsScQueueItem)) // If queue has a command
        {
            tsScQueueItem queItem;

            // Fill sending structure
            params->sending.preamble = START_OF_FRAME;
            FIFO_POP(params->queue, &queItem, sizeof(queItem));
            params->sending.identifier = queItem.identifier;
            params->sending.length     = queItem.length;
            if (queItem.length)
            {
                FIFO_POP(params->queue, params->sending.data, queItem.length);
            }
            params->sending.crc = scSpiCalcCrc(&params->sending);

#if SC_OLD_BUG_FIX
            {
                uint8_t *low, *high;
                low  = params->sending.data;
                high = &params->sending.data[params->sending.length - 1];

                while (low < high)
                {
                    uint8_t temp = *low;
                    *low         = *high;
                    *high        = temp;
                    low++;
                    high--;
                }
            }
#endif

            stateEnterSend(process);
        }
    }
    else
    {
        defaultEventProcess(process);
    }
}

/// @brief      Event handler for anything that comes from other processes
/// @warning    Should be called when eventCurrent.event != eScSpiLoop
static void defaultEventProcess(tsProcess *process)
{
    tsScSpiSlParams *params = process->parameters;
    uint8_t idx;

    if (FIFO_FREE(params->queue) >= (sizeof(tsScQueueItem) + eventCurrent.length))
    {
        for (idx = 0; idx < params->mapSize; idx++) // Check map for a corresponding serial communication command
        {
            if ((eventCurrent.event == params->mapArr[idx].event) &&
                (eventCurrent.source == params->mapArr[idx].process)) // If a match is found
            {
                tsScQueueItem item;

                item.identifier = params->mapArr[idx].identifier;
                item.length     = eventCurrent.length;
                FIFO_PUSH(params->queue, &item, sizeof(item)); // Place serial communication command into queue
                if (item.length)
                {
                    FIFO_PUSH(params->queue, eventData, item.length);
                }

                if (scSpiIdle == PROCESS_STATE_CURRENT(process)) // If currently in idle state
                {
                    eventPost(process->enumeration, eScSpiLoop, NULL, 0); // trigger it to check the queue
                    // timerEventStart(&params->tev, 10); // trigger it to check the queue after a timeout
                }
                break; // out of for loop
            }
        }
    }
    else
    {
        // internal queue is full, drop message
    }
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

static uint8_t scSpiCalcCrc(tsFrame *frame)
{
    uint8_t retVal, idx;

    retVal = crc8MaximInit();
    // retVal = crc8MaximAddData(retVal, frame->preamble);
    retVal = crc8MaximAddData(retVal, frame->identifier);
    retVal = crc8MaximAddData(retVal, frame->length);
    for (idx = 0; idx < frame->length; idx++)
    {
        retVal = crc8MaximAddData(retVal, frame->data[idx]);
    }
    retVal = crc8MaximFinish(retVal);

    return retVal;
}

/** @} */

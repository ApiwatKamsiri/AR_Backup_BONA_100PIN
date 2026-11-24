/** @file       tft_spi.c
 *  @brief      Serial Communication Protocol over SPI Master
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @author     Hamed Mohammadi
 *  @date       09.01.2020
 */
#define FILE_TFT_SPI_C

/**
 *  @addtogroup TFT_SPI
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"
#include "tft_spi.h"
#include "libs/crc.h"

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
static PROCESS_HANDLER_PROTO(tftSpiIdle);
static uint8_t tftSpiCalcCrc(tsTftFrame *frame);
static void defaultEventProcess(tsProcess *process);
static void stateEnterSend(tsProcess *process);
static void stateEnterReceive(tsProcess *process);
static void stateEnterIdle(tsProcess *process);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(tftSpiInit)
{
    tsTftSpiParams *params = process->parameters;

    ADD_RCOS_VERSION("tft_spi", "2.0.2")

    devIoInit(params->tftBusy, NULL);
    devIoInit(params->target.cs, NULL);

    PROCESS_STATE_CHANGE(process, tftSpiIdle);
    process->initialized = 1; // If process needs other checks, set this another time
    devComInit(params->com);
}

PROCESS_DEINIT_PROTO(tftSpiDeinit)
{
    tsTftSpiParams *params = process->parameters;

    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler for send state
static PROCESS_HANDLER_PROTO(scSpiSend)
{
    tsTftSpiParams *params = process->parameters;

    if (eventCurrent.source == eventCurrent.destination)
    {
        if (!devComTicketValid(params->com, params->comTicket))
        {
            eventPostPone();
        }
        else
        {
            devComOpen(params->com, &params->target);
            devComSend(params->com, &params->sending, offsetof(tsTftFrame, data));
            if (params->sending.length)
            {
                devComSend(params->com, params->sending.data, params->sending.length);
                params->lastCrc = params->sending.crc;
            }
            devComSend(params->com, &params->sending.crc, sizeof(params->sending.crc));
            devComClose(params->com, &params->target);

            stateEnterReceive(process); // Enter receiving state right after sending the package
        }
    }
    else
    {
        defaultEventProcess(process);
    }
}

static void stateEnterSend(tsProcess *process)
{
    tsTftSpiParams *params = process->parameters;

    params->comTicket = devComTicketGet(params->com);

    PROCESS_STATE_CHANGE(process, scSpiSend);
    timerEventStart(&params->tev, 10);
}

/// @brief  Event handler for receive state
static PROCESS_HANDLER_PROTO(scSpiReceive)
{
    tsTftSpiParams *params = process->parameters;
    uint8_t tryCount       = TFT_PREAMBLE_TIMEOUT;

    if (eventCurrent.source == eventCurrent.destination)
    {
        if (!devComTicketValid(params->com, params->comTicket))
        {
            eventPostPone();
        }
        else
        {
            switch (params->recState)
            {
                case eTftSpiHeader:
                    devComOpen(params->com, &params->target);
                    // Capture Preamble
                    do
                    {
                        devComSend(params->com, NULL, SIZE_OF_MEMBER(tsTftFrame, preamble));               // SPI master sends before receive
                        while (devComReceive(params->com, NULL, 0) < SIZE_OF_MEMBER(tsTftFrame, preamble)) // SPI wait until complete
                            ;
                        devComReceive(params->com, &params->receiving, SIZE_OF_MEMBER(tsTftFrame, preamble));
                    } while ((TFT_START_OF_FRAME != params->receiving.preamble) && (--tryCount));

                    if (TFT_START_OF_FRAME == params->receiving.preamble)
                    {
                        // Get header
#define HEADER_REMAIN (offsetof(tsTftFrame, data) - SIZE_OF_MEMBER(tsTftFrame, preamble))
                        devComSend(params->com, NULL, HEADER_REMAIN);               // SPI master sends before receive
                        while (devComReceive(params->com, NULL, 0) < HEADER_REMAIN) // SPI wait until complete
                            ;
                        devComReceive(params->com, &params->receiving.identifier, HEADER_REMAIN);
                        if (params->receiving.length)
                        {
                            devComSend(params->com, NULL, params->receiving.length); // SPI master sends before receive
                            params->recState = eTftSpiData;
                        }
                        else
                        {
                            devComSend(params->com, NULL, sizeof(params->receiving.crc)); // SPI master sends before receive
                            params->recState = eTftSpiCrc;
                        }
                        eventPostPone();
                    }
                    else // if (TFT_START_OF_FRAME == params->receiving.preamble)
                    {
                        // No response
                        devComClose(params->com, &params->target);
                        // WARNING: THERE'S NO CONNECTION ERROR HANDLING
                        stateEnterIdle(process); // Return to idle to send another
                    }
                    break; // case eTftSpiHeader:

                case eTftSpiData:
                    if (devComReceive(params->com, NULL, 0) >= params->receiving.length) // SPI wait until complete
                    {
                        devComReceive(params->com, params->receiving.data, params->receiving.length);

                        devComSend(params->com, NULL, sizeof(params->receiving.crc)); // SPI master sends before receive
                        params->recState = eTftSpiCrc;
                    }
                    eventPostPone();
                    break;

                case eTftSpiCrc:
                    if (devComReceive(params->com, NULL, 0) >= sizeof(params->receiving.crc)) // SPI wait until complete
                    {
                        devComReceive(params->com, &params->receiving.crc, sizeof(params->receiving.crc));
                        devComClose(params->com, &params->target);

                        if (params->receiving.crc == tftSpiCalcCrc(&params->receiving))
                        {
                            if (TFT_SET_ACK == params->receiving.identifier)
                            {
                                stateEnterIdle(process); // Return to idle to send another
                                if (params->receiving.data[2] == params->lastCrc)
                                {
                                    eventPost(params->targetProcess, params->targetEvent, params->receiving.data, params->receiving.length);
                                }
                                else
                                {
                                    eventPost(params->targetProcess, params->errorEvent, NULL, 0);
                                }
                            }
                            else if (TFT_SET_NACK == params->receiving.identifier)
                            {
                                stateEnterSend(process); // Resend
                            }
                            else
                            {
                                stateEnterIdle(process); // Return to idle to send another
                            }
                        }
                        else // if (params->receiving.crc == tftSpiCalcCrc(&params->receiving))
                        {
                            // Receive ERROR???
                            stateEnterSend(process); // Resend
                        }
                    }
                    else // if (devComReceive(params->com, NULL, 0) >= sizeof(params->receiving.crc))
                    {
                        eventPostPone();
                    }
                    break;

                default:
                    params->recState = eTftSpiHeader;
                    break;
            }
        }
    }
    else
    {
        defaultEventProcess(process);
    }
}

static void stateEnterReceive(tsProcess *process)
{
    tsTftSpiParams *params = process->parameters;

    params->comTicket = devComTicketGet(params->com);
    params->recState  = eTftSpiHeader;
    memset(&params->receiving, 0, sizeof(params->receiving));

    PROCESS_STATE_CHANGE(process, scSpiReceive);
    timerEventStart(&params->tev, 10);
}

/// @brief  Event handler for idle state
static PROCESS_HANDLER_PROTO(tftSpiIdle)
{
    tsTftSpiParams *params = process->parameters;

    if (eventCurrent.source == eventCurrent.destination)
    {
        if (!devIoGet(params->tftBusy))
        {
            if (DEQUE_LOAD(params->queue) >= sizeof(tsTftQueueItem)) // If queue has a command
            {
                tsTftQueueItem queItem;

                // Fill sending structure
                params->sending.preamble = TFT_START_OF_FRAME;
                FIFO_POP(params->queue, &queItem, sizeof(queItem));
                params->sending.identifier = queItem.identifier;
                params->sending.length     = queItem.length;
                if (queItem.length)
                {
                    // warning: does not check if queue has enough data
                    FIFO_POP(params->queue, params->sending.data, queItem.length);
                }
                params->sending.crc = tftSpiCalcCrc(&params->sending);
                stateEnterSend(process);
            }
        }
        else
        {
            timerEventStart(&params->tev, 15);
        }
    }
    else
    {
        defaultEventProcess(process);
    }
}

static void stateEnterIdle(tsProcess *process)
{
    tsTftSpiParams *params = process->parameters;

    PROCESS_STATE_CHANGE(process, tftSpiIdle);
    timerEventStart(&params->tev, 10); // trigger it to check the queue after a timeout
}

/// @brief      Event handler for anything that comes from other processes
/// @warning    Should be called when eventCurrent.event != eTftSpiLoop
static void defaultEventProcess(tsProcess *process)
{
    tsTftSpiParams *params = process->parameters;
    if (!(devIoGet(params->tftBusy) && eventCurrent.event == TFT_HEART_BEAT))
    {
        if (FIFO_FREE(params->queue) >= (sizeof(tsTftQueueItem) + eventCurrent.length))
        {
            tsTftQueueItem item;

            item.identifier = eventCurrent.event;
            item.length     = eventCurrent.length;
            FIFO_PUSH(params->queue, &item, sizeof(item)); // Place serial communication command into queue

            if (item.length)
            {
                FIFO_PUSH(params->queue, eventData, item.length);
            }

            if (tftSpiIdle == PROCESS_STATE_CURRENT(process)) // If currently in idle state
            {
                timerEventStart(&params->tev, 10); // trigger it to check the queue after a timeout
            }
        }
        else
        {
            // internal queue is full, drop message
        }
    }
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

static uint8_t tftSpiCalcCrc(tsTftFrame *frame)
{
    uint8_t retVal, idx;

    retVal = crc8MaximInit();
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

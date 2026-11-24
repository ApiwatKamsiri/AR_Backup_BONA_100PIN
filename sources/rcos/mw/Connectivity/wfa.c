/** @file       wfa.c
 *  @brief      wfa
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     mertGemalmaz
 *  @date       24.09.2019
 */
#define FILE_WFA_C

/**
 *  @addtogroup WFA
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include <string.h>
#include "wfa.h"
/*********************************************************************
 *  LOCAL CONSTANTS
 */

/*********************************************************************
 *  LOCAL TYPEDEFS
 */

/*********************************************************************
 *  LOCAL MACROS
 */
#define INDEX_ADDR_H 0
#define INDEX_ADDR_L 1
#define INDEX_ADDR_DATA 2
#define INDEX_ADDR_DYNAMIC_BYTES 23

#define WFA_WRITE_SIZE 2 // data[1] + index

#define WFA_DYNAMIC_BYTES_SEND_SIZE (params->mapsize - INDEX_ADDR_DYNAMIC_BYTES)
#define WFA_DYNAMIC_START_INDEX_WFAMAP 13 //WifiState

#define ARRAY_SIZE_WFA 73
#define ARRAY_SIZE_MERCEK 83

/*********************************************************************
 *  LOCAL VARIABLES
 */

typedef struct PLATFORM_PACKED
{
    uint8_t index; //TODO: later will be defined as uint16_t
    uint8_t data;  //TODO: max data set is 8 on wfa(Password_AP), so no need --> data[8]
} tsWriteWfa;

typedef struct PLATFORM_PACKED
{
    uint8_t addrHigh;
    uint8_t addrLow;
    uint8_t value;

} tsByteRead;

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */
static PROCESS_HANDLER_PROTO(wfaHandler);

void wfaSendCommand(tsProcess *process, uint8_t command, uint8_t *buf, uint8_t datasize);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(wfaInit)
{
    tsWfaParams *params = process->parameters;
    ADD_RCOS_VERSION("wfa", "2.0.7") // ESRCOS2P-186  (version updated from master. missing merge into development connectivity)
    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, wfaHandler);
    process->initialized = 1; // If process needs other checks, set this another time
}

PROCESS_DEINIT_PROTO(wfaDeinit)
{
    tsWfaParams *params = process->parameters;
    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

static PROCESS_HANDLER_PROTO(wfaHandler)
{
    tsWfaParams *params = process->parameters;

    UNUSED(params); // REMOVE IF USED

    switch (eventCurrent.event)
    {
        case eWfaReadByte:
        {
            uint8_t idx;
            uint8_t arrayIndex = (eventData[INDEX_ADDR_H] << 8) | eventData[INDEX_ADDR_L];
            tsByteRead byteReadBuf;

            for (idx = 0; idx < params->mapsize; idx++)
            {
                if (params->wfaMap[idx].length > arrayIndex)
                {
                    byteReadBuf.addrHigh = eventData[INDEX_ADDR_H];
                    byteReadBuf.addrLow  = eventData[INDEX_ADDR_L];
                    memcpy(&byteReadBuf.value, (params->wfaMap[idx].address + arrayIndex), sizeof(byteReadBuf.value));
                    eventReply(eWfaByteRead, &byteReadBuf, sizeof(byteReadBuf));
                    break;
                }
                else
                {
                    arrayIndex -= params->wfaMap[idx].length;
                }
            }

            if (idx == params->mapsize)
            {
                // send error
            }
        }
        break;

        case eWfaWriteByte:
        {
            tsWriteWfa wfaWrite;
            uint8_t fooData = 0xAA;
            uint8_t arrayIndex = (eventData[INDEX_ADDR_H] << 8) | eventData[INDEX_ADDR_L];

            eventReply(eWfaByteWritten, &fooData, sizeof(fooData)); //wfa Response

            //send data&index to eFindTargetEvent
            wfaWrite.index = arrayIndex; //INDEX_ADDR_DATA;
            memcpy(&wfaWrite.data, &eventData[INDEX_ADDR_DATA], 1);
            eventPost(process->enumeration, eFindTargetEvent, &wfaWrite, WFA_WRITE_SIZE);
        }
        break;

        case eWfaReadDynamicBytes:
        {
            uint8_t idx;
            uint8_t iidx = 0;

            for (idx = WFA_DYNAMIC_START_INDEX_WFAMAP; idx < params->mapsize; idx++)
            {
                memcpy(&eventData[iidx], params->wfaMap[idx].address, params->wfaMap[idx].length);
                iidx += params->wfaMap[idx].length;
            }

            eventReply(eWfaDynamicBytesRead, eventData, iidx);
        }
        break;

        case eWfaReadAllBytes:
        {
            uint8_t idx;
            uint8_t iidx = 0;

            for (idx = 0; idx < params->mapsize; idx++)
            {
                memcpy(&eventData[iidx], params->wfaMap[idx].address, params->wfaMap[idx].length);
                iidx += params->wfaMap[idx].length;
            }
            eventReply(eWfaAllBytesRead, eventData, iidx);
        }
        break;

        case eWfaReadMercek:
        {
           
            uint8_t idx;
            uint8_t iidx = 0;
 
            for (idx = 0; idx < params->mercekSize; idx++)
            {
                memcpy(&eventData[iidx], params->mercekArrayMap[idx].address, params->mercekArrayMap[idx].lenght);
                iidx += params->mercekArrayMap[idx].lenght;
            }
            eventReply(eWfaMercekRead, eventData, iidx);

            eventPost(params->mercekSentProcess,params->mercekSentEvent,NULL,0);
        }
        break;

        case eWfaReadSecureMercek:
        {
            if(params->secureMercekArrayMap == NULL)
            {
                break;
            }

            uint8_t idx;
            uint8_t iidx = 0;

            for (idx = 0; idx < params->secureMercekSize; idx++)
            {
                memcpy(&eventData[iidx], params->secureMercekArrayMap[idx].address, params->secureMercekArrayMap[idx].lenght);
                iidx += params->secureMercekArrayMap[idx].lenght;
            }
            eventReply(eWfaSecureMercekRead, eventData, iidx);
        }
        break;

        case eWfaReadMercekVersion:
        {
            uint8_t mercekVersion[2];
            mercekVersion[0] = params->mercekVersionMajor;
            mercekVersion[1] = params->mercekVersionMinor;
            eventReply(eWfaMercekVersionRead, mercekVersion, sizeof(mercekVersion));
        }
        break;

        case eFindTargetEvent:
        {
            tsWriteWfa wfaWrite;
            volatile uint8_t idx;
            uint8_t wfaElementBuf[8];

            memcpy(&wfaWrite, eventData, sizeof(wfaWrite));
            for (idx = 0; idx < params->mapsize; idx++)
            {
                if (params->wfaMap[idx].length > wfaWrite.index)
                {
                    memcpy(&wfaElementBuf, params->wfaMap[idx].address, params->wfaMap[idx].length);

                    memcpy(&wfaElementBuf[wfaWrite.index], &wfaWrite.data, sizeof(wfaWrite.data));

                    eventPost(params->wfaMap[idx].process, params->wfaMap[idx].event, wfaElementBuf, params->wfaMap[idx].length);
                    break;
                }
                else
                {
                    wfaWrite.index -= params->wfaMap[idx].length;
                }
            }
        }
        break;

        default:
            break;
    }
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

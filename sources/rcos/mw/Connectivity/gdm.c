/** @file       process_gdm.c
 *  @brief      GDM 
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     mertGemalmaz
 *  @date       17.09.2019
 *  @author     Burak OZAN
 *  @date       26.08.2022
 */
#define FILE_GDM_C

/**
 *  @addtogroup PROCESS_GDM
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"
#include <string.h>
#include "gdm.h"
#include "libs/crc.h"

/*********************************************************************
 *  LOCAL CONSTANTS
 */

/*********************************************************************
 *  LOCAL TYPEDEFS
 */
enum packageTypes
{
    PACK_TYPE_INVALID = 0,
    PACK_TYPE_CONFIG,
    PACK_TYPE_CRYPTO,
    PACK_TYPE_OTA,
    PACK_TYPE_CONTROL,
    PACK_TYPE_WFA,
    PACK_TYPE_PROGRAM_CONFIG,
    PACK_TYPE_DATA_COLLECTION,
    PACK_TYPE_SENSOR_DATA,
    PACK_TYPE_CONSUMPTION_DATA,
    PACK_TYPE_FEATURE,
    PACK_TYPE_CONTINUATION,
    PACK_TYPE_RESPONDS_START,
    PACK_TYPE_ACK = PACK_TYPE_RESPONDS_START,
    PACK_TYPE_RESUME,
    PACK_TYPE_RESEND,
    PACK_TYPE_ABORT,
    PACK_TYPE_ACK_PAUSE,
    PACK_TYPE_ABORT_PAUSE,
    PACK_TYPE_END,
};

const uint8_t packageTypeIDs[PACK_TYPE_END] =
    {
        0x00, //INVALID
        0x10, //CONFIG
        0x20, //CRYPTO
        0x30, //OTA
        0x40, //CONTROL
        0xF0, //WFA
        0x60, //PROGRAM_CONFIG
        0x70, //DATA_COLLECTION
        0x80, //SENSOR_DATA
        0x90, //CONSUMPTION_DATA
        0xA0, //FEATURE_POOL
        0xAA, //CONTINUATION
        0xB1, //ACK
        0xB4, //RESUME
        0xB2, //RESEND
        0xB6, //ABORT
        0xB5, //ACKPAUSE
        0xB7, //ABORTPAUSE
};

/*********************************************************************
 *  LOCAL MACROS
 */
#define GDM_PACK_TIMEOUT_VAL 5
#define OTA_CMD_OTA_TRANSFER 0x20
/*********************************************************************
 *  LOCAL VARIABLES
 */
uint8_t uartTestBuf[50];

uint8_t gTest = 0;

struct
{
    unsigned char counter;
    unsigned char started;
} sLowLevelPackTimeout;

typedef struct
{
    uint8_t waitForAck : 1;
    uint8_t readyToSend : 1;
    uint8_t readyToRecv : 1;
    uint8_t resend : 1;
    uint8_t resume : 1;
} teTransferControlStatus;

teTransferControlStatus transferStatus;
teBool gLowLevelPackDrop = FALSE;

volatile uint8_t packTypeTemp;
/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */
static PROCESS_HANDLER_PROTO(processGdmHandler);

void clearRxBuf(tsProcess *process);
uint8_t checkPackTypeIsValid(uint8_t packType);
uint16_t gdmCalcCrc(stGdmFrame *frame, uint8_t size);

void sendTranferControlPack(tsProcess *process, uint8_t packType);

void gdmPackTimeOutStop(void);
void gdmPackTimeOutStart(void);
uint8_t checkComReceiveTimeout(tsGdmParams *params);
void getTransferControlPacks(tsProcess *process, uint8_t packType);
uint8_t lowLevelCommStarted = 0;
/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(gdmInit)
{
    tsGdmParams *params = process->parameters;
    ADD_RCOS_VERSION("gdm", "2.0.11")

    PROCESS_STATE_CHANGE(process, processGdmHandler);
    process->initialized = 1; // If process needs other checks, set this another time

    devComInit(params->com);

    timerEventStart(&params->framerLowLevelTimer, 10); //gdm rx

    timerEventStart(&params->framerTimerTx, 5); //gdm Tx

    timerEventStart(&params->tevMaxFrameSize, 50); //maxFrameSize
}

PROCESS_DEINIT_PROTO(gdmDeinit)
{
    tsGdmParams *params = process->parameters;

    UNUSED(params);

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

uint8_t resendCounter = 0;
// uint8_t safirPowerStatus= 0;
//TODO: create timer for frame receive
static PROCESS_HANDLER_PROTO(processGdmHandler)
{
    tsGdmParams *params = process->parameters;
    uint8_t rxSwBufSize, i;
    //    teBool preambleFound = FALSE;
    uint8_t temp = 0;
    volatile uint16_t temp16;

    UNUSED(params); // REMOVE IF USED

    if (eventCurrent.source == eventCurrent.destination) //run process_gdm events
    {
        switch (eventCurrent.event)
        {
            case eGdmSendMaxFrameSize:
            {
                params->maxFrameSize += WIFI_OTA_FRAMER_OVERHEAD_BYTES_EXCEPT_DATA;
                eventPost(params->maxFrameProcess,params->maxFrameEvent,&params->maxFrameSize,sizeof(params->maxFrameSize));
            }
            break;

            case eLowLevel:
            {
                uint8_t gPreamble = FALSE;
                rxSwBufSize       = devComReceive(params->com, NULL, 0); //check whether SW uart buf is empty or not

                if (rxSwBufSize)
                {
                    for (i = 0; i <= rxSwBufSize; i++)
                    {
                        devComReceive(params->com, &params->rxBufGdm.preamble, sizeof(params->rxBufGdm.preamble));

                        if (GDM_FRAME_PREAMBLE == params->rxBufGdm.preamble)
                        {
                            gdmPackTimeOutStart(); //start timeout for this pack
                            gPreamble = TRUE;
                            eventPost(process->enumeration, eGetPackType, NULL, 0); //go get pack type
                            break;
                        }
                    }
                }

                if (gPreamble == FALSE)
                {
                    timerEventStart(&params->framerLowLevelTimer, 10);
                }
            }
            break;

            case eGetPackType:
            {
                //TODO: check how many bytes are receieved !!!!! devcomrecieve returns length
                if (devComReceive(params->com, NULL, 0) >= sizeof(params->rxBufGdm.packType))
                {
                    params->retryCount = 0;
                    devComReceive(params->com, &params->rxBufGdm.packType, sizeof(params->rxBufGdm.packType));
                    temp = (checkPackTypeIsValid(params->rxBufGdm.packType));

                    if (temp) // if pack type is valid
                    {
                        if (temp < PACK_TYPE_RESPONDS_START) // such as ota wfa pack types(core packs)
                        {
                            eventPost(process->enumeration, eGetCommand, NULL, 0);
                            packTypeTemp = temp;
                        }
                        else // such as ack, nack ack pause etc  (transfer control packs)
                        {
                            eventPost(process->enumeration, eGetCrc, NULL, 0);
                        }
                    }
                    else //pack type is not valid
                    {
                        clearRxBuf(process);
                        eventPost(process->enumeration, eLowLevel, NULL, 0); //check any more new data in the SW buffer
                    }                    
                }
                else
                {
                    if(EXIT_SUCCESS == checkComReceiveTimeout(params))
                    {
                        timerEventStart(&params->getPackTypeTimer, 5);
                    }      
                }
            }
            break;

            case eGetCommand:
            {
                teBool commandValid = FALSE;
                //TODO: check how many bytes are receieved !!!!! devcomrecieve returns length
                if (devComReceive(params->com, NULL, 0) >= sizeof(params->rxBufGdm.command))
                {
                    params->retryCount = 0;
                    devComReceive(params->com, &params->rxBufGdm.command, sizeof(params->rxBufGdm.command));
                    //TODO: valid command? not valid --> gdmPackTimeOutStop() and send resend or abort
                    // pack not valid > gLowLevelPackDrop = TRUE;

                    for (i = 0; i < params->mapsize; i++)
                    {
                        if (params->rxBufGdm.command == params->mapArr[i].command)
                        {
                            eventPost(process->enumeration, eGetDataLength, NULL, 0);
                            commandValid = TRUE;
                            break;
                        }
                    }

                    if (!commandValid)
                    {
                        gdmPackTimeOutStop();
                        gLowLevelPackDrop = TRUE;
                        eventPost(process->enumeration, eLowLevel, NULL, 0); //check any more new data in the SW buffer
                    }
                }
                else
                {                    
                    if(EXIT_SUCCESS == checkComReceiveTimeout(params))
                    {
                        timerEventStart(&params->getCommandTimer, 5);
                    }
                }
            }
            break;

            case eGetDataLength:
            {
                //TODO: check how many bytes are receieved !!!!! devcomrecieve returns length
                if (devComReceive(params->com, NULL, 0) >= sizeof(params->rxBufGdm.packDataSize))
                {
                    params->retryCount = 0;
                    devComReceive(params->com, &params->rxBufGdm.packDataSize, sizeof(params->rxBufGdm.packDataSize));

                    params->dataLength = ntohl(params->rxBufGdm.packDataSize);
                    //TODO: calculate data length and verify

                    if (params->dataLength > GDM_MAX_DATA_SIZE) //TODO: how to calculate maxFrameSize!!! GDM_MAX_FRAME_SIZE
                    {
                        if (PACK_TYPE_CONTINUATION == packTypeTemp)
                        {
                            params->dataLength = CONTINUATION_MAX_DATA_SIZE;
                        }
                        if (PACK_TYPE_OTA == packTypeTemp)
                        {
                            params->dataLength = OTA_MAX_DATA_SIZE;
                        }
                        eventPost(process->enumeration, eGetData, NULL, 0);//dataLength 0 sa girmemesi icin buraya alindi.
                    }
                    else if (0 == params->dataLength)
                    {
                        eventPost(process->enumeration, eGetCrc, NULL, 0);
                    }
                    else //not continuation pack
                    {
                        // params->rxBufGdm.packDataSize = params->dataLength;
                        eventPost(process->enumeration, eGetData, NULL, 0);//dataLength 0 sa girmemesi icin buraya alindi.
                    }
                }
                else
                {
                    if(EXIT_SUCCESS == checkComReceiveTimeout(params))
                    {
                        timerEventStart(&params->getDataLengthTimer, 5);
                    }
                }
            }
            break;

            case eGetData:
            {
                //TODO: check how many bytes are receieved !!!!! devcomrecieve returns length
                if (devComReceive(params->com, NULL, 0) >= params->dataLength)
                {
                    params->retryCount = 0;
                    devComReceive(params->com, params->rxBufGdm.data, params->dataLength);
                    eventPost(process->enumeration, eGetCrc, NULL, 0);
                }
                else
                {
                    if(EXIT_SUCCESS == checkComReceiveTimeout(params))
                    {
                        timerEventStart(&params->getDataTimer, 5);
                    }
                }
            }
            break;

            case eGetCrc:
            {
                uint8_t temp;
                //TODO: check how many bytes are receieved !!!!! devcomrecieve returns length
                if (devComReceive(params->com, NULL, 0) >= sizeof(params->rxBufGdm.crc))
                {
                    params->retryCount = 0;
                    devComReceive(params->com, &params->rxBufGdm.crc, sizeof(params->rxBufGdm.crc));

                    temp16 = htons(params->rxBufGdm.crc);

                    if (temp16 == gdmCalcCrc(&params->rxBufGdm, params->dataLength))
                    {
                        //frame is valid
                        temp = checkPackTypeIsValid(params->rxBufGdm.packType);
                        if (temp < PACK_TYPE_RESPONDS_START) // no need to send ack for ACKs
                        {
                            if ((PACK_TYPE_CONTINUATION == temp)) //(PACK_TYPE_OTA == temp) ||
                            {
                                sendTranferControlPack(process, PACK_TYPE_ACK_PAUSE);
                            }
                            if (PACK_TYPE_OTA == temp)
                            {
                                if (OTA_CMD_OTA_TRANSFER == params->rxBufGdm.command)
                                {
                                    sendTranferControlPack(process, PACK_TYPE_ACK_PAUSE);
                                }
                                else
                                {
                                    sendTranferControlPack(process, PACK_TYPE_ACK);
                                }
                            }
                            else
                            {
                                sendTranferControlPack(process, PACK_TYPE_ACK);
                            }
                        }
                        if (temp >= PACK_TYPE_RESPONDS_START)
                        {
                            // getTransferControlPacks(process,params->rxBufGdm.packType);
                            eventPost(process->enumeration, eControlPackSafir, &temp, sizeof(temp));
                        }
                        else 
                        {
                            eventPost(process->enumeration, eFrameRooter, NULL, 0);
                        }
                        gdmPackTimeOutStop(); //stop timeout counter
                        if(!lowLevelCommStarted)
                        {
                            eventPost(params->commStatProcess,params->commStatEvent,NULL,0);    //inform ccm_safir that comm started
                            lowLevelCommStarted = 1;
                        }
                        
                    }
                    else //crc does not match
                    {
                        sendTranferControlPack(process, PACK_TYPE_RESEND);   //TODO: resend or ack
                        eventPost(process->enumeration, eLowLevel, NULL, 0); //check if there is any data in the Uart-SW buf
                        clearRxBuf(process);
                        gdmPackTimeOutStop(); //stop timeout counter
                        //TODO: send NACK or resend
                    }
                    eventPost(process->enumeration, eLowLevel, NULL, 0); //check if there is any data in the Uart-SW buf
                }
                else
                {
                    if(EXIT_SUCCESS == checkComReceiveTimeout(params))
                    {
                        timerEventStart(&params->getCrcTimer, 5);
                    }
                }
            }
            break;

            case eFrameRooter:
            {
                //root valid frame to necessary event in a process
                for (i = 0; i < params->mapsize; i++)
                {
                    if (params->rxBufGdm.packType == params->mapArr[i].packType)
                    {
                        if (params->rxBufGdm.command == params->mapArr[i].command)
                        {
                            eventPost(params->mapArr[i].process, params->mapArr[i].event, params->rxBufGdm.data, params->dataLength);
                            break;
                        }
                    }
                }
            }
            break;

            case eControlPackSafir: // no ACK no new message, resend
            {
                uint8_t controlPack;
                memcpy(&controlPack, eventData, sizeof(controlPack));
                getTransferControlPacks(process, controlPack);
            }
            break;

            case eGDMDTx: //framer TX
            {
                //                if(transferStatus.readyToSend)
                if (transferStatus.resume)
                {
                    sendTranferControlPack(process, PACK_TYPE_RESUME);
                    transferStatus.resume = FALSE;
                }
                else
                {
                    if (FIFO_LOAD(params->queue))
                    {
                        tsGdmtQueueItem queData;

                        FIFO_POP(params->queue, &queData, sizeof(queData));

                        params->txBufGdm.preamble     = GDM_FRAME_PREAMBLE;
                        params->txBufGdm.packType     = queData.packType;
                        params->txBufGdm.command      = queData.command;
                        params->txBufGdm.packDataSize = htonl(queData.length);

                        if (queData.length)
                        {
                            FIFO_POP(params->queue, params->txBufGdm.data, queData.length);
                        }
                        params->txBufGdm.crc = htons(gdmCalcCrc(&params->txBufGdm, queData.length));

                        memcpy(&params->txBufGdmResend, &params->txBufGdm, sizeof(params->txBufGdm)); // for resend buf

                        devComSend(params->com, &params->txBufGdm, WIFI_OTA_FRAMER_OVERHEAD_BYTES_EXCEPT_DATA_AND_CRC);
                        devComSend(params->com, params->txBufGdm.data, queData.length);
                        devComSend(params->com, &params->txBufGdm.crc, sizeof(params->txBufGdm.crc));

                        transferStatus.waitForAck  = TRUE;
                        transferStatus.readyToSend = FALSE;
                    }
                }
                if (transferStatus.resend) //TODO: Dont send directly pu this data into LIFO
                {
                    tsGdmtQueueItem que;
                    que.packType = params->txBufGdmResend.packType;
                    que.command  = params->txBufGdmResend.command;
                    que.length   = params->txBufGdmResend.packDataSize;

                    if (params->txBufGdmResend.packDataSize) //TODO: check thiss!!!!
                    {
                        LIFO_PUSH(params->queue, params->txBufGdmResend.data, que.length);
                    }

                    LIFO_PUSH(params->queue, &que, sizeof(que));

                    transferStatus.resend = FALSE;
                }

                timerEventStart(&params->framerTimerTx, 5); //gdm Tx
            }
            break;

            case eLowLevelPackTimeOut:
            {
                if (sLowLevelPackTimeout.started)
                {
                    if (sLowLevelPackTimeout.counter)
                    {
                        sLowLevelPackTimeout.counter--;
                    }
                    else
                    {
                        gLowLevelPackDrop = TRUE; //AOM TODO: timeout occurs resend should be sent!!!!

                        // gdm_uart_init();            //TODO: uart module reset is necessary?????
                    }
                }

                if (gLowLevelPackDrop)
                {
                    sendTranferControlPack(process, PACK_TYPE_RESEND); //packet timeout finished or send resend
                    gLowLevelPackDrop = FALSE;
                }

                timerEventStart(&params->framerLowLevelPackTimeOut, 15); //gdm low level packet timeout
            }
            break;

            case eSendResume:
            {
                transferStatus.resume = TRUE;
                eventPost(process->enumeration, eGDMDTx, NULL, 0);
                // sendTranferControlPack(process, PACK_TYPE_RESUME);
            }
            break;

            default:
                break;
        }
    }
    else //run  events coming from outside of process_gdm--> eGDMDTx event will send the data
    {
        uint8_t i;

        for (i = 0; i < params->mapsize; i++)
        {
            if ((eventCurrent.source == params->mapArr[i].process) &&
                (eventCurrent.event == params->mapArr[i].event))

            {
                if ((sizeof(tsGdmtQueueItem) + eventCurrent.length) <= FIFO_FREE(params->queue)) // check if there is enough space in que
                {
                    tsGdmtQueueItem que;

                    que.packType = params->mapArr[i].packType;

                    que.command = params->mapArr[i].command;

                    que.length = eventCurrent.length;

                    FIFO_PUSH(params->queue, &que, sizeof(que));
                    if (eventCurrent.length)
                    {
                        FIFO_PUSH(params->queue, eventData, que.length);
                    }
                }
                break;
            }
        }
    }
}

void clearRxBuf(tsProcess *process)
{
    tsGdmParams *params = process->parameters;
    memset(&params->rxBufGdm, 0, offsetof(stGdmFrame,data));
    memset(params->rxBufGdm.data, 0, params->dataLength) ;
    params->rxBufGdm.crc = 0;
}

/*return = 0 --> not valid
 return >0 --> index number of "packageTypeIDs"*/

uint8_t checkPackTypeIsValid(uint8_t packType)
{
    uint8_t isValid = 0;
    uint8_t i;

    for (i = 0; i < PACK_TYPE_END; i++)
    {
        if (packageTypeIDs[i] == packType)
        {
            isValid = i;
            break;
        }
    }
    return isValid;
}

/*calculate crc for incoming messages*/
uint16_t gdmCalcCrc(stGdmFrame *frame, uint8_t size)
{
    volatile uint16_t retVal, idx;

    retVal = 0;
    retVal = crc16CcittAddData(retVal, frame->preamble);
    retVal = crc16CcittAddData(retVal, frame->packType);

    if (checkPackTypeIsValid(frame->packType) < PACK_TYPE_RESPONDS_START)
    {
        retVal = crc16CcittAddData(retVal, frame->command);
        retVal = crc16CcittAddData(retVal, (frame->packDataSize & 0xff));
        retVal = crc16CcittAddData(retVal, ((frame->packDataSize >> 8) & 0xff));
        retVal = crc16CcittAddData(retVal, ((frame->packDataSize >> 16) & 0xff));
        retVal = crc16CcittAddData(retVal, ((frame->packDataSize >> 24) & 0xff));
        for (idx = 0; idx < size; idx++)
        {
            retVal = crc16CcittAddData(retVal, frame->data[idx]);
        }
    }
    retVal = crc16CcittFinish(retVal);

    return retVal;
}

/*calculate crc for transfer packs such as ACK*/
uint16_t gdmTransferCalcCrc(stGdmTransfer *frame)
{
    volatile uint16_t retVal;

    retVal = 0;
    retVal = crc16CcittAddData(retVal, frame->preamble);
    retVal = crc16CcittAddData(retVal, frame->packType);

    retVal = crc16CcittFinish(retVal);

    return retVal;
}

void sendTranferControlPack(tsProcess *process, uint8_t packType)
{
    tsGdmParams *params = process->parameters;
    uint16_t temp16;

    if (packType < PACK_TYPE_RESPONDS_START) // such as ota wfa pack types(core packs)
    {
        params->txBufGdm.preamble = GDM_FRAME_PREAMBLE;
        params->txBufGdm.packType = packageTypeIDs[packType];
    }
    else // such as ack, nack ack pause etc  (transfer control packs)
    {
        params->txTransfer.preamble = GDM_FRAME_PREAMBLE;
        params->txTransfer.packType = packageTypeIDs[packType];
        temp16                      = htons(gdmTransferCalcCrc(&params->txTransfer));
        params->txTransfer.crc      = temp16;

        devComSend(params->com, &params->txTransfer, sizeof(params->txTransfer));
        //TODO: devcomsend!!!!!
    }
}

void getTransferControlPacks(tsProcess *process, uint8_t packType)
{
    switch (packType)
    {
        case PACK_TYPE_ACK:
        {
            transferStatus.waitForAck  = FALSE;
            transferStatus.readyToSend = TRUE;
            transferStatus.readyToRecv = TRUE;
        }
        break;
        case PACK_TYPE_RESEND:
        {
            transferStatus.waitForAck  = TRUE;
            transferStatus.readyToSend = FALSE;
            transferStatus.readyToRecv = TRUE;
        }
        break;
        case PACK_TYPE_ACK_PAUSE:
        {
        }
        break;
        case PACK_TYPE_ABORT:
        {
        }
        break;
        case PACK_TYPE_ABORT_PAUSE:
        {
        }
        break;
        case PACK_TYPE_RESUME:
        {
        }
        break;

        default:
            break;
    }
}

void gdmPackTimeOutStop(void)
{
    sLowLevelPackTimeout.started = 0;
    sLowLevelPackTimeout.counter = GDM_PACK_TIMEOUT_VAL;
}

void gdmPackTimeOutStart(void)
{
    sLowLevelPackTimeout.started = 1;
    sLowLevelPackTimeout.counter = GDM_PACK_TIMEOUT_VAL;
}

uint8_t checkComReceiveTimeout(tsGdmParams *params)
{
    if((params->retryCount++) >= MAX_RETRY_VALUE)
    {
        ///< If timeout expires, go back to start and wait for receiving preamble data
        params->retryCount = 0;
        timerEventStart(&params->framerLowLevelTimer, 5);
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}
/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */
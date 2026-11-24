/** @file       ccm.c
 *  @brief      ccm process
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     mertGemalmaz
 *  @date       23.10.2019
 */
#define FILE_CCM_C

/**
 *  @addtogroup CCM
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"
#include <string.h>
#include "ccm.h"

/*********************************************************************
 *  LOCAL CONSTANTS
 */

/*********************************************************************
 *  LOCAL TYPEDEFS
 */

/*********************************************************************
 *  LOCAL MACROS
 */
#define WFA_STATUS_INDEX 26 // wifi array status byte

#define WEMOD_CONNECTION_TIMEOUT 5         // 5 seconds
#define WEMOD_DUMMY_COMMAND_RETRY_COUNT 40 // 40 seconds

#define CCM_ATTRIBUTE_WRITTEN_PACKAGE_SIZE 3
#define CCM_WRITE_MAX_FRAME_SIZE_ATT_PACKAGE_SIZE 5      // cmdConf[1] + att[1] + datasize[1] + data[2]
#define CCM_READ_SUPPORTED_COMM_TYPES_ATT_PACKAGE_SIZE 2 // cmdConf[1] + att[1]
#define CCM_WRITE_CONN_BOARD_ID_ATT_PACKAGE_SIZE 4       // cmdConf[1] + att[1] + datasize[1] + data[1]
#define CCM_WRITE_COMM_MODE_ATT_PACKAGE_SIZE 5           // cmdConf[1] + att[1] + datasize[1] + data[2]
#define CCM_READ_STORED_SSID_ATT_PACKAGE_SIZE 2          // cmdConf[1] + att[1]
#define CCM_READ_COMM_PROT_VERSION_ATT_PACKAGE_SIZE 2    // cmdConf[1] + att[1]
#define CCM_WRITE_DEVICE_DESCP_ATT_PACKAGE_SIZE 4        // cmdConf[1] + att[1] + datasize[1] + data[1]
#define CCM_WRITE_CONN_STATE_WIFI_ATT_PACKAGE_SIZE 4     // cmdConf[1] + att[1] + datasize[1] + data[1]
#define CCM_READ_CONN_STATE_WIFI_ATT_PACKAGE_SIZE 5      // cmdConf[1] + att[1] + result[1] + datasize[1] + data[1]
#define CCM_ATT_READ_MAX_FRAME_SIZE_PACKAGE_SIZE 5       // cmdConf[1] + att[1] + datasize[1] + data[2]
#define CCM_ATT_READ_COMM_PROTOCOL_PACKAGE_SIZE 5        // cmdConf[1] + att[1] + datasize[1] + data[2]
#define CCM_READ_NETWORK_CARD_SW_VERSION_SIZE 2

#define CCM_CMDCONF_START_INDEX 0
#define CCM_ATTRIBUTE_ID_START_INDEX 1
#define CCM_ATTRIBUTE_RESULT_INDEX 2
#define CCM_ATTRIBUTE_WRITE_DATA__SIZE_INDEX 2
#define CCM_ATTRIBUTE_WRITE_DATA__START_INDEX 3
#define CCM_ATTRIBUTE_DATA_SIZE_INDEX 3
#define CCM_ATTRIBUTE_DATA_START_INDEX 4

#define CCM_ATTRIBUTE_READ_DATA_SIZE 2
#define CCM_ATTRIBUTE_READ_HEADER_SIZE 4            //cmdConf[1] + att[1] + result[1] + datasize[1]
#define CCM_ATTRIBUTE_READ_MAX_FRAMESIZE_PACKSIZE 6 //cmdConf[1] + att[1] + result[1] + datasize[1] + data[2]

#define BOARD_ID 5

#define CCM_SAFIR_TARGET_EVENT 0 //eccmSafirRxHandler event in ccmSafirProcess

/*********************************************************************
 *  LOCAL VARIABLES
 */

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */
static PROCESS_HANDLER_PROTO(ccmHandler);

void sendToCcmSafir(tsProcess *process, uint8_t idx, uint8_t ccmCommand, uint8_t *data, uint8_t length);

void sendAttrNotValid(void);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(ccmInit)
{
    tsCcmParams *params = process->parameters;
    ADD_RCOS_VERSION("ccm", "2.0.2")
    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, ccmHandler);
    process->initialized = 1; // If process needs other checks, set this another time
}

PROCESS_DEINIT_PROTO(ccmDeinit)
{
    tsCcmParams *params = process->parameters;
    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

static PROCESS_HANDLER_PROTO(ccmHandler)
{
    tsCcmParams *params = process->parameters;

    switch (eventCurrent.event)
    {
    case eCcmRecievedReadAttribute:
    {
        uint8_t i;
        tsAttributeReadType attResponse;
        uint8_t temp = 0;
        tsReadAttributeType attRecv;

        attRecv.cmdConf = eventData[CCM_CMDCONF_START_INDEX]; //TODO: what happens??
        attRecv.attrId = eventData[CCM_ATTRIBUTE_ID_START_INDEX];

        for (i = 0; i < params->mapsize; i++)
        {
            if (attRecv.attrId == params->attrbuteMap[i].attrId)
            {
                attResponse.attrId = params->attrbuteMap[i].attrId;
                attResponse.cmdConf = 0; //Always 0
                attResponse.result = resultSUCCESS;
                attResponse.dataSize = params->attrbuteMap[i].length;
                memcpy(&attResponse.data, params->attrbuteMap[i].data, params->attrbuteMap[i].length);

                eventReply(eCcmRecievedAttributeRead, &attResponse, (CCM_ATTRIBUTE_READ_HEADER_SIZE + attResponse.dataSize)); //TODO: No magic number !!!!!!

                sendToCcmSafir(process, i, CCM_CMD_READ_ATTRIBUTE, NULL, 0);

                temp = 0;
                break;
            }
            else
            {
                temp = 1;
            }
        }
        if (temp)
        {
            sendAttrNotValid();
        }
    }
    break;

    case eCcmRecievedWriteAttribute: //for now no write attr message should be recieved !!!!! TODO: check!!!!!
    {

        uint8_t i;
        tsWriteAttributeType attRecv;
        uint8_t temp = 0;

        attRecv.cmdConf = eventData[CCM_CMDCONF_START_INDEX]; //TODO: what happens??
        attRecv.attrId = eventData[CCM_ATTRIBUTE_ID_START_INDEX];
        attRecv.dataSize = eventData[CCM_ATTRIBUTE_WRITE_DATA__SIZE_INDEX];
        memcpy(&attRecv.data, &eventData[CCM_ATTRIBUTE_WRITE_DATA__START_INDEX], attRecv.dataSize);

        for (i = 0; i < params->mapsize; i++)
        {
            if (attRecv.attrId == params->attrbuteMap[i].attrId)
            {
                //TODO: check whether attribute is writable
                if (attRecv.dataSize == params->attrbuteMap[i].length)
                {
                    temp = 0;
                    break;
                }
                else
                {
                    temp = 1;
                }
            }
        }
        if (temp)
        {
            sendAttrNotValid();
        }
    }
    break;

    case eCcmRecievedAttributeWritten:
    {
        tsAttributeWrittenType attRecv;
        uint8_t i, temp;

        attRecv.cmdConf = eventData[CCM_CMDCONF_START_INDEX];
        attRecv.attrId = eventData[CCM_ATTRIBUTE_ID_START_INDEX];
        attRecv.result = eventData[CCM_ATTRIBUTE_RESULT_INDEX];

        for (i = 0; i < params->mapsize; i++)
        {
            if (attRecv.attrId == params->attrbuteMap[i].attrId)
            {
                sendToCcmSafir(process, i, CCM_CMD_ATTRIBUTE_WRITTEN, NULL, 0);
                temp = 0; //check valid command
                break;
            }
            else
            {
                temp = 1; //check valid command
            }
        }
        if (temp) //not a valid command !!!
        {
            sendAttrNotValid();
        }
    }
    break;

    case eCcmRecievedAttributeRead:
    {
        tsAttributeReadType attRec;
        uint8_t i, temp;

        attRec.cmdConf = eventData[CCM_CMDCONF_START_INDEX];
        attRec.attrId = eventData[CCM_ATTRIBUTE_ID_START_INDEX];
        attRec.result = eventData[CCM_ATTRIBUTE_RESULT_INDEX];
        attRec.dataSize = eventData[CCM_ATTRIBUTE_DATA_SIZE_INDEX];
        attRec.data = &eventData[CCM_ATTRIBUTE_DATA_START_INDEX];

        for (i = 0; i < params->mapsize; i++)
        {
            if (attRec.attrId == params->attrbuteMap[i].attrId) //TODO: no need to check data size is whether ok or not here
            {
                {
                    sendToCcmSafir(process, i, CCM_CMD_ATTRIBUTE_READ, attRec.data, attRec.dataSize); //attRec.data
                    temp = 0;                                                                           //check valid command
                    break;
                }
            }
            else
            {
                temp = 1; //check valid command
            }
        }
        if (temp) //not a valid command !!!
        {
            sendAttrNotValid();
        }
    }
    break;

    case eCcmRecievedAttributeEmitted:
    {
        uint8_t i;
        tsWriteEmitAttributeType attRecv;
        uint8_t temp = 0;
        attRecv.cmdConf = eventData[CCM_CMDCONF_START_INDEX]; //TODO: what happens??
        attRecv.attrId = eventData[CCM_ATTRIBUTE_ID_START_INDEX];
        attRecv.dataSize = eventData[CCM_ATTRIBUTE_WRITE_DATA__SIZE_INDEX];
        attRecv.data = &eventData[CCM_ATTRIBUTE_WRITE_DATA__START_INDEX];
        //memcpy(&attRecv.data, &eventData[CCM_ATTRIBUTE_WRITE_DATA__START_INDEX], attRecv.dataSize);

        for (i = 0; i < params->mapsize; i++)
        {
            if (attRecv.attrId == params->attrbuteMap[i].attrId)
            {

                sendToCcmSafir(process, i, CCM_CMD_ATTRIBUTE_EMITTED, attRecv.data, attRecv.dataSize); //
                //send info to ccmSafir END

                temp = 0;
                break;
                //}
            }
            else
            {
                temp = 1;
            }
        }
        if (temp)
        {
            sendAttrNotValid();
        }
    }
    break;

    case eCcmRecievedNotValidCommand:
    {
        //TODO: what happens NB send not valid command??
    }
    break;

    case eCcmSafirTx:
    {
        stCcmSafir ccmSafirRxBuf;
        uint8_t tempSize;

        memcpy(&ccmSafirRxBuf, eventData, sizeof(ccmSafirRxBuf));

        if (CCM_CMD_WRITE_ATTRIBUTE == ccmSafirRxBuf.comand)
        {
            tsWriteAttributeType attSend;
            attSend.attrId = ccmSafirRxBuf.attrId;
            attSend.cmdConf = 0x00;
            attSend.dataSize = ccmSafirRxBuf.size;
            memcpy(&attSend.data, ccmSafirRxBuf.data, ccmSafirRxBuf.size);

            tempSize = 1 + 1 + 1 + attSend.dataSize; //attrID + cmdConf + size

            eventPost(params->targetFramerProcess, eCcmRecievedWriteAttribute, &attSend, tempSize); //send to gdm
        }

        else if (CCM_CMD_READ_ATTRIBUTE == ccmSafirRxBuf.comand)
        {
            tsReadAttributeType attSend;

            attSend.attrId = ccmSafirRxBuf.attrId;
            attSend.cmdConf = 0x00;
            tempSize = 1 + 1;                                                                      //cmdConf + attrId
            eventPost(params->targetFramerProcess, eCcmRecievedReadAttribute, &attSend, tempSize); //send to gdm
        }
    }
    break;

    default:
        break;
    }
}

//notify ccmSafir
// TODO eventdata will be crashed after using this function

void sendToCcmSafir(tsProcess *process, uint8_t idx, uint8_t ccmCommand, uint8_t *data, uint8_t length)
{
    tsCcmParams *params = process->parameters;
    stCcmSafir ccmSafirTxBuf;
    uint8_t tempSize;
    // tsDeque buffer = DEQUE_INIT(eventData);
    // buffer.back = eventCurrent.length;

    // FIFO_PUSH(&buffer, params->attrbuteMap[idx].attrId, sizeof(params->attrbuteMap[idx].attrId));
    // FIFO_PUSH(&buffer, ccmCommand, sizeof(ccmCommand));
    // FIFO_PUSH(&buffer, params->attrbuteMap[idx].data, params->attrbuteMap[idx].length);

    // eventPost(params->targetSafirProcess, CCM_SAFIR_TARGET_EVENT, eventData, buffer.count);

    ccmSafirTxBuf.attrId = params->attrbuteMap[idx].attrId;
    ccmSafirTxBuf.comand = (teCcmCommands)ccmCommand;

    if (length >= params->attrbuteMap[idx].length)
    {
        length = params->attrbuteMap[idx].length;
    }
    ccmSafirTxBuf.size = length;

    memcpy(&ccmSafirTxBuf.data, data, length); //params->attrbuteMap[idx].length

    tempSize = 1 + 1 + 1 + ccmSafirTxBuf.size; //attrId + rwe + size + data

    eventPost(params->targetSafirProcess, CCM_SAFIR_TARGET_EVENT, &ccmSafirTxBuf, tempSize);
}

void sendAttrNotValid(void)
{
    tsAttributeNotValid attNoValid;

    attNoValid.cmdConf = 0; //Always 0
    attNoValid.result = NO_SUCH_ATTRIBUTE;

    eventReply(eCcmRecievedNotValidCommand, &attNoValid, sizeof(attNoValid));
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

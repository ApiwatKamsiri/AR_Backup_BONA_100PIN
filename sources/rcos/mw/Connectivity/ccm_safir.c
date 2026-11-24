/** @file       ccm_safir.c
 *  @brief      ccm_safir
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     mertGemalmaz
 *  @date       27.11.2019
 */
#define FILE_CCMSAFIR_C

/**
 *  @addtogroup CCMSAFIR
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"
#include <string.h>
#include "ccm_safir.h"

/*********************************************************************
 *  LOCAL CONSTANTS
 */

/*********************************************************************
 *  LOCAL TYPEDEFS
 */

/*********************************************************************
 *  LOCAL MACROS
 */

#define CCM_GDM_MAX_FRAME_SIZE 200

#define SUPPORTED_CONN_TYPE_WIFI 1
#define SUPPORTED_CONN_TYPE_BLE 2

#define CCM_SAFIR_TUNNEL_HEADER_SIZE 3 // attrId + cmd + size
#define CCM_SAFIR_TUNNEL_SIZE (CCM_SAFIR_TUNNEL_HEADER_SIZE + ccmTxBuf.size)

#define CCM_SAFIR_TUNNEL_READ_ATTR_SIZE 2 //attrId + cmd

/*********************************************************************
 *  LOCAL VARIABLES
 */

uint8_t dummyAttribute = 0; //TODO: Never used in db side

uint8_t connStateWifi = CONN_STATE_OFF; //CONN_STATE_UNDEFINED;
// uint8_t  tempConnStateWifi     ;
enum_connectionSubStateWifi connSubStateWifi = CONN_SUB_STATE_UNDEFINED;
uint16_t maxFrameSizeApplience;
uint16_t suppConnTypes;
uint8_t storedSSID[9] = "homeWhiz";
uint8_t commProtocolVersion[2] = {0xAA, 0xCC};
tsNbSwVersion nbSwVersion;
uint8_t safirPowerStatus= 0;

struct
{
    unsigned char counter;
    unsigned char status;
} sLowLevelCommStartTimeout;

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */
static PROCESS_HANDLER_PROTO(ccmSafirHandler);
void ccmVarInit(void);
void changeUiState(tsProcess *process, uint8_t uiState);

void ccmSend(tsProcess *process, atrributeIDs_e _attrId, teCcmCommands _command, uint8_t dataSize, uint8_t *data);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(ccmSafirInit)
{
    tsCcmSafirParams *params = process->parameters;
    ADD_RCOS_VERSION("ccmsafir", "2.0.10")

    PROCESS_STATE_CHANGE(process, ccmSafirHandler);
    process->initialized = 1; // If process needs other checks, set this another time

    ccmVarInit(); //init ccm variables

    timerEventStart(&params->tevframeSizeAtt, 5000);

    timerEventStart(&params->tevConnStatWifiAttr, 5000);

    timerEventStart(&params->tevConnSubState, 3000);

    timerEventStart(&params->tevNwBoardSwVerAttr, 10000);

    timerEventStart(&params->tevBarisTest, 5555);
    
    sLowLevelCommStartTimeout.status = eCommFail;
}

int8_t tttttttest;

PROCESS_DEINIT_PROTO(ccmSafirDeinit)
{
    tsCcmSafirParams *params = process->parameters;
    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

static PROCESS_HANDLER_PROTO(ccmSafirHandler)
{
    tsCcmSafirParams *params = process->parameters;

    UNUSED(params); // REMOVE IF USED

    switch (eventCurrent.event)
    {
    case eCcmSafirRxHandler: //tunnel messages coming from ccm.c
    {
        stCcmSafir ccmRxBuf;
        uint8_t i;

        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf));

        if (CCM_CMD_ATTRIBUTE_EMITTED == ccmRxBuf.comand)
        {
            for (i = 0; i < params->mapsize; i++)
            {
                if (ccmRxBuf.attrId == params->attrbuteMap[i].attrId)
                {
                    memcpy(params->attrbuteMap[i].data, ccmRxBuf.data, params->attrbuteMap[i].length);
                    break;
                }
            }
        }

        else //(CCM_CMD_ATTRIBUTE_WRITTEN == ccmRxBuf.comand) || (CCM_CMD_ATTRIBUTE_READ == ccmRxBuf.comand)
        {
            for (i = 0; i < params->mapsize; i++)
            {
                if (ccmRxBuf.attrId == params->attrbuteMap[i].attrId)
                {
                    eventPost(eventCurrent.destination, params->attrbuteMap[i].eventHandler, &ccmRxBuf, sizeof(ccmRxBuf));
                }
            }
        }
    }
    break;

    case eCcmSendWriteAttMaxFrameSize:
    {
        uint8_t maxFrameSizeTemp[2];
        memcpy(maxFrameSizeTemp, &maxFrameSizeApplience, sizeof(maxFrameSizeApplience));
        ccmSend(process, MAX_FRAME_SIZE, CCM_CMD_WRITE_ATTRIBUTE, sizeof(maxFrameSizeApplience), maxFrameSizeTemp);
    }
    break;

    case eCcmwriteAttMaxFrameSizeOta:
    {
        uint8_t maxFrameSizeTemp[2];
        maxFrameSizeApplience = (eventData[0] << 8) | (eventData[1]);
        memcpy(maxFrameSizeTemp, &maxFrameSizeApplience, sizeof(maxFrameSizeApplience));
        ccmSend(process, MAX_FRAME_SIZE, CCM_CMD_WRITE_ATTRIBUTE, sizeof(maxFrameSizeApplience), maxFrameSizeTemp);
    }
    break;

    case eCcmGetNetworkBoardSwVersion:
    {
        // timerEventStart(&params->tevNwBoardSwVerAttr, 5000);        
        ccmSend(process, NETWORK_CARD_SW_VERSION, CCM_CMD_READ_ATTRIBUTE, 0, NULL);        
    }
    break;

    case eCcmGetConnectivityStatus:
    {
    }
    break;

    case eCcmGetConnectionSubStateWifi:
    {

        if (connStateWifi > WIFI_STATE_CONFIG)
        {

            ccmSend(process, CONNECTION_SUBSTATE, CCM_CMD_READ_ATTRIBUTE, 0, NULL);
        }

        timerEventStart(&params->tevConnSubState, 3000);
    }
    break;

    case AttrHandlerMaxFrameSize:
    {
        stCcmSafir ccmRxBuf;
        tsAttributeWrittenType attRecv;

        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf)); //take data from eventque

        memcpy(&attRecv, &ccmRxBuf.data, ccmRxBuf.size);

        if (CCM_CMD_ATTRIBUTE_WRITTEN == ccmRxBuf.comand)
        {
            if (resultSUCCESS == attRecv.result)
            {
                ccmSend(process, SUPPORTED_CONNECTION_TYPES, CCM_CMD_READ_ATTRIBUTE, 0, NULL);
            }
            else
            {
                //TODO: what happens if result is not success!!!!!
            }
        }
    }
    break;
    case AttrHandlerDeviceDescription:
    {
        stCcmSafir ccmRxBuf;
        tsAttributeWrittenType attRecv;

        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf));
        memcpy(&attRecv, &ccmRxBuf.data, ccmRxBuf.size);

        if (CCM_CMD_ATTRIBUTE_WRITTEN == ccmRxBuf.comand)
        {
            if (resultSUCCESS == attRecv.result)
            {

                ccmSend(process, SUPPORTED_CONNECTION_TYPES, CCM_CMD_READ_ATTRIBUTE, 0, NULL);
            }
        }
    }
    break;
    case AttrHandlerSupportedConnTypes:
    {
        stCcmSafir ccmRxBuf;
        tsAttributeReadType attRecv;

        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf)); //take data from eventque

        memcpy(&attRecv, &ccmRxBuf.data, ccmRxBuf.size);

        if (CCM_CMD_ATTRIBUTE_READ == ccmRxBuf.comand)
        {
            if (resultSUCCESS == attRecv.result)
            {
                memcpy(&suppConnTypes, &attRecv.data, sizeof(suppConnTypes));

                ccmSend(process, CONNECTIVITY_BOARD_ID, CCM_CMD_WRITE_ATTRIBUTE, 0, NULL);
            }
            else
            {
                //TODO: what happens if result is not success!!!!!
            }
        }
    }
    break;
    case AttrHandlerCommMode:
    {
        stCcmSafir ccmRxBuf;
        tsAttributeWrittenType attRecv;

        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf)); //take data from eventque

        memcpy(&attRecv, &ccmRxBuf.data, ccmRxBuf.size);

        if (CCM_CMD_ATTRIBUTE_WRITTEN == ccmRxBuf.comand)
        {
            if (resultSUCCESS == attRecv.result)
            {
                ccmSend(process, STORED_SSID, CCM_CMD_READ_ATTRIBUTE, 0, NULL);
            }
            else
            {
                //TODO: what happens if result is not success!!!!!
            }
        }
    }
    break;
    case AttrHandlerMacAddress:
    {
        tttttttest = AttrHandlerMacAddress;
    }
    break;
    case AttrHandlerCommProtocolVersion:
    {
        stCcmSafir ccmRxBuf;
        tsAttributeReadType attRecv;

        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf)); //take data from eventque

        memcpy(&attRecv, &ccmRxBuf.data, ccmRxBuf.size);

        if (CCM_CMD_ATTRIBUTE_READ == ccmRxBuf.comand)
        {
            if (resultSUCCESS == attRecv.result)
            {
                memcpy(&commProtocolVersion, &attRecv.data, sizeof(commProtocolVersion));

                ccmSend(process, COMM_PROTOCOL_VERSION, CCM_CMD_READ_ATTRIBUTE, 0, NULL);
            }
            else
            {
                //TODO: what happens if result is not success!!!!!
            }
        }
    }
    break;
    case AttrHandlerConnStateWifi:
    {
        tttttttest = AttrHandlerConnStateWifi;
    }
    break;
    case AttrHandlerStoredSSID:
    {
        stCcmSafir ccmRxBuf;
        tsAttributeReadType attRecv;

        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf)); //take data from eventque

        memcpy(&attRecv, &ccmRxBuf.data, ccmRxBuf.size);

        if (CCM_CMD_ATTRIBUTE_READ == ccmRxBuf.comand)
        {
            if (resultSUCCESS == attRecv.result)
            {
                memcpy(&storedSSID, &attRecv.data, sizeof(storedSSID)); //TODO: stored SSID is not used in the protocol thats why we dont need to add ssid buffer for now!!!!

                ccmSend(process, DEVICE_DESCRIPTION, CCM_CMD_WRITE_ATTRIBUTE, 0, NULL);
            }
            else
            {
                //TODO: what happens if result is not success!!!!!
            }
        }
    }
    break;
    case AttrHandlerConnSubStateWifi:
    {
        stCcmSafir ccmRxBuf;
        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf));
        if (CCM_CMD_ATTRIBUTE_WRITTEN == ccmRxBuf.comand)
        {
        }
        if (CCM_CMD_ATTRIBUTE_READ == ccmRxBuf.comand)
        {
        }
    }
    break;
    case AttrHandlerOnboardingTimeOutDurationSec:
    {
        tttttttest = AttrHandlerOnboardingTimeOutDurationSec;
    }
    break;
    case AttrHandlerCustomizedName:
    {
        tttttttest = AttrHandlerCustomizedName;
    }
    break;
    case AttrHandlerConnBoardId:
    {

        stCcmSafir ccmRxBuf;
        tsAttributeWrittenType attRecv;

        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf)); //take data from eventque

        memcpy(&attRecv, &ccmRxBuf.data, ccmRxBuf.size);

        if (CCM_CMD_ATTRIBUTE_WRITTEN == ccmRxBuf.comand)
        {
            if (resultSUCCESS == attRecv.result)
            {
                ccmSend(process, COMMUNICATION_MODE, CCM_CMD_WRITE_ATTRIBUTE, 0, NULL);
            }
            else
            {
                //TODO: what happens if result is not success!!!!!
            }
        }
    }
    break;
    case AttrHandlerOtaState:
    {
        tttttttest = AttrHandlerOtaState;
    }
    break;
    
	case AttrHandlerNetworkCardSwVersion:
    {
        stCcmSafir ccmRxBuf;
        tsAttributeReadType attRecv;

        memcpy(&ccmRxBuf, eventData, sizeof(ccmRxBuf)); //take data from eventque

        memcpy(&attRecv, &ccmRxBuf.data, ccmRxBuf.size);

        if (CCM_CMD_ATTRIBUTE_READ == ccmRxBuf.comand)
        {
            // if (resultSUCCESS == attRecv.result)
            {
//                    memcpy(&nbSwVersion, &attRecv.data, sizeof(nbSwVersion));
                memcpy(&nbSwVersion, &ccmRxBuf.data, sizeof(nbSwVersion));
                
                // if(nbSwVersion.networkBoardSw[0] +
                //     nbSwVersion.networkBoardSw[1] +
                //     nbSwVersion.networkBoardSw[2] != 0)
                // {
                //     timerEventStop(&params->tevNwBoardSwVerAttr);
                // }
            }
        }
    }
    break;

    case eCcmSafirTest_sendconnStateWifi:
    {
        //            connStateWifi         = CONN_STATE_STA; //WIFI_STATE_CONFIG;
        ccmSend(process, CONNECTION_STATE_WIFI, CCM_CMD_WRITE_ATTRIBUTE, sizeof(connStateWifi), &connStateWifi);

        timerEventStart(&params->tevBarisTest, 3000);
    }
    break;

    case eCcmStartOnboarding:
    {
        connSubStateWifi = 0;
        connStateWifi = CONN_STATE_ONBOARDING;
        ccmSend(process, CONNECTION_STATE_WIFI, CCM_CMD_WRITE_ATTRIBUTE, sizeof(connStateWifi), &connStateWifi);

        changeUiState(process, UI_STATE_BROADCASTING);
        timerEventStart(&params->tevCheckOnboarding, 1000);
    }
    break;

    case eCcmCheckOnboarding:
    {
        if ((CONN_SUB_STATE_CONNECTING == connSubStateWifi) && (CONN_STATE_ONBOARDING == connStateWifi))
        {
            connStateWifi = CONN_STATE_STA;
            ccmSend(process, CONNECTION_STATE_WIFI, CCM_CMD_WRITE_ATTRIBUTE, sizeof(connStateWifi), &connStateWifi);
            /*UiEvent */
            changeUiState(process, UI_STATE_CONNECTING);
        }

        if (CONN_SUB_STATE_CONNECTED == connSubStateWifi)
        {
            //TODO: onboarding finished write to flash -->(onboarding finished)
            connStateWifi = CONN_STATE_STA;
            ccmSend(process, CONNECTION_STATE_WIFI, CCM_CMD_WRITE_ATTRIBUTE, sizeof(connStateWifi), &connStateWifi);

            changeUiState(process, UI_STATE_CONNECTED);
        }

        if (CONN_SUB_STATE_CONNECTED != connSubStateWifi)
        {
                timerEventStart(&params->tevCheckOnboarding, 50);
        }
    }
    break;
    
    case eCcmCheckHomewhizConnected:    //turn on homewhiz 
        connStateWifi = CONN_STATE_STA;
        break;
    
    case eCcmHomewhizStopBroadcasting:  //turn off broadcasting after onboarding timeout
    {
        if(eventData[0])    //0: no onboarding, //1: onboarded
        {
            connStateWifi = CONN_STATE_STA;
        }
        else
        {
            connStateWifi = CONN_STATE_OFF;
        }
    }
    break;
    
    case eCcmArrangeMaxFrameSize:
    {
        maxFrameSizeApplience = (eventData[0] << 8) | (eventData[1]);
        eventPost(process->enumeration,eCcmSendWriteAttMaxFrameSize,NULL,0);
    }
    break;

    case eCcmSafirGetCommStatus:
    {
        sLowLevelCommStartTimeout.status = eCommStarted;
    }
    break;

    case eCcmSafirCheckCommStarted:
    {
        safirPowerStatus = eventData[0];    //0: noPower, 1:power turned on
        eventPost(process->enumeration,eCcmSafirCheckCommStatus,NULL,0);
    }
    break;

    case eCcmSafirCheckCommStatus:
    {
        if(safirPowerStatus)
        {
            if(sLowLevelCommStartTimeout.status)    // =eCommStarted
            {
                eventPost(params->commStatProcess,params->commStatEvent,&sLowLevelCommStartTimeout.status,sizeof(sLowLevelCommStartTimeout.status));
            }
            else
            {
                if(sLowLevelCommStartTimeout.counter > 10)
                {
                    sLowLevelCommStartTimeout.status = eCommFail;
                    eventPost(params->commStatProcess,params->commStatEvent,&sLowLevelCommStartTimeout.status,sizeof(sLowLevelCommStartTimeout.status));
                    sLowLevelCommStartTimeout.counter = 0 ;
                }
                else
                {
                    sLowLevelCommStartTimeout.counter ++;
                    timerEventStart(&params->tevCommCheck,1000);
                }
            }
        }
    }
    break;

    default:
        break;
    }
}

//changes ui state according to onboarding state
void changeUiState(tsProcess *process, uint8_t uiState)
{
    tsCcmSafirParams *params = process->parameters;
    // uiState = CONN_SUB_STATE_CONNECTED;
    eventPost(params->uiProcess, params->uiEvent, &uiState, sizeof(uiState));
}

//init ccm variables
void ccmVarInit(void)
{
    suppConnTypes = SUPPORTED_CONN_TYPE_WIFI; //bit0 = 1 --> wifi from "supportedConnectionTypes_t"
    dummyAttribute = 0xFF;
    maxFrameSizeApplience = CCM_GDM_MAX_FRAME_SIZE;
    //connStateWifi         = CONN_STATE_STA; //WIFI_STATE_NOT_ACTIVE;
    connSubStateWifi = WIFI_STATE_CONNECTING;
}

//send ccmSafirdata to ccm in order to be sent via gdmFramer
void ccmSend(tsProcess *process, atrributeIDs_e _attrId, teCcmCommands _command, uint8_t dataSize, uint8_t *data)
{
    tsCcmSafirParams *params = process->parameters;
    stCcmSafir ccmTxBuf;
    uint8_t tempSize;

    ccmTxBuf.attrId = _attrId;
    ccmTxBuf.comand = _command;
    tempSize = CCM_SAFIR_TUNNEL_READ_ATTR_SIZE;

    if (CCM_CMD_WRITE_ATTRIBUTE == _command)
    {
        ccmTxBuf.size = dataSize;
        memcpy(&ccmTxBuf.data, data, dataSize);
        tempSize = CCM_SAFIR_TUNNEL_READ_ATTR_SIZE + sizeof(ccmTxBuf.size) + ccmTxBuf.size;
    }

    eventPost(params->carrierProcess, eCcmSafirTx, &ccmTxBuf, tempSize);
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

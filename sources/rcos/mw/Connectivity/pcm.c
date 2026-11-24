/** @file       pcm.c
 *  @brief      Program Config Model
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Umit Sahin
 *  @date       19.02.2021
 *  @author     Burak OZAN
 *  @date       09.09.2022
 */
#define FILE_PCM_C

/**
 *  @addtogroup PCM
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"
#include "pcm.h"
/*********************************************************************
 *  LOCAL CONSTANTS
 */

/*********************************************************************
 *  LOCAL TYPEDEFS
 */

/*********************************************************************
 *  LOCAL MACROS
 */
#define UPDATE_INDEX_ADDR_CNT 0
#define UPDATE_INDEX_ADDR_H 1
#define UPDATE_INDEX_ADDR_L 2
#define UPDATE_INDEX_ADDR_DATA 3
#define PCM_DEFAULT_DATA_SIZE 3

#define READ_INDEX_START_ADDR_H 0
#define READ_INDEX_START_ADDR_L 1
#define READ_INDEX_END_ADDR_H 2
#define READ_INDEX_END_ADDR_L 3
#define READ_INDEX_DATA 4

/*********************************************************************
 *  LOCAL VARIABLES
 */

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */
static PROCESS_HANDLER_PROTO(pcmHandler);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(pcmInit)
{
    tsPcmParams *params = process->parameters;
    ADD_RCOS_VERSION("pcm", "1.0.1") 
    UNUSED(params);                  // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, pcmHandler);
    process->initialized = 1; // If process needs other checks, set this another time
}

PROCESS_DEINIT_PROTO(pcmDeinit)
{
    tsPcmParams *params = process->parameters;
    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

static PROCESS_HANDLER_PROTO(pcmHandler)
{
    tsPcmParams *params = process->parameters;

    UNUSED(params); // REMOVE IF USED

    switch (eventCurrent.event)
    {
        case ePcmUpdateParameter:
        {
            uint8_t newData, idx, updateStatus = UPDATE_FAIL;
            uint8_t updateSize = eventData[UPDATE_INDEX_ADDR_CNT];
            uint16_t totalLength, arrayIndex;

            for (; updateSize > 0;)
            {//Update the datas from the gdm
                updateSize--;
				totalLength = 0;

                //Save the array index of data that will be updated
                arrayIndex = (eventData[UPDATE_INDEX_ADDR_H + PCM_DEFAULT_DATA_SIZE*updateSize] << 8) | eventData[UPDATE_INDEX_ADDR_L + PCM_DEFAULT_DATA_SIZE*updateSize];
                //Save the data that will be updated
                newData = eventData[UPDATE_INDEX_ADDR_DATA + PCM_DEFAULT_DATA_SIZE*updateSize];

                for (idx = 0; idx < params->mapsize; idx++)
                {//Find parameter from the map by looking at arrayIndex and update
                    if ((totalLength + params->pcmMap[idx].length) > arrayIndex)
                    {//Checking if arrayIndex less then next parameter's array index, so true map item is found
                        uint8_t paramByteIdx = arrayIndex - totalLength;
                        if(paramByteIdx < params->pcmMap[idx].length)
                        {//update the byte
                            memcpy((params->pcmMap[idx].address + paramByteIdx), &newData, 1);
                        }
                        break;
                    }

                    totalLength += params->pcmMap[idx].length;//next parameter's array index(start of the parameter)
                }

                updateStatus = UPDATE_SUCCESS;
            }

            eventReply(ePcmUpdateStatus, &updateStatus, sizeof(updateStatus));
        }
        break;

        case ePcmReadParameters:
        {
            uint8_t idx;
            uint16_t startAddress, endAddress, dataSize, totalLength;

            //Save the first index of data will be read
            startAddress = (eventData[READ_INDEX_START_ADDR_H] << 8) | eventData[READ_INDEX_START_ADDR_L];
            //Save the last index of data will be read
            endAddress = (eventData[READ_INDEX_END_ADDR_H] << 8) | eventData[READ_INDEX_END_ADDR_L];
            //Save the amount of data will be read
            dataSize = (endAddress - startAddress) + 1;

            for (idx = 0; idx < params->mapsize; idx++)
            {//Calculate the total length of pcm map items
                totalLength += params->pcmMap[idx].length;
            }

            if ((endAddress > totalLength) ||//end address exceed the total length
                (startAddress > endAddress) ||//faulty address
                (dataSize + READ_INDEX_DATA > sizeof(eventData)))//event buffer is smaller than amont of data will be read
            {//Send fail
                uint8_t updateStatus = UPDATE_FAIL;

                eventReply(ePcmUpdateStatus, &updateStatus, sizeof(updateStatus));
            }
            else
            {//Save the data and send
                memcpy(&eventData[4], (params->pcmMap[0].address + startAddress), dataSize);

                eventReply(ePcmParametersRead, &eventData, (dataSize + READ_INDEX_DATA));
            }
        }
        break;

        case ePcmResetAllParameters:
        {//Set 0 to all pcm map items
            uint8_t idx;

            for (idx = 0; idx < params->mapsize; idx++)
            {
                memset(params->pcmMap[idx].address, 0x00, params->pcmMap[idx].length);
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

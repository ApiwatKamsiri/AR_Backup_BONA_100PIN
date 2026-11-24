/** @file       data_linker.c
 *  @brief      Connects event carried data to volatile memories
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author
 *  @date       28.02.2019
 */
#define FILE_DATA_LINKER_C

/**
 *  @addtogroup DATA_LINKER
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "data_linker.h"
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
PROCESS_HANDLER_PROTO(dataLinkerHandler);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(dataLinkerInit)
{
    ADD_RCOS_VERSION("datalinker", "2.0.0")

    PROCESS_STATE_CHANGE(process, dataLinkerHandler);
}

PROCESS_DEINIT_PROTO(dataLinkerDeinit)
{
    PROCESS_STATE_CHANGE(process, NULL);
}

extern const tsDataSet *dataSetArrPtr;
extern const uint8_t dataSetArrSize;

PROCESS_HANDLER_PROTO(dataLinkerHandler)
{
    switch (eventCurrent.event)
    {
        case eDataLinkerSet:
            if (eventCurrent.length > sizeof(tsDataLinkerCommand))
            {
                tsDataLinkerCommand *command = (tsDataLinkerCommand *)eventData;
                uint8_t idx;

                if (eventCurrent.length == (sizeof(tsDataLinkerCommand) + command->size))
                {
                    for (idx = 0; idx < dataSetArrSize; idx++)
                    {
                        if (dataSetArrPtr[idx].index == command->dataSet)
                        {
                            if (dataSetArrPtr[idx].size >= (command->offset + command->size))
                            {
                                memcpy(dataSetArrPtr[idx].address, &eventData[sizeof(tsDataLinkerCommand)], command->size);
                            }
                            break;
                        }
                    }
                }
            }
            break;

        case eDataLinkerGet:
            if (eventCurrent.length == sizeof(tsDataLinkerCommand))
            {
                tsDataLinkerCommand *command = (tsDataLinkerCommand *)eventData;
                uint8_t idx;

                for (idx = 0; idx < dataSetArrSize; idx++)
                {
                    if (dataSetArrPtr[idx].index == command->dataSet)
                    {
                        if (dataSetArrPtr[idx].size >= (command->offset + command->size))
                        {
                            memcpy(&eventData[sizeof(tsDataLinkerCommand)], dataSetArrPtr[idx].address, command->size); ///< @warning  Dirty trick depends on eventData size, be careful!!!
                            eventReply(eDataLinkerSet, eventData, sizeof(tsDataLinkerCommand) + command->size);
                        }
                        break;
                    }
                }
            }
            break;

        default:
            break;
    }
}

uint8_t dataSetSend(tProcessEnum comProcess, uint8_t dataSetNo)
{
    uint8_t idx;

    for (idx = 0; idx < dataSetArrSize; idx++)
    {
        if (dataSetArrPtr[idx].index == dataSetNo)
        {
            tsDataLinkerCommand command;
            uint8_t buffer[128]; ///< @warning PREVENTS SENDING DATASET SIZE LARGER THAN 125 BYTES

            command.dataSet = dataSetNo;
            command.offset  = 0;
            command.size    = MIN(dataSetArrPtr[idx].size, sizeof(buffer));

            memcpy(buffer, &command, sizeof(command));
            memcpy(&buffer[sizeof(command)], dataSetArrPtr[idx].address, command.size);

            eventPost(comProcess, eDataLinkerSet, buffer, command.size + sizeof(command));

            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

uint8_t dataSetRequest(tProcessEnum comProcess, uint8_t dataSetNo)
{
    uint8_t idx;

    for (idx = 0; idx < dataSetArrSize; idx++)
    {
        if (dataSetArrPtr[idx].index == dataSetNo)
        {
            tsDataLinkerCommand command;

            command.dataSet = dataSetNo;
            command.offset  = 0;
            command.size    = dataSetArrPtr[idx].size;

            eventPost(comProcess, eDataLinkerGet, &command, sizeof(command));

            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

uint8_t dataSetSendPart(tProcessEnum comProcess, uint8_t dataSetNo, uint8_t offset, uint8_t length)
{
    uint8_t idx;

    for (idx = 0; idx < dataSetArrSize; idx++)
    {
        if (dataSetArrPtr[idx].index == dataSetNo)
        {
            tsDataLinkerCommand command;
            uint8_t buffer[128]; ///< @warning PREVENTS SENDING DATASET SIZE LARGER THAN 125 BYTES

            command.dataSet = dataSetNo;
            command.offset  = offset;
            command.size    = MIN(length, sizeof(buffer));

            memcpy(buffer, &command, sizeof(command));
            memcpy(&buffer[sizeof(command)], (uint8_t *)dataSetArrPtr[idx].address + offset, command.size);

            eventPost(comProcess, eDataLinkerSet, buffer, command.size + sizeof(command));

            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}

uint8_t dataSetRequestPart(tProcessEnum comProcess, uint8_t dataSetNo, uint8_t offset, uint8_t length)
{
    uint8_t idx;

    for (idx = 0; idx < dataSetArrSize; idx++)
    {
        if (dataSetArrPtr[idx].index == dataSetNo)
        {
            if (dataSetArrPtr[idx].size >= (offset + length))
            {
                tsDataLinkerCommand command;

                command.dataSet = dataSetNo;
                command.offset  = offset;
                command.size    = length;

                eventPost(comProcess, eDataLinkerGet, &command, sizeof(command));
                return EXIT_SUCCESS;
            }
            else
            {
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_FAILURE;
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

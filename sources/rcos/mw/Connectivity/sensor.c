/** @file       sensor.c
 *  @brief      SENSOR application program file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ozkan AKGUL
 *  @date       1.12.2022
 */
#define FILE_SENSOR_C

#include "sensor.h"

#define DEBUG_FILE_NAME "sensor"

///< Index definitions in packet acording to protocol - sensor packet - element start indexes
#define SENSOR_TYPE_INDEX 0
#define SENSOR_ID_INDEX 1
#define SENSOR_VALUE_INDEX 2
#define SENSOR_UNIT_INDEX 4
#define SENSOR_STATUS_LABEL_LENGTH_INDEX  6
#define SENSOR_STATUS_LABEL_INDEX   7
#define SENSOR_DATA_PACKET_LENGTH 7 ///< 7 bytes packet data 

#define SEND_TYPE_EMIT  1
#define SEND_TYPE_RESPONSE  2

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup SENSOR
 *  @{
 */

static PT_THREAD(sensorThread)
{
    tsSensorParams *params       = process->parameters;
    const tsSensorConsts *consts = process->constants;

    UNUSED(params);
    UNUSED(consts);
    PT_BEGIN();

    for (;;)
    {
        PT_YIELD();
    }

    PT_END();
}

static PROCESS_HANDLER_PROTO(sensorHandler);
static void prepareAndSendSensorData(tsSensorParams *params, const tsSensorConsts *consts, uint8_t sensorIndex, teSensorEvents event, uint8_t sendType);

static void prepareAndSendSensorData(tsSensorParams *params, const tsSensorConsts *consts, uint8_t sensorIndex, teSensorEvents event, uint8_t sendType)
{
    tsSensorElement *sensor;
    uint8_t result = EXIT_FAILURE;
    uint8_t baseIndex = 0;

    ///< get sensor data from array
    sensor = (tsSensorElement *)&consts->sensorArray[sensorIndex];
    
    char * sensorStatusLabel = (char *)*sensor->sensorStatus;
    uint8_t sensorStatusLength = strlen(sensorStatusLabel); 
    ///< check event data, it is index of the array.
    if ( sensorStatusLength > (params->sensorBufferSize - SENSOR_DATA_PACKET_LENGTH))
    {
        ///< do not try to send data - packet exceeds buffer
        return;
    }
    else
    {
        if (sendType == SEND_TYPE_RESPONSE)
        {
            ///< if this command is for just a single sensor read
            baseIndex = 1; ///< start from index 1 - first index is number of sensor -> it is 1
            params->sensorDataBuffer[0] = 1;    ///< Number of sensors - for single sensor it should be 1
        }

        uint16_t sensorValue = htons(*sensor->sensorValue);    ///< for MSB first issue
        uint16_t measurementUnit = htons(sensor->measurementUnit);  ///< for MSB first issue
        
        ///< fill buffer with sensor data and send it to eventqueue
        params->sensorDataBuffer[baseIndex + SENSOR_TYPE_INDEX] = sensor->sensorType; ///< sensor type
        params->sensorDataBuffer[baseIndex + SENSOR_ID_INDEX] = sensor->sensorId; ///< sensor id
        memcpy(&params->sensorDataBuffer[baseIndex + SENSOR_VALUE_INDEX], &sensorValue, sizeof(sensorValue));  ///< value
        memcpy(&params->sensorDataBuffer[baseIndex + SENSOR_UNIT_INDEX], &measurementUnit, sizeof(measurementUnit));  ///< measurement unit
        params->sensorDataBuffer[baseIndex + SENSOR_STATUS_LABEL_LENGTH_INDEX] = sensorStatusLength; ///< length of label
        memcpy(&params->sensorDataBuffer[baseIndex + SENSOR_STATUS_LABEL_INDEX], sensorStatusLabel, sensorStatusLength);  ///< status label

        ///< send packet to gdm process
        result = eventPost(consts->gdmProcessEnum, event, params->sensorDataBuffer, (baseIndex + sensorStatusLength + SENSOR_DATA_PACKET_LENGTH));
        
        if (result == EXIT_FAILURE)
        {
            /// don't add postpone event, instead reset sensor counter to start squence immediately
            sensor->counter = 0;
        }
        else
        {
            ///< reset period - to avoid recurrent sendings
            sensor->counter = sensor->sendPeriod;
        }
    }
}

/// @brief  Initialization function of sensor
PROCESS_INIT_PROTO(sensorInit)
{
    tsSensorParams *params       = process->parameters;
    const tsSensorConsts *consts = process->constants;
    ADD_RCOS_VERSION("sensor", "1.0.1")

    UNUSED(consts); // REMOVE IF USED
    params->timerCheckPeriods.post.destination = process->enumeration;
    params->timerCheckPeriods.post.event = eSensorEventsCheckPeriods;
    
    params->timerResendPacket.post.destination = process->enumeration;
    params->timerResendPacket.post.event = eSensorEventsReadAllSensors;
    
    timerEventStart(&params->timerCheckPeriods, 1000); ///< every one second

    PROCESS_STATE_CHANGE(process, sensorHandler);
    //threadStart(process, sensorThread);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of sensor
PROCESS_DEINIT_PROTO(sensorDeinit)
{
    tsSensorParams *params       = process->parameters;
    const tsSensorConsts *consts = process->constants;

    UNUSED(consts); // REMOVE IF USED
    UNUSED(params); // REMOVE IF USED    

    PROCESS_STATE_CHANGE(process, NULL);
    threadStop(process, process->threadFunction);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of sensor
static PROCESS_HANDLER_PROTO(sensorHandler)
{
    tsSensorParams *params       = process->parameters;
    const tsSensorConsts *consts = process->constants;  

    switch (eventCurrent.event)
    {
        case EVENT_NONE: // FILL WITH NEW EVENTS
            break;

        case eSensorEventsCheckPeriods:
        {
            uint8_t index = 0;
            uint8_t result = EXIT_FAILURE;
            tsSensorElement *sensor;
            for (index = 0; index < consts->sensorCount; index++)
            {
                sensor = &consts->sensorArray[index];

                if (sensor->periodicSend == PERIODIC_SEND_DISABLED)
                {
                    ///< if it is disabled do not count or send any sensor data.
                    continue;
                }

                if (sensor->counter <= 0)
                {
                    ///< Send all parameters within another event to reduce process time
                    result = eventPost(process->enumeration, eSensorEventsSensorDataEmit, &index, sizeof(index));
                    
                    ///< 
                    if (result == EXIT_SUCCESS)
                    {
                        ///< update counter with period
                        sensor->counter = sensor->sendPeriod;
                    }
                }
                else
                {
                    sensor->counter--;
                }
            }
            timerEventStart(&params->timerCheckPeriods, 1000); ///< every one second
        }
        break;
        case eSensorEventsListSensors:
        {
            ///< send all sensors data
            uint8_t index = 0;
            uint16_t bufferIndex = 0;
            uint8_t result = EXIT_FAILURE;
            tsSensorElement *sensor;
            //char * sensorStatusLabel;
            
            ///< number of sensors
            params->sensorDataBuffer[0] = consts->sensorCount;
            bufferIndex++;  ///< number of sensors
            
            for (index = 0; index < consts->sensorCount; index++)
            {
                sensor = &consts->sensorArray[index];
                
                if (bufferIndex >= params->sensorBufferSize) break;
                
                uint32_t readPeriod = htonl(sensor->sendPeriod);    ///< for MSB first issue
                uint16_t measurementUnit = htons(sensor->measurementUnit);  ///< for MSB first issue
                
                ///< fill buffer with sensor data and send it to eventqueue
                params->sensorDataBuffer[bufferIndex + 0] = sensor->sensorType; ///< sensor type
                params->sensorDataBuffer[bufferIndex + 1] = sensor->sensorId; ///< sensor id
                memcpy(&params->sensorDataBuffer[bufferIndex + 2], &measurementUnit, sizeof(measurementUnit)); ///< measurementUnit
                memcpy(&params->sensorDataBuffer[bufferIndex + 4], &readPeriod, sizeof(readPeriod)); ///< read period
                
                               
                bufferIndex += 8; ///<  sensor type[1] + sensorId[1] + sensorMeasurementUnit[2]  + period[4]= 8
            }

            if (consts->sensorCount == 0)
            {
                ///< sensor is not found
                params->sensorDataBuffer[0] = 0;    ///< number of sensor
                params->sensorDataBuffer[1] = 0;    ///< sensor type
                params->sensorDataBuffer[2] = 0;    ///< sensor ID

                bufferIndex = 3;    /// number of sensors + sensor type + sensor ID -- all is zero
            }
            
            ///< send packet to gdm process
            result = eventPost(consts->gdmProcessEnum, eSensorEventsSensorList, params->sensorDataBuffer, bufferIndex);
            if (result == EXIT_FAILURE)
            {
                ///< resend packets - if it there is failure while adding event
                timerEventStart(&(params->timerResendPacket), 500);
            }
        }
        break;
        case eSensorEventsReadSensorData:
        {
            uint8_t index = 0;
            uint8_t result = EXIT_FAILURE;
            uint8_t sensorFound = EXIT_FAILURE;
            tsSensorElement *sensor;
            for (index = 0; index < consts->sensorCount; index++)
            {
                sensor = &consts->sensorArray[index];
                ///< eventData[0] -> Sensor type
                ///< eventData[1] -> Sensor ID
                if ((sensor->sensorType == eventData[0]) && (sensor->sensorId == eventData[1]))
                {
                    sensorFound = EXIT_SUCCESS;
                    ///< Send all parameters within another event to reduce process time
                    result = eventPost(process->enumeration, eSensorEventsSensorDataSingleRead, &index, sizeof(index));
                    
                    ///< 
                    if (result == EXIT_SUCCESS)
                    {
                        ///< update counter with period
                        sensor->counter = sensor->sendPeriod;
                    }
                }
            }

            if (sensorFound == EXIT_FAILURE)
            {
                ///< sensor is not found
                params->sensorDataBuffer[0] = 0;    ///< number of sensor
                params->sensorDataBuffer[1] = 0;    ///< sensor type
                params->sensorDataBuffer[2] = 0;    ///< sensor ID
                eventPost(consts->gdmProcessEnum, eSensorEventsSensorDataNotFound, params->sensorDataBuffer, 3);
            }
            
        }
        break;
        case eSensorEventsSensorDataEmit:
        {
            ///< eventData[0] -> index related with emitted sensor
            prepareAndSendSensorData(params, consts, eventData[0], eSensorEventsSensorDataEmit, SEND_TYPE_EMIT);          
        }
        break;
        
        case eSensorEventsReadAllSensors:
        {
            ///< send all sensors data
            uint8_t index = 0;
            uint8_t sensorStatusLength = 0;
            uint16_t bufferIndex = 0;
            uint8_t result = EXIT_FAILURE;
            tsSensorElement *sensor;
            tsSensorWorstStatusLabel *sensorWSLabelElement;
            char * sensorWorstStatusLabel;
            char * sensorStatusLabel;
            
            ///< number of sensors
            params->sensorDataBuffer[0] = consts->sensorCount;
            bufferIndex++;  ///< number of sensors
            
            for (index = 0; index < consts->sensorCount; index++)
            {
                sensor = &consts->sensorArray[index];
                
                if (bufferIndex >= params->sensorBufferSize) break;
                
                sensorStatusLabel = (char *)*sensor->sensorStatus;
                sensorStatusLength = strlen(sensorStatusLabel); 
                
                ///< fill buffer with sensor data and send it to eventqueue
                params->sensorDataBuffer[bufferIndex + SENSOR_TYPE_INDEX] = sensor->sensorType; ///< sensor type
                params->sensorDataBuffer[bufferIndex + SENSOR_ID_INDEX] = sensor->sensorId; ///< sensor id
                params->sensorDataBuffer[bufferIndex + SENSOR_VALUE_INDEX + 1] = *sensor->sensorValue; ///< LSB first
                params->sensorDataBuffer[bufferIndex + SENSOR_VALUE_INDEX] = *((uint8_t *)sensor->sensorValue + 1); ///< MSB second (protocol takes MSB first)
                params->sensorDataBuffer[bufferIndex + SENSOR_UNIT_INDEX + 1] = sensor->measurementUnit & 0x00FF; ///< LSB first
                params->sensorDataBuffer[bufferIndex + SENSOR_UNIT_INDEX] = (sensor->measurementUnit >> 8) & 0x00FF; ///< MSB second (protocol takes MSB first)
                params->sensorDataBuffer[bufferIndex + SENSOR_STATUS_LABEL_LENGTH_INDEX] = sensorStatusLength; ///< length of label
                memcpy(&params->sensorDataBuffer[bufferIndex + SENSOR_STATUS_LABEL_INDEX], sensorStatusLabel, sensorStatusLength);  ///< status label
               
                bufferIndex += SENSOR_DATA_PACKET_LENGTH + sensorStatusLength;

                ///< reset counter to avoid recurrent sendings
                sensor->counter = sensor->sendPeriod;
            }

            if (consts->sensorCount == 0)
            {
                ///< sensor is not found
                params->sensorDataBuffer[0] = 0;    ///< number of sensor
                params->sensorDataBuffer[1] = 0;    ///< sensor type
                params->sensorDataBuffer[2] = 0;    ///< sensor ID

                bufferIndex = 3;    /// number of sensors + sensor type + sensor ID -- all is zero
            }
            else
            {
                ///<    if sensor count is not zero - then check if there is worst label items
                ///<    For worst labels
                if ((bufferIndex + 1) < ARRAY_SIZE(params->sensorDataBuffer))
                {
                    params->sensorDataBuffer[bufferIndex] = consts->sensorWorstLabelCount;   ///< add sensor label count
                    bufferIndex++;
                } 

                for (index = 0; index < consts->sensorWorstLabelCount; index++)
                {
                    if (consts->sensorWorstLabelArray == NULL) break;
                    
                    sensorWSLabelElement = &consts->sensorWorstLabelArray[index];
                    sensorWorstStatusLabel = (char *)*sensorWSLabelElement->sensorStatus;

                    if (bufferIndex >= params->sensorBufferSize) break;

                    sensorStatusLength = strlen(sensorWorstStatusLabel);

                    params->sensorDataBuffer[bufferIndex] = sensorWSLabelElement->sensorType;   ///< add sensor type 
                    params->sensorDataBuffer[bufferIndex + 1] = sensorStatusLength; ///< length of label  (+1 = sensor Type[1])
                    memcpy(&params->sensorDataBuffer[bufferIndex + 2], sensorWorstStatusLabel, sensorStatusLength);  ///< status label - sensor type and length equals two bytes

                    bufferIndex += 2 + sensorStatusLength;  ///<    Sensor Type[1] + WStatus Label Length[1] = 2
                }
            }
            
            
            ///< send packet to gdm process
            result = eventPost(consts->gdmProcessEnum, eSensorEventsSensorDataRead, params->sensorDataBuffer, bufferIndex);
            if (result == EXIT_FAILURE)
            {
                ///< resend packets - if it there is failure while adding event
                timerEventStart(&(params->timerResendPacket), 500);
            }
        }
        break;
        
        case eSensorEventsSensorDataSingleRead:
        {
            ///< eventData[0] -> index related with emitted sensor
            prepareAndSendSensorData(params, consts, eventData[0], eSensorEventsSensorDataRead, SEND_TYPE_RESPONSE);
        }
        break;

        default:
            break;
    }
}

/** @} */

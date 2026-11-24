/** @file       sensor.h
 *  @brief      SENSOR application header file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ozkan AKGUL
 *  @date       1.12.2022
 */
#ifndef FILE_SENSOR_H
#define FILE_SENSOR_H

/** INCLUDES ******************************************************************/
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_SENSOR_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   SENSOR SENSOR
 *  @ingroup    APP
 *  @brief      GDM sensor packet control 
 *  @{
 */

#define PACK_TYPE_SENSOR    0x80
#define PERIODIC_SEND_ENABLED   1
#define PERIODIC_SEND_DISABLED  2

typedef enum
{
    SENSOR_LIST                 = 0x01,         ///< request from network board for sensor list
    SENSOR_READ_DATA            = 0x03,         ///< request from network board for single sensor value - async read
    SENSOR_READ_ALL             = 0x05,         ///< request from network board for reading all sensor values
    SENSOR_LISTED               = 0x02,         ///< response from product board for sensor list request
    SENSOR_DATA_READ            = 0x04,         ///< response from product board for single sensor read
    SENSOR_DATA_EMIT            = 0x06,         ///< emit from product for single sensor value emit.
    SENSOR_DATA_NOT_FOUND       = 0x07,         ///< sensor data couldn't found - type and ID mismatch
} teSensorCommands;

/// @brief  Events of a Sensor process
typedef enum
{
    eSensorEventsNone,
    eSensorEventsCheckPeriods,
    eSensorEventsListSensors,
    eSensorEventsReadSensorData,
    eSensorEventsReadAllSensors,
    eSensorEventsSensorList,
    eSensorEventsSensorDataRead,
    eSensorEventsSensorDataSingleRead,
    eSensorEventsSensorDataEmit,
    eSensorEventsSensorDataNotFound,
    eSensorEventChangeActiveStatus,
    eSensorEventChangePeriod,
} teSensorEvents;

/// @brief Sensor Types
typedef enum
{
    eSensorTypeNone,
    eSensorTypePMSensor,
    eSensorTypeHumidity,
    eSensorTypeTemperature,
}teSensorTypes;

/// @brief Measurement Units
typedef enum
{
    eMeasurementUnitNone,
    eMeasurementUnitPMSensorUnit1,
    eMeasurementUnitPMSensorUnit2,
    eMeasurementUnitPMSensorUnit3,
    eMeasurementUnitPMSensorUnit4,
    eMeasurementUnitPMSensorUnit5,
    eMeasurementUnitHumiditySensorUnit1,
    eMeasurementUnitHumiditySensorUnit2,
    eMeasurementUnitHumiditySensorUnit3,
    eMeasurementUnitHumiditySensorUnit4,
    eMeasurementUnitHumiditySensorUnit5,
    eMeasurementUnitEnergySensorUnit1,
    eMeasurementUnitEnergySensorUnit2,
    eMeasurementUnitEnergySensorUnit3,
    eMeasurementUnitEnergySensorUnit4,
    eMeasurementUnitEnergySensorUnit5,
}teMeasurementUnits;

/// @brief Sensor component
typedef struct
{
    teSensorTypes sensorType;                       ///<    Sensor Type
    uint16_t measurementUnit;                       ///<    Measurement Unit
    uint8_t sensorId;                               ///<    Sensor ID
    uint16_t *sensorValue;                          ///<    Sensor Value
    char **sensorStatus;                            ///<    Sensor Status (string label) adress
    uint32_t sendPeriod;                            ///<    Send period of the sensor value in seconds
    uint32_t counter;                               ///<    Counter for sending operations
    uint8_t periodicSend;                           ///<    Periodic send enabled or disabled
}tsSensorElement;

/// @brief Sensor worst status label 
typedef struct
{
    teSensorTypes sensorType;                       ///<    Sensor Type
    char **sensorStatus;                            ///<    Sensor Worst Status (string label) adress
}tsSensorWorstStatusLabel;

/// @brief  Parameters of a Sensor process
typedef struct
{
    tsTimerEvent timerCheckPeriods;                 ///<    Timer for checking periods.
    tsTimerEvent timerResendPacket;                 ///<    Timer for resending sensor data
    uint8_t *sensorDataBuffer; ///<    Buffer for the temporary sensor values.
    uint16_t sensorBufferSize; ///<     Buffer size for temporary sensor values.
} tsSensorParams;

/// @brief  Constants of a Sensor process
typedef struct
{
    tsSensorElement *sensorArray;                       ///<    Pointer for the sensor array
    tsSensorWorstStatusLabel *sensorWorstLabelArray;    ///<    Sensor worst status label array
    uint8_t sensorWorstLabelCount;                      ///<    Total sensor worst label count for the system. Limited to 256
    uint8_t sensorCount;                                ///<    Total sensor count for the system. Limited to 256
    uint8_t gdmProcessEnum;                             ///<    Gdm process enum
} tsSensorConsts;

/** @brief  Sensor element creation macro
 *  @param  _type   Type of the sensor
 *  @param  _unit   Measurement unit of the sensor
 *  @param  _value  Value of the sensor
 *  @param  _label  Status label of the sensor - char pointer
 *  @param  _sendPeriod   Send period of the sensor value - in seconds.
 *  @param  _periodicSend   Periodic send enabled or disabled - use PERIODIC_SEND_ENABLED/PERIODIC_SEND_DISABLED
 */
#define SENSOR_ELEMENT(_type, _unit, _id, _value, _label, _sendPeriod, _periodicSend)   \
    {                                                               \
        _type, _unit, _id, _value, _label, _sendPeriod, _sendPeriod, _periodicSend,   \
    }  

 /** @brief  Sensor worst label element creation macro
 *  @param  _type   Type of the sensor
 *  @param  _label  Status label of the sensor - char pointer
 */   
#define SENSOR_WORST_STATUS_LABEL_ELEMENT(_type, _label)    \
    {                                                       \
        _type, _label                                       \
    }


/** @brief  Sensor process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 *  @param  _sensorArray    Pointer for the sensor array. 
 *  @param  _sensorWSLabelArray Sensor worst status label array 
 *  @param  _gdmProcessEnum Process enum for carrier operations - gdm
 *  @param  _maxSensorPacketSize    Maximum packet size for data buffer 
 */
#define PROCESS_SENSOR_CREATE(_name, _enum, _sensorArray, _sensorWSLabelArray, _gdmProcessEnum, _maxSensorPacketSize) \
    uint8_t _name##sensorDataBuffer[_maxSensorPacketSize];                                      \
    tsSensorParams _name##Params =           \
        {                                         \
        .sensorDataBuffer = _name##sensorDataBuffer,   \
        .sensorBufferSize = _maxSensorPacketSize,   \
    };                                            \
    const tsSensorConsts _name##Consts =     \
        {                                                                       \
        .sensorArray = _sensorArray,                                            \
        .sensorWorstLabelArray = _sensorWSLabelArray,                           \
        .sensorWorstLabelCount = ARRAY_SIZE(_sensorWSLabelArray),               \
        .sensorCount = ARRAY_SIZE(_sensorArray),                                \
        .gdmProcessEnum = _gdmProcessEnum,                                      \
    };                                                                          \
    PROCESS_CREATE(_name, sensorInit, sensorDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_SENSOR_H

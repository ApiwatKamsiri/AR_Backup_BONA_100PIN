/** @file       consumption.h
 *  @brief      CONSUMPTION application header file
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ozkan AKGUL
 *  @date       23.01.2023
 */
#ifndef FILE_CONSUMPTION_H
#define FILE_CONSUMPTION_H

/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "sensor.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_CONSUMPTION_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   CONSUMPTION CONSUMPTION
 *  @ingroup    APP
 *  @brief      GDM consumption packet layer
 *  @{
 */
#define PACK_TYPE_CONSUMPTION   0x90

/// @brief Consumption Packet Commands
typedef enum
{
    CONSUMPTION_READ_DATA                 = 0x01,         ///< request from network board for getting consumption values
    CONSUMPTION_DATA_PUBLISHED            = 0x03,         ///< info from network board for sending consumtion data to cloud
    CONSUMPTION_EMIT_DATA                 = 0x02,         ///< consumption data emit from product board
} teConsumptionCommands;

/// @brief sending options
typedef enum
{
    eSendingOptionsStop,                   ///<    Do not send any consumption data - stop
    eSendingOptionsAuto,                   ///<    Automatic send acording to element settings
    eSendingOptionsManual,                 ///<    Manual send with an event - no calculations.
} teSendingStatus;

/// @brief sending states
typedef enum
{
    eSendingStateDefault,                   ///<    Default case
    eSendingStateSentToNB,                  ///<    State after sending to nb
    eSendingStateSentToCloud,               ///<    State after sending to cloud
} teSendingStates;

/// @brief consumption types
typedef enum
{
    eConsumptionType_None,
    eConsumptionType_Energy_1,      //  1
    eConsumptionType_Energy_2,      //  2
    eConsumptionType_Energy_3,      //  3
    eConsumptionType_Energy_4,      //  4
    eConsumptionType_Energy_5,      //  5
    eConsumptionType_Water_1,       //  6
    eConsumptionType_Water_2,       //  7
    eConsumptionType_Water_3,       //  8
    eConsumptionType_Water_4,       //  9
    eConsumptionType_Water_5,       //  10
} teConsumptionTypes;

///< @brief return data type for published result
typedef struct PLATFORM_PACKED
{
    uint8_t type;           ///< consumption data
    uint8_t publishResult;         ///< result of publish action.
}tsPublishResultData;

/// @brief consumption data structure
typedef struct PLATFORM_PACKED
{
    uint8_t type;                       ///<    Consumption type from the list
    uint32_t totalConsumption;          ///<    Total consumption value.
    uint32_t totalDuration;             ///<    Duration for the consumption value - in minutes
    uint16_t measurementUnit;           ///<    Measurement unit from the list.
} tsConsumptionData;

/// @brief consumption element
typedef struct 
{
    teConsumptionTypes type;                        ///<    Type of the consumption element
    uint32_t * instantConsumption;                  ///<    Instant consumption value for the element
    uint32_t sendingLimit;                          ///<    Sending consumption limit for the element
    uint32_t sendingPeriod;                         ///<    Sending period for the element - in minmutes
    uint32_t storageMaxLimit;                       ///<    Consumption up limit for storage - no connection state
    uint32_t storagePeriod;                         ///<    Storage period for for offline case
    tsDevMem * storageDevice;                       ///<    Memory device for storage element
    uint32_t storageAddress;                        ///<    Storage start address for the element
    uint32_t timerCnt;                              ///<    Timer counter for periodic sending and total duration values
    uint32_t totalConsumption;                      ///<    Total consumption value.
    uint32_t convConstant;                          ///<    Conversion constant for conversion from instant consumption to total consumption - division value
    uint8_t sendingStatus;                          ///<    Sending status for the element - 0 : stop, 1 : auto send, 2 : manual send
    teMeasurementUnits measurementUnit;             ///<    Consumption value unit - from list
    uint8_t sendingState;                           ///<    Sending state for the element - 0 : default, 1 : sent to network board, 2 : sent to cloud.
} tsConsumptionElement;


/// @brief  Events of a Consumption process
typedef enum
{
    eCunsumptionEventsNone,
    eConsumptionEventsEmitConsumptionData,              ///<    This event for emitting consumption data
    eConsumptionEventsConsumptionDataPublished,         ///<    This event is triggered by GDM 
    eConsumptionEventsReadConsumptionData,              ///<    This event request from GDM to get consumption data
    eConsumptionEventsSendPeriodicData,                 ///<    This event is for sending consumption data periodically - auto mode
    eConsumptionEventsSendConsumptionDataManual,        ///<    This event is used to send consumption data manually
} teConsumptionEvents;

/// @brief  Parameters of a Consumption process
typedef struct
{
    tsConsumptionElement * consumptionArray;        ///<    Array of the consumption elements
    tsTimerEvent timerPeriodic;                     ///<    Timer for periodic consumption sending operations
} tsConsumptionParams;

/// @brief  Constants of a Consumption process
typedef struct
{
    tProcessEnum gdmProcessEnum;                ///<    GDM process enum
    tProcessEnum targetProcessEnum;             ///<    Target process enum
    uint8_t consumptionArraySize;               ///<    Size of consumption array
    tEventEnum targetEvent;                     ///<    Target event enum - redirecting response from NB
} tsConsumptionConsts;

/** @brief  Consumption element creation macro
 *  @param  _type   Type of the consumption - energy, water, etc.
 *  @param  _instantConsumption   Instant value of the constumption
 *  @param  _sendingLimit  Up limit for the consumption value for sending - send after this limit. Ie : >100Kwh
 *  @param  _sendingPeriod  Sending period if there is no sending consumption limit. Ie : 60 - in minutes
 *  @param  _storageMaxLimit   Maximum consumption level for storing in NVM device
 *  @param  _storagePeriod   Storage period for storing elements in NVM
 *  @param  _storageDevice   NVM device to store consumption elements
 *  @param  _storageAddress   NVM device start address for storing consumption elements
 *  @param  _convConstant   Conversion constant to convert from instant value to total value - ie W > KWh
 *  @param  _sendingStatus   Sending status for the element - 0 : stop, 1 : auto send, 2 : manual send 
 *  @param  _measurementUnit   Unit for the consumption value
 */
#define CONSUMPTION_ELEMENT(_type, _instantConsumption, _sendingLimit, _sendingPeriod, \
        _storageMaxLimit, _storagePeriod, _storageDevice, _storageAddress, _convConstant, _sendingStatus, _measurementUnit)   \
    {                                                               \
        _type, _instantConsumption, _sendingLimit, _sendingPeriod, _storageMaxLimit,\
        _storagePeriod, _storageDevice, _storageAddress, 0, 0, _convConstant, _sendingStatus, _measurementUnit, 0 \
    } 

/** @brief  Consumption process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_CONSUMPTION_CREATE(_name, _enum, _consumptionArray, _gdmProcessEnum, _targetProcess, _targetEvent) \
    tsConsumptionParams _name##Params =           \
    {                                             \
        .consumptionArray = _consumptionArray,    \
    };                                            \
    const tsConsumptionConsts _name##Consts =     \
    {                                             \
        .gdmProcessEnum = _gdmProcessEnum,        \
        .targetProcessEnum = _targetProcess,      \
        .consumptionArraySize = ARRAY_SIZE(_consumptionArray), \
        .targetEvent = _targetEvent,              \
    };                                            \
    PROCESS_CREATE(_name, consumptionInit, consumptionDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_CONSUMPTION_H

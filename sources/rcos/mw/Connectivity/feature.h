/** @file       feature.h
 *  @brief      FEATURE application header file
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Enes Inanc
 *  @date       6.07.2023
 */
#ifndef FILE_FEATURE_H
#define FILE_FEATURE_H

/** INCLUDES ******************************************************************/
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_FEATURE_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   FEATURE FEATURE
 *  @ingroup    APP
 *  @brief      Feature Pool
 *  @{
 */

#define PACK_TYPE_FEATURE 0xA0

/// @brief  Feature commands
typedef enum
{
    FEATURE_LIST   = 0x01, ///< request from network board for feature version list
    FEATURE_LISTED = 0x02, ///< response from product board for feature version list
} teFeatureCommands;

/// @brief Feature element
typedef struct
{
    uint8_t featureID;      ///<    Feature ID
    uint8_t featureVersion; ///<    Feature Version
} tsFeatureElement;

/// @brief  Events of a Feature process
typedef enum
{
    eFeatureEventsNone,
    eFeatureEventsListFeatures,
    eFeatureEventsFeatureListed,
} teFeatureEvents;

/// @brief  Parameters of a Feature process
typedef struct
{
    tsTimerEvent timerResendPacket; ///<    Timer for resending feature list
    uint8_t *featureDataBuffer;     ///<    Buffer for the temporary feature values
    uint16_t featureBufferSize;     ///<    Buffer size for temporary feature values
} tsFeatureParams;

/// @brief  Constants of a Feature process
typedef struct
{
    tsFeatureElement *featureArray; ///<    Pointer for the feature array
    uint8_t gdmProcessEnum;         ///<    Gdm process enum
    uint8_t featureCount;           ///<    Total feature count for the system. Limited to 255
} tsFeatureConsts;

typedef enum
{
    FEATURE_POOL_STARTS,  //   0
    ALLERGENFREE,         //   1
    AUTOBAKE,             //   2
    AWASH,                //   3
    CAMFRIDGE,            //   4
    COOLONDEMAND,         //   5
    DUSTFREE,             //   6
    ENERGYMANAGEMENT,     //   7
    SERVICECALL,          //   8
    VEGA,                 //   9
    WATERMANAGEMENT,      //  10
    WMTDSMARTPAIRING,     //  11
    ENERGY_SAVING_MODE,   //  12
} teFeatureType;


/** @brief  Feature element creation macro
 *  @param  _featureID          Feature ID
 *  @param  _featureVersion     Feature Version
 */
#define FEATURE_ELEMENT(_featureID, _featureVersion) \
    {                                                \
        _featureID, _featureVersion                  \
    }

/** @brief  Feature process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_FEATURE_CREATE(_name, _enum, _featureArray, _gdmProcessEnum, _maxFeaturePacketSize) \
    uint8_t _name##featureDataBuffer[_maxFeaturePacketSize];                                        \
    tsFeatureParams _name##Params =                                                                 \
        {                                                                                           \
            .timerResendPacket = TIMER_EVENT_INIT(_enum, _enum, eFeatureEventsListFeatures),        \
            .featureBufferSize = _maxFeaturePacketSize,                                             \
            .featureDataBuffer = _name##featureDataBuffer,                                          \
    };                                                                                              \
    const tsFeatureConsts _name##Consts =                                                           \
        {                                                                                           \
            .gdmProcessEnum = _gdmProcessEnum,                                                      \
            .featureArray   = _featureArray,                                                        \
            .featureCount   = ARRAY_SIZE(_featureArray),                                            \
    };                                                                                              \
    PROCESS_CREATE(_name, featureInit, featureDeinit, _enum, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif           // FILE_FEATURE_H

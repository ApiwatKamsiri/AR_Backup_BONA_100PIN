/** @file       pcm.h
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
#ifndef FILE_PCM_H
#define FILE_PCM_H

/**
 *  @defgroup   PCM PCM
 *  @ingroup    MW
 *  @brief      Program Config Model
 *  @{
 */

/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"

/*********************************************************************
 *  EXPORTED TYPEDEFS
 */
#define PACK_TYPE_PROGRAM_CONFIG 0x60

typedef enum
{
    PCM_UPDATE_PROGRAM_PARAMETER = 0x01,
    PCM_READ_PROGRAM_PARAMETERS = 0x02,
    PCM_UPDATE_STATUS = 0x03,
    PCM_PROGRAM_PARAMETERS_READ = 0x04,
    PCM_RESET_ALL_PARAMETERS = 0x10,
} tePcmCommands;

typedef struct PLATFORM_PACKED
{
    uint8_t *address;
    uint8_t length;
} tsPcmMap;

/// @brief  Parameters of a AppPcm process
typedef struct
{
    const tsPcmMap *pcmMap;
    uint16_t mapsize;
} tsPcmParams;

/// @brief  Events of a AppPcm process
typedef enum
{
    ePcmUpdateParameter,
    ePcmReadParameters,
    ePcmUpdateStatus,
    ePcmParametersRead,
    ePcmResetAllParameters,
} teAppPcmEvents;

typedef enum
{
    UPDATE_STATUS_START = 0,
    UPDATE_SUCCESS,
    UPDATE_FAIL,
    UPDATE_STATUS_END,
} tePcmUpdateStatus;

/*********************************************************************
 *  EXPORTED MACROS
 */
/** @brief  AppPcm process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 *  @param  _mapPcm Map to point pcm parameters
 */
#define PROCESS_APPPCM_CREATE(_name, _enum, _mapPcm) \
    tsPcmParams _name##Params =                      \
        {                                            \
            _mapPcm,                                 \
            ARRAY_SIZE(_mapPcm),                     \
    };                                               \
    PROCESS_CREATE(_name, pcmInit, pcmDeinit, _enum, &_name##Params, NULL)

/*********************************************************************
 *  INTERFACES
 */

#ifndef FILE_PCM_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/*********************************************************************
 *  INTERFACES: CONSTANTS
 */

/*********************************************************************
 *  INTERFACES: VARIABLES
 */

/*********************************************************************
 *  INTERFACES: FUNCTIONS
 */

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_PCM_H

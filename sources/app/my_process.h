/** @file       my_process.h
 *  @brief      MY_PROCESS application header file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     muhammed
 *  @date       2022-03-01
 */
#ifndef FILE_MY_PROCESS_H
#define FILE_MY_PROCESS_H

/** INCLUDES ******************************************************************/
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_MY_PROCESS_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   MY_PROCESS MY_PROCESS
 *  @ingroup    APP
 *  @brief      test devices
 *  @{
 */

/// @brief  Events of a MyProcess process
typedef enum
{
    eEvent1 = 2,
} teMyProcessEvents;

/// @brief  Parameters of a MyProcess process
typedef struct
{
    uint32_t device_data;
} tsMyProcessParams;

/// @brief  Constants of a MyProcess process
typedef struct
{
    tsDevIo *device;
} tsMyProcessConsts;

/** @brief  MyProcess process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_MY_PROCESS_CREATE(_name)    \
    tsMyProcessParams _name##Params =       \
        {                                   \
            .device_data = 100,             \
    };                                      \
    const tsMyProcessConsts _name##Consts = \
        {                                   \
            .device = NULL,                 \
    };                                      \
    PROCESS_CREATE(_name, myProcessInit, myProcessDeinit, 2, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_MY_PROCESS_H

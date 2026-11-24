/** @file       segger_debug_com.h
 *  @brief      Header file of SEGGER_DEBUG_COM device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Vehbi Umur Cabuk
 *  @date       18.03.2021
 */
#ifndef FILE_SEGGER_DEBUG_COM_H
#define FILE_SEGGER_DEBUG_COM_H

/// Includes
#include "rcos.h"
#include "SEGGER_RTT.h"
/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_SEGGER_DEBUG_COM_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   SEGGER_DEBUG_COM SEGGER_DEBUG_COM
 *  @ingroup    DEV_COM
 *  @brief      Segger RTT
 *  @{
 */

/// @brief  Functions for SEGGER_DEBUG_COM devices
INTERFACE const tsDevComFuncs devComSeggerDebugComFuncs;

/// @brief  Device specific parameters
typedef struct
{
    uint8_t _temp;
} tsSeggerDebugComParams;

/// @brief  Device specific constants
typedef struct
{
    uint8_t _temp;
} tsSeggerDebugComConsts;

/** @brief  Create a devCom
 *  @param  _name   Name of devCom object
 */
#define DEV_COM_SEGGER_DEBUG_COM_CREATE(_name)   \
    tsSeggerDebugComParams _name##Params =       \
        {                                        \
            ._temp = 0x00,                       \
    };                                           \
    const tsSeggerDebugComConsts _name##Consts = \
        {                                        \
            ._temp = 0x00,                       \
    };                                           \
    DEV_COM_CREATE(_name, devComSeggerDebugComFuncs, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_SEGGER_DEBUG_COM_H

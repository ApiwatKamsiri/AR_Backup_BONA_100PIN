/** @file       isr.h
 *  @brief      Header file of ISR device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Dilasude Göktürk
 *  @date       01.10.2022
 *  @author     Burak OZAN
 *  @date       08.11.2022
 */
#ifndef FILE_ISR_H
#define FILE_ISR_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_ISR_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   ISR ISR
 *  @ingroup    DEV_CPX
 *  @brief      ISR device for Renesas RA2L1
 *  @{
 */

/// @brief  Callback function type
typedef void (*isrCallbackFuncPtrType)(void);

/// @brief  Functions for ISR devices
INTERFACE const tsDevCpxFuncs devCpxIsrFuncs;

/// @brief  Device specific Isr function
void isrCallbackInterface(external_irq_callback_args_t *p_args, const tsDevCpx *device);

/// @brief  Device specific parameters
typedef struct
{
    isrCallbackFuncPtrType callbackFunc;   ///< Callback function pointer to call by device when interrupt occured
} tsIsrParams;

/// @brief  Device specific constants
typedef struct
{
    const external_irq_instance_t *isr; ///< ISR stack pointer
} tsIsrConsts;

/** @brief  Create a devCpx
 *  @param  _name           Name of devCpx object
 *  @param  _isr            Name of the ISR stack
 *  @param _isrCallbackName Name of the callback function calling by renesas driver
 */
#define DEV_CPX_ISR_CREATE(_name, _isr, _isrCallbackName)       \
    const tsDevCpx _name;                                       \
    void _isrCallbackName(external_irq_callback_args_t *p_args) \
    {                                                           \
        isrCallbackInterface(p_args, &_name);                   \
    }                                                           \
    tsIsrParams _name##Params =                                 \
        {                                                       \
            .callbackFunc = NULL,                               \
    };                                                          \
    const tsIsrConsts _name##Consts =                           \
        {                                                       \
            .isr = &_isr,                                       \
    };                                                          \
    DEV_CPX_CREATE(_name, devCpxIsrFuncs, &_name##Params, &_name##Consts)

/** @} */
#undef INTERFACE // Should not let this roam free

#endif // FILE_ISR_H

/** @file       isr.h
 *  @brief      Header file of ISR device
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     halil
 *  @date       7.03.2023
 */
#ifndef FILE_ISR_H
#define FILE_ISR_H

/// Includes
#include "rcos.h"
#include "sh32f9xx_sb0_exti.h"
#include "sh32f9xx_sb0_gpio.h"

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
 *  @brief      Interrupt
 *  @{
 */

/// PullUp PullDown Configuration
#define NOPULL GPIO_PuPd_NOPULL
#define PULLUP GPIO_PuPd_UP
#define PULLDOWN GPIO_PuPd_DOWN

/// EXTI Trigger Configuration
#define Rising EXTI_Trigger_Rising
#define Falling EXTI_Trigger_Falling
#define Rising_Falling EXTI_Trigger_Rising_Falling
#define HighLevel EXTI_Trigger_HighLevel
#define LowLevel EXTI_Trigger_LowLevel
#define HighLevel_LowLevel EXTI_Trigger_HighLevel_LowLevel

/// @brief  Callback function type
typedef void (*isrCallbackFuncPtrType)(void);

/// @brief  Functions for ISR devices
INTERFACE const tsDevCpxFuncs devCpxIsrFuncs;

/// @brief  Device specific Isr function
#define ISR_CALLBACK_FUNCTION(_name) void _name(void)

/// @brief  Device specific parameters
typedef struct
{
    uint32_t exti_line;
    EXTITrigger_TypeDef trigger_mode;
    EXTI_Port_Type exti_port;
    EXTI_PIN_Type exti_pinsource;
    isrCallbackFuncPtrType callbackFunc;
    uint8_t dummy;

} tsIsrParams;

/// @brief  Device specific constants
typedef struct
{
    GPIO_TypeDef *port;      ///< Pointer to the used port
    const uint16_t pin;      ///< Pin number on the port
    const GPIOPuPd_TypeDef pull_config; ///<  PullUp PullDown
    uint8_t dummy;

} tsIsrConsts;

/** @brief  Create a devCpx
 *  @param  _name   Name of devCpx object
 *  @param  _port   Name of GPIO Port (GPIOA, GPIOB, GPIOC, GPIOD, GPIOE)
 *  @param  _pin    Number of Pin (0, 1, .. ,15)
 *  @param  _pullConfig    PullUp PullDown Configuration (NOPULL, PULLUP, PULLDOWN)
 *  @param  _trigger_mode    EXTI Trigger Configuration (Rising, Falling, Rising_Falling, HighLevel, LowLevel, HighLevel_LowLevel)
 *  @param  _exti_isr        EXTI ISR Function (Pin 0-4 : EXTI0, EXTI1,.., EXTI4, Pin 5-9 : EXTI9_5, Pin 10-15 : EXTI15_10)
 *  @param  _callbackfunc    Callback function pointer to call by device when interrupt occured
 */
#define DEV_CPX_ISR_CREATE(_name, _port, _pin, _pullConfig, _trigger_mode, _exti_isr, _callbackfunc)  \
    const tsDevCpx _name;                       \
    ISR_CALLBACK_FUNCTION (_exti_isr##_Handler)\
        {                                       \
           IsrCallbackFunc(&_name);             \
        }                                       \
    tsIsrParams _name##Params  =                \
    {                                           \
        .exti_line = EXTI_Line##_pin,           \
        .trigger_mode = _trigger_mode,          \
        .exti_port = EXTI_Port_##_port,         \
        .exti_pinsource = EXTI_PinSource##_pin, \
        .callbackFunc = _callbackfunc,          \
    };                                          \
    const tsIsrConsts _name##Consts =           \
    {                                           \
        .port =_port,                           \
        .pin = GPIO_Pin_##_pin,                 \
        .pull_config = _pullConfig,             \
    };                                          \
    DEV_CPX_CREATE(_name, devCpxIsrFuncs, &_name##Params, &_name##Consts)
/** @} */
INTERFACE void IsrCallbackFunc(const tsDevCpx *device);
#undef INTERFACE // Should not let this roam free

#endif // FILE_ISR_H
/*
    ISR_CALLBACK_FUNCTION (EXTI15_10_Handler)   \
    {                                           \
        IsrCallbackFunc(&_name);                \
    }                                           \
    ISR_CALLBACK_FUNCTION (EXTI9_5_Handler)   \
    {                                           \
        IsrCallbackFunc(&_name);                \
    }                                           \
    ISR_CALLBACK_FUNCTION (EXTI4_Handler)   \
    {                                           \
        IsrCallbackFunc(&_name);                \
    }                                           \
    ISR_CALLBACK_FUNCTION (EXTI3_Handler)   \
    {                                           \
        IsrCallbackFunc(&_name);                \
    }                                           \
    ISR_CALLBACK_FUNCTION (EXTI2_Handler)   \
    {                                           \
        IsrCallbackFunc(&_name);                \
    }                                           \
    ISR_CALLBACK_FUNCTION (EXTI1_Handler)   \
    {                                           \
        IsrCallbackFunc(&_name);                \
    }                                           \
    ISR_CALLBACK_FUNCTION (EXTI0_Handler)   \
    {                                           \
        IsrCallbackFunc(&_name);                \
    }                                           \
*/
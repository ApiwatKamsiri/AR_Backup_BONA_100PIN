/** @file       gpio.h
 *  @brief      Header file of GPIO device
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     halil
 *  @date       25.01.2023
 */
#ifndef FILE_GPIO_H
#define FILE_GPIO_H

/// Includes
#include "rcos.h"
#include "sh32f9xx_sb0_gpio.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_GPIO_C
#define INTERFACE extern
#else
#define INTERFACE
#endif


/**
 *  @defgroup   GPIO GPIO
 *  @ingroup    DEV_IO
 *  @brief      gpio
 *  @{
 */

/// Pin mode macros
#define INPUT GPIO_Mode_IN
#define OUTPUT GPIO_Mode_OUT
/// Output Driver Configuration 
#define NORMAL GPIO_ODrv_NORMAL
#define WEAK GPIO_ODrv_WEAK
#define VERYWEAK GPIO_ODrv_VERYWEAK
#define VERYSTRONG GPIO_ODrv_VERYSTRONG
/// Output Type
#define PP GPIO_OType_PP
#define OD GPIO_OType_OD
/// PullUp PullDown Configuration
#define NOPULL GPIO_PuPd_NOPULL
#define PULLUP GPIO_PuPd_UP
#define PULLDOWN GPIO_PuPd_DOWN


/// @brief  Functions for GPIO devices
INTERFACE const tsDevIoFuncs devIoGpioFuncs;

// /// @brief  Device specific parameters
typedef struct
{
  uint8_t dummy;
} tsGpioParams;


/// @brief  Device specific constants
typedef struct
{
  GPIO_TypeDef *port;      ///< Pointer to the used port
  const uint16_t pin;      ///< Pin number on the port
  const GPIOMode_TypeDef pin_mode; ///< Pin mode INPUT, OUTPUT
  const GPIOODrv_TypeDef drive_mode; ///< Driving capability
  const GPIOOType_TypeDef output_type; ///< Output Type
  const GPIOPuPd_TypeDef pull_config; ///<  PullUp PullDown
  const uint8_t active;    ///< Activity level of the pin 0: low, 1: high
} tsGpioConsts;

/** @brief  Create a devIo
 *  @param  _name        Name of devIo object
 *  @param  _port        Name of the port (GPIOA, GPIOB, GPIOC, GPIOD, GPIOE)
 *  @param  _pin         Pin number (0 ~ 15)
 *  @param  _pin_mode    Pin mode (INPUT, OUTPUT)
 *  @param  _active      Active level for this GPIO(0:active low, 1:active high)
 *  @param  _drive_mode  Output Driver Configuration (NORMAL, WEAK, VERYWEAK, VERYSTRONG)
 *  @param  _output_type Output Type (PP : Push Pull, OD : Open Drain)
 *  @param  _pull_config PullUp PullDown Configuration (NOPULL, PULLUP, PULLDOWN)
 */


#define DEV_IO_GPIO_CREATE(_name, _port, _pin, _pin_mode, _active, _drive_mode, _output_type, _pull_config)      \
    tsGpioParams _name##Params;         \
    const tsGpioConsts _name##Consts =  \
    {                                   \
    .port = _port,                      \
    .pin = GPIO_Pin_##_pin,             \
    .pin_mode = _pin_mode,              \
    .active = _active,                  \
    .drive_mode =_drive_mode,           \
    .output_type =_output_type,         \
    .pull_config =_pull_config,         \
    };                                  \
    DEV_IO_CREATE(_name, devIoGpioFuncs, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_GPIO_H

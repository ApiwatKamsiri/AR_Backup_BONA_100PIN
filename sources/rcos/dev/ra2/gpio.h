/** @file       gpio.h
 *  @brief      Header file of GPIO device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       4/9/2021
 */
#ifndef FILE_GPIO_H
#define FILE_GPIO_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_GPIO_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   RA2_GPIO GPIO
 *  @ingroup    RA2_DEV_IO
 *  @brief      Renesas RA2 series MCU General Purpose I/O Device
 *  @{
 */

/// @brief  Functions for GPIO devices
INTERFACE const tsDevIoFuncs devIoGpioFuncs;

/// @brief  Device specific constants
typedef struct
{
    const ioport_port_pin_t pin; ///< Symbolic name of the pin
    const uint8_t active;        ///< Activity level of the pin (0: low, 1: high)
} tsGpioConsts;

/** @brief  Create a devIo
 *  @param  _name       Name of devIo object
 *  @param  _symbName   Symbolic name of the pin
 *  @param  _active     Activity level of the pin (0: low, 1: high)
 *  @attention          You need to define IO stack once using DEV_IO_GPIO_STACK before using this macro
 */
//ÖCE Warning elimination (ioport_port_pin_t) _symbName,  

#define DEV_IO_GPIO_CREATE(_name, _symbName, _active) \
    const tsGpioConsts _name##Consts =                \
        {                                             \
            (ioport_port_pin_t) _symbName,            \
            (_active) ? 1 : 0,                        \
    };                                                \
    DEV_IO_CREATE(_name, devIoGpioFuncs, NULL, &_name##Consts)

/** @brief  Define IO stack name
 *  @param  _name   Name of the IO stack
 */
#define DEV_IO_GPIO_STACK(_name) \
    ioport_ctrl_t *ioStack = &_name##_ctrl;

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_GPIO_H

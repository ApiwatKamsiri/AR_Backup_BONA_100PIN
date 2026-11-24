/** @file       gpio.c
 *  @brief      C file of GPIO device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       4/9/2021
 */
#define FILE_GPIO_C

#include "gpio.h"

/**
 *  @addtogroup GPIO
 *  @{
 */

static DEV_IO_FUNC_INIT(gpioInit);
static DEV_IO_FUNC_DEINIT(gpioDeinit);
static DEV_IO_FUNC_GET(gpioGet);
static DEV_IO_FUNC_PUT(gpioPut);

extern ioport_ctrl_t *ioStack;

/// @brief  Structure that defines the functions for gpio
const tsDevIoFuncs devIoGpioFuncs =
    {
        gpioInit,
        gpioDeinit,
        gpioGet,
        gpioPut,
};

/// @brief  Init function for a gpio
static DEV_IO_FUNC_INIT(gpioInit)
{
    ADD_RCOS_VERSION("ra2/gpio", "1.0.1")

    UNUSED(config);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a gpio
static DEV_IO_FUNC_DEINIT(gpioDeinit)
{
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

/// @brief  Get function for a gpio
static DEV_IO_FUNC_GET(gpioGet)
{
    const tsGpioConsts *consts = device->constants;
    bsp_io_level_t value;

    R_IOPORT_PinRead(ioStack, (bsp_io_port_pin_t)consts->pin, &value);

    if (consts->active)
    {
        return value;
    }
    else
    {
        return !value;
    }
}

/// @brief  Put function for a gpio
static DEV_IO_FUNC_PUT(gpioPut)
{
    const tsGpioConsts *consts = device->constants;

    if (data)
    {
        R_IOPORT_PinWrite(ioStack, (bsp_io_port_pin_t)consts->pin, (bsp_io_level_t)consts->active);
    }
    else
    {
        R_IOPORT_PinWrite(ioStack, (bsp_io_port_pin_t)consts->pin, (bsp_io_level_t)!consts->active);
    }

    return EXIT_SUCCESS;
}

/** @} */

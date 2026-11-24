/** @file       gpio.c
 *  @brief      C file of GPIO device
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     halil
 *  @date       25.01.2023
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
static void pinWrite(const tsGpioConsts *, uint32_t);


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
    ADD_RCOS_VERSION("sh32f9/gpio", "1.0.0")
    RCC_AHBPeriphClockOnOff(RCC_AHB_IOCLK, SH_ON);

    if(!device->sys->initialized)
    {
    device->sys->initialized = 1;

    //tsGpioParams *params       = device->parameters;
    const tsGpioConsts *consts = device->constants;

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = consts->pin;
    GPIO_InitStructure.GPIO_Mode  = consts->pin_mode;
    GPIO_InitStructure.GPIO_ODrv  = consts->drive_mode;
    GPIO_InitStructure.GPIO_OType = consts->output_type;
    GPIO_InitStructure.GPIO_PuPd  = consts->pull_config;

    GPIO_Init(consts->port, &GPIO_InitStructure);  
    GPIO_PinAFConfig(consts->port, consts->pin, GPIO_AF_GPIO);

    //UNUSED(params); // REMOVE IF USED
    //UNUSED(consts); // REMOVE IF USED
    }
    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a gpio
static DEV_IO_FUNC_DEINIT(gpioDeinit)
{
    //tsGpioParams *params       = device->parameters;
    const tsGpioConsts *consts = device->constants;

    /// TODO : add deinit func 

    device->sys->initialized = 0;
    //UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    return EXIT_SUCCESS;
}

/// @brief  Get function for a gpio
static DEV_IO_FUNC_GET(gpioGet)
{
    //tsGpioParams *params       = device->parameters;
    const tsGpioConsts *consts = device->constants;
    uint32_t value = (GPIO_ReadInputDataBit(consts->port, consts->pin));
    //value1 = (GPIO_ReadInputData(GPIOD));
    
   if(consts->active)
   {
    return value;
   }
   else
   {
    return !value;
   }

    //UNUSED(params); // REMOVE IF USED
    //UNUSED(consts); // REMOVE IF USED
}

/// @brief  Put function for a gpio
static DEV_IO_FUNC_PUT(gpioPut)
{
    //tsGpioParams *params       = device->parameters;
    const tsGpioConsts *consts = device->constants;
   
    if (data)
    {
        pinWrite(consts, consts->active);
    }
    else
    {
        pinWrite(consts, !consts->active);
    }

    //UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    //UNUSED(data);   // REMOVE IF USED

    return EXIT_SUCCESS;
}

static void pinWrite(const tsGpioConsts *consts, uint32_t value)
{
    if (value)
    {
       GPIO_SetBits(consts->port,consts->pin); 
    }
    else
    {
      GPIO_ResetBits(consts->port,consts->pin);
    }
}
/** @} */

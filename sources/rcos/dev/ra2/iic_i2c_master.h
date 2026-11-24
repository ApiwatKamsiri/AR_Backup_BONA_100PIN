/** @file       iic_i2c_master.h
 *  @brief      Header file of I2C device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Enes Inanc
 *  @date       20.10.2021
 */
#ifndef FILE_I2C_H
#define FILE_I2C_H

/// Includes
#include "rcos.h"
#include "r_iic_master.h"
#include "r_i2c_master_api.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_I2C_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   I2C I2C
 *  @ingroup    DEV_COM
 *  @brief      ra2 i2c com device
 *  @{
 */

/// @brief  Functions for I2C devices
INTERFACE const tsDevComFuncs devComI2cFuncs;

/// @brief  Device specific parameters
typedef struct
{
    iic_master_instance_ctrl_t *i2c_master_ctrl;
    const i2c_master_cfg_t *i2c_master_cfg;
    volatile uint8_t flag;
} tsI2cParams;

/// @brief  Device specific constants
typedef struct
{
    i2c_master_api_t const *i2c_master_on_iic;
} tsI2cConsts;

/** @brief  Create a devCom
 *  @param  _name   Name of devCom object
 *  @param  _ra2_i2c_name I2C module name must be defined in e2studio
 *  @param  _i2cCallback Callback function name must be defined in e2studio
 */
#define DEV_COM_I2C_CREATE(_name, _ra2_i2c_name, _i2cCallback) \
    const tsDevCom _name;                                      \
    tsI2cParams _name##Params =                                \
        {                                                      \
            .i2c_master_ctrl = &_ra2_i2c_name##_ctrl,          \
            .i2c_master_cfg = &_ra2_i2c_name##_cfg,            \
            .flag = 0,                                         \
    };                                                         \
    const tsI2cConsts _name##Consts =                          \
        {                                                      \
            .i2c_master_on_iic = &g_i2c_master_on_iic,         \
    };                                                         \
    void _i2cCallback(i2c_master_callback_args_t *p_args)      \
    {                                                          \
        i2c_Callback(p_args, &_name);                          \
    }                                                          \
    DEV_COM_CREATE(_name, devComI2cFuncs, &_name##Params, &_name##Consts)

INTERFACE void i2c_Callback(i2c_master_callback_args_t *p_args, const tsDevCom *device);

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_I2C_H

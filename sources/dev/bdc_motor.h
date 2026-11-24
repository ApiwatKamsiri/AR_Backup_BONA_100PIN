/** @file       bdc_motor.h
 *  @brief      Header file of BDC_MOTOR device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-02
 *
 *  @details
 * The purpose of this device is to control a brushed DC motor (stop, rotate cw, rotate cww) with a simple interface
 * This device implements a dead time used before changing the direction of motor or re-energizing it. It's useful to protect the motor.
 *
 * @code
 * #include "dev/ke14/gpio.h"
 * DEV_IO_GPIO_CREATE(gpioMotorCW, UC_MOTOR_P, 0, 0, 1)
 * DEV_IO_GPIO_CREATE(gpioMotorCCW, UC_MOTOR_N, 0, 0, 1)
 *
 * #include "dev/bdc_motor.h"
 * DEV_IO_BDC_MOTOR_CREATE(bdcMotor, gpioMotorCW, gpioMotorCCW, 500)
 *
 * // initialization ...
 * devIoInit(&bdcMotor, NULL);
 *
 * 
 * // usage ....
 * devIoPut(&bdcMotor, eBDCMotorRotateCW);
 * 
 * 
 * devIoPut(&bdcMotor, eBDCMotorRotateCCW); // here the motor will stop, wait for 500 ms then put 1 to gpioMotorCCW device
 * @endcode
 */
#ifndef FILE_BDC_MOTOR_H
#define FILE_BDC_MOTOR_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_BDC_MOTOR_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   BDC_MOTOR BDC_MOTOR
 *  @ingroup    DEV_IO
 *  @brief      Controling brushed DC motors
 *  @{
 */

/// @brief  Functions for BDC_MOTOR devices
INTERFACE const tsDevIoFuncs devIoBdcMotorFuncs;
TIMER_CALLBACK_FUNC(bdcMotorDeadTimerHandler);

typedef enum
{
    eBDCMotorStop = 0,
    eBDCMotorRotateCW = 1,
    eBDCMotorRotateCCW = 2,
} teBDCMotorStates;

/// @brief  Device specific parameters
typedef struct
{
    teBDCMotorStates currentState : 2;
    teBDCMotorStates requestedState : 2;
    bool isDead : 1;
    tsTimerCallBack deadTimer; ///< timer used to re-enable the motor after a dead time
} tsBdcMotorParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevIo *cwDev, *ccwDev; ///< ptr to the device that is used to turn the motor cw/ccw by writing 1 to it
    const uint32_t deadTimeMs;     ///< the dead time to wait before changing the direction of motor or re-energizing it
} tsBdcMotorConsts;

/** @brief  Create a bdc motor controller device
 *  @param  _name   Name of devIo object
 *  @param _cwDev the device that is used to turn the motor cw by writing 1 to it
 *  @param _ccwDev the device that is used to turn the motor ccw by writing 1 to it
 *  @param _deadTimeMs the dead time to wait before changing the direction of motor or re-energizing it
 *  @note _cwDev & _ccwDev are initialized in the devIoInit function of this device, no need to initialize them before
 */
#define DEV_IO_BDC_MOTOR_CREATE(_name, _cwDev, _ccwDev, _deadTimeMs)            \
    const tsDevIo _name;                                                        \
    tsBdcMotorParams _name##Params =                                            \
        {                                                                       \
            .currentState = eBDCMotorStop,                                      \
            .requestedState = eBDCMotorStop,                                    \
            .deadTimer = TIMER_CALLBACK_INIT(bdcMotorDeadTimerHandler, &_name), \
            .isDead = false,                                                    \
    };                                                                          \
    const tsBdcMotorConsts _name##Consts =                                      \
        {                                                                       \
            .cwDev = &_cwDev,                                                   \
            .ccwDev = &_ccwDev,                                                 \
            .deadTimeMs = _deadTimeMs,                                          \
    };                                                                          \
    DEV_IO_CREATE(_name, devIoBdcMotorFuncs, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_BDC_MOTOR_H

/** @file       bdc_motor.c
 *  @brief      C file of BDC_MOTOR device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-02
 */
#define FILE_BDC_MOTOR_C

#include "bdc_motor.h"

/**
 *  @addtogroup BDC_MOTOR
 *  @{
 */

static DEV_IO_FUNC_INIT(bdcMotorInit);
static DEV_IO_FUNC_DEINIT(bdcMotorDeinit);
static DEV_IO_FUNC_GET(bdcMotorGet);
static DEV_IO_FUNC_PUT(bdcMotorPut);
static void driveMotor(const tsDevIo *device, teBDCMotorStates state);

/// @brief  Structure that defines the functions for bdcMotor
const tsDevIoFuncs devIoBdcMotorFuncs =
    {
        bdcMotorInit,
        bdcMotorDeinit,
        bdcMotorGet,
        bdcMotorPut,
};

/// @brief  Init function for a bdcMotor
static DEV_IO_FUNC_INIT(bdcMotorInit)
{
    tsBdcMotorParams *params = device->parameters;
    const tsBdcMotorConsts *consts = device->constants;

    UNUSED(params); // REMOVE IF USED

    devIoInit(consts->cwDev, NULL);
    devIoInit(consts->ccwDev, NULL);

    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a bdcMotor
static DEV_IO_FUNC_DEINIT(bdcMotorDeinit)
{
    tsBdcMotorParams *params = device->parameters;
    const tsBdcMotorConsts *consts = device->constants;

    device->sys->initialized = 0;
    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    devIoDeinit(consts->cwDev);
    devIoDeinit(consts->ccwDev);
    timerCallBackStop(&params->deadTimer);

    return EXIT_SUCCESS;
}

/// @brief  Get function for a bdcMotor
/// @return the current motor state from teBDCMotorStates enum
static DEV_IO_FUNC_GET(bdcMotorGet)
{
    tsBdcMotorParams *params = device->parameters;
    const tsBdcMotorConsts *consts = device->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    return params->currentState;    // or requested state ??
}

/// @brief  Put function for a bdcMotor
/// @param data the new required motor state from teBDCMotorStates enum
static DEV_IO_FUNC_PUT(bdcMotorPut)
{
    tsBdcMotorParams *params = device->parameters;
    const tsBdcMotorConsts *consts = device->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    UNUSED(data);   // REMOVE IF USED
    
    if (data > eBDCMotorRotateCCW)
        return EXIT_FAILURE;

    params->requestedState = (teBDCMotorStates)data;
    if (params->requestedState != params->currentState)
    {
        // requested state can be the opposite direction or stop
        if (params->currentState != eBDCMotorStop)     // the motor is running at the current time
            driveMotor(device, eBDCMotorStop);
        else if (params->isDead == false)           // the motor is stopped for along time
            driveMotor(device, params->requestedState);
        // else the motor is stopped for a short time (less than dead time) 
        //-> do nothing, when dead time is elapsed callback timer will update the position
    }
    return EXIT_SUCCESS;
}

/// @brief this function is called after a dead time has elapsed after stopping the motor
TIMER_CALLBACK_FUNC(bdcMotorDeadTimerHandler)
{
    tsDevIo *device = (tsDevIo*) parameter;
    tsBdcMotorParams *params = device->parameters;

    params->isDead = false;
    // requested state may be changed during dead time to stop for example, so don't update then
    if (params->requestedState != eBDCMotorStop)
    {
        driveMotor(device, params->requestedState);
    }
    return 0;
}

/// @brief drives the poles of the motor according to the given state
static void driveMotor(const tsDevIo *device, teBDCMotorStates state)
{
    tsBdcMotorParams *params = device->parameters;
    const tsBdcMotorConsts *consts = device->constants;

    switch (state)
    {
    case eBDCMotorStop:
        devIoPut(consts->cwDev, 0);
        devIoPut(consts->ccwDev, 0);
        params->isDead = true;
        timerCallBackStart(&params->deadTimer, consts->deadTimeMs);
        break;

    case eBDCMotorRotateCW:
        devIoPut(consts->ccwDev, 0);
        devIoPut(consts->cwDev, 1);
        break;

    case eBDCMotorRotateCCW:
        devIoPut(consts->cwDev, 0);
        devIoPut(consts->ccwDev, 1);
        break;
    }

    params->currentState = state;
}

/** @} */

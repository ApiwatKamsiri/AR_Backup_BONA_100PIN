/** @file       stepper_motor.h
 *  @brief      Header file of STEPPER_MOTOR device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-06
 *
 *  @details
 *  This device is used to drive stepper (step) motors using 4 GPIO pins
 *  The stepper motor can be driven in three methods: Full step, Half step, and Wave drive.
 *  The device can be configured to do calibration at many events:
 *  1- At initializing the device,
 *  2- After specific period of hours,
 *  3- After specific count of movements.
 *  Driving method and calibration settings are passed to the device as configurations with devIoInit func.
 *
 *  The device implements a blocking time used before changing the direction of motor or re-energizing it.
 *  It's useful to protect the motor.
 *  
 *  The position of the motor after initializing it is 0. Any given position value with devIoPut function
 *  causes the motor to run in CW direction.
 *  Position value represents the number of steps away from the initial position (0)
 *
 *  FMI about driving stepper motors see: https://www.ti.com/lit/an/slva767a/slva767a.pdf
 *    
 *  @code
 *  // Creation ...
 *  #include "dev/ke14/gpio.h"
 *  DEV_IO_GPIO_CREATE(motorAp, UC_SV_A_P, 0, 0, 1)
 *  DEV_IO_GPIO_CREATE(motorAn, UC_SV_A_N, 0, 0, 1)
 *  DEV_IO_GPIO_CREATE(motorBp, UC_SV_B_P, 0, 0, 1)
 *  DEV_IO_GPIO_CREATE(motorBn, UC_SV_B_N, 0, 0, 1)
 *
 *  #include "dev/stepper_motor.h"
 *  DEV_IO_STEPPER_MOTOR_CREATE(stepperMotor, 5, motorAp, motorAn, motorBp, motorBn)
 *
 *  // Initialization ...
 *  const static tsStepperMotorConfig motorConfig =
 *  {
 *      .drivingMethod = eStepperMotorDrivingMethodWaveDrive,
 *      .calibrateAtInit = true,
 *      .calibrationPosition = 300,
 *      .calibrationTimeH = 1,
 *      .movementsCountForCalibration = 200,
 *  };
 *
 *  devIoInit(&stepperMotor, &motorConfig);
 *
 *  // Usage ...
 *  devIoPut(&stepperMotor, 100);   // This will drive the motor 100 steps in CW direction to go to position 100
 *  devIoPut(&stepperMotor, 200);   // This will drive the motor another 100 steps in CW direction to go to position 200
 *  devIoPut(&stepperMotor, 100);   // This will drive the motor 100 steps in CCW direction to return to position 100
 *  devIoPut(&stepperMotor, 0);     // This will drive the motor 100 steps in CCW direction to return to position 0
 *
 *  @endcode
 */
#ifndef FILE_STEPPER_MOTOR_H
#define FILE_STEPPER_MOTOR_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_STEPPER_MOTOR_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   STEPPER_MOTOR STEPPER_MOTOR
 *  @ingroup    DEV_IO
 *  @brief      Driving stepper motors
 *  @{
 */

typedef enum
{
    eStepperMotorDrivingMethodFullStep = 0,
    eStepperMotorDrivingMethodHalfStep,
    eStepperMotorDrivingMethodWaveDrive,
    eStepperMotorDrivingMethodsCount,
} teStepperMotorDrivingMethods;

/// @brief  Functions for STEPPER_MOTOR devices
INTERFACE const tsDevIoFuncs devIoStepperMotorFuncs;

/// @brief  Device specific configuration
typedef struct
{
    teStepperMotorDrivingMethods drivingMethod : 3;
    bool calibrateAtInit : 1;
    uint16_t calibrationPosition;
    uint8_t calibrationTimeH;
    uint8_t movementsCountForCalibration;
} tsStepperMotorConfig;

/// @brief  Device specific parameters
typedef struct
{
    const tsStepperMotorConfig *configs; ///< pointer to the configuration struct
    uint16_t currentPosition;            ///< current motor position
    uint16_t requiredPosition;           ///< required motor position
    uint8_t currentStep : 3;             ///< the current applied step number of the driving method
    uint8_t currentDirection : 2;        ///< stop, CW or CCW
    bool calibrationNeeded : 1;          ///< this flag is cleared only when calibration operation is finished,
                                         ///< so setting it during the calibration operation doesn't trigger the calibration process again.
    bool calibrationOngoing : 1;         ///< flag indicates that calibration process is ongoing
    bool blocked : 1;                    ///< flag indicates that the motor is blocked to protect it
    uint8_t movementsCounter;            ///< when this counter reaches movementsCountForCalibration, calibration is triggered
    tsTimerCallBack calibrationTimer;    ///< when this timer is elapsed, calibration is triggered
    tsTimerCallBack unblockingTimer;     ///< when this timer is elapsed, the motor is unblocked
} tsStepperMotorParams;

/// @brief  Device specific constants
typedef struct
{
    uint32_t periodBetweenStepsMs; ///< time to wait before applying the next step in milliseconds
    tsProcess *process;            ///< Process that handles the jobs for this device
    const tsDevIo *poleDevices[4]; ///< array of ptrs to poles gpio devices
} tsStepperMotorConsts;

/** @brief  Create a stepper motor device
 *  @param  _name   Name of devIo object
 *  @param __periodBetweenStepsMs Time to wait before applying the next step in milliseconds
 *  @param _poleApDevice Pole A+ GPIO device
 *  @param _poleAnDevice Pole A- GPIO device
 *  @param _poleBpDevice Pole B+ GPIO device
 *  @param _poleBnDevice Pole A- GPIO device
 */
#define DEV_IO_STEPPER_MOTOR_CREATE(_name, _periodBetweenStepsMs, _poleApDevice, _poleAnDevice, _poleBpDevice, _poleBnDevice)        \
    tsProcess _name##Process;                                                                                                        \
    const tsDevIo _name;                                                                                                             \
    tsStepperMotorParams _name##Params =                                                                                             \
        {                                                                                                                            \
            .calibrationTimer = TIMER_CALLBACK_INIT(NULL, &_name),                                                                   \
            .unblockingTimer = TIMER_CALLBACK_INIT(NULL, &_name),                                                                    \
    };                                                                                                                               \
    const tsStepperMotorConsts _name##Consts =                                                                                       \
        {                                                                                                                            \
            .periodBetweenStepsMs = _periodBetweenStepsMs,                                                                           \
            .process = &_name##Process,                                                                                              \
            .poleDevices = {                                                                                                         \
                [0] = &_poleApDevice,                                                                                                \
                [1] = &_poleAnDevice,                                                                                                \
                [2] = &_poleBpDevice,                                                                                                \
                [3] = &_poleBnDevice,                                                                                                \
            },                                                                                                                       \
    };                                                                                                                               \
    PROCESS_CREATE(_name##Process, stepperMotorProcessInit, stepperMotorProcessDeinit, PROCESS_NONE, &_name##Params, &_name##Consts) \
    DEV_IO_CREATE(_name, devIoStepperMotorFuncs, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_STEPPER_MOTOR_H

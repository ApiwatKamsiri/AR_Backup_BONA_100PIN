/** @file       fan.h
 *  @brief      Header file of FAN device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-04
 *  @details
 * 
 *  This device is used to control fans in many methods according to the given configuration
 *  at the initialization. 
 * 
 * 
 *  In creation macro the device accepts 4 devices:
 *  1- PWM device: A PWM duty device used when the fan is configured to be controlled with PWM.
 *  2- Switching device: A GPIO output device used when the fan is configured to be controlled
 *                       with a constant DC signal only (on/off).
 *  3- Power device: A GPIO output device used when the fan is configured to use auxiliary enabling
 *                   / powering signal (4-wire fan).  
 *  4- FG device: A GPIO input device used when the fan provides feedback (tach) signal, it indicates
 *                  whether the fan is running and its rate of speed.
 * 
 * 
 *  PWM controlled fans have 2 control stages:
 *  1- Startup stage: PWM duty cycle is increased to 70% by 5% steps to give the motor a ramp
 *  2- Running stage: * If the fan speed cannot be calculated (FG pin is not used) : The PWM 
 *                      duty cycle is increases or decreased by 1% steps to meet the required
 *                      PWM value
 *                    * If the fan speed can be calculated (FG pin is used) : The PWM duty cycle
 *                      is increased or decreased by 1% steps until the current fan speed meets 
 *                      the required fan speed (which is the gotten ratio from put func. * fan RPM)
 *  If a fan with FG pin is detected to not being turning, the fan falls into failure state where
 *  the fan is locked (stopped) for a specific time, and error detector device returns 1. 
 *
 *  @note [1-5, 96-99] PWM values shouldn't be applied on the fan, so they are filtered in the code.
 * 
 *  @code
 *  // Creation ...
 *  #define BOARD_INITPINS_UC_FF_FAN_GPIO GPIOB
 *  #include "dev/ke14/gpio.h"
 *  DEV_IO_GPIO_CREATE(fanPower, UC_FF_FAN_POWER, 0, 0, 1)
 *  DEV_IO_GPIO_CREATE(fanDC, UC_FF_FAN, 0, 0, 1)
 *  DEV_IO_GPIO_CREATE(fanFg, UC_FF_FG, 1, 0, 1)
 *
 *  #include "dev/ke14/pwm.h"
 *  DEV_CPX_PWM_CREATE(pwmCore, FTM0, 15784, kFTM_Prescale_Divide_1, PWM_CHNL(4, 1, 0, 0))
 *  DEV_IO_PWM_CREATE(fanDuty, pwmCore, 0)

 *  #include "dev/fan.h"
 *  DEV_IO_FAN_CREATE(fan, fanDuty, fanDC, fanPower, fanFg)
 *  DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanError, fan)


 *  // Initialization ...
 *  static const stFanConfig fanConfig = {.fgPinUsed = true, .powerPinUsed = false, .pwmUsed = true, .rpm = 2100, .ppr = 2};
 *  .
 *  .
 *  devCpxInit(&pwmCore, NULL);
 *  devIoInit(&fan, &fanConfig);

 *  // Usage ...
 *  devIoPut(&fan, 80);
 *  bool error = devIoGet(&fan);
 *  @endcode
 */
#ifndef FILE_FAN_H
#define FILE_FAN_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_FAN_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   FAN FAN
 *  @ingroup    DEV_IO
 *  @brief      Controlling DC fans
 *  @{
 */

/// @brief  Functions for FAN devices
INTERFACE const tsDevIoFuncs devIoFanFuncs;

/// @brief  Controlled fan specific configuration
typedef struct
{
    bool pwmUsed : 1;      ///< true: fan controlled by PWM signal, false: fan controlled by DC signal
    bool powerPinUsed : 1; ///< true: fan has power pin
    bool fgPinUsed : 1;    ///< Fan has FG pin and its actual speed can be calculated
    uint8_t ppr;           ///< Pulses per Revolution: Number of pulses the fan generates on FG pin for each revolution
    uint16_t rpm;          ///< Revolution per Minute: Fan speed
} tsFanConfig;

/// @brief  Device specific parameters
typedef struct
{
    const tsFanConfig *configs;         ///< pointer to the configuration struct
    uint8_t currentPwm;                 ///< Holds the current applied PWM duty cycle
    uint8_t requiredPwm;                ///< Holds the requested PWM duty cycle, or the requested speed ratio
    bool failureDetected : 1;           ///< Flag indicates if the fan isn't rotating when controlled signals are applied
    bool failureStatus: 1;           ///<
    bool failureShow: 1;           ///<
    bool speedCalculationActivated : 1; ///< Flag indicates the fan speed is currently measured using FG pin
    bool windowOfPulsesReady : 1;       ///< Flag indicates that a window of FG pulses are captured and ready to be used to calculate the current speed
    bool fanLocked : 1;                 ///< Flag indicates that the fan is locked for a specific time (no signal is applied) because a failure is detected
    uint8_t lastFgLevel : 1;            ///< Last read FG pin level

    uint16_t passedMsWithNoPulses;               ///< Count of milliseconds with no new FG pulse is detected
    uint16_t windowMsCounter;                   ///< Milliseconds counter for FG pulses capturing window
    uint16_t fgPulsesCounter;                   ///< Count of FG pulses in the currently capturing window
    uint16_t fgPulsesCountInWindow;             ///< Count of FG pulses in the last captured window
    uint16_t currentSpeedRpm;                   ///< The calculated fan speed
    tsTimerEvent periodicTimer;                 ///< Timer posts periodic event
    tsTimerEvent activateSpeedCalculationTimer; ///< Timer used to delay activating speed calculation
    tsTimerCallBack fgPulsesCountingTimer;      ///< Timer used to call a function each 1 ms to capture FG pulses
    tsTimerCallBack fanUnlockTimer;             ///< Timer used to unlock the fan after a failure is detected
    tsTimerCallBack fanErrorHideTimer;          ///< Timer used to show error time after
    tsTimerCallBack fanErrorCheckTimer;         ///< Timer used to check error
} tsFanParams;

/// @brief  Fan device specific constants
typedef struct
{
    const tsDevIo *pwmDev;       ///< Device to control the fan by applying a PWM signal (PWM duty)
    const tsDevIo *switchingDev; ///< Device to control the fan by applying a constant DC signal (GPIO output)
    const tsDevIo *powerDev;     ///< Device to control the power signal of the fan (GPIO output)
    const tsDevIo *fgDev;        ///< Device to read the FG signal of the fan (GPIO input)
    tsProcess *process;          ///< Process that handles the jobs for this device
} tsFanConsts;

/** @brief  Create a devIo for fan device
 *  @param  _name           Name of devIo object
 *  @param _pwmDev          Device to control the fan by applying a PWM signal (PWM duty)
 *  @param _switchingDev    Device to control the fan by applying a constant DC signal (GPIO output)
 *  @param _powerDev        Device to control the power signal of the fan (GPIO output)
 *  @param _fgDev           Device to read the FG signal of the fan (GPIO input)
 */
#define DEV_IO_FAN_CREATE(_name, _pwmDev, _switchingDev, _powerDev, _fgDev)                                        \
    tsProcess _name##Process;                                                                                      \
    const tsDevIo _name;                                                                                           \
    tsFanParams _name##Params =                                                                                    \
        {                                                                                                          \
            .fanUnlockTimer = TIMER_CALLBACK_INIT(NULL, &_name),                                                   \
            .fgPulsesCountingTimer = TIMER_CALLBACK_INIT(NULL, &_name),                                            \
            .fanErrorHideTimer = TIMER_CALLBACK_INIT(NULL, &_name),                                                \
            .fanErrorCheckTimer = TIMER_CALLBACK_INIT(NULL, &_name),                                                \
            .lastFgLevel = 0,                                                                                      \
    };                                                                                                             \
    const tsFanConsts _name##Consts =                                                                              \
        {                                                                                                          \
            .pwmDev = &_pwmDev,                                                                                    \
            .switchingDev = &_switchingDev,                                                                        \
            .powerDev = &_powerDev,                                                                                \
            .fgDev = &_fgDev,                                                                                      \
            .process = &_name##Process,                                                                            \
    };                                                                                                             \
    PROCESS_CREATE(_name##Process, fanProcessInit, fanProcessDeinit, PROCESS_NONE, &_name##Params, &_name##Consts) \
    DEV_IO_CREATE(_name, devIoFanFuncs, &_name##Params, &_name##Consts)

INTERFACE const tsDevIoFuncs devIoFanErrorDetectorFuncs;

/// @brief  Fan error detector device specific constants
typedef struct
{
    const tsDevIo *fanDevice; ///< The fan device that the error detector is associated to it
} tsFanErrorDetectorConsts;

/** @brief  Create a devIo for fan error detector device
 *  @param  _name       Name of devIo object
 *  @param _fanDevice   The fan device that the error detector is associated to it
 */
#define DEV_IO_FAN_ERROR_DETECTOR_CREATE(_name, _fanDevice) \
    const tsFanErrorDetectorConsts _name##Consts =          \
        {                                                   \
            .fanDevice = &_fanDevice,                       \
    };                                                      \
    DEV_IO_CREATE(_name, devIoFanErrorDetectorFuncs, NULL, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_FAN_H

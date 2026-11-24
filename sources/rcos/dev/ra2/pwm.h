/** @file       pwm.h
 *  @brief      Header file of PWM device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       4/12/2021
 *  @author     Burak OZAN
 *  @date       09/09/2022
 */
#ifndef FILE_PWM_H
#define FILE_PWM_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_PWM_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   RA2_PWM PWM
 *  @ingroup    RA2_DEV_IO
 *  @brief      Pulse Width Modulation device driver for Renesas RA2
 *  @{
 */

/// @brief  Functions for PWM devices
INTERFACE const tsDevCpxFuncs devCpxPwmCoreFuncs; ///< Functions for PWM core devices

/// @brief  Device specific constants
typedef struct
{
    const timer_instance_t *timer; ///< Timer instance
    const gpt_io_pin_t pin;    ///< Timer pin to output the waveform
} tsPwmCoreConsts;

/// @brief  Device specific parameters
typedef struct
{
    uint16_t freq; ///< Configured frequency
    uint8_t duty;  ///< Configured duty cycle
} tsPwmCoreParams;

/** @brief  Create a PWM core devCpx
 *  @param  _name   Name of devCPX object
 *  @param  _timer  Name of the r_agt timer instance
 *  @param  _pin    Pin to output the waveforem (A or B)
 *  @param  _freq   PWM Frequency
 *  @param  _duty   PWM Duty (0 ... 100)
 */
#define DEV_CPX_PWM_CORE_CREATE(_name, _timer, _pin, _freq, _duty) \
    const tsPwmCoreConsts _name##Consts =                          \
        {                                                          \
            &_timer,                                               \
            GPT_IO_PIN_GTIOC##_pin,                             \
    };                                                             \
    tsPwmCoreParams _name##Params =                                \
        {                                                          \
            _freq,                                                 \
            _duty,                                                 \
    };                                                             \
    DEV_CPX_CREATE(_name, devCpxPwmCoreFuncs, &_name##Params, &_name##Consts)

INTERFACE const tsDevIoFuncs devIoPwmDutyFuncs; ///< Functions for PWM Duty devices

/** @brief  Create a PWM_DUTY devIO
 *  @param  _name   Name of devIO object
 *  @param  _devCpx Target devCpx that is a PWM_CORE
 */
#define DEV_IO_PWM_DUTY_CREATE(_name, _devCpx) \
    DEV_IO_CREATE(_name, devIoPwmDutyFuncs, &_devCpx##Params, &_devCpx##Consts)

INTERFACE const tsDevIoFuncs devIoPwmFreqFuncs; ///< Functions for PWM Frequency devices

/** @brief  Create a PWM_FREQ devIO
 *  @param  _name   Name of devIO object
 *  @param  _devCpx Target devCpx that is a PWM_CORE
 */
#define DEV_IO_PWM_FREQ_CREATE(_name, _devCpx) \
    DEV_IO_CREATE(_name, devIoPwmFreqFuncs, &_devCpx##Params, &_devCpx##Consts)

#undef INTERFACE // Should not let this roam free

#endif // FILE_PWM_H

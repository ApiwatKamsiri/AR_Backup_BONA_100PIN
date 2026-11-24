/** @file       buzzer.h
 *  @brief      Header file of BUZZER device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-10
 *
 * @details
 * This module is used to play pre-defined melodies with a buzzer circuit that has two inputs: PWM & High
 * PWM input is used to play a specific note/tone by changing the applied frequency
 * High input is used to power/release the buzzer. Note that this pin is connected to a charging circuit,
 * which supply some power to the buzzer for a short time even if High pin is low. This gives echo effect.
 *
 * If echo effect is desired, high pin should be at low level, and PWM pin should continue to supply a PWM signal
 *
 * The supported melodies are presented in teBuzzerMelodies enum. The supported melodies data are at the top of the .c file
 *
 * The device is implemented using the builtin devices pattern & pulse
 * pattern device is used with pwm device to generate sequential different notes with different periods
 * pulse device is used with power (high) device to charge the buzzer circuit for a specific period only
 * 
 * @code
 *
 * // Creation
 * #include "dev/ke14/gpio.h"
 * DEV_IO_GPIO_CREATE(buzzerHigh, UC_BUZZER_HIGH, 0, 0, 1)

 * #include "dev/ke14/pwm.h"
 * #include "dev/ke14/fmo.h"
 * DEV_CPX_PWM_CREATE(pwmCore, FTM2, 1000, kFTM_Prescale_Divide_1, PWM_CHNL(0, 0, 50, 0))
 * DEV_IO_PWM_CREATE(buzzerDuty, pwmCore, 0)
 * DEV_IO_FMO_CREATE(buzzerFreq, 1000, 50, &pwmCore, &buzzerDuty)
 *
 * 
 * #include "dev/buzzer.h"
 * DEV_IO_BUZZER_CREATE(buzzer, buzzerHigh, buzzerFreq)
 * 
 * 
 * // Initialization ....
 *  devCpxInit(&pwmCore, NULL);
 *  devIoInit(&buzzer, NULL);
 *
 *
 * // Usage ....
 * devIoPut(&buzzer, eBuzzerMelodyStart);
 * 
 * @endcode
 */

#ifndef FILE_BUZZER_H
#define FILE_BUZZER_H

/// Includes
#include "rcos.h"
#include "dev/pulse.h"
#include "dev/pattern.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_BUZZER_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   BUZZER BUZZER
 *  @ingroup    DEV_IO
 *  @brief      Driving main board buzzer with simple IO interface
 *  @{
 */

/// @brief  Functions for BUZZER devices
INTERFACE const tsDevIoFuncs devIoBuzzerFuncs;

typedef enum
{
    eBuzzerMelodyNone = 0,
    eBuzzerMelodyOpen,
    eBuzzerMelodyClose,
    eBuzzerMelodyStart,
    eBuzzerMelodyEnd,
    eBuzzerMelodyButton,
    eBuzzerMelodyWarning,
    eBuzzerMelodyDoorsAlertAHTH,        //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
    eBuzzerMelodiesCount,
} teBuzzerMelodies; ///< supported melodies, pass them to buzzer device with put function to play them

/// @brief  Device specific constants
typedef struct
{
    const tsDevIo *pulseDevice; ///< ptr to the created pulse device that controls powerDevice
    const tsDevIo *patternDevice; ///< ptr to the created pattern device that controls freqDevice
} tsBuzzerConsts;

/** @brief  Create a buzzer devIo
 *  @param  _name   Name of the buzzer object
 *  @param _powerDevice the gpio device that is used to supply High signal / charge buzzer circuit. FMI see above.
 *  @param _freqDevice the fmo device that is used to generate a PWM signal
 *  @note _powerDevice & _freqDevice are initialized in the devIoInit function of this device, no need to initialize them before
 */
#define DEV_IO_BUZZER_CREATE(_name, _powerDevice, _freqDevice)  \
    DEV_IO_PULSE_CREATE(_name##PulseDevice, _powerDevice, 1, 0) \
    DEV_IO_PATTERN_CREATE(_name##PatternDevice, _freqDevice, 0) \
    const tsBuzzerConsts _name##Consts =                        \
        {                                                       \
            .pulseDevice = &_name##PulseDevice,                 \
            .patternDevice = &_name##PatternDevice,             \
    };                                                          \
    DEV_IO_CREATE(_name, devIoBuzzerFuncs, NULL, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_BUZZER_H

/** @file       psoc4/pwm.h
 *  @brief      Pulse Width Modulation device driver for Sinowealth
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Enes Inanc
 *  @date       10.03.2023
 */
#ifndef FILE_PWM_H
#define FILE_PWM_H

#include "rcos.h"

/**
 *  @defgroup   PSOC4_PWM PWM
 *  @ingroup    PSOC4_DEV_IO
 *  @brief      CYPRESS PSOC4 Pulse Width Modulation
 *  @{
 */

#ifndef FILE_PWM_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE const tsDevCpxFuncs devCpxPwmCoreFuncs; ///< Functions for PWM core devices

/// @brief  Device specific constants
typedef struct
{
    PWM_TypeDef *PWMx;
    uint8_t PWM_CLKDIVNum;
    PWM_CLKDIV_Type PWM_CLKDIV;
    RCC_APB0_Type RCC_APBx_Type;
    uint32_t PWMchannel;
    GPIO_TypeDef *port;
    uint8_t GPIO_AF_PWMx;
    uint32_t pin;
    void (*onOff)(PWM_TypeDef *PWMx, CmdState OnOffState);          ///< CYPRESS module Start function
} tsPwmCoreConsts;

// void (*stop)(void);           ///< CYPRESS module Stop function
// void (*writePeriod)(uint32);  ///< CYPRESS module WritePeriod function
// void (*writeCompare)(uint32); ///< CYPRESS module WriteCompare function
// void (*writeCounter)(uint32); ///< CYPRESS module WriteCounter function
// uint32 (*readCounter)(void);  ///< CYPRESS module ReadCounter function

/// @brief  Device specific parameters
typedef struct
{
    uint32_t clock; ///< Input clock of module
    uint32_t period;  ///< Configured frequency
    uint32_t freq;  ///< Configured frequency
    uint32_t duty;  ///< Configured duty cycle
} tsPwmCoreParams;

/** @brief  Create a PWM core devCpx
 *  @param  _name           Name of devIO object
 *  @param  _PWMx           Number of PWM peripheral(0-1-2-3)
 *  @param  _PWMchannel     PWM channel(A-B)
 *  @param  _port           Name of GPIO port(A-B-C-D-E-F)
 *  @param  _pin            Pin number(1-2-3..15)
 *  @param  _PWM_CLKDIVNum  PWM clock divider(1-2-4-8-16-32-64-128-256)
 *  @param  _freq           PWM frequency
 *  @param  _duty           PWM duty cycle(0-100)
 */
// #define DEV_CPX_PWM_CORE_CREATE(_name, _PWMx, port, pin, SYSCLKDIVx, pwmFreq, pwmDuty)
#define DEV_CPX_PWM_CORE_CREATE(_name, _PWMx, _PWMchannel, _port, _pin, _PWM_CLKDIVNum, _freq, _duty) \
    const tsPwmCoreConsts _name##Consts =                                 \
        {                                                                 \
            .PWMx = PWM##_PWMx,              \
            .onOff = PWM_OnOff,       \
            .PWM_CLKDIVNum = _PWM_CLKDIVNum,       \
            .PWM_CLKDIV = SYSCLKDIV##_PWM_CLKDIVNum,      \
            .PWMchannel = PWM_CHANNEL_##_PWMchannel,      \
            .port = GPIO##_port,      \
            .pin = GPIO_Pin_##_pin,        \
            .GPIO_AF_PWMx = GPIO_AF_PWM##_PWMx,            \
            .RCC_APBx_Type = RCC_APB0_PWM##_PWMx,            \
    };                                                                    \
    tsPwmCoreParams _name##Params =                                       \
        {                                                                 \
            .freq = _freq,                                                      \
            .duty = _duty,                                                        \
    };                                                                    \
    DEV_CPX_CREATE(_name, devCpxPwmCoreFuncs, &_name##Params, &_name##Consts)

// _cyName##_Start,                                              
// _cyName##_Stop,                                               
// _cyName##_WritePeriod,                                        
// _cyName##_WriteCompare,                                       
// _cyName##_WriteCounter,                                       
// _cyName##_ReadCounter,                                        






INTERFACE const tsDevIoFuncs devIoPwmDutyFuncs; ///< Functions for PWM Duty devices

/// @brief  Sub device constants
typedef struct
{
    const tsDevCpx *mainDevice;
    tsPwmCoreParams *mainParams;
} tsPwmSubConsts;

/** @brief  Create a PWM_DUTY devIO
 *  @param  _name   Name of devIO object
 *  @param  _devCpx Target devCpx that is a PWM_CORE
 */
#define DEV_IO_PWM_DUTY_CREATE(_name, _devCpx) \
    const tsPwmSubConsts _name##Consts =       \
        {                                      \
            &_devCpx,                          \
            &_devCpx##Params,                  \
    };                                         \
    DEV_IO_CREATE(_name, devIoPwmDutyFuncs, &_devCpx##Params, &_name##Consts)

INTERFACE const tsDevIoFuncs devIoPwmFreqFuncs; ///< Functions for PWM Frequency devices

/** @brief  Create a PWM_FREQ devIO
 *  @param  _name   Name of devIO object
 *  @param  _devCpx Target devCpx that is a PWM_CORE
 */
#define DEV_IO_PWM_FREQ_CREATE(_name, _devCpx) \
    const tsPwmSubConsts _name##Consts =       \
        {                                      \
            &_devCpx,                          \
            &_devCpx##Params,                  \
    };                                         \
    DEV_IO_CREATE(_name, devIoPwmFreqFuncs, &_devCpx##Params, &_name##Consts)

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_PWM_H

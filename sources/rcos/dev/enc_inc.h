/** @file       enc_inc.h
 *  @brief      Header file of ENC_INC device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       15.01.2019
 */
#ifndef FILE_ENC_INC_H
#define FILE_ENC_INC_H

#include "rcos.h"

/**
 *  @defgroup   ENC_INC ENC_INC
 *  @ingroup    DEV_IO
 *  @brief      Incremental encoder device driver with ISR
 *  @details    Assumes that signal A changes prior to signal B during clock-wise rotation.
 *  @code
 *              (A) _---___---__
 *              (B) __---___---_
 *  @endcode
 *  @{
 */

#ifndef FILE_ENC_INC_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/// @brief  Polarity macros
#define ENC_INC_POS_POLARITY (0)
#define ENC_INC_NEG_POLARITY (1)

/// @brief  Functions for ENCINC devices
INTERFACE const tsDevIoFuncs devIoEncincFuncs;

/// @brief  Generic function called to determine which enc_inc device had a signal
INTERFACE void encincCheckIsr(const tsDevIo *device);

typedef enum
{
    ENC_INC_STATE_IDLE = 0,
    ENC_INC_STATE_INTERRUPT,
    ENC_INC_STATE_INTERRUPT_CONSISTENCY,
    ENC_INC_STATE_INTERRUPT_APPROVED,
} teEncIncIsrState;

/// @brief  Device specific parameters
typedef struct
{
    tsTimerCallBack timer;  ///< Timer callback to check IOs periodically
    int32_t count; ///< Polarity 0(Pos) => +, Polarity 1(Neg) => -: clock-wise rotation
                   ///< Polarity 0(Pos) => -, Polarity 1(Neg) => +: counter-clock-wise rotation
    uint8_t intState;
    uint8_t initialA;
    uint8_t initialB;
    uint8_t countDeb;
} tsEncIncParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevIo *signalA; ///< Signal A
    const tsDevIo *signalB; ///< Signal B
    const tsDevCpx *isrA;   ///< Isr for signal A
    void (*isrFunc)(void);  ///< This device isr function
    uint8_t polarity;
} tsEncIncConsts;

/** @brief  Create a devIo with default(pos) polarity
 *  @param  _name       Name of devIo object
 *  @param  _signalA    devIo for signal A input
 *  @param  _signalB    devIo for signal B input
 *  @param  _isrA       devCpx for ISR of signal A
 */
#define DEV_IO_ENC_INC_CREATE(_name, _signalA, _signalB, _isrA) \
    extern TIMER_CALLBACK_FUNC(encoderCheck);                   \
    const tsDevIo _name;                                        \
    void _name##Isr(void)                                       \
    {                                                           \
        encincCheckIsr(&_name);                                 \
    }                                                           \
    tsEncIncParams _name##Params = {                            \
        .timer    = TIMER_CALLBACK_INIT(encoderCheck, &_name),  \
        .count    = 0,                                          \
        .intState = ENC_INC_STATE_IDLE,                         \
        .initialA = 0,                                          \
        .initialB = 0,                                          \
        .countDeb = 0,                                          \
    };                                                          \
    const tsEncIncConsts _name##Consts =                        \
        {                                                       \
            .signalA  = &_signalA,                              \
            .signalB  = &_signalB,                              \
            .isrA     = &_isrA,                                 \
            .isrFunc  = _name##Isr,                             \
            .polarity = ENC_INC_POS_POLARITY,                   \
    };                                                          \
    DEV_IO_CREATE(_name, devIoEncincFuncs, &_name##Params, &_name##Consts)

/** @brief  Create a devIo with polarity parameter, if used pins are opposite of desired design's pin it can be use as negative polarity
 *  @param  _name       Name of devIo object
 *  @param  _signalA    devIo for signal A input
 *  @param  _signalB    devIo for signal B input
 *  @param  _isrA       devCpx for ISR of signal A
 *  @param  _polarity   Direction of rotary (ENC_INC_POS_POLARITY or ENC_INC_NEG_POLARITY)
 */
#define DEV_IO_ENC_INC_WPOL_CREATE(_name, _signalA, _signalB, _isrA, _polarity) \
    extern TIMER_CALLBACK_FUNC(encoderCheck);                                   \
    const tsDevIo _name;                                                        \
    void _name##Isr(void)                                                       \
    {                                                                           \
        encincCheckIsr(&_name);                                                 \
    }                                                                           \
    tsEncIncParams _name##Params = {                                            \
        .timer    = TIMER_CALLBACK_INIT(encoderCheck, &_name),                  \
        .count    = 0,                                                          \
        .intState = ENC_INC_STATE_IDLE,                                         \
        .initialA = 0,                                                          \
        .initialB = 0,                                                          \
        .countDeb = 0,                                                          \
    };                                                                          \
    const tsEncIncConsts _name##Consts =                                        \
        {                                                                       \
            .signalA  = &_signalA,                                              \
            .signalB  = &_signalB,                                              \
            .isrA     = &_isrA,                                                 \
            .isrFunc  = _name##Isr,                                             \
            .polarity = _polarity,                                              \
    };                                                                          \
    DEV_IO_CREATE(_name, devIoEncincFuncs, &_name##Params, &_name##Consts)

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_ENC_INC_H
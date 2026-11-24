/** @file       pwt.h
 *  @brief      Header file of PWT device for  RA2 agt or gpt timer
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Dilasude GOKTURK
 *  @author     Burak OZAN
 *  @date       18.06.2022
 */
/** Example GPT settings:
    General: Mode: Periodic
    General: Period: 0x10000
    General: Period Unit: Raw Counts
    Input: Count Up Source: 
    Input: Count Down Source: 
    Input: Start Source: GTIOCB Rising Edge While GTIOCA Low, GTIOCB Rising Edge While GTIOCA High
    Input: Stop Source: 
    Input: Clear Source: GTIOCB Rising Edge While GTIOCA Low, GTIOCB Rising Edge While GTIOCA High
    Input: Capture A Source: 
    Input: Capture B Source: GTIOCB Rising Edge While GTIOCA Low, GTIOCB Rising Edge While GTIOCA High**/

#ifndef FILE_PWT_H
#define FILE_PWT_H

/** INCLUDES ******************************************************************/
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_PWT_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   
 *  @ingroup    
 *  @brief      Pulse Width Timer
 *  @{
 */

/** CONSTANTS *****************************************************************/
#define SIZE_OF_CAPTURE (sizeof(uint32_t))


/** VARIABLES *****************************************************************/
/// @brief  Functions for PWT devices
INTERFACE const tsDevIoFuncs devIoPwtFuncs;

/** INTERFACE FUNCTION DECLARATIONS ********************************************/
/// @brief  Device specific Isr Callback function
INTERFACE void pwtIsrCallBack(timer_callback_args_t *p_args, const tsDevIo *device);

/** TYPEDEFS ******************************************************************/
/// @brief  Callback function type
typedef void (pwtCallbackFunctionType)(uint32_t capture, uint32_t clockFrequency);

/// @brief  Device specific parameters
typedef struct
{
    tsDeque *queue;                       ///< Queue to store every value
    pwtCallbackFunctionType *pwtCallback; ///< Callback function calling when a capture occured by device
    uint32_t timerOverflowCnt;            ///< Counter for timer overflow
    uint8_t captured;                     ///< Flag indicates signal captured or not
} tsPwtParams;

#ifdef PWT_CAC_ACTIVE
/// @brief  Device specific constant parameters
typedef struct
{
    const tsDevIo *cac;                         ///< Name of the cac stack (agt, gpt)
    const timer_instance_t *timer;              ///< Timer instance
    const uint32_t clockRatioConstant;          ///< Calculated value ratio of the Ext to HOCO
    const uint32_t resolutionCoef;              ///< Resolution coefficient of the period(if resolution is 0.1 S coef must be 10, if resolution is 1 S coef must be 1)
    timer_event_t captureEvent;           ///< Name of the capture event (TIMER_EVENT_CAPTURE_A, TIMER_EVENT_CAPTURE_B)
} tsPwtConsts;

/** MACROS ********************************************************************/
/** @brief  Create a devIo
 *  @param  _name                       Name of devIo object
 *  @param  _timer                      Name of the timer stack (agt, gpt)
 *  @param  _cac                        Name of the cac stack (agt, gpt)
 *  @param  _clockRatioConstant         Calculated value ratio of the Ext to HOCO
 *  @param  _resolutionCoef             Resolution coefficient of the period(if resolution is 0.1 S coef must be 10, if resolution is 1 S coef must be 1)
 *  @param  _captureEvent               Name of the capture event (TIMER_EVENT_CAPTURE_A, TIMER_EVENT_CAPTURE_B)
 *  @param  _meanBufferSize             Size of deque for calculating moving average (Range: 0 - 255)
 *  @param  _isrCallbackName            ISR Callback function name given to RASC, calling when a capture occured by Renesas driver
 */
#define DEV_IO_PWT_CREATE(_name, _timer, _cac, _clockRatioConstant, _resolutionCoef, _captureEvent, _meanBufferSize, _isrCallbackName) \
    DEQUE_CREATE(_name##Deq, _meanBufferSize * SIZE_OF_CAPTURE)                                                                        \
    const tsDevIo _name;                                                                                                               \
    void _isrCallbackName(timer_callback_args_t *p_args)                                                                               \
    {                                                                                                                                  \
        pwtIsrCallBack(p_args, &_name);                                                                                                \
    }                                                                                                                                  \
    tsPwtParams _name##Params =                                                                                                        \
        {                                                                                                                              \
            .queue            = &_name##Deq,                                                                                           \
            .pwtCallback      = NULL,                                                                                                  \
            .timerOverflowCnt = 0,                                                                                                     \
            .captured         = 0,                                                                                                     \
    };                                                                                                                                 \
    const tsPwtConsts _name##Consts =                                                                                                  \
        {                                                                                                                              \
            .cac                = &_cac,                                                                                               \
            .timer              = &_timer,                                                                                             \
            .clockRatioConstant = (const uint32_t)(_clockRatioConstant),                                                               \
            .resolutionCoef     = (const uint32_t)_resolutionCoef,                                                                     \
            .captureEvent       = _captureEvent,                                                                                       \
    };                                                                                                                                 \
    DEV_IO_CREATE(_name, devIoPwtFuncs, &_name##Params, &_name##Consts)

#else
/// @brief  Device specific constant parameters
typedef struct
{
    const timer_instance_t *timer;              ///< Timer instance
    const uint32_t resolutionCoef;              ///< Resolution coefficient of the period(if resolution is 0.1 S coef must be 10, if resolution is 1 S coef must be 1)
    timer_event_t captureEvent;           ///< Name of the capture event (TIMER_EVENT_CAPTURE_A, TIMER_EVENT_CAPTURE_B)
} tsPwtConsts;

/** MACROS ********************************************************************/
/** @brief  Create a devIo
 *  @param  _name                       Name of devIo object
 *  @param  _timer                      Name of the timer stack (agt, gpt)
 *  @param  _resolutionCoef             Resolution coefficient of the period(if resolution is 0.1 S coef must be 10, if resolution is 1 S coef must be 1)
 *  @param  _captureEvent               Name of the capture event (TIMER_EVENT_CAPTURE_A, TIMER_EVENT_CAPTURE_B)
 *  @param  _meanBufferSize             Size of deque for calculating moving average (Range: 0 - 255)
 *  @param  _isrCallbackName            ISR Callback function name given to RASC, calling when a capture occured by Renesas driver
 */
#define DEV_IO_PWT_CREATE(_name, _timer, _resolutionCoef, _captureEvent, _meanBufferSize, _isrCallbackName) \
    DEQUE_CREATE(_name##Deq, _meanBufferSize * SIZE_OF_CAPTURE)                                             \
    const tsDevIo _name;                                                                                    \
    void _isrCallbackName(timer_callback_args_t *p_args)                                                    \
    {                                                                                                       \
        pwtIsrCallBack(p_args, &_name);                                                                     \
    }                                                                                                       \
    tsPwtParams _name##Params =                                                                             \
        {                                                                                                   \
            .queue            = &_name##Deq,                                                                \
            .pwtCallback      = NULL,                                                                       \
            .timerOverflowCnt = 0,                                                                          \
            .captured         = 0,                                                                          \
    };                                                                                                      \
    const tsPwtConsts _name##Consts =                                                                       \
        {                                                                                                   \
            .timer          = &_timer,                                                                      \
            .resolutionCoef = (const uint32_t)_resolutionCoef,                                              \
            .captureEvent   = _captureEvent,                                                                \
    };                                                                                                      \
    DEV_IO_CREATE(_name, devIoPwtFuncs, &_name##Params, &_name##Consts)

#endif

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_PWT_H

/** @file       pwt2freq.h
 *  @brief      Header file of PWT 2 FREQ device for  RA2 (It is generic for all mcus)
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Dilasude GOKTURK
 *  @author     Burak OZAN
 *  @date       18.06.2022
 */

#ifndef FILE_PWT_2_FREQ_H
#define FILE_PWT_2_FREQ_H

/** INCLUDES ******************************************************************/
#include "rcos.h"
#include "pwt.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_PWT_2_FREQ_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   
 *  @ingroup    
 *  @brief      Period to frequency conversion
 *  @{
 */

/** CONSTANTS *****************************************************************/
#define SIZE_OF_FREQ (sizeof(uint32_t))


/** VARIABLES *****************************************************************/
/// @brief  Functions for PWT2FREQ devices
INTERFACE const tsDevIoFuncs devIoPwt2FreqFuncs;

/** INTERFACE FUNCTION DECLARATIONS ********************************************/
INTERFACE void pwt2freqCallBack(uint32_t capture, uint32_t clockFrequency, const tsDevIo *device);

/** TYPEDEFS ******************************************************************/
/// @brief  Device specific parameters
typedef struct
{
    tsDeque *queue;                     ///< Queue to store every value
    uint32_t prevCapture;               ///< Previous capture
    uint16_t signalMissed;              ///< Flag using for False Signal Filter
    uint16_t consecutiveFalseSignalCnt; ///< Counter using for False Signal Filter
    uint8_t newValueFlag;               ///< Flag indicates signal new value exist or not
} tsPwt2FreqParams;

/// @brief  Device specific constant parameters
typedef struct
{
    const tsDevIo *pwtDevice;                ///< Pwt device
    pwtCallbackFunctionType *callbackFunc;   ///< Callback func will give to pwt device
    const uint32_t resolutionCoef;           ///< Resolution coefficient of the freq(if resolution is 0.1 coef must be 10, if resolution is 1 coef must be 1)
    const uint8_t falseSignalFilterActive;   ///< Flag to apply filter(TRUE, FALSE)
} tsPwt2FreqConsts;

/** MACROS ********************************************************************/
/** @brief  Create a devIo
 *  @param  _name                       Name of devIo object
 *  @param  _pwtDevice                  Pwt device
 *  @param  _resolutionCoef             Resolution coefficient of the freq(if resolution is 0.1 coef must be 10, if resolution is 1 coef must be 1)
 *  @param  _falseSignalFilterActive    Flag to apply filter(TRUE, FALSE)
 *  @param  _meanBufferSize             Size of deque for calculating moving average (Range: 0 - 255)
 */
#define DEV_IO_PWT_2_FREQ_CREATE(_name, _pwtDevice, _resolutionCoef, _falseSignalFilterActive, _meanBufferSize) \
    DEQUE_CREATE(_name##Deq, _meanBufferSize * SIZE_OF_FREQ)                                                    \
    const tsDevIo _name;                                                                                        \
    void _name##Callback(uint32_t capture, uint32_t clockFrequency)                                             \
    {                                                                                                           \
        pwt2freqCallBack(capture, clockFrequency, &_name);                                                      \
    }                                                                                                           \
    tsPwt2FreqParams _name##Params =                                                                            \
        {                                                                                                       \
            .queue                     = &_name##Deq,                                                           \
            .prevCapture               = 0,                                                                     \
            .signalMissed              = 0,                                                                     \
            .consecutiveFalseSignalCnt = 0,                                                                     \
            .newValueFlag              = 0,                                                                     \
    };                                                                                                          \
    const tsPwt2FreqConsts _name##Consts =                                                                      \
        {                                                                                                       \
            .pwtDevice               = &_pwtDevice,                                                             \
            .callbackFunc            = _name##Callback,                                                         \
            .resolutionCoef          = (const uint32_t)_resolutionCoef,                                         \
            .falseSignalFilterActive = _falseSignalFilterActive,                                                \
    };                                                                                                          \
    DEV_IO_CREATE(_name, devIoPwt2FreqFuncs, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_PWT_2_FREQ_H

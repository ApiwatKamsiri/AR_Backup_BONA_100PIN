/** @file       knob.h
 *  @brief      Header file of KNOB device
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Arif Salih OZMEN
 *  @date       7.06.2023
 */
#ifndef FILE_KNOB_H
#define FILE_KNOB_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_KNOB_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   KNOB KNOB
 *  @ingroup    DEV_IO
 *  @brief      Knob Position Control
 *  @{
 */

/// @brief  Functions for KNOB devices
INTERFACE const tsDevIoFuncs devIoKNOBFuncs;
INTERFACE const tsDevIoFuncs devIoKNOBErrorDetecorFuncs;

typedef enum
{
    eKNOBErrorNone = 0,
    eKNOBErrorOpenCircuit,
    eKNOBErrorShortCircuit,
} teKnobErrorType;

/// @brief  Device specific parameters
typedef struct
{
    teKnobErrorType error;
    uint8_t         valueInPosition;
    uint16_t        value;
    uint16_t        positionChanged;
} tsKNOBParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevIo *adcDevice; ///< Ptr to the ADC device of the NTC
} tsKNOBConsts;

/** @brief  Create a devIo
 *  @param  _name   Name of devIo object
 */
#define DEV_IO_KNOB_CREATE(_name, _adcDevice )      \
    tsKNOBParams _name##Params =         \
    {                                           \
        .error = eKNOBErrorNone,                \
        .valueInPosition = 0,                   \
        .value = 0,                             \
        .positionChanged = 0,                   \
    };                                          \
    const tsKNOBConsts _name##Consts =   \
    {                                           \
        .adcDevice = &_adcDevice,                \
    };                                          \
    DEV_IO_CREATE(_name, devIoKNOBFuncs, &_name##Params, &_name##Consts)

typedef struct
{
    const tsDevIo *knobDevice;   ///< The NTC device that the error detector is associated to it
} tsKnobErrorDetectorConsts;

/** @brief  Create a devIo for NTC error detector device
 *  @param  _name   Name of devIo object
 *  @param  _ntcDevice  The NTC device that the error detector is associated to it
 * 
 */
#define DEV_IO_KNOB_ERROR_DETECTOR_CREATE(_name, _knobDevice) \
    const tsKnobErrorDetectorConsts _name##Consts =          \
        {                                                   \
            .knobDevice = &_knobDevice,                       \
    };                                                      \
    DEV_IO_CREATE(_name, devIoKNOBErrorDetecorFuncs, NULL, &_name##Consts)
/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_KNOB_H

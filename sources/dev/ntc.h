/** @file       ntc.h
 *  @brief      Header file of NTC device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-05
 *  @details
 *  This device is used to translate the values that are measured by ADC which are
 *  connected to NTCs to its equalevent temperature values in celsius.
 *  The output is multiplied by 10, so a measured 23.3 C temp is returned as 233.
 *  The returned value is signed value with type int32_t.
 *
 *  There is an additional sub-device (NTC error detector) to get the error status of the NTC,
 *  like open circuit or short circuit error.
 *  The error detector value is updated each time the get function of the NTC device is called.
 *  So you shouldn't get a value from error detector device before getting a value from its NTC device.
 *
 *  @code
 *  // Creation ...
 *  #include "dev/ke14/adc.h"
 *  DEV_CPX_ADC_CORE_CREATE(adcCore, ADC0, 20, ADC_CH(9, 0), ADC_CH(8, 0))
 *  DEV_IO_ADC_CREATE(adcFFAir, adcCore, 0)
 *  DEV_IO_ADC_CREATE(adcFFEva, adcCore, 1)
 *
 *  #include"dev/ntc.h"
 *  DEV_IO_NTC_CREATE(ntcFFAir, adcFFAir)
 *  DEV_IO_NTC_CREATE(ntcFFEva, adcFFEva)
 *
 *  DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFFAir, ntcFFAir)
 *  DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFFEva, ntcFFEva)
 *
 *  // Initialization ...
 *  devCpxInit(&adcCore, NULL);
 *
 *  devIoInit(&ntcFFAir, NULL);
 *  devIoInit(&ntcFFEva, NULL);
 *
 *  // Usage ...
 *  int32_t temp = devIoGet(&ntcFFAir);
 *  teNtcErrorType error = devIoGet(&ntcErrorFFAir);
 *
 *  @endcode
 *
 *
 */
#ifndef FILE_NTC_H
#define FILE_NTC_H

/// Includes
#include "rcos.h"
#include "math.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_NTC_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   NTC NTC
 *  @ingroup    DEV_IO
 *  @brief      Translates ADC values of NTC sensors to temperature values multiplied with 10
 *  @{
 */

/// @brief  Functions for NTC devices
INTERFACE const tsDevIoFuncs devIoNtcFuncs;
INTERFACE const tsDevIoFuncs devIoNtcErrorDetectorFuncs;

typedef enum
{
    eNTCErrorNone = 0,
    eNTCErrorOpenCircuit,
    eNTCErrorShortCircuit,
} teNtcErrorType;

/// @brief  Device specific parameters
typedef struct
{
    teNtcErrorType error; ///< Holds the detected error type after last ADC read
} tsNtcParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevIo *adcDevice; ///< Ptr to the ADC device of the NTC
} tsNtcConsts;

/** @brief  Create a devIo for NTC device
 *  @param  _name   Name of devIo object
 *  @param  _adcDevice  The ADC device of that is connected to the NTC
 *  @note   _adcDevice can be filtered ADC values
 */
#define DEV_IO_NTC_CREATE(_name, _adcDevice) \
    tsNtcParams _name##Params =              \
        {                                    \
            .error = eNTCErrorNone,          \
    };                                       \
    const tsNtcConsts _name##Consts =        \
        {                                    \
            .adcDevice = &_adcDevice,        \
    };                                       \
    DEV_IO_CREATE(_name, devIoNtcFuncs, &_name##Params, &_name##Consts)

typedef struct
{
    const tsDevIo *ntcDevice;   ///< The NTC device that the error detector is associated to it
} tsNtcErrorDetectorConsts;

/** @brief  Create a devIo for NTC error detector device
 *  @param  _name   Name of devIo object
 *  @param  _ntcDevice  The NTC device that the error detector is associated to it
 * 
 */
#define DEV_IO_NTC_ERROR_DETECTOR_CREATE(_name, _ntcDevice) \
    const tsNtcErrorDetectorConsts _name##Consts =          \
        {                                                   \
            .ntcDevice = &_ntcDevice,                       \
    };                                                      \
    DEV_IO_CREATE(_name, devIoNtcErrorDetectorFuncs, NULL, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_NTC_H

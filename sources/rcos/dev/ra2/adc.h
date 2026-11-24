/** @file       adc.h
 *  @brief      Header file of ADC device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       4/9/2021
 */
#ifndef FILE_ADC_H
#define FILE_ADC_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_ADC_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   RA2_ADC ADC
 *  @ingroup    RA2_DEV_IO
 *  @brief      Analog-Digital Converter device driver for Renesas RA2 series
 *  @{
 */
#define ADC_ISR_FUNCTION(_name) void _name(adc_callback_args_t *p_args)

//// @brief  Functions for ADC_CORE devices
INTERFACE const tsDevCpxFuncs devCpxAdcCoreFuncs;

/// @brief  Device specific constant parameters
typedef struct
{
    const adc_instance_t *adc;  ///< ADC instance
    const uint8_t *chList;      ///< List of active channels
    const uint16_t refreshTime; ///< Start a new conversion sequence
    const uint8_t chListSize;   ///< Channel list size
} tsAdcCoreConsts;

/// @brief  Device specific parameters
typedef struct
{
    tsTimerCallBack timer; ///< Callback timer
    uint16_t *chValues;    ///< Last conversion values for active channels
} tsAdcCoreParams;

/** @brief  Create a devCpx
 *  @param  _name           Name of devCpx object
 *  @param  _adcStack       Name of the r_adc ADC stack
 *  @param  _callBack       Name of scan end interrupt callback
 *  @param  _refreshTime    Timeout duration for values to be refreshed
 *  @param  ...             Channels that are going to be converted in order
 */
#define DEV_CPX_ADC_CORE_CREATE(_name, _adcStack, _callBack, _refreshTime, ...) \
    const tsDevCpx _name;                                                       \
    ADC_ISR_FUNCTION(_callBack)                                                 \
    {                                                                           \
        UNUSED(p_args);                                                         \
        adcCoreIrqHandler(&_name);                                              \
    }                                                                           \
    const uint8_t _name##ChList[] =                                             \
        {                                                                       \
            __VA_ARGS__,                                                        \
    };                                                                          \
    uint16_t _name##ChValues[ARRAY_SIZE(_name##ChList)];                        \
    const tsAdcCoreConsts _name##Consts =                                       \
        {                                                                       \
            &_adcStack,                                                         \
            _name##ChList,                                                      \
            _refreshTime,                                                       \
            ARRAY_SIZE(_name##ChList),                                          \
    };                                                                          \
    extern TIMER_CALLBACK_FUNC(adcCoreRefresh);                                 \
    tsAdcCoreParams _name##Params =                                             \
        {                                                                       \
            TIMER_CALLBACK_INIT(adcCoreRefresh, &_name),                        \
            _name##ChValues,                                                    \
    };                                                                          \
    DEV_CPX_CREATE(_name, devCpxAdcCoreFuncs, &_name##Params, &_name##Consts)

/// @brief  Function to handle interrupt requests of an ADC_CORE device. DO NOT CALL OUTSIDE THE TARGET ISR
INTERFACE void adcCoreIrqHandler(const tsDevCpx *device);

/** @} */

/// @brief  Functions for ADC devices
INTERFACE const tsDevIoFuncs devIoAdcFuncs;

/// @brief  Device specific parameters
typedef struct
{
    uint8_t index; ///< Index for the list of enabled channels in target ADC_CORE device
} tsAdcParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevCpx *adcCore; ///< Target ADC_CORE device
} tsAdcConsts;

/** @brief  Create a devIo
 *  @param  _name       Name of devIo object
 *  @param  _adcCore    Target ADC_CORE
 *  @param  _idx        Index for the list of enabled channels in target ADC_CORE device
 */
#define DEV_IO_ADC_CREATE(_name, _adcCore, _idx) \
    const tsAdcParams _name##Params =            \
        {                                        \
            (_idx),                              \
    };                                           \
    const tsAdcConsts _name##Consts =            \
        {                                        \
            &(_adcCore),                         \
    };                                           \
    DEV_IO_CREATE(_name, devIoAdcFuncs, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_ADC_H

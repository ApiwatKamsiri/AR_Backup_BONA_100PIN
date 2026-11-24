/** @file       psoc4/adc.h
 *  @brief      CYPRESS PSOC4 Analog-Digital Conversion
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       11.01.2019
 */
#ifndef FILE_ADC_H
#define FILE_ADC_H

#include "rcos.h"
#include "sh32f9xx_sb0_libcfg.h"
#include "sh32f9xx_sb0_gpio.h"
#include "sh32f9xx_sb0_adc.h"

/**
 *  @defgroup   PSOC4_ADC ADC
 *  @ingroup    PSOC4_DEV_IO
 *  @brief      CYPRESS PSOC4 Analog-Digital Conversion
 *  @{
 */

#ifndef FILE_ADC_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/** @defgroup   PSOC4_ADC_CORE ADC_CORE
 *  @brief      Peripheral core access
 *  @{
 */
/// @brief  Functions for ADC_CORE devices
INTERFACE const tsDevCpxFuncs devCpxAdcCoreFuncs;

/// @brief  ADC ISR Funciton
#define ADC_ISR_FUNCTION(_name) void _name(void)

/// @brief  Device specific parameters
typedef struct
{
    ADC_TypeDef *ADCx;
    void (*startStop)(ADC_TypeDef *ADCx, CmdState OnOffState);                  ///< Component start function
    void (*startConvert)(ADC_TypeDef *ADCx);           ///< Component start conversion function
    void (*stopConvert)(ADC_TypeDef *ADCx);            ///< Component stop conversion function
    const uint16_t refreshTime; ///< Start a new conversion sequence
    const uint8_t *chList;      ///< List of active channels
    const uint8_t chListSize;   ///< Channel list size
} tsAdcCoreConsts;

/// @brief  Device specific parameters
typedef struct
{
    tsTimerCallBack timer; ///< Callback timer
    GPIO_TypeDef *port;
    uint32_t pin;
    uint16_t *chValues;    ///< Last conversion values for active channels
} tsAdcCoreParams;

/** @brief  Create a devCpx
 *  @param  _name       Name of devCpx object
 *  @param  _cyBlock    CYPRESS ADC block name
 */
#define DEV_CPX_ADC_CORE_CREATE(_name, _ADC, _refreshTime, ...)           \
    const tsDevCpx _name;                                                       \
    ADC_ISR_FUNCTION(ADC_Handler)                                                 \
    {                                                                           \
        adcCoreIrqHandler(&_name);                                              \
    }                                                                           \
    const uint8_t _name##ChList[] =                                             \
        {                                                                       \
            __VA_ARGS__,                                                        \
    };                                                                          \
    uint16_t _name##ChValues[ARRAY_SIZE(_name##ChList)];                        \
    const tsAdcCoreConsts _name##Consts =                  \
        {                                                  \
            .ADCx           = _ADC,     \
            .startStop        = ADC_OnOff,              \
            .startConvert = ADC_SoftwareStartConversion,       \
            .stopConvert  = ADC_SoftwareStopConversion,        \
            .refreshTime = _refreshTime,        \
            .chList = _name##ChList,            \
            .chListSize = ARRAY_SIZE(_name##ChList),     \
    };                                                     \
    extern TIMER_CALLBACK_FUNC(adcCoreRefresh);                                 \
    tsAdcCoreParams _name##Params =                                             \
        {                                                                       \
            .timer = TIMER_CALLBACK_INIT(adcCoreRefresh, &_name),                        \
            .chValues = _name##ChValues,                                                    \
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
    const tsDevCpx *adcCore; ///< Target ADC_CORE device
    uint8_t channel;         ///< Channel number on target ADC_CORE device
} tsAdcConsts;

/** @brief  Create a devIo
 *  @param  _name       Name of devIo object
 *  @param  _adcCore    Target tsDevCpx used as ADC_CORE device
 *  @param  _channel    Target channel number on ADC_CORE device
 */
#define DEV_IO_ADC_CREATE(_name, _adcCore, _channel) \
    const tsAdcConsts _name##Consts =                \
        {                                            \
            .adcCore = &_adcCore,                    \
            .channel = (_channel),                   \
    };                                               \
    DEV_IO_CREATE(_name, devIoAdcFuncs, NULL, &_name##Consts)

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_ADC_H

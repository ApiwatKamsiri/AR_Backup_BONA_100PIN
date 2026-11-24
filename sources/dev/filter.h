/** @file       filter.h
 *  @brief      Header file of FILTER device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-02
 * 
 *  @details
 * 
 * This device is used to filter the data that are gotten from another devIo device.
 * Each time devIoGet function is called, the filter read a value form the target
 * device and return a filtered data.
 *
 * The filtered value is calculated as below:
 * 1- read data values are sorted in ascending order (their count is specified with filterSize)
 * 2- the median of the mean values (their count is specified with meanCount) is calculated
 * 3- the median of result calculated up and the old filtered value is calculated again
 *
 * @note filter data are stored as int32_t so the filter can work with devIos that return
 * negative values
 * @note at first read operation the filter's dequeue is filled with the same read value
 * @note remember median values are the values at the center of a sorted sequence of values
 * @note remember mean value is the arithmetic average of a sequence of values
 *
 * @code
 *
 * // Creation ...
 *  #include "dev/ke14/adc.h"
 *  DEV_CPX_ADC_CORE_CREATE(adcCore, ADC0, 20, ADC_CH(9, 0), ADC_CH(8, 0))
 *  DEV_IO_ADC_CREATE(adcFFAir, adcCore, 0)
 *
 * #include "dev/filter.h"
 * DEV_IO_FILTER_CREATE(filteredAdc, adcFFAir, 8, 2)
 *
 * // Initialization ...
 * devCpxInit(&adcCore, NULL);
 * devIoInit(&filteredAdc, NULL);
 *
 *  // Usage ...
 * int32_t filteredAdcValue = devIoGet(&filteredAdc);
 *
 * @endcode
 *
 */
#ifndef FILE_FILTER_H
#define FILE_FILTER_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_FILTER_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   FILTER FILTER
 *  @ingroup    DEV_IO
 *  @brief      Filtering data gotten from a devIo
 *  @{
 */

/// @brief  Functions for FILTER devices
INTERFACE const tsDevIoFuncs devIoFilterFuncs;

/// @brief  Device specific parameters
typedef struct
{
    int32_t filteredData; ///< Holds the result of the last calculated filtered value
    bool firstReadDone;   ///< Flag indicates if the filter has read data before
} tsFilterParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevIo *target; ///< Ptr to the target device which will supply unfiltered data
    tsDeque *rawDataQueue; ///< Ptr to queue that will hold the raw (unfiltered) data
    size_t filterSize;     ///< Item count that will be holden in the queue
    size_t meanCount;      ///< Count of mean values that their median value will be calculated
} tsFilterConsts;

/** @brief  Create a devIo for filter device
 *  @param  _name      Name of devIo object
 *  @param _target     Target device which will supply unfiltered data
 *  @param _filterSize Item count that will be holden in the filter queue
 *  @param _meanCount  Count of mean values that their median value will be calculated
 */
#define DEV_IO_FILTER_CREATE(_name, _target, _filterSize, _meanCount)                     \
    DEQUE_CREATE(_name##Deq, (((_filterSize) < 3) ? 3 : (_filterSize)) * sizeof(int32_t)) \
    tsFilterParams _name##Params;                                                         \
    const tsFilterConsts _name##Consts =                                                  \
        {                                                                                 \
            .target = &_target,                                                           \
            .rawDataQueue = &_name##Deq,                                                  \
            .filterSize = _filterSize,                                                    \
            .meanCount = _meanCount,                                                      \
    };                                                                                    \
    DEV_IO_CREATE(_name, devIoFilterFuncs, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_FILTER_H

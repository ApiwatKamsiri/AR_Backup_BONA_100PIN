/** @file       mean.h
 *  @brief      Collect and calculate average value of a target devIo
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       8.01.2019
 *  @author     Burak OZAN
 *  @date       09.09.2022
 */
#ifndef FILE_MEAN_H
#define FILE_MEAN_H

#include "rcos.h"

/**
 *  @defgroup   MEAN MEAN
 *  @ingroup    DEV_IO
 *  @brief      Collect and calculate average value of a target devIo
 *  @{
 */

#ifndef FILE_MEAN_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/// @brief  Functions for MEAN devices
INTERFACE const tsDevIoFuncs devIoMeanFuncs;

/// @brief Collecting raw data Timer Callback Function
INTERFACE TIMER_CALLBACK_FUNC(rawDataGetTcb);

/// @brief  Device specific parameters
typedef struct
{
    tsTimerCallBack tcb; ///< Callback function to collect raw data
} tsMeanParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevIo *target; ///< Target I/O device
    tsDeque *queue;        ///< Queue to store every value
    uint16_t period;       ///< tcb's period
    uint8_t disExt;        ///< Dismiss extreme values(min and max values are dismissed)
    uint8_t count;         ///< Number of items stored in queue
} tsMeanConsts;

/** @brief  Create a devIo
 *  @param  _name   Name of devIo object
 *  @param  _target devIo to get values from
 *  @param  _count  Number of values to collect for calculating the mean value(should be 3 at least and less than 255)
 *  @param  _disExt TRUE or FALSE
 */
#define DEV_IO_MEAN_CREATE(_name, _target, _count, _disExt)             \
    DEQUE_CREATE(_name##Deq, (((_count) < 3) ? 3 : (_count)) * sizeof(uint32_t)) \
    tsMeanParams _name##Params =                                                 \
        {                                                                        \
            TIMER_CALLBACK_INIT(rawDataGetTcb, NULL),                            \
    };                                                                           \
    const tsMeanConsts _name##Consts =                                           \
        {                                                                        \
            .target = &_target,                                                  \
            .queue  = &_name##Deq,                                               \
            .period = 0,                                                         \
            .disExt = (_disExt) ? TRUE : FALSE,                                  \
            .count  = (_count),                                                  \
    };                                                                           \
    DEV_IO_CREATE(_name, devIoMeanFuncs, &_name##Params, &_name##Consts)

/** @brief  Create a devIo
 *  @param  _name   Name of devIo object
 *  @param  _target devIo to get values from
 *  @param  _count  Number of values to collect for calculating the mean value(should be 3 at least and less than 255)
 *  @param  _period Period of collecting raw data for calculating the mean value in units of ms(If this parameter is 0, raw data will collecting when the user calls the devIoMean object's get function)
 *  @param  _disExt TRUE or FALSE
 */
#define DEV_IO_MEAN_W_PERIOD_CREATE(_name, _target, _count, _period, _disExt)    \
    DEQUE_CREATE(_name##Deq, (((_count) < 3) ? 3 : (_count)) * sizeof(uint32_t)) \
    tsMeanParams _name##Params =                                                 \
        {                                                                        \
            TIMER_CALLBACK_INIT(rawDataGetTcb, NULL),                            \
    };                                                                           \
    const tsMeanConsts _name##Consts =                                           \
        {                                                                        \
            .target = &_target,                                                  \
            .queue  = &_name##Deq,                                               \
            .period = _period,                                                   \
            .disExt = (_disExt) ? TRUE : FALSE,                                  \
            .count  = (_count),                                                  \
    };                                                                           \
    DEV_IO_CREATE(_name, devIoMeanFuncs, &_name##Params, &_name##Consts)

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_MEAN_H

/** @file       contdebounce.h
 *  @brief      Continuous debounce engine for multiple tsDevIo
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Burak OZAN
 *  @date       7.11.2022
 */
#ifndef FILE_CONT_DEBOUNCE_H
#define FILE_CONT_DEBOUNCE_H

#include "rcos.h"

/**
 *  @defgroup   CONTINUOUS DEBOUNCE
 *  @ingroup    DEV_CPX
 *  @brief      Continuous debounce engine for multiple tsDevIo
 *  @{
 */

#ifndef FILE_CONT_DEBOUNCE_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/// @brief  Maximum value of hysteresis percentage value
#define CONT_DEB_HYS_PER_MAX_VAL (100)

/// @brief  Functions for CONTINUOUS DEBOUNCE devices
INTERFACE const tsDevCpxFuncs devCpxContDebounceFuncs;

/// @brief  Continuous debounce item structure
typedef struct
{
    const tsDevIo *devIo;  ///< Target devIo that will be checked
    uint32_t stable;       ///< Last stable value
    uint32_t changing;     ///< Changing into
    uint8_t stabilityCnt;  ///< Number of times changing value was stable
    uint8_t limit;         ///< Limit for stabilityCnt value
    uint8_t hysteresisPer; ///< Hysteresis of the limit (% value)
    uint8_t hysteresisCnt; ///< Counter for hysteresis
} tsContDebounceItem;

/// @brief  Device specific parameters
typedef struct
{
    tsTimerCallBack timer;      ///< Timer callback to check every IO periodically
    tsContDebounceItem *ioList; ///< Array of CONTINUOUS DEBOUNCE ITEMS that are going to be checked
    uint8_t ioCount;            ///< Number of IOs in the ioList
    uint8_t period;             ///< ioList is checked every period in milliseconds
} tsContDebounceParams;

/** @brief  Initialize a continuous debounce item
 *  @param  _devIo  target devIo
 *  @param  _contDebCnt number of times counted for stability
 *  @param  _contDebHysPer Hysteresis percentage for stability (0-100, _contDebCnt = 10 and _contDebHysPer = 80 => 8 of last 10 values must be the same for stability)
 */
#define CONT_DEBOUNCE_ITEM(_devIo, _contDebCnt, _contDebHysPer) \
    {                                                           \
        &(_devIo), 0, 0, 0, (_contDebCnt), (_contDebHysPer), 0  \
    }

/** @brief  Create a CONTINUOUS DEBOUNCE devCpx
 *  @param  _name       Name of devCpx object
 *  @param  _period     Duration of each period to check array of devIo
 *  @param  ...         Continuous debounce items' initial values created by @ref CONT_DEBOUNCE_ITEM
 */
#define DEV_CPX_CONT_DEBOUNCE_CREATE(_name, _period, ...)              \
    extern TIMER_CALLBACK_FUNC(contDebounceCheck);                     \
    const tsDevCpx _name;                                              \
    tsContDebounceItem _name##List[] =                                 \
        {                                                              \
            __VA_ARGS__,                                               \
    };                                                                 \
    tsContDebounceParams _name##Params =                               \
        {                                                              \
            .timer   = TIMER_CALLBACK_INIT(contDebounceCheck, &_name), \
            .ioList  = _name##List,                                    \
            .ioCount = ARRAY_SIZE(_name##List),                        \
            .period  = (_period),                                      \
    };                                                                 \
    DEV_CPX_CREATE(_name, devCpxContDebounceFuncs, &_name##Params, NULL)

/// @brief  Functions for CONTINUOUS DEBOUNCE input devices
INTERFACE const tsDevIoFuncs devIoContDebIpFuncs;

/// @brief  Device specific parameters
typedef struct
{
    const tsDevCpx *contDebounce; ///< Target continuous debounce tsDevCpx object
    uint8_t idx;                  ///< Index of continuous debounce item
} tsContDebIpParams;

/** @brief  Create a CONTINUOUS DEBOUNCE devIo
 *  @param  _name           Name of devIo object
 *  @param  _devContDebounce    Name of tsDevCpx object that is a CONTINUOUS DEBOUNCE engine
 *  @param  _idx            Index of continuous debounced item in CONTINUOUS DEBOUNCE engine
 */
#define DEV_IO_CONT_DEBOUNCE_CREATE(_name, _devContDebounce, _idx) \
    const tsContDebIpParams _name##Params =                        \
        {                                                          \
            .contDebounce = &(_devContDebounce),                   \
            .idx          = (_idx),                                \
    };                                                             \
    DEV_IO_CREATE(_name, devIoContDebIpFuncs, &_name##Params, NULL)

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_CONT_DEBOUNCE_H

/** @file       ac_output.h
 *  @brief      Header file of AC_OUTPUT device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-01
 *  @details
 *
 * The purpose of this device is to prevent switching AC outputs at the same time by putting some delay between switching operations
 * Switching AC outputs at the same time leads to problems during EMC tests
 *
 * The system consists of two IO devices: AC outputs & AC output controller
 * The state of an AC outputs is not controlled with AC output device itself but with the AC controller
 *
 * "Blocking switching ac outputs at the same time" feature can be enabled/disabled with the controller put func
 * Furthermore, each output can be configured (at the initialization time) to not participate in blocking behavior so they are switched immediately
 * Each AC output device's configuration can provide a parameter tells how long the next outputs should be blocked(waited) before being switched
 *
 *
 *
 *  @code
 * #include "dev/ke14/gpio.h"
 * DEV_IO_GPIO_CREATE(gpioCompressor, UC_AC_COMPRESSOR, 0, 0, 1)
 * DEV_IO_GPIO_CREATE(gpioFRZDefrost, UC_AC_FRZ_DEF, 0, 0, 1)
 * DEV_IO_GPIO_CREATE(gpioFRZFan, UC_AC_FRZ_FAN, 0, 0, 1)
 *
 * #include "dev/ac_output.h"
 * DEV_IO_AC_OUTPUT_CREATE(acCompressor, gpioCompressor)
 * DEV_IO_AC_OUTPUT_CREATE(acFRZDefrsot, gpioFRZDefrost)
 * DEV_IO_AC_OUTPUT_CREATE(acFRZFan, gpioFRZFan)
 *
 * DEV_IO_AC_OUTPUT_CONTROLLER_CREATE(acController, 20, &acCompressor, &acFRZDefrsot, &acFRZFan)
 *
 *
 * //initialization ........
 *
 * devIoInit(&acController, NULL);
 *
 * tsAcOutputConfig cnfg = {.blockingTimeMs = 1000, .participateInBlocking = true};
 * devIoInit(&acCompressor, &cnfg);
 * devIoInit(&acFRZDefrsot, &cnfg);
 *
 *
 * tsAcOutputConfig cnfg2 = {.blockingTimeMs = 0, .participateInBlocking = false};
 * devIoInit(&acFRZFan, &cnfg2);
 *
 *
 *
 *
 * // usage ........
 *
 * devIoPut(&acController, 1); // enable blocking functionality
 *
 * devIoPut(&acCompressor, 1);
 * devIoPut(&acFRZDefrsot, 1); // this output will be switched after 1 second (blockingTimeMs = 1000)
 * devIoPut(&acFRZFan, 1); // this output will be switched immediately because it's configured to not particabate in blocking
 *
 *
 *
 *
 *
 * devIoPut(&acController, 0); // disable blocking functionality
 *
 * devIoPut(&acCompressor, 0);
 * devIoPut(&acFRZDefrsot, 0);
 * devIoPut(&acFRZFan, 0); // all outputs will be switched at the same time
 * @endcode
 */

#ifndef FILE_AC_OUTPUT_H
#define FILE_AC_OUTPUT_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_AC_OUTPUT_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   AC_OUTPUT AC_OUTPUT
 *  @ingroup    DEV_IO
 *  @brief
 *  @{
 */

/// @brief  Functions for AC output controller io devices
INTERFACE const tsDevIoFuncs devIoACOutputsControllerFuncs;

typedef struct
{
    bool blocked : 1;             ///< controller blocking state
    bool blockingFuncEnabled : 1; ///< false: all outputs can be switched immediately by the controller, true: some outputs can be delayed/blocked before being switched
    tsTimerEvent unblockTimer;    ///< timer used to disable blocking when it's elapsed
    tsTimerEvent periodicTimer;   ///< timer used to check the outputs of the controller that need to be switched periodically
} tsACOutputsControllerParams;

typedef struct
{
    const tsDevIo **devPtrArray; ///< constant array that holds pointers to all outputs of the controller
    size_t devPtrArraySize;      ///< size of the array mentioned up
    uint32_t scanningTimeMs;     ///< the controller will check all outputs periodically with this time
    tsProcess *process;          ///< pointer to the controller process
} tsACOutputsControllerConsts;

/** @brief  Create a devIo as PIT module
 *  @param  _name   Name of devIo object
 *  @param _scanningTimeMs period to check output states in MS
 */
#define DEV_IO_AC_OUTPUT_CONTROLLER_CREATE(_name, _scanningTimeMs, ...)                              \
    tsProcess _name##Process;                                                                        \
    const tsDevIo *_name##PtrDevArray[] = {__VA_ARGS__};                                             \
    tsACOutputsControllerParams _name##Params =                                                      \
        {                                                                                            \
            .blocked = false,                                                                        \
            .blockingFuncEnabled = false,                                                            \
    };                                                                                               \
    tsACOutputsControllerConsts _name##Consts =                                                      \
        {                                                                                            \
            .devPtrArray = _name##PtrDevArray,                                                       \
            .devPtrArraySize = ARRAY_SIZE(_name##PtrDevArray),                                       \
            .scanningTimeMs = _scanningTimeMs,                                                       \
            .process = &_name##Process,                                                              \
    };                                                                                               \
    PROCESS_CREATE(_name##Process, ACOutputsControllerProcessInit, ACOutputsControllerProcessDeinit, \
                   PROCESS_NONE, &_name##Params, &_name##Consts)                                     \
    DEV_IO_CREATE(_name, devIoACOutputsControllerFuncs, &_name##Params, &_name##Consts)

/// @brief  Functions for AC_OUTPUT devices
INTERFACE const tsDevIoFuncs devIoAcOutputFuncs;

/// @brief  Device specific configuration
typedef struct
{
    bool participateInBlocking; ///< false: the controller will switch the output even if blocking is active, true: the controller wil switch it only when blocking is not active
    uint32_t blockingTimeMs;    ///< How long blocking will stay active after switching this output
} tsAcOutputConfig;

/// @brief  Device specific parameters
typedef struct
{
    uint8_t currentState : 1;       ///< AC output current state
    uint8_t requestedState : 1;     ///< AC output requested state with devIoPut func
    bool participateInBlocking : 1; ///< false: the controller will switch the output even if blocking is active, true: the controller wil switch it only when blocking is not active
    uint32_t blockingTimeMs;        ///< How long blocking will stay after switching this output
} tsAcOutputParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevIo *gpioDevice; ///< holds the GPIO device that is used to control the AC output
} tsAcOutputConsts;

/** @brief  Create an AC outout device that will be controlled with an AC output controller
 *  @param  _name   Name of the AC output
 *  @param _gpioDevice the GPIO device that is used to control the AC output
 *  @note _gpioDevice is initialized in the devIoInit function of this device, no need to initialize it before
 */
#define DEV_IO_AC_OUTPUT_CREATE(_name, _gpioDevice) \
    tsAcOutputParams _name##Params =                \
        {                                           \
            .currentState = 0,                      \
            .requestedState = 0,                    \
            .participateInBlocking = false,         \
    };                                              \
    const tsAcOutputConsts _name##Consts =          \
        {                                           \
            .gpioDevice = &_gpioDevice,             \
    };                                              \
    DEV_IO_CREATE(_name, devIoAcOutputFuncs, &_name##Params, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_AC_OUTPUT_H

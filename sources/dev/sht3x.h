/** @file       sht3x.h
 *  @brief      Header file of SHT3X device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-09
 *  @details
 *
 *  This device is used te read temperature and humidity measurements from sht3x sensor over I2C bus.
 *  It consists of 1 core device (as devIo) and two other devices (devIos also) to get temperature and humidity.
 *
 *  The operation of reading the sensor is handled in a process belongs to the core device.
 *  The error status of the operations can be obtained using devIoGet function of the core.
 *  The time interval between two measurements can be set at compile time using the creating macro
 *  and at run time using devIoPut function of the core.
 *
 *  The temperature and humidity measurements are obtained using devIoGet functions of the related devices.
 *  The returned temperature value is the measured temperature in celsius * 10. For example: 23.3 C will be returned as 233.
 *  The returned humidity value is measured humidity in percentage. For example: 65% will be  returned as 65.
 *
 *  @code
 *
 *  // Creation ...
 *  #include "dev/i2c.h"
 *  DEV_COM_I2C_CREATE(i2c, 1, 75000)
 *
 *  #include "dev/sht3x.h"
 *  DEV_IO_SHT3X_CORE_CREATE(sht3xCore, i2c, 0x44, 50)
 *  DEV_IO_SHT3X_TEMP_CREATE(sht3xTemp, sht3xCore)
 *  DEV_IO_SHT3X_HUMIDITY_CREATE(sht3xHum, sht3xCore)
 *
 *  // Initializing ...
 *  devIoInit(&sht3xCore, NULL);
 *
 *
 *  //Usage ...
 *  uint32_t sht3xError = devIoGet(&sht3xCore);
 *  devIoPut(&sht3xCore, 200); // change the interval between two measurements to 200 ms
 *  int32_t temp = devIoGet(&sht3xTemp); // temp = real temp * 10
 *  uint32_t hum = devIoGet(&sht3xHum);
 *
 *
 *  @endcode
 */
#ifndef FILE_SHT3X_H
#define FILE_SHT3X_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_SHT3X_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   SHT3X SHT3X
 *  @ingroup    DEV_CPX
 *  @brief      read humidity and temperature measurements from sht3x sensor
 *  @{
 */

/// @brief  Functions for SHT3X devices
INTERFACE const tsDevIoFuncs devIoSht3xCoreFuncs;
INTERFACE const tsDevIoFuncs devIoSht3xTemperatureFuncs;
INTERFACE const tsDevIoFuncs devIoSht3xHumidityFuncs;

/// @brief  Device specific parameters
typedef struct
{
    tsTarget target;       ///< Device target information on communication device
    tTicket ticket;        ///< Ticket that will be used for I2C communication
    uint8_t buffer[6];     ///< Holds ingoing and outgoing data with the sensor
    uint16_t errorCounter; ///< the count of the errors occurred durring i2c communications
    int32_t temperature;   ///< The obtained temperature value
    uint8_t humidity;      ///< The obtained humidity value
    uint32_t intervalMs;   ///< The interval between two measurements.
} tsSht3xParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevCom *i2c; ///< the i2c device that will be used in communications
    tsProcess *process;  ///< Process that handles the jobs for this device
} tsSht3xConsts;

/** @brief  Create a devIo for sht3xCore
 *  @param  _name   Name of devIo object
 *  @param _slaveAddress SHT3X slave address, can be 0x44 or 0x45
 *  @param _intervalMs interval between two sequential measurements in ms. Read the next note.
 *  @note a single measurement takes about 17 ms to be acquiesced. so the real period between
 *        two measurements will be _intervalMs + 17 ms
 */
#define DEV_IO_SHT3X_CORE_CREATE(_name, _i2c, _slaveAddress, _intervalMs)                                        \
    tsProcess _name##Process;                                                                                    \
    tsSht3xParams _name##Params =                                                                                \
        {                                                                                                        \
            .intervalMs = _intervalMs,                                                                           \
            .target = {.regAdrSize = 2, .slave = _slaveAddress},                                                 \
    };                                                                                                           \
    const tsSht3xConsts _name##Consts =                                                                          \
        {                                                                                                        \
            .i2c = &_i2c,                                                                                        \
            .process = &_name##Process,                                                                          \
    };                                                                                                           \
    PROCESS_CREATE(_name##Process, sht3xProcInit, sht3xProcDeinit, PROCESS_NONE, &_name##Params, &_name##Consts) \
    DEV_IO_CREATE(_name, devIoSht3xCoreFuncs, &_name##Params, &_name##Consts)

/// @brief  Device specific constants
typedef struct
{
    const tsDevIo *core;
} tsSht3xSubDevConsts;

/** @brief  Create a devIo for sht3xTemp
 *  @param  _name   Name of devIo object
 *  @param  _core   the sht3xCore device
 */
#define DEV_IO_SHT3X_TEMP_CREATE(_name, _core) \
    const tsSht3xSubDevConsts _name##Consts =  \
        {                                      \
            .core = &_core,                    \
    };                                         \
    DEV_IO_CREATE(_name, devIoSht3xTemperatureFuncs, NULL, &_name##Consts)

/** @brief  Create a devIo for sht3xHumidity
 *  @param  _name   Name of devIo object
 *  @param  _core   the sht3xCore device
 */
#define DEV_IO_SHT3X_HUMIDITY_CREATE(_name, _core) \
    const tsSht3xSubDevConsts _name##Consts =      \
        {                                          \
            .core = &_core,                        \
    };                                             \
    DEV_IO_CREATE(_name, devIoSht3xHumidityFuncs, NULL, &_name##Consts)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_SHT3X_H

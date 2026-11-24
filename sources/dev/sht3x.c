/** @file       sht3x.c
 *  @brief      C file of SHT3X device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-09
 */
#define FILE_SHT3X_C

#include "sht3x.h"

/**
 *  @addtogroup SHT3X
 *  @{
 */

#define MEASUREMENT_DURATION_MS 17
#define MAX_ALLOWED_ERROR_COUNT 30

static DEV_IO_FUNC_INIT(sht3xCoreInit);
static DEV_IO_FUNC_DEINIT(sht3xCoreDeinit);
static DEV_IO_FUNC_GET(sht3xCoreGetError);
static DEV_IO_FUNC_PUT(sht3xCorePutInterval);

static PT_THREAD(sht3xThread);
static uint8_t calcCrc(uint8_t *dataPtr, size_t dataLength);
static uint8_t reverseByte(uint8_t byte);

/// @brief  Structure that defines the functions for sht3x
const tsDevIoFuncs devIoSht3xCoreFuncs = {sht3xCoreInit, sht3xCoreDeinit, sht3xCoreGetError, sht3xCorePutInterval};

/// @brief  Init function for a sht3xCore
static DEV_IO_FUNC_INIT(sht3xCoreInit)
{
    tsSht3xParams *params = device->parameters;
    const tsSht3xConsts *consts = device->constants;
    UNUSED(config); // REMOVE IF USED

    params->humidity = 0;
    params->temperature = 0;
    params->errorCounter = 0;
    if (!device->sys->initialized)
    {
        device->sys->initialized = 1;
        devComInit(consts->i2c);
        processStart(consts->process);
    }
    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a sht3xCore
static DEV_IO_FUNC_DEINIT(sht3xCoreDeinit)
{
    tsSht3xParams *params = device->parameters;
    const tsSht3xConsts *consts = device->constants;
    UNUSED(params); // REMOVE IF USED

    device->sys->initialized = 0;
    processStop(consts->process);

    return EXIT_SUCCESS;
}

/// @brief Gives the error status of sht3x sensor
static DEV_IO_FUNC_GET(sht3xCoreGetError)
{
    tsSht3xParams *params = device->parameters;
    return params->errorCounter > MAX_ALLOWED_ERROR_COUNT;
}

/// @brief Sets the interval time between 2 measurements
static DEV_IO_FUNC_PUT(sht3xCorePutInterval)
{
    tsSht3xParams *params = device->parameters;
    if (data > 0)
    {
        params->intervalMs = data;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
/****************************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 */

static DEV_IO_FUNC_GET(sht3xTemperatureGet);
static DEV_IO_FUNC_GET(sht3xHumidityGet);

const tsDevIoFuncs devIoSht3xTemperatureFuncs = {NULL, NULL, sht3xTemperatureGet, NULL};
const tsDevIoFuncs devIoSht3xHumidityFuncs = {NULL, NULL, sht3xHumidityGet, NULL};

/// @brief Gives the measured temperature by sht3x sensor in Celsius * 10
static DEV_IO_FUNC_GET(sht3xTemperatureGet)
{
    const tsSht3xSubDevConsts *consts = device->constants;
    tsSht3xParams *coreParams = consts->core->parameters;
    return coreParams->temperature;
}

/// @brief Gives the measured humidity by sht3x sensor in percentage
static DEV_IO_FUNC_GET(sht3xHumidityGet)
{
    const tsSht3xSubDevConsts *consts = device->constants;
    tsSht3xParams *coreParams = consts->core->parameters;
    return coreParams->humidity;
}

/****************************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 */

PROCESS_INIT_PROTO(sht3xProcInit)
{
    process->initialized = 1;
    threadStart(process, sht3xThread);
}

PROCESS_DEINIT_PROTO(sht3xProcDeinit)
{
    process->initialized = 0;
    threadStop(process, sht3xThread);
}
/****************************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 */

static PT_THREAD(sht3xThread)
{
    tsSht3xParams *params = process->parameters;
    const tsSht3xConsts *consts = process->constants;
    PT_BEGIN();

    for (;;)
    {
        /**
         * To read humidity and temperature measurements:
         * 1- Send 2-byte "single shot" command to start the measurement operation
         *    (0x2400) is used = high repeatability and clock stretching disabled
         * 2- wait until the measurement is ready
         *    Max measurement duration in high repeatability mode = 15 ms + (2 ms to be sure)
         * 3- Read 6 bytes from the sensor
         *    received 6 bytes = {temp MSB, temp LSB, temp CRC, hum MSB, hum LSB, hum CRC }
         */

        // start measurement with high repeatability and clock stretching disabled
        params->buffer[0] = 0x24;
        params->buffer[1] = 0x00;

        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->i2c, params->ticket), 5);
        PT_WAIT_WHILE(EXIT_FAILURE == devComOpen(consts->i2c, &params->target));
        devComSend(consts->i2c, params->buffer, 2);
        PT_WAIT_WHILE(devComSend(consts->i2c, NULL, 0));
        // close the channel so other devices can use i2c while the measurement process is ongoing
        devComClose(consts->i2c, &params->target);
        PT_WAIT_WHILE(devComTicketValid(consts->i2c, params->ticket));

        PT_YIELD_TIMEOUT(MEASUREMENT_DURATION_MS);

        memset(&params->buffer, 0, sizeof(params->buffer)); // clear buffer before starting receiving
        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->i2c, params->ticket), 5);
        PT_WAIT_WHILE(EXIT_FAILURE == devComOpen(consts->i2c, &params->target));
        devComReceive(consts->i2c, params->buffer, 6);
        devComClose(consts->i2c, &params->target);
        PT_WAIT_WHILE(devComTicketValid(consts->i2c, params->ticket));

        if (params->buffer[2] != calcCrc(params->buffer, 2) ||
            params->buffer[5] != calcCrc(&params->buffer[3], 2))
        {
            if (params->errorCounter < 0xffff)
                params->errorCounter++;
        }
        else
        {
            int32_t receivedTemp = (int32_t)(((uint32_t)params->buffer[0] << 8) | ((uint32_t)params->buffer[1]));
            receivedTemp = ((1750L * receivedTemp / 65535L) - 450);
            params->temperature = receivedTemp;

            uint32_t receivedHumidity = ((uint32_t)params->buffer[3] << 8) | ((uint32_t)params->buffer[4]);
            receivedHumidity = 100 * receivedHumidity / 65535UL;
            params->humidity = receivedHumidity;
        }

        PT_YIELD_TIMEOUT(params->intervalMs);
    }

    PT_END();
}

#include "libs/crc.h"

/**
 * @brief calculates the crc of an array for sht3x sensor
 *
 * @param dataPtr ptr to the data, input data array
 * @param dataLength the number of bytes of the data (data array)
 * @return uint8_t the calculated crc value
 */
static uint8_t calcCrc(uint8_t *dataPtr, size_t dataLength)
{
    /*
     the used crc function properties:
     poly : 0x31
     init : 0xff
     reflect input : false
     reflect output : false
     --------------------------------
     MAXIM crc function properties:
     poly : 0x31
     init : 0x00
     reflect input : true
     reflect output : true
     --------------------------------
     we can use MAXIM crc table if we use different initial value
     and reflect/reverse the input and the output
    */

    uint8_t input, result = 0xff; // crc initial value
    for (size_t i = 0; i < dataLength; ++i, ++dataPtr)
    {
        input = reverseByte(*dataPtr);
        result = crc8MaximTable[result ^ input];
    }
    return reverseByte(result);
}

/**
 * @brief Reverses bits order in a byte. For example: 0x01 -> 0x80, 0x0F -> 0xF0
 */
static uint8_t reverseByte(uint8_t byte)
{
    // ref: http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith32Bits
    return (((byte * 0x0802LU & 0x22110LU) | (byte * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
}

/** @} */

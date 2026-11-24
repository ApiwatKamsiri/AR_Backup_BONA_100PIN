/** @file       matrixout.c
 *  @brief      Matrix Output Driver
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       9.11.2018
 */
#define FILE_MATRIXOUT_C

/**
 *  @addtogroup MATRIXOUT
 *  @{
 */
#include "matrixout.h"
#include <string.h>

#define MATRIXOUT_COL_REFRESH_TIMEOUT (1)

static DEV_CPX_FUNC_INIT(matrixoutInit);
static DEV_CPX_FUNC_DEINIT(matrixoutDeinit);

const tsDevCpxFuncs devCpxMatrixoutFuncs =
    {
        matrixoutInit,
        matrixoutDeinit,
};

TIMER_CALLBACK_FUNC(matrixoutIsr)
{
    const tsDevCpx *device          = parameter;
    tsMatrixoutParams *params       = device->parameters;
    const tsMatrixoutConsts *consts = device->constants;

    if (consts->encCommon)
    {
        // Collect encoder value
        if (devIoGet(consts->encCommon))
        {
            params->encBuffer |= (1ul << params->colIdx);
        }
    }

    if (++params->colIdx >= consts->colCount + params->brightness)
    {
        // Column scan finished, return to beginning
        params->colIdx = 0;

        if (consts->encCommon)
        {
            // Encoder value finalised
            params->encValue  = params->encBuffer;
            params->encBuffer = 0;
        }
    }

    devIoPut(consts->columns, 0); /// Disable all columns
    devIoPut(consts->rows, 0);    /// Disable all rows
    {
        volatile uint16_t count = consts->deadTime;
        while (count--)
            ;
    }
    if (params->colIdx < consts->colCount)
    {
        devIoPut(consts->rows, consts->raw[params->colIdx]); /// Place data
    }

    devIoPut(consts->columns, (1ul << params->colIdx)); /// Enable a single column

    return MATRIXOUT_COL_REFRESH_TIMEOUT; // Column refresh timeout
}

static DEV_CPX_FUNC_INIT(matrixoutInit)
{
    tsMatrixoutParams *params       = device->parameters;
    const tsMatrixoutConsts *consts = device->constants;
    ADD_RCOS_VERSION("matrixout", "2.0.0")

    UNUSED(config); // REMOVE IF USED

    params->colIdx = 0;
    devIoInit(consts->columns, NULL);
    devIoInit(consts->rows, NULL);
    timerCallBackStart(&params->timerCb, MATRIXOUT_COL_REFRESH_TIMEOUT); // All columns must be visited in 10ms
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_CPX_FUNC_DEINIT(matrixoutDeinit)
{
    tsMatrixoutParams *params       = device->parameters;
    const tsMatrixoutConsts *consts = device->constants;

    device->sys->initialized = 0;
    timerCallBackStop(&params->timerCb);
    params->colIdx = 0;
    memset(consts->raw, 0, consts->colCount * sizeof(uint32_t));
    devIoPut(consts->columns, 0); // Disable all columns
    devIoPut(consts->rows, 0);    // Clear row data

    return EXIT_SUCCESS;
}

uint32_t matrixoutGet(const tsDevCpx *device, uint8_t row, uint8_t col)
{
    const tsMatrixoutConsts *consts = device->constants;

    if (col < consts->colCount)
    {
        return (consts->raw[col] & (1ul << row)) ? 1 : 0;
    }
    else
    {
        return 0;
    }
}

uint8_t matrixoutPut(const tsDevCpx *device, uint8_t row, uint8_t col, uint32_t data)
{
    const tsMatrixoutConsts *consts = device->constants;

    if (col < consts->colCount)
    {
        if (data)
        {
            consts->raw[col] |= 1ul << row;
        }
        else
        {
            consts->raw[col] &= ~(1ul << row);
        }
    }

    return EXIT_SUCCESS;
}

uint32_t matrixoutGetRow(const tsDevCpx *device, uint8_t row)
{
    const tsMatrixoutConsts *consts = device->constants;
    uint32_t retVal                 = 0;
    uint8_t idx;

    for (idx = 0; idx < consts->colCount; idx++)
    {
        retVal |= (consts->raw[idx] & (1ul << row)) ? (1ul << idx) : 0;
    }

    return retVal;
}

uint8_t matrixoutPutRow(const tsDevCpx *device, uint8_t row, uint32_t data)
{
    const tsMatrixoutConsts *consts = device->constants;
    uint8_t idx;

    for (idx = 0; idx < consts->colCount; idx++)
    {
        if (data & 1)
        {
            consts->raw[idx] |= 1ul << row;
        }
        else
        {
            consts->raw[idx] &= ~(1ul << row);
        }
        data >>= 1;
    }

    return EXIT_SUCCESS;
}

uint32_t matrixoutGetCol(const tsDevCpx *device, uint8_t col)
{
    const tsMatrixoutConsts *consts = device->constants;

    if (col < consts->colCount)
    {
        return consts->raw[col];
    }
    else
    {
        return 0;
    }
}

uint8_t matrixoutPutCol(const tsDevCpx *device, uint8_t col, uint32_t data)
{
    const tsMatrixoutConsts *consts = device->constants;

    if (col < consts->colCount)
    {
        consts->raw[col] = data;

        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

/** @} */

/**
 *  @addtogroup MATRIXOUT_GPIO
 *  @{
 */
static DEV_IO_FUNC_GET(matrixoutGpioGet);
static DEV_IO_FUNC_PUT(matrixoutGpioPut);

const tsDevIoFuncs devIoMatrixoutGpioFuncs =
    {
        NULL,
        NULL,
        matrixoutGpioGet,
        matrixoutGpioPut,
};

static DEV_IO_FUNC_GET(matrixoutGpioGet)
{
    const tsMatrixoutGpioConsts *consts = device->constants;

    return matrixoutGet(consts->matrixout, consts->row, consts->col);
}

static DEV_IO_FUNC_PUT(matrixoutGpioPut)
{
    const tsMatrixoutGpioConsts *consts = device->constants;

    return matrixoutPut(consts->matrixout, consts->row, consts->col, data);
}

/** @} */

/**
 *  @addtogroup MATRIXOUT_PORT
 *  @{
 */
static DEV_IO_FUNC_GET(matrixoutPortGet);
static DEV_IO_FUNC_PUT(matrixoutPortPut);

const tsDevIoFuncs devIoMatrixoutPortFuncs =
    {
        NULL,
        NULL,
        matrixoutPortGet,
        matrixoutPortPut,
};

static DEV_IO_FUNC_GET(matrixoutPortGet)
{
    const tsMatrixoutPortConsts *consts = device->constants;

    uint32_t data = 0;
    uint32_t mask = 1;

    for (int idx = 0; idx < consts->pinCount; idx++)
    {
        if (matrixoutGet(consts->matrixout, consts->pins[idx].row, consts->pins[idx].col))
        {
            data |= mask;
        }
        mask <<= 1;
    }

    return data;
}

static DEV_IO_FUNC_PUT(matrixoutPortPut)
{
    const tsMatrixoutPortConsts *consts = device->constants;

    for (int idx = 0; idx < consts->pinCount; idx++)
    {
        matrixoutPut(consts->matrixout, consts->pins[idx].row, consts->pins[idx].col, data & 1);
        data >>= 1;
    }

    return EXIT_SUCCESS;
}

/** @} */

/**
 *  @addtogroup MATRIXOUT_ENCODER
 *  @{
 */
static DEV_IO_FUNC_GET(matrixoutEncGet);

const tsDevIoFuncs devIoMatrixoutEncFuncs =
    {
        NULL,
        NULL,
        matrixoutEncGet,
        NULL,
};

static DEV_IO_FUNC_GET(matrixoutEncGet)
{
    const tsMatrixoutEncConsts *consts = device->constants;

    uint32_t encValue = ((tsMatrixoutParams *)consts->matrixout->parameters)->encValue;
    uint32_t data     = 0;
    uint32_t mask     = 1;
    uint8_t idx;

    for (idx = 0; idx < consts->encDecodeCnt; idx++)
    {
        uint32_t check = 1ul << consts->encDecodeLut[idx];

        data |= (encValue & check) ? mask : 0;
        mask <<= 1;
    }

    if (consts->gray)
    {
        return grayDecode(data);
    }
    else
    {
        return data;
    }
}

/** @} */

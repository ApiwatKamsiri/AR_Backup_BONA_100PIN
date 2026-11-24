/** @file       lerp.c
 *  @brief      C file of LERP device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Özkan Akgül
 *  @date       16.07.2019
 */
#define FILE_LERP_C

/**
 *  @addtogroup LERP
 *  @{
 */
#include "lerp.h"

static DEV_IO_FUNC_INIT(lerpInit);
static DEV_IO_FUNC_DEINIT(lerpDeinit);
static DEV_IO_FUNC_GET(lerpGet);
static DEV_IO_FUNC_PUT(lerpPut);

const tsDevIoFuncs devIoLerpFuncs =
    {
        lerpInit,
        lerpDeinit,
        lerpGet,
        lerpPut,
};

static DEV_IO_FUNC_INIT(lerpInit)
{
    const tsLerpConsts *consts = device->constants;
    ADD_RCOS_VERSION("lerp", "2.0.0")

    UNUSED(config);
    device->sys->initialized = 1;

    return devIoInit(consts->devX, NULL);
}

static DEV_IO_FUNC_DEINIT(lerpDeinit)
{
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(lerpGet)
{
    const tsLerpConsts *consts = device->constants;
    int32_t readValue    = devIoGet(consts->devX);
    uint16_t idx; // Index where the readValue is located

    if ((readValue < consts->table[0].x) ||
        (readValue > consts->table[consts->tableSize - 1].x))
    {
        // Read value should not be lesser than minimum value or bigger than maximum value
        return 0;
    }

    for (idx = consts->tableSize - 2; consts->table[idx].x > readValue; idx--) // idx point has bigger value than readValue
        ;

    {
        float x1, y1; // point 1
        float x2, y2; // point 2

        x1 = consts->table[idx].x;
        y1 = consts->table[idx].y;
        x2 = consts->table[idx + 1].x;
        y2 = consts->table[idx + 1].y;

        // a = ((y2 - y1) / (x2 - x1));
        // b = (y1 - a * x1);
        x2 = (y2 - y1) / (x2 - x1); // a
        y2 = y1 - x2 * x1; // b

        return (int32_t)(x2 * readValue + y2); // y = a * x + b
    }
}

static DEV_IO_FUNC_PUT(lerpPut)
{
    const tsLerpConsts *consts = device->constants;
    uint16_t idx;        // Index where the readValue is located
    int correlation = 0; // Correlation

    if (consts->table[0].y < consts->table[consts->tableSize - 1].y)
    {
        // Positive correlation
        if (((int32_t)data < (consts->table[0].y)) ||
            ((int32_t)data > (consts->table[consts->tableSize - 1].y)))
        {
            // Input value should not be lesser than minimum value or bigger than maximum value
            return 0;
        }
        correlation = 1;
        idx         = 1;
    }
    else
    {
        // Negative correlation
        if (((int32_t)data < (consts->table[consts->tableSize - 1].y)) ||
            ((int32_t)data > (consts->table[0].y)))
        {
            // Input value should not be lesser than minimum value or bigger than maximum value
            return 0;
        }
        correlation = -1;
        idx         = consts->tableSize - 2;
    }

    for (; (int32_t)data > consts->table[idx].y; idx += correlation) // idx point has bigger value than data
        ;

    {
        float x1, y1; // point 1
        float x2, y2; // point 2

        x1 = consts->table[idx].x;
        y1 = consts->table[idx].y;
        x2 = consts->table[idx - correlation].x;
        y2 = consts->table[idx - correlation].y;

        // a = ((y2 - y1) / (x2 - x1));
        // b = (y1 - a * x1);
        x2 = (y2 - y1) / (x2 - x1); // a
        y2 = y1 - x2 * x1; // b

        return devIoPut(consts->devX, ((float)((int32_t)data) - y2) / x2); // x = (y - b) / a
    }
}

/** @} */

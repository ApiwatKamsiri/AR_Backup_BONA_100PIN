/** @file       mean.c
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
#define FILE_MEAN_C

/**
 *  @addtogroup MEAN
 *  @{
 */
#include "mean.h"

static DEV_IO_FUNC_INIT(meanInit);
static DEV_IO_FUNC_DEINIT(meanDeinit);
static DEV_IO_FUNC_GET(meanGet);

const tsDevIoFuncs devIoMeanFuncs =
    {
        meanInit,
        meanDeinit,
        meanGet,
        NULL,
};

/// @brief Collecting raw data Timer Callback Function
TIMER_CALLBACK_FUNC(rawDataGetTcb)
{//If period is set, raw datas will be collected in tcb. Otherwise it will be collected when the meanGet function called.
    const tsDevIo *device = parameter;
    const tsMeanConsts *consts = device->constants;
    uint32_t readValue;

    dequePopFront(consts->queue, &readValue, sizeof(readValue)); // Remove one
    readValue = devIoGet(consts->target);
    dequePushBack(consts->queue, &readValue, sizeof(readValue)); // Add a new one

    return consts->period;
}

static DEV_IO_FUNC_INIT(meanInit)
{
    tsMeanParams *params = device->parameters;
    const tsMeanConsts *consts = device->constants;
    ADD_RCOS_VERSION("mean", "2.0.1")

    UNUSED(config); // REMOVE IF USED

    devIoInit(consts->target, NULL);
    dequeFlush(consts->queue);
    while (!DEQUE_IS_FULL(consts->queue))
    {
        uint32_t readValue = devIoGet(consts->target);

        dequePushBack(consts->queue, &readValue, sizeof(readValue));
    }
    if(consts->period)
    {//If period is set, tcb will be started to collect raw data
        params->tcb.parameter = (void *)device;
        timerCallBackStart(&params->tcb, consts->period);
    }
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(meanDeinit)
{
    tsMeanParams *params = device->parameters;
    const tsMeanConsts *consts = device->constants;

    dequeFlush(consts->queue);
    if(consts->period)
    {//If period was set, tcb had been started. So it will be stopped.
        params->tcb.parameter = NULL;
        timerCallBackStop(&params->tcb);
    }
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(meanGet)
{
    const tsMeanConsts *consts = device->constants;

    if(consts->period == 0)
    {//If period is set, raw datas will be collected in tcb. Otherwise it will be collected when the meanGet function called.
        uint32_t readValue;

        dequePopFront(consts->queue, &readValue, sizeof(readValue)); // Remove one
        readValue = devIoGet(consts->target);
        dequePushBack(consts->queue, &readValue, sizeof(readValue)); // Add a new one
    }

    // Calculate average of all values inside the queue
    uint32_t *buffer = (uint32_t *)consts->queue->buffer;
    int count        = consts->count;
    uint64_t total   = 0;
    uint32_t minVal  = (uint32_t)-1ul;
    uint32_t maxVal  = 0;

    for (; count; count--)
    {
        uint32_t prevVal = *buffer++;

        if (consts->disExt)
        {
            // Remember extreme values
            if (prevVal > maxVal)
            {
                maxVal = prevVal;
            }
            if (prevVal < minVal)
            {
                minVal = prevVal;
            }
        }
        total += prevVal;
    }

    if (consts->disExt)
    {
        // Remove extreme values from calculation
        total -= minVal;
        total -= maxVal;
        return total / (consts->count - 2);
    }
    else
    {
        return total / consts->count;
    }
}

/** @} */

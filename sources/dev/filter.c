/** @file       filter.c
 *  @brief      C file of FILTER device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-06-02
 */
#define FILE_FILTER_C

#include "filter.h"

/**
 *  @addtogroup FILTER
 *  @{
 */

static DEV_IO_FUNC_INIT(filterInit);
static DEV_IO_FUNC_DEINIT(filterDeinit);
static DEV_IO_FUNC_GET(filterGet);
static void sortData(int32_t data[], size_t size);

/// @brief  Structure that defines the functions for filter
const tsDevIoFuncs devIoFilterFuncs =
    {
        filterInit,
        filterDeinit,
        filterGet,
        NULL,
};

/// @brief  Init function for a filter
static DEV_IO_FUNC_INIT(filterInit)
{
    tsFilterParams *params = device->parameters;
    const tsFilterConsts *consts = device->constants;
    UNUSED(config); // REMOVE IF USED

    params->firstReadDone = false;
    dequeFlush(consts->rawDataQueue);
    devIoInit(consts->target, NULL);

    device->sys->initialized = 1;
    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a filter
static DEV_IO_FUNC_DEINIT(filterDeinit)
{
    const tsFilterConsts *consts = device->constants;

    devIoDeinit(consts->target);
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

/// @brief  Get function for a filter
static DEV_IO_FUNC_GET(filterGet)
{
    tsFilterParams *params = device->parameters;
    const tsFilterConsts *consts = device->constants;
  
    int32_t data = devIoGet(consts->target);
    if (!params->firstReadDone)
    {
        // fill the queue with the first read value
        params->firstReadDone = true;
        for (size_t i = 0; i < consts->filterSize; ++i)
            dequePushBack(consts->rawDataQueue, &data, sizeof(int32_t));
        params->filteredData = data;
        return data;
    }
    //int32_t fsize = (int32_t)consts->filterSize;           //cadd
    
    // dequeue will always be not empty no need to check
    dequeRemFront(consts->rawDataQueue, sizeof(int32_t));                                // remove oldest value
    dequePushBack(consts->rawDataQueue, &data, sizeof(int32_t));                         // add the new one
    //int32_t *buffer = (int32_t *)malloc(/*consts->filterSize*/fsize * sizeof(int32_t));                     // allocate a temp buffer
    int32_t bufferA[20];
    memcpy(bufferA, consts->rawDataQueue->buffer, consts->filterSize * sizeof(int32_t)); // copy the data to it
    sortData(bufferA, consts->filterSize);                                               // order the data

    // find the mean value of the values in the center
    int64_t sum = 0;
    size_t meanStartIdx = (consts->filterSize / 2) - (consts->meanCount / 2);
    for (size_t i = meanStartIdx; i < meanStartIdx + consts->meanCount; ++i)
        sum += bufferA[i];
    sum = (sum + (consts->meanCount / 2)) / consts->meanCount; // round the result of the dividing operation

   // free(buffer); // no need to buffer any more

    // find the average of the old filtered data and the new one
    sum = (sum + params->filteredData) / 2;
    params->filteredData = sum;
    return (params->filteredData);
}

/// @brief sort an array of int32_t values using bubble sort algo
static void sortData(int32_t data[], size_t size)
{
    bool dataReordered=false;
    do
    {
        dataReordered = false;
        for (size_t i = 0; i < size - 1; ++i)
        {
            if (data[i] > data[i + 1])
            {
                SWAP(data[i], data[i + 1]);
                dataReordered = true;
            }
        }
    } while (dataReordered);
}

/** @} */

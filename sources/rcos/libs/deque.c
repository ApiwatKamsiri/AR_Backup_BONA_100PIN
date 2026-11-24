/** @file       deque.c
 *  @brief      Generic structures and functions to create double ended queues
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       12.10.2018
 */
#define FILE_DEQUE_C

/** @addtogroup DEQUE
 *  @{
 */

/*********************************************************************
 *  INCLUDES
 */
#include <string.h>
#include "deque.h"
#include "version.h"

/*********************************************************************
 *  LOCAL CONSTANTS
 */

/*********************************************************************
 *  LOCAL TYPEDEFS
 */

/*********************************************************************
 *  LOCAL MACROS
 */

/*********************************************************************
 *  LOCAL VARIABLES
 */

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */
static void dequeRead(tsDeque *deq, uint16_t idx, void *destination, uint16_t size);
static void dequeWrite(tsDeque *deq, uint16_t idx, const void *source, uint16_t size);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */

/** @brief  Updates the indexes only
 *          ( can be used to fix DMA transfer counts )
 *  @param  deq pointer
 *  @param  size of increment needed
 */

void addDequeVersion()
{
    ADD_RCOS_VERSION("deque", "2.0.1")
}

void dequeAddBack(tsDeque *deq, uint16_t size)
{
    // Update back index
    deq->back += size;
    if (deq->back >= deq->size)
    {
        deq->back -= deq->size;
    }

    // Update size
    deq->count += size;
    if (deq->count > deq->size) // Overwrite
    {
        deq->count = deq->size;
        deq->front = deq->back; // Update front index to drop excess data
    }
}

/** @brief  Updates the indexes only
 *          ( can be used to fix DMA transfer counts )
 *  @param  deq pointer
 *  @param  size of increment needed
 */
void dequeAddFront(tsDeque *deq, uint16_t size)
{
    // Update front index
    if (deq->front < size)
    {
        deq->front += deq->size - size;
    }
    else
    {
        deq->front -= size;
    }

    // Update size
    deq->count += size;
    if (deq->count > deq->size) // Overwrite
    {
        deq->count = deq->size;
        deq->back  = deq->front; // Drop data from other-side
    }
}

/** @brief  Updates the indexes only
 *          ( can be used to fix DMA transfer counts )
 *  @param  deq pointer
 *  @param  size of decrement needed
 */
void dequeRemBack(tsDeque *deq, uint16_t size)
{
    if (deq->count <= size)
    {
        deq->count = 0;
        deq->back  = deq->front;
    }
    else
    {
        deq->count -= size;
        if (deq->back < size)
        {
            deq->back += deq->size;
        }
        deq->back -= size;
    }
}

/** @brief  Updates the indexes only
 *          ( can be used to fix DMA transfer counts )
 *  @param  deq pointer
 *  @param  size of decrement needed
 */
void dequeRemFront(tsDeque *deq, uint16_t size)
{
    if (deq->count <= size)
    {
        deq->count = 0;
        deq->front = deq->back;
    }
    else
    {
        deq->count -= size;
        deq->front += size;
        if (deq->front >= deq->size)
        {
            deq->front -= deq->size;
        }
    }
}

void dequeFlush(tsDeque *deq)
{
    deq->count = 0;
    deq->back  = 0;
    deq->front = 0;
}

void dequeClear(tsDeque *deq)
{
    dequeFlush(deq);
    memset(deq->buffer, 0, deq->size);
}

uint16_t dequePopBack(tsDeque *deq, void *destination, uint16_t size)
{
    dequeRemBack(deq, size);
    dequeRead(deq, deq->back, destination, size);
    return size;
}

uint16_t dequePopFront(tsDeque *deq, void *destination, uint16_t size)
{
    dequeRead(deq, deq->front, destination, size);
    dequeRemFront(deq, size);
    return size;
}

uint16_t dequePushBack(tsDeque *deq, const void *source, uint16_t size)
{
    dequeWrite(deq, deq->back, source, size);
    dequeAddBack(deq, size);
    return size;
}

uint16_t dequePushFront(tsDeque *deq, const void *source, uint16_t size)
{
    dequeAddFront(deq, size);
    dequeWrite(deq, deq->front, source, size);
    return size;
}

void dequePeek(tsDeque *deq, uint16_t index, void *destination, uint16_t size)
{
    uint32_t target = index + deq->front;

    if (target >= deq->size)
    {
        target -= deq->size;
    }

    dequeRead(deq, target, destination, size);
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @brief  Sub-level read function
 *  @param  deq pointer
 *  @param  idx index to read from
 *  @param  destination buffer pointer
 *  @param  size of data to read from deq
 */
static void dequeRead(tsDeque *deq, uint16_t idx, void *destination, uint16_t size)
{
    uint8_t *destPtr = destination;
    uint16_t untilTheEnd;

    untilTheEnd = deq->size - idx; // Data size until end of physical buffer

    if (untilTheEnd >= size)
    {
        memcpy(destPtr, &deq->buffer[idx], size); // Copy required data
    }
    else
    {
        memcpy(destPtr, &deq->buffer[idx], untilTheEnd);                // Copy until end of physical buffer
        memcpy(&destPtr[untilTheEnd], deq->buffer, size - untilTheEnd); // Copy the rest from beginning of physical buffer
    }
}

/** @brief  Sub-level write function
 *  @param  deq pointer
 *  @param  idx index to write to
 *  @param  source buffer pointer
 *  @param  size of data to write into deq
 */
static void dequeWrite(tsDeque *deq, uint16_t idx, const void *source, uint16_t size)
{
    const uint8_t *srcPtr = source;
    uint16_t untilTheEnd;

    untilTheEnd = deq->size - idx;

    if (untilTheEnd >= size)
    {
        memcpy(&deq->buffer[idx], srcPtr, size);
    }
    else
    {
        memcpy(&deq->buffer[idx], srcPtr, untilTheEnd);
        memcpy(deq->buffer, &srcPtr[untilTheEnd], size - untilTheEnd);
    }
}

/** @} */

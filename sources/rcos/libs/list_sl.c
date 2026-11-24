/**
 *  @file       list_sl.c
 *  @brief      Singly Linked List structures
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       19.10.2018
 */
#define FILE_LIST_SL_C

/** @addtogroup LIST_SL
 *  @{
 */

/*********************************************************************
 *  INCLUDES
 */
#include "list_sl.h"
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

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */

void addListslVersion()
{
    ADD_RCOS_VERSION("listsl", "2.0.0")
}


void lslInsertAfter(tsLsl *list, void *item, void *newItem)
{
    ((tsLslItem *)newItem)->next = ((tsLslItem *)item)->next;
    ((tsLslItem *)item)->next    = newItem;
    ++list->count;
}

void lslInsertBefore(tsLsl *list, void *item, void *newItem)
{
    tsLslItem *liItem;

    LIST_SL_FOREACH(liItem, list)
    {
        if (liItem->next == item) // Find the one before *item
        {
            ((tsLslItem *)newItem)->next = item;
            liItem->next                 = newItem;
            ++list->count;
            break;
        }
    }
}

void lslInsertHead(tsLsl *list, void *newItem)
{
    ((tsLslItem *)newItem)->next = list->next;
    list->next                   = newItem;
    ++list->count;
}

void lslInsertTail(tsLsl *list, void *newItem)
{
    tsLslItem *liItem;

    LIST_SL_FOREACH(liItem, list)
    {
        if (liItem->next == NULL) // Find the last one
        {
            liItem->next = newItem;
            ++list->count;
            return;
        }
    }
    lslInsertHead(list, newItem);
}

void lslDelete(tsLsl *list, void *item)
{
    tsLslItem *liItem;

    LIST_SL_FOREACH(liItem, list)
    {
        if (liItem->next == item) // Find the one before *item
        {
            liItem->next              = ((tsLslItem *)item)->next;
            ((tsLslItem *)item)->next = NULL;
            --list->count;
            break;
        }
    }
}

void *lslPopHead(tsLsl *list)
{
    tsLslItem *liItem = list->next;

    if (liItem)
    {
        list->next   = liItem->next;
        liItem->next = NULL;
        --list->count;
    }

    return liItem;
}

void *lslPopTail(tsLsl *list)
{
    tsLslItem *liItem, *lastItem;

    LIST_SL_FOREACH(liItem, list)
    {
        lastItem = liItem->next;
        if (lastItem)
        {
            if (!lastItem->next)
            {
                // Next item is the last one
                // Remove lastItem from list
                liItem->next = NULL;
                --list->count;
                return lastItem;
            }
        }
        else
        {
            // List has only one element
            list->next  = NULL;
            list->count = 0;
            return liItem;
        }
    }

    return NULL; // List was empty
}

void *lslTailFrom(void *item)
{
    tsLslItem *liItem;

    for (liItem = (tsLslItem *)item; liItem->next != NULL; liItem = liItem->next)
        ;

    return liItem;
}

uint32_t lslRecount(tsLsl *list)
{
    uint32_t count = 0;

    if (list->next != NULL)
    {
        tsLslItem *liItem;

        LIST_SL_FOREACH(liItem, list)
        {
            count++;
        }
    }

    return count;
}

void *lslItemAtIdx(tsLsl *list, uint32_t idx)
{
    if (idx >= list->count)
    {
        return NULL;
    }
    else
    {
        tsLslItem *liItem;

        for (liItem = list->next; idx--; liItem = liItem->next)
            ;

        return liItem;
    }
}

uint32_t lslIdxOfItem(tsLsl *list, void *item)
{
    tsLslItem *liItem;
    uint32_t idx = 0;

    for (liItem = list->next; liItem != NULL; liItem = liItem->next)
    {
        if (liItem == item)
        {
            return idx;
        }
        idx++;
    }

    return (uint32_t)-1ul;
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

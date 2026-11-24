/** @file       json.c
 *  @brief      Functions to print-out data in json format
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       06.07.2020
 */
#define FILE_JSON_C

/** @addtogroup JSON
 *  @{
 */

/*********************************************************************
 *  INCLUDES
 */
#include "json.h"
#include "version.h"
#include <stdarg.h>
#include <stdio.h>

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
static uint8_t firstItem = 1;
static int (*jsonPrint)(const char *format, ...);

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */

/*********************************************************************
 *  LOCAL FUNCTIONS
 */
void jsonInit(int (*printf)(const char *format, ...))
{
    ADD_RCOS_VERSION("json", "2.0.0")

    jsonPrint = printf;
    firstItem = 1;
}

/* Obj */
void jsonObjOpen(const char *key)
{
    if (key)
    {
        jsonPrint("%s\"%s\":{", firstItem ? "" : ",", key);
    }
    else
    {
        jsonPrint("%s{", firstItem ? "" : ",", key);
    }
    firstItem = 1;
}

void jsonObjClose(void)
{
    jsonPrint("}");
    firstItem = 0;
}

/* Array */
void jsonArrOpen(const char *key)
{
    if (key)
    {
        jsonPrint("%s\"%s\":[", firstItem ? "" : ",", key);
    }
    else
    {
        jsonPrint("%s[", firstItem ? "" : ",");
    }
    firstItem = 1;
}

void jsonArrClose(void)
{
    jsonPrint("]");
    firstItem = 0;
}

/* Value */
void jsonString(const char *key, const char *string)
{
    if (key)
    {
        jsonPrint("%s\"%s\":\"%s\"", firstItem ? "" : ",", key, string);
    }
    else
    {
        jsonPrint("%s\"%s\"", firstItem ? "" : ",", string);
    }
    if (firstItem)
    {
        firstItem = 0;
    }
}

void jsonText(const char *key, const char *format, ...)
{
    char array[256]; // warning! long text may overflow
    int retVal = 0;
    va_list args;

    va_start(args, format);

    retVal = vsnprintf(array, sizeof(array), format, args);

    va_end(args);

    jsonString(key, array);
}

void jsonNumber(const char *key, uint32_t number)
{
    if (key)
    {
        jsonPrint("%s\"%s\":%d", firstItem ? "" : ",", key, number);
    }
    else
    {
        jsonPrint("%s%d", firstItem ? "" : ",", number);
    }
    if (firstItem)
    {
        firstItem = 0;
    }
}

void jsonFloat(const char *key, float number, uint8_t precision)
{
    if (key)
    {
        jsonPrint("%s\"%s\":%.*f", firstItem ? "" : ",", key, precision, number);
    }
    else
    {
        jsonPrint("%s%.*f", firstItem ? "" : ",", precision, number);
    }
    if (firstItem)
    {
        firstItem = 0;
    }
}

void jsonBool(const char *key, uint8_t bool)
{
    if (key)
    {
        jsonPrint("%s\"%s\":%c", firstItem ? "" : ",", key, bool ? '1' : '0');
    }
    else
    {
        jsonPrint("%s%c", firstItem ? "" : ",", bool ? '1' : '0');
    }
    if (firstItem)
    {
        firstItem = 0;
    }
}

void jsonNull(const char *key)
{
    if (key)
    {
        jsonPrint("%s\"%s\":null", firstItem ? "" : ",", key);
    }
    else
    {
        jsonPrint("%snull", firstItem ? "" : ",");
    }
    if (firstItem)
    {
        firstItem = 0;
    }
}

/** @} */

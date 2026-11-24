/** @file       debug.c
 *  @brief      Print debug string
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Rüştü Irklı
 *  @author     Deniz Fer
 *  @date       18.01.2018
 *  @date       6.12.2018
 */
#define FILE_DEBUG_C

#include "debug.h"
#include "libs/libs.h"
#include "version.h"

/** @addtogroup DEBUG
 *  @{
 */

void addDebugVersion()
{
    ADD_RCOS_VERSION("debug", "2.0.0")
}

void debugDump(int (*printf)(const char *format, ...), const void *address, size_t length)
{
    const uint8_t *ptr = (address);
    uint16_t total     = (length);

#define BYTE_PER_LINE (16)
    // "XXXX: xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx  ................"

    while (total) // Display a line
    {
        printf("%04x: ", (uint32_t)ptr - (uint32_t)address); // address offset

        for (int idx = 0; idx < BYTE_PER_LINE; idx++) // HEX presentation
        {
            if (idx >= total)
            {
                printf("   ");
            }
            else
            {
                printf(" %02x", ptr[idx]);
            }
        }

        printf("  "); // Seperator

        for (int idx = 0; idx < BYTE_PER_LINE; idx++) // Graphical presentation
        {
            if (idx >= total)
            {
                printf(" ");
            }
            else
            {
                if (!isgraph(ptr[idx]))
                {
                    printf(".");
                }
                else
                {
                    printf("%c", ptr[idx]);
                }
            }
        }

        printf("\r\n"); // EOL

        ptr += BYTE_PER_LINE;
        total -= MIN(BYTE_PER_LINE, total);
    }
}

/** @} */

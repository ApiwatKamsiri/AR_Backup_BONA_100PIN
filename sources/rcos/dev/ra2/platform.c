/** @file       ra2/platform.c
 *  @brief      Platform specific modifications for Renesas RA2 series MCUs
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     xx
 *  @date       xx
 *  @author     Burak OZAN
 *  @date       09.09.2022
 */
#define FILE_PLATFORM_C

/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"
#include <string.h>
#include "bsp_api.h"

#ifndef RCOS_PLATFORM_RA2
#error this file is prepared for RA2 series Renesas mcu only
#endif

/** @addtogroup RA2
 *  @{
 */

/*********************************************************************
 *  EXPORTED TYPEDEFS
 */

/*********************************************************************
 *  EXPORTED MACROS
 */

/*********************************************************************
 *  INTERFACES: CONSTANTS
 */

/*********************************************************************
 *  INTERFACES: VARIABLES
 */
FSP_CRITICAL_SECTION_DEFINE;

/*********************************************************************
 *  INTERFACES: FUNCTIONS
 */
void platformInit(void)
{
    ADD_RCOS_VERSION("ra2/platform", "1.0.3")
}

void globalIntEnable(void)
{
    FSP_CRITICAL_SECTION_EXIT;
}

void globalIntDisable(void)
{
    FSP_CRITICAL_SECTION_ENTER;
}

teBool isIsrActive(void)
{
    return 0xff;
}

void getUniqId(uint8_t *ptr, uint8_t length)
{
    ///< Returns the 128-bit unique id of the device by loading them into the integer array pointed to by ptr.
    bsp_unique_id_t const *id = R_BSP_UniqueIdGet();
    memcpy(ptr, id, length);
}

uint32_t ntohl(uint32_t data)
{
    return htonl(data);
}

uint32_t htonl(uint32_t _32)
{
    return ((((_32)&0xfful) << 24) |
            (((_32)&0xff00ul) << 8) |
            (((_32)&0xff0000ul) >> 8) |
            (((_32)&0xff000000ul) >> 24));
}

uint16_t ntohs(uint16_t _16)
{
    return htons(_16);
}

uint16_t htons(uint16_t _16)
{
    return ((((_16)&0xfful) << 8) | (((_16)&0xff00ul) >> 8));
}

/** @} */

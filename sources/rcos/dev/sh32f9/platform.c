/** @file       sh32F9/platform.c
 *  @brief      Platform specific modifications for Sinowealth SH32F9 series MCUs
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ozkan AKGUL
 *  @date       17.01.2023
 */
#define FILE_PLATFORM_C

/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"
#include <string.h>

#ifndef RCOS_PLATFORM_SH32F9
#error this file is prepared for SH32F9 series Sinowealth mcu only
#endif

/** @addtogroup SH32F9
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

/*********************************************************************
 *  INTERFACES: FUNCTIONS
 */
void platformInit(void)
{
    ADD_RCOS_VERSION("sh32f9/platform", "1.0.0")
}

void globalIntEnable(void)
{
    __enable_irq(); // Clear PRIMASK;
}

void globalIntDisable(void)
{
    __disable_irq(); // Set PRIMASK
}

teBool isIsrActive(void)
{
    return 0xff;
}

void getUniqId(uint8_t *ptr, uint8_t length)
{
    ///< Returns the 128-bit unique id of the device by loading them into the integer array pointed to by ptr.
    /// TODO : add unique id 
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

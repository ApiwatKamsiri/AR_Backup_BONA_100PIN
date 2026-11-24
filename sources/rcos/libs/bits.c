/** @file       bits.c
 *  @brief      Bit manipulations
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       22.05.2019
 */
#define FILE_BITS_C

/** @addtogroup BITS
 *  @{
 */

/*********************************************************************
 *  INCLUDES
 */
#include "bits.h"
#include "version.h"

/*********************************************************************
 *  LOCAL CONSTANTS
 */
const uint8_t lutBitCount[256]   = BIT_COUNT_LOOKUP_TABLE;
const uint8_t lutDeBruijnMsb[32] = DE_BRUIJN_MSB;
const uint8_t lutDeBruijnLsb[32] = DE_BRUIJN_LSB;

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
void addBitsVersion()
{
    ADD_RCOS_VERSION("bits", "2.0.0")
}

uint8_t find_msb(uint32_t v)
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return lutDeBruijnMsb[(v * 0x07C4ACDDu) >> 27];
}

#if 1
uint8_t find_lsb(uint32_t v)
{
    uint8_t r;

    r = lutDeBruijnLsb[((uint32_t)((v & -v) * 0x077CB531U)) >> 27];

    return r;
}
#else
uint8_t find_lsb(uint32_t value)
{
    uint32_t less = value - 1;

    value = (less | value) ^ less;

    return find_msb(value);
}
#endif

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

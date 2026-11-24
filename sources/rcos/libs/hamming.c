/** @file       hamming.c
 *  @brief      Hamming code conversion for error detection and correction
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       9.05.2019
 */
#define FILE_HAMMING_C

/** @addtogroup HAMMING
 *  @{
 */

/*********************************************************************
 *  INCLUDES
 */
#include "hamming.h"
#include "porting.h"
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

/// @brief  Convert 4-bit data into Hamming code
const uint8_t hammingArray[16] = HAMMING_8_4_MSB;

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */

void addHammingVersion()
{
    ADD_RCOS_VERSION("hamming", "2.0.0")
}

uint16_t hammingEncodeByte(uint8_t data)
{
    uint16_t retVal = 0;

    retVal |= hammingEncode(data);                     /// Low half
    retVal |= (uint16_t)hammingEncode(data >> 4) << 8; /// High half

    return htons(retVal);
}

uint16_t hammingDecodeByte(uint16_t encoded)
{
    uint16_t retVal = 0;

    encoded = ntohs(encoded);

    retVal |= hammingDecode(encoded & 0xff); /// Low half
    if (retVal > 0xf)
    {
        return 0xffff; // error detected
    }
    retVal |= hammingDecode(encoded >> 8) << 4; /// High half

    return retVal;
}

void hammingEncodeArray(uint16_t *destination, const uint8_t *source, uint32_t count)
{
    for (; count; count--)
    {
        *destination = hammingEncodeByte(*source);
        destination++;
        source++;
    }
}

uint8_t hammingDecodeArray(uint8_t *destination, const uint16_t *source, uint32_t count)
{
    uint16_t decoded = 0;
    uint8_t retVal = EXIT_SUCCESS;

    for (; count; count--)
    {
        decoded = hammingDecodeByte(*source);
        if (decoded > 0xff)
        {
            *destination = 0;
            retVal = EXIT_FAILURE;
        }
        else
        {
            *destination = decoded;
        }
        destination++;
        source++;
    }

    return retVal;
}

uint8_t hammingEncode(uint8_t data)
{
    return hammingArray[data & 0x0f];
}

uint8_t hammingDecode(uint8_t data)
{
    uint8_t retVal;

    for (retVal = 0; retVal < ARRAY_SIZE(hammingArray); retVal++)
    {
        if (data == hammingArray[retVal])
        {
            return retVal; /// Return found value
        }
        else if (BIT_COUNT(data ^ hammingArray[retVal]) == 1)
        {
            return retVal; /// Fix 1-bit error
        }
    }

    return 0xff; /// There're multiple errors
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

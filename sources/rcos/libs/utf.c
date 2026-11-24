/** @file       utf.c
 *  @brief      Unicode Transformation Format
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       10.09.2019
 */
#define FILE_UTF_C

#include "utf.h"
#include "bits.h"
#include "version.h"

/** @addtogroup UTF8
 *  @{
 */

void addUtfVersion()
{
    ADD_RCOS_VERSION("utf", "2.0.0")
}


#define _UTF8_BITS_FOR_1_BYTE (7)   ///< Maximum number of bits that can be represented by 1 byte encoding
#define _UTF8_BITS_FOR_2_BYTES (11) ///< Maximum number of bits that can be represented by 2 bytes encoding
#define _UTF8_BITS_FOR_3_BYTES (16) ///< Maximum number of bits that can be represented by 3 bytes encoding
#define _UTF8_BITS_FOR_4_BYTES (21) ///< Maximum number of bits that can be represented by 4 bytes encoding

#define _UTF8_LAST_VALUE_FOR_1_BYTE (BIT(_UTF8_BITS_FOR_1_BYTE) - 1)   ///< Maximum value that can be represented by 1 byte encoding
#define _UTF8_LAST_VALUE_FOR_2_BYTES (BIT(_UTF8_BITS_FOR_2_BYTES) - 1) ///< Maximum value that can be represented by 2 bytes encoding
#define _UTF8_LAST_VALUE_FOR_3_BYTES (BIT(_UTF8_BITS_FOR_3_BYTES) - 1) ///< Maximum value that can be represented by 3 bytes encoding
#define _UTF8_LAST_VALUE_FOR_4_BYTES (0x10FFFF)                        ///< Maximum value that can be represented by 4 bytes encoding

#define _UTF8_DATA_BITS_REM_FOR_REST (6)                                                              ///< Number of data bits remaining for each byte after the first
#define _UTF8_DATA_BITS_REM_FOR_1_BYTE _UTF8_BITS_FOR_1_BYTE                                          ///< Number of data bits remaining for 1 byte encoding
#define _UTF8_DATA_BITS_REM_FOR_2_BYTES (_UTF8_BITS_FOR_2_BYTES - _UTF8_DATA_BITS_REM_FOR_REST)       ///< Number of data bits remaining for the first byte of 2 bytes encoding
#define _UTF8_DATA_BITS_REM_FOR_3_BYTES (_UTF8_BITS_FOR_3_BYTES - (_UTF8_DATA_BITS_REM_FOR_REST * 2)) ///< Number of data bits remaining for the first byte of 3 bytes encoding
#define _UTF8_DATA_BITS_REM_FOR_4_BYTES (_UTF8_BITS_FOR_4_BYTES - (_UTF8_DATA_BITS_REM_FOR_REST * 3)) ///< Number of data bits remaining for the first byte of 4 bytes encoding

#define _UTF8_DEF_REST (0x80)    ///< Definitive value of each byte after the first
#define _UTF8_DEF_1_BYTE (0x00)  ///< Definitive value of first byte of 1 byte encoding
#define _UTF8_DEF_2_BYTES (0xc0) ///< Definitive value of first byte of 2 bytes encoding
#define _UTF8_DEF_3_BYTES (0xe0) ///< Definitive value of first byte of 3 bytes encoding
#define _UTF8_DEF_4_BYTES (0xf0) ///< Definitive value of first byte of 4 bytes encoding

uint8_t utf8Size(uint32_t codePoint)
{
    if (codePoint <= _UTF8_LAST_VALUE_FOR_1_BYTE)
    {
        return 1;
    }
    else if (codePoint <= _UTF8_LAST_VALUE_FOR_2_BYTES)
    {
        return 2;
    }
    else if (codePoint <= _UTF8_LAST_VALUE_FOR_3_BYTES)
    {
        return 3;
    }
    else if (codePoint <= _UTF8_LAST_VALUE_FOR_4_BYTES)
    {
        return 4;
    }
    else
    {
        return 0; // Out of range
    }
}

uint8_t *utf8Encode(uint8_t *destination, uint32_t codePoint)
{
    uint8_t size = utf8Size(codePoint);

    if (size == 0)
    {
        return destination;
    }

    // Place least significant bytes first
    switch (size)
    {
        case 4:
            destination[3] = _UTF8_DEF_REST | (codePoint & (BIT(_UTF8_DATA_BITS_REM_FOR_REST) - 1));
            codePoint >>= _UTF8_DATA_BITS_REM_FOR_REST;
            // break; // Fall-through intentional
        case 3:
            destination[2] = _UTF8_DEF_REST | (codePoint & (BIT(_UTF8_DATA_BITS_REM_FOR_REST) - 1));
            codePoint >>= _UTF8_DATA_BITS_REM_FOR_REST;
            // break; // Fall-through intentional
        case 2:
            destination[1] = _UTF8_DEF_REST | (codePoint & (BIT(_UTF8_DATA_BITS_REM_FOR_REST) - 1));
            codePoint >>= _UTF8_DATA_BITS_REM_FOR_REST;
            break;
        case 1:
            // No follow-up bytes
            break;
        default:
            // Can not be encoded
            break;
    }

    // Place first byte for definition
    switch (size)
    {
        case 4:
            destination[0] = _UTF8_DEF_4_BYTES | (codePoint & (BIT(_UTF8_DATA_BITS_REM_FOR_4_BYTES) - 1));
            break;
        case 3:
            destination[0] = _UTF8_DEF_3_BYTES | (codePoint & (BIT(_UTF8_DATA_BITS_REM_FOR_3_BYTES) - 1));
            break;
        case 2:
            destination[0] = _UTF8_DEF_2_BYTES | (codePoint & (BIT(_UTF8_DATA_BITS_REM_FOR_2_BYTES) - 1));
            break;
        case 1:
            destination[0] = _UTF8_DEF_1_BYTE | (codePoint & (BIT(_UTF8_DATA_BITS_REM_FOR_1_BYTE) - 1));
            break;
        default:
            // Can not be encoded
            break;
    }

    return (destination + size);
}

uint32_t utf8Decode(const uint8_t *textArr, const uint8_t **nextChar)
{
    uint32_t retVal     = '?';
    uint8_t followUpCnt = 0;

    // Check preceding byte to determine encoding size
    if ((*textArr & ~(BIT(_UTF8_DATA_BITS_REM_FOR_4_BYTES) - 1)) == _UTF8_DEF_4_BYTES)
    {
        retVal      = *textArr++ & (BIT(_UTF8_DATA_BITS_REM_FOR_4_BYTES) - 1);
        followUpCnt = 3;
    }
    else if ((*textArr & ~(BIT(_UTF8_DATA_BITS_REM_FOR_3_BYTES) - 1)) == _UTF8_DEF_3_BYTES)
    {
        retVal      = *textArr++ & (BIT(_UTF8_DATA_BITS_REM_FOR_3_BYTES) - 1);
        followUpCnt = 2;
    }
    else if ((*textArr & ~(BIT(_UTF8_DATA_BITS_REM_FOR_2_BYTES) - 1)) == _UTF8_DEF_2_BYTES)
    {
        retVal      = *textArr++ & (BIT(_UTF8_DATA_BITS_REM_FOR_2_BYTES) - 1);
        followUpCnt = 1;
    }
    else if ((*textArr & ~(BIT(_UTF8_DATA_BITS_REM_FOR_1_BYTE) - 1)) == _UTF8_DEF_1_BYTE)
    {
        retVal = *textArr++ & (BIT(_UTF8_DATA_BITS_REM_FOR_1_BYTE) - 1);
    }
    else
    {
        // Undefined
        textArr++;
    }

    // Check succeeding bytes if there's any
    while (followUpCnt--)
    {
        if ((*textArr & ~(BIT(_UTF8_DATA_BITS_REM_FOR_REST) - 1)) == _UTF8_DEF_REST)
        {
            retVal <<= _UTF8_DATA_BITS_REM_FOR_REST;
            retVal |= *textArr++ & (BIT(_UTF8_DATA_BITS_REM_FOR_REST) - 1);
        }
        else
        {
            retVal = '?'; // Undefined
            break;        // while (followUpCnt--)
        }
    }

    // There might be more bytes according to first encoded byte. Fix it if the rest failed.
    textArr += followUpCnt;

    if (nextChar)
    {
        *nextChar = textArr;
    }

    return retVal;
}

/** @} */

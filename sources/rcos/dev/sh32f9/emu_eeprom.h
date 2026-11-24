/** @file       SH32F9/emu_eeprom.h
 *  @brief      Emulator for an EEPROM
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Cihan YILDIRIM
 *  @date       14.06.2023
 */
#ifndef FILE_EMU_EEPROM_H
#define FILE_EMU_EEPROM_H

#include "rcos.h"

/**
 *  @defgroup   SH32F9_EMU_EEPROM EMU_EEPROM
 *  @ingroup    SH32F9_DEV_MEM
 *  @brief      Emulator for an EEPROM
 *  @{
 */

#ifndef FILE_EMU_EEPROM_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/// @brief  Functions for EMU_EEPROM devices
INTERFACE const tsDevMemFuncs devMemEmuEepromFuncs;

/// @brief  Device specific parameters
typedef struct
{
    // cy_en_em_eeprom_status_t (*init)(uint32 startAddress); ///< Init function of specific EEPROM emulator
    const uint8_t *blockList;
    const uint8_t blockListSize;
    const uint16_t blockSize;
} tsEmuEepromConsts;

/** @brief      Create a devMem EMU_EEPROOM for target area in memory
 *  @warning    Does not check target area size
 *  @param      _name Name of devMem object
 *  @param      _blockSize Size of eeprom block
 *  @param      ... Block numbers in eeprom
 */
#define DEV_MEM_EMU_ADDRESS_CREATE(_name, _blockSize, ...) \
    const uint8_t _name##BlockList[] =                                    \
        {                                                                 \
            __VA_ARGS__,                                                  \
    };                                                                    \
    const tsEmuEepromConsts _name##Consts =                               \
        {                                                                 \
            .blockList = _name##BlockList,                                \
            .blockListSize = ARRAY_SIZE(_name##BlockList),                \
            .blockSize = _blockSize,                                      \
    };                                                                    \
    DEV_MEM_CREATE(_name, devMemEmuEepromFuncs, NULL, &_name##Consts)

//.init         = _cyName##_Init,
//.context      = &_cyName##_context,

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_EMU_EEPROM_H

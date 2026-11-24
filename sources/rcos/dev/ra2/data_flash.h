/** @file       data_flash.h
 *  @brief      Header file of FLASH device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Vehbi Umur Cabuk
 *  @date       05.05.2021
 *  @author     Burak OZAN
 *  @date       05.05.2022
 */
#ifndef FILE_DATA_FLASH_H
#define FILE_DATA_FLASH_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_DATA_FLASH_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

#ifdef R_CGC_CFG_H_
#define DATA_FLASH_CGC_ACTIVE
#endif

/**
 *  @defgroup   RA2_DATA_FLASH FLASH
 *  @ingroup    RA2_DEV_CPX
 *  @brief      RENESAS RA2 Data Flash access
 *  @{
 */

/// @brief  Functions for FLASH devices
INTERFACE const tsDevCpxFuncs devCpxFlashFuncs;

/// @brief  Device specific parameters
typedef struct
{
    const flash_instance_t *config;
#ifdef DATA_FLASH_CGC_ACTIVE
    const cgc_instance_t *cgc;  ///< CGC instance
#endif
} tsFlashParams;

#ifdef DATA_FLASH_CGC_ACTIVE
/** @brief  Create a devCpx object to access internal flash of RA2
 *  @param  _name   Name of devCpx object
 *  @param  _stackName   Name of Renesas flash stack
 *  @param  _cgcStackName   Name of Renesas cgc stack
 */
#define DEV_CPX_FLASH_CREATE(_name, _stackName, _cgcStackName) \
    tsFlashParams _name##Params =                              \
        {                                                      \
            .config = &_stackName,                             \
            .cgc    = &_cgcStackName,                          \
    };                                                         \
    DEV_CPX_CREATE(_name, devCpxFlashFuncs, &_name##Params, NULL)

#else
/** @brief  Create a devCpx object to access internal flash of RA2
 *  @param  _name   Name of devCpx object
 *  @param  _stackName   Name of Renesas flash stack
 */
#define DEV_CPX_FLASH_CREATE(_name, _stackName) \
    tsFlashParams _name##Params =               \
        {                                       \
            .config = &_stackName,              \
    };                                          \
    DEV_CPX_CREATE(_name, devCpxFlashFuncs, &_name##Params, NULL)

#endif

/** @} */

/**
 *  @defgroup   RA2_SECTION SECTION
 *  @ingroup    RA2_DEV_MEM
 *  @ingroup    RA2_DATA_FLASH
 *  @brief      RENESAS RA2 Data Flash section as a devMem
 *  @{
 */
/// @brief  Functions for SECTION devices
INTERFACE const tsDevMemFuncs devMemSectionFuncs;

/// @brief  Device specific parameters
typedef struct
{
    const tsDevCpx *flash;
    tsFlashParams *fparams;
    const uint8_t *area;
    uint8_t *sectorBuffer;
    uint16_t areaSize;
    uint16_t sectorSize;
} tsSectionParams;

/** @brief      Create a devMem object for a SECTION of KE04
 *  @warning    Macro assumes MCUxpresso style section naming
 *  @param      _name           Name of devMem object
 *  @param      _sectionName    Name of target memory section(must be a "string")
 *  @param      _size           Size of section
 *  @param      _flash          Flash core device
 */
#define DEV_MEM_SECTION_CREATE(_name, _sectionName, _size, _flash) \
    PLATFORM_SECTION(_sectionName)                                 \
    PLATFORM_ALIGNED(BSP_FEATURE_FLASH_LP_DF_BLOCK_SIZE)           \
    const uint8_t _name##PhysicalArea[(_size)] = {0};              \
    uint8_t _name##SectorBuffer[_size];                            \
    tsSectionParams _name##Params =                                \
        {                                                          \
            &_flash,                                               \
            &_flash##Params,                                       \
            _name##PhysicalArea,                                   \
            _name##SectorBuffer,                                   \
            ARRAY_SIZE(_name##PhysicalArea),                       \
            ARRAY_SIZE(_name##SectorBuffer),                       \
    };                                                             \
    DEV_MEM_CREATE(_name, devMemSectionFuncs, &_name##Params, NULL)

/** @} */

#undef INTERFACE // Should not let this roam free

#endif // FILE_DATA_FLASH_H

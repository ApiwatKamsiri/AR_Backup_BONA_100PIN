/** @file       cpxvirtual.h
 *  @brief      Creating virtual complex device with no function
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Erdeniz Dalyan
 *  @date       27.09.2022
 */
#ifndef FILE_CPXVIRTUAL_H
#define FILE_CPXVIRTUAL_H

#include "rcos.h"

/**
 *  @defgroup   CPXVIRTUAL CPXVIRTUAL
 *  @ingroup    DEV_CPX
 *  @brief      A complex device to use as dummy complex device
 *  @{
 */

#ifndef FILE_CPXVIRTUAL_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/// @brief  Functions for CPXVIRTUAL devices
INTERFACE const tsDevCpxFuncs devCPXvirtualFuncs;

/** @brief      Create a devCpx
 *  @details    A complex device to use as dummy complex device
 *  @param      _name   Name of devCpx object
 */
#define DEV_CPX_VIRTUAL_CREATE(_name) \
    uint32_t _name##Value;            \
    DEV_CPX_CREATE(_name, devCPXvirtualFuncs, NULL, NULL)

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_CPXVIRTUAL_H

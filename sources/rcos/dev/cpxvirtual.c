/** @file       cpxvirtual.c
 *  @brief      Creating virtual complex device with no function
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Erdeniz Dalyan
 *  @date       27.09.2022
 */
#define FILE_CPXVIRTUAL_C

/**
 *  @addtogroup CPXVIRTUAL
 *  @{
 */
#include "cpxvirtual.h"

static DEV_CPX_FUNC_INIT(cpxvirtualInit);
static DEV_CPX_FUNC_DEINIT(cpxvirtualDeinit);

const tsDevCpxFuncs devCPXvirtualFuncs =
    {
        cpxvirtualInit,
        cpxvirtualDeinit,
};

static DEV_CPX_FUNC_INIT(cpxvirtualInit)
{
    ADD_RCOS_VERSION("cpxvirtual", "1.0.0")

    UNUSED(config); // REMOVE IF USED
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_CPX_FUNC_DEINIT(cpxvirtualDeinit)
{
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

/** @} */

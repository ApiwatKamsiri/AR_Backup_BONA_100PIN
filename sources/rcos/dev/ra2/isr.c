/** @file       ra2/isr.c
 *  @brief      RENESAS RA2L1 Interrupt Sub-Routine driver
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Dilasude Göktürk
 *  @date       01.10.2022
 *  @author     Burak OZAN
 *  @date       08.11.2022
 */
#define FILE_ISR_C

/**
 *  @addtogroup RA2_ISR
 *  @{
 */
#include "isr.h"

static DEV_CPX_FUNC_INIT(isrInit);
static DEV_CPX_FUNC_DEINIT(isrDeinit);

const tsDevCpxFuncs devCpxIsrFuncs =
    {
        isrInit,
        isrDeinit,
};

void isrCallbackInterface(external_irq_callback_args_t *p_args, const tsDevCpx *device)
{
    tsIsrParams *params = device->parameters;

    if(params->callbackFunc != NULL)
    {//Calling the callback given when device init
        params->callbackFunc();
    }
}

static DEV_CPX_FUNC_INIT(isrInit)
{
    const tsIsrConsts *consts = device->constants;
    tsIsrParams *params = device->parameters;
    ADD_RCOS_VERSION("ra2/isr", "2.0.1")

    if (config)
    {
        params->callbackFunc = (isrCallbackFuncPtrType)config;//The function will be called when isr function called
        R_ICU_ExternalIrqOpen(consts->isr->p_ctrl, consts->isr->p_cfg);
        R_ICU_ExternalIrqEnable(consts->isr->p_ctrl);
        device->sys->initialized = 1;
        return EXIT_SUCCESS;
    }
    else
    {
        device->sys->initialized = 0;
	    R_ICU_ExternalIrqDisable(consts->isr->p_ctrl);
        R_ICU_ExternalIrqClose(consts->isr->p_ctrl);
        params->callbackFunc = NULL;
        return EXIT_FAILURE;
    }
}

static DEV_CPX_FUNC_DEINIT(isrDeinit)
{
    const tsIsrConsts *consts = device->constants;
    tsIsrParams *params = device->parameters;

    device->sys->initialized = 0;
	R_ICU_ExternalIrqDisable(consts->isr->p_ctrl);
    R_ICU_ExternalIrqClose(consts->isr->p_ctrl);
    params->callbackFunc = NULL;

    return EXIT_SUCCESS;
}

/** @} */

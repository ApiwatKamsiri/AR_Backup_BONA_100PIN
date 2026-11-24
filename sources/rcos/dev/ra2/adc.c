/** @file       adc.c
 *  @brief      C file of ADC device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       4/9/2021
 */
#define FILE_ADC_C

#include "adc.h"

/**
 *  @addtogroup ADC
 *  @{
 */

static DEV_CPX_FUNC_INIT(adcCoreInit);
static DEV_CPX_FUNC_DEINIT(adcCoreDeinit);
TIMER_CALLBACK_FUNC(adcCoreRefresh);

const tsDevCpxFuncs devCpxAdcCoreFuncs =
    {
        adcCoreInit,
        adcCoreDeinit,
};

TIMER_CALLBACK_FUNC(adcCoreRefresh)
{
    const tsDevCpx *device        = parameter;
    const tsAdcCoreConsts *consts = device->constants;
    fsp_err_t err = FSP_SUCCESS;

    err = R_ADC_ScanStart(consts->adc->p_ctrl);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

    return consts->refreshTime;
}

static DEV_CPX_FUNC_INIT(adcCoreInit)
{
    tsAdcCoreParams *params       = device->parameters;
    const tsAdcCoreConsts *consts = device->constants;
    fsp_err_t err = FSP_SUCCESS;

    ADD_RCOS_VERSION("ra2/adc", "1.0.1")

    UNUSED(config);

    err = R_ADC_Open(consts->adc->p_ctrl, consts->adc->p_cfg);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    err = R_ADC_ScanCfg(consts->adc->p_ctrl, consts->adc->p_channel_cfg);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

    if (consts->refreshTime == 0)
    {
        err = R_ADC_ScanStart(consts->adc->p_ctrl);
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    }
    else
    {
        timerCallBackStart(&params->timer, consts->refreshTime);
    }
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_CPX_FUNC_DEINIT(adcCoreDeinit)
{
    const tsAdcCoreConsts *consts = device->constants;
    tsAdcCoreParams *params       = device->parameters;
    fsp_err_t err = FSP_SUCCESS;

    device->sys->initialized = 0;

    err = R_ADC_Close(consts->adc->p_ctrl);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

    timerCallBackStop(&params->timer);

    return EXIT_SUCCESS;
}

void adcCoreIrqHandler(const tsDevCpx *device)
{
    tsAdcCoreParams *params       = device->parameters;
    const tsAdcCoreConsts *consts = device->constants;
    fsp_err_t err = FSP_SUCCESS;

    for (uint8_t idx = 0; idx < consts->chListSize; idx++)
    {
        err = R_ADC_Read(consts->adc->p_ctrl, (adc_channel_t)consts->chList[idx], &(params->chValues[idx]));
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    }

    if (consts->refreshTime == 0)
    {
        err = R_ADC_ScanStart(consts->adc->p_ctrl);
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    }
}

static uint32_t adcCoreGet(const tsDevCpx *adcCore, uint16_t index)
{
    const tsAdcCoreConsts *consts = adcCore->constants;
    tsAdcCoreParams *params       = adcCore->parameters;

    if (index < consts->chListSize)
    {
        return params->chValues[index];
    }
    else
    {
        return 0;
    }
}

/** @} */

/** @addtogroup RA2_ADC
 *  @{
 */

static DEV_IO_FUNC_GET(adcGet);

const tsDevIoFuncs devIoAdcFuncs =
    {
        NULL,
        NULL,
        adcGet,
        NULL,
};

static DEV_IO_FUNC_GET(adcGet)
{
    const tsAdcParams *params = device->parameters;
    const tsAdcConsts *consts = device->constants;

    return adcCoreGet(consts->adcCore, params->index);
}

/** @} */

/** @file       pwm.c
 *  @brief      C file of PWM device
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       4/12/2021
 *  @author     Burak OZAN
 *  @date       09/09/2022
 */
#define FILE_PWM_C

#include "pwm.h"

/**
 *  @addtogroup PWM
 *  @{
 */

static void pwmSet(const tsDevIo *device)
{
    tsPwmCoreParams *params       = device->parameters;
    const tsPwmCoreConsts *consts = device->constants;
    fsp_err_t err = FSP_SUCCESS;
  
    if (params->freq && params->duty)
    {
        uint32_t period = ((BSP_STARTUP_PCLKD_HZ/(uint16_t) (pow(2,(double) consts->timer->p_cfg->source_div))) / params->freq); //**updated** GPT uses PCLKD not PCLKB, and we need to change formula divide prescaler to eliminate manuel changes after changed RASC Build operation
        err = consts->timer->p_api->periodSet(consts->timer->p_ctrl, period);
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
        err = consts->timer->p_api->dutyCycleSet(consts->timer->p_ctrl, (period * params->duty) / 100, consts->pin);
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    }
    else
    {
        err = consts->timer->p_api->dutyCycleSet(consts->timer->p_ctrl, 0, consts->pin);
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    }
    err = consts->timer->p_api->start(consts->timer->p_ctrl);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
}

static DEV_CPX_FUNC_INIT(pwmInit);
static DEV_CPX_FUNC_DEINIT(pwmDeinit);

/// @brief  Structure that defines the functions for pwm
const tsDevCpxFuncs devCpxPwmCoreFuncs =
    {
        pwmInit,
        pwmDeinit,
};

/// @brief  Init function for a pwm
static DEV_CPX_FUNC_INIT(pwmInit)
{
    const tsPwmCoreConsts *consts = device->constants;
    fsp_err_t err = FSP_SUCCESS;

    ADD_RCOS_VERSION("ra2/pwm", "1.0.1")

    UNUSED(config);

    // Initialize the timer
    err = consts->timer->p_api->open(consts->timer->p_ctrl, consts->timer->p_cfg);
    RCOS_ASSERT((FSP_SUCCESS == err) || (FSP_ERR_ALREADY_OPEN == err), RASSERT_PRI_LOW);

    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a pwm
static DEV_CPX_FUNC_DEINIT(pwmDeinit)
{
    const tsPwmCoreConsts *consts = device->constants;
    fsp_err_t err = FSP_SUCCESS;

    // Disable timer
    err = consts->timer->p_api->close(consts->timer->p_ctrl);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);

    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(pwmDutyGet);
static DEV_IO_FUNC_PUT(pwmDutyPut);

const tsDevIoFuncs devIoPwmDutyFuncs =
    {
        NULL,
        NULL,
        pwmDutyGet,
        pwmDutyPut,
};

static DEV_IO_FUNC_GET(pwmDutyGet)
{
    tsPwmCoreParams *params = device->parameters;

    return params->duty;
}

static DEV_IO_FUNC_PUT(pwmDutyPut)
{
    tsPwmCoreParams *params       = device->parameters;
   // const tsPwmCoreConsts *consts = device->constants;

    params->duty = data > 100 ? 100 : data;

    pwmSet(device);

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(pwmFreqGet);
static DEV_IO_FUNC_PUT(pwmFreqPut);

const tsDevIoFuncs devIoPwmFreqFuncs =
    {
        NULL,
        NULL,
        pwmFreqGet,
        pwmFreqPut,
};

static DEV_IO_FUNC_GET(pwmFreqGet)
{
    tsPwmCoreParams *params = device->parameters;

    return params->freq;
}

static DEV_IO_FUNC_PUT(pwmFreqPut)
{
    tsPwmCoreParams *params       = device->parameters;
   // const tsPwmCoreConsts *consts = device->constants;

    params->freq = data;

    pwmSet(device);

    return EXIT_SUCCESS;
}
/** @} */

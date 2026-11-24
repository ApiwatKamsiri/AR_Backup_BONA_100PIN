/** @file       enc_inc_qd.c
 *  @brief      C file of ENC_INC_QD device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Atilla Osman Mert Gemalmaz
 *  @date       01.07.2019
 */
#define FILE_ENC_INC_QD_C

#include "enc_inc_qd.h"

/**
 *  @addtogroup ENC_INC_QD
 *  @{
 */

static DEV_IO_FUNC_INIT(encIncQdInit);
static DEV_IO_FUNC_DEINIT(encIncQdDeinit);
static DEV_IO_FUNC_GET(encIncQdGet);

/// @brief  Structure that defines the functions for encIncQd
const tsDevIoFuncs devIoEncIncQdFuncs =
    {
        encIncQdInit,
        encIncQdDeinit,
        encIncQdGet,
        NULL,
};

TIMER_CALLBACK_FUNC(encIncQdDecoder)
{
    const tsDevIo *device    = parameter;
    tsEncIncQdParams *params = device->parameters;
    uint32_t tempVal         = devIoGet(params->sigA);

    if (tempVal == devIoGet(params->sigB))
    {
        // Encoder is stable
        if (params->lastSignal != tempVal)
        {
            // Encoder changed position
            params->lastSignal = tempVal;
            tempVal            = devIoGet(params->quadDec);

            if (tempVal >= (params->resetValue + params->noise))
            {
                params->gEncoderCounter++;
            }
            else if (tempVal <= (params->resetValue - params->noise))
            {
                params->gEncoderCounter--;
            }
            devIoPut(params->quadDec, params->resetValue);
        }
        else
        {
            // Encoder did not change position or one of the signal is lost
        }
    }
    else
    {
        // Encoder is in transition or stuck in between
    }

    return params->encoderTimerCallback;
}

/// @brief  Init function for a encIncQd
static DEV_IO_FUNC_INIT(encIncQdInit)
{
    tsEncIncQdParams *params = device->parameters;
    ADD_RCOS_VERSION("encincqd","2.0.0")

    UNUSED(config);

    params->gEncoderCounter = 0; // init Encoder counter
    devIoInit(params->sigA, NULL);
    devIoInit(params->sigB, NULL);
    if (devIoGet(params->sigA) == devIoGet(params->sigB))
    {
        // Encoder stable
        params->lastSignal = devIoGet(params->sigA);
    }
    else
    {
        // Encoder not stable
    }

    devIoInit(params->quadDec, NULL);
    devIoPut(params->quadDec, params->resetValue);

    timerCallBackStart(&params->timerEnc, params->encoderTimerCallback);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a encIncQd
static DEV_IO_FUNC_DEINIT(encIncQdDeinit)
{
    tsEncIncQdParams *params = device->parameters;

    device->sys->initialized = 0;
    timerCallBackStop(&params->timerEnc);

    return EXIT_SUCCESS;
}

/// @brief  Get function for a encIncQd
static DEV_IO_FUNC_GET(encIncQdGet)
{
    tsEncIncQdParams *params = device->parameters;
    uint32_t retVal          = params->gEncoderCounter;
    params->gEncoderCounter  = 0;

    return retVal;
}

/** @} */

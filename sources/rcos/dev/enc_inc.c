/** @file       enc_inc.c
 *  @brief      C file of ENC_INC device
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Deniz Fer
 *  @date       15.01.2019
 *  @author     Umit SAHIN / Burak OZAN
 *  @date       04.11.2022
 */
#define FILE_ENC_INC_C

/**
 *  @addtogroup ENC_INC
 *  @{
 */
#include "enc_inc.h"

#define ENC_INT_PERIOD (50)
#define ENC_INT_STATE_CHANGE_INTERVAL (1)
#define ENC_INT_DEBOUNCE_PERIOD (1)
#define ENC_INT_DEBOUNCE (5)

static DEV_IO_FUNC_INIT(encincInit);
static DEV_IO_FUNC_DEINIT(encincDeinit);
static DEV_IO_FUNC_GET(encincGet);
static DEV_IO_FUNC_PUT(encincPut);

const tsDevIoFuncs devIoEncincFuncs =
    {
        encincInit,
        encincDeinit,
        encincGet,
        encincPut,
};

void encincCheckIsr(const tsDevIo *device)
{
    tsEncIncParams *params       = device->parameters;

    if (params->intState == ENC_INC_STATE_IDLE)
    {
	  	params->intState = ENC_INC_STATE_INTERRUPT;
    }
    
    timerCallBackStart(&params->timer, 1);
}

TIMER_CALLBACK_FUNC(encoderCheck)
{
    const tsDevCpx *device   = parameter;
    tsEncIncParams *params       = device->parameters;
    const tsEncIncConsts *consts = device->constants;

    uint8_t valueA, valueB, callBackPeriod = ENC_INT_PERIOD;

    switch (params->intState)
    {
        case ENC_INC_STATE_IDLE:
        {
            timerCallBackStop(&params->timer);
        }
        break;

        case ENC_INC_STATE_INTERRUPT: // rising or falling edge interrupt is occured.
        {
            params->initialA = devIoGet(consts->signalA) ? 1 : 0;
            params->initialB = devIoGet(consts->signalB) ? 1 : 0;

            if (params->initialB) // false alarm
            {
                params->intState = ENC_INC_STATE_IDLE;
            }
            else // correct edge interrupt
            {
                params->countDeb           = ENC_INT_DEBOUNCE;
                params->intState = ENC_INC_STATE_INTERRUPT_CONSISTENCY;
            }

            callBackPeriod = ENC_INT_STATE_CHANGE_INTERVAL;
        }
        break;

        case ENC_INC_STATE_INTERRUPT_CONSISTENCY: // rising or falling edge interrupt is occured.
        {
            valueA = devIoGet(consts->signalA) ? 1 : 0;
            valueB = devIoGet(consts->signalB) ? 1 : 0;

            if ((valueB == params->initialB) && (valueA == params->initialA))
            {
                if (--params->countDeb == 0)
                {
                    params->intState = ENC_INC_STATE_INTERRUPT_APPROVED;
                }
                else
                {
                    params->intState = ENC_INC_STATE_INTERRUPT_CONSISTENCY;
                }
                
                callBackPeriod   = ENC_INT_DEBOUNCE_PERIOD;
            }
            else
            {
                params->intState = ENC_INC_STATE_IDLE;
            }
        }
        break;

        case ENC_INC_STATE_INTERRUPT_APPROVED:
        {
            if (params->initialB == params->initialA)
            {
                if(consts->polarity == ENC_INC_POS_POLARITY)
                {
                    params->count--; // counter-clock-wise
                }
                else if(consts->polarity == ENC_INC_NEG_POLARITY)
                {
                    params->count++; // counter-clock-wise
                }
            }
            else
            {
                if(consts->polarity == ENC_INC_POS_POLARITY)
                {
                    params->count++; // clock-wise
                }
                else if(consts->polarity == ENC_INC_NEG_POLARITY)
                {
                    params->count--; // clock-wise
                }
            }

            params->intState = ENC_INC_STATE_IDLE;
        }
        break;

        default:
        {
            params->intState = ENC_INC_STATE_IDLE;
        }
        break;
    }

    return callBackPeriod;
}

static DEV_IO_FUNC_INIT(encincInit)
{
    tsEncIncParams *params       = device->parameters;
    const tsEncIncConsts *consts = device->constants;
    ADD_RCOS_VERSION("encinc", "2.0.1")

    UNUSED(config); // REMOVE IF USED

    devIoInit(consts->signalA, NULL);
    devIoInit(consts->signalB, NULL);
    devCpxInit(consts->isrA, (void *)consts->isrFunc);
    device->sys->initialized = 1;
    params->intState = ENC_INC_STATE_IDLE;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(encincDeinit)
{
    tsEncIncParams *params       = device->parameters;
    const tsEncIncConsts *consts = device->constants;

    device->sys->initialized = 0;
    devCpxDeinit(consts->isrA);
    devIoDeinit(consts->signalA);
    devIoDeinit(consts->signalB);
    timerCallBackStop(&params->timer);
    params->count = 0;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(encincGet)
{
    tsEncIncParams *params = device->parameters;
    uint32_t retVal        = (uint32_t)params->count;
    params->count          = 0;

    return retVal;
}

static DEV_IO_FUNC_PUT(encincPut)
{
    tsEncIncParams *params = device->parameters;

    params->count = data;

    return EXIT_SUCCESS;
}

/** @} */

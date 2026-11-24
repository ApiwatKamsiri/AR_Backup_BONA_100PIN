/** @file       psoc4/adc.c
 *  @brief      CYPRESS PSOC4 Analog-Digital Conversion
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Enes İnanç
 *  @date       11.01.2019
 */
#define FILE_ADC_C

#include "adc.h"
#include "sh32f9xx_sb0.h"

/** @addtogroup SH32F9_ADC_CORE
 *  @{
 */

volatile uint16_t ADC_value;

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
    const tsDevCpx *device = parameter;
    const tsAdcCoreConsts *consts = device->constants;

    ADC_SoftwareStartConversion(consts->ADCx);

    return consts->refreshTime;
}

static DEV_CPX_FUNC_INIT(adcCoreInit)
{
    tsAdcCoreParams *params = device->parameters;
    const tsAdcCoreConsts *consts = device->constants;
    ADD_RCOS_VERSION("sh32f9/adc", "1.0.0")

    UNUSED(config); // REMOVE IF USED

    if(consts->chListSize <= 8 && !device->sys->initialized) // ADC can be measured max 8 channel.
    {
        // consts->startStop(consts->ADCx, SH_ON);
        
        for (uint8_t idx = 0; idx < consts->chListSize; idx++)
        {
            switch (consts->chList[idx])
            {
            case ADC_Channel_0:
            {
                params->port = GPIOA;
                params->pin = GPIO_Pin_0;
            }
            break;
            case ADC_Channel_1:
            {
                params->port = GPIOA;
                params->pin = GPIO_Pin_1;
            }
            break;
            case ADC_Channel_2:
            {
                params->port = GPIOA;
                params->pin = GPIO_Pin_2;
            }
            break;
            case ADC_Channel_3:
            {
                params->port = GPIOA;
                params->pin = GPIO_Pin_4;
            }
            break;
            case ADC_Channel_4:
            {
                params->port = GPIOB;
                params->pin = GPIO_Pin_0;
            }
            break;
            case ADC_Channel_5:
            {
                params->port = GPIOB;
                params->pin = GPIO_Pin_1;
            }
            break;
            case ADC_Channel_6:
            {
                params->port = GPIOB;
                params->pin = GPIO_Pin_2;
            }
            break;
            case ADC_Channel_7:
            {
                params->port = GPIOB;
                params->pin = GPIO_Pin_3;
            }
            break;
            case ADC_Channel_8:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_2;
            }
            break;
            case ADC_Channel_9:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_3;
            }
            break;
            case ADC_Channel_10:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_4;
            }
            break;
            case ADC_Channel_11:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_5;
            }
            break;
            case ADC_Channel_12:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_6;
            }
            break;
            case ADC_Channel_13:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_7;
            }
            break;
            case ADC_Channel_14:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_8;
            }
            break;
            case ADC_Channel_15:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_9;
            }
            break;
            case ADC_Channel_16:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_10;
            }
            break;
            case ADC_Channel_17:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_11;
            }
            break;
            case ADC_Channel_18:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_12;
            }
            break;
            case ADC_Channel_19:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_13;
            }
            break;
            case ADC_Channel_20:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_14;
            }
            break;
            case ADC_Channel_21:
            {
                params->port = GPIOD;
                params->pin = GPIO_Pin_15;
            }
            break;
            case ADC_Channel_22:
            {
                params->port = GPIOE;
                params->pin = GPIO_Pin_2;
            }
            break;
            case ADC_Channel_23:
            {
                params->port = GPIOE;
                params->pin = GPIO_Pin_3;
            }
            break;
                // case ADC_Channel_24: // MCU TEMP CAN BE MEASURED
                // {
                // }
                // break;

            default:
                break;
            }

            RCC_AHBPeriphClockOnOff(RCC_AHB_IOCLK, SH_ON);

            GPIO_InitTypeDef GPIO_InitStructure;
            GPIO_InitStructure.GPIO_Pin = params->pin;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
            GPIO_InitStructure.GPIO_ODrv = GPIO_ODrv_NORMAL;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
            GPIO_Init(params->port, &GPIO_InitStructure);
            GPIO_PinAFConfig(params->port, params->pin, GPIO_AF_GPIO);

            ADC_InitTypeDef ADC_InitStructure;
            /* ADC1 configuration */
            ADC_StructInit(&ADC_InitStructure);
            /* Enable ADC clock */
            RCC_APB1PeriphClockOnOff(RCC_APB1_ADC, SH_ON);

            GPIO_PinAFConfig(params->port, params->pin, GPIO_AF_ADC);

            ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_Single;
            ADC_InitStructure.ADC_NumOfConversion = 1+idx; // ADC Channel number may be between 1-8 because ADC can measure maximum 8 channel
            ADC_InitStructure.ADC_Prescaler = ADC_Prescaler_1;
            ADC_InitStructure.ADC_RefVoltage = ADC_RefVoltage_AVDD;
            ADC_InitStructure.ADC_Resolution = ADC_Resolution_10Bit;
            ADC_InitStructure.ADC_SampleTime = ADC_SampleTime_15Cycles;
            ADC_InitStructure.ADC_SequencePointer = ADC_Sequence_0 + 4 * idx;
            ADC_InitStructure.ADC_Trigger = ADC_Trigger_Software;
            ADC_InitStructure.ADC_TwoSampleGap = ADC_TwoSampleGap_4Cycles;
            ADC_Init(consts->ADCx, &ADC_InitStructure);

            ADC_ChannelConfig(consts->ADCx, consts->chList[idx], ADC_Sequence_0 + 4 * idx, ADC_TwoSampleGap_Disable); // ADC_Sequence_0 or ADC_Sequence_1 or ADC_Sequence_2 may be used

            ADC_INTConfig(consts->ADCx, ADC_INT_EOC, SH_ENABLE);

            ADC_OnOff(consts->ADCx, SH_ON); // Power on ADC module

            NVIC_EnableIRQ(ADC_IRQn);
            
        }

        if (consts->refreshTime == 0)
        {
            ADC_SoftwareStartConversion(consts->ADCx);
        }
        else
        {
            timerCallBackStart(&params->timer, consts->refreshTime);
        }

        device->sys->initialized = 1;
    }
    else
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static DEV_CPX_FUNC_DEINIT(adcCoreDeinit)
{
    const tsAdcCoreConsts *consts = device->constants;

    device->sys->initialized = 0;
    consts->stopConvert(consts->ADCx);
    consts->startStop(consts->ADCx, SH_OFF);

    return EXIT_SUCCESS;
}

void adcCoreIrqHandler(const tsDevCpx *device)
{
    tsAdcCoreParams *params = device->parameters;
    const tsAdcCoreConsts *consts = device->constants;

    ADC_ClearFlag(consts->ADCx, ADC_FLAG_EOC);

    for (uint8_t idx = 0; idx < consts->chListSize; idx++)
    {
        params->chValues[idx] = ADC_GetConversionValue(consts->ADCx, ADC_Sequence_0 + 4*idx);
    }

    if (consts->refreshTime == 0) // refreshtime "0" means that ADC will start to measure immediately.
    {
        ADC_SoftwareStartConversion(consts->ADCx);
    }
}

static uint32_t adcCoreGet(const tsDevCpx *adcCore, uint16_t channel)
{
    const tsAdcCoreConsts *consts = adcCore->constants;
    tsAdcCoreParams *params = adcCore->parameters;

    if (channel < consts->chListSize)
    {
        return params->chValues[channel];
    }
    else
    {
        return 0;
    }
}

/** @} */

/**
 *  @addtogroup PSOC4_ADC
 *  @{
 */

static DEV_IO_FUNC_INIT(adcInit);
static DEV_IO_FUNC_DEINIT(adcDeinit);
static DEV_IO_FUNC_GET(adcGet);

const tsDevIoFuncs devIoAdcFuncs =
    {
        adcInit,
        adcDeinit,
        adcGet,
        NULL,
};

static DEV_IO_FUNC_INIT(adcInit)
{
    const tsAdcConsts *consts = device->constants;

    UNUSED(config); // REMOVE IF USED
    if (!consts->adcCore->sys->initialized)
    {
        devCpxInit(consts->adcCore, NULL);
        device->sys->initialized = 1;
    }
    else
    {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(adcDeinit)
{
    const tsAdcConsts *consts = device->constants;

    devCpxDeinit(consts->adcCore);
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(adcGet)
{
    const tsAdcConsts *consts = device->constants;

    return adcCoreGet(consts->adcCore, consts->channel);
}

/** @} */

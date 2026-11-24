/** @file       psoc4/pwm.c
 *  @brief      Pulse Width Modulation device driver for Sinowealth
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Enes Inanc
 *  @date       10.03.2023
 */
#define FILE_PWM_C

/**
 *  @addtogroup PSOC4_PWM
 *  @{
 */
#include "pwm.h"

static void pwmSet(const tsDevCpx *device)
{
    tsPwmCoreParams *params       = device->parameters;
    const tsPwmCoreConsts *consts = device->constants;

    if (params->freq && params->duty)
    {
        // consts->onOff(consts->PWMx, SH_OFF);
        // PWM_OutPutEnable(consts->PWMx, consts->PWMchannel, SH_DISABLE);
        
        params->period = params->clock/params->freq; 
        PWM_SetCycle(consts->PWMx, params->period);
        PWM_SetDuty(consts->PWMx, (params->period * params->duty) / 100);

        // PWM_OutPutEnable(consts->PWMx, consts->PWMchannel, SH_ENABLE);
        // consts->onOff(consts->PWMx, SH_ON);
    }
}

static DEV_CPX_FUNC_INIT(pwmCoreInit);
static DEV_CPX_FUNC_DEINIT(pwmCoreDeinit);

const tsDevCpxFuncs devCpxPwmCoreFuncs =
    {
        pwmCoreInit,
        pwmCoreDeinit,
};

static DEV_CPX_FUNC_INIT(pwmCoreInit)
{
    const tsPwmCoreConsts *consts = device->constants;
    tsPwmCoreParams *params = device->parameters;

    ADD_RCOS_VERSION("psoc4/pwm", "2.0.1")

    UNUSED(config); // REMOVE IF USED

    RCC_Clocks_TypeDef temp;
    RCC_GetClocksFreq(&temp);
    params->clock = temp.pclk0Freq / consts->PWM_CLKDIVNum; //SYSCLKDIV1;

    PWM_InitTypeDef PWM_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB0PeriphClockOnOff(consts->RCC_APBx_Type,SH_ON); 
    RCC_AHBPeriphClockOnOff(RCC_AHB_IOCLK,SH_ON); 
    
    GPIO_InitStructure.GPIO_Pin  = consts->pin;           //Set SPI0 CS Pin  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         //Set Output
    GPIO_InitStructure.GPIO_ODrv = GPIO_ODrv_NORMAL;      //Low Speed 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        //output Push-Pull
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      //No Pull-up Pull-down
    GPIO_Init(consts->port, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(consts->port, consts->pin, consts->GPIO_AF_PWMx);
    
    params->period = params->clock/params->freq;   // 24mhz -- 24000 -- 1khz    6mhz - 24000 -- 4khz
    PWM_InitStructure.PWM_Period = params->period;  //24000;//1KHz
    PWM_InitStructure.PWM_Duty = (params->period * params->duty) / 100;//12000;//50%
    PWM_InitStructure.PWM_DeadBand = 1;
    PWM_InitStructure.PWM_CLKSource = consts->PWM_CLKDIV; // 24mhz
    PWM_Init(consts->PWMx,&PWM_InitStructure);

    PWM_OutPutEnable(consts->PWMx, consts->PWMchannel, SH_ENABLE);
    consts->onOff(consts->PWMx, SH_ON);

    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_CPX_FUNC_DEINIT(pwmCoreDeinit)
{
    const tsPwmCoreConsts *consts = device->constants;

    consts->onOff(consts->PWMx, SH_OFF);

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
    tsPwmCoreParams *params      = device->parameters;
    const tsPwmSubConsts *consts = device->constants;

    if (data <= 100)
    {
        params->duty = data;

        if (data)
        {
            pwmSet(consts->mainDevice);
            ((const tsPwmCoreConsts *)consts->mainDevice->constants)->onOff(((const tsPwmCoreConsts *)consts->mainDevice->constants)->PWMx, SH_ON);
        }
        else
        {
            ((const tsPwmCoreConsts *)consts->mainDevice->constants)->onOff(((const tsPwmCoreConsts *)consts->mainDevice->constants)->PWMx, SH_OFF);
        }

        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
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
    tsPwmCoreParams *params      = device->parameters;
    const tsPwmSubConsts *consts = device->constants;

    params->freq = data;

    if (data)
    {
        // ((const tsPwmCoreConsts *)consts->mainDevice->constants)->onOff(((const tsPwmCoreConsts *)consts->mainDevice->constants)->PWMx, SH_OFF);
        pwmSet(consts->mainDevice);
        ((const tsPwmCoreConsts *)consts->mainDevice->constants)->onOff(((const tsPwmCoreConsts *)consts->mainDevice->constants)->PWMx, SH_ON);
    }
    else
    {
        ((const tsPwmCoreConsts *)consts->mainDevice->constants)->onOff(((const tsPwmCoreConsts *)consts->mainDevice->constants)->PWMx, SH_OFF);
    }

    return EXIT_SUCCESS;
}

/** @} */

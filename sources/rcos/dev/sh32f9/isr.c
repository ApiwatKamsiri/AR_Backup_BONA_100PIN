/** @file       isr.c
 *  @brief      C file of ISR device
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     halil
 *  @date       7.03.2023
 */
#define FILE_ISR_C

#include "isr.h"
#include "sh32f9xx_sb0_exti.h"
/**
 *  @addtogroup ISR
 *  @{
 */

static DEV_CPX_FUNC_INIT(isrInit);
static DEV_CPX_FUNC_DEINIT(isrDeinit);


 void IsrCallbackFunc(const tsDevCpx *device)
 {
     tsIsrParams *params = device->parameters;
     //const tsIsrConsts *consts = device->constants;
     if (EXTI_GetPendingFlagStatus(params->exti_line) == SH_SET)
     {
         EXTI_ClearPendingFlag(params->exti_line);
         params->callbackFunc();
     }
 }


/// @brief  Structure that defines the functions for isr
const tsDevCpxFuncs devCpxIsrFuncs =
{
    isrInit,
    isrDeinit,
};

/// @brief  Init function for a isr
static DEV_CPX_FUNC_INIT(isrInit)
{
    tsIsrParams *params       = device->parameters;
    
    const tsIsrConsts *consts = device->constants;
    EXTI_InitTypeDef EXTI_InitStructure;
    
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOE clock */
    RCC_AHBPeriphClockOnOff(RCC_AHB_IOCLK, SH_ON);

    GPIO_InitStructure.GPIO_Pin = consts->pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_ODrv = GPIO_ODrv_NORMAL;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = consts->pull_config;
    GPIO_Init(consts->port, &GPIO_InitStructure);
    GPIO_PinAFConfig(consts->port, consts->pin , GPIO_AF_GPIO);
   
    /* Enable EXTI clock */
    RCC_APB0PeriphClockOnOff(RCC_APB0_EXTI, SH_ON);   
    EXTI_PinConfig(params->exti_port, params->exti_pinsource);
    EXTI_InitStructure.EXTI_Line =  params->exti_line; 
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = params->trigger_mode;
    EXTI_InitStructure.EXTI_SampleCycle = EXTI_SampleCycle_1;
    EXTI_InitStructure.EXTI_SampleClockDivision = EXTI_SampleClockDivision_1;
    EXTI_InitStructure.EXTI_LineCmd = SH_ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    if(consts->pin==GPIO_Pin_0){
        NVIC_EnableIRQ(EXTI0_IRQn);
    }else if(consts->pin==GPIO_Pin_1){
        NVIC_EnableIRQ(EXTI1_IRQn);
    }else if(consts->pin==GPIO_Pin_2){
        NVIC_EnableIRQ(EXTI2_IRQn);
    }else if(consts->pin==GPIO_Pin_3){
        NVIC_EnableIRQ(EXTI3_IRQn);
    }else if(consts->pin==GPIO_Pin_4){
        NVIC_EnableIRQ(EXTI4_IRQn);
    }else if(consts->pin>=GPIO_Pin_5 && consts->pin<=GPIO_Pin_9){
        NVIC_EnableIRQ(EXTI9_5_IRQn);
    }else if(consts->pin>=GPIO_Pin_10 && consts->pin<=GPIO_Pin_15){
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
    // software Interrupt trigger
    //EXTI_GenerateSWInterrupt(params->exti_line); //< EXTI_Line12
    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    UNUSED(config); // REMOVE IF USED
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a isr
static DEV_CPX_FUNC_DEINIT(isrDeinit)
{
    tsIsrParams *params       = device->parameters;
    const tsIsrConsts *consts = device->constants;

    device->sys->initialized = 0;
    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    return EXIT_SUCCESS;
}

/** @} */

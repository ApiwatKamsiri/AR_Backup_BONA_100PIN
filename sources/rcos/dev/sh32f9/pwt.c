/** @file       psoc4/pwt.c
 *  @brief      C file of PWT device for CYPRESS PSOC4
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Enes Inanc
 *  @date       29.03.2023
 */
#define FILE_PWT_C

#include "pwt.h"

#define PCA_TOPR_MAX 0xFFFF

/**
 *  @addtogroup PSOC4_PWT_CORE
 *  @{
 */

static DEV_CPX_FUNC_INIT(pwtCoreInit);
static DEV_CPX_FUNC_DEINIT(pwtCoreDeinit);

/// @brief  Structure that defines the functions for pwt
const tsDevCpxFuncs devCpxPwtCoreFuncs =
    {
        pwtCoreInit,
        pwtCoreDeinit,
};

volatile uint16_t timercnt = 0, timercnt1 = 0, timerflag = 0, chazhi = 0;
volatile uint16_t enes;
volatile uint32_t capture;
volatile uint32_t capture2, diff;
/// @brief  Interrupt return function
void pwtIsr(const tsDevCpx *device)
{
    tsPwtCoreParams *params       = device->parameters;
    const tsPwtCoreConsts *consts = device->constants;
    // volatile uint32_t capture     = PCA_GetCounter(consts->PCAx, consts->PCA_CHANEELx);

    PCA_ClearFlag(consts->PCAx, PCA_FLAG_CCF0 | PCA_FLAG_CCF1 | PCA_FLAG_CF);

    // PCA_SetCPRValue
    // capture2 = PCA_GetCounter(consts->PCAx, consts->PCA_CHANEELx);
    // diff = capture2 - capture;
    // diff++;
    // capture = capture2;
    

    if (timerflag == 0)
    {
        capture = PCA_GetCounter(consts->PCAx, consts->PCA_CHANEELx);
        timerflag++;
    }
    else
    {   
        uint16_t temp = PCA_GetCounter(consts->PCAx, consts->PCA_CHANEELx);
        if(temp < capture)
        {
            chazhi = PCA_TOPR_MAX + temp - capture;
        }
        else
        {
            chazhi = temp - capture;
        }
        
        // PCA_SetCPRValue(consts->PCAx, consts->PCA_CHANEELx, 0);

        // params->duty = (consts->dutyMax * params->pulse) / chazhi;
        params->freq = params->clock / chazhi;

        timerflag = 0;
    }

}

/// @brief  Init function for a pwt
static DEV_CPX_FUNC_INIT(pwtCoreInit)
{
    const tsPwtCoreConsts *consts = device->constants;
    tsPwtCoreParams *params = device->parameters;
    ADD_RCOS_VERSION("sinowealth/pwt", "1.0.0")

    UNUSED(config);

    // consts->isrStartEx(consts->interrupt);
    // consts->cntrStart();

    RCC_Clocks_TypeDef temp;
    RCC_GetClocksFreq(&temp);
    params->clock = temp.pclk0Freq / consts->PCA_CLKDIV_Num; //SYSCLKDIV1;
    

    // PCA MOULD GPIO CONFIG
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Enable GPIO clock */
    RCC_AHBPeriphClockOnOff(RCC_AHB_IOCLK, SH_ON);

    /* PCA MOULD COM PC7 */
    GPIO_InitStructure.GPIO_Pin = consts->pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_ODrv = GPIO_ODrv_NORMAL;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(consts->port, &GPIO_InitStructure);

    GPIO_PinAFConfig(consts->port, consts->pin, consts->GPIO_AF_PCAx);


    PCA_InitTypeDef PCA_InitStructure;

    // Open PCAx clock gate
    switch(consts->PCAnum)
    {
        case 0:
            RCC_AHBPeriphClockOnOff(RCC_AHB_PCA0, SH_ON); // PCA0/1
        break;
        case 1:
            RCC_AHBPeriphClockOnOff(RCC_AHB_PCA1, SH_ON); // PCA0/1
        break;
        case 2:
            RCC_APB0PeriphClockOnOff(RCC_APB0_PCA2,SH_ON);  //PCA2-7
        break;
        case 3:
            RCC_APB0PeriphClockOnOff(RCC_APB0_PCA3,SH_ON);  //PCA2-7
        break;
        case 4:
            RCC_APB0PeriphClockOnOff(RCC_APB0_PCA4,SH_ON);  //PCA2-7
        break;
        case 5:
            RCC_APB0PeriphClockOnOff(RCC_APB0_PCA5,SH_ON);  //PCA2-7
        break;
        case 6:
            RCC_APB0PeriphClockOnOff(RCC_APB0_PCA6,SH_ON);  //PCA2-7
        break;
        case 7:
            RCC_APB0PeriphClockOnOff(RCC_APB0_PCA7,SH_ON);  //PCA2-7
        break;
        
        default:
        break;
    }

    PCA_StructInit(&PCA_InitStructure);
    GPIO_StructInit(&GPIO_InitStructure);

    PCA_InitStructure.PCA_CMD.ClkSel = consts->PCA_CLKDIVx;  //SYSTEMCLKDIV4;
    PCA_InitStructure.PCA_CPM0.PCA_ECOM = SH_ENABLE;
    PCA_InitStructure.PCA_CPM0.ModeSelect = CAPTUREMODE;
    PCA_InitStructure.PCA_CPM0.FunctionSelect = RISING_EDGE_TRIGGER;
    PCA_InitStructure.PCA_CMD.PCACounterMode = SINGLEEDGE;
    PCA_InitStructure.PCA_CPM0.PCA_TCP = SH_DISABLE;

    PCA_InitStructure.PCA_TOPR = PCA_TOPR_MAX;  // 0xFFFF;
    PCA_InitStructure.PCA_CPR0 = 0;
    PCA_InitStructure.PCA_CPM0.Match = SH_ENABLE;

    PCA_Init(consts->PCAx, &PCA_InitStructure);
    
    PCA_INTConfig(consts->PCAx, SH_DISABLE);
    PCA_CapINTConfig(consts->PCAx, consts->PCA_CHANEELx, SH_ENABLE);

    consts->PCAOnOff(consts->PCAx, SH_ON); // enable PCA
    
    NVIC_EnableIRQ(consts->PCAx_IRQn); // PCAx_IRQn


    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a pwt
static DEV_CPX_FUNC_DEINIT(pwtCoreDeinit)
{
    // const tsPwtCoreConsts *consts = device->constants;

    device->sys->initialized = 0;
    // consts->cntrStop();
    // consts->isrStop();

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_INIT(pwtChildInit)
{
    const tsPwtChildParams *params = device->parameters;

    UNUSED(config);

    if (0 == params->core->sys->initialized)
    {
        devCpxInit(params->core, NULL);
    }
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_DEINIT(pwtChildDeinit)
{
    const tsPwtChildParams *params = device->parameters;

    device->sys->initialized = 0;
    if (1 == params->core->sys->initialized)
    {
        devCpxDeinit(params->core);
    }

    return EXIT_SUCCESS;
}

/** @} */

/**
 *  @addtogroup PSOC4_PWT_DUTY
 *  @{
 */

// static DEV_IO_FUNC_GET(pwtDutyGet);

// const tsDevIoFuncs devIoPwtDutyFuncs =
//     {
//         pwtChildInit,
//         pwtChildDeinit,
//         pwtDutyGet,
//         NULL,
// };

// static DEV_IO_FUNC_GET(pwtDutyGet)
// {
//     const tsPwtChildParams *params = device->parameters;

//     return params->coreParams->duty;
// }

/** @} */

/**
 *  @addtogroup PSOC4_PWT_FREQ
 *  @{
 */

static DEV_IO_FUNC_GET(pwtFreqGet);

const tsDevIoFuncs devIoPwtFreqFuncs =
    {
        pwtChildInit,
        pwtChildDeinit,
        pwtFreqGet,
        NULL,
};

static DEV_IO_FUNC_GET(pwtFreqGet)
{
    const tsPwtChildParams *params = device->parameters;

    return params->coreParams->freq;
}

/** @} */

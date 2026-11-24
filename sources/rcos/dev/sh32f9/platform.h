/** @file       sh32f9/platform.h
 *  @brief      Platform specific modifications for Sinowealth SH32F9 series MCUs
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ozkan AKGUL
 *  @date       12.01.2023
 */
#ifndef FILE_PLATFORM_H
#define FILE_PLATFORM_H

/**
 *  @defgroup   SH32F9
 *  @ingroup    PLATFORM
 *  @brief      Sinowealth SH32F9 series family devices
 *  @{
 *      @defgroup   SH32F9_DEV_IO DEV_IO
 *      @defgroup   SH32F9_DEV_COM DEV_COM
 *      @defgroup   SH32F9_DEV_MEM DEV_MEM
 *      @defgroup   SH32F9_DEV_CPX DEV_CPX
 */

/*********************************************************************
 *  INCLUDES
 */
#include "sh32f9xx_sb0_retarget.h"
#include <stdbool.h>

/*********************************************************************
 *  FUNCTION DECLARATIONS
 */
void tickStart(void);
void tickEnable(void);
void tickDisable(void);
void SysTick_Handler(void);
void wdtRefresh(void);
void wdtStart(void);
void wdtStop(void);

/*********************************************************************
 *  EXPORTED MACROS
 */

/// @brief  Create a default tick configuration for this platform
/// @param  _duration   Period of RCoS Tick in ms
#define CORE_TICK_DEFAULT(_duration)                   \
    void tickStart(void)                               \
    {                                                  \
        RCC_Clocks_TypeDef rcc_clocks;                 \
        /* get current system clocks */                \
        RCC_GetClocksFreq(&rcc_clocks);                \
        /* set systick interrupt : 1ms */              \
        /* TODO: add duration parameter : 1ms */       \
        SysTick_Config(rcc_clocks.hclkFreq / 1000);    \
    }                                                  \
    void tickEnable(void)                              \
    {                                                  \
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      \
    }                                                  \
    void tickDisable(void)                             \
    {                                                  \
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;     \
    }                                                  \
    void SysTick_Handler(void)                         \
    {                                                  \
        rcosTickIsr();                                 \
    }                                                  \
    CORE_TICK_CREATE(tickStart, tickEnable, tickDisable)

/// @brief  Create a default watchdog timer
/// @param  _prescaler    Name of Renesas WDT Module
/// @param  _flagCount  Number of flags
#define CORE_WDT_DEFAULT(_flagCount)                         \
    void wdtRefresh(void)                                                \
    {                                                                    \
        /*TODO : add duration conversion here*/                          \
        IWDG_ReloadCounter();                                            \
    }                                                                    \
    void wdtStart(void)                                                  \
    {                                                                    \
        /* TODO : add prescaler parameter*/                              \
        IWDT_Enable(IWDT_Prescaler_8, 0xFF);                             \
    }                                                                    \
    void wdtStop(void)                                                   \
    {                                                                    \
        return;                                                          \
    }                                                                    \
    CORE_WDT_CREATE(wdtRefresh, wdtStart, wdtStop, _flagCount)

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
/*********************************************************************
 *  INTERFACES
 */
#define PLATFORM_ASM(_asm) __ASM volatile(_asm)
#define PLATFORM_SECTION(_name) __attribute__((section(_name)))
#define PLATFORM_ALIGNED(_align) __attribute__((aligned(_align)))
#define PLATFORM_STATIC_INLINE __STATIC_INLINE
#define PLATFORM_WEAK __WEAK
#define PLATFORM_PACKED __PACKED

/*********************************************************************
 *  INTERFACES: CONSTANTS
 */

/*********************************************************************
 *  INTERFACES: VARIABLES
 */

/*********************************************************************
 *  INTERFACES: FUNCTIONS
 */

/** @} */

#endif // FILE_PLATFORM_H

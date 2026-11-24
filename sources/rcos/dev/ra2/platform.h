/** @file       ra2/platform.h
 *  @brief      Platform specific modifications for Renesas RA2 series MCUs
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     xx
 *  @date       xx
 *  @author     Burak OZAN
 *  @date       09.09.2022
 */
#ifndef FILE_PLATFORM_H
#define FILE_PLATFORM_H

/**
 *  @defgroup   RA2
 *  @ingroup    PLATFORM
 *  @brief      Renesas RA2 series family devices
 *  @{
 *      @defgroup   RA2_DEV_IO DEV_IO
 *      @defgroup   RA2_DEV_COM DEV_COM
 *      @defgroup   RA2_DEV_MEM DEV_MEM
 *      @defgroup   RA2_DEV_CPX DEV_CPX
 */

/*********************************************************************
 *  INCLUDES
 */
#include "hal_data.h"

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

#ifdef R_CAC_CFG_H_
#define CAC_ACTIVE
#if ( (BSP_CFG_CLOCK_SOURCE) == (BSP_CLOCKS_SOURCE_CLOCK_HOCO) )
#define PWT_CAC_ACTIVE
#endif
#ifndef R_CGC_CFG_H_
#error CAC module requires CGC module
#endif
#endif

/// @brief  Create a default tick configuration for this platform
/// @param  _freq   Frequency of RCoS Tick in Hz
#define CORE_TICK_DEFAULT(_freq)                       \
    void tickStart(void)                               \
    {                                                  \
        SysTick_Config(BSP_STARTUP_ICLK_HZ / (_freq)); \
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
/// @param  _g_wdt    Name of Renesas WDT Module
/// @param  _flagCount  Number of flags
#define CORE_WDT_DEFAULT(_g_wdt, _flagCount)                             \
    void wdtRefresh(void)                                                \
    {                                                                    \
        fsp_err_t err = FSP_SUCCESS;                                     \
        err           = _g_wdt.p_api->refresh(_g_wdt.p_ctrl);            \
        RCOS_ASSERT(FSP_ERR_ASSERTION != err, RASSERT_PRI_LOW);          \
    }                                                                    \
    void wdtStart(void)                                                  \
    {                                                                    \
        fsp_err_t err = FSP_SUCCESS;                                     \
        err           = _g_wdt.p_api->open(_g_wdt.p_ctrl, _g_wdt.p_cfg); \
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);                \
    }                                                                    \
    void wdtStop(void)                                                   \
    {                                                                    \
        return;                                                          \
    }                                                                    \
    CORE_WDT_CREATE(wdtRefresh, wdtStart, wdtStop, _flagCount)

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

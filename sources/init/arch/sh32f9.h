/** @file       psoc4_init.h
 *  @brief      RCoS+ configuration file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Emre Karabek
 *  @date       20.03.2023
 */
#ifndef FILE_PSOC4_INIT_H
#define FILE_PSOC4_INIT_H

//#include "project.h"
#include "rcos.h"
#include "rcos/dev/sh32f9/platform.h" ///< CPU platform information
// static void psoc4TempSysTick();

/* -------------------------------------------------------------------- */
/* TEMPORARY SYSTICK -------------------------------------------------- */
/* -------------------------------------------------------------------- */
// void psoc4TempSysTickEnable(void)
// {
//     AltSysTickCall_StartEx(psoc4TempSysTick);
// }

// void psoc4TempSysTickStop(void)
// {
//     AltSysTickCall_Stop();
// }

// void psoc4TempSysTick()
// {
//     rcosTickIsr();

//     AltSysTick_ClearInterrupt(AltSysTick_INTR_MASK_TC);
// }

/* -------------------------------------------------------------------- */
/* ADC INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */


/* -------------------------------------------------------------------- */
/* UART INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */


/* -------------------------------------------------------------------- */
/* GPIO INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */



/* -------------------------------------------------------------------- */
/* I2C INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */


/* -------------------------------------------------------------------- */
/* PWT DRIVER --------------------------------------------------------- */
/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* EMULATED EEPROM INIT ----------------------------------------------- */
/* -------------------------------------------------------------------- */
#undef PLATFORM_ALIGNED
#define PLATFORM_ALIGNED(_align) __attribute__((aligned(_align)))
#undef PLATFORM_SECTION
#define PLATFORM_SECTION(_name) __attribute__((section(_name)))
//#include "rcos/dev/sh32f9/emu_eeprom.h"
#if (BOOTLOADER_AVAILABLE == COMP_AVAILABLE)
#define EMULATED_EEPROM_START_ADDRESS (0x2000)
//DEV_MEM_EMU_ADDRESS_CREATE(emulatedEepromSection, EmulatedEeprom, EMULATED_EEPROM_START_ADDRESS)
#else
#define EMULATED_EEPROM_START_ADDRESS (0x3A400)
#pragma location = EMULATED_EEPROM_START_ADDRESS
//DEV_MEM_EMU_SECTION_CREATE(emulatedEepromSection, EmulatedEeprom, ".emulated_eeprom")
#endif
#endif // FILE_RCOS_RC_H

/** @file       ke14_init.h
 *  @brief      RCoS+ configuration file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Vehbi Umur Cabuk
 *  @date       29.06.2021
 */
#ifndef FILE_KE14_INIT_H
#define FILE_KE14_INIT_H

#include "rcos.h"
#include "rcos/dev/ke14/platform.h" ///< CPU platform information

/* -------------------------------------------------------------------- */
/* ADC INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */
#include "rcos/dev/ke14/adc.h"
DEV_CPX_ADC_CORE_CREATE(adcBlock, ADC1, 1, ADC_CH(5, 0))
DEV_IO_ADC_CREATE(washerNtc, adcBlock, 0)
#if (DRYER_NTC_AVAILABILITY == COMP_AVAILABLE)
DEV_IO_ADC_CREATE(drumEntryNtc, adcBlock, 1)
#endif

/* -------------------------------------------------------------------- */
/* UART INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */
#include "rcos/dev/ke14/uart.h"
DEV_COM_UART_CREATE(dbScUart, 2, 38400, Disabled, 500, 500)

#if ((SAFIR_AVAILABILITY == COMP_AVAILABLE) && (UDAQ_AVAILABILITY != COMP_AVAILABLE))
DEV_COM_UART_CREATE(connCom, 0, 9600, Disabled, 50, 250)
#else
DEV_COM_UART_CREATE(udaqCom, 0, 9600, Disabled, 50, 250)
#if (SAFIR_AVAILABILITY == COMP_AVAILABLE)
DEV_COM_UART_CREATE(connCom, UART_Wifi)
#endif // SAFIR_AVAILABILITY
#endif // SAFIR_AVAILABILITY && UDAQ_AVAILABILITY

/* -------------------------------------------------------------------- */
/* GPIO INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */
#include "rcos/dev/ke14/gpio.h"
#include "rcos/dev/iovirtual.h"
#if (DOOR_SWITCH_AVAILABILITY_ON_MB == COMP_AVAILABLE)
DEV_IO_GPIO_CREATE(doorSwitchIo, uc_DoorSwitch, 1)
#endif
DEV_IO_GPIO_CREATE(flowmeterIo, uc_Flowmeter, 1, 0, 1)
#if (FLOWMETER_2_AVAILABILITY == 1)
DEV_IO_GPIO_CREATE(flowmeter2Io, uc_Flowmeter_2, 1, 0, 1)
#endif
DEV_IO_IOVIRTUAL_CREATE(aquaStopIo)
#if (MB_ENABLE_AVAILABILITY == 1)
DEV_IO_GPIO_CREATE(ucMbEnable, uc_MB_Enable, 0, 1, 0)
#endif // MB_ENABLE_AVAILABILITY
#if (SAFIR_AVAILABILITY == COMP_AVAILABLE)
DEV_IO_GPIO_CREATE(ucWiFiEnable, uc_SFR_pow_EN, 1)
#endif // SAFIR_AVAILABILITY
DEV_IO_GPIO_CREATE(ucI2cPowerEnable, uc_I2C_pow_EN, 0, 0, 0)
#if (I2C_2_AVAILABILITY == 1)
DEV_IO_GPIO_CREATE(ucI2c2PowerEnable, uc_I2C_2_pow_EN, 1)
#endif

#if (REED_SWITCH_AVAILABILITY == 1)
DEV_IO_GPIO_CREATE(uc_reedSwitchIo_1, uc_reedSwitch_1, 1)
DEV_IO_GPIO_CREATE(uc_reedSwitchIo_2, uc_reedSwitch_2, 1)
DEV_IO_GPIO_CREATE(uc_reedSwitchIo_3, uc_reedSwitch_3, 1)
DEV_IO_GPIO_CREATE(uc_reedSwitchIo_4, uc_reedSwitch_4, 1)
#endif

#if (DRUM_LED_AVAILABILITY == 1)
DEV_IO_GPIO_CREATE(uc_DrumLedIo, uc_DrumLed, 1)
#endif // DRUM_LED_AVAILABILITY

#if (DOOR_LED_AVAILABILITY == 1)
DEV_IO_GPIO_CREATE(uc_DoorLedIo, uc_DoorLed, 1)
#endif // DOOR_LED_AVAILABILITY

/* -------------------------------------------------------------------- */
/* I2C INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */

#include "dev/ke14/i2c.h"
DEV_COM_I2C_CREATE(i2cBus, LPI2C1)

#if (I2C_2_AVAILABILITY == 1)
DEV_COM_I2C_CREATE(i2cBus2, LPI2C2)
#endif

/* -------------------------------------------------------------------- */
/* PWT DRIVER --------------------------------------------------------- */
/* -------------------------------------------------------------------- */
#include "dev/ke14/pwt.h"
DEV_IO_PWT_CREATE(waterLevelPwt, 1, FTM1)

/* -------------------------------------------------------------------- */
/* EMULATED EEPROM INIT ----------------------------------------------- */
/* -------------------------------------------------------------------- */
#undef PLATFORM_ALIGNED
#define PLATFORM_ALIGNED(_align) __attribute__((aligned(_align)))
#undef PLATFORM_SECTION
#define PLATFORM_SECTION(_name) __attribute__((section(_name)))
#include "rcos/dev/ke14/flash.h"
#if (BOOTLOADER_AVAILABLE == COMP_AVAILABLE)
#define EMULATED_EEPROM_START_ADDRESS (0x2000)
DEV_MEM_EMU_ADDRESS_CREATE(emulatedEepromSection, EmulatedEeprom, EMULATED_EEPROM_START_ADDRESS)
#else
DEV_CPX_FLASH_CREATE(emulatedEepromCore)
DEV_MEM_SECTION_CREATE(emulatedEepromSection, "EEPROM_EM_SECTION", 2048, emulatedEepromCore, 512)
#endif
#endif // FILE_RCOS_RC_H

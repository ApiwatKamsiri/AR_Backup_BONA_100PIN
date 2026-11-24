/** @file       psoc6_init.h
 *  @brief      RCoS+ configuration file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Emre Onder
 *  @date       07.06.2021
 */
#ifndef FILE_PSOC6_INIT_H
#define FILE_PSOC6_INIT_H



#if (CY_CPU_CORTEX_M0P)

#else//CY_CPU_CORTEX_M4

#include "rcos.h"
#include "rcos/dev/psoc6/platform.h" ///< CPU platform information

static void psoc6TempSysTick();

/* -------------------------------------------------------------------- */
/* TEMPORARY SYSTICK -------------------------------------------------- */
/* -------------------------------------------------------------------- */
void psoc6TempSysTickEnable(void)
{
    Cy_SysInt_Init(&AltSysTickCall_cfg, psoc6TempSysTick);
}

void psoc6TempSysTickStop(void)
{
    Cy_TCPWM_Disable_Multiple(AltSysTick_HW, AltSysTick_CNT_MASK);
}

void psoc6TempSysTick()
{
    rcosTickIsr();
    AltSysTick_ClearInterrupt(CY_TCPWM_INT_ON_TC);
}

/* -------------------------------------------------------------------- */
/* ADC INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */
#include "rcos/dev/psoc6/adc.h"
DEV_CPX_ADC_CORE_CREATE(adcBlock, ADC_seq)
DEV_IO_ADC_CREATE(washerNtc, adcBlock, 0)
#if (DRYER_NTC_AVAILABILITY == COMP_AVAILABLE)
DEV_IO_ADC_CREATE(drumEntryNtc, adcBlock, 2)
#endif

/* -------------------------------------------------------------------- */
/* PWT DRIVER --------------------------------------------------------- */
/* -------------------------------------------------------------------- */
#include "rcos/dev/psoc6/pwt.h"
DEV_IO_PWT_CREATE(waterLevelPwt, waterLevelCounter, isr_cc ,8000000, 50);

/* -------------------------------------------------------------------- */
/* UART INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */
#include "rcos/dev/psoc6/uart.h"
DEV_COM_UART_WOB_CREATE(dbScUart, UART_1) 

#if ((SAFIR_AVAILABILITY == COMP_AVAILABLE) && (UDAQ_AVAILABILITY != COMP_AVAILABLE))
DEV_COM_UART_WOB_CREATE(connCom, UART_WifiUdaq)
#else
DEV_COM_UART_WOB_CREATE(udaqCom, UART_WifiUdaq)  
#if (SAFIR_AVAILABILITY == COMP_AVAILABLE)
DEV_COM_UART_WOB_CREATE(connCom, UART_Wifi)
#elif (BLUETOOTH_AVAILABLE == COMP_AVAILABLE)
#include "dev/psoc6/ipc.h"
DEV_COM_IPC_CREATE(connCom, IPC_DATA_SIZE_FROM_M0, IPC_DATA_SIZE_FROM_M4)
#endif // SAFIR_AVAILABILITY
#endif // SAFIR_AVAILA"BILITY && UDAQ_AVAILABILITY


/* -------------------------------------------------------------------- */
/* GPIO INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */
#include "rcos/dev/psoc6/gpio.h"

#if (DOOR_SWITCH_AVAILABILITY_ON_MB == COMP_AVAILABLE)//ok
DEV_IO_GPIO_CREATE(doorSwitchIo, uc_DoorSwitch, 1)
#endif
DEV_IO_GPIO_CREATE(flowmeterIo, uc_Flowmeter , 1)

#if (FLOWMETER_2_AVAILABILITY == 1)
DEV_IO_GPIO_CREATE(flowmeter2Io, uc_Flowmeter_2, 1)
#endif

DEV_IO_GPIO_CREATE(aquaStopIo, uc_Aquastop, 1)
#if (MB_ENABLE_AVAILABILITY == 1)
DEV_IO_GPIO_CREATE(ucMbEnable, uC_MB_Enable, 1)
#endif // MB_ENABLE_AVAILABILITY

#if (SAFIR_AVAILABILITY == COMP_AVAILABLE)//ok
DEV_IO_GPIO_CREATE(ucWiFiEnable, uc_SFR_pow_EN, 1)
#endif // SAFIR_AVAILABILITY
DEV_IO_GPIO_CREATE(ucI2cPowerEnable, uc_I2C1_pow_EN, 0)
#if (I2C_2_AVAILABILITY == 1)
DEV_IO_GPIO_CREATE(ucI2c2PowerEnable, uc_I2C2_pow_EN, 0)
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

/* -------------------------------------------------------------------- */
/* I2C INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */
#include "rcos/dev/psoc6/i2c.h"
DEV_COM_I2C_CREATE(i2cBus, I2C_Bus, 5)

#if (I2C_2_AVAILABILITY == 1)
DEV_COM_I2C_CREATE(i2cBus2, I2C_2, 10)
#endif
	
// /* -------------------------------------------------------------------- */
// /* EMULATED EEPROM INIT ----------------------------------------------- */
// /* -------------------------------------------------------------------- */
#undef PLATFORM_ALIGNED
#define PLATFORM_ALIGNED(_align) __attribute__((aligned(_align)))
#undef PLATFORM_SECTION
#define PLATFORM_SECTION(_name) __attribute__((section(_name)))
#include "rcos/dev/psoc6/emu_eeprom.h"
#if (BOOTLOADER_AVAILABLE == COMP_AVAILABLE)
#define EMULATED_EEPROM_START_ADDRESS (0x2000)
DEV_MEM_EMU_ADDRESS_CREATE(emulatedEepromSection, EmulatedEeprom, EMULATED_EEPROM_START_ADDRESS)
#else
DEV_MEM_EMU_SECTION_CREATE(emulatedEepromSection,EmulatedEeprom,".cy_em_eeprom")
#endif
#endif// CY_CPU_CORTEX_M4
#endif // FILE_RCOS_RC_H

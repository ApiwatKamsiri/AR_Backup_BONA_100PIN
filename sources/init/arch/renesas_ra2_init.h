//** @file       renesas_ra2_init.h
// *  @brief      RCoS+ main file
// *  @copyright  (c) 2019-Arcelik - All Rights Reserved
// *              Permission to use, reproduce, copy, prepare derivative works,
// *              modify, distribute, perform, display or sell this software and/or
// *              its documentation for any purpose is prohibited without the express
// *              written consent of Arcelik A.S.
// *  @author     Erdeniz Dalyan
// *  @date       30.10.2021
// */
//
//#ifndef _RENESAS_RA2_INIT_H_
//#define _RENESAS_RA2_INIT_H_
//#include "rcos.h"
//#include "rcos/dev/ra2/platform.h"
//
//#ifndef MB_ENABLE_ACTIVE_TRIGGER
//#error Please define MB_ENABLE_ACTIVE_TRIGGER. Activity level of the pin (0: low, 1: high)
//#endif
//#ifndef I2C_ENABLE_ACTIVE_TRIGGER
//#error Please define I2C_ENABLE_ACTIVE_TRIGGER. Activity level of the pin (0: low, 1: high)
//#endif
//#ifndef PWT_TIMER_CAPTURE_CHANNEL
//#error Please define PWT_TIMER_CAPTURE_CHANNEL as a TIMER_EVENT_CAPTURE_B OR TIMER_EVENT_CAPTURE_A
//#endif
//
//#if (DRIVER_BOARD == DEF_NOT_AVAILABLE)
//    #if (DRYER_NTC_AVAILABILITY == COMP_NOT_AVAILABLE)
//        #define LINE_LEVEL_ADC_INDEX (1)
//    #else
//        #define DRUM_ENTRY_NTC_ADC_INDEX (1)
//        #define LINE_LEVEL_ADC_INDEX (2)
//    #endif
//#else
//    #ifdef DRYER_NTC_AVAILABILITY
//        #define DRUM_ENTRY_NTC_ADC_INDEX (1)
//    #endif
//#endif
///* ---------------------------------------------------------- */
///* RCOS IO DRIVERS ------------------------------------------ */
///* ---------------------------------------------------------- */
//#include "../../rcos/dev/ra2/gpio.h"
//#include "../../rcos/dev/iovirtual.h"
//DEV_IO_GPIO_STACK(g_ioport)
//
//
///* ---------------------------------------------------------- */
///* ADC MODULE ----------------------------------------------- */
///* ---------------------------------------------------------- */
//#include "../../rcos/dev/ra2/adc.h"
//DEV_CPX_ADC_CORE_CREATE(adcBlock,
//                        ADC_seq,
//                        adcIsr,
//                        1,
//                        DISP_WASHER_NTC_CHANNEL
//#if (DRYER_NTC_AVAILABILITY == COMP_AVAILABLE)
//                        ,DISP_DRUM_ENTRY_NTC_CHANNEL
//#endif
//#if (DRIVER_BOARD == DEF_NOT_AVAILABLE)
//                        ,LINE_LEVEL_CHANNEL
//#endif
//                        )
//DEV_IO_ADC_CREATE(washerNtc, adcBlock, 0)
//#if (DRYER_NTC_AVAILABILITY == COMP_AVAILABLE)
//DEV_IO_ADC_CREATE(drumEntryNtc, adcBlock, DRUM_ENTRY_NTC_ADC_INDEX)
//#endif
//#if (DRIVER_BOARD == DEF_NOT_AVAILABLE)
//DEV_IO_ADC_CREATE(lineLevelAdc, adcBlock, LINE_LEVEL_ADC_INDEX)
//#endif
//#ifdef CAC_ACTIVE
///* ---------------------------------------------------------- */
///* CAC MODULE ----------------------------------------------- */
///* ---------------------------------------------------------- */
//#include "../../dev/ra2/cac.h"
//DEV_IO_CAC_CREATE(cacConfig, g_cac0, g_cgc0, cac_callback)
//#endif
///* ---------------------------------------------------------- */
///* UART COMMUNICATION LINE ---------------------------------- */
///* ---------------------------------------------------------- */
//#include "../../rcos/dev/ra2/uart.h"
//#if (DRIVER_BOARD == DEF_AVAILABLE)
//DEV_COM_UART_CREATE(dbScUart, UART_1, uart1_callback, 250, 250)
//#elif (DRIVER_BOARD == DEF_NOT_AVAILABLE)
//DEV_COM_UART_CREATE(motorUart, motorCom, motorCom_uart_callback, 48, 50)
//#endif
//#if ((CONNECTIVITY == WIFI_CONNECTED) && (UDAQ_AVAILABILITY != COMP_AVAILABLE))
//DEV_COM_UART_CREATE(connCom, UART_WifiUdaq, uart_wifi_udaq_callback, 250, 250)
//#else
//DEV_COM_UART_CREATE(udaqCom, UART_WifiUdaq, uart_wifi_udaq_callback, 64, 512)
//#if (CONNECTIVITY == WIFI_CONNECTED)
//DEV_COM_UART_CREATE(connCom, UART_Wifi, uart_wifi_callback, 250, 250)
//#endif // CONNECTIVITY
//#endif // CONNECTIVITY && UDAQ_AVAILABILITY
//
///* -------------------------------------------------------------------- */
///* GPIO INIT ----------------------------------------------------------- */
///* -------------------------------------------------------------------- */
//#if (FLOWMETER_COMPONENT == DEF_AVAILABLE)
//DEV_IO_GPIO_CREATE(flowmeterIo, uc_Flowmeter , 1)
//#if (FLOWMETER_2_AVAILABILITY == 1)
//DEV_IO_GPIO_CREATE(flowmeter2Io, uc_Flowmeter_2, 1)
//#endif
//#endif // FLOWMETER_COMPONENT
//
//#if (DOOR_SWITCH_AVAILABILITY_ON_MB == COMP_AVAILABLE)
//DEV_IO_GPIO_CREATE(doorSwitchIo, uc_Doorswitch, 0)
//#endif
//
//#if (AQUA_STOP_AVAILABILITY == COMP_AVAILABLE)
//DEV_IO_GPIO_CREATE(aquaStopIo, uc_Aquastop, 1)
//#endif
//
//#if (MB_ENABLE_AVAILABILITY == 1) // available
//DEV_IO_GPIO_CREATE(ucMbEnable, uc_MB_Enable, MB_ENABLE_ACTIVE_TRIGGER)
//#elif (MB_ENABLE_AVAILABILITY == 2) // virtual
//DEV_IO_IOVIRTUAL_CREATE(ucMbEnable)
//#endif // MB_ENABLE_AVAILABILITY
//
//#if (REED_SWITCH_AVAILABILITY == 1)
////DEV_IO_GPIO_CREATE(uc_reedSwitchIo_1, uc_reedSwitch_1, 1)
////DEV_IO_GPIO_CREATE(uc_reedSwitchIo_2, uc_reedSwitch_2, 1)
//DEV_IO_IOVIRTUAL_CREATE(uc_reedSwitchIo_1)
//DEV_IO_IOVIRTUAL_CREATE(uc_reedSwitchIo_2)
//DEV_IO_GPIO_CREATE(uc_reedSwitchIo_3, uc_reedSwitch_3, 1)
//DEV_IO_GPIO_CREATE(uc_reedSwitchIo_4, uc_reedSwitch_4, 1)
//#endif
//
//#if (CONNECTIVITY == WIFI_CONNECTED)
//DEV_IO_GPIO_CREATE(ucWiFiEnable, uc_SFR_pow_EN, 1)
//#endif // CONNECTIVITY
//
//DEV_IO_GPIO_CREATE(ucI2cPowerEnable, uc_I2C_pow_EN, I2C_ENABLE_ACTIVE_TRIGGER)
//
//#if (I2C_2_AVAILABILITY == 1)
//DEV_IO_GPIO_CREATE(ucI2c2PowerEnable, uc_I2C_2_pow_EN, 0)
//#endif
//
//#if (DRUM_LED_AVAILABILITY == 1) // available
//DEV_IO_GPIO_CREATE(uc_DrumLedIo, uc_DrumLed, 1)
//#elif (DRUM_LED_AVAILABILITY == 2) // virtual
//DEV_IO_IOVIRTUAL_CREATE(uc_DrumLedIo)
//#endif // DRUM_LED_AVAILABILITY
//
//#if (DOOR_LED_AVAILABILITY == 1)
//DEV_IO_GPIO_CREATE(uc_DoorLedIo, uc_DoorLed, 1)
//#endif // DOOR_LED_AVAILABILITY
//
//#if (DRIVER_BOARD == DEF_NOT_AVAILABLE)
//DEV_IO_GPIO_CREATE(DLRelay, uc_DoorLockRL, 1)
//DEV_IO_GPIO_CREATE(DLTriac, uc_DoorLockTriac, 1)
//DEV_IO_GPIO_CREATE(MainHeaterRelay, uc_MainHeaterRL, 1)
//DEV_IO_GPIO_CREATE(DrainPump, uc_DrainPump, 1)
//DEV_IO_GPIO_CREATE(Prewash, uc_Prewash, 1)
//DEV_IO_GPIO_CREATE(Mainwash, uc_Mainwash, 1)
//DEV_IO_GPIO_CREATE(GUOut4ATriac1, uc_GUOut4A_Triac1, 1)
//DEV_IO_GPIO_CREATE(GUOut1ATriac1, uc_GUOut1A_Triac1, 1)
//
///* ---------------------------------------------------------- */
///* RCOS ZEROCROSS DETECTION --------------------------------- */
///* ---------------------------------------------------------- */
//#include "../../rcos/dev/ra2/isr.h"
//DEV_CPX_ISR_CREATE(zeroCrossInt, zcIRQStack, zcIrqStackIsr)
//#endif
//
//
///* -------------------------------------------------------------------- */
///* I2C INIT ----------------------------------------------------------- */
///* -------------------------------------------------------------------- */
//#ifndef I2C1_PERIPHERAL_TYPE
//#error Please define the I2C1 type
//#else
//#if (I2C1_PERIPHERAL_TYPE == 1)
//#include "../../rcos/dev/ra2/i2c.h"
//DEV_COM_I2C_CREATE(i2cBus, I2C_Bus, iic_master_callback)
//#elif (I2C1_PERIPHERAL_TYPE == 2)
//#include "../../rcos/dev/ra2/sci_i2c_master.h"
//DEV_COM_I2C_CREATE(i2cBus, I2C_Bus, sci_i2c_master_callback)
//#endif
//#endif
//
//
//#if (I2C_2_AVAILABILITY == 1)
//DEV_COM_I2C_CREATE(i2cBus2, I2C_Bus_2, iic_master_callback_2)
//#endif
///* -------------------------------------------------------------------- */
///* PWT DRIVER --------------------------------------------------------- */
///* -------------------------------------------------------------------- */
//#include "rcos/dev/ra2/pwt2freq.h"
//#include "rcos/dev/ra2/pwt.h"
//
//DEV_IO_PWT_CREATE(waterLevelPwtDriver, waterLevelCounter, 10, PWT_TIMER_CAPTURE_CHANNEL, 1, pwtIsr)
//
//#ifdef PWT_CAC_ACTIVE
//
//#ifndef CAC_CLOCK_RATIO_CONSTANT
//#error Please define CAC_CLOCK_RATIO_CONSTANT
//#endif
//
//DEV_IO_PWT_2_FREQ_CREATE(waterLevelPwt, waterLevelPwtDriver, cacConfig, CAC_CLOCK_RATIO_CONSTANT, 10, TRUE, 2);
//
//#else
//
//DEV_IO_PWT_2_FREQ_CREATE(waterLevelPwt, waterLevelPwtDriver, 10, TRUE, 2);
//
//#endif
//
//
///* ---------------------------------------------------------- */
///* MEMORY DEVICE -------------------------------------------- */
///* ---------------------------------------------------------- */
//#include "../../rcos/dev/ra2/data_flash.h"
//DEV_CPX_FLASH_CREATE(flashCoreDevice, g_flash0, g_cgc0)
//#if (BOOTLOADER_AVAILABLE == COMP_AVAILABLE)
//#define EMULATED_EEPROM_START_ADDRESS (0x04000)
//#else
//#define EMULATED_EEPROM_START_ADDRESS (0x39C00)
//#endif
//#pragma location = EMULATED_EEPROM_START_ADDRESS
////pragma location dan dolayı emulated_eeprom_1_section a gerek kalmamakta, olması da birseyi etkilememekte
//DEV_MEM_SECTION_CREATE(emulatedEepromSection, ".emulated_eeprom_1_section", 1024, flashCoreDevice)
//
//void arch_init()
//{
//    PLATFORM_ASM("NOP");
//}
//
//#endif /* _RENESAS_RA2_INIT_H_ */
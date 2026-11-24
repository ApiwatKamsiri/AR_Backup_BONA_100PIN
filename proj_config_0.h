/** @file       proj_config.h
 *  @brief      PROJ_CONFIG application header file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Umit Sahin
 *  @date       23.10.2020
 */
#ifndef FILE_PROJ_CONFIG_H
#define FILE_PROJ_CONFIG_H

#define RCOS_PLATFORM_SH32F9
#define HARDWARE_VERSION (0)
#define VERSION_BUILD (0)
#define ADDRESS_TYPE_256K
#define ADC_UNSIGNED_RESOLUTION (10)
#define EXTERNAL_UI_AVAILABLE

#define SAFIR_AVAILABILITY (0)   // 0: NA, 1: AVAILABLE
#define BOOTLOADER_AVAILABLE  (1)
#define UDAQ_AVAILABILITY (1)   // 0: NA, 1: AVAILABLE
#define BLUETOOTH_AVAIALBLE (0)
#define I2C_2_AVAILABILITY (0) // 0: NA, 1: AVAILABLE

/** @} */

#endif // FILE_UI_CONFIG_H

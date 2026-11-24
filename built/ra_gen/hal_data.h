/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_wdt.h"
#include "r_wdt_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_flash_lp.h"
#include "r_flash_api.h"
#include "r_spi.h"
#include "r_sci_uart.h"
            #include "r_uart_api.h"
#include "r_iic_master.h"
#include "r_i2c_master_api.h"
#include "r_adc.h"
#include "r_adc_api.h"
FSP_HEADER
/** Timer on GPT Instance. */
extern const timer_instance_t ASOUT_T;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t ASOUT_T_ctrl;
extern const timer_cfg_t ASOUT_T_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** WDT on WDT Instance. */
extern const wdt_instance_t g_wdt0;

/** Access the WDT instance using these structures when calling API functions directly (::p_api is not used). */
extern wdt_instance_ctrl_t g_wdt0_ctrl;
extern const wdt_cfg_t g_wdt0_cfg;

#ifndef NULL
void NULL(wdt_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t FFAN_B;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t FFAN_B_ctrl;
extern const timer_cfg_t FFAN_B_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t RFAN_B;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t RFAN_B_ctrl;
extern const timer_cfg_t RFAN_B_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t R_LED_A;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t R_LED_A_ctrl;
extern const timer_cfg_t R_LED_A_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t B_LED_A_G_LED_B;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t B_LED_A_G_LED_B_ctrl;
extern const timer_cfg_t B_LED_A_G_LED_B_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t BTLED_A_XLED_B;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t BTLED_A_XLED_B_ctrl;
extern const timer_cfg_t BTLED_A_XLED_B_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t LED_DRIVE;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t LED_DRIVE_ctrl;
extern const timer_cfg_t LED_DRIVE_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t BUZ_BUZZER;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t BUZ_BUZZER_ctrl;
extern const timer_cfg_t BUZ_BUZZER_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t COMP_OUT_T;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t COMP_OUT_T_ctrl;
extern const timer_cfg_t COMP_OUT_T_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/* Flash on Flash LP Instance. */
extern const flash_instance_t EEPROM;

/** Access the Flash LP instance using these structures when calling API functions directly (::p_api is not used). */
extern flash_lp_instance_ctrl_t EEPROM_ctrl;
extern const flash_cfg_t EEPROM_cfg;

#ifndef bgo_callback
void bgo_callback(flash_callback_args_t * p_args);
#endif
/** SPI on SPI Instance. */
extern const spi_instance_t DOOR_SPI_BUS;

/** Access the SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern spi_instance_ctrl_t DOOR_SPI_BUS_ctrl;
extern const spi_cfg_t DOOR_SPI_BUS_cfg;

/** Callback used by SPI Instance. */
#ifndef spi_callback
void spi_callback(spi_callback_args_t * p_args);
#endif


#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
    #define DOOR_SPI_BUS_P_TRANSFER_TX (NULL)
#else
    #define DOOR_SPI_BUS_P_TRANSFER_TX (&RA_NOT_DEFINED)
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
    #define DOOR_SPI_BUS_P_TRANSFER_RX (NULL)
#else
    #define DOOR_SPI_BUS_P_TRANSFER_RX (&RA_NOT_DEFINED)
#endif
#undef RA_NOT_DEFINED
/** UART on SCI Instance. */
            extern const uart_instance_t      WIFI_UART_BUS;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     WIFI_UART_BUS_ctrl;
            extern const uart_cfg_t WIFI_UART_BUS_cfg;
            extern const sci_uart_extended_cfg_t WIFI_UART_BUS_cfg_extend;

            #ifndef wifi_callback
            void wifi_callback(uart_callback_args_t * p_args);
            #endif
/** UART on SCI Instance. */
            extern const uart_instance_t      UDAQ_UART_BUS;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     UDAQ_UART_BUS_ctrl;
            extern const uart_cfg_t UDAQ_UART_BUS_cfg;
            extern const sci_uart_extended_cfg_t UDAQ_UART_BUS_cfg_extend;

            #ifndef udaq_callback
            void udaq_callback(uart_callback_args_t * p_args);
            #endif
/** UART on SCI Instance. */
            extern const uart_instance_t      COMP_UART_BUS;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     COMP_UART_BUS_ctrl;
            extern const uart_cfg_t COMP_UART_BUS_cfg;
            extern const sci_uart_extended_cfg_t COMP_UART_BUS_cfg_extend;

            #ifndef comp_callback
            void comp_callback(uart_callback_args_t * p_args);
            #endif
/* I2C Master on IIC Instance. */
extern const i2c_master_instance_t EEPROM_I2C_BUS;

/** Access the I2C Master instance using these structures when calling API functions directly (::p_api is not used). */
extern iic_master_instance_ctrl_t EEPROM_I2C_BUS_ctrl;
extern const i2c_master_cfg_t EEPROM_I2C_BUS_cfg;

#ifndef EEPROM_i2c_callback
void EEPROM_i2c_callback(i2c_master_callback_args_t * p_args);
#endif
/** ADC on ADC Instance. */
extern const adc_instance_t ADC_12_BUS;

/** Access the ADC instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_instance_ctrl_t ADC_12_BUS_ctrl;
extern const adc_cfg_t ADC_12_BUS_cfg;
extern const adc_channel_cfg_t ADC_12_BUS_channel_cfg;

#ifndef adcIsr
void adcIsr(adc_callback_args_t * p_args);
#endif

#ifndef NULL
#define ADC_DMAC_CHANNELS_PER_BLOCK_NULL  15
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */

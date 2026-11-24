/** @file       proj_config.h
 *  @brief      PROJ_CONFIG application header file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Emre Onder
 *  @date       07.06.2021
 */
#ifndef FILE_PROJ_CONFIG_H
#define FILE_PROJ_CONFIG_H
 
#define RCOS_PLATFORM_RA2
#define ADDRESS_TYPE_256K
//#define VOLTAGE_TYPE_120V				
#define ADC_UNSIGNED_RESOLUTION (12)
#define HARDWARE_VERSION (0) //dilas
#define VERSION_BUILD	(0)

#define ICESOURCEavailable
#define DISPENSERavailable


//Renesas Macros
#define RASSERT_LEVEL   (RASSERT_LEVEL_1_LOG)
#ifdef FILE_ADC_C
    #define RASSERT_LEVEL   (RASSERT_LEVEL_1_LOG)
#endif
#define ASSERT_FILE_NAME_SIZE	(8)
#define ASSERT_LOG_QUEUE_SIZE	(3)

//Renesas Macros
#define I2C1_PERIPHERAL_TYPE        (2)// 1: iic, 2: sci
#define REED_SWITCH_AVAILABILITY    (0)
#define AQUA_STOP_AVAILABILITY      (0)
#define CAC_CLOCK_RATIO_CONSTANT    (13107UL)
#define MB_ENABLE_ACTIVE_TRIGGER    (1)

/*	--	ENCODER	--	*/
#define PROJ_ENC_TYPE (2) // 0: NONE, 1: Incremental, 2: Absolute
#define ENCODER_DRIVEN_BY_MUX (1)
#define PROJ_ENC_NUM_OF_ROTARY_POSITION (16)

/*	--	BUTTONS	--	*/
#define PROJ_BUTTON_NUM_OF_BUTTONS (8)
#define BUTTON_PRESS_8 (UIC_TEMPERATURE) 
#define BUTTON_PRESS_7 (UIC_SPIN)
#define BUTTON_PRESS_6 (UIC_PASSIVE)
#define BUTTON_PRESS_5 (UIC_START_PAUSE)
#define BUTTON_PRESS_4 (UIC_TIME_DELAY)
#define BUTTON_PRESS_3 (UIC_OPTION_3_1)
#define BUTTON_PRESS_2 (UIC_OPTION_2_1)
#define BUTTON_PRESS_1 (UIC_OPTION_1_1)
#define COMB_PRESS_1 (UIC_ERROR_MODE)
//#define COMB_PRESS_2 (UIC_WIFI_CONFIG)	
#define COMB_PRESS_2 (UIC_PASSIVE)
#define COMB_PRESS_3 (UIC_SERVICE_USER_INFO)
//#define COMB_PRESS_4 (UIC_HOMEWHIZ_RESET)
#define COMB_PRESS_4 (UIC_PASSIVE)
#define BUTTON_COMB_1 (BUTTON_1_VALUE | BUTTON_3_VALUE)
#define BUTTON_COMB_2 (BUTTON_8_VALUE | BUTTON_5_VALUE)
#define BUTTON_COMB_3 (BUTTON_1_VALUE | BUTTON_2_VALUE)
#define BUTTON_COMB_4 (BUTTON_4_VALUE | BUTTON_5_VALUE)

#define UIC_ABSOLUTE_ENC_SERVICE_MODE_ENTRY_BUTTON (UIC_TEMPERATURE)
#define UIC_ABSOLUTE_ENC_AEA_MODE_ENTRY_BUTTON (UIC_SPIN)

#define BUTTON_TOTAL_SCAN_TIME (4)

#define DOOR_SWITCH_AVAILABILITY (0) // 0: NA, 1: AVAILABLE 
#define SAFIR_AVAILABILITY (0)
#define BOOTLOADER_AVAILABLE (0)
#define UDAQ_AVAILABILITY (1)
#define I2C_2_AVAILABILITY (0) // 0: NA, 1: AVAILABLE
#define BLUETOOTH_AVAIALBLE (0)
#define WIFI1_LED_ROW (1)
#define WIFI1_LED_COL (3)
#define DRYER_NTC_AVAILABILITY (1) // 0: NA, 1: AVAILABLE
#define PHONE_CONNECT_LED_ROW (0)
#define PHONE_CONNECT_LED_COL (3)
#define FLOWMETER_2_AVAILABILITY (1)	// HotWater FM AVAILABLE
#define DRUM_LED_AVAILABILITY (1) // 0: NA, 1: AVAILABLE, 2: Virtual
#define DOOR_LED_AVAILABILITY (0) // 0: NA, 1: AVAILABLE
#define WATER_LEVEL_SWITCH_AVAIALBLE (1) // 0: NA, 1: AVAILABLE
#define DOOR_SWITCH_AVAILABILITY_ON_MB (0)	// 0: NA, 1: AVAILABLE
#define MB_ENABLE_AVAILABILITY (1)	// 0: NA, 1: AVAILABLE, 2: Virtual
#define FT_ENTERENCE_TYPE (0)	// 0: CRYPTION and START-PAUSE, 1: CRYPTION and FAST-ENTERENCE
#define SHOW_WATER_CUT_ERROR_CODE (0)
#define I2C_ENABLE_ACTIVE_TRIGGER (0)
#define BLUETOOTH_AVAILABLE (0)
#define PWT_TIMER_CAPTURE_CHANNEL (TIMER_EVENT_CAPTURE_B)
#define MATRIX_DRIVING_TYPE (2) // 0: NONE, 1: GPIO, 2: MUX, 3: TWO MUX
#define FIRST_MUX_NUMBER_OF_CASCADES (1)
#define MATRIX_DRIVING_NUMBER_OF_COL (7)
#define MATRIX_DRIVING_NUMBER_OF_ROW (8)

#define MUX_IO_DIRECT_DRIVING (0) 

#define PROJ_WATER_LEVEL_SENSOR_TYPE (1) // 0: None, 1:Freq Input, 2: Digital

/*	--	TIME DISPLAY	--	*/
#define TIME_DISP_NUMBER_OF_DIGITS (3)
// MIN-0
#define TIME_DISP_MIN_0_A_ROW (1)
#define TIME_DISP_MIN_0_A_COL (1)
#define TIME_DISP_MIN_0_B_ROW (3)
#define TIME_DISP_MIN_0_B_COL (2)
#define TIME_DISP_MIN_0_C_ROW (2)
#define TIME_DISP_MIN_0_C_COL (2)
#define TIME_DISP_MIN_0_D_ROW (5) 
#define TIME_DISP_MIN_0_D_COL (2)
#define TIME_DISP_MIN_0_E_ROW (4)
#define TIME_DISP_MIN_0_E_COL (2)
#define TIME_DISP_MIN_0_F_ROW (6)
#define TIME_DISP_MIN_0_F_COL (2)
#define TIME_DISP_MIN_0_G_ROW (7)
#define TIME_DISP_MIN_0_G_COL (2)

// MIN-1
#define TIME_DISP_MIN_1_A_ROW (6)
#define TIME_DISP_MIN_1_A_COL (1)
#define TIME_DISP_MIN_1_B_ROW (0)
#define TIME_DISP_MIN_1_B_COL (1)
#define TIME_DISP_MIN_1_C_ROW (3)
#define TIME_DISP_MIN_1_C_COL (1)
#define TIME_DISP_MIN_1_D_ROW (2)
#define TIME_DISP_MIN_1_D_COL (1)
#define TIME_DISP_MIN_1_E_ROW (5)
#define TIME_DISP_MIN_1_E_COL (1)
#define TIME_DISP_MIN_1_F_ROW (7)
#define TIME_DISP_MIN_1_F_COL (1)
#define TIME_DISP_MIN_1_G_ROW (4)
#define TIME_DISP_MIN_1_G_COL (1)

// HOUR-0
#define TIME_DISP_HOUR_0_A_ROW (7)
#define TIME_DISP_HOUR_0_A_COL (0)
#define TIME_DISP_HOUR_0_B_ROW (6)
#define TIME_DISP_HOUR_0_B_COL (0)
#define TIME_DISP_HOUR_0_C_ROW (0)
#define TIME_DISP_HOUR_0_C_COL (0)
#define TIME_DISP_HOUR_0_D_ROW (3)
#define TIME_DISP_HOUR_0_D_COL (0)
#define TIME_DISP_HOUR_0_E_ROW (2)
#define TIME_DISP_HOUR_0_E_COL (0)
#define TIME_DISP_HOUR_0_F_ROW (4)
#define TIME_DISP_HOUR_0_F_COL (0)
#define TIME_DISP_HOUR_0_G_ROW (5)
#define TIME_DISP_HOUR_0_G_COL (0)
// COLON
#define TIME_DISP_COLON_ROW (1)
#define TIME_DISP_COLON_COL (0)

/*	--	SPIN DISPLAY	--	*/
#define SPIN_DISP_TYPE (1) // 0: NONE, 1: LEDS, 2: 7SEG
#define SPIN_NUM_OF_LEDS (6)
//#define SPIN_NUM_OF_LEDS (5)
#define SPIN_LED_0_ROW (3)//0 
#define SPIN_LED_0_COL (5)
#define SPIN_LED_1_ROW (2)//RH
#define SPIN_LED_1_COL (5)
#define SPIN_LED_2_ROW (5)//800
#define SPIN_LED_2_COL (5)
#define SPIN_LED_3_ROW (4)//1000
#define SPIN_LED_3_COL (5)
#define SPIN_LED_4_ROW (7)//1200	
#define SPIN_LED_4_COL (5)
#define SPIN_LED_5_ROW (6)//1400
#define SPIN_LED_5_COL (5)


/*	--	TEMPERATURE DISPLAY	--	*/
#define TEMP_DISP_TYPE (1) // 0: NONE, 1: LEDS, 2: 7SEG
#define TEMP_NUM_OF_LEDS (6)
//#define TEMP_NUM_OF_LEDS (5)
#define TEMP_LED_1_ROW (2)//20
#define TEMP_LED_1_COL (6)
#define TEMP_LED_2_ROW (5)//30
#define TEMP_LED_2_COL (6)
#define TEMP_LED_3_ROW (4)//40
#define TEMP_LED_3_COL (6)
#define TEMP_LED_4_ROW (7)//60
#define TEMP_LED_4_COL (6)
#define TEMP_LED_5_ROW (6)//90
#define TEMP_LED_5_COL (6)
#define TEMP_LED_COLD_ROW (3)
#define TEMP_LED_COLD_COL (6) 

/*	--	PROGRAM NUMBER LEDS	--	*/
#define PROG_NO_DISP_TYPE (0) // 0: NONE, 1: LEDS, 2: 7SEG

/*	--	AUX LEDS	--	*/
#define AUX_NUMBER_OF_BUTTONS (3)
#define AUX_LED_1_1_ROW (1)
#define AUX_LED_1_1_COL (5)
#define AUX_LED_1_2_ROW (0)
#define AUX_LED_1_2_COL (5)

#define AUX_LED_2_1_ROW (4)
#define AUX_LED_2_1_COL (4)
#define AUX_LED_2_2_ROW (5)
#define AUX_LED_2_2_COL (4)

#define AUX_LED_3_1_ROW (2)
#define AUX_LED_3_1_COL (4)
#define AUX_LED_3_2_ROW (1)
#define AUX_LED_3_2_COL (4)

//#define AUX_LED_4_1_ROW (0)
//#define AUX_LED_4_1_COL (3)
//#define AUX_LED_4_2_ROW (1)
//#define AUX_LED_4_2_COL (3)

/*	--	CUSTOM LEDS	--	*/
#define CHILD_LOCK_LED_AVAILABILITY (1) // 0: NA, 1: AVAILABLE 
#define CHILD_LOCK_ROW (3)
#define CHILD_LOCK_COL (4)
#define DOOR_LOCK_LED_AVAILABILITY (1) // 0: NA, 1: AVAILABLE 
#define DOOR_LOCK_ROW (0)
#define DOOR_LOCK_COL (2)
#define TIME_DELAY_LED_AVAILABILITY (1) // 0: NA, 1: AVAILABLE 
#define TIME_DELAY_ROW (0)
#define TIME_DELAY_COL (4)
#define ADD_GARMENT_LED_AVAILABILITY (0) // 0: NA, 1: AVAILABLE 
//#define ADD_GARMENT_LED_AVAILABILITY (1) // 0: NA, 1: AVAILABLE 
//#define ADD_GARMENT_ROW (6) 
//#define ADD_GARMENT_COL (3) 
#define NO_WATER_LED_AVAILABILITY (1) // 0: NA, 1: AVAILABLE 
//#define NO_WATER_ROW (7) 
//#define NO_WATER_COL (3) 
#define NO_WATER_ROW (2) 
#define NO_WATER_COL (3) 

/*	--	PROGRAM FOLLOWER LEDS	--	*/
#define WASHING_STAGE_NUMBER_OF_STAGES (4)
//#define WASHING_STAGE_NUMBER_OF_STAGES (5)
#define WASHING_STAGE_1_ROW (1)//play
#define WASHING_STAGE_1_COL (2)
#define WASHING_STAGE_2_ROW (5)//wash
#define WASHING_STAGE_2_COL (3)
#define WASHING_STAGE_3_ROW (4)//rinse
#define WASHING_STAGE_3_COL (3)
#define WASHING_STAGE_4_ROW (3)//spin
#define WASHING_STAGE_4_COL (3)
//#define WASHING_STAGE_5_ROW (2)//acp
//#define WASHING_STAGE_5_COL (3)
/** @} */

#endif // FILE_UI_CONFIG_H
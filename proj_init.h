/** @file       proj_init.h
 *  @brief      PROJ_INIT application header file
 *  @copyright  (c) 2021-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Vehbi Umur Cabuk
 *  @date       29.06.2021
 */

#ifndef FILE_PROJ_INIT_H
#define FILE_PROJ_INIT_H
#include "rcos/dev/ra2/gpio.h"
#include "rcos/dev/ra2/pwm.h"
// #include "rcos/dev/psoc6/isr.h"
#include "rcos/dev/ra2/touch_cap.h"

/* -------------------------------------------------------------------- */
/* GPIO INIT ---------------------------------------------------------- */
/* -------------------------------------------------------------------- */
DEV_IO_GPIO_CREATE(serial_data, uc_SerialClk, 1)
DEV_IO_GPIO_CREATE(storage_clk, uc_StorageClk, 1)
DEV_IO_GPIO_CREATE(shift_clk, uc_ShiftClk, 1)

DEV_IO_GPIO_CREATE(uc_Enc_Common, uc_ENC_Value, 1)


// DEV_IO_GPIO_CREATE(serial_data_rot, uc_RotarySerialClk, 1)
// DEV_IO_GPIO_CREATE(storage_clk_rot, uc_RotaryStorageClk, 1)
// DEV_IO_GPIO_CREATE(shift_clk_rot, uc_RotaryShiftClk, 1)
// DEV_IO_GPIO_CREATE(rotaryCol_0, uc_RotaryCol_0, 0)
// DEV_IO_GPIO_CREATE(rotaryCol_1, uc_RotaryCol_1, 0)
// DEV_IO_GPIO_CREATE(enc_1, uc_Rotary_1, 1)
// DEV_IO_GPIO_CREATE(enc_2, uc_Rotary_2, 1)
/* -------------------------------------------------------------------- */
/* ISR INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */
// DEV_CPX_ISR_CREATE(irqRot, isrRotary_1)
/* -------------------------------------------------------------------- */
/* PWM INIT ----------------------------------------------------------- */
/* -------------------------------------------------------------------- */
 DEV_CPX_PWM_CORE_CREATE(buzzer_cpx, buzzerCounter, B , 1000000, 50)
 DEV_IO_PWM_FREQ_CREATE(buzzer_freq, buzzer_cpx)
 DEV_IO_PWM_DUTY_CREATE(buzzer_duty, buzzer_cpx)  

/* -------------------------------------------------------------------- */
/* BUTTON INIT -------------------------------------------------------- */
/* -------------------------------------------------------------------- */
//DEV_IO_CAPSENSE_CREATE(sysCapSenseButtons , BUTTON_TOTAL_SCAN_TIME)
DEV_IO_TOUCH_CAP_CREATE(sysCapSenseButtons, g_qe_touch_instance_config01, 25)


/* -------------------------------------------------------------------- */
/* DIRECT DRIVE ------------------------------------------------------- */
/* -------------------------------------------------------------------- */
void directDriveIo(teDirectDrive ioEnum, uint8_t state)
{
  /* NOTE: Fill this function to boost you code in speed. When the directDrive feature is enabled,
       some often used IOs is driven by this function.
       Be careful and put "!" sign for active low IOs.
  */
}


/* -------------------------------------------------------------------- */
/* CAPSENSE BUTTON DIFF VALUES ---------------------------------------- */
/* -------------------------------------------------------------------- */
uint16_t getButtonDiffValues(uint8_t pickOne) // for debugging purposes only.
{
  /* NOTE: Fill this function to increase debugging abilities over UDAQ.
       Enter the return parameter for each button.
       Let the function return zero for unavailable buttons.
  */
  return 0;
}

#endif /* FILE_PROJ_INIT_H */
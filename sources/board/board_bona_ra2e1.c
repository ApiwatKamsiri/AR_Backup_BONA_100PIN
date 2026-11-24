
#include "board.h"
//cdis #include "fsl_port.h"
#include "../app/refrigerator/rf_ram.h"

/*************************************    AC Outputs    *****************************************/
//cdis #include "dev/ke14/gpio.h"
#include "rcos/dev/ra2/gpio.h"       //cadd
#include "rcos/dev/ra2/data_flash.h"
#define SOLVER          0x0200       // port 200 is unused pins that's why i can use for solver

DEV_IO_GPIO_STACK(g_ioport)

DEV_IO_GPIO_CREATE(gpioAcFdef, F_DEF, 1)
DEV_IO_GPIO_CREATE(gpioAcImheat, IM_HEAT, 1)
DEV_IO_GPIO_CREATE(gpioAcRdef, R_DEF, 1)
DEV_IO_GPIO_CREATE(gpioAcSpheat, SP_HEAT, 1)
DEV_IO_GPIO_CREATE(gpioAcRpheat, RP_HEAT, 1)

#include "dev/ac_output.h"

DEV_IO_AC_OUTPUT_CREATE(acFdef, gpioAcFdef)
DEV_IO_AC_OUTPUT_CREATE(acImheat, gpioAcImheat)
DEV_IO_AC_OUTPUT_CREATE(acRdef, gpioAcRdef)
DEV_IO_AC_OUTPUT_CREATE(acSpheat, gpioAcSpheat)
DEV_IO_AC_OUTPUT_CREATE(acRpheat, gpioAcRpheat)

DEV_IO_AC_OUTPUT_CONTROLLER_CREATE(acOutputsController, 20 /*Screening time*/, &acFdef, &acImheat, &acRdef, &acSpheat, &acRpheat)

const tsDevIo *acOutputs[] = {
	&acFdef,
	&acImheat,
	&acRdef,
	&acSpheat,
        &acRpheat,
};


static tsAcOutputConfig acOutputCompConfigs = {.participateInBlocking = false, .blockingTimeMs = 0};
static tsAcOutputConfig acOutputDelayableConfigs = {.participateInBlocking = false, .blockingTimeMs = 0};
static tsAcOutputConfig acOutputNotDelayableConfigs = {.participateInBlocking = false, .blockingTimeMs = 0};


///*************************************    Digital Inputs    *****************************************/

DEV_IO_GPIO_CREATE(digInDOORR, DOORR, 0)
DEV_IO_GPIO_CREATE(digInDOORF, DOORF, 0)
DEV_IO_GPIO_CREATE(digInIMLSW, IMLSW, 0)
DEV_IO_GPIO_CREATE(digInFFG, FFG, 0)
DEV_IO_GPIO_CREATE(digInRFG, RFG, 0)  
DEV_IO_GPIO_CREATE(digInTEST1, TEST1, 0)
DEV_IO_GPIO_CREATE(digInTEST2, TEST2, 0)
DEV_IO_GPIO_CREATE(digInSOLVER, SOLVER, 0)



/******/

/*
DEV_IO_GPIO_CREATE(revDigInDOORR, DOORR, 0)
DEV_IO_GPIO_CREATE(revDigInDOORF, DOORF, 0)
DEV_IO_GPIO_CREATE(revDigInIMLSW, IMLSW, 0)
DEV_IO_GPIO_CREATE(revDigInFFG, FFG, 0)
DEV_IO_GPIO_CREATE(revDigInRFG, RFG, 0)  
DEV_IO_GPIO_CREATE(revDigInTEST1, TEST1, 0)
DEV_IO_GPIO_CREATE(revDigInTEST2, TEST2, 0)
DEV_IO_GPIO_CREATE(revDigInSOLVER, SOLVER, 0)
*/
/*****/


const tsDevIo *digInputs[] = {
        &digInDOORR,
        &digInDOORF,
        &digInIMLSW,     
        &digInFFG,
        &digInRFG,
        &digInTEST1,  
        &digInTEST2,
        &digInSOLVER,
};
/*
const tsDevIo *revDigInputs[] = {
        &revDigInDOORR,
        &revDigInDOORF,
        &revDigInIMLSW,     
        &revDigInFFG,
        &revDigInRFG,
        &revDigInTEST1,  
        &revDigInTEST2, 
        &revDigInSOLVER,
};
*/



#include "dev/debounce_patch.h"
DEV_CPX_DEBOUNCE_CREATE(debounceCore, 20,
                                                DEBOUNCE_ITEM(digInDOORR, 5),
                                                DEBOUNCE_ITEM(digInDOORF, 5),
                                                DEBOUNCE_ITEM(digInIMLSW, 5),
                                                DEBOUNCE_ITEM(digInFFG, 5),
                                                DEBOUNCE_ITEM(digInRFG, 5),
                                                DEBOUNCE_ITEM(digInTEST1, 5),
                                                DEBOUNCE_ITEM(digInTEST2, 5),
                                                DEBOUNCE_ITEM(digInSOLVER, 5)/*,
						
                                                // Reversed inputs
                                                DEBOUNCE_ITEM(revDigInDOORR, 5),
                                                DEBOUNCE_ITEM(revDigInDOORF, 5),
                                                DEBOUNCE_ITEM(revDigInIMLSW, 5),
                                                DEBOUNCE_ITEM(revDigInFFG, 5),
                                                DEBOUNCE_ITEM(revDigInRFG, 5),
                                                DEBOUNCE_ITEM(revDigInTEST1, 5),
                                                DEBOUNCE_ITEM(revDigInTEST2, 5),
                                                DEBOUNCE_ITEM(revDigInSOLVER, 5)*/) 

                                                

DEV_IO_DEBOUNCE_CREATE(debDOORR, debounceCore, 0)
DEV_IO_DEBOUNCE_CREATE(debDOORF, debounceCore, 1)
DEV_IO_DEBOUNCE_CREATE(debIMLSW, debounceCore, 2)
DEV_IO_DEBOUNCE_CREATE(debFFG, debounceCore, 3)
DEV_IO_DEBOUNCE_CREATE(debRFG, debounceCore, 4)
DEV_IO_DEBOUNCE_CREATE(debTEST1, debounceCore, 5)
DEV_IO_DEBOUNCE_CREATE(debTEST2, debounceCore, 6)
DEV_IO_DEBOUNCE_CREATE(debSOLVER, debounceCore, 7)


const tsDevIo *debDigInputs[] = {
        &debDOORR,
        &debDOORF,
        &debIMLSW,
        &debFFG,
        &debRFG,
        &debTEST1,
        &debTEST2,
        &debSOLVER,
};
/*
// Reversed debounced digital inputs
DEV_IO_DEBOUNCE_CREATE(debRevDOORR, debounceCore, 8)
DEV_IO_DEBOUNCE_CREATE(debRevDOORF, debounceCore, 9)
DEV_IO_DEBOUNCE_CREATE(debRevIMLSW, debounceCore, 10)
DEV_IO_DEBOUNCE_CREATE(debRevFFG, debounceCore, 11)
DEV_IO_DEBOUNCE_CREATE(debRevRFG, debounceCore, 12)
DEV_IO_DEBOUNCE_CREATE(debRevTEST1, debounceCore, 13)
DEV_IO_DEBOUNCE_CREATE(debRevTEST2, debounceCore, 14)
DEV_IO_DEBOUNCE_CREATE(debRevSOLVER, debounceCore, 15)

const tsDevIo *debRevDigInputs[] = {
	&debRevDOORR,
	&debRevDOORF,
        &debRevIMLSW,
        &debRevFFG,
        &debRevRFG,
        &debRevTEST1,
        &debRevTEST2,
        &debRevSOLVER,
};*/


///*************************************    Digital Outputs    *****************************************/

//DEV_IO_GPIO_CREATE(digOutFFLight, GPIOC, 6, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
//DEV_IO_GPIO_CREATE(digOutFrzLight, GPIOD, 10, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
//DEV_IO_GPIO_CREATE(digOutFFFanDC, GPIOA, 3, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
//DEV_IO_GPIO_CREATE(digOutFFFanPower, GPIOA, 0, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)

DEV_IO_GPIO_CREATE(digOutRM_DC, RM, 1)
DEV_IO_GPIO_CREATE(digOutFM_DC, FM, 1)
DEV_IO_GPIO_CREATE(digOutKM_DC, KM, 1)
DEV_IO_GPIO_CREATE(digOutIM_DC, IM_FAN, 1)
DEV_IO_GPIO_CREATE(digOutRM_POWER, RM, 1)
DEV_IO_GPIO_CREATE(digOutFM_POWER, FM, 1)
DEV_IO_GPIO_CREATE(digOutKM_POWER, KM, 1)
DEV_IO_GPIO_CREATE(digOutIM_POWER, IM_FAN, 1)
DEV_IO_GPIO_CREATE(digOutRLED, RLED, 1)
DEV_IO_GPIO_CREATE(digOutFLED, FLED, 1)
DEV_IO_GPIO_CREATE(digOutIM_PUMP_CW, IM_PUMP_CW, 1)
DEV_IO_GPIO_CREATE(digOutIM_PUMP_CCW, IM_PUMP_CCW, 1)
DEV_IO_GPIO_CREATE(digOutIM_MOTOR_CW, IM_MOTOR_CW, 1)
DEV_IO_GPIO_CREATE(digOutIM_MOTOR_CCW, IM_MOTOR_CCW, 1)
DEV_IO_GPIO_CREATE(vccDriveEn, INV_SHUTDOWN, 1)
DEV_IO_GPIO_CREATE(digOutARM, ARM, 1)   


#include "dev/bdc_motor.h"
DEV_IO_BDC_MOTOR_CREATE(bdcIceMotor, digOutIM_MOTOR_CW, digOutIM_MOTOR_CCW, 500)
DEV_IO_BDC_MOTOR_CREATE(bdcPumpMotor, digOutIM_PUMP_CW, digOutIM_PUMP_CCW, 500)

const tsDevIo *bdcMotors[] = {
	&bdcIceMotor,
        &bdcPumpMotor,
};

/******/


const tsDevIo *digOutputs[] = {
        &digOutRM_DC,
        &digOutFM_DC,
        &digOutKM_DC,
	&digOutIM_DC,
        &digOutRM_POWER,
        &digOutFM_POWER,
        &digOutKM_POWER,
	&digOutIM_POWER,
	&digOutIM_PUMP_CW,
        &digOutIM_PUMP_CCW,
        &digOutIM_MOTOR_CW,
	&digOutIM_MOTOR_CCW,
        &vccDriveEn,
        &digOutARM, 
};


#include "dev/ra2/gpio.h"

DEV_IO_GPIO_CREATE(motorAp, GA, 1)
DEV_IO_GPIO_CREATE(motorAn, GAX, 1)
DEV_IO_GPIO_CREATE(motorBp, GB, 1)
DEV_IO_GPIO_CREATE(motorBn, GBX, 1)


#include "dev/stepper_motor.h"
#ifdef HOSHI_H30
DEV_IO_STEPPER_MOTOR_CREATE(stepperMotor, 4, motorAp, motorAn, motorBp, motorBn)  // 4*3 = 6ms 6pps     //APW : Setting 4 to same H10
#else
DEV_IO_STEPPER_MOTOR_CREATE(stepperMotor, 11, motorAp, motorAn, motorBp, motorBn)  // 11*3 = 33ms 30pps
#endif
const tsDevIo *stepperMotors[] = {
	&stepperMotor,
};


///*************************************    ADC Inputs    *****************************************/
#include "rcos/dev/ra2/adc.h"       //cadd
DEV_CPX_ADC_CORE_CREATE(adcCore, ADC_12_BUS, adcIsr  , 20 /*refresh time*/
						/* FF Hava RSEN */          ,ADC_CHANNEL_6
						/* FF Eva  RDSEN*/          ,ADC_CHANNEL_8
						/* FRZ Hava FSEN*/          ,ADC_CHANNEL_5
						/* FRZ Eva FDSEN*/          ,ADC_CHANNEL_7
						/* ICE temp IMSEN*/         ,ADC_CHANNEL_2
						/* Amb Temp TCSEN*/         ,ADC_CHANNEL_4
                                                /* Humidity HSEN*/          ,ADC_CHANNEL_9
                                                /* LockFFfan RML*/          ,ADC_CHANNEL_0
                                                /* LockIcefan IMFL*/        ,ADC_CHANNEL_3           
                                                /* LockConfan KML*/         ,ADC_CHANNEL_1
                                                /* LockIMEmpty IMTEL*/      ,ADC_CHANNEL_10)

DEV_IO_ADC_CREATE(adcRSEN, adcCore,  0 )
DEV_IO_ADC_CREATE(adcRDSEN, adcCore, 1 )
DEV_IO_ADC_CREATE(adcFSEN, adcCore,  2 )
DEV_IO_ADC_CREATE(adcFDSEN, adcCore, 3 )
DEV_IO_ADC_CREATE(adcIMSEN, adcCore, 4 )
DEV_IO_ADC_CREATE(adcTCSEN, adcCore, 5 )
DEV_IO_ADC_CREATE(adcHSEN, adcCore,  6 )
DEV_IO_ADC_CREATE(adcRML, adcCore,   7 )
DEV_IO_ADC_CREATE(adcIMFL, adcCore,  8 )
DEV_IO_ADC_CREATE(adcKML, adcCore,   9 )
DEV_IO_ADC_CREATE(adcIMTEL, adcCore, 10)

const tsDevIo *adcs[] = {
	&adcRSEN,
	&adcRDSEN,
	&adcFSEN,
        &adcFDSEN,
        &adcIMSEN,
        &adcTCSEN,    
        &adcHSEN,
        &adcRML,
        &adcIMFL,
        &adcKML,
        &adcIMTEL,
};

/*************************************    NTC Inputs    *****************************************/

#include "dev/filter.h"
DEV_IO_FILTER_CREATE(filteredAdcRSEN, adcRSEN, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcRDSEN, adcRDSEN, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcFSEN, adcFSEN, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcFDSEN, adcFDSEN, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcIMSEN, adcIMSEN, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcTCSEN, adcTCSEN, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcHSEN, adcHSEN, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcRML, adcRML, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcIMFL, adcIMFL, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcKML, adcKML, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcIMTEL, adcIMTEL, 20, 4)


#include "dev/ntc.h"
DEV_IO_NTC_CREATE(ntcRSEN, filteredAdcRSEN)
DEV_IO_NTC_CREATE(ntcRDSEN, filteredAdcRDSEN)
DEV_IO_NTC_CREATE(ntcFSEN, filteredAdcFSEN)
DEV_IO_NTC_CREATE(ntcFDSEN, filteredAdcFDSEN)
DEV_IO_NTC_CREATE(ntcIMSEN, filteredAdcIMSEN)
DEV_IO_NTC_CREATE(ntcTCSEN, filteredAdcTCSEN)
DEV_IO_NTC_CREATE(ntcHSEN, filteredAdcHSEN)
DEV_IO_NTC_CREATE(ntcRML, filteredAdcRML)
DEV_IO_NTC_CREATE(ntcIMFL, filteredAdcIMFL)
DEV_IO_NTC_CREATE(ntcKML, filteredAdcKML)
DEV_IO_NTC_CREATE(ntcIMTEL, filteredAdcIMTEL)


DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorRSEN, ntcRSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorRDSEN, ntcRDSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFSEN, ntcFSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFDSEN, ntcFDSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorIMSEN, ntcIMSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorTCSEN, ntcTCSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorHSEN, ntcHSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorRML, ntcRML)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorIMFL, ntcIMFL)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorKML, ntcKML)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorIMTEL, ntcIMTEL)

const tsDevIo *ntcs[] = {
	&ntcRSEN,
	&ntcRDSEN,
	&ntcFSEN,
	&ntcFDSEN,
	&ntcIMSEN,
	&ntcTCSEN,
	&ntcHSEN,
	&ntcRML,
	&ntcIMFL,
	&ntcKML,
        &ntcIMTEL,
};

const tsDevIo *ntcErrorDetectors[] = {
	&ntcErrorRSEN,
	&ntcErrorRDSEN,
	&ntcErrorFSEN,
	&ntcErrorFDSEN,
	&ntcErrorIMSEN,
	&ntcErrorTCSEN,
	&ntcErrorHSEN,
	&ntcErrorRML,
	&ntcErrorIMFL,
	&ntcErrorKML,
        &ntcErrorIMTEL,
};

//#include "dev/knob.h"
//DEV_IO_KNOB_CREATE(knob, filteredAdcPOT)
//DEV_IO_KNOB_ERROR_DETECTOR_CREATE(knobError, knob)
//const tsDevIo *knobs[] = {
//        &knob
//};
//const tsDevIo *knobErrorDetectors[] = {
//        &knobError
//};

/**
 *
 *
 *
 *
 *
 *
 */



///*************************************    HW PWM Outputs    *****************************************/

#include "dev/ra2/pwm.h"

// KFAN No timer
//DEV_CPX_PWM_CORE_CREATE(kfanPwmCore , RFAN_A_FFAN_B_PWM, B, 600, 0)     // KFAN No timer RFAN_A_FFAN_B_PWM B does not support just wrote like that but can not use pwm
//DEV_IO_PWM_DUTY_CREATE(dutyKFAN, kfanPwmCore)  

// IMFAN No timer
//DEV_CPX_PWM_CORE_CREATE(imfanPwmCore , RFAN_A_FFAN_B_PWM, B , 600, 0)     // IMFAN No timer RFAN_A_FFAN_B_PWM B does not support just wrote like that but can not use pwm
//DEV_IO_PWM_DUTY_CREATE(dutyIMFAN, imfanPwmCore)  

// RFAN Channel A   
DEV_CPX_PWM_CORE_CREATE(rfanPwmCore, RFAN_A_FFAN_B_PWM, B , 32000, 0)     // RFAN Channel A
DEV_IO_PWM_DUTY_CREATE(dutyRFAN, rfanPwmCore)  

// FFAN Channel B
DEV_CPX_PWM_CORE_CREATE(ffanPwmCore, RFAN_A_FFAN_B_PWM, A , 32000, 0)     // FFAN Channel B
DEV_IO_PWM_DUTY_CREATE(dutyFFAN, ffanPwmCore)  


// RLED Channel A
DEV_CPX_PWM_CORE_CREATE(rledPwmCore, LED_DRIVE, B , 600, 0)             // RLED Channel A
DEV_IO_PWM_FREQ_CREATE(rled_freq, rledPwmCore)
DEV_IO_PWM_DUTY_CREATE(rled_duty, rledPwmCore)  

// FLED Channel B
DEV_CPX_PWM_CORE_CREATE(fledPwmCore, LED_DRIVE, A , 600, 0)             // FLED Channel A
DEV_IO_PWM_FREQ_CREATE(fled_freq, fledPwmCore)
DEV_IO_PWM_DUTY_CREATE(fled_duty, fledPwmCore)  

// COMP_OUT Channel A
DEV_CPX_PWM_CORE_CREATE(vccDrivePwmCore, COMP_OUT_T, A , 600, 50)         // COMP_OUT Channel A
DEV_IO_PWM_FREQ_CREATE(comp_out_freq, vccDrivePwmCore)
DEV_IO_PWM_DUTY_CREATE(comp_out_duty, vccDrivePwmCore)  


const tsDevIo *softPwmFreqOutputs[] = {
	&comp_out_freq,
};

const tsDevIo *softPwmDutyOutputs[] = {
	&comp_out_duty,
};


/*************************************    Lights    *****************************************/

#include "dev/fade.h"
DEV_IO_FADE_CREATE(fadingRLED, rled_duty, 1, 10)
DEV_IO_FADE_CREATE(fadingFLED, fled_duty, 1, 10)

const tsDevIo *fadingLights[] = {
	&fadingRLED,
        &fadingFLED,
};


/*************************************    Fans    *****************************************/
#include "dev/fan.h"
//DEV_IO_FAN_CREATE(fanFF, dutyFFFan, digOutFFFanDC, digOutFFFanPower, digInFFFanFg)

DEV_IO_FAN_CREATE(fanR, dutyRFAN, digOutRM_DC, digOutRM_POWER, digInRFG)      //It should be changed FeedBack or NO FeedBack
//DEV_IO_FAN_CREATE(fanR, dutyRFAN, digOutRM_DC, digOutRM_POWER, digInRFG)    //ntcRML   ADC Lock voltage implementation ÖCE
DEV_IO_FAN_CREATE(fanF, dutyFFAN, digOutFM_DC, digOutFM_POWER, digInFFG)
//DEV_IO_FAN_CREATE(fanK, dutyKFAN, digOutKM_DC, digOutKM_POWER, digInSOLVER)          //ntcKML   ADC Lock voltage implementation ÖCE     
//DEV_IO_FAN_CREATE(fanIM, dutyIMFAN, digOutIM_DC, digOutIM_POWER, digInSOLVER)        //ntcIML   ADC Lock voltage implementation ÖCE    


DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorR, fanR)
DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorF, fanF)
/*DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorK, fanK)
DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorIM, fanIM)*/

const tsDevIo *fans[] = {
	&fanR,
        &fanF,
        /*&fanK,
        &fanIM,*/
};

const tsDevIo *fanErrorDetectors[] = {
	&fanErrorR,
        &fanErrorF,
        /*&fanErrorK,
        &fanErrorIM,*/
};

/**
 *
 *
 *
 *
 *
 *
 */

/*************************************    Buzzer    *****************************************/

//DEV_IO_GPIO_CREATE(buzzerHigh, GPIOA, 13, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(buzzerHigh, SOLVER, 1)
/****/

#include "dev/ra2/pwm.h"
//BUZ Channel A  
DEV_CPX_PWM_CORE_CREATE(buzzerPwmCore, BUZ_BUZZER, B , 1000, 50)        //BUZ Channel A
DEV_IO_PWM_FREQ_CREATE(buzzerFreq, buzzerPwmCore)
DEV_IO_PWM_DUTY_CREATE(buzzerDuty, buzzerPwmCore)  


#include "dev/buzzer.h"
//const tsDevIo *target;

DEV_IO_BUZZER_CREATE(buzzer, buzzerHigh , buzzerFreq)


///*************************************    I2C Devices    *****************************************/
///*cdis todo: bu kisim daha sonra aktif edilecek
//#include "dev/ke14/gpio.h"
//DEV_IO_GPIO_CREATE(i2cEn, I2C_ENABLE, 0, 0, 1)
//
//#include "dev/i2c.h"
//DEV_COM_I2C_CREATE(i2cCore, 1, 75000)
#include "dev/24cxx.h"
#include "ra2/iic_i2c_master.h"
DEV_COM_I2C_CREATE( i2cen, EEPROM_I2C_BUS , EEPROM_i2c_callback)
DEV_MEM_24CXX_CREATE(eepromDev, i2cen, 0, 32)
//
//#include "dev/sht3x.h"
//DEV_IO_SHT3X_CORE_CREATE(sht3xCore, i2cCore, 0x44, 50)
//DEV_IO_SHT3X_TEMP_CREATE(sht3xTemp, sht3xCore)
//DEV_IO_SHT3X_HUMIDITY_CREATE(sht3xHum, sht3xCore)
//#include "dev/filter.h"
//DEV_IO_FILTER_CREATE(filteredSht3xTemp, sht3xTemp, 8, 2)
//DEV_IO_FILTER_CREATE(filteredSht3xHum, sht3xHum, 8, 2)
//
//#include "dev/display_spi.h"
//DEV_CPX_DISPLAY_CREATE(display, i2cCore, 20)
//
//#include "dev/24cxx.h"
//static tsTarget eepromTarget;
//DEV_MEM_24CXX_CREATE(eeprom, i2cCore, eepromTarget, 0, 8)
//DEV_IO_GPIO_CREATE(eepromEn, UC_EEP_5V, 0, 0, 1)
//*/
///**
// *
// *
// *
// *
// *
// *
// */
//
/*************************************    UART Devices    *****************************************/

#include "dev/ra2/uart.h"     // Queue size should be checked
DEV_COM_UART_CREATE(udaqUart, UDAQ_UART_BUS, udaq_callback, 256, 256)   // 9600 Baud for udaq
DEV_COM_UART_CREATE(wifiUart, WIFI_UART_BUS, wifi_callback, 256, 256)   // 38400 Baud for wifi card

#include "dev/display_spi.h"
#include "dev/ra2/spi_slave.h"         

DEV_COM_SPI_SLAVE_CREATE(devComSpiSlave, DOOR_SPI_BUS, spi_callback , 30, 30)
DEV_CPX_DISPLAY_SPI_CREATE(display, devComSpiSlave, 30)  // 20 ms olmasi gerekebilir.

/*******/

/**
 *
 *
 *
 *
 *
 *
 */

///*************************************    WIFI Devices    *****************************************/
//// TODO : Fill wifi devices
///*cdis todo: bunlari sonra aktif edersin
//DEV_IO_GPIO_CREATE(wifiCS, UC_SLAVE_SS, 0, 0, 0) // Attention: that the active level of a CS device is 0 !!!
//DEV_IO_GPIO_CREATE(wifiEn, UC_WIFI_POWER, 0, 0, 1)
//*/
//
/**
 *
 *
 *
 *
 *
 *
 */

///*************************************    MEMORY Devices    *****************************************/
#include "dev/ra2/data_flash.h"

/*#if (BOOTLOADER_AVAILABLE == COMP_AVAILABLE)
#define EMULATED_EEPROM_START_ADDRESS (0x04000)
#else
#define EMULATED_EEPROM_START_ADDRESS (0x39C00)
#endif*/
//#pragma location = 0x39C00
//pragma location dan dolayi emulated_eeprom_1_section a gerek kalmamakta, olmasi da birseyi etkilememekte


void boardInitBoard(void)
{

  
	devCpxInit(&adcCore, NULL);
        
	devCpxInit(&buzzerPwmCore, NULL);
	devIoInit(&buzzer, NULL);
	devIoInit(&acOutputsController, NULL);
	devCpxInit(&debounceCore, NULL);
	
        devCpxInit(&ffanPwmCore, NULL);
        devCpxInit(&rfanPwmCore, NULL);
	devCpxInit(&fledPwmCore, NULL);
        devCpxInit(&rledPwmCore, NULL);
	devCpxInit(&vccDrivePwmCore, NULL);
        devCpxInit(&display, NULL);
        devIoInit(&stepperMotor, NULL);
        devIoInit(&bdcIceMotor, NULL);
        devIoInit(&bdcPumpMotor, NULL);
        devMemInit(&eepromDev); // MJI

        /*cdis todo: bunlari sonra aktif edersin
	devIoInit(&i2cEn, NULL);
	devIoPut(&i2cEn, 1);
	devComInit(&i2cCore);

	devIoInit(&eepromEn, NULL);
	devIoPut(&eepromEn, 1);
        */
}

const tsDevIo *boardInitDevice(teBoardDeviceType deviceType, uint8_t deviceIdx, const void *config)
{
	const tsDevIo *dev = NULL;
       
	switch (deviceType)
	{
	case eDeviceTypeACOutputComp:
	case eDeviceTypeACOutputDelayable:
	case eDeviceTypeACOutputNotDelayable:
		if (deviceIdx < ARRAY_SIZE(acOutputs))
		{
			dev = acOutputs[deviceIdx];

			if (deviceType == eDeviceTypeACOutputComp)
				config = &acOutputCompConfigs;
			else if (deviceType == eDeviceTypeACOutputDelayable)
				config = &acOutputDelayableConfigs;
			else
				config = &acOutputNotDelayableConfigs;

		}
		break;

	case eDeviceTypeDigInput:
		if (deviceIdx < ARRAY_SIZE(digInputs))
			dev = digInputs[deviceIdx];
		break;

/*	case eDeviceTypeReversedDigInput:
		if (deviceIdx < ARRAY_SIZE(revDigInputs))
			dev = revDigInputs[deviceIdx];
		break;
*/
	case eDeviceTypeFilteredDigInput:
		if (deviceIdx < ARRAY_SIZE(debDigInputs))
			dev = debDigInputs[deviceIdx];
		break;

/*	case eDeviceTypeFilteredReversedDigInput:
		if (deviceIdx < ARRAY_SIZE(debRevDigInputs))
			dev = debRevDigInputs[deviceIdx];
		break;
*/
	case eDeviceTypeDigOutput:

		if (deviceIdx < ARRAY_SIZE(digOutputs))
		{
			dev = digOutputs[deviceIdx];
                        devIoInit(digOutputs[deviceIdx], NULL);
                        
/*
			if (deviceIdx == 3) // digOutFFFanDC
				PORT_SetPinMux(BOARD_INITPINS_UC_FF_FAN_PORT, BOARD_INITPINS_UC_FF_FAN_PIN, kPORT_MuxAsGpio);
			else if (deviceIdx == 5) // digOutFrzFanDC
				PORT_SetPinMux(BOARD_INITPINS_UC_FRZ_FAN_PORT, BOARD_INITPINS_UC_FRZ_FAN_PIN, kPORT_MuxAsGpio);
			else if (deviceIdx == 7) // digOutCondFanDC
				PORT_SetPinMux(BOARD_INITPINS_UC_CON_FAN_PORT, BOARD_INITPINS_UC_CON_FAN_PIN, kPORT_MuxAsGpio);
			else if (deviceIdx == 9) // digOutJkrFanDC
				PORT_SetPinMux(BOARD_INITPINS_UC_JKR_FAN_PORT, BOARD_INITPINS_UC_JKR_FAN_PIN, kPORT_MuxAsGpio);
*/
		}
		break;

	case eDeviceTypeSoftFreq:
        case eDeviceTypeComm:
          
		if (deviceIdx < ARRAY_SIZE(softPwmFreqOutputs))
		{
			dev = softPwmFreqOutputs[deviceIdx];
			devIoInit(softPwmDutyOutputs[deviceIdx], NULL);
		}

		break;

	case eDeviceTypeFadingLight:

		if (deviceIdx < ARRAY_SIZE(fadingLights))
		{
			dev = fadingLights[deviceIdx];
                        devIoInit(&rled_freq, NULL);
                        devIoInit(&fled_freq, NULL);
		}

		break;

	case eDeviceTypeNTC:
        case eDeviceTypeNTC_R_air:
        case eDeviceTypeNTC_R_eva:
        case eDeviceTypeNTC_F_air:
        case eDeviceTypeNTC_F_eva:
        case eDeviceTypeNTC_Ambient:
        case eDeviceTypeNTC_IM_case:
		if (deviceIdx < ARRAY_SIZE(ntcs))
			dev = ntcs[deviceIdx];
		break;

	case eDeviceTypeNTCErrorDetector:
		if (deviceIdx < ARRAY_SIZE(ntcErrorDetectors))
			dev = ntcErrorDetectors[deviceIdx];
		break;
                
        case eDeviceTypeKnob:
		/*if (deviceIdx < ARRAY_SIZE(ntcs))
			dev = knobs[deviceIdx];*/
		break;

	case eDeviceTypeKnobError:
		/*if (deviceIdx < ARRAY_SIZE(ntcErrorDetectors))
			dev = knobErrorDetectors[deviceIdx];*/
		break;

	case eDeviceTypeFan:

		if (deviceIdx < ARRAY_SIZE(fans))
		{
			dev = fans[deviceIdx];

			/*
			tsFanConfig *fConfig = (tsFanConfig *)config;
			
			if (!fConfig->pwmUsed)
			{
				if (deviceIdx == 0) // digOutFFFanDC
					PORT_SetPinMux(BOARD_INITPINS_UC_FF_FAN_PORT, BOARD_INITPINS_UC_FF_FAN_PIN, kPORT_MuxAsGpio);
				else if (deviceIdx == 1) // digOutFrzFanDC
					PORT_SetPinMux(BOARD_INITPINS_UC_FRZ_FAN_PORT, BOARD_INITPINS_UC_FRZ_FAN_PIN, kPORT_MuxAsGpio);
				else if (deviceIdx == 2) // digOutCondFanDC
					PORT_SetPinMux(BOARD_INITPINS_UC_CON_FAN_PORT, BOARD_INITPINS_UC_CON_FAN_PIN, kPORT_MuxAsGpio);
				else if (deviceIdx == 3) // digOutJkrFanDC
					PORT_SetPinMux(BOARD_INITPINS_UC_JKR_FAN_PORT, BOARD_INITPINS_UC_JKR_FAN_PIN, kPORT_MuxAsGpio);
			}
			*/
		}

		break;

	case eDeviceTypeFanErrorDetector:
		if (deviceIdx < ARRAY_SIZE(fanErrorDetectors))
			dev = fanErrorDetectors[deviceIdx];
		break;

	case eDeviceTypeStepperMotor:
		if (deviceIdx < ARRAY_SIZE(stepperMotors))
			dev = stepperMotors[deviceIdx];
                        //config = &motorConfig;

		break;

	case eDeviceTypeDCMotor:
          //cdis todo: sonra aktif et
		if (deviceIdx < ARRAY_SIZE(bdcMotors))
			dev = bdcMotors[deviceIdx];

		break;

	case eDeviceTypeADC:
		if (deviceIdx < ARRAY_SIZE(adcs))
			dev = adcs[deviceIdx];
		break;

	case eDeviceTypeSHT3XTemp:
          /*cdis todo: burayi sonra duzelt
		devIoInit(&sht3xCore, NULL);
		dev = &filteredSht3xTemp;
*/
		break;

	case eDeviceTypeSHT3XHumidity:
          /*cdis todo: burayi sonra duzelt
		devIoInit(&sht3xCore, NULL);
		dev = &filteredSht3xHum;
*/
		break;

	case eDeviceTypeSHT3XErrorDetector:
          /*cdis todo: burayi sonra duzelt
		dev = &sht3xCore;
*/
                break;
	}

	devIoInit(dev, config);
	return dev;
}

stFTC FTC;


/* BONA
	&acFdef,
	&acImheat,
	&acRdef,
	&acSpheat,
        &acRpheat,
*/
void boardAcOutFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.ACOut.index < ARRAY_SIZE(acOutputs))
      dev = acOutputs[FTC.ACOut.index];

    devIoPut(dev, FTC.ACOut.value);
}

/*BONA
        &digOutRM_DC,
        &digOutFM_DC,
        &digOutKM_DC,
	&digOutIM_DC,
        &digOutRM_POWER,
        &digOutFM_POWER,
        &digOutKM_POWER,
	&digOutIM_POWER,
	&digOutIM_PUMP_CW,
        &digOutIM_PUMP_CCW,
        &digOutIM_MOTOR_CW,
	&digOutIM_MOTOR_CCW,
        &vccDriveEn,
*/
void boardDcOutFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.DCOut.index < ARRAY_SIZE(digOutputs))
      dev = digOutputs[FTC.DCOut.index];

    devIoPut(dev, FTC.DCOut.value);
}

/*BONA
        &digInDOORR,
        &digInDOORF,
        &digInIMLSW,     
        &digInFFG,
        &digInRFG,
        &digInTEST1,  
        &digInTEST2,
        &digInSOLVER,
*/
void boardDcInFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.DCSwitch.index < ARRAY_SIZE(digInputs))
      dev = digInputs[FTC.DCSwitch.index];

    FTC.DCSwitch.value = devIoGet(dev);
}

/*BONA
	&adcRSEN,
	&adcRDSEN,
	&adcFSEN,
        &adcFDSEN,
        &adcIMSEN,
        &adcTCSEN,    
        &adcHSEN,
        &adcRML,
        &adcIMFL,
        &adcKML,
        &adcIMTEL,
*/
void boardAdcFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.ADC_.index < ARRAY_SIZE(adcs))
      dev = adcs[FTC.ADC_.index];

    FTC.ADC_.value = devIoGet(dev);
}

/*BONA
	&buzzer;        
*/
void boardBuzzerFTC(void)
{
    const tsDevIo *dev = NULL;
    static uint8_t buzzerTimer = 0;
    
    dev = &buzzer;
    
    if(buzzerTimer)
      buzzerTimer--;
    else
      buzzerTimer = 50;
    
    if(!buzzerTimer && FTC.Buzzer.value == 1)  
      devIoPut(dev,1);
}

/*BONA
	&softPwmFreqOutputs;        
*/
void boardVccFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.VCC.index < ARRAY_SIZE(softPwmFreqOutputs))
      dev = softPwmFreqOutputs[FTC.VCC.index];
     refrigerator_R.Cooler.components.compressor.value = FTC.VCC.value;
    devIoPut(dev, FTC.VCC.value);
}
/*BONA  OFF       ON (shift) check with ossiloscope
motorAp 0       0 - 1
motorAn 0       1 - 0
motorBp 0       0 - 1
motorBn 0       1 - 0
*/
void boardStepMotorFTC(void)
{
    const tsDevIo *dev = NULL;

    /*if (FTC.StepMotor.index < ARRAY_SIZE(fans))
      dev = fans[FTC.StepMotor.index];*/
    static uint8_t step = 0;
    if(FTC.StepMotor.value == 0)
    {
        dev = &motorAp;
        devIoPut(dev, 0);
        dev = &motorAn;
        devIoPut(dev, 0);
        dev = &motorBp;
        devIoPut(dev, 0);
        dev = &motorBn;
        devIoPut(dev, 0);
        step=0;
    }
    else
    {
        if(step == 0)
        {
            step = 1;
        }
        else
        {
            step = 0;
        }
        dev = &motorAp;
        devIoPut(dev, step);
        dev = &motorAn;
        devIoPut(dev, !step);
        dev = &motorBp;
        devIoPut(dev, step);
        dev = &motorBn;
        devIoPut(dev, !step);
    }

}
/*
rfanPwmCore - vsdc - RM
ffanPwmCore - vsdc - KM
*/

void boardVSDCMotorFTC(void)
{
    const tsDevIo *dev = NULL;

    if (FTC.VSDCMotor.index < ARRAY_SIZE(fans))
      dev = fans[FTC.VSDCMotor.index];

    devIoPut(dev, FTC.VSDCMotor.value);
}


/*

    eBDCMotorStop = 0,
    eBDCMotorRotateCW = 1,
    eBDCMotorRotateCCW = 2,

&bdcIceMotor,
&bdcPumpMotor,
*/

void boardBDCMotorFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.BDCMotor.index < ARRAY_SIZE(bdcMotors))
      dev = bdcMotors[FTC.BDCMotor.index];
    
    devIoPut(dev, FTC.BDCMotor.value);
}

/*	&fadingRLED,
        &fadingFLED,
*/
void boardLedDutyFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.PWMOut.index < ARRAY_SIZE(fadingLights))
      dev = fadingLights[FTC.PWMOut.index];
    
    devIoPut(dev, FTC.PWMOut.value);
}
void boardEepromFTC(void)
{
    const tsDevMem *dev = NULL;
 //  uint32_t * eepvalue = 0x00000000;
    static uint32_t eepval[8] = {0,0,0,0,0,0,0,0};
    static unsigned int addr = 0;
    if (FTC.Eeprom.index < 1)
      dev = &eepromDev;
    if(FTC.Eeprom.value)
    {
      if(addr > 0x0FFF) 
      {
        return;
      }
      devMemWrite(dev, addr, &eepval, 32); 
      addr += 32;  
//      for(unsigned int i = 0; i<1024; i=i+4)
//      {
//        devMemWrite(dev, i, eepvalue, 4); 
//      }
    }
}


void boardInitFTC(void)
{
    const tsDevIo *dev = NULL;
    //AC COMPONENTS GPIO - OUT
    for(uint8_t i = 0; i < ARRAY_SIZE(acOutputs); i++)
    {
      dev = acOutputs[i];
      devIoInit(dev, NULL);
      devIoPut(dev,0);
    }

    //DC COMPONENTS GPIO - OUT
    for(uint8_t i = 0; i < ARRAY_SIZE(digOutputs); i++)
    {
      dev = digOutputs[i];
      devIoInit(dev, NULL);
      devIoPut(dev,0);
    }
    
    //DC COMPONENTS GPIO - IN
    for(uint8_t i = 0; i < ARRAY_SIZE(digInputs); i++)
    {
      dev = digInputs[i];
      devIoInit(dev, NULL);
    }
    
    // NTC COMPONENTS ADC
    for(uint8_t i = 0; i < ARRAY_SIZE(adcs); i++)
    {
      dev = adcs[i];
      devIoInit(dev, NULL);
    }
    
    // BUZZER
    dev = &buzzer;
    devIoInit(dev, NULL);

    // VCC COMPONENTS PWM
    for(uint8_t i = 0; i < ARRAY_SIZE(softPwmFreqOutputs); i++)
    {
      dev = softPwmFreqOutputs[i];
      devIoInit(dev, NULL);
    }
        // VSDCMotor COMPONENTS PWM
    for(uint8_t i = 0; i < ARRAY_SIZE(fans); i++)
    {
      dev = fans[i];
      devIoInit(dev, NULL);
    }
       // BDCMotor COMPONENTS PWM
    for(uint8_t i = 0; i < ARRAY_SIZE(bdcMotors); i++)
    {
      dev = bdcMotors[i];
      devIoInit(dev, NULL);
    }
    // PWM LED COMPONENTS PWM
    for(uint8_t i = 0; i < ARRAY_SIZE(fadingLights); i++)
    {
      dev = fadingLights[i];
      devIoInit(dev, NULL);
    }
    devIoInit(&rled_freq, NULL);
    devIoInit(&fled_freq, NULL);
    devCpxInit(&fledPwmCore, NULL);
    devCpxInit(&rledPwmCore, NULL);
    
    devCpxInit(&rfanPwmCore, NULL);
    devCpxInit(&ffanPwmCore, NULL);
    devCpxInit(&display, NULL);
    devMemInit(&eepromDev);

}

void boardFTC20MsTask(void)
{
  if( FTC.Tests.ICT != 0xAA ) FTC.Tests.ICT = 0;
  if( FTC.Tests.FTC != 0xAA ) FTC.Tests.FTC = 0;
  
  boardAcOutFTC();
  boardDcOutFTC();
  boardDcInFTC();
  boardAdcFTC();
  boardBuzzerFTC();
  boardVccFTC();
  boardVSDCMotorFTC();
  boardStepMotorFTC();
  boardBDCMotorFTC();
  boardLedDutyFTC();
  boardEepromFTC();
  // displat entegrasyonu bitince ui_ftc_state dosyasi kullanilarak display kontrol?de eklenmelidir.
  
}

/*** APW : Add Dev control ***/
stDevForce DevForce;

void boardDevCompOut(void)
{
    const tsDevIo *dev = NULL;

    if (DevForce.CompOut.index < ARRAY_SIZE(softPwmFreqOutputs))
        dev = softPwmFreqOutputs[DevForce.CompOut.index];

    DevForce.CompOut.value = refrigerator_R.Cooler.components.compressor.value;
    
    if(DevForce.CompOut.value)
    {
      if(refrigerator_R.Cooler.components.compressor.Readvalue)
        devIoPut(dev, DevForce.CompOut.value);
      else
        devIoPut(dev, 0);
    }
    else
      devIoPut(dev, DevForce.CompOut.value);
}

void boardDevForce20MsTask(void)
{
    boardDevCompOut();
}
/*** APW : Add Dev control ***/

#include "board.h"
//cdis #include "fsl_port.h"

/*************************************    AC Outputs    *****************************************/
//cdis #include "dev/ke14/gpio.h"
#include "rcos/dev/sh32f9/gpio.h"       //cadd
#include "rcos/dev/sh32f9/emu_eeprom.h"


DEV_IO_GPIO_CREATE(gpioAcComp, GPIOB, 11, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(gpioAcFrzDef, GPIOB, 14, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(gpioAcFFHeater, GPIOB, 15, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(gpioAcFrzFan, GPIOC, 0, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
/*
DEV_IO_GPIO_CREATE(gpioAcIonizer, GPIOE, 11, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(gpioAcDidHeater, GPIOE, 10, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(gpioAcWaterHeater, GPIOE, 13, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(gpioAcJkrDef, GPIOE, 5, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(gpioAcCondFan, GPIOE, 4, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(gpioAcHabHeater, GPIOE, 14, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
*/
/*****/

#include "dev/ac_output.h"
DEV_IO_AC_OUTPUT_CREATE(acComp, gpioAcComp)
DEV_IO_AC_OUTPUT_CREATE(acFrzDef, gpioAcFrzDef)
DEV_IO_AC_OUTPUT_CREATE(acFFHeater, gpioAcFFHeater)
DEV_IO_AC_OUTPUT_CREATE(acFrzFan, gpioAcFrzFan)
/*
DEV_IO_AC_OUTPUT_CREATE(acIonizer, gpioAcIonizer)
DEV_IO_AC_OUTPUT_CREATE(acDidHeater, gpioAcDidHeater)
DEV_IO_AC_OUTPUT_CREATE(acWaterHeater, gpioAcWaterHeater)
DEV_IO_AC_OUTPUT_CREATE(acJkrDef, gpioAcJkrDef)
DEV_IO_AC_OUTPUT_CREATE(acCondFan, gpioAcCondFan)
DEV_IO_AC_OUTPUT_CREATE(acHabHeater, gpioAcHabHeater)
*/

DEV_IO_AC_OUTPUT_CONTROLLER_CREATE(acOutputsController, 20 /*Screening time*/, &acComp, &acFrzDef, &acFFHeater, &acFrzFan/*,
								   &acIonizer, &acDidHeater, &acWaterHeater, &acJkrDef, &acCondFan, &acHabHeater*/)

const tsDevIo *acOutputs[] = {
	&acComp,
	&acFrzDef,
	&acFFHeater,
	&acFrzFan,/*
	&acIonizer,
	&acDidHeater,
	&acWaterHeater,
	&acJkrDef,
	&acCondFan,
	&acHabHeater,*/
};

static tsAcOutputConfig acOutputCompConfigs = {.participateInBlocking = false, .blockingTimeMs = 0};
static tsAcOutputConfig acOutputDelayableConfigs = {.participateInBlocking = false, .blockingTimeMs = 0};
static tsAcOutputConfig acOutputNotDelayableConfigs = {.participateInBlocking = false, .blockingTimeMs = 0};

/**
 *
 *
 *
 *
 *
 *
 */

/*************************************    Digital Inputs    *****************************************/

DEV_IO_GPIO_CREATE(digInDCFFDoor, GPIOC, 2, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInDCFrzDoor, GPIOC, 1, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInFFFanFg, GPIOE, 5, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)

/*
DEV_IO_GPIO_CREATE(digInDCJkrDoor, GPIOC, 12, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInDCDoor1, GPIOE, 7, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInDCDoor2, GPIOA, 2, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInDCBuzKabi, GPIOE, 3, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInDCBuzSW, GPIOE, 12, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInFrzFanFg, GPIOD, 2, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInCondFanFg, GPIOA, 3, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInJkrFanFg, GPIOB, 11, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digInButton, GPIOB, 10, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
*/
/******/

DEV_IO_GPIO_CREATE(revDigInDCFFDoor, GPIOC, 2, GPIO_Mode_IN, 0, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInDCFrzDoor, GPIOC, 1, GPIO_Mode_IN, 0, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInFFFanFg, GPIOE, 5, GPIO_Mode_IN, 0, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)

/*
DEV_IO_GPIO_CREATE(revDigInDCJkrDoor, GPIOC, 12, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInDCDoor2, GPIOE, 7, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInDCDoor2, GPIOA, 2, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInDCBuzKabi, GPIOE, 3, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInDCBuzSW, GPIOE, 12, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInFrzFanFg, GPIOD, 2, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInCondFanFg, GPIOA, 3, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInJkrFanFg, GPIOB, 11, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(revDigInButton, GPIOB, 10, GPIO_Mode_IN, 1, GPIO_ODrv_NORMAL, GPIO_OType_OD, GPIO_PuPd_NOPULL)
*/
/*****/



const tsDevIo *digInputs[] = {
	&digInDCFFDoor,
	&digInDCFrzDoor,
	&digInFFFanFg,        
        /*
	&digInDCJkrDoor,
	&digInDCDoor1,
	&digInDCDoor2,
	&digInDCBuzKabi,
	&digInDCBuzSW,
	&digInFrzFanFg,
	&digInCondFanFg,
	&digInJkrFanFg,
	&digInButton,
*/
};

const tsDevIo *revDigInputs[] = {
	&revDigInDCFFDoor,
	&revDigInDCFrzDoor,
    &revDigInFFFanFg,
        /*
	&revDigInDCJkrDoor,
	&revDigInDCDoor1,
	&revDigInDCDoor2,
	&revDigInDCBuzKabi,
	&revDigInDCBuzSW,
	&revDigInFrzFanFg,
	&revDigInCondFanFg,
	&revDigInJkrFanFg,
	&revDigInButton,
*/
};

#include "dev/debounce_patch.h"
DEV_CPX_DEBOUNCE_CREATE(debounceCore, 20,
						DEBOUNCE_ITEM(digInDCFFDoor, 5),
						DEBOUNCE_ITEM(digInDCFrzDoor, 5),
                        DEBOUNCE_ITEM(digInFFFanFg, 5),
                                                /*
						DEBOUNCE_ITEM(digInDCJkrDoor, 5),
						DEBOUNCE_ITEM(digInDCDoor1, 5),
						DEBOUNCE_ITEM(digInDCDoor2, 5),
						DEBOUNCE_ITEM(digInDCBuzKabi, 5),
						DEBOUNCE_ITEM(digInDCBuzSW, 5),
						DEBOUNCE_ITEM(digInFrzFanFg, 5),
						DEBOUNCE_ITEM(digInCondFanFg, 5),
						DEBOUNCE_ITEM(digInJkrFanFg, 5),
						DEBOUNCE_ITEM(digInButton, 5),
*/
						// Reversed inputs
						DEBOUNCE_ITEM(revDigInDCFFDoor, 5),
						DEBOUNCE_ITEM(revDigInDCFrzDoor, 5),
                        DEBOUNCE_ITEM(revDigInFFFanFg, 5)
                                                /*
						DEBOUNCE_ITEM(revDigInDCJkrDoor, 5),
						DEBOUNCE_ITEM(revDigInDCDoor1, 5),
						DEBOUNCE_ITEM(revDigInDCDoor2, 5),
						DEBOUNCE_ITEM(revDigInDCBuzKabi, 5),
						DEBOUNCE_ITEM(revDigInDCBuzSW, 5),
						DEBOUNCE_ITEM(revDigInFrzFanFg, 5),
						DEBOUNCE_ITEM(revDigInCondFanFg, 5),
						DEBOUNCE_ITEM(revDigInJkrFanFg, 5),
						DEBOUNCE_ITEM(revDigInButton, 5)*/)

DEV_IO_DEBOUNCE_CREATE(debDCFFDoor, debounceCore, 0)
DEV_IO_DEBOUNCE_CREATE(debDCFrzDoor, debounceCore, 1)
DEV_IO_DEBOUNCE_CREATE(debFFFanFg, debounceCore, 2)
/*
DEV_IO_DEBOUNCE_CREATE(debDCJkrDoor, debounceCore, 3)
DEV_IO_DEBOUNCE_CREATE(debDCDoor1, debounceCore, 4)
DEV_IO_DEBOUNCE_CREATE(debDCDoor2, debounceCore, 5)
DEV_IO_DEBOUNCE_CREATE(debDCBuzKabi, debounceCore, 6)
DEV_IO_DEBOUNCE_CREATE(debDCBuzSW, debounceCore, 7)
DEV_IO_DEBOUNCE_CREATE(debFrzFanFg, debounceCore, 8)
DEV_IO_DEBOUNCE_CREATE(debCondFanFg, debounceCore, 9)
DEV_IO_DEBOUNCE_CREATE(debJkrFanFg, debounceCore, 10)
DEV_IO_DEBOUNCE_CREATE(debButton, debounceCore, 11)
*/

const tsDevIo *debDigInputs[] = {
	&debDCFFDoor,
	&debDCFrzDoor,
    &debFFFanFg,
        /*
	&debDCJkrDoor,
	&debDCDoor1,
	&debDCDoor2,
	&debDCBuzKabi,
	&debDCBuzSW,
	&debFrzFanFg,
	&debCondFanFg,
	&debJkrFanFg,
	&debButton,
*/
};

// Reversed debounced digital inputs
DEV_IO_DEBOUNCE_CREATE(debRevDCFFDoor, debounceCore, 3)
DEV_IO_DEBOUNCE_CREATE(debRevDCFrzDoor, debounceCore, 4)
DEV_IO_DEBOUNCE_CREATE(debRevFFFanFg, debounceCore, 5)
/*
DEV_IO_DEBOUNCE_CREATE(debRevDCJkrDoor, debounceCore, 14)
DEV_IO_DEBOUNCE_CREATE(debRevDCDoor1, debounceCore, 15)
DEV_IO_DEBOUNCE_CREATE(debRevDCDoor2, debounceCore, 16)

DEV_IO_DEBOUNCE_CREATE(debRevDCBuzKabi, debounceCore, 17)
DEV_IO_DEBOUNCE_CREATE(debRevDCBuzSW, debounceCore, 18)


DEV_IO_DEBOUNCE_CREATE(debRevFrzFanFg, debounceCore, 20)
DEV_IO_DEBOUNCE_CREATE(debRevCondFanFg, debounceCore, 21)
DEV_IO_DEBOUNCE_CREATE(debRevJkrFanFg, debounceCore, 22)

DEV_IO_DEBOUNCE_CREATE(debRevButton, debounceCore, 23)
*/

const tsDevIo *debRevDigInputs[] = {
	&debRevDCFFDoor,
	&debRevDCFrzDoor,
    &debRevFFFanFg,
        /*
	&debRevDCJkrDoor,
	&debRevDCDoor1,
	&debRevDCDoor2,
	&debRevDCBuzKabi,
	&debRevDCBuzSW,
	&debRevFrzFanFg,
	&debRevCondFanFg,
	&debRevJkrFanFg,
	&debRevButton,
*/
};

/**
 *
 *
 *
 *
 *
 *
 */

/*************************************    Digital Outputs    *****************************************/

DEV_IO_GPIO_CREATE(digOutFFLight, GPIOC, 6, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digOutFrzLight, GPIOD, 10, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digOutFFFanDC, GPIOA, 3, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digOutFFFanPower, GPIOA, 0, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
/*
DEV_IO_GPIO_CREATE(digOutMaviLight, GPIOB, 2, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digOutFrzFanDC, GPIOA, 3, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digOutFrzFanPower, GPIOA, 0, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)

DEV_IO_GPIO_CREATE(digOutCondFanDC, GPIOB, 5, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digOutCondFanPower, GPIOD, 15, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)

DEV_IO_GPIO_CREATE(digOutJkrFanDC, GPIOC, 3, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digOutJkrFanPower, GPIOC, 2, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)

DEV_IO_GPIO_CREATE(digOutDCPwm1, GPIOD, 14, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digOutDCPwm2, GPIOD, 13, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
DEV_IO_GPIO_CREATE(digOutDCDispenserLed, GPIOC, 10, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
*/
/******/


const tsDevIo *digOutputs[] = {
	&digOutFFLight,
	&digOutFrzLight,
	&digOutFFFanDC,
	&digOutFFFanPower,
        /*
        &digOutMaviLight,

	&digOutFrzFanDC,
	&digOutFrzFanPower,

	&digOutCondFanDC,
	&digOutCondFanPower,

	&digOutJkrFanDC,
	&digOutJkrFanPower,

	&digOutDCPwm1,
	&digOutDCPwm2,

	&digOutDCDispenserLed,
*/
};





/*************************************    ADC Inputs    *****************************************/
#include "rcos/dev/sh32f9/adc.h"       //cadd

DEV_CPX_ADC_CORE_CREATE(adcCore, ADC, 20 /*refresh time*/,
						/* FF Hava */ ADC_Channel_14,
						/* FF Eva */  ADC_Channel_13,
						/* FRZ Hava */ADC_Channel_12,
						/* FRZ Eva */ ADC_Channel_4,
//						/* JKR HAVA */ADC_Channel_4,
//						/* JKR Eva */ ADC_Channel_5,
//						/* ICE TEMP */ADC_Channel_6,
//						/*ORTAM TEMP*/ADC_Channel_7,
						/* POT */     ADC_Channel_5)
//						/* BACKUP 2 */ADC_Channel_9)

DEV_IO_ADC_CREATE(adcFFAir, adcCore, 0)
DEV_IO_ADC_CREATE(adcFFEva, adcCore, 1)
DEV_IO_ADC_CREATE(adcFrzAir, adcCore, 2)
DEV_IO_ADC_CREATE(adcFrzEva, adcCore, 3)
//DEV_IO_ADC_CREATE(adcJkrAir, adcCore, 4)
//DEV_IO_ADC_CREATE(adcJkrEva, adcCore, 5)
//DEV_IO_ADC_CREATE(adcIceTemp, adcCore, 6)
//DEV_IO_ADC_CREATE(adcAmbTemp, adcCore, 7)
 DEV_IO_ADC_CREATE(adcPOT, adcCore, 4)
// DEV_IO_ADC_CREATE(adcBackup2, adcCore, 9)

const tsDevIo *adcs[] = {
	&adcFFAir,
	&adcFFEva,
	&adcFrzAir,
	&adcFrzEva,
        /*
	&adcJkrAir,
	&adcJkrEva,
	&adcIceTemp,
	&adcAmbTemp,
*/
        &adcPOT
};

/**
 *
 *
 *
 *
 *
 *
 */

/*************************************    NTC Inputs    *****************************************/

#include "dev/filter.h"
DEV_IO_FILTER_CREATE(filteredAdcFFAir, adcFFAir, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcFFEva, adcFFEva, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcFrzAir, adcFrzAir, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcFrzEva, adcFrzEva, 20, 4)
/*
DEV_IO_FILTER_CREATE(filteredAdcJkrAir, adcJkrAir, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcJkrEva, adcJkrEva, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcIceTemp, adcIceTemp, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcAmbTemp, adcAmbTemp, 20, 4)
*/
DEV_IO_FILTER_CREATE(filteredAdcPOT, adcPOT, 20, 4)
// DEV_IO_FILTER_CREATE(filteredAdcBackup2, adcBackup2, 20, 4)

#include "dev/ntc.h"
DEV_IO_NTC_CREATE(ntcFFAir, filteredAdcFFAir)
DEV_IO_NTC_CREATE(ntcFFEva, filteredAdcFFEva)
DEV_IO_NTC_CREATE(ntcFrzAir, filteredAdcFrzAir)
DEV_IO_NTC_CREATE(ntcFrzEva, filteredAdcFrzEva)
/*
DEV_IO_NTC_CREATE(ntcJkrAir, filteredAdcJkrAir)
DEV_IO_NTC_CREATE(ntcJkrEva, filteredAdcJkrEva)
DEV_IO_NTC_CREATE(ntcIceTemp, filteredAdcIceTemp)
DEV_IO_NTC_CREATE(ntcAmbTemp, filteredAdcAmbTemp)
*/
DEV_IO_NTC_CREATE(ntcPOT, filteredAdcPOT)

DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFFAir, ntcFFAir)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFFEva, ntcFFEva)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFrzAir, ntcFrzAir)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFrzEva, ntcFrzEva)
/*
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorJkrAir, ntcJkrAir)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorJkrEva, ntcJkrEva)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorIceTemp, ntcIceTemp)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorAmbTemp, ntcAmbTemp)
*/
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorPOT, ntcPOT)
// DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorBackup2, ntcBackup2)

const tsDevIo *ntcs[] = {
	&ntcFFAir,
	&ntcFFEva,
	&ntcFrzAir,
	&ntcFrzEva,
        /*
	&ntcJkrAir,
	&ntcJkrEva,
	&ntcIceTemp,
	&ntcAmbTemp,
*/
        &ntcPOT
};

const tsDevIo *ntcErrorDetectors[] = {
	&ntcErrorFFAir,
	&ntcErrorFFEva,
	&ntcErrorFrzAir,
	&ntcErrorFrzEva,
        /*
	&ntcErrorJkrAir,
	&ntcErrorJkrEva,
	&ntcErrorIceTemp,
	&ntcErrorAmbTemp,
*/
        &ntcErrorPOT
};

#include "dev/knob.h"
DEV_IO_KNOB_CREATE(knob, filteredAdcPOT)
DEV_IO_KNOB_ERROR_DETECTOR_CREATE(knobError, knob)
const tsDevIo *knobs[] = {
        &knob
};
const tsDevIo *knobErrorDetectors[] = {
        &knobError
};

/**
 *
 *
 *
 *
 *
 *
 */



/*************************************    HW PWM Outputs    *****************************************/

#include "dev/sh32f9/pwm.h"

//FF Fan
DEV_CPX_PWM_CORE_CREATE(fanPwmCore, 0, A, A, 3, 4, 600, 0) // For Buzzer ------ PA3 PWM0 Channel-A  600hz %50duty (divide 4)
DEV_IO_PWM_DUTY_CREATE(dutyFFFan, fanPwmCore)

//FF Light
DEV_CPX_PWM_CORE_CREATE(ffLightPwmCore, 3, A, C, 6, 4, 600, 0) // For FF Light ------ PC6 PWM3 Channel-A  600hz %50duty (divide 4)
DEV_IO_PWM_DUTY_CREATE(dutyFFLight, ffLightPwmCore)
DEV_IO_PWM_FREQ_CREATE(freqFFLight, ffLightPwmCore)

//VCC Drive
DEV_CPX_PWM_CORE_CREATE(vccDrivePwmCore, 2, A, C, 3, 4, 0, 50) // For VCC Drive ------ PC3 PWM2 Channel-A  600hz %50duty (divide 4)
DEV_IO_PWM_DUTY_CREATE(dutyVccDrive, vccDrivePwmCore)
DEV_IO_PWM_FREQ_CREATE(freqVccDrive, vccDrivePwmCore)

const tsDevIo *softPwmFreqOutputs[] = {
	&freqVccDrive,
};

const tsDevIo *softPwmDutyOutputs[] = {
	&dutyVccDrive,
};


/*************************************    Lights    *****************************************/

#include "dev/fade.h"
DEV_IO_FADE_CREATE(fadingFFLight, dutyFFLight, 1, 10)

const tsDevIo *fadingLights[] = {
	&fadingFFLight,
};



/*************************************    Fans    *****************************************/
#include "dev/fan.h"
DEV_IO_FAN_CREATE(fanFF, dutyFFFan, digOutFFFanDC, digOutFFFanPower, digInFFFanFg)

/*
DEV_IO_FAN_CREATE(fanFrz, dutyFrzFan, digOutFrzFanDC, digOutFrzFanPower, digInFrzFanFg)
DEV_IO_FAN_CREATE(fanCond, dutyCondFan, digOutCondFanDC, digOutCondFanPower, digInCondFanFg)
DEV_IO_FAN_CREATE(fanJkr, dutyJkrFan, digOutJkrFanDC, digOutJkrFanPower, digInJkrFanFg)
*/

DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorFF, fanFF)
/*
DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorFrz, fanFrz)
DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorCond, fanCond)
DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorJkr, fanJkr)
*/

const tsDevIo *fans[] = {
	&fanFF,
        /*
	&fanFrz,
	&fanCond,
	&fanJkr,
*/
};

const tsDevIo *fanErrorDetectors[] = {
	&fanErrorFF,
        /*
	&fanErrorFrz,
	&fanErrorCond,
	&fanErrorJkr,
*/
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
DEV_IO_GPIO_CREATE(buzzerHigh, GPIOA, 13, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
/****/

#include "dev/sh32f9/pwm.h"
DEV_CPX_PWM_CORE_CREATE(buzzerPwmCore, 1, A, A, 14, 4, 1000, 50) // For Buzzer ------ PA14 PWM1 Channel-A  1000hz %50duty (divide 4)
DEV_IO_PWM_DUTY_CREATE(buzzerDuty, buzzerPwmCore)
DEV_IO_PWM_FREQ_CREATE(buzzerFreq, buzzerPwmCore)
/*****/

#include "dev/buzzer.h"
DEV_IO_BUZZER_CREATE(buzzer, buzzerHigh, buzzerFreq)


/*************************************    I2C Devices    *****************************************/
/*cdis todo: bu kisim daha sonra aktif edilecek
#include "dev/ke14/gpio.h"
DEV_IO_GPIO_CREATE(i2cEn, I2C_ENABLE, 0, 0, 1)

#include "dev/i2c.h"
DEV_COM_I2C_CREATE(i2cCore, 1, 75000)

#include "dev/sht3x.h"
DEV_IO_SHT3X_CORE_CREATE(sht3xCore, i2cCore, 0x44, 50)
DEV_IO_SHT3X_TEMP_CREATE(sht3xTemp, sht3xCore)
DEV_IO_SHT3X_HUMIDITY_CREATE(sht3xHum, sht3xCore)
#include "dev/filter.h"
DEV_IO_FILTER_CREATE(filteredSht3xTemp, sht3xTemp, 8, 2)
DEV_IO_FILTER_CREATE(filteredSht3xHum, sht3xHum, 8, 2)

#include "dev/display_uart.h"
DEV_CPX_DISPLAY_CREATE(display, i2cCore, 20)

#include "dev/24cxx.h"
static tsTarget eepromTarget;
DEV_MEM_24CXX_CREATE(eeprom, i2cCore, eepromTarget, 0, 8)
DEV_IO_GPIO_CREATE(eepromEn, UC_EEP_5V, 0, 0, 1)
*/
/**
 *
 *
 *
 *
 *
 *
 */

/*************************************    UART Devices    *****************************************/
#include "dev/display_uart.h"
#include "dev/sh32f9/uart.h"
DEV_COM_UART_CREATE(udaqUart, UART0, /*Uart0_Remap*/ 1, 9600, 1, None, 256, 256)  // for udaq
DEV_COM_UART_CREATE(displayUart, UART1, /*Uart1_Remap*/ 1, 115200, 1, None, 8, 16)  // for display
DEV_CPX_DISPLAY_UART_CREATE(display, displayUart, 20)
/*******/

/**
 *
 *
 *
 *
 *
 *
 */

/*************************************    WIFI Devices    *****************************************/
// TODO : Fill wifi devices
/*cdis todo: bunlari sonra aktif edersin
DEV_IO_GPIO_CREATE(wifiCS, UC_SLAVE_SS, 0, 0, 0) // Attention: that the active level of a CS device is 0 !!!
DEV_IO_GPIO_CREATE(wifiEn, UC_WIFI_POWER, 0, 0, 1)
*/

/**
 *
 *
 *
 *
 *
 *
 */


void boardInitBoard(void)
{
	devCpxInit(&adcCore, NULL);
        
	devCpxInit(&buzzerPwmCore, NULL);
	devIoInit(&buzzer, NULL);
	devIoInit(&acOutputsController, NULL);
	devCpxInit(&debounceCore, NULL);

	devCpxInit(&fanPwmCore, NULL);
	devCpxInit(&ffLightPwmCore, NULL);
	devCpxInit(&vccDrivePwmCore, NULL);
        
        

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

	case eDeviceTypeReversedDigInput:
		if (deviceIdx < ARRAY_SIZE(revDigInputs))
			dev = revDigInputs[deviceIdx];
		break;

	case eDeviceTypeFilteredDigInput:
		if (deviceIdx < ARRAY_SIZE(debDigInputs))
			dev = debDigInputs[deviceIdx];
		break;

	case eDeviceTypeFilteredReversedDigInput:
		if (deviceIdx < ARRAY_SIZE(debRevDigInputs))
			dev = debRevDigInputs[deviceIdx];
		break;

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

		if (deviceIdx < ARRAY_SIZE(softPwmFreqOutputs))
		{
			dev = softPwmFreqOutputs[deviceIdx];

//			devIoInit(softPwmDutyOutputs[deviceIdx], NULL);
		}

		break;

	case eDeviceTypeFadingLight:

		if (deviceIdx < ARRAY_SIZE(fadingLights))
		{
			dev = fadingLights[deviceIdx];
                        devIoInit(&freqFFLight, NULL);
		}

		break;

	case eDeviceTypeNTC:
		if (deviceIdx < ARRAY_SIZE(ntcs))
			dev = ntcs[deviceIdx];
		break;

	case eDeviceTypeNTCErrorDetector:
		if (deviceIdx < ARRAY_SIZE(ntcErrorDetectors))
			dev = ntcErrorDetectors[deviceIdx];
		break;
                
        case eDeviceTypeKnob:
		if (deviceIdx < ARRAY_SIZE(ntcs))
			dev = knobs[deviceIdx];
		break;

	case eDeviceTypeKnobError:
		if (deviceIdx < ARRAY_SIZE(ntcErrorDetectors))
			dev = knobErrorDetectors[deviceIdx];
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
          /*cdis todo: sonra aktif et
		if (deviceIdx < ARRAY_SIZE(stepperMotors))
			dev = stepperMotors[deviceIdx];
*/
		break;

	case eDeviceTypeDCMotor:
          /*cdis todo: sonra aktif et
		if (deviceIdx < ARRAY_SIZE(dcMotors))
			dev = dcMotors[deviceIdx];
*/
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


/* U-1
	&acComp,        KN5/6
	&acFrzDef,      KN5/4
	&acFFHeater,    KN5/2
	&acFrzFan,      KN5/1
*/
void boardAcOutFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.ACOut.index < ARRAY_SIZE(acOutputs))
      dev = acOutputs[FTC.ACOut.index];

    devIoPut(dev, FTC.ACOut.value);
}

/*U-1
	&digOutFFLight,         KN6/8
	&digOutFrzLight,        KN6/9
	&digOutFFFanDC,         KN6/12
	&digOutFFFanPower,      KN6/13
*/
void boardDcOutFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.DCOut.index < ARRAY_SIZE(digOutputs))
      dev = digOutputs[FTC.DCOut.index];

    devIoPut(dev, FTC.DCOut.value);
}

/*U-1
	&digInDCFFDoor,         KN4/3
	&digInDCFrzDoor,        KN4/2
	&digInFFFanFg,          KN6/14
*/
void boardDcInFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.DCSwitch.index < ARRAY_SIZE(digInputs))
      dev = digInputs[FTC.DCSwitch.index];

    FTC.DCSwitch.value = devIoGet(dev);
}

/*U-1
	&adcFFAir,      KN4/13
	&adcFFEva,      KN4/11
	&adcFrzAir,     KN4/9
	&adcFrzEva,     KN4/7
        &adcPOT,        KN4/5
*/
void boardAdcFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.ADC_.index < ARRAY_SIZE(adcs))
      dev = adcs[FTC.ADC_.index];

    FTC.ADC_.value = devIoGet(dev);
}

/*U-1
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

/*U-1
	&softPwmFreqOutputs;  KN6/9      
*/
void boardVccFTC(void)
{
    const tsDevIo *dev = NULL;
    
    if (FTC.VCC.index < ARRAY_SIZE(softPwmFreqOutputs))
      dev = softPwmFreqOutputs[FTC.VCC.index];

    devIoPut(dev, FTC.VCC.value);
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
  
  // displat entegrasyonu bitince ui_ftc_state dosyasi kullanilarak display kontrol�de eklenmelidir.
  
}
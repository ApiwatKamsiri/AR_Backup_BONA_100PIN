
#include "board.h"
//cdis #include "fsl_port.h"
#include "../app/refrigerator/rf_ram.h"

/*************************************    AC Outputs    *****************************************/
//cdis #include "dev/ke14/gpio.h"
#include "rcos/dev/ra2/gpio.h"       //cadd
#include "rcos/dev/ra2/data_flash.h"
#define SOLVER          0x0200       // port 200 is unused pins that's why i can use for solver

DEV_IO_GPIO_STACK(g_ioport)


DEV_IO_GPIO_CREATE(gpioAcRdef, RDEF, 1)
DEV_IO_GPIO_CREATE(gpioAcFdef, FDEF, 1)
DEV_IO_GPIO_CREATE(gpioAcSpheat, SPHEAT, 1)
DEV_IO_GPIO_CREATE(gpioAcRpheat, RPHEAT, 1)
DEV_IO_GPIO_CREATE(gpioAcImheat, IMHEAT, 1)
DEV_IO_GPIO_CREATE(gpioAcXheat, XHEAT, 1)
DEV_IO_GPIO_CREATE(gpioAcSpareHeat, Spare_HEAT, 1)
DEV_IO_GPIO_CREATE(gpioAcIcSol, IC_SOL, 1)
DEV_IO_GPIO_CREATE(gpioAcIdSol, ID_SOL, 1)
DEV_IO_GPIO_CREATE(gpioAcIdGmotor, GMOTOR, 1)


#include "dev/ac_output.h"
DEV_IO_AC_OUTPUT_CREATE(acRdef, gpioAcRdef)
DEV_IO_AC_OUTPUT_CREATE(acFdef, gpioAcFdef)
DEV_IO_AC_OUTPUT_CREATE(acSpheat, gpioAcSpheat)
DEV_IO_AC_OUTPUT_CREATE(acRpheat, gpioAcRpheat)
DEV_IO_AC_OUTPUT_CREATE(acImheat, gpioAcImheat)
DEV_IO_AC_OUTPUT_CREATE(acXheat, gpioAcXheat)
DEV_IO_AC_OUTPUT_CREATE(acSpareHeat, gpioAcSpareHeat)
DEV_IO_AC_OUTPUT_CREATE(acIcSol, gpioAcIcSol)
DEV_IO_AC_OUTPUT_CREATE(acIdSol, gpioAcIdSol)
DEV_IO_AC_OUTPUT_CREATE(acIdGmotor, gpioAcIdGmotor)


DEV_IO_AC_OUTPUT_CONTROLLER_CREATE(acOutputsController, 20 /*Screening time*/,&acRdef, &acFdef, &acSpheat, &acRpheat, &acImheat, &acXheat, &acSpareHeat, &acIcSol, &acIdSol, &acIdGmotor)

const tsDevIo *acOutputs[] = {
		&acRdef,        //Real Heater : Gutter Heater
		&acFdef,        //Real Heater : FRZ DEF Heater
		&acSpheat,      //Real Heater : IM Pipe Heater
		&acRpheat,      //Real Heater : BOOT Heater (WD+ICD Heater)
		&acImheat,      //Real Heater : Heat Up Heater
		&acXheat,       //Real Heater : Vacuum Heater
		&acSpareHeat,   //Real Heater : Handle Heater
		&acIcSol,
		&acIdSol,
		&acIdGmotor,
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
//DEV_IO_GPIO_CREATE(digInKFG, KFG, 0)  
DEV_IO_GPIO_CREATE(digInTEST1, TEST1, 0)
DEV_IO_GPIO_CREATE(digInTEST2, TEST2, 0)
DEV_IO_GPIO_CREATE(digInNFaultGasValve, n_Fault_GasValve, 0)
DEV_IO_GPIO_CREATE(digInNFaultIce, n_Fault_Ice, 0)
DEV_IO_GPIO_CREATE(digInNFaultDispenser, n_Fault_Dispenser, 0)
DEV_IO_GPIO_CREATE(digInHUMANSEN, HUMANSEN, 1)
DEV_IO_GPIO_CREATE(digInBSW, BSW, 0)
DEV_IO_GPIO_CREATE(digInSOLVER, SOLVER, 0)



/******/


DEV_IO_GPIO_CREATE(revDigInDOORR, DOORR, 0)
DEV_IO_GPIO_CREATE(revDigInDOORF, DOORF, 0)
DEV_IO_GPIO_CREATE(revDigInIMLSW, IMLSW, 0)
DEV_IO_GPIO_CREATE(revDigInFFG, FFG, 0)
DEV_IO_GPIO_CREATE(revDigInRFG, RFG, 0)  
//DEV_IO_GPIO_CREATE(revDigInKFG, KFG, 0)  
DEV_IO_GPIO_CREATE(revDigInTEST1, TEST1, 0)
DEV_IO_GPIO_CREATE(revDigInTEST2, TEST2, 0)
DEV_IO_GPIO_CREATE(revDigInNFaultGasValve, n_Fault_GasValve, 0)
DEV_IO_GPIO_CREATE(revDigInNFaultIce, n_Fault_Ice, 0)
DEV_IO_GPIO_CREATE(revDigInNFaultDispenser, n_Fault_Dispenser, 0)
DEV_IO_GPIO_CREATE(revDigInHUMANSEN, HUMANSEN, 1)
DEV_IO_GPIO_CREATE(revDigInBSW, BSW, 0)
DEV_IO_GPIO_CREATE(revDigInSOLVER, SOLVER, 0)

/*****/


const tsDevIo *digInputs[] = {
        &digInDOORR,
        &digInDOORF,
        &digInIMLSW,     
        &digInFFG,
        &digInRFG,
        &digInTEST1,  
        &digInTEST2,
        &digInNFaultGasValve,
        &digInNFaultIce,
        &digInNFaultDispenser,
        &digInHUMANSEN,
        &digInBSW,
        &digInSOLVER,
};

const tsDevIo *revDigInputs[] = {
        &revDigInDOORR,
        &revDigInDOORF,
        &revDigInIMLSW,     
        &revDigInFFG,
        &revDigInRFG,
        &revDigInTEST1,  
        &revDigInTEST2, 
        &revDigInNFaultGasValve,
        &revDigInNFaultIce,
        &revDigInNFaultDispenser,
        &revDigInHUMANSEN,
        &revDigInBSW,
        &revDigInSOLVER,
};




#include "dev/debounce_patch.h"
DEV_CPX_DEBOUNCE_CREATE(debounceCore, 20,
                                                DEBOUNCE_ITEM(digInDOORR, 5),
                                                DEBOUNCE_ITEM(digInDOORF, 5),
                                                DEBOUNCE_ITEM(digInIMLSW, 5),
                                                DEBOUNCE_ITEM(digInFFG, 5),
                                                DEBOUNCE_ITEM(digInRFG, 5),
                                                DEBOUNCE_ITEM(digInTEST1, 5),
                                                DEBOUNCE_ITEM(digInTEST2, 5),
                                                DEBOUNCE_ITEM(digInNFaultGasValve, 5),
                                                DEBOUNCE_ITEM(digInNFaultIce, 5),
                                                DEBOUNCE_ITEM(digInNFaultDispenser, 5),
                                                DEBOUNCE_ITEM(digInHUMANSEN, 5),
                                                DEBOUNCE_ITEM(digInBSW, 5),
                                                DEBOUNCE_ITEM(digInSOLVER, 5),
						
                                                // Reversed inputs
                                                DEBOUNCE_ITEM(revDigInDOORR, 5),
                                                DEBOUNCE_ITEM(revDigInDOORF, 5),
                                                DEBOUNCE_ITEM(revDigInIMLSW, 5),
                                                DEBOUNCE_ITEM(revDigInFFG, 5),
                                                DEBOUNCE_ITEM(revDigInRFG, 5),
                                                DEBOUNCE_ITEM(revDigInTEST1, 5),
                                                DEBOUNCE_ITEM(revDigInTEST2, 5),
                                                DEBOUNCE_ITEM(revDigInNFaultGasValve, 5),
                                                DEBOUNCE_ITEM(revDigInNFaultIce, 5),
                                                DEBOUNCE_ITEM(revDigInNFaultDispenser, 5),
                                                DEBOUNCE_ITEM(revDigInHUMANSEN, 5),
                                                DEBOUNCE_ITEM(revDigInBSW, 5),
                                                DEBOUNCE_ITEM(revDigInSOLVER, 5)) 

                                                

DEV_IO_DEBOUNCE_CREATE(debDOORR, debounceCore, 0)
DEV_IO_DEBOUNCE_CREATE(debDOORF, debounceCore, 1)
DEV_IO_DEBOUNCE_CREATE(debIMLSW, debounceCore, 2)
DEV_IO_DEBOUNCE_CREATE(debFFG, debounceCore, 3)
DEV_IO_DEBOUNCE_CREATE(debRFG, debounceCore, 4)
DEV_IO_DEBOUNCE_CREATE(debTEST1, debounceCore, 5)
DEV_IO_DEBOUNCE_CREATE(debTEST2, debounceCore, 6)
DEV_IO_DEBOUNCE_CREATE(debNFaultGasValve, debounceCore, 7)
DEV_IO_DEBOUNCE_CREATE(debNFaultIce, debounceCore, 8)
DEV_IO_DEBOUNCE_CREATE(debNFaultDispenser, debounceCore, 9)
DEV_IO_DEBOUNCE_CREATE(debHUMANSEN, debounceCore, 10)
DEV_IO_DEBOUNCE_CREATE(debBSW, debounceCore, 11)
DEV_IO_DEBOUNCE_CREATE(debSOLVER, debounceCore, 12)


const tsDevIo *debDigInputs[] = {
        &debDOORR,
        &debDOORF,
        &debIMLSW,
        &debFFG,
        &debRFG,
        &debTEST1,
        &debTEST2,
        &debNFaultGasValve,
        &debNFaultIce,
        &debNFaultDispenser,
        &debHUMANSEN,
        &debBSW,
        &debSOLVER,
};

// Reversed debounced digital inputs
DEV_IO_DEBOUNCE_CREATE(debRevDOORR, debounceCore, 13)
DEV_IO_DEBOUNCE_CREATE(debRevDOORF, debounceCore, 14)
DEV_IO_DEBOUNCE_CREATE(debRevIMLSW, debounceCore, 15)
DEV_IO_DEBOUNCE_CREATE(debRevFFG, debounceCore, 16)
DEV_IO_DEBOUNCE_CREATE(debRevRFG, debounceCore, 17)
DEV_IO_DEBOUNCE_CREATE(debRevTEST1, debounceCore, 18)
DEV_IO_DEBOUNCE_CREATE(debRevTEST2, debounceCore, 19)
DEV_IO_DEBOUNCE_CREATE(debRevNFaultGasValve, debounceCore, 20)
DEV_IO_DEBOUNCE_CREATE(debRevNFaultIce, debounceCore, 21)
DEV_IO_DEBOUNCE_CREATE(debRevNFaultDispenser, debounceCore, 22)
DEV_IO_DEBOUNCE_CREATE(debRevHUMANSEN, debounceCore, 23)
DEV_IO_DEBOUNCE_CREATE(debRevBSW, debounceCore, 24)
DEV_IO_DEBOUNCE_CREATE(debRevSOLVER, debounceCore, 25)


const tsDevIo *debRevDigInputs[] = {
        &debRevDOORR,
        &debRevDOORF,
        &debRevIMLSW,
        &debRevFFG,
        &debRevRFG,
        &debRevTEST1,
        &debRevTEST2,
        &debRevNFaultGasValve,
        &debRevNFaultIce,
        &debRevNFaultDispenser,
        &debRevHUMANSEN,
        &debRevBSW,
        &debRevSOLVER,
};


///*************************************    Digital Outputs    *****************************************/

//DEV_IO_GPIO_CREATE(digOutFFLight, GPIOC, 6, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
//DEV_IO_GPIO_CREATE(digOutFrzLight, GPIOD, 10, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
//DEV_IO_GPIO_CREATE(digOutFFFanDC, GPIOA, 3, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)
//DEV_IO_GPIO_CREATE(digOutFFFanPower, GPIOA, 0, GPIO_Mode_OUT, 1, GPIO_ODrv_NORMAL, GPIO_OType_PP, GPIO_PuPd_NOPULL)

DEV_IO_GPIO_CREATE(digOutRM_DC, RM, 1)
DEV_IO_GPIO_CREATE(digOutFM_DC, FM, 1)
DEV_IO_GPIO_CREATE(digOutKM_DC, KM, 1)
DEV_IO_GPIO_CREATE(digOutXM_DC, XM, 1)
DEV_IO_GPIO_CREATE(digOutIM_DC, IMF, 1)
DEV_IO_GPIO_CREATE(digOutRM_POWER, RM, 1)
DEV_IO_GPIO_CREATE(digOutFM_POWER, FM, 1)
DEV_IO_GPIO_CREATE(digOutKM_POWER, KM, 1)
DEV_IO_GPIO_CREATE(digOutXM_POWER, XM, 1)
DEV_IO_GPIO_CREATE(digOutIM_POWER, IMF, 1)
DEV_IO_GPIO_CREATE(digOutRLED, RLED, 1)
DEV_IO_GPIO_CREATE(digOutFLED, FLED, 1)
DEV_IO_GPIO_CREATE(digOutXLED, XLED, 1)
DEV_IO_GPIO_CREATE(digOutBT_LED, BT_LED, 1)
DEV_IO_GPIO_CREATE(digOutR_LED, R_LED, 1)
DEV_IO_GPIO_CREATE(digOutG_LED, G_LED, 1)
DEV_IO_GPIO_CREATE(digOutB_LED, B_LED, 1)
DEV_IO_GPIO_CREATE(digOutIM_PUMP_CW, IM_CW, 1)
DEV_IO_GPIO_CREATE(digOutIM_PUMP_CCW, IM_CCW, 1)
DEV_IO_GPIO_CREATE(digOutWD_PUMP_CW, WD1, 1)
DEV_IO_GPIO_CREATE(digOutWD_PUMP_CCW, WD2, 1)
DEV_IO_GPIO_CREATE(digOutIM_MOTOR_CW, IMM_CW, 1)
DEV_IO_GPIO_CREATE(digOutIM_MOTOR_CCW, IMM_CCW, 1)
/*DEV_IO_GPIO_CREATE(digOutIC_SOL, IC_SOL, 1)
DEV_IO_GPIO_CREATE(digOutID_SOL, ID_SOL, 1)
DEV_IO_GPIO_CREATE(digOutID_GMOTOR, GMOTOR, 1)*/
//DEV_IO_GPIO_CREATE(vccDriveEn, INV_SHUTDOWN, 1)         //APW : Disable for new MCU
DEV_IO_GPIO_CREATE(digOutARM, ARM, 1)   


#include "dev/bdc_motor.h"
DEV_IO_BDC_MOTOR_CREATE(bdcIceMotor, digOutIM_MOTOR_CW, digOutIM_MOTOR_CCW, 500)
DEV_IO_BDC_MOTOR_CREATE(bdcIcePumpMotor, digOutIM_PUMP_CW, digOutIM_PUMP_CCW, 500)
DEV_IO_BDC_MOTOR_CREATE(bdcDispPumpMotor, digOutWD_PUMP_CW, digOutWD_PUMP_CCW, 500)

const tsDevIo *bdcMotors[] = {
	&bdcIceMotor,
        &bdcIcePumpMotor,
        &bdcDispPumpMotor,
};

/******/


const tsDevIo *digOutputs[] = {
        &digOutRM_DC,
        &digOutFM_DC,
        &digOutKM_DC,
        &digOutXM_DC,
        &digOutIM_DC,
        &digOutRM_POWER,
        &digOutFM_POWER,
        &digOutKM_POWER,
        &digOutXM_POWER,
        &digOutIM_POWER,
        &digOutIM_PUMP_CW,
        &digOutIM_PUMP_CCW,
        &digOutWD_PUMP_CW,
        &digOutWD_PUMP_CCW,
        &digOutIM_MOTOR_CW,
        &digOutIM_MOTOR_CCW,
/*        &digOutIC_SOL,
        &digOutID_SOL,
        &digOutID_GMOTOR,*/
        //&vccDriveEn,
        &digOutARM, 
};


#include "dev/ra2/gpio.h"

DEV_IO_GPIO_CREATE(motorAp, GA, 1)
DEV_IO_GPIO_CREATE(motorAn, GXA, 1)
DEV_IO_GPIO_CREATE(motorBp, GB, 1)
DEV_IO_GPIO_CREATE(motorBn, GXB, 1)


#include "dev/stepper_motor.h"
#ifdef HOSHI_H30
DEV_IO_STEPPER_MOTOR_CREATE(stepperMotor, 4, motorAp, motorAn, motorBp, motorBn)        //Damper : 300pps -> 1/300pps = 3.3ms but set 4ms
#else                                                                                   //because if set 3ms when convert to pps is 333pps that over step.
DEV_IO_STEPPER_MOTOR_CREATE(stepperMotor, 33, motorAp, motorAn, motorBp, motorBn)       //Valve : 30pps -> 1/30pps = 33.3ms
#endif
const tsDevIo *stepperMotors[] = {
	&stepperMotor,
};


///*************************************    ADC Inputs    *****************************************/
#include "rcos/dev/ra2/adc.h"       //cadd
DEV_CPX_ADC_CORE_CREATE(adcCore, ADC_12_BUS, adcIsr  , 20 /*refresh time*/
						/* FF Hava RSEN */          ,ADC_CHANNEL_12
						/* FF Eva  RDSEN*/          ,ADC_CHANNEL_11
						/* FRZ Hava FSEN*/          ,ADC_CHANNEL_8
						/* FRZ Eva FDSEN*/          ,ADC_CHANNEL_4
						/* ICE temp IMSEN*/         ,ADC_CHANNEL_1
						/* Amb Temp TCSEN*/         ,ADC_CHANNEL_2
                                                /* Humidity HSEN*/          ,ADC_CHANNEL_0
                                                /* Vacuum Box XSEN*/        ,ADC_CHANNEL_3
                                                /* LockFFfan RML*/          ,ADC_CHANNEL_13
                                                /* LockIcefan IMFL*/        ,ADC_CHANNEL_14           
                                                /* LockConfan KML*/         ,ADC_CHANNEL_5
                                                /* LockVacuumPump XML*/     ,ADC_CHANNEL_7
                                                /* LockWDispenPump WD_IN*/  ,ADC_CHANNEL_9
                                                /* LockIMEmpty IMTEL*/      ,ADC_CHANNEL_6)

DEV_IO_ADC_CREATE(adcRSEN, adcCore,  0 )
DEV_IO_ADC_CREATE(adcRDSEN, adcCore, 1 )
DEV_IO_ADC_CREATE(adcFSEN, adcCore,  2 )
DEV_IO_ADC_CREATE(adcFDSEN, adcCore, 3 )
DEV_IO_ADC_CREATE(adcIMSEN, adcCore, 4 )
DEV_IO_ADC_CREATE(adcTCSEN, adcCore, 5 )
DEV_IO_ADC_CREATE(adcHSEN, adcCore,  6 )
DEV_IO_ADC_CREATE(adcXSEN, adcCore,  7 )
DEV_IO_ADC_CREATE(adcRML, adcCore,   8 )    
DEV_IO_ADC_CREATE(adcIMFL, adcCore,  9 )     
DEV_IO_ADC_CREATE(adcKML, adcCore,   10)
DEV_IO_ADC_CREATE(adcXML, adcCore,   11)
DEV_IO_ADC_CREATE(adcWD_IN, adcCore, 12)
DEV_IO_ADC_CREATE(adcIMTEL, adcCore, 13)


const tsDevIo *adcs[] = {
	&adcRSEN,
	&adcRDSEN,
	&adcFSEN,
        &adcFDSEN,
        &adcIMSEN,
        &adcTCSEN,    
        &adcHSEN,
        &adcXSEN,
        &adcRML,     
        &adcIMFL,     
        &adcKML,
        &adcXML,
        &adcWD_IN,
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
DEV_IO_FILTER_CREATE(filteredAdcXSEN, adcXSEN, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcRML, adcRML, 20, 4)           
DEV_IO_FILTER_CREATE(filteredAdcIMFL, adcIMFL, 20, 4)         
DEV_IO_FILTER_CREATE(filteredAdcKML, adcKML, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcXML, adcXML, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcWD_IN, adcWD_IN, 20, 4)
DEV_IO_FILTER_CREATE(filteredAdcIMTEL, adcIMTEL, 20, 4)


#include "dev/ntc.h"
DEV_IO_NTC_CREATE(ntcRSEN, filteredAdcRSEN)
DEV_IO_NTC_CREATE(ntcRDSEN, filteredAdcRDSEN)
DEV_IO_NTC_CREATE(ntcFSEN, filteredAdcFSEN)
DEV_IO_NTC_CREATE(ntcFDSEN, filteredAdcFDSEN)
DEV_IO_NTC_CREATE(ntcIMSEN, filteredAdcIMSEN)
DEV_IO_NTC_CREATE(ntcTCSEN, filteredAdcTCSEN)
DEV_IO_NTC_CREATE(ntcHSEN, filteredAdcHSEN)
DEV_IO_NTC_CREATE(ntcXSEN, filteredAdcXSEN)
DEV_IO_NTC_CREATE(ntcRML, filteredAdcRML)             
DEV_IO_NTC_CREATE(ntcIMFL, filteredAdcIMFL)           
DEV_IO_NTC_CREATE(ntcKML, filteredAdcKML)
DEV_IO_NTC_CREATE(ntcXML, filteredAdcXML)
DEV_IO_NTC_CREATE(ntcWD_IN, filteredAdcWD_IN)
DEV_IO_NTC_CREATE(ntcIMTEL, filteredAdcIMTEL)


DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorRSEN, ntcRSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorRDSEN, ntcRDSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFSEN, ntcFSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorFDSEN, ntcFDSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorIMSEN, ntcIMSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorTCSEN, ntcTCSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorHSEN, ntcHSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorXSEN, ntcXSEN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorRML, ntcRML)         
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorIMFL, ntcIMFL)       
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorKML, ntcKML)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorXML, ntcXML)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorWD_IN, ntcWD_IN)
DEV_IO_NTC_ERROR_DETECTOR_CREATE(ntcErrorIMTEL, ntcIMTEL)

const tsDevIo *ntcs[] = {
		&ntcRSEN,
		&ntcRDSEN,
		&ntcFSEN,
		&ntcFDSEN,
		&ntcIMSEN,
		&ntcTCSEN,
		&ntcHSEN,
		&ntcXSEN,
		&ntcRML,              
		&ntcIMFL,             
		&ntcKML,
		&ntcXML,
		&ntcWD_IN,
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
                &ntcErrorXSEN,
		&ntcErrorRML,         
                &ntcErrorIMFL,        
		&ntcErrorKML,
                &ntcErrorXML,
                &ntcErrorWD_IN,
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

// RFAN Channel A    RFAN_B
DEV_CPX_PWM_CORE_CREATE(rfanPwmCore, RFAN_B, B , 32000, 0)     // RFAN Channel B
DEV_IO_PWM_DUTY_CREATE(dutyRFAN, rfanPwmCore)  

// FFAN Channel B
DEV_CPX_PWM_CORE_CREATE(ffanPwmCore, FFAN_B, B , 32000, 0)     // FFAN Channel B
DEV_IO_PWM_DUTY_CREATE(dutyFFAN, ffanPwmCore)  


// RLED Channel A
DEV_CPX_PWM_CORE_CREATE(rledPwmCore, LED_DRIVE, A, 600, 0)             // RLED Channel A
DEV_IO_PWM_FREQ_CREATE(rled_freq, rledPwmCore)
DEV_IO_PWM_DUTY_CREATE(rled_duty, rledPwmCore)  

// FLED Channel B
DEV_CPX_PWM_CORE_CREATE(fledPwmCore, LED_DRIVE, B , 600, 0)             // FLED Channel B
DEV_IO_PWM_FREQ_CREATE(fled_freq, fledPwmCore)
DEV_IO_PWM_DUTY_CREATE(fled_duty, fledPwmCore)  

// R_LED Channel A
DEV_CPX_PWM_CORE_CREATE(r_ledPwmCore, R_LED_A, A , 600, 0)                   // R_LED Channel A
DEV_IO_PWM_FREQ_CREATE(r_led_freq, r_ledPwmCore)
DEV_IO_PWM_DUTY_CREATE(r_led_duty, r_ledPwmCore)    
// G_LED Channel B
DEV_CPX_PWM_CORE_CREATE(g_ledPwmCore, B_LED_A_G_LED_B, B , 600, 0)             // GLED Channel B
DEV_IO_PWM_FREQ_CREATE(g_led_freq, g_ledPwmCore)
DEV_IO_PWM_DUTY_CREATE(g_led_duty, g_ledPwmCore)  
// B_LED Channel A
DEV_CPX_PWM_CORE_CREATE(b_ledPwmCore, B_LED_A_G_LED_B, A , 600, 0)             // B_LED Channel A
DEV_IO_PWM_FREQ_CREATE(b_led_freq, b_ledPwmCore)
DEV_IO_PWM_DUTY_CREATE(b_led_duty, b_ledPwmCore)  



// BT_LED Channel A   
DEV_CPX_PWM_CORE_CREATE(btledPwmCore, BTLED_A_XLED_B, A , 600, 0)             // BT_LED Channel A
DEV_IO_PWM_FREQ_CREATE(btled_freq, btledPwmCore)
DEV_IO_PWM_DUTY_CREATE(btled_duty, btledPwmCore)    
// XLED Channel B
DEV_CPX_PWM_CORE_CREATE(xledPwmCore, BTLED_A_XLED_B, B , 600, 0)             // XLED Channel B
DEV_IO_PWM_FREQ_CREATE(xled_freq, xledPwmCore)
DEV_IO_PWM_DUTY_CREATE(xled_duty, xledPwmCore)    

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
DEV_IO_FADE_CREATE(fadingR_LED, r_led_duty, 1, 10)
DEV_IO_FADE_CREATE(fadingG_LED, g_led_duty, 1, 10)
DEV_IO_FADE_CREATE(fadingB_LED, b_led_duty, 1, 10)
DEV_IO_FADE_CREATE(fadingBT_LED, btled_duty, 1, 10)
DEV_IO_FADE_CREATE(fadingXLED, xled_duty, 1, 10)


const tsDevIo *fadingLights[] = {
	&fadingRLED,
        &fadingFLED,
        &fadingR_LED,
        &fadingG_LED,
        &fadingB_LED,
        &fadingBT_LED,
        &fadingXLED,
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
//DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorK, fanK)
//DEV_IO_FAN_ERROR_DETECTOR_CREATE(fanErrorIM, fanIM)

const tsDevIo *fans[] = {
	&fanR,
        &fanF,
      /*&fanK,
        &fanIM,*/
};

const tsDevIo *fanErrorDetectors[] = {
	&fanErrorR,
        &fanErrorF,
     /* &fanErrorK,
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
        devCpxInit(&r_ledPwmCore, NULL);
        devCpxInit(&g_ledPwmCore, NULL);
        devCpxInit(&b_ledPwmCore, NULL);
        devCpxInit(&btledPwmCore, NULL);
        devCpxInit(&xledPwmCore, NULL);
		devCpxInit(&vccDrivePwmCore, NULL);
        devCpxInit(&display, NULL);
        devIoInit(&stepperMotor, NULL);
        devIoInit(&bdcIceMotor, NULL);
        devIoInit(&bdcIcePumpMotor, NULL);
        devIoInit(&bdcDispPumpMotor, NULL);
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
                        devIoInit(&r_led_freq, NULL);
                        devIoInit(&g_led_freq, NULL);
                        devIoInit(&b_led_freq, NULL);
                        devIoInit(&btled_freq, NULL);
                        devIoInit(&xled_freq, NULL);
                        
		}

		break;

	case eDeviceTypeNTC:
        case eDeviceTypeNTC_R_air:
        case eDeviceTypeNTC_R_eva:
        case eDeviceTypeNTC_F_air:
        case eDeviceTypeNTC_F_eva:
        case eDeviceTypeNTC_Ambient:
        case eDeviceTypeNTC_Humidity:
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
    static uint32_t eepval[8] = {0,0,0,0,0,0,0,0};
    static uint32_t eepread[8];
    static unsigned int addr = 0;
    if (FTC.Eeprom.index < 1)
      dev = &eepromDev;
    if(FTC.Eeprom.value)
    {
      if(addr > 0x0FFF)
      {
        FTC.Eeprom.feedback = 3;
        FTC.Eeprom.value = 0;
        addr = 0;
        return;
      }
      devMemWrite(dev, addr, &eepval, 32); 
      addr += 32;  
      //Add feedback writing
      FTC.Eeprom.feedback = 2;  
    }
    //Add feedback complete or error
    if(!FTC.Eeprom.value && FTC.Eeprom.feedback == 3)
    {
      if(addr > 0x0FFF)
      {
        FTC.Eeprom.feedback = 1;
        return;
      }
      devMemRead(dev, addr, &eepread, 32);
      if(memcmp(eepread, eepval, sizeof(eepval)) == 0)
      {
        FTC.Eeprom.feedback = 3;
        addr += 32;
      }
      else
      {
        FTC.Eeprom.feedback = 4;
        addr = 0xFFFF;
      }
    }
    //Clear address after complete or error
    if(!FTC.Eeprom.value && (FTC.Eeprom.feedback == 1 || FTC.Eeprom.feedback == 4))
      addr = 0;
    //Status
    //1: is completed , 2: is writing, 3: is Reading, 4: error(Need the command write again)
}

void boardWiFiUartFTC(void)
{
      const tsDevCom *dev = NULL;
                                        /*H  R  F  A  0  7  5  1  A  0*/        //Drawing number of AHTH.
      static uint8_t WiFiUartSend[10] = {72,82,70,65,48,55,53,49,65,48};        //ASCII of drawing number.
      static uint8_t WiFiUartRead[10];
      //Define for data receive check and waiting time receive.
      uint16_t rxLen = 0;
      uint16_t timeout = 50000;
      
      if (FTC.WiFiUart.index < 1)
        dev = &wifiUart;
      else
        return;
      
      if(!FTC.WiFiUart.value)
      {//Standby for start test (Waiting send command).
        if(!FTC.WiFiUart.feedback)
          devComClose(dev, NULL);
        else
          FTC.WiFiUart.feedback = FTC.WiFiUart.feedback;
      }
      else
      {//Start working to test
        //Priority 1 : First operation (first start, start again when status ok or ng).
        if(FTC.WiFiUart.feedback == 0 || FTC.WiFiUart.feedback == 1 || FTC.WiFiUart.feedback == 4)
        {
            devComOpen(dev, NULL);
            FTC.WiFiUart.feedback = 2;
            //Reset data in WiFiUartRead store for keep new data.
            for(uint8_t i = 0; i < sizeof(WiFiUartRead);i++)
            {
              WiFiUartRead[i] = 0;
            }
        }
        //Priority 2 : Start send packet and read packet data operation.
        else if(FTC.WiFiUart.feedback == 2)
        {
            //Send/Receive packet.
            devComSend(dev, &WiFiUartSend, sizeof(WiFiUartSend));
            while ((rxLen < sizeof(WiFiUartRead)) && timeout--)
            {
                rxLen += devComReceive(dev, &WiFiUartRead[rxLen],
                                        sizeof(WiFiUartRead) - rxLen);
            }
            devComClose(dev, NULL);
            
            //Confirm the packet receive complete as send pack size
            if (rxLen == sizeof(WiFiUartSend))
            {//Receive pack data correct as pack size.
              FTC.WiFiUart.feedback = 3;
            }
            else
            {//Receive pack data incorrect as pack size.
              FTC.WiFiUart.feedback = 4;
              FTC.WiFiUart.value = 0;
            }
        }
        //Priority 3 : Comfirm data in read and send packet are same or not.
        else if(FTC.WiFiUart.feedback == 3)
        {
            //The data in read and send are same
            if(memcmp(WiFiUartRead, WiFiUartSend, sizeof(WiFiUartSend)) == 0)
            {
              FTC.WiFiUart.feedback = 1;
              FTC.WiFiUart.value = 0;
            }
            else
            {
              FTC.WiFiUart.feedback = 4;
              FTC.WiFiUart.value = 0;
            }
        }
      }
//    //Status
//    //1: is completed , 2: is writing, 3: is Reading, 4: error(Need the command write again)
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
    devCpxInit(&r_ledPwmCore, NULL);
    devCpxInit(&g_ledPwmCore, NULL);
    devCpxInit(&b_ledPwmCore, NULL);
    devCpxInit(&btledPwmCore, NULL);
    devCpxInit(&xledPwmCore, NULL);
    devCpxInit(&rfanPwmCore, NULL);
    devCpxInit(&ffanPwmCore, NULL);
    devCpxInit(&display, NULL);
    devMemInit(&eepromDev);
    devComInit(&wifiUart);

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
  boardWiFiUartFTC();
  // displat entegrasyonu bitince ui_ftc_state dosyasi kullanilarak display kontrol?de eklenmelidir.
  
}

/*** APW : Add Dev control ***/
stDevForce DevForce;

void boardDevCompOut(void)
{
    const tsDevIo *dev = NULL;

    if (DevForce.CompOut.index < ARRAY_SIZE(softPwmFreqOutputs))
    {
        dev = softPwmFreqOutputs[DevForce.CompOut.index];
        devIoInit(softPwmDutyOutputs[DevForce.CompOut.index], NULL);
    }
    
    DevForce.CompOut.value = refrigerator_R.Cooler.components.compressor.value;
    
    if(DevForce.CompOut.value)
    {
      if(DevForce.CompOut.PrvValue != DevForce.CompOut.value)
      {
        DevForce.CompOut.PrvValue = DevForce.CompOut.value;
        if(refrigerator_R.Cooler.components.compressor.Readvalue)
          devIoPut(dev, DevForce.CompOut.value);
        else
        {
          devIoPut(dev, 0);
          DevForce.CompOut.PrvValue = 0;
        }
      }
      else
        if(!refrigerator_R.Cooler.components.compressor.Readvalue)
          DevForce.CompOut.PrvValue = 0;
    }
    else
      devIoPut(dev, DevForce.CompOut.value);
}

void boardDevForce20MsTask(void)
{
    boardDevCompOut();
}
/*** APW : Add Dev control ***/
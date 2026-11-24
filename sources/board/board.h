/*
 * Copyright 2017, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    board.h
 * @brief   Board initialization header file.
 */

/* This is an empty template for board specific configuration.*/

#ifndef _BOARD_H_
#define _BOARD_H_

#include "rcos.h"

/**
 * @brief	The board name
 */
#define BOARD_NAME "board"

//#define ENERGYMONITORING              //APW : Defined in options compiler
//#define New_algorithm                 //APW : Inverter -> Defined in options compiler
//#define SERVICEMODEavailable          //APW : -> Defined in options compiler
//#define HOSHI_H30                     //APW : -> If need to define, move to defined in options compiler

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

	/**
	 * @brief 	Initialize board specific settings.
	 */
	void BOARD_InitDebugConsole(void);

	typedef enum
	{
		eDeviceTypeNone = 0,
		eDeviceTypeACOutputComp,
		eDeviceTypeACOutputDelayable,
		eDeviceTypeACOutputNotDelayable,
		eDeviceTypeDigInput,
		eDeviceTypeReversedDigInput,
		eDeviceTypeFilteredDigInput,		 // debounced
		eDeviceTypeFilteredReversedDigInput, // debounced
		eDeviceTypeDigOutput,
		eDeviceTypeSoftFreq,
#ifdef ALPS_H50
                eDeviceTypeSoftDuty,
#endif
                eDeviceTypeComm,
		eDeviceTypeFadingLight,
		eDeviceTypeNTC,
                eDeviceTypeNTC_R_air,
                eDeviceTypeNTC_R_eva,
                eDeviceTypeNTC_F_air,
                eDeviceTypeNTC_F_eva,
                eDeviceTypeNTC_Ambient,
                eDeviceTypeNTC_Humidity,
                eDeviceTypeNTC_IM_case,
		eDeviceTypeNTCErrorDetector,
		eDeviceTypeKnob,
		eDeviceTypeKnobError,
		eDeviceTypeFan,
		eDeviceTypeFanErrorDetector,
		eDeviceTypeStepperMotor,
		eDeviceTypeDCMotor,
		eDeviceTypeADC,
		eDeviceTypeSHT3XTemp,
		eDeviceTypeSHT3XHumidity,
		eDeviceTypeSHT3XErrorDetector,
	} teBoardDeviceType;

	extern const tsDevIo buzzer;
	extern const tsDevCpx display;

	extern const tsDevCom debugUart;
	extern const tsDevCom udaqUart;
        extern const tsDevCom devComSpiSlave;
        extern const tsDevCom i2cen;  
        extern const tsDevCom wifiUart;
	//extern const tsDevMem eeprom;
        extern const tsDevMem eepromDev; // MJI
	extern const tsDevIo *adcs[];
	extern const tsDevIo acOutputsController;
	extern const tsDevIo *ntcs[];
        extern const tsDevIo    filteredAdcRSEN;
        extern const tsDevIo    filteredAdcRDSEN;
        extern const tsDevIo    filteredAdcFSEN;
        extern const tsDevIo    filteredAdcFDSEN;
        extern const tsDevIo    filteredAdcIMSEN;
        extern const tsDevIo    filteredAdcTCSEN;
        extern const tsDevIo    filteredAdcHSEN;
        //extern const tsDevIo vccDriveEn;      //APW : Disable for new MCU
	const tsDevIo *boardInitDevice(teBoardDeviceType deviceType, uint8_t deviceIdx, const void *config);
	void boardInitBoard(void);
	void boardInitFTC(void);
	void boardFTC20MsTask(void);
        void boardDevForce20MsTask(void);    //APW : Add Dev control


typedef struct{
  uint8_t ICT;
  uint8_t FTC;
}stTests;

typedef struct{
  struct{ uint8_t value;                                        } Buzzer;
  struct{ uint8_t ICT;    uint8_t  FTC;                         } Tests;  
  struct{ uint8_t index;  uint8_t  value;                       } ACOut;
  struct{ uint8_t index;  uint8_t  value;                       } ACSwitch;
  struct{ uint8_t index;  uint8_t  value;                       } BDCMotor;
  struct{ uint8_t index;  uint8_t  value;                       } CSDCMotor;
  struct{ uint8_t index;  uint8_t  value;                       } DCOut;
  struct{ uint8_t index;  uint8_t  value;                       } DCSwitch;
  struct{ uint8_t index;  uint8_t  value;                       } VCC;  
  struct{ uint8_t index;  uint16_t value;                       } ADC_;
  struct{ uint8_t index;  uint16_t value;                       } PWMOut;
  struct{ uint8_t index;  uint16_t value;                       } StepMotor;
  struct{ uint8_t index;  uint16_t value; uint16_t speed;       } VSDCMotor; 
  struct{ uint8_t index;  uint8_t  value; uint8_t  feedback;    } Eeprom;
  struct{ uint8_t index;  uint8_t  value; uint8_t  feedback;    } WiFiUart;
#ifdef ALPS_H50
  struct{ uint8_t index;  uint8_t  value;                       } DUTY;  
#endif
}stFTC;

extern stFTC FTC;
/*** APW : Add Dev control ***/
typedef struct{
  struct{ uint8_t index;  uint16_t value; uint16_t PrvValue;    } CompOut;
}stDevForce;

extern stDevForce DevForce;
/*** APW : Add Dev control ***/
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */

/** @file       tle941.h
 *  @brief      Header file of TLE941 device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-18
 *
 *  @details
 *  This device is used to control the outputs of a TLE941 IC (multi-half bridge output)
 *  which is used commonly to control DC motors and stepper motors.
 * 
 *  In output devices creation macros "1" indexed output numbers are used (as the order in the datasheet)
 *  but in the code "0" indexed output numbers are used.
 * 
 *  The active level of an output can be set, 
 *  For outputs with high active levels: 
 *      writing 1: switches the high side switch on, and the low side switch off.
 *      writing 0: does the opposite.
 *  
 *  For outputs with low active levels:
 *      writing 1: switches the low side switch on, and the high side switch off.
 *      writing 0: does the opposite.
 *
 *  After initializing the core, both high and low side switches of all outputs are switched off.
 *  After initializing an output, the INActive level is applied for that output.
 *  
 *  @note   When we use this device to drive motors, Outputs are used in pairs. In this case driving
 *          one output only is not useful. Both outputs should be driven at the same time.
 * 
 *          For example: 
 *          If outputs 1 & 2 are connected to a DC motor. The creation macro of the outputs will be like this
 *          DEV_IO_TLE941_OUTPUT_CREATE(motorP, tle941Core, 1, 1)
 *          DEV_IO_TLE941_OUTPUT_CREATE(motorN, tle941Core, 2, 1) 
 
 *          The motor can be controlled as below :
 *          // To rotate in CW direction
 *          devIoPut(&motorP, 1);
 *          devIoPut(&motorN, 0);
 *          // To stop the motor
 *          devIoPut(&motorP, 0);
 *          devIoPut(&motorN, 0);
 *          // To rotate in CCW direction
 *          devIoPut(&motorP, 0);
 *          devIoPut(&motorN, 1);
 * 
 * 
 *  @code
 *  // Creation ...
 *  #include "dev/spi.h"
 *  DEV_COM_SPI_CREATE(spiCore, 1, 100000, kLPSPI_LsbFirst, kLPSPI_SdiInSdoOut, kLPSPI_ClockPolarityActiveHigh, kLPSPI_ClockPhaseSecondEdge, 10, 10)
 *
 *  #include "dev/ke14/gpio.h"
 *  DEV_IO_GPIO_CREATE(tle941CS, T_CSN, 0, 0, 0)    // Attention: that the active level of a CS device is 0 !!!
 *  DEV_IO_GPIO_CREATE(tle941EN, T_EN, 0, 0, 1)
 *
 *  include "dev/tle941.h"
 *  DEV_CPX_TLE941_CORE_CREATE(tle941Core, spiCore, tle941CS, tle941EN)
 *  // The next devices will be used to controller unipolar stepper motor with 5-leads
 *  // where the common pin is connected to 12V, so the active level of the poles should be 0 (low)
 *  DEV_IO_TLE941_OUTPUT_CREATE(motorAp, tle941Core, 1, 0)  
 *  DEV_IO_TLE941_OUTPUT_CREATE(motorAn, tle941Core, 2, 0)
 *  DEV_IO_TLE941_OUTPUT_CREATE(motorBp, tle941Core, 4, 0)
 *  DEV_IO_TLE941_OUTPUT_CREATE(motorBn, tle941Core, 3, 0)
 *
 *  // Initialization ...
 *  devCpxInit(&tle941Core, NULL);
 *  devIoInit(&motorAp, NULL);
 *  devIoInit(&motorAn, NULL);
 *  devIoInit(&motorBp, NULL);
 *  devIoInit(&motorBn, NULL);
 * 
 *  // Usage ..
 *  devIoPut(&motorAp, 1);
 *  devIoPut(&motorAn, 0);
 *  devIoPut(&motorBp, 0);
 *  devIoPut(&motorBn, 0);
 *  @endcode
 */
#ifndef FILE_TLE941_H
#define FILE_TLE941_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_TLE941_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   TLE941 TLE941
 *  @ingroup    DEV_CPX
 *  @brief      Driving TLE941 ICs multi-half bridge outputs through SPI
 *  @{
 */

/// @brief  Functions for TLE941 devices
INTERFACE const tsDevCpxFuncs devCpxTLE941Funcs;

/// @brief  Device specific parameters
typedef struct
{
    tsTarget target;           ///< Device target information on communication device
    size_t regIdx;             ///< Current register index, defined here to use it inside the PThread
    tTicket ticket;            ///< Ticket that will be used for SPI communication
    uint8_t regHBCTRL[3];      ///< Half-bridge output control registers values
    uint8_t buffer[2];         ///< buffer used in SPI communication
    volatile uint8_t writeReg; ///< each bit is a flag indicates if regHBCTRL byte with that index should be written
                               ///< For example: 0x5 indicates that register  HBCTRL[0] and HBCTRL[2] should be written
} tsTLE941CoreParams;

/// @brief  Device specific constants
typedef struct
{
    const tsDevCom *spi; ///< the SPI device that will be used in communications
    const tsDevIo *en;   ///< The GPIO device that is used to generate EN signal
    tsProcess *process;  ///< Process that handles the jobs for this device

} tsTLE941CoreConsts;

/** @brief  Create a devCpx for TLE941
 *  @param  _name   Name of devCpx object
 *  @param  _spi    the SPI device that will be used in communications
 *  @param  _cs     The GPIO device that will be used to generate chip select signal
 *  @param  _en     The GPIO device that will be used to generate EN signal
 *  @note   spi, cs and en devices are initialized in devIoInit function of this device.
 *          No need to initialize them again.
 */
#define DEV_CPX_TLE941_CORE_CREATE(_name, _spi, _cs, _en)                                                                \
    tsProcess _name##Process;                                                                                            \
    tsTLE941CoreParams _name##Params =                                                                                   \
        {                                                                                                                \
            .target = {.cs = &_cs},                                                                                      \
    };                                                                                                                   \
    const tsTLE941CoreConsts _name##Consts =                                                                             \
        {                                                                                                                \
            .spi = &_spi,                                                                                                \
            .en = &_en,                                                                                                  \
            .process = &_name##Process,                                                                                  \
    };                                                                                                                   \
    PROCESS_CREATE(_name##Process, tle941ProcessInit, tle941ProcessDeinit, PROCESS_NONE, &_name##Params, &_name##Consts) \
    DEV_CPX_CREATE(_name, devCpxTLE941Funcs, &_name##Params, &_name##Consts)

INTERFACE const tsDevIoFuncs devIoTLE941OutputFuncs;

typedef struct
{
    const tsDevCpx *core;    ///< the TLE941 device that the output is associated to it
    uint8_t outputNo : 4;    ///< The output number 0 indexed
    uint8_t activeLevel : 1; ///< 0 = low level, 1= high level
} tsTLE941OutputConsts;

/** @brief  Create a devIo for an output in a TLE941 device
 *  @param  _name   Name of devIo object
 *  @param  _core   The TLE941 device that the output is associated to it
 *  @param  _outputNo   The output number 1 indexed (as the number in the datasheet)
 *  @param _activeLevel 0 = low level, 1= high level
 */
#define DEV_IO_TLE941_OUTPUT_CREATE(_name, _core, _outputNo, _activeLevel) \
    tsTLE941OutputConsts _name##Consts =                                   \
        {                                                                  \
            .core = &_core,                                                \
            .outputNo = _outputNo - 1,                                     \
            .activeLevel = _activeLevel,                                   \
    };                                                                     \
    DEV_IO_CREATE(_name, devIoTLE941OutputFuncs, NULL, &_name##Consts)

#undef INTERFACE // Should not let this roam free

/** @} */
#endif // FILE_TLE941_H

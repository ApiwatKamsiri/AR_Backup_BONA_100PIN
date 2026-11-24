/** @file       stepper_motor.c
 *  @brief      C file of STEPPER_MOTOR device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-06
 */
#define FILE_STEPPER_MOTOR_C
#include "app/refrigerator/rf_ram.h" 		//APW
#include "app/refrigerator/rf_configs.h" 	//APW
#include "app/cabin/cabin_configs.h"    	//APW
#include "stepper_motor.h"

/**
 *  @addtogroup STEPPER_MOTOR
 *  @{
 */
#define BLOCKING_TIME_MS 500    //200
#define ONE_HOUR_IN_MS (60UL * 60UL * 1000UL)
#define FIRST_STEP_TIME_MS 500

uint8_t StepPositions[4] = {34, 195, 100, 154};
unsigned char FirstStepFlag = 1;

enum eStepperMotorDirections
{
    eStop = 0,
    eCW,
    eCWW,
};

enum eStepperMotorPoles
{
    AP = 0, // A positive
    AN,     // A negative
    BP,     // B positive
    BN,     // B negative
    POLES_COUNT,
};

typedef struct
{
    uint8_t stepsCount;              ///< number of steps in this driving method
    uint8_t polesState[POLES_COUNT]; ///< each byte represents a pole, each bit represents the pole state at this bit'th step
} tsDrivingMethod;

/// @brief Struct holds the steps count and the poles states of all driving methods
const tsDrivingMethod drivingMethods[eStepperMotorDrivingMethodsCount] =
    {
        [eStepperMotorDrivingMethodFullStep] =
            {
                .stepsCount = 4,
                .polesState = {[AP] = 0x9 /*1001*/, [AN] = 0x6, [BP] = 0x3, [BN] = 0xC},
            },
        [eStepperMotorDrivingMethodHalfStep] =
            {
                .stepsCount = 8,
                .polesState = {[AP] = 0xC1, [AN] = 0x1C, [BP] = 0x70, [BN] = 0x07},
               // .polesState = {[AP] = 0x83, [AN] = 0x38, [BP] = 0x0E, [BN] = 0xE0},
            },
        [eStepperMotorDrivingMethodWaveDrive] =
            {
                .stepsCount = 4,
                .polesState = {[AP] = 0x1, [AN] = 0x4, [BP] = 0x2, [BN] = 0x8},
            },
};

static PT_THREAD(stepperMotorThrdHandler);
static TIMER_CALLBACK_FUNC(calibrationTimerHandler);
static TIMER_CALLBACK_FUNC(unblockingTimerHandler);
static void startMotor(const tsStepperMotorConsts *consts);
static void stopMotor(tsStepperMotorParams *params, const tsStepperMotorConsts *consts);
static void applyCurrentStep(tsStepperMotorParams *params, const tsStepperMotorConsts *consts);
static bool anyWorkToDo(tsStepperMotorParams *params);

/* DevIo Functions *********************************************/

static DEV_IO_FUNC_INIT(stepperMotorInit);
static DEV_IO_FUNC_DEINIT(stepperMotorDeinit);
static DEV_IO_FUNC_GET(stepperMotorGet);
static DEV_IO_FUNC_PUT(stepperMotorPut);

/// @brief  Structure that defines the functions for stepperMotor
const tsDevIoFuncs devIoStepperMotorFuncs =
    {
        stepperMotorInit,
        stepperMotorDeinit,
        stepperMotorGet,
        stepperMotorPut,
};

/// @brief  Init function for a stepperMotor
static DEV_IO_FUNC_INIT(stepperMotorInit)
{
    tsStepperMotorParams *params = device->parameters;
    const tsStepperMotorConsts *consts = device->constants;

    // re initializing the motor with NULL pointer allowed if it's firstly initialized with valid configs
    if (config)
        params->configs = config;
    if (params->configs == NULL)
        return EXIT_FAILURE;

    if (device->sys->initialized == 0)
    {
        device->sys->initialized = 1;

        // assign the functions of the callback timers
        params->calibrationTimer.callBack = calibrationTimerHandler;
        params->unblockingTimer.callBack = unblockingTimerHandler;

        for (size_t i = 0; i < POLES_COUNT; ++i)
            devIoInit(consts->poleDevices[i], NULL);

        processStart(consts->process);
    }

    // (re)initializing the motor triggers the calibration operation
    if (params->configs->calibrateAtInit)
        params->calibrationNeeded = true;

    startMotor(consts);
    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a stepperMotor
static DEV_IO_FUNC_DEINIT(stepperMotorDeinit)
{
    tsStepperMotorParams *params = device->parameters;
    const tsStepperMotorConsts *consts = device->constants;

    if (device->sys->initialized)
    {
        device->sys->initialized = 0;
        stopMotor(params, consts);
        processStop(consts->process);
        for (size_t i = 0; i < POLES_COUNT; ++i)
            devIoDeinit(consts->poleDevices[i]);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/// @brief  Get function for a stepperMotor
/// @return the current position of the motor
static DEV_IO_FUNC_GET(stepperMotorGet)
{
    tsStepperMotorParams *params = device->parameters;
    return params->currentPosition;
}

/// @brief  Put function for a stepperMotor
/// @param data the required position (number of steps away from the inital position (0))
static DEV_IO_FUNC_PUT(stepperMotorPut)
{
    tsStepperMotorParams *params = device->parameters;
    const tsStepperMotorConsts *consts = device->constants;

    if (!device->sys->initialized)
        return EXIT_FAILURE;

    if (params->requiredPosition != data)
    {
        params->requiredPosition = data;
        if (params->configs->movementsCountForCalibration > 0 && !params->calibrationOngoing)
        {
            if (params->movementsCounter < params->configs->movementsCountForCalibration)
                params->movementsCounter++;
            else
                params->calibrationNeeded = true;
        }
        if (!params->blocked)
            startMotor(consts);
    }
    return EXIT_SUCCESS;
}
/**************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 */

/* Process Functions *******************************************/

PROCESS_INIT_PROTO(stepperMotorProcessInit)
{
    tsStepperMotorParams *params = process->parameters;

    params->calibrationTimer.callBack = calibrationTimerHandler;
    params->unblockingTimer.callBack = unblockingTimerHandler;
    process->initialized = 1;
}

PROCESS_DEINIT_PROTO(stepperMotorProcessDeinit)
{
    process->initialized = 0;
}
/**************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 */

/* Thread Function *********************************************/

static PT_THREAD(stepperMotorThrdHandler)
{
    tsStepperMotorParams *params = process->parameters;
    const tsStepperMotorConsts *consts = process->constants;

    uint32_t targetPosition;
    uint8_t newDirection;

    PT_BEGIN();
    for (;;)
    {
        if (!anyWorkToDo(params))
        {
            stopMotor(params, consts);
            PT_EXIT();
        }

        // calibration starting condition : motor is stopped (last movement is finished) && calibration is needed
        if (params->currentDirection == eStop && params->calibrationNeeded && !params->calibrationOngoing)
        {
            params->calibrationOngoing = true;
            params->currentPosition = params->configs->calibrationPosition;
        }
        /*** APW : For separate step moter between valve and damper ***/  
        for (uint8_t i = 0; i < 2; i++)
        if(refrigerator_F.Cooler.type != _CoolerType_DEFAULT && 
            refrigerator_F.Cooler.components.solenoidValve[i].type == _SolenoidValveType_StepMotor)
              targetPosition = params->calibrationOngoing ? 0 : StepPositions[params->requiredPosition];
          
        for (uint8_t i = 0; i < _NumOfCabin; i++)        
          if(refrigerator_F.Cabin[i].type == _CabinType_FLAP)
              targetPosition = params->calibrationOngoing ? 0 : params->requiredPosition;     
        /*** APW : For separate step moter between valve and damper ***/                                   
        newDirection = targetPosition > params->currentPosition   ? eCWW
                       : targetPosition < params->currentPosition ? eCW
                                                                  : eStop;
        /*
        currentDirection        newDirection        Action
        eStop                   eStop               Stop
        eStop                   eCW                 Move CW (if motor is not blocked)
        eStop                   eCWW                Move CWW (if motor is not blocked)
        eCW                     eStop               Stop
        eCW                     eCW                 Continue in CW direction
        eCW                     eCWW                Stop, wait block time, then Move CWW
        eCWW                    eStop               Stop
        eCWW                    eCW                 Stop, wait block time, then Move CW
        eCWW                    eCWW                Continue in CWW direction
        */
        if (newDirection == eStop || (params->currentDirection != eStop && newDirection != params->currentDirection))
        {
            stopMotor(params, consts);
            PT_EXIT();
        }

        if (newDirection == eCW)
        {
            params->currentStep = (params->currentStep + 1) % drivingMethods[params->configs->drivingMethod].stepsCount;
            params->currentPosition--;
        }
        else // (newDirection == eCWW)
        {
            if (params->currentStep == 0)
                params->currentStep = drivingMethods[params->configs->drivingMethod].stepsCount - 1;
            else
                params->currentStep--;
            params->currentPosition++;
        }    
        /*** APW : Add damper operation in CT ***/
          for (uint8_t i = 0; i < _NumOfCabin; i++)
            if(refrigerator_F.Cabin[i].type == _CabinType_FLAP)
              refrigerator_R.Cabin[i].components.flap.flapFeed = params->currentPosition;
        /*** APW : Add damper operation in CT ***/
        applyCurrentStep(params, consts);
        params->currentDirection = newDirection;

        // calibration ending condition : motor reached 0 position during calibration operation
        if (params->calibrationOngoing && params->currentPosition == 0)
        {
            params->calibrationOngoing = false;
            params->calibrationNeeded = false;
            params->movementsCounter = 0;
            if (params->configs->calibrationTimeH > 0)
                timerCallBackStart(&params->calibrationTimer, params->configs->calibrationTimeH * ONE_HOUR_IN_MS);
        }
        
        if(targetPosition == params->currentPosition)
          FirstStepFlag = 1;
        
        if(FirstStepFlag)
        {
          FirstStepFlag = 0;
          PT_YIELD_TIMEOUT(FIRST_STEP_TIME_MS);
        }
        else
        {
          PT_YIELD_TIMEOUT(consts->periodBetweenStepsMs);
        }
//        PT_YIELD_TIMEOUT(consts->periodBetweenStepsMs);
    }
    PT_END();
}

/**************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 */

/* Static Functions *******************************************/

static TIMER_CALLBACK_FUNC(unblockingTimerHandler)
{
    tsStepperMotorParams *params = ((tsDevIo *)parameter)->parameters;
    const tsStepperMotorConsts *consts = ((tsDevIo *)parameter)->constants;

    params->blocked = false;
    if (anyWorkToDo(params))
        startMotor(consts);

    return 0;
}

static TIMER_CALLBACK_FUNC(calibrationTimerHandler)
{
    tsStepperMotorParams *params = ((tsDevIo *)parameter)->parameters;
    const tsStepperMotorConsts *consts = ((tsDevIo *)parameter)->constants;

    params->calibrationNeeded = true;
    if (!params->blocked)
      startMotor(consts);
    return 0; // starting the time will be when calibration is finished
}

static void startMotor(const tsStepperMotorConsts *consts)
{
    threadStart(consts->process, stepperMotorThrdHandler);
}

static void stopMotor(tsStepperMotorParams *params, const tsStepperMotorConsts *consts)
{
    // deenergize all poles
    for (size_t i = 0; i < POLES_COUNT; ++i)
        devIoPut(consts->poleDevices[i], 0);

    // re-triggering this function shouldn't re-block the motor if it's already stopped
    if (params->currentDirection != eStop)
    {
        params->currentDirection = eStop;
        params->blocked = true;
        timerCallBackStart(&params->unblockingTimer, BLOCKING_TIME_MS);
    }
    
    FirstStepFlag = 1;
}

/// @brief apply the proper signals on the motor poles according to the driving method and the current step
static void applyCurrentStep(tsStepperMotorParams *params, const tsStepperMotorConsts *consts)
{
    const tsDrivingMethod *dm = &drivingMethods[params->configs->drivingMethod];
    for (size_t i = 0; i < POLES_COUNT; ++i)
    {
        uint8_t state = (dm->polesState[i] & BIT(params->currentStep)) != 0;
        devIoPut(consts->poleDevices[i], state);
    }
}

/// @brief check if the motor has to rotate or has no thing to do
static bool anyWorkToDo(tsStepperMotorParams *params)
{
	/*return !params->blocked &&
	   (params->calibrationNeeded ||  StepPositions[params->requiredPosition] != params->currentPosition);*/
  /*** APW : For separate step moter between valve and damper ***/
  uint32_t targetPosition;
 
      for (uint8_t i = 0; i < 2; i++)
      if(refrigerator_F.Cooler.type != _CoolerType_DEFAULT && 
          refrigerator_F.Cooler.components.solenoidValve[i].type == _SolenoidValveType_StepMotor)
            targetPosition = StepPositions[params->requiredPosition];
        
      for (uint8_t i = 0; i < _NumOfCabin; i++)        
        if(refrigerator_F.Cabin[i].type == _CabinType_FLAP)
            targetPosition = params->requiredPosition;

  return !params->blocked &&
           (params->calibrationNeeded ||  targetPosition != params->currentPosition);

  /*** APW : For separate step moter between valve and damper ***/
}

/**************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 */

/** @} */

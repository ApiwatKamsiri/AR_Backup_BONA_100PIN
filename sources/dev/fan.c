/** @file       fan.c
 *  @brief      C file of FAN device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-04
 */
#define FILE_FAN_C

#include "fan.h"
#include "..\app\refrigerator\rf_ram.h"

/**
 *  @addtogroup FAN
 *  @{
 */

#define PERIODIC_EVENT_TIME_MS 20
#define SPEED_CALCULATION_ACTIVATION_DELAY_MS 2000
#define FAILURE_LOCK_TIME_MS 5000
#define FAILURE_HIDE_ERROR_TIME_MS 1000*60*10 // 10 Dk.
#define FAILURE_CHECK_ERROR_TIME_MS 5000 // 5 ms
#define FG_PULSES_COUNTING_TIMER_PERIOD_MS 1
#define MAX_ALLOWED_TIME_WITH_NO_PULSES_MS 250
#define FG_CAPTURING_WINDOW_TIME_MS 1000

static void switchFanOnOff(tsFanParams *params, const tsFanConsts *consts, uint32_t fanState);
static void setRequiredPwm(tsFanParams *params, const tsFanConsts *consts, uint32_t pwmValue);
static void setCurrentPwm(tsFanParams *params, const tsFanConsts *consts, uint8_t pwmValue);
static void startPwmFan(tsFanParams *params, const tsFanConsts *consts);
static void stopPwmFan(tsFanParams *params, const tsFanConsts *consts);
static void checkFanError(tsFanParams *params, const tsFanConsts *consts);

TIMER_CALLBACK_FUNC(fgPulsesCountingTimerHandler);
TIMER_CALLBACK_FUNC(fanUnlockTimerHandler);
TIMER_CALLBACK_FUNC(fanErrorHideTimerHandler);
TIMER_CALLBACK_FUNC(fanErrorCheckTimerHandler);

/* Process Functions *******************************************/
static PROCESS_HANDLER_PROTO(fanStartupStateHandler);
static PROCESS_HANDLER_PROTO(fanRunningStateHandler);

enum FanProcessEvents
{
    ePeriodicEvent = 2,
    eStartCalculatingSpeedEvent,
};

PROCESS_INIT_PROTO(fanProcessInit)
{
    tsFanParams *params = process->parameters;

    // Assign timers' initial values internally
    params->periodicTimer.post.source = process->enumeration;
    params->periodicTimer.post.destination = process->enumeration;
    params->periodicTimer.post.event = ePeriodicEvent;

    params->activateSpeedCalculationTimer.post.source = process->enumeration;
    params->activateSpeedCalculationTimer.post.destination = process->enumeration;
    params->activateSpeedCalculationTimer.post.event = eStartCalculatingSpeedEvent;

    params->fgPulsesCountingTimer.callBack = fgPulsesCountingTimerHandler;

    params->fanUnlockTimer.callBack = fanUnlockTimerHandler;
    params->fanErrorHideTimer.callBack = fanErrorHideTimerHandler;
    params->fanErrorCheckTimer.callBack = fanErrorCheckTimerHandler;
                  
    timerCallBackStart(&params->fanErrorCheckTimer, FAILURE_CHECK_ERROR_TIME_MS);

    process->initialized = 1;
}

PROCESS_DEINIT_PROTO(fanProcessDeinit)
{
    process->initialized = 0;
}

/// @brief Fan process handler at startup state
static PROCESS_HANDLER_PROTO(fanStartupStateHandler)
{
    tsFanParams *params = process->parameters;
    const tsFanConsts *consts = process->constants;
    uint8_t newPwm = params->currentPwm;

    // only ePeriodicEvent assumed to be occurred in this state -> no need to check the current event

    if (params->currentPwm < 70)
    {
        // give the motor a ramp at the startup
        newPwm += 5;
        setCurrentPwm(params, consts, newPwm);
    }
    else
    {
        params->speedCalculationActivated = false;
        if (params->configs->fgPinUsed)
            timerEventStart(&params->activateSpeedCalculationTimer, SPEED_CALCULATION_ACTIVATION_DELAY_MS);
        PROCESS_STATE_CHANGE(process, fanRunningStateHandler);
    }

    timerEventStart(&params->periodicTimer, PERIODIC_EVENT_TIME_MS);
}

/// @brief Fan process handler at running state (after startup)
static PROCESS_HANDLER_PROTO(fanRunningStateHandler)
{
    tsFanParams *params = process->parameters;
    const tsFanConsts *consts = process->constants;
    uint8_t newPwm = params->currentPwm;

    switch (eventCurrent.event)
    {
    case ePeriodicEvent:
        if (params->speedCalculationActivated == false)
        {
            if (params->requiredPwm > params->currentPwm)
                newPwm++;
            else if (params->requiredPwm < params->currentPwm)
                newPwm--;
        }
        else /*params->speedCalculationActivated == true*/
        {
            if (params->passedMsWithNoPulses > MAX_ALLOWED_TIME_WITH_NO_PULSES_MS)
            {
                // no pulses are detected in 2250 ms (2000ms+250ms) -> the fan is not turning
                if(params->failureStatus == false)
                  timerCallBackStart(&params->fanErrorHideTimer, FAILURE_HIDE_ERROR_TIME_MS);
                  
                params->failureStatus = true;
                checkFanError(params, consts);
                params->fanLocked = true;
                stopPwmFan(params, consts);
                timerCallBackStart(&params->fanUnlockTimer, FAILURE_LOCK_TIME_MS);
                break;
            }

            if (params->windowOfPulsesReady)
            {
                /*
                when speed calculation is activated, required pwm value is used as the required speed ratio.
                for example if the full speed of a fan = 2000 rpm and the required pwm value = 50%,
                then the required speed = 1000 rpm
                */

                /* currentSpeedRpm = pulses count in one minute / pulses per revolution */

                params->currentSpeedRpm = 60UL * (1000UL / FG_CAPTURING_WINDOW_TIME_MS) *
                                          params->fgPulsesCountInWindow / params->configs->ppr;
                
                uint32_t requiredSpeedRpm = params->configs->rpm * params->requiredPwm / 100;
                
                if (params->currentSpeedRpm < requiredSpeedRpm && params->currentPwm < 100)
                    newPwm++;
                else if (params->currentSpeedRpm > requiredSpeedRpm && params->currentPwm > 0)
                    newPwm--;

                params->windowOfPulsesReady = false;
            }
        }
        // [96 - 99] is pwm prohibited region
        // remember that required pwm can be {0,[5-95], 100} as we filtered it in put function
        if (newPwm > 95)
            newPwm = params->requiredPwm == 100 ? 100 : 95;

        setCurrentPwm(params, consts, newPwm);
        timerEventStart(&params->periodicTimer, PERIODIC_EVENT_TIME_MS);
        break;

    case eStartCalculatingSpeedEvent:
        params->speedCalculationActivated = true;
        params->windowMsCounter = 0;
        params->windowOfPulsesReady = false;
        params->passedMsWithNoPulses = 0;
        params->fgPulsesCounter = 0;
        timerCallBackStart(&params->fgPulsesCountingTimer, FG_PULSES_COUNTING_TIMER_PERIOD_MS);
        break;
    }
}

/// @brief Function called periodically to capture FG pulses and prepare a window of FG pulses
TIMER_CALLBACK_FUNC(fgPulsesCountingTimerHandler)
{
    tsFanParams *params = ((tsDevIo *)parameter)->parameters;
    const tsFanConsts *consts = ((tsDevIo *)parameter)->constants;

    uint8_t currentFgLevel = devIoGet(consts->fgDev);
    // detect new pulses (at its rising edges)
    if (currentFgLevel == 1 && params->lastFgLevel == 0)
    {
        params->fgPulsesCounter++;
        params->passedMsWithNoPulses = 0;
        params->failureStatus = false;
        params->failureShow = false;
        params->failureDetected = false;
        timerCallBackStop(&params->fanErrorHideTimer);
    }
    else if (params->passedMsWithNoPulses < 0xffff)
        params->passedMsWithNoPulses++;

    params->lastFgLevel = currentFgLevel;

    if (++params->windowMsCounter >= FG_CAPTURING_WINDOW_TIME_MS)
    {
        params->windowMsCounter = 0;
        params->fgPulsesCountInWindow = params->fgPulsesCounter;
        params->fgPulsesCounter = 0;
        params->windowOfPulsesReady = true;
    }
    return FG_PULSES_COUNTING_TIMER_PERIOD_MS;
}

/// @brief Unlocks a previously locked fan
TIMER_CALLBACK_FUNC(fanUnlockTimerHandler)
{
    tsFanParams *params = ((tsDevIo *)parameter)->parameters;
    const tsFanConsts *consts = ((tsDevIo *)parameter)->constants;

    params->fanLocked = false;
    if (params->requiredPwm > 0)
        startPwmFan(params, consts);
    return 0;
}

/// @brief Unlocks a previously locked fan
TIMER_CALLBACK_FUNC(fanErrorHideTimerHandler)
{
    tsFanParams *params = ((tsDevIo *)parameter)->parameters;
    const tsFanConsts *consts = ((tsDevIo *)parameter)->constants;
    params->failureShow = true;
    checkFanError(params, consts);
    return 0;
}

/// @brief Error Check
TIMER_CALLBACK_FUNC(fanErrorCheckTimerHandler)
{
    tsFanParams *params = ((tsDevIo *)parameter)->parameters;
    const tsFanConsts *consts = ((tsDevIo *)parameter)->constants;
    checkFanError(params, consts);    
    // 5 Sn'de bir Timer tekrar cagirilir
    return FAILURE_CHECK_ERROR_TIME_MS;
}
/**************************************************************
 *
 *
 *
 *
 *
 *
 *
 */

/* Fan DevIo Functions *****************************************/
static DEV_IO_FUNC_INIT(fanInit);
static DEV_IO_FUNC_DEINIT(fanDeinit);
static DEV_IO_FUNC_GET(fanGet);
static DEV_IO_FUNC_PUT(fanPut);

/// @brief  Structure that defines the functions for fan
const tsDevIoFuncs devIoFanFuncs =
    {
        fanInit,
        fanDeinit,
        fanGet,
        fanPut,
};

/// @brief  Init function for a fan
static DEV_IO_FUNC_INIT(fanInit)
{
    tsFanParams *params = device->parameters;
    const tsFanConsts *consts = device->constants;
    if (device->sys->initialized == 0 && config != NULL)
    {
        device->sys->initialized = 1;
        params->configs = config;
        params->failureDetected = false;
        params->failureStatus = false;
        params->failureShow = false;
        params->fanLocked = false;
        if (params->configs->pwmUsed)
        {
            processStart(consts->process);
            // process state is only valid when the fan is not stopped
            PROCESS_STATE_CHANGE(consts->process, NULL);
            devIoInit(consts->pwmDev, NULL);
            if (params->configs->fgPinUsed)
                devIoInit(consts->fgDev, NULL);
        }
        else
            devIoInit(consts->switchingDev, NULL);
        if (params->configs->powerPinUsed)
            devIoInit(consts->powerDev, NULL);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/// @brief  Deinit function for a fan
static DEV_IO_FUNC_DEINIT(fanDeinit)
{
    tsFanParams *params = device->parameters;
    const tsFanConsts *consts = device->constants;
    if (device->sys->initialized)
    {
        device->sys->initialized = 0;
        if (params->configs->pwmUsed)
        {
            processStop(consts->process);
            devIoDeinit(consts->pwmDev);
            if (params->configs->fgPinUsed)
                devIoDeinit(consts->fgDev);
        }
        else
            devIoDeinit(consts->switchingDev);
        if (params->configs->powerPinUsed)
            devIoDeinit(consts->powerDev);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/// @brief  Returns the fan actual speed
static DEV_IO_FUNC_GET(fanGet)
{
    tsFanParams *params = device->parameters;
    return params->currentSpeedRpm;
}

/// @brief  Sets the fan operating value (PWM duty, Speed ratio or DC signal level)
static DEV_IO_FUNC_PUT(fanPut)
{
    tsFanParams *params = device->parameters;
    const tsFanConsts *consts = device->constants;
    UNUSED(consts); // REMOVE IF USED

    if (params->configs->pwmUsed == false)
    {
        switchFanOnOff(params, consts, data);
        return EXIT_SUCCESS;
    }
    else
    {
        if (data <= 100)
        {
            setRequiredPwm(params, consts, data);
            return EXIT_SUCCESS;
        }
        return EXIT_FAILURE;
    }
}

/// @brief Switches on/off a fan that is controlled by a constant DC signal
static void switchFanOnOff(tsFanParams *params, const tsFanConsts *consts, uint32_t fanState)
{
    if (params->configs->powerPinUsed)
        devIoPut(consts->powerDev, fanState != 0);

    devIoPut(consts->switchingDev, fanState != 0);
}

/// @brief Sets the required PWM value / speed ratio of a fan that is controlled with a PWM signal
static void setRequiredPwm(tsFanParams *params, const tsFanConsts *consts, uint32_t pwmValue)
{
    if (pwmValue < 5)
        pwmValue = 0;
    else if (pwmValue > 95)
        pwmValue = 100;

    if (pwmValue != params->requiredPwm)
    {
        params->requiredPwm = pwmValue;
        if (params->requiredPwm == 0)
            stopPwmFan(params, consts);
        else if (params->currentPwm == 0 && !params->fanLocked)
            startPwmFan(params, consts);
    }
}

/// @brief Applies the given value on the outputs of a PWM controlled fan
static void setCurrentPwm(tsFanParams *params, const tsFanConsts *consts, uint8_t pwmValue)
{
    if (pwmValue != params->currentPwm)
    {
        params->currentPwm = pwmValue;
        if (params->configs->powerPinUsed)
            devIoPut(consts->powerDev, pwmValue > 0);
        devIoPut(consts->pwmDev, pwmValue);
    }
}

/// @brief Starts PWM fan controlling operation
static void startPwmFan(tsFanParams *params, const tsFanConsts *consts)
{
    PROCESS_STATE_CHANGE(consts->process, fanStartupStateHandler);
    timerEventStart(&params->periodicTimer, PERIODIC_EVENT_TIME_MS);
}

/// @brief Stops PWM fan controlling operation
static void stopPwmFan(tsFanParams *params, const tsFanConsts *consts)
{
    // if the timer is not started the next line has no effect
    timerCallBackStop(&params->fgPulsesCountingTimer);
    timerEventStop(&params->periodicTimer);
    timerEventStop(&params->activateSpeedCalculationTimer);
    PROCESS_STATE_CHANGE(consts->process, NULL);
    params->currentSpeedRpm = 0;
    setCurrentPwm(params, consts, 0);
}

static void checkFanError(tsFanParams *params, const tsFanConsts *consts)
{
    if((refrigerator_R.workMode.values.workMode == _WM_Service) ||
       (refrigerator_R.workMode.values.prodTest.minTimer < 1))
      params->failureDetected = params->failureStatus;
    else
    {
      if(params->requiredPwm)
        params->failureDetected = params->failureShow;
      else
      {
        /* Fan Hiz Bilgisi yoksa  o s?re boyunca hata verilmesin.
         * Fan tekrar calistiginda hata devam ederse hata 10 dk sonra g?sterilir.
         * Servise modunda hata surekli verilir. */
        params->failureShow = false;
        params->failureDetected = false;        
        params->failureStatus = false;
        timerCallBackStop(&params->fanErrorHideTimer);
      }
    }        
}

/**************************************************************
 *
 *
 *
 *
 *
 *
 *
 */

/* Fan Error Detector DevIo Functions *********************************/

static DEV_IO_FUNC_GET(fanErrorDetectorGet);

/// @brief  Structure that defines the functions for fan error detector
const tsDevIoFuncs devIoFanErrorDetectorFuncs = {NULL, NULL, fanErrorDetectorGet, NULL};

/// @brief Returns the error status of a fan device which the error detector is associated to it. 
static DEV_IO_FUNC_GET(fanErrorDetectorGet)
{
    const tsFanErrorDetectorConsts *consts = device->constants;
    return ((tsFanParams *)consts->fanDevice->parameters)->failureDetected;
}

/**************************************************************
 *
 *
 *
 *
 *
 *
 *
 */

/** @} */

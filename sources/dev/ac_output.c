/** @file       ac_output.c
 *  @brief      C file of AC_OUTPUT device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-01
 */
#define FILE_AC_OUTPUT_C

#include "ac_output.h"

/**
 *  @addtogroup AC_OUTPUT
 *  @{
 */

/* Cpx Device Functions ****************************************/

static DEV_IO_FUNC_INIT(ACOutputsControllerInit);
static DEV_IO_FUNC_DEINIT(ACOutputsControllerDeinit);
static DEV_IO_FUNC_GET(ACOutputsControllerGet);
static DEV_IO_FUNC_PUT(ACOutputsControllerPut);

const tsDevIoFuncs devIoACOutputsControllerFuncs = {
    ACOutputsControllerInit,
    ACOutputsControllerDeinit,
    ACOutputsControllerGet,
    ACOutputsControllerPut,
};

/// @brief  AC output controller init function
static DEV_IO_FUNC_INIT(ACOutputsControllerInit)
{
    ADD_RCOS_VERSION("ac_output", "2.0.0")
    UNUSED(config); // REMOVE IF USED

    const tsACOutputsControllerConsts *consts = device->constants;
    processStart(consts->process);
    device->sys->initialized = 1;
    return EXIT_SUCCESS;
}

/// @brief  AC output controller deinit function
static DEV_IO_FUNC_DEINIT(ACOutputsControllerDeinit)
{
    const tsACOutputsControllerConsts *consts = device->constants;
    processStop(consts->process);
    device->sys->initialized = 0;
    return EXIT_SUCCESS;
}

/// @brief  Returns true if blocking feature is enabled, else false
static DEV_IO_FUNC_GET(ACOutputsControllerGet)
{
    tsACOutputsControllerParams *params = device->parameters;
    return params->blockingFuncEnabled;
}

/// @brief  Sets the state of the blocking feature
static DEV_IO_FUNC_PUT(ACOutputsControllerPut)
{
    tsACOutputsControllerParams *params = device->parameters;
    params->blockingFuncEnabled = data != 0;
    if (params->blockingFuncEnabled == false)
    {
        params->blocked = false;
        timerEventStop(&params->unblockTimer);
    }
    return EXIT_SUCCESS;
}

/***************************************************************/

/* Process Functions *******************************************/
enum ACControllerEvents
{
    eEventPeriodicChecks = 1,
    eEventUnblockController = 2,
};

static PROCESS_HANDLER_PROTO(ACOutputControllerHandler);
static void ScanACOutputs(tsProcess *process);

PROCESS_INIT_PROTO(ACOutputsControllerProcessInit)
{
    tsACOutputsControllerParams *params = process->parameters;
    const tsACOutputsControllerConsts *consts = process->constants;

    // As the process's enum of the controller device is unknown at compile time (PROCESS_NONE)
    // we need to set the source and the destination processes of the event timers manually
    params->unblockTimer.post.source = process->enumeration;
    params->unblockTimer.post.destination = process->enumeration;
    params->unblockTimer.post.event = eEventUnblockController;

    params->periodicTimer.post.source = process->enumeration;
    params->periodicTimer.post.destination = process->enumeration;
    params->periodicTimer.post.event = eEventPeriodicChecks;
    timerEventStart(&params->periodicTimer, consts->scanningTimeMs);

    PROCESS_STATE_CHANGE(process, ACOutputControllerHandler);
}

PROCESS_DEINIT_PROTO(ACOutputsControllerProcessDeinit)
{
    tsACOutputsControllerParams *params = process->parameters;

    PROCESS_STATE_CHANGE(process, NULL);
    timerEventStop(&params->unblockTimer);
    timerEventStop(&params->periodicTimer);
}

PROCESS_HANDLER_PROTO(ACOutputControllerHandler)
{
    tsACOutputsControllerParams *params = process->parameters;
    const tsACOutputsControllerConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
    case eEventUnblockController:
        params->blocked = false;
        ScanACOutputs(process);
        break;

    case eEventPeriodicChecks:
        ScanACOutputs(process);
        timerEventStart(&params->periodicTimer, consts->scanningTimeMs);
        break;
    }
}

/// @brief  This function is called periodically to update the controlled AC outputs states
static void ScanACOutputs(tsProcess *process)
{
    tsACOutputsControllerParams *controllerParams = process->parameters;
    const tsACOutputsControllerConsts *controllerConsts = process->constants;

    for (size_t i = 0; i < controllerConsts->devPtrArraySize; ++i)
    {
        if (controllerConsts->devPtrArray[i]->sys->initialized) // ignore uninitialized outputs
        {
            tsAcOutputParams *devParams = controllerConsts->devPtrArray[i]->parameters;
            const tsAcOutputConsts *devConsts = controllerConsts->devPtrArray[i]->constants;

            if (devParams->requestedState != devParams->currentState)
            {
                if (controllerParams->blocked == false || devParams->participateInBlocking == false)
                {
                    devParams->currentState = devParams->requestedState;
                    devIoPut(devConsts->gpioDevice, devParams->currentState);
                    if (controllerParams->blockingFuncEnabled && devParams->blockingTimeMs > 0)
                    {
                        controllerParams->blocked = true;
                        timerEventStart(&controllerParams->unblockTimer, devParams->blockingTimeMs);
                    }
                }
            }
        }
    }
}

/***************************************************************/

/* IO Device Functions ****************************************/

static DEV_IO_FUNC_INIT(acOutputInit);
static DEV_IO_FUNC_DEINIT(acOutputDeinit);
static DEV_IO_FUNC_GET(acOutputGet);
static DEV_IO_FUNC_PUT(acOutputPut);

/// @brief  Structure that defines the functions for acOutput
const tsDevIoFuncs devIoAcOutputFuncs =
    {
        acOutputInit,
        acOutputDeinit,
        acOutputGet,
        acOutputPut,
};

/** @brief  Init function for acOutput
 * @param config ptr to struct the holds the necessary device configurations, pass NULL to use default values (participateInBlocking = false, blocking time 0)
 * @note this function initializes the gpioDevice also.
 */
static DEV_IO_FUNC_INIT(acOutputInit)
{
    tsAcOutputParams *params = device->parameters;
    const tsAcOutputConsts *consts = device->constants;
    const tsAcOutputConfig *cfg = config;

    if (cfg != NULL)
    {
        params->participateInBlocking = cfg->participateInBlocking;
        params->blockingTimeMs = cfg->blockingTimeMs;
    }
    else
    {
        params->participateInBlocking = false;
        params->blockingTimeMs = 0;
    }

    devIoInit(consts->gpioDevice, NULL);
    device->sys->initialized = 1;
    return EXIT_SUCCESS;
}

/** @brief  Deinit function for acOutput
 * @note this function de-initializes the gpioDevice also.
 */
static DEV_IO_FUNC_DEINIT(acOutputDeinit)
{
    tsAcOutputParams *params = device->parameters;
    const tsAcOutputConsts *consts = device->constants;
    UNUSED(params); // REMOVE IF USED

    device->sys->initialized = 0;
    devIoDeinit(consts->gpioDevice);
    return EXIT_SUCCESS;
}

/// @brief  Returns the current state of the AC output
static DEV_IO_FUNC_GET(acOutputGet)
{
    tsAcOutputParams *params = device->parameters;
    return params->currentState;
}

/// @brief  Sets the required state of the AC output. The real state is set by the controller
static DEV_IO_FUNC_PUT(acOutputPut)
{
    tsAcOutputParams *params = device->parameters;
    
    params->requestedState = data ? 1 : 0;
    

    
    
    return EXIT_SUCCESS;
}

/** @} */

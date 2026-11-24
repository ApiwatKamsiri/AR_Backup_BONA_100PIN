/** @file       knob.c
 *  @brief      C file of KNOB device
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Arif Salih OZMEN
 *  @date       7.06.2023
 */
#define FILE_KNOB_C

#include "knob.h"

/**
 *  @addtogroup KNOB
 *  @{
 */

static DEV_IO_FUNC_INIT(KNOBInit);
static DEV_IO_FUNC_DEINIT(KNOBDeinit);
static DEV_IO_FUNC_GET(KNOBGet);

/// @brief  Structure that defines the functions for KNOB
const tsDevIoFuncs devIoKNOBFuncs =
{
    KNOBInit,
    KNOBDeinit,
    KNOBGet,
    NULL,
};

const uint8_t knob6PositionBoundary[] = { 17, 59, 95, 138, 230};
#define KNOB_6POSITION_BOUNDARY_SIZE (sizeof(knob6PositionBoundary) / sizeof(knob6PositionBoundary[0]))

#define _KNOB_6POSITION_OPEN_CIRCUIT 1
#define _Knob_6POSITION_SHORT_CIRCUIT 1023

static uint8_t knobValueInPosition(uint8_t position, uint16_t adcValue)
{
    bool updatePosition = false;

    if (position >= KNOB_6POSITION_BOUNDARY_SIZE)
    { // Update if adcValue is outside Hysterisis
        position = KNOB_6POSITION_BOUNDARY_SIZE;
        if (adcValue < (knob6PositionBoundary[KNOB_6POSITION_BOUNDARY_SIZE - 1] - 1))
            updatePosition = true;
    }
    else if (position == 0)
    {
        if (adcValue > (knob6PositionBoundary[0] + 1))
            updatePosition = true;
    }
    else
    {
        if ((adcValue < (knob6PositionBoundary[position - 1] - 1)) ||
            (adcValue > (knob6PositionBoundary[position] + 1)))
            updatePosition = true;
    }

    if (updatePosition)
    {
        uint8_t i;
        for (i = 0; i < KNOB_6POSITION_BOUNDARY_SIZE; i++)
        {
            if (adcValue < knob6PositionBoundary[i])
            {
                break;
            }
        }
        return i;
    }
    else
    {
        return position;
    }
}
/// @brief  Init function for a KNOB
static DEV_IO_FUNC_INIT(KNOBInit)
{
    tsKNOBParams *params       = device->parameters;
    const tsKNOBConsts *consts = device->constants;

    UNUSED(config);    // REMOVE IF USED
    UNUSED(params);

    if (device->sys->initialized == 0)
    {
        device->sys->initialized = 1;
        devIoInit(consts->adcDevice, NULL);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/// @brief  Deinit function for a KNOB
static DEV_IO_FUNC_DEINIT(KNOBDeinit)
{
    tsKNOBParams *params       = device->parameters;
    const tsKNOBConsts *consts = device->constants;
    UNUSED(params);
    
    if (device->sys->initialized)
    {
        device->sys->initialized = 0;
        devIoDeinit(consts->adcDevice);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/// @brief  Get function for a KNOB
static DEV_IO_FUNC_GET(KNOBGet)
{
    tsKNOBParams *params       = device->parameters;
    const tsKNOBConsts *consts = device->constants;

    uint32_t adcValue = devIoGet(consts->adcDevice);
    uint8_t newValueInPosition = knobValueInPosition(params->valueInPosition, (adcValue >> 2));

    if(adcValue < _KNOB_6POSITION_OPEN_CIRCUIT)
    {
        params->error = eKNOBErrorOpenCircuit;
        //return EXIT_FAILURE;
    }
    else if(adcValue > _Knob_6POSITION_SHORT_CIRCUIT)
    {
        params->error = eKNOBErrorShortCircuit;
        //return EXIT_FAILURE;
    }
    else
    {
        params->error = eKNOBErrorNone;
    }

    if((KNOB_6POSITION_BOUNDARY_SIZE - newValueInPosition) != params->valueInPosition)
    {
        params->positionChanged = true;
        //return EXIT_SUCCESS;
    }

    params->valueInPosition = KNOB_6POSITION_BOUNDARY_SIZE - newValueInPosition;

    return params->valueInPosition;
}

static DEV_IO_FUNC_GET(knobErrorDetectorGet);

const tsDevIoFuncs devIoKNOBErrorDetecorFuncs =
    {
        NULL,
        NULL,
        knobErrorDetectorGet,
        NULL,
};

/// @brief Gives the error status of a KNOB device which the error detector is associated to it.
static DEV_IO_FUNC_GET(knobErrorDetectorGet)
{

    tsKNOBParams *knobParams = ((tsKnobErrorDetectorConsts *)device->constants)->knobDevice->parameters;
    return knobParams->error;
}

/** @} */

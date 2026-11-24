/** @file       feature.c
 *  @brief      FEATURE application program file
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Enes Inanc
 *  @date       6.07.2023
 */
#define FILE_FEATURE_C

#include "feature.h"

#define DEBUG_FILE_NAME ""

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup FEATURE
 *  @{
 */

static PT_THREAD(featureThread)
{
    tsFeatureParams *params       = process->parameters;
    const tsFeatureConsts *consts = process->constants;
    UNUSED(params);
    UNUSED(consts);
    PT_BEGIN();

    for (;;)
    {
        // PT_WHILE();
        PT_YIELD();
    }

    PT_END();
}

static PROCESS_HANDLER_PROTO(featureHandler);

/// @brief  Initialization function of feature
PROCESS_INIT_PROTO(featureInit)
{
    tsFeatureParams *params       = process->parameters;
    const tsFeatureConsts *consts = process->constants;
	ADD_RCOS_VERSION("feature", "1.0.0")
	
    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, featureHandler);
    // threadStart(process, featureThread);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of feature
PROCESS_DEINIT_PROTO(featureDeinit)
{
    tsFeatureParams *params       = process->parameters;
    const tsFeatureConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    threadStop(process, process->threadFunction);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of feature
static PROCESS_HANDLER_PROTO(featureHandler)
{
    tsFeatureParams *params       = process->parameters;
    const tsFeatureConsts *consts = process->constants;

    switch (eventCurrent.event)
    {
        case eFeatureEventsListFeatures:
        {
            // Send all feature elements
            uint8_t index       = 0;
            uint8_t bufferIndex = 0;
            uint8_t featureSize = sizeof(tsFeatureElement);
            uint8_t result      = EXIT_FAILURE;
            tsFeatureElement *feature;

            params->featureDataBuffer[bufferIndex++] = consts->featureCount; ///< number of features

            for (index = 0; index < consts->featureCount; index++)
            {
                feature = (tsFeatureElement *)&consts->featureArray[index];

                if (bufferIndex >= params->featureBufferSize)
                    break;

                memcpy(&params->featureDataBuffer[bufferIndex], feature, featureSize); ///< feature element

                bufferIndex += featureSize;
            }

            ///< send packet to gdm process
            result = eventPost(consts->gdmProcessEnum, eFeatureEventsFeatureListed, params->featureDataBuffer, bufferIndex);
            if (result == EXIT_FAILURE)
            {
                ///< resend packets - if it there is failure while adding event
                timerEventStart(&(params->timerResendPacket), 500);
            }
        }
        break;

        default:
            break;
    }
}

/** @} */

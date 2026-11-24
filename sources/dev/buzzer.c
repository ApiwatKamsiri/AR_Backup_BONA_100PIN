/** @file       buzzer.c
 *  @brief      C file of BUZZER device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-10
 */
#define FILE_BUZZER_C

#include "buzzer.h"

/**
 *  @addtogroup BUZZER
 *  @{
 */

// Old notes are the written notes divided by ~ 3.15

// structs that hold the notes and their periods of each melody
static const tsPatternNode mOpen[] = {
    PATTERN_NODE(NOTE_5_C, 150),
    PATTERN_NODE(NOTE_5_G, 150),
    PATTERN_NODE(NOTE_5_E, 150),
    PATTERN_NODE(NOTE_6_C, 150),
    PATTERN_NODE(NOTE_6_C, 300),    //echo
    PATTERN_NODE_END(0),
};

static const tsPatternNode mClose[] = {
    PATTERN_NODE(NOTE_6_C, 150),
    PATTERN_NODE(NOTE_5_E, 150),
    PATTERN_NODE(NOTE_5_G, 150),
    PATTERN_NODE(NOTE_5_C, 150),
    PATTERN_NODE(NOTE_5_C, 300),    //echo
    PATTERN_NODE_END(0),
};

static const tsPatternNode mStart[] = {
    PATTERN_NODE(NOTE_5_G, 125),
    PATTERN_NODE(NOTE_5_E, 125),
    PATTERN_NODE(NOTE_6_C, 125),
    PATTERN_NODE(NOTE_6_C, 300),    //echo
    PATTERN_NODE_END(0),
};

static const tsPatternNode mEnd[] = {
    PATTERN_NODE(NOTE_5_G, 125),
    PATTERN_NODE(NOTE_6_C, 125),
    PATTERN_NODE(NOTE_5_C, 125),
    PATTERN_NODE(NOTE_5_C, 300),    //echo
    PATTERN_NODE_END(0),
};

static const tsPatternNode mButton[] = {
    PATTERN_NODE(AHTH_Warning, 32),
    PATTERN_NODE(AHTH_Warning, 300),    //echo
    PATTERN_NODE_END(0),
};

static const tsPatternNode mWarning[] = {
    PATTERN_NODE(AHTH_Warning, 500), 
    PATTERN_NODE(AHTH_Warning, 300), // the second half of this melody will be applied while high pin is low, this will give an echo
    PATTERN_NODE_END(0),
};

/*** APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1) ***/
static const tsPatternNode mDoorAlarmAHTH[] = {
      PATTERN_NODE(AHTH_Warning, 500),
      PATTERN_NODE(0, 500),
      PATTERN_NODE_END(0),
};
/*** APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1) ***/

struct sMelody
{
    const tsPatternNode *freqPattern;
    const uint32_t highDuration;
} ;

// table to map melodies notes with the required high pulse duration to apply
static const struct sMelody melodies[eBuzzerMelodiesCount] = {
    [eBuzzerMelodyNone] = {.freqPattern = NULL, .highDuration = 0},
    [eBuzzerMelodyOpen] = {.freqPattern = mOpen, .highDuration = 600},
    [eBuzzerMelodyClose] = {.freqPattern = mClose, .highDuration = 600},
    [eBuzzerMelodyStart] = {.freqPattern = mStart, .highDuration = 375},
    [eBuzzerMelodyEnd] = {.freqPattern = mEnd, .highDuration = 375},
    [eBuzzerMelodyButton] = {.freqPattern = mButton, .highDuration = 32},
    [eBuzzerMelodyWarning] = {.freqPattern = mWarning, .highDuration = 500},    // this will give an echo for the second half of the node
    [eBuzzerMelodyDoorsAlertAHTH] = {.freqPattern = mDoorAlarmAHTH, .highDuration = 500},     //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
};

static DEV_IO_FUNC_INIT(buzzerInit);
static DEV_IO_FUNC_DEINIT(buzzerDeinit);
static DEV_IO_FUNC_PUT(buzzerPut);

/// @brief  Structure that defines the functions for buzzer
const tsDevIoFuncs devIoBuzzerFuncs =
    {
        buzzerInit,
        buzzerDeinit,
        NULL,
        buzzerPut,
};

/// @brief  Init function for a buzzer
static DEV_IO_FUNC_INIT(buzzerInit)
{
    const tsBuzzerConsts *consts = device->constants;

    device->sys->initialized = 1;
    devIoInit(consts->pulseDevice, NULL);
    devIoInit(consts->patternDevice, NULL);
    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a buzzer
static DEV_IO_FUNC_DEINIT(buzzerDeinit)
{
    const tsBuzzerConsts *consts = device->constants;

    device->sys->initialized = 0;
    devIoDeinit(consts->pulseDevice);
    devIoDeinit(consts->patternDevice);
    return EXIT_SUCCESS;
}

/// @brief  Put function for a buzzer
static DEV_IO_FUNC_PUT(buzzerPut)
{
    const tsBuzzerConsts *consts = device->constants;

    if (data < eBuzzerMelodiesCount)
    {
        devIoPut(consts->pulseDevice, melodies[data].highDuration);
        devIoPut(consts->patternDevice, (uint32_t)melodies[data].freqPattern);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/** @} */

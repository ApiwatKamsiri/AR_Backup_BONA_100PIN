/** @file       display.c
 *  @brief      C file of DISPLAY device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-11
 */
#define FILE_DISPLAY_C

#include "display.h"
#include "libs/crc.h"

/**
 *  @addtogroup DISPLAY
 *  @{
 */

#define GetBit(var, bit_no) (((var) >> (bit_no)) & 1)
#define WriteBit(var, bit_no, bit_val) ((var) = (var & ~(1 << (bit_no))) | ((bit_val) << (bit_no)))
#define DELAY_BETWEEN_READ_AND_WRITE_MS 5
static PT_THREAD(displayThread);

static DEV_CPX_FUNC_INIT(displayInit);
static DEV_CPX_FUNC_DEINIT(displayDeinit);

/// @brief  Structure that defines the functions for display
const tsDevCpxFuncs devCpxDisplayFuncs =
    {
        displayInit,
        displayDeinit,
};

/// @brief  Init function for a display
static DEV_CPX_FUNC_INIT(displayInit)
{
    tsDisplayParams *params = device->parameters;
    const tsDisplayConsts *consts = device->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    UNUSED(config); // REMOVE IF USED
    device->sys->initialized = 1;
    devComInit(consts->i2c);
    dequeFlush(params->commandsQueue);
    dequeFlush(params->eventsQueue);
    processStart(consts->process);
    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a display
static DEV_CPX_FUNC_DEINIT(displayDeinit)
{
    tsDisplayParams *params = device->parameters;
    const tsDisplayConsts *consts = device->constants;

    device->sys->initialized = 0;
    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    processStop(consts->process);
    return EXIT_SUCCESS;
}

PROCESS_INIT_PROTO(displayProcInit)
{
    process->initialized = 1;
    threadStart(process, displayThread);
}

PROCESS_DEINIT_PROTO(displayProcDeinit)
{
    process->initialized = 0;
    threadStop(process, displayThread);
}

static PT_THREAD(displayThread)
{
    tsDisplayParams *params = process->parameters;
    const tsDisplayConsts *consts = process->constants;

    PT_BEGIN();
    for (;;)
    {
        // Fill the buffer with main board data
        memcpy(params->buffer, &params->data, 14);
        if (DEQUE_IS_EMPTY(params->commandsQueue))
            params->buffer[14] = eMainBoardCommandNone;
        else
            dequePopFront(params->commandsQueue, &params->buffer[14], 1);
        memset(&params->buffer[15], 0, 11);
        params->buffer[26] = crc8MaximArray(crc8MaximInit(), params->buffer, 26);

        // Send the buffer
        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->i2c, params->ticket), 5);
        PT_WAIT_WHILE(EXIT_FAILURE == devComOpen(consts->i2c, &params->target));
        devComSend(consts->i2c, params->buffer, 27);
        PT_WAIT_WHILE(devComSend(consts->i2c, NULL, 0));
        devComClose(consts->i2c, &params->target);
        PT_WAIT_WHILE(devComTicketValid(consts->i2c, params->ticket));

        PT_YIELD_TIMEOUT(DELAY_BETWEEN_READ_AND_WRITE_MS);
        
        // clear the buffer before you start receiving
        memset(&params->buffer, 0, sizeof(params->buffer));
        // Get display board data to the buffer
        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->i2c, params->ticket), 5);
        PT_WAIT_WHILE(EXIT_FAILURE == devComOpen(consts->i2c, &params->target));
        devComReceive(consts->i2c, params->buffer, 27);
        devComClose(consts->i2c, &params->target);
        PT_WAIT_WHILE(devComTicketValid(consts->i2c, params->ticket));

        // Extract received data from buffer
        if (params->buffer[26] != crc8MaximArray(crc8MaximInit(), params->buffer, 26))
        {
            if (params->errorCounter < UINT32_MAX)
                params->errorCounter++;
        }
        else
        {
            if (params->buffer[15] != eDisplayEventNone)
                dequePushBack(params->eventsQueue, &params->buffer[15], 1);
            memcpy(&params->data.displayInfo, &params->buffer[16], sizeof(tsDisplayInfo));
        }

        PT_YIELD_TIMEOUT(consts->period);
    }
    PT_END();
}

void devDspSetCabinData(const tsDevCpx *device, teDisplayCabins cabin, teDisplayCabinDataTypes dataType, int8_t data)
{
    tsDisplayParams *params = device->parameters;
    if (cabin < eDisplayCabinsCount && dataType < eDisplayCabinDataTypesCount)
    {
        WriteBit(params->data.cabinDataType, cabin, dataType);
        params->data.cabinData[cabin] = data;
    }
}

void devDspSendCommand(const tsDevCpx *device, teMainBoardCommand command)
{
    tsDisplayParams *params = device->parameters;
    // don't check if queue is full, the new commands should overwrite old ones
    if (command != eMainBoardCommandNone)
        dequePushBack(params->commandsQueue, &command, 1);
}

teDisplayEvent devDspGetEvent(const tsDevCpx *device)
{
    tsDisplayParams *params = device->parameters;

    if (!DEQUE_IS_EMPTY(params->eventsQueue))
    {
        uint8_t event;
        dequePopFront(params->eventsQueue, &event, 1);
        return (teDisplayEvent)event;
    }
    return eDisplayEventNone;
}

tsDisplayInfo devDspGetInfo(const tsDevCpx *device)
{
    tsDisplayParams *params = device->parameters;

    return params->data.displayInfo;
}

uint8_t devDspGetId(const tsDevCpx *device)
{
    tsDisplayParams *params = device->parameters;

    return params->data.displayInfo.id;
}

void devDspSetLedState(const tsDevCpx *device, teDisplayLeds led, teDisplayLedState state)
{
    tsDisplayParams *params = device->parameters;

    uint8_t byteIdx = led >> 3; // == led / 8;
    uint8_t bitIdx = led & 0x7; // == led % 8;
    if (led >= eDisplayLedCabin0Quick && led <= eDisplayLedCabin4Quick)
    {
        // second bit is 1 only if state is animation, first bit is 1 only if stat is on
        WriteBit(params->data.funcs.allFunctions[byteIdx], bitIdx, state == eDisplayLedStateOn);
        WriteBit(params->data.funcs.allFunctions[byteIdx], bitIdx + 1, state == eDisplayLedStateAnimation);
    }
    else if (state <= eDisplayLedStateOn) // all other leds can be on off only
        WriteBit(params->data.funcs.allFunctions[byteIdx], bitIdx, state);
}

teDisplayLedState devDspGetLedState(const tsDevCpx *device, teDisplayLeds led)
{
    tsDisplayParams *params = device->parameters;

    uint8_t byteIdx = led >> 3; // == led / 8;
    uint8_t bitIdx = led & 0x7; // == led % 8;
    uint8_t state = 0;
    if (led >= eDisplayLedCabin0Quick && led <= eDisplayLedCabin4Quick)
    {
        state = GetBit(params->data.funcs.allFunctions[byteIdx], bitIdx + 1);
        state = (state << 1) | GetBit(params->data.funcs.allFunctions[byteIdx], bitIdx);
    }
    else
        state = GetBit(params->data.funcs.allFunctions[byteIdx], bitIdx);
    return (teDisplayLedState)state;
}

void devDspSetDimLevel(const tsDevCpx *device, teDisplayDimLevels level)
{
    tsDisplayParams *params = device->parameters;

    if (level >= eDisplayDimLevelStart && level < eDisplayDimLevelsCount)
    {
        params->data.funcs.dimLevel = level;
    }
}

void devDspSetDisplayOff(const tsDevCpx *device)
{
    tsDisplayParams *params = device->parameters;

    params->data.funcs.displayOff = 1;
}

void devDspSyncPacket(const tsDevCpx *device, tsDisplayPacket *packet)
{
    tsDisplayParams *params = device->parameters;
    
    // synchronize outgoing display data
    memcpy(&params->data, packet, 14); // first 14 bytes, cabin data(6) + its type(1) + funcs(7)
    
    // synchronize incoming display data with giving packet
    memcpy(&packet->displayInfo, &params->data.displayInfo, sizeof(tsDisplayInfo));
}
/** @} */

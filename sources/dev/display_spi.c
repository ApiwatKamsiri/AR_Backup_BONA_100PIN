/** @file       display_uart.c
 *  @brief      C file of DISPLAY device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Maria Asif
 *  @date       2023-07-16
 */
#define FILE_DISPLAY_C

#include "display_spi.h"
#include "spi_slave.h"
#include "libs/crc.h"
#include "ui/command_events/ce_service.h"
#include "refrigerator/refrigerator.h"
#include "board/board.h"


extern volatile uint8_t spi_transfer_done,spi_transfer_abort,spi_fault_flag;

#define IS_MODE_QF(params) ((params->data.cabinData[0] >= 1) && (params->data.cabinData[0] <= 9))

#define GALAXY
/**
 *  @addtogroup DISPLAY
 *  @{
 */

#define GetBit(var, bit_no) (((var) >> (bit_no)) & 1)
#define WriteBit(var, bit_no, bit_val) ((var) = (var & ~(1 << (bit_no))) | ((bit_val) << (bit_no)))

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
    devComInit(consts->spi);
  
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
     tsSpiSlaveParams *ptr  = consts->spi->parameters;
    const tsSpiSlaveConsts *ptrConsts = consts->spi->constants;
    uint8_t WriteByte = 0;
    PT_BEGIN();

    for (;;)
    {
        if (ptr->completed)
        {
          memcpy((void*)ptr->txbuffer, &params->data, 14);
          if (DEQUE_IS_EMPTY(ptr->commandsQueue))
            ptr->txbuffer[14] = eMainBoardCommandNone;
          else
            dequePopFront(ptr->commandsQueue,(void*) &ptr->txbuffer[14], 1);
          //memset((void*)&ptr->txbuffer[15], 0, 11);
          memcpy((void*)(ptr->txbuffer+16),/*(void*)*/ (void*)((&params->data.dpEvent)+1), 10*sizeof(params->data.dpEvent));
          if (ptr->txbuffer[0]==0x08)
            ptr->txbuffer[4] = 0xAA;
          memcpy(&params->data.displayInfo,(void*) &ptr->rxbuffer[16], sizeof(tsDisplayInfo));
          memcpy(&params->data.displayInfo2,(void*) &ptr->rxbuffer[19], sizeof(tsDisplayInfo));
          //memcpy(&params->data.funcs2,(void*) &ptr->rxbuffer[22], sizeof(tuDisplayFunctions2));
          
          ptr->startErrorDisplayComTimes = false;       //Add monitor reset completed
          
        }
        if(ptr->timeOutComError > 80)
        {
            ptr->timeOutComError = 0;
            ptr->eSPIState = _SPIState_Start;
            /*** Add monitor reset completed ***/
            if(!ptr->startErrorDisplayComTimes)
            {
              ptr->errorDisplayComTimes++;
              ptr->startErrorDisplayComTimes = true;
            }
             /*** Add monitor reset completed ***/
            R_SPI_Write(ptrConsts->spi->p_ctrl, &WriteByte, 1, SPI_BIT_WIDTH_8_BITS);
            R_SPI_Close(ptrConsts->spi->p_ctrl);
            PT_YIELD_TIMEOUT(100);
            R_SPI_Open(ptrConsts->spi->p_ctrl, ptrConsts->spi->p_cfg);

        }
        else if( ptr->timeOut)
        {
            ptr->timeOut--;
        }
        else
        {
            ptr->timeOut = 2;
            ptr->eSPIState = _SPIState_Start;
            R_SPI_Write(ptrConsts->spi->p_ctrl, &WriteByte, 1, SPI_BIT_WIDTH_8_BITS);
        }
        
        if (spi_fault_flag)
        {
          spi_fault_flag = 0;
          R_SPI_Close(ptrConsts->spi->p_ctrl);
          PT_YIELD_TIMEOUT(2);
          R_SPI_Open(ptrConsts->spi->p_ctrl, ptrConsts->spi->p_cfg);
          ptr->eSPIState = _SPIState_Start;
        }
        
        if (spi_transfer_done)
        {
          spi_transfer_done = 0;
          // package received well
        }
        if (spi_transfer_abort)
        {
          spi_transfer_abort = 0;
          // package not received
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
//to use these commands, we must add device name in the consts part of the create macro. to be used.
void devDspSendCommand(const tsDevCpx *device, teMainBoardCommand command)
{
    tsDisplayParams *params = device->parameters;
     const tsDisplayConsts *consts = device->constants;
	UNUSED(params);
     tsSpiSlaveParams *ptr  = consts->spi->parameters;
    
    // don't check if queue is full, the new commands should overwrite old ones
    if (command != eMainBoardCommandNone)
        dequePushBack(ptr->commandsQueue, &command, 1);
}

teDisplayEvent devDspGetEvent(const tsDevCpx *device)
{
     tsDisplayParams *params = device->parameters;
     UNUSED(params);
     const tsDisplayConsts *consts = device->constants;
     tsSpiSlaveParams *ptr  = consts->spi->parameters;

    if (!DEQUE_IS_EMPTY(ptr->eventsQueue))
    {
        uint8_t event;
        dequePopFront(ptr->eventsQueue, &event, 1);
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
    globalIntDisable();
    // synchronize outgoing display data
    memcpy(&params->data, packet, 14); // first 14 bytes, cabin data(6) + its type(1) + funcs(7)
    
    // synchronize incoming display data with giving packet 
    memcpy(&packet->displayInfo, &params->data.displayInfo, sizeof(tsDisplayInfo));
    memcpy(&packet->displayInfo2, &params->data.displayInfo2, sizeof(tsDisplayInfo));
    memcpy(&params->data.funcs2, &packet->funcs2, sizeof(params->data.funcs2));
    globalIntEnable();
}
/** @} */

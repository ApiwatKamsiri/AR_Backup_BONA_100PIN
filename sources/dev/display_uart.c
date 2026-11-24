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

#include "display_uart.h"
#include "libs/crc.h"

#define SW1_BUTTON_ON 0x40
#define SW1_BUTTON_OFF 0x00
#define SW2_BUTTON_ON 0x80
#define SW2_BUTTON_OFF 0x00
#define BOTH_BUTTONS_ON 0xC0
#define BOTH_BUTTONS_OFF 0x00
#define LONGPRESS_THRESHOLD 0x28
#define IS_MODE_QF(params) ((params->data.cabinData[0] >= 1) && (params->data.cabinData[0] <= 4))

#define ATHENA
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
    devComInit(consts->uart);
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
tsTarget target;
uint8_t i=0;

bool buttonState_SW1 = false; // Initialize with off
bool prevButtonState_SW1 = false;
bool buttonState_SW2 = false; // Initialize with off
bool prevButtonState_SW2 = false;
uint8_t receiveddata[8];
volatile uint8_t BackupBuffer[DATA_BUTTONS];

void Initialize_Buffer()
{
    BackupBuffer[PREAMBLE]=0xAA;
    BackupBuffer[COMMAND_ID]=0x01;
    BackupBuffer[DATA_LENGTH]=0x01;
    BackupBuffer[DATA_LEDS]=0x01;
}

uint8_t checkingCRC;
uint8_t failed=0;
bool startcounting1=false;
bool stopcounting1=false;
bool startcounting2=false;
bool stopcounting2=false;
uint8_t counter1 = 0; // Variable to store the count
uint8_t counter2 = 0; // Variable to store the count
bool QF_Flag=false;
uint8_t QF_Event=0;

static PT_THREAD(displayThread)
{
    tsDisplayParams *params = process->parameters;
    const tsDisplayConsts *consts = process->constants;
    
    PT_BEGIN();

    Initialize_Buffer();
    #ifdef GALAXY
    for (;;)
    {
        params->buffer[PREAMBLE]=BackupBuffer[PREAMBLE]; //preamble
        params->buffer[COMMAND_ID]=BackupBuffer[COMMAND_ID];
        params->buffer[DATA_LENGTH]=BackupBuffer[DATA_LENGTH];
        params->buffer[DATA_LEDS]=BackupBuffer[DATA_LEDS];  //LEDs
        params->buffer[DATA_BUTTONS]=crc8MaximArray(crc8MaximInit(), params->buffer, 4);        
        devComSend(consts->uart, params->buffer, 5);
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT((devComReceive(consts->uart, receiveddata, 8) == 8), 10, 50)
        {
            failed = 1;
        }
        checkingCRC= crc8MaximArray(crc8MaximInit(), receiveddata, 7);

        if (receiveddata[4] == SW1_BUTTON_ON)//if pressed
        {   
            buttonState_SW1=true;
            if (!prevButtonState_SW1 && buttonState_SW1) 
            {
                startcounting1=true;
                stopcounting1=false;
            }
        }
        else
        {
            buttonState_SW1 = false;
            if (prevButtonState_SW1 && !buttonState_SW1) 
            {
                startcounting1=false;
                stopcounting1=true;
            }
        }
        prevButtonState_SW1 = buttonState_SW1;
        if(startcounting1==true)
        {
            counter1+=1;
        }
        //////////////////////////////////////////////////////////////////////////////
        ////                      FOR SW1                                           ///
        //////////////////////////////////////////////////////////////////////////////
        if((stopcounting1==true) && (counter1!=0))
        {
            if(counter1>LONGPRESS_THRESHOLD) //60*50=3000ms=3seconds
            {
                //long press - Quick Freeze Mode
                uint8_t eventBuf = eDisplayEventC0QCool;
                dequePushBack(params->eventsQueue, &eventBuf, 1);
            } 
            else
            {
                    uint8_t eventBuf = eDisplayEventC0SetUp;
                    dequePushBack(params->eventsQueue, &eventBuf, 1);       
            }
            counter1=0;
        }
        //////////////////////////////////////////////////////////////////////////////
        ////                     FOR SWITCH2                                       ///
        //////////////////////////////////////////////////////////////////////////////
        if (receiveddata[4] == SW2_BUTTON_ON)//if pressed
        {   
            buttonState_SW2=true;
            if (!prevButtonState_SW2 && buttonState_SW2) 
            {
                startcounting2=true;
                stopcounting2=false;
            }
        }
        else
        {
            buttonState_SW2 = false;
            if (prevButtonState_SW2 && !buttonState_SW2) 
            {
                startcounting2=false;
                stopcounting2=true;
            }
        }
        prevButtonState_SW2 = buttonState_SW2;
        if(startcounting2==true)
        {
            counter2+=1;
        }
        if((stopcounting2==true) && (counter2!=0))
        {
            if(counter2>LONGPRESS_THRESHOLD) 
            {    
                //long press 
                uint8_t eventBuf = eDisplayEventC0Mode;
                dequePushBack(params->eventsQueue, &eventBuf, 1);
            } 
            else
            {
                //short press
            }
            counter2=0;
        }
        if ((params->data.cabinData[0] == 1) || (params->data.cabinData[0] == -24))
        {
            BackupBuffer[DATA_LEDS] = FRZ_LED_LOW;
        }
        else if ((params->data.cabinData[0] == 2) || (params->data.cabinData[0] == -23))
        {
            BackupBuffer[DATA_LEDS] = FRZ_LED_MID_LOW;
        }
        else if ((params->data.cabinData[0] == 3) || (params->data.cabinData[0] == -22))
        {
            BackupBuffer[DATA_LEDS] = FRZ_LED_MED_MAX;
        }
        else if ((params->data.cabinData[0] == 4) || (params->data.cabinData[0] == -21))
        {
            BackupBuffer[DATA_LEDS] = FRZ_LED_MAX;
        }
        else 
        {
            ;
        }
        if(params->data.funcs.ledCabin0Quick)
        {
            BackupBuffer[DATA_LEDS] = FRZ_LED_QUICKFREEZE;
        }
        if(params->data.funcs.ledAlert)
        {
            BackupBuffer[DATA_LEDS] |= (1 << THSEN_BIT);
        }
        else
        {
            BackupBuffer[DATA_LEDS] &= ~(1 << THSEN_BIT);
        }
        if (IS_MODE_QF(params)) 
        {
            BackupBuffer[DATA_LEDS] |= (1 << LED_COOLERMODE_BIT);
        }
        else
        {
            BackupBuffer[DATA_LEDS] &= ~(1 << LED_COOLERMODE_BIT);
        }
        
        if (params->data.funcs.ledApplianceOff || params->data.funcs.displayOff) 
        {
            BackupBuffer[DATA_LEDS] = 0;
        }
            PT_YIELD_TIMEOUT(consts->period);
    }
    #endif

    #ifdef ATHENA
    for (;;)
    {
        params->buffer[PREAMBLE]=BackupBuffer[PREAMBLE]; //preamble
        params->buffer[COMMAND_ID]=BackupBuffer[COMMAND_ID];
        params->buffer[DATA_LENGTH]=BackupBuffer[DATA_LENGTH];
        params->buffer[DATA_LEDS]=BackupBuffer[DATA_LEDS];  //LEDs
        params->buffer[DATA_BUTTONS]=crc8MaximArray(crc8MaximInit(), params->buffer, 4);        
        devComSend(consts->uart, params->buffer, 5);
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT((devComReceive(consts->uart, receiveddata, 8) == 8), 10, 50)
        {
            failed = 1;
        }
        checkingCRC= crc8MaximArray(crc8MaximInit(), receiveddata, 7);

        if (receiveddata[4] == SW1_BUTTON_ON)//if pressed
        {   
            buttonState_SW1=true;
            if (!prevButtonState_SW1 && buttonState_SW1) 
            {
                startcounting1=true;
                stopcounting1=false;

            }
        }
        else
        {
            buttonState_SW1 = false;
            if (prevButtonState_SW1 && !buttonState_SW1) 
            {
                startcounting1=false;
                stopcounting1=true;
            }
        }
        prevButtonState_SW1 = buttonState_SW1;
        if(startcounting1==true)
        {
            counter1+=1;
        }
        
        // BUTTON SW1
        
        if((stopcounting1==true) && (counter1!=0))
        {
            if(counter1>LONGPRESS_THRESHOLD)
            {
                //long press - Quick Freeze Mode
                uint8_t eventBuf = eDisplayEventApplianceOFF;
                dequePushBack(params->eventsQueue, &eventBuf, 1);
            } 
            else
            {    
                if(QF_Flag!=true)
                {
                uint8_t eventBuf = eDisplayEventC0SetUp;
                dequePushBack(params->eventsQueue, &eventBuf, 1); 
                }
                else
                {
                    QF_Event+=1;
                    uint8_t eventBuf = eDisplayEventC0QCool;
                    dequePushBack(params->eventsQueue, &eventBuf, 1);
                    if(QF_Event==2)
                    {
                        uint8_t eventBuf = eDisplayEventC0SetUp;
                        dequePushBack(params->eventsQueue, &eventBuf, 1);
                        QF_Flag=false; 
                        PT_WAIT_UNTIL(params->data.cabinData[0] == -24);
                        QF_Event=0;
                    }
                
                }   
                
            }     
            counter1=0;
        }
        
        if(QF_Flag!=true) 
          {
          if (params->data.cabinData[0] == -24) 
          {
              BackupBuffer[DATA_LEDS] = ATHENA_LED_LOW;
          }
          else if (params->data.cabinData[0] == -23)
          {
              BackupBuffer[DATA_LEDS] = ATHENA_LED_MID_LOW;
          }
          else if (params->data.cabinData[0] == -22)//5
          {
              BackupBuffer[DATA_LEDS] = ATHENA_LED_MID;
          }
          else if (params->data.cabinData[0] == -21)//6
          {
              BackupBuffer[DATA_LEDS] = ATHENA_LED_MED_MAX;
          }
          else if (params->data.cabinData[0] == -20)//7
          {
              BackupBuffer[DATA_LEDS] = ATHENA_LED_MAX;
              QF_Flag=true; 
          }
          else
          {
              ; 
          }}
        
        
        if(params->data.funcs.ledCabin0Quick)
        {
            BackupBuffer[DATA_LEDS] = ATHENA_LED_QUICKFREEZE;
        }
        if(params->data.funcs.ledAlert)
        {
            BackupBuffer[DATA_LEDS] |= (1 << THSEN_BIT);
        }
        else
        {
            BackupBuffer[DATA_LEDS] &= ~(1 << THSEN_BIT);
        }
       
        if (params->data.funcs.ledApplianceOff || params->data.funcs.displayOff) 
        {
            BackupBuffer[DATA_LEDS] = 0;
        }
            
            PT_YIELD_TIMEOUT(consts->period);

    }
    #endif
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

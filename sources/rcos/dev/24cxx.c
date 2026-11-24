/** @file       24cxx.c
 *  @brief      C file of 24CXX device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Rohat Ibishukcu
 *  @date       28.06.2020
 */
#define FILE_24CXX_C

#include "24cxx.h"

/**
 *  @addtogroup 24CXX
 *  @{
 */

#define BUS_ADDRESS (0x50 )//| consts->aaa) ///< Last 3 bits change depending on device size and address pins

// XX (01, 02, 04, 08, 16, 32, 64, 128, 256)
/// Page size of XX
const uint8_t pageSize[] = {8, 8, 16, 16, 16, 32, 32, 64, 64};
#define PAGE_SIZE  (pageSize[BIT_LSB(consts->xx)])
#define REG_SIZE   ((consts->xx >= 32) ? 2 : 1)
#define TOTAL_SIZE ((consts->xx * 1024 / 8))

#define M24CXX_PT_ENTRY()                                               \
    const tsDevMem *device      = (const tsDevMem *)process->constants; \
    ts24cxxParams *params       = device->parameters;                   \
    const ts24cxxConsts *consts = device->constants;



static PT_THREAD(nvm24cxxPtWrite)
{
    M24CXX_PT_ENTRY();
    //uint8_t Addr1 = (params->address & 0xFF);
    //uint8_t Addr2 = (params->address >> 8) & 0xFF;
    PT_BEGIN();
   
    do
    {
 //      consts->target->slave = BUS_ADDRESS ;//| ((params->address >> (8 * REG_SIZE)) & 0x07); // Recalculate slave address

        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->i2c, params->ticket), 10); // Wait for ticket validation

        params->subLen = PAGE_SIZE - (params->address & (PAGE_SIZE - 1)); /// Program until end of current page
        params->subLen = MIN(params->length, params->subLen);
       
       //for renesas mcu
      params->address_Data[1]= (params->address & 0xFF);
       params->address_Data[0]= (params->address >> 8) & 0xFF;
       memcpy(params->address_Data+2,params->buffer,params->subLen);


         PT_WAIT_WHILE_W_PERIOD((EXIT_FAILURE == devComOpen(consts->i2c, consts->target)),4); // Wait until eeprom is available
    //    devComSend(consts->i2c, (void *)&params->address, sizeof(params->address));
  //      devComSend(consts->i2c, (void *)&Addr2, sizeof(Addr2));
  //      devComSend(consts->i2c, (void *)&Addr1, sizeof(Addr1));     
       // PT_WAIT_WHILE(devComSend(consts->i2c, NULL, 0));
  //     devComSend(consts->i2c, params->buffer, params->subLen);
     devComSend(consts->i2c, params->address_Data, params->subLen+2);
        devComClose(consts->i2c, consts->target);
          
        PT_WAIT_WHILE(devComTicketValid(consts->i2c, params->ticket));
       
        params->address += params->subLen;                           /// Update address
        params->buffer = (uint8_t *)params->buffer + params->subLen; /// Update pointer
        params->length -= params->subLen;                            /// This amount of data is written
       PT_YIELD_TIMEOUT(5);
    } while (params->length);

    // Operation finished
    TICKET_REMOVE(device->sys->tvm);

    PT_END();
}



PROCESS_INIT_PROTO(nvm24cxxProcInit)
{
    M24CXX_PT_ENTRY();

    UNUSED(params);
 //   consts->target->regAdrSize = REG_SIZE;
  //  consts->target->slave      = BUS_ADDRESS;

    process->initialized     = TRUE;
    device->sys->initialized = TRUE;
}

PROCESS_DEINIT_PROTO(nvm24cxxProcDeinit)
{
    process->initialized = FALSE;
    threadStop(process, process->threadFunction); // Whatever operation is done currently
}

static DEV_MEM_FUNC_INIT(nvm24cxxInit);
static DEV_MEM_FUNC_DEINIT(nvm24cxxDeinit);
static DEV_MEM_FUNC_READ(nvm24cxxRead);
static DEV_MEM_FUNC_WRITE(nvm24cxxWrite);
// static DEV_MEM_FUNC_PROGRAM(nvm24cxxProgram);
//static DEV_MEM_FUNC_ERASE(nvm24cxxErase);
// static DEV_MEM_FUNC_TICKET_GET(nvm24cxxTicketGet);
// static DEV_MEM_FUNC_TICKET_VALID(nvm24cxxTicketValid);

/// @brief  Structure that defines the functions for 24cxx
const tsDevMemFuncs devMem24cxxFuncs =
    {
        .init        = nvm24cxxInit,
        .deinit      = nvm24cxxDeinit,
        .read        = nvm24cxxRead,
        .write       = nvm24cxxWrite,
        .program     = nvm24cxxWrite,
        .erase       = NULL,
        .ticketGet   = NULL,
        .ticketValid = NULL,
};

/// @brief  Init function for a 24cxx
static DEV_MEM_FUNC_INIT(nvm24cxxInit)
{
    const ts24cxxConsts *consts = device->constants;

    ADD_RCOS_VERSION("24cxx", "1.0.0");
    devComInit(consts->i2c);
    processStart(consts->process);

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a 24cxx
static DEV_MEM_FUNC_DEINIT(nvm24cxxDeinit)
{
    const ts24cxxConsts *consts = device->constants;

    device->sys->initialized = FALSE;
    processStop(consts->process);

    return EXIT_SUCCESS;
}

/// @brief  Read function for a 24cxx
static DEV_MEM_FUNC_READ(nvm24cxxRead)
{
  ts24cxxParams *params       = device->parameters;
    const ts24cxxConsts *consts = device->constants;
    uint8_t addr[2];
    uint8_t sublen;
    addr[1]= (address & 0xFF);
    addr[0] = (address >> 8) & 0xFF;
    if (address + length > TOTAL_SIZE)
    {
        return 0;
    }
    if(length >128)
    {
      sublen = length -128;
    params->ticket = devComTicketGet(consts->i2c);

    while(EXIT_FAILURE == devComOpen(consts->i2c, consts->target)); // Wait until eeprom is available
    devComSend(consts->i2c, (void *)addr, 2); 

    devComReceive(consts->i2c, readData, 128); // Receive memory data
    devComClose(consts->i2c, consts->target);
    
    if(addr[1]>127)
      addr[0]++;
    addr[1] += 128;
    
    params->ticket = devComTicketGet(consts->i2c);

    while(EXIT_FAILURE == devComOpen(consts->i2c, consts->target)); // Wait until eeprom is available
   
    devComSend(consts->i2c, (void *)addr, 2); 
    uint8_t * shiftedptr = (uint8_t *)readData;
    shiftedptr+=128;
    devComReceive(consts->i2c, shiftedptr, sublen); // Receive memory data
    devComClose(consts->i2c, consts->target);
  //  TICKET_REMOVE(sys->tvm);
    }
    
    else if(length<=128)
    {
    
    params->ticket = devComTicketGet(consts->i2c);

    while(EXIT_FAILURE == devComOpen(consts->i2c, consts->target)); // Wait until eeprom is available
   // devComSend(consts->i2c, (void *)&Addr2, sizeof(Addr2));
   // devComSend(consts->i2c, (void *)&Addr1, sizeof(Addr1));                 // Send registery address
    devComSend(consts->i2c, (void *)addr, 2); 

    devComReceive(consts->i2c, readData, length); // Receive memory data
    devComClose(consts->i2c, consts->target);
    }
    return length;
    
}

/// @brief  Write function for a 24cxx
static DEV_MEM_FUNC_WRITE(nvm24cxxWrite)
{
    ts24cxxParams *params       = device->parameters;
    const ts24cxxConsts *consts = device->constants;
    //tsDevMemSystem *sys         = device->sys;

    if (address + length > TOTAL_SIZE)
    {
        return 0;
    }

    if (!consts->process->threadFunction)
    {
        params->address = address;
        params->buffer  = (void *)writeData;
        params->length  = length;
        threadStart(consts->process, nvm24cxxPtWrite);

        return length;
    }
    else
    {
        return 0;
    }
}

/// @brief  Erase function for a 24cxx


#if 0 // Enable if 24cxx requires another functionality other than generic ticket functions

/// @brief  Ticket get function for a 24cxx
static DEV_MEM_FUNC_TICKET_GET(nvm24cxxTicketGet)
{
    ts24cxxParams *params       = device->parameters;
    const ts24cxxConsts *consts = device->constants;
    tsDevMemSystem *sys               = device->sys;
}

/// @brief  Ticket valid function for a 24cxx
static DEV_MEM_FUNC_TICKET_VALID(nvm24cxxTicketValid)
{
    ts24cxxParams *params       = device->parameters;
    const ts24cxxConsts *consts = device->constants;
    tsDevMemSystem *sys               = device->sys;
}

#endif

/** @} */

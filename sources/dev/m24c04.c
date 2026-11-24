/** @file       m24c04.c
 *  @brief      C file of M24C04 device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Vehbi Umur Cabuk
 *  @date       8.10.2020
 */
#define FILE_M24C04_C

#include "m24c04.h"

/**
 *  @addtogroup M24C04
 *  @{
 */
#define E2_INITIAL_WRITE_DELAY (2000) // in terms of ms

static PROCESS_HANDLER_PROTO(m24c04Handler);
PROCESS_INIT_PROTO(m24c04ProInit);
PROCESS_DEINIT_PROTO(m24c04ProDeinit);

static DEV_MEM_FUNC_INIT(m24c04Init);
static DEV_MEM_FUNC_DEINIT(m24c04Deinit);
static DEV_MEM_FUNC_READ(m24c04Read);
static DEV_MEM_FUNC_WRITE(m24c04Write);
static DEV_MEM_FUNC_PROGRAM(m24c04Program);
static DEV_MEM_FUNC_ERASE(m24c04Erase);
static DEV_MEM_FUNC_READ(m24c04ReadBlocking);
static DEV_MEM_FUNC_WRITE(m24c04WriteBlocking);
// static DEV_MEM_FUNC_TICKET_GET(m24c04TicketGet);
// static DEV_MEM_FUNC_TICKET_VALID(m24c04TicketValid);

/// @brief  Structure that defines the functions for m24c04
const tsDevMemFuncs devMemM24c04Funcs =
    {
        m24c04Init,
        m24c04Deinit,
        m24c04Read,
        m24c04Write,
        m24c04Program,
        m24c04Erase,
        NULL,
        NULL,
};

static PT_THREAD(m24c04WriteThread)
{
    const tsDevMem *device = (const tsDevMem *)process->constants; // constants ile device yollaniyor aslinda
    const tsM24c04Consts *consts = (const tsM24c04Consts *)device->constants;
    tsM24c04Params *params = (tsM24c04Params *)process->parameters;
    static uint16_t address = 0;
    static uint8_t writeBuffer[EEPROM_24_C_04_PAGE_SIZE] = {0};
    static uint8_t readBuffer[EEPROM_24_C_04_PAGE_SIZE] = {0};
    uint8_t tryCnt = 0;

    PT_BEGIN();

    for (;;)
    {
        PT_WAIT_UNTIL(params->tge._cnt == 0);

        PT_WAIT_UNTIL((!DEQUE_IS_EMPTY(params->addrQueue)));

        params->ticket = devComTicketGet(consts->i2c);

        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(devComTicketValid(consts->i2c, params->ticket), 5, 100)
        {
            TICKET_REMOVE(consts->i2c->sys->tvm);
        }
        else
        {
            dequePopFront(params->addrQueue, &address, sizeof(address));

            if (address < EEPROM_24_C_04_PART1)
                consts->target->slave = EEPROM_24_C_04_ADDR_PART1;
            else if (address < EEPROM_24_C_04_PART2)
                consts->target->slave = EEPROM_24_C_04_ADDR_PART2;

            memcpy(writeBuffer, params->ramCopy + address, EEPROM_24_C_04_PAGE_SIZE);
            if (m24c04WriteBlocking(device, address, params->ramCopy + address, EEPROM_24_C_04_PAGE_SIZE) == EXIT_FAILURE)
            {
                params->regs.bits.comErr = TRUE;

                dequePushBack(params->addrQueue, &address, sizeof(address));
            }
            else
            {
                params->regs.bits.comErr = FALSE;
            }
        }

        PT_YIELD_TIMEOUT(EEPROM_24_C_04_TMR_PERIOD_WRITING);

        tryCnt = 100;
        while (m24c04ReadBlocking(device, address, readBuffer, EEPROM_24_C_04_PAGE_SIZE) != EEPROM_24_C_04_PAGE_SIZE)
        {
            if (--tryCnt == 0)
            {
                RCOS_ASSERT(0, RASSERT_PRI_LOW);
                break;
            }
        }
        RCOS_ASSERT(memcmp(writeBuffer,readBuffer,EEPROM_24_C_04_PAGE_SIZE)==0, RASSERT_PRI_LOW);
    }

    PT_END();
}

/// @brief  Init handler function of m24c04 eeprom
PROCESS_INIT_PROTO(m24c04ProInit)
{
    threadStart(process, m24c04WriteThread);
    process->initialized = TRUE;
}

/// @brief  Deinit handler function of m24c04 eeprom
PROCESS_DEINIT_PROTO(m24c04ProDeinit)
{
    PROCESS_STATE_CHANGE(process, NULL);
    threadStop(process, m24c04WriteThread);
    process->initialized = FALSE;
}

/// @brief  Init function for a m24c04
static DEV_MEM_FUNC_INIT(m24c04Init)
{
    const tsM24c04Consts *consts = (const tsM24c04Consts *)device->constants;
    tsM24c04Params *params = (tsM24c04Params *)device->parameters;
    static uint8_t tryCnt = 20;

    devComInit(consts->i2c);
    device->sys->initialized = TRUE;

    consts->target->slave = EEPROM_24_C_04_ADDR_PART1;
    tryCnt = 20;
    while (m24c04ReadBlocking(device, 0x00, params->ramCopy, EEPROM_24_C_04_PART_SIZE) != EEPROM_24_C_04_PART_SIZE)
    {
        if (--tryCnt == 0)
            break;
    }

    consts->target->slave = EEPROM_24_C_04_ADDR_PART2;
    tryCnt = 100; // arka arkaya iki komut gelmesi durumunda tepki vermesi uzun surebiliyor.
    while (m24c04ReadBlocking(device, 0x00, params->ramCopy + EEPROM_24_C_04_PART_SIZE, EEPROM_24_C_04_PART_SIZE) != EEPROM_24_C_04_PART_SIZE)
    {
        if (--tryCnt == 0)
            break;
    }

    timerGenericStart(&params->tge, E2_INITIAL_WRITE_DELAY);

    processStart(consts->process);

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a m24c04
static DEV_MEM_FUNC_DEINIT(m24c04Deinit)
{
    const tsM24c04Consts *consts = (const tsM24c04Consts *)device->constants;

    device->sys->initialized = FALSE;

    processStop(consts->process);

    return EXIT_SUCCESS;
}

/// @brief  Read function for a m24c04
static DEV_MEM_FUNC_READ(m24c04Read)
{
    tsM24c04Params *params = (tsM24c04Params *)device->parameters;
    tsDevMemSystem *sys = (tsDevMemSystem *)device->sys;

    if ((!sys->initialized) || (address > EEPROM_24_C_04_SIZE - 1) || (address + length > EEPROM_24_C_04_SIZE - 1))
    {
        return 0;
    }

    memcpy(readData, (params->ramCopy + address), length);

    TICKET_REMOVE(sys->tvm);

    return length;
}

/// @brief  Write function for a m24c04
static DEV_MEM_FUNC_WRITE(m24c04Write)
{
    tsM24c04Params *params = (tsM24c04Params *)device->parameters;
    tsDevMemSystem *sys = (tsDevMemSystem *)device->sys;
    uint16_t finishAddr = 0;
    uint16_t addAddress = 0;

    if ((!sys->initialized) || (address > EEPROM_24_C_04_SIZE - 1) || (address + length > EEPROM_24_C_04_SIZE - 1))
    {
        return 0;
    }

    memcpy(params->ramCopy + address, writeData, length);

    // Generate write request and add address to queue
    finishAddr = address + length;

    // Generate write request and add address to queue
    for (uint16_t idx = 0; idx < EEPROM_24_C_04_SIZE; idx += EEPROM_24_C_04_PAGE_SIZE)
    {
        addAddress = (address & EEPROM_24_C_04_PAGE_MASK) + idx;

        if (addAddress < finishAddr)
        {
            if (!DEQUE_IS_FULL(params->addrQueue))
            {
                params->regs.bits.outOfQueueMemory = FALSE;

                dequePushBack(params->addrQueue, &addAddress, sizeof(addAddress));
            }
            else
            {
                params->regs.bits.outOfQueueMemory = TRUE;

                TICKET_REMOVE(sys->tvm);

                return 0;
            }
        }
        else
        {
            break;
        }
    }

    TICKET_REMOVE(sys->tvm);

    return length;
}

/// @brief  Program function for a m24c04
static DEV_MEM_FUNC_PROGRAM(m24c04Program)
{
    tsM24c04Params *params = (tsM24c04Params *)device->parameters;
    const tsM24c04Consts *consts = (const tsM24c04Consts *)device->constants;
    tsDevMemSystem *sys = (tsDevMemSystem *)device->sys;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    if (!sys->initialized)
    {
        return 0;
    }

    UNUSED(address);  // REMOVE IF USED
    UNUSED(progData); // REMOVE IF USED
    UNUSED(length);   // REMOVE IF USED

    TICKET_REMOVE(sys->tvm);

    return length;
}

/// @brief  Erase function for a m24c04
static DEV_MEM_FUNC_ERASE(m24c04Erase)
{
    tsM24c04Params *params = (tsM24c04Params *)device->parameters;
    tsDevMemSystem *sys = (tsDevMemSystem *)device->sys;
    uint16_t finishAddr = address + size;
    uint16_t addAddress = 0;

    if ((!sys->initialized) || (address > EEPROM_24_C_04_SIZE - 1) || (address + size > EEPROM_24_C_04_SIZE - 1))
    {
        return 0;
    }

    memset(params->ramCopy + address, 0x00, size);

    // Generate write request and add address to queue
    for (uint16_t idx = 0; idx < EEPROM_24_C_04_SIZE; idx += EEPROM_24_C_04_PAGE_SIZE)
    {
        addAddress = (address & EEPROM_24_C_04_PAGE_MASK) + idx;

        if (addAddress < finishAddr)
        {
            if (!DEQUE_IS_FULL(params->addrQueue))
            {
                params->regs.bits.outOfQueueMemory = FALSE;

                dequePushBack(params->addrQueue, &addAddress, sizeof(addAddress));
            }
            else
            {
                params->regs.bits.outOfQueueMemory = TRUE;

                TICKET_REMOVE(sys->tvm);

                return 0;
            }
        }
        else
        {
            break;
        }
    }

    TICKET_REMOVE(sys->tvm);

    return size;
}

/// @brief  Blocking read function for a m24c04
static DEV_MEM_FUNC_READ(m24c04ReadBlocking)
{
    tsM24c04Params *params = (tsM24c04Params *)device->parameters;
    const tsM24c04Consts *consts = (const tsM24c04Consts *)device->constants;
    tsDevMemSystem *sys = (tsDevMemSystem *)device->sys;
    static uint8_t tryCount = 0;
    uint8_t _8bAddr = EEPROM_24_C_04_ADDR_A7_A0(address);

    if ((!sys->initialized) || (address > EEPROM_24_C_04_SIZE - 1) || (address + length > EEPROM_24_C_04_SIZE - 1))
    {
        return 0;
    }

    params->ticket = devComTicketGet(consts->i2c);

    // send buffer to i2c bus
    while (devComOpen(consts->i2c, consts->target) == EXIT_FAILURE)
    {
        if (++tryCount == 50)
        {
            TICKET_REMOVE(consts->i2c->sys->tvm);

            TICKET_REMOVE(sys->tvm);

            return 0;
        }
    }
    devComSend(consts->i2c, (void *)&_8bAddr, sizeof(_8bAddr));

    // prepare buffer for receiving
    // send buffer to i2c bus. This is repeated start.
    devComReceive(consts->i2c, (void *)readData, length);
    devComClose(consts->i2c, consts->target);

    TICKET_REMOVE(sys->tvm);

    return length;
}

/// @brief  Blocking write function for a m24c04
static DEV_MEM_FUNC_WRITE(m24c04WriteBlocking)
{
    tsM24c04Params *params = (tsM24c04Params *)device->parameters;
    const tsM24c04Consts *consts = (const tsM24c04Consts *)device->constants;
    tsDevMemSystem *sys = (tsDevMemSystem *)device->sys;
    static uint8_t tryCount = 0;
    uint8_t _8bAddr = EEPROM_24_C_04_ADDR_A7_A0(address);

    if ((!sys->initialized) || (address > EEPROM_24_C_04_SIZE - 1) || (address + length > EEPROM_24_C_04_SIZE - 1))
    {
        return 0;
    }

    // send buffer to i2c bus
    while (devComOpen(consts->i2c, consts->target) == EXIT_FAILURE)
    {
        if (++tryCount == 50)
        {
            TICKET_REMOVE(consts->i2c->sys->tvm);

            TICKET_REMOVE(sys->tvm);

            return 0;
        }
    }
    devComSend(consts->i2c, (void *)&_8bAddr, sizeof(_8bAddr));

    // prepare buffer for transmitting
    devComSend(consts->i2c, (void *)writeData, length);
    devComClose(consts->i2c, consts->target);

    TICKET_REMOVE(sys->tvm);

    return length;
}

#if 0 // Enable if m24c04 requires another functionality other than generic ticket functions

/// @brief  Ticket get function for a m24c04
static DEV_MEM_FUNC_TICKET_GET(m24c04TicketGet)
{
    tsM24c04Params *params       = device->parameters;
    const tsM24c04Consts *consts = device->constants;
    tsDevMemSystem *sys               = device->sys;
}

/// @brief  Ticket valid function for a m24c04
static DEV_MEM_FUNC_TICKET_VALID(m24c04TicketValid)
{
    tsM24c04Params *params       = device->parameters;
    const tsM24c04Consts *consts = device->constants;
    tsDevMemSystem *sys               = device->sys;
}

#endif

static DEV_IO_FUNC_INIT(m24c04StatusInit);
static DEV_IO_FUNC_DEINIT(m24c04StatusDeinit);
static DEV_IO_FUNC_GET(m24c04StatusGet);
static DEV_IO_FUNC_PUT(m24c04StatusPut);

/// @brief  Structure that defines the functions for m24c04
const tsDevIoFuncs devIoM24c04StatusFuncs =
    {
        m24c04StatusInit,
        m24c04StatusDeinit,
        m24c04StatusGet,
        m24c04StatusPut,
};

/// @brief Memory Status Device Init
static DEV_IO_FUNC_INIT(m24c04StatusInit)
{
    UNUSED(device);
    UNUSED(config);
    return EXIT_SUCCESS;
}

/// @brief Memory Status Device DeInit
static DEV_IO_FUNC_DEINIT(m24c04StatusDeinit)
{
    UNUSED(device);
    return EXIT_SUCCESS;
}

/// @brief Memory Status Device Get
static DEV_IO_FUNC_GET(m24c04StatusGet)
{
    const tsM24c04StatusConsts *statusConst = device->constants;
    const tsDevMem *memDevice = statusConst->mem;
    tsM24c04Params *memParams = memDevice->parameters;

    return (memParams->regs.byte);
}

/// @brief Memory Status Device Put
static DEV_IO_FUNC_PUT(m24c04StatusPut)
{
    UNUSED(device);
    UNUSED(data);
    return EXIT_SUCCESS;
}

/** @} */

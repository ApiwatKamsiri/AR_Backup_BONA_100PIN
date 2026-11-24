/** @file       m41t00.c
 *  @brief      C file of M41T00 device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Furkan Gullu
 *  @date       2019
 */
#define FILE_M41T00_C

#include "m41t00.h"

/**
 *  @addtogroup M41T00
 *  @{
 */

typedef enum
{
    eM41t00Init,
    eM41t00Periodic,
    eM41t00Read,
    eM41t00Write,
} teM41t00Events;

PT_THREAD(m41t00Thread);

PROCESS_INIT_PROTO(m41t00ProcInit)
{
    const tsM41t00CoreConsts *consts = process->constants;
    ADD_RCOS_VERSION("m41t00", "2.0.0")

    devComInit(consts->i2c);
    consts->target->slave      = 0x68;
    consts->target->regAdrSize = 1;
    process->initialized       = 1;

    threadStart(process, m41t00Thread);
}

PROCESS_DEINIT_PROTO(m41t00ProcDeinit)
{
    threadStop(process, m41t00Thread);
    process->initialized = 0;
}

PT_THREAD(m41t00Thread)
{
    tsM41t00CoreParams *params       = process->parameters;
    const tsM41t00CoreConsts *consts = process->constants;

    uint8_t regAdr   = 0; // First address of registries
    uint32_t timeVal = 0;
    uint32_t dateVal = 0;

    PT_BEGIN();

    for (;;)
    {
        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(devComTicketValid(consts->i2c, params->ticket),
                                          10, 100)
        {
            // Ticket timeout
        }
        devComOpen(consts->i2c, consts->target);
        devComSend(consts->i2c, &regAdr, sizeof(regAdr));
        PT_WAIT_WHILE(devComSend(consts->i2c, NULL, 0));
        devComReceive(consts->i2c, &params->regs, sizeof(params->regs));
        devComClose(consts->i2c, consts->target);
        PT_WAIT_WHILE(devComTicketValid(consts->i2c, params->ticket));

        // Time value
        timeVal = bcdDecode8(params->regs.secondsBcd);
        timeVal += (uint32_t)bcdDecode8(params->regs.minutesBcd) * 60;
        timeVal += (uint32_t)bcdDecode8(params->regs.hoursBcd) * 60 * 60;

        // Date value
        dateVal = (uint32_t)bcdDecode8(params->regs.dateBcd);         // 1..31
        dateVal |= (uint32_t)bcdDecode8(params->regs.monthBcd) << 8;  // 1..12
        dateVal |= (uint32_t)bcdDecode8(params->regs.yearsBcd) << 16; // 0..99

        if (params->writeReq)
        // if (((timeVal ^ params->time) & ~0x3ul) &&
        // consts->device->sys->initialized)
        {
            uint32_t temp1, temp2;

            // Write new time value into registers
            temp1                   = params->time;                     // temp1 = sec
            temp2                   = temp1 / 60;                       // temp2 = min
            params->regs.secondsBcd = bcdEncode8(temp1 - (temp2 * 60)); // seconds remainder
            temp1                   = temp2 / 60;                       // temp1 = hour
            params->regs.minutesBcd = bcdEncode8(temp2 - (temp1 * 60)); // minutes remainder
            params->regs.hoursBcd   = bcdEncode8(temp1);                // it's composed of secs + mins + hours, hence remainder is only hours

            // Write new date value into registers
            params->regs.dateBcd  = bcdEncode8(params->date);
            params->regs.monthBcd = bcdEncode8(params->date >> 8);
            params->regs.yearsBcd = bcdEncode8(params->date >> 16);

            params->ticket = devComTicketGet(consts->i2c);
            PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(devComTicketValid(consts->i2c, params->ticket),
                                              10, 100)
            {
                // Ticket timeout
            }
            devComOpen(consts->i2c, consts->target);
            devComSend(consts->i2c, &regAdr, sizeof(regAdr));
            PT_WAIT_WHILE(devComSend(consts->i2c, NULL, 0));
            devComSend(consts->i2c, &params->regs, sizeof(params->regs));
            devComClose(consts->i2c, consts->target);

            // Write complete
            params->writeReq = 0;
        }
        else
        {
            // Read complete
            params->time = timeVal;
            params->date = dateVal;

            // Values are received and ready to use
            consts->device->sys->initialized = 1;
        }
        PT_YIELD_TIMEOUT(500);
    }

    PT_END();
}

static DEV_CPX_FUNC_INIT(m41t00CoreInit);
static DEV_CPX_FUNC_DEINIT(m41t00CoreDeinit);

/// @brief  Structure that defines the functions for m41t00
const tsDevCpxFuncs devCpxM41t00CoreFuncs =
    {
        m41t00CoreInit,
        m41t00CoreDeinit,
};

/// @brief  Init function for a m41t00
static DEV_CPX_FUNC_INIT(m41t00CoreInit)
{
    const tsM41t00CoreConsts *consts = device->constants;

    UNUSED(config);
    processStart(consts->process);

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a m41t00
static DEV_CPX_FUNC_DEINIT(m41t00CoreDeinit)
{
    const tsM41t00CoreConsts *consts = device->constants;

    device->sys->initialized = 0;
    processStop(consts->process);

    return EXIT_SUCCESS;
}

/** @} */

/**
 *  @addtogroup M41T00_TIME
 *  @{
 */

static DEV_IO_FUNC_GET(m41t00TimeGet);
static DEV_IO_FUNC_PUT(m41t00TimePut);

/// @brief  Structure that defines the functions for m41t00 time device
const tsDevIoFuncs devIoM41t00TimeFuncs =
    {
        NULL,
        NULL,
        m41t00TimeGet,
        m41t00TimePut,
};

/// @brief  Get function for a m41t00 time device
static DEV_IO_FUNC_GET(m41t00TimeGet)
{
    const tsM41t00SubConsts *consts = device->constants;

    return consts->mainParams->time;
}

/// @brief  Put function for a m41t00 time device
static DEV_IO_FUNC_PUT(m41t00TimePut)
{
    const tsM41t00SubConsts *consts = device->constants;

    consts->mainParams->time     = data;
    consts->mainParams->writeReq = 1;

    return EXIT_SUCCESS;
}

/** @} */

/**
 *  @addtogroup M41T00_DATE
 *  @{
 */

static DEV_IO_FUNC_GET(m41t00DateGet);
static DEV_IO_FUNC_PUT(m41t00DatePut);

/// @brief  Structure that defines the functions for m41t00 date device
const tsDevIoFuncs devIoM41t00DateFuncs =
    {
        NULL,
        NULL,
        m41t00DateGet,
        m41t00DatePut,
};

/// @brief  Get function for a m41t00 date device
static DEV_IO_FUNC_GET(m41t00DateGet)
{
    const tsM41t00SubConsts *consts = device->constants;

    return consts->mainParams->date;
}

/// @brief  Put function for a m41t00 date device
static DEV_IO_FUNC_PUT(m41t00DatePut)
{
    const tsM41t00SubConsts *consts = device->constants;

    consts->mainParams->date     = data;
    consts->mainParams->writeReq = 1;

    return EXIT_SUCCESS;
}

/** @} */

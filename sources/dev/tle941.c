/** @file       tle941.c
 *  @brief      C file of TLE941 device
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Muhammed Sairoglu
 *  @date       2022-03-18
 */
#define FILE_TLE941_C

#include "tle941.h"

#define GetBit(var, bit_no) (((var) >> (bit_no)) & 1)
#define SetBit(var, bit_no) ((var) |= (1 << (bit_no)))
#define ClearBit(var, bit_no) ((var) &= ~(1 << (bit_no)))
#define WriteBit(var, bit_no, bit_val) ((var) = ((var) & ~(1 << (bit_no))) | ((bit_val) << (bit_no)))

static PT_THREAD(tle941ThrdHandler);

static void setOutput(tsTLE941CoreParams *coreParams, uint8_t outputNo, uint8_t state, uint8_t activeLevel);

/**
 *  @addtogroup TLE941
 *  @{
 */

static DEV_CPX_FUNC_INIT(TLE941CoreInit);
static DEV_CPX_FUNC_DEINIT(TLE941CoreDeinit);

/// @brief  Structure that defines the functions for TLE941
const tsDevCpxFuncs devCpxTLE941Funcs =
    {
        TLE941CoreInit,
        TLE941CoreDeinit,
};

/// @brief  Init function for a TLE941
static DEV_CPX_FUNC_INIT(TLE941CoreInit)
{
    tsTLE941CoreParams *params = device->parameters;
    const tsTLE941CoreConsts *consts = device->constants;
    UNUSED(config); // REMOVE IF USED

    devComInit(consts->spi);
    devIoInit(params->target.cs, NULL);
    devIoInit(consts->en, NULL);
    devIoPut(consts->en, 1);

    device->sys->initialized = 1;

    for (size_t i = 0; i < 3; i++)
        params->regHBCTRL[i] = 0;

    params->writeReg = 0x7; // all registers should be written

    processStart(consts->process);

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a TLE941
static DEV_CPX_FUNC_DEINIT(TLE941CoreDeinit)
{
    tsTLE941CoreParams *params = device->parameters;
    const tsTLE941CoreConsts *consts = device->constants;

    device->sys->initialized = 0;
    processStop(consts->process);
    devIoDeinit(params->target.cs);
    devIoPut(consts->en, 0);
    devIoDeinit(consts->en);

    return EXIT_SUCCESS;
}

/* Process Functions *******************************************/

PROCESS_INIT_PROTO(tle941ProcessInit)
{
    process->initialized = 1;
    threadStart(process, tle941ThrdHandler);
}

PROCESS_DEINIT_PROTO(tle941ProcessDeinit)
{
    threadStop(process, tle941ThrdHandler);
    process->initialized = 0;
}
/**************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 */

/* Thread Function *********************************************/

// addresses of HB_ACT_CTRL registers with write bit set
const uint8_t addHBCTRL[3] = {0x83, 0xc3, 0xa3}; 

static PT_THREAD(tle941ThrdHandler)
{
    tsTLE941CoreParams *params = process->parameters;
    const tsTLE941CoreConsts *consts = process->constants;
    PT_BEGIN();

    for (params->regIdx = 0; params->regIdx < 3; params->regIdx++)
    {
        if (GetBit(params->writeReg, params->regIdx))
        {
            params->ticket = devComTicketGet(consts->spi);
            PT_WAIT_UNTIL_W_PERIOD(devComTicketValid(consts->spi, params->ticket), 5);
            devComOpen(consts->spi, &params->target);

            ClearBit(params->writeReg, params->regIdx);
            params->buffer[0] = addHBCTRL[params->regIdx];         // address
            params->buffer[1] = params->regHBCTRL[params->regIdx]; // data
            devComSend(consts->spi, params->buffer, 2);
            PT_WAIT_WHILE(devComSend(consts->spi, NULL, 0));
            devComClose(consts->spi, &params->target);
            PT_WAIT_WHILE(devComTicketValid(consts->spi, params->ticket));
        }
    }
    // check if a register is updated during writing another register
    if (params->writeReg)
        PT_RESTART();
    PT_END();
}

/**************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 */

/* Output Device Functions *****************************************************/
static DEV_IO_FUNC_INIT(tle941OutputInit);
static DEV_IO_FUNC_DEINIT(tle941OutputDeinit);
static DEV_IO_FUNC_PUT(tle941OutputPut);

const tsDevIoFuncs devIoTLE941OutputFuncs =
    {
        tle941OutputInit,
        tle941OutputDeinit,
        NULL,
        tle941OutputPut,
};

#define RegIdx(outputNo) ((outputNo) / 4)
#define SelfBitIdx(outputNo, side) ((((outputNo)&0x3) << 1) + (side))

enum HBSides
{
    LS = 0,
    HS = 1,
};

static DEV_IO_FUNC_INIT(tle941OutputInit)
{
    const tsTLE941OutputConsts *consts = device->constants;
    tsTLE941CoreParams *coreParams = consts->core->parameters;
    UNUSED(config);

    device->sys->initialized = 1;
    setOutput(coreParams, consts->outputNo, 0, consts->activeLevel);
    return EXIT_SUCCESS;
}
static DEV_IO_FUNC_DEINIT(tle941OutputDeinit)
{
    const tsTLE941OutputConsts *consts = device->constants;
    tsTLE941CoreParams *coreParams = consts->core->parameters;
    device->sys->initialized = 0;
    setOutput(coreParams, consts->outputNo, 0, consts->activeLevel);
    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_PUT(tle941OutputPut)
{
    const tsTLE941OutputConsts *consts = device->constants;
    tsTLE941CoreParams *coreParams = consts->core->parameters;
    const tsTLE941CoreConsts *coreConsts = consts->core->constants;
    setOutput(coreParams, consts->outputNo, data > 0, consts->activeLevel);
    threadStart(coreConsts->process, tle941ThrdHandler);
    return EXIT_SUCCESS;
}

/// @brief  switches both HS and LS switches of an output according to its active level and the given state
/// @note   this is done by changing the corresponding bit in regHBCTRL bytes of the core
static void setOutput(tsTLE941CoreParams *coreParams, uint8_t outputNo, uint8_t state, uint8_t activeLevel)
{
    if (activeLevel == 0)
        state = !state;

    uint8_t regIdx = RegIdx(outputNo);
    WriteBit(coreParams->regHBCTRL[regIdx], SelfBitIdx(outputNo, HS), state);
    WriteBit(coreParams->regHBCTRL[regIdx], SelfBitIdx(outputNo, LS), !state);
    SetBit(coreParams->writeReg, regIdx);
}

/** @} */

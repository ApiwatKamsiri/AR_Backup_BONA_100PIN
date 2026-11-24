/** @file       bu16501ks2.c
 *  @brief      C file of BU16501KS2 device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Furkan Gullu
 *  @author     Deniz Fer
 *  @date       4.02.2020
 */
#define FILE_BU16501KS2_C

#include "bu16501ks2.h"

/**
 *  @addtogroup BU16501KS2
 *  @{
 */

static DEV_CPX_FUNC_INIT(bu16501ks2Init);
static DEV_CPX_FUNC_DEINIT(bu16501ks2Deinit);

/// @brief  Structure that defines the functions for bu16501ks2
const tsDevCpxFuncs devCpxBu16501ks2Funcs =
    {
        bu16501ks2Init,
        bu16501ks2Deinit,
};

/// @brief  Init function for a bu16501ks2
static DEV_CPX_FUNC_INIT(bu16501ks2Init)
{
    const tsBu16501ks2CpxConsts *consts = device->constants;
    ADD_RCOS_VERSION("bu16501ks2", "2.0.0")

    UNUSED(config); // REMOVE IF USED

    processStart(consts->process);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a bu16501ks2
static DEV_CPX_FUNC_DEINIT(bu16501ks2Deinit)
{
    const tsBu16501ks2CpxConsts *consts = device->constants;

    device->sys->initialized = 0;
    processStop(consts->process);

    return EXIT_SUCCESS;
}

#define Adr_Reset (0x00)
#define Adr_OSCEN (0x01)
#define Adr_LEDONL (0x11)
#define Adr_LEDONH (0x12)
#define Adr_PWMDuty (0x20)
#define PWM_DUTY_MASK (0x3f)
#define Adr_CLK (0x21)
#define Adr_PWMSW (0x2D)
#define Adr_MatrixCtrl (0x30)
#define Adr_MatrixCLR (0x31)
#define Adr_RegMap (0x7F)
#define Adr_PattData (0x01)

#define Data_Reset (0x01)
#define Data_OSCEN (0x08)
#define Data_LEDAllON (0xFF)
#define Data_InterOSC (0x00)
#define Data_PWMOFF (0x00)
#define Data_PWMON (0x04)
#define Data_MatrixON (0x01)
#define Data_MatrixOFF (0x00)
#define Data_MatrixCLR (0x01)
#define Data_CtrlMap (0x00)
#define Data_PattMap (0x01)

#define BU16501_EmptyDuty (0x00)

const tsRegData initCombo[] =
    {
        {Adr_RegMap, Data_CtrlMap},      // 0x7f 0x00
        {Adr_CLK, Data_InterOSC},        // 0x21 0x00
        {Adr_OSCEN, Data_OSCEN},         // 0x01 0x08
        {Adr_LEDONL, Data_LEDAllON},     // 0x11 0xff
        {Adr_LEDONH, Data_LEDAllON},     // 0x12 0xff
        {Adr_PWMDuty, BU16501_FullDuty}, // 0x20 0x3f
        {Adr_PWMSW, Data_PWMON},         // 0x2d 0x04

        // UPDATE
        // {Adr_PWMDuty, BU16501_FullDuty}, // 0x20 0x3f
        // {Adr_RegMap, Data_PattMap}, // 0x7f 0x01
        // {61, 0xff},                 // 1..64
        // {62, 0xff},
        // {63, 0xff},
        // {Adr_RegMap, Data_CtrlMap},      // 0x7f 0x00
        // {Adr_MatrixCtrl, Data_MatrixON}, // 0x30 0x01
};

PT_THREAD(bu16501ks2Thread);

PROCESS_INIT_PROTO(bu16501ks2ProcInit)
{
    const tsBu16501ks2CpxConsts *consts = process->constants;

    devComInit(consts->i2c);
    threadStart(process, bu16501ks2Thread);
}

PROCESS_DEINIT_PROTO(bu16501ks2ProcDeinit)
{
    threadStop(process, bu16501ks2Thread);
    process->initialized = 0; // If process needs other checks, clear this another time
}

static uint8_t writeRegArr(tsProcess *process, uint8_t registry, uint8_t *array, uint8_t length)
{
    tsBu16501ks2CpxParams *params       = process->parameters;
    const tsBu16501ks2CpxConsts *consts = process->constants;

    if (EXIT_FAILURE == devComOpen(consts->i2c, &params->target))
    {
        return EXIT_FAILURE;
    }
    devComSend(consts->i2c, &registry, sizeof(registry));
    devComSend(consts->i2c, array, length);
    devComClose(consts->i2c, &params->target);

    return EXIT_SUCCESS;
}

static uint8_t writeReg(tsProcess *process, uint8_t registry, uint8_t data)
{
    tsBu16501ks2CpxParams *params       = process->parameters;
    const tsBu16501ks2CpxConsts *consts = process->constants;

    if (!devComTicketValid(consts->i2c, params->ticket))
    {
        return EXIT_FAILURE;
    }
    if (EXIT_FAILURE == devComOpen(consts->i2c, &params->target))
    {
        return EXIT_FAILURE;
    }
    devComSend(consts->i2c, &registry, sizeof(registry));
    devComSend(consts->i2c, &data, sizeof(data));
    devComClose(consts->i2c, &params->target);

    return EXIT_SUCCESS;
}

PT_THREAD(bu16501ks2Thread)
{
    tsBu16501ks2CpxParams *params       = process->parameters;
    const tsBu16501ks2CpxConsts *consts = process->constants;

    PT_BEGIN();

    // initCombo
    for (params->idx = 0; params->idx < ARRAY_SIZE(initCombo); params->idx++)
    {
        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(EXIT_FAILURE == writeReg(process, initCombo[params->idx].registry, initCombo[params->idx].data),
                                          10, 100)
        {
            // failed to open, send, close
            continue; // WARNING! we'll be taking a new ticket
        }
    }
    process->initialized = 1; // If process needs other checks, set this another time

    // update
    for (;;)
    {
        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(EXIT_SUCCESS == writeReg(process, Adr_PWMDuty, params->brightness & PWM_DUTY_MASK), // 0x20 0x3f
                                          10, 100)
        {
            // failed to open, send, close
            continue; // WARNING! we'll be taking a new ticket
        }

        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(EXIT_SUCCESS == writeReg(process, Adr_RegMap, Data_PattMap), // 0x7f 0x01
                                          10, 100)
        {
            // failed to open, send, close
            continue; // WARNING! we'll be taking a new ticket
        }

        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(EXIT_SUCCESS == writeRegArr(process, 0x01, consts->map, BU_SW_CNT * BU_LED_CNT / BU_LED_PER_REG),
                                          10, 100)
        {
            // failed to open, send, close
            continue; // WARNING! we'll be taking a new ticket
        }

        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(EXIT_SUCCESS == writeReg(process, Adr_RegMap, Data_CtrlMap), // 0x7f 0x00
                                          10, 100)
        {
            // failed to open, send, close
            continue; // WARNING! we'll be taking a new ticket
        }

        params->ticket = devComTicketGet(consts->i2c);
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(EXIT_SUCCESS == writeReg(process, Adr_MatrixCtrl, Data_MatrixON), // 0x30 0x01
                                          10, 100)
        {
            // failed to open, send, close
            continue; // WARNING! we'll be taking a new ticket
        }

        PT_YIELD_TIMEOUT(100);
    }

    PT_END();
}

static DEV_IO_FUNC_GET(bu16501ks2PxGet);
static DEV_IO_FUNC_PUT(bu16501ks2PxPut);

/// @brief  Structure that defines the functions for bu16501ks2 Pixel
const tsDevIoFuncs devIoBu16501ks2PxFuncs =
    {
        NULL,
        NULL,
        bu16501ks2PxGet,
        bu16501ks2PxPut,
};

/// @brief  Get function for a bu16501ks2 Pixel
static DEV_IO_FUNC_GET(bu16501ks2PxGet)
{
    tsBu16501ks2PxParams *params = device->parameters;

    return params->value;
}

/// @brief  Put function for a bu16501ks2 Pixel
static DEV_IO_FUNC_PUT(bu16501ks2PxPut)
{
    tsBu16501ks2PxParams *params           = device->parameters;
    const tsBu16501ks2CpxConsts *constsCpx = params->core->constants;
    // uint32_t diff                    = data ^ params->value;

    params->value = data;

    for (int idx = 0; idx < params->listCnt; idx++)
    {
        // if (diff & 0x01)
        {
            // There's a change
            uint8_t led, sw, mdx, shift;

            led   = params->list[idx] >> 4;                                       // High nibble is LED coordinate
            sw    = params->list[idx] & 0x0f;                                     // Low nibble is SW coordinate
            mdx   = (led * (BU_SW_CNT / BU_LED_PER_REG)) + (sw / BU_LED_PER_REG); // Map index
            shift = (sw % BU_LED_PER_REG) * (8 / BU_LED_PER_REG);                 // Shift in registry

            constsCpx->map[mdx] &= ~((BIT(8 / BU_LED_PER_REG) - 1) << shift);
            if (data & 0x01)
            {
                constsCpx->map[mdx] |= (params->current & (BIT(8 / BU_LED_PER_REG) - 1)) << shift;
            }
        }
        // diff >>= 1;
        data >>= 1;
    }

    return EXIT_SUCCESS;
}

static DEV_IO_FUNC_GET(bu16501ks2PxBrGet);
static DEV_IO_FUNC_PUT(bu16501ks2PxBrPut);

/// @brief  Structure that defines the functions for bu16501ks2 Pixel Brightness
const tsDevIoFuncs devIoBu16501ks2PxBrFuncs =
    {
        NULL,
        NULL,
        bu16501ks2PxBrGet,
        bu16501ks2PxBrPut,
};

/// @brief  Get function for a bu16501ks2 Pixel Brightness
static DEV_IO_FUNC_GET(bu16501ks2PxBrGet)
{
    const tsBu16501ks2PxBrConsts *consts = device->constants;
    tsBu16501ks2PxParams *pxParams       = consts->mainDev->parameters;

    return pxParams->current;
}

/// @brief  Put function for a bu16501ks2 Pixel Brightness
static DEV_IO_FUNC_PUT(bu16501ks2PxBrPut)
{
    const tsBu16501ks2PxBrConsts *consts = device->constants;
    tsBu16501ks2PxParams *pxParams       = consts->mainDev->parameters;

    pxParams->current = data & (BIT(8 / BU_LED_PER_REG) - 1);

    return devIoPut(consts->mainDev, devIoGet(consts->mainDev));
}

/** @} */

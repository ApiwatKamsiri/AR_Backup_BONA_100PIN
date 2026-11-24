/** @file       m24c04.h
 *  @brief      Header file of M24C04 device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Vehbi Umur Cabuk
 *  @date       8.10.2020
 */
#ifndef FILE_M24C04_H
#define FILE_M24C04_H

/// Includes
#include "rcos.h"

/// Interface definition for this file,
/// there should not be any includes after this define.
#ifndef FILE_M24C04_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/**
 *  @defgroup   M24C04 M24C04
 *  @ingroup    DEV_MEM
 *  @brief      I2C Memory Device
 *  @{
 */

/// @brief 24c04 eeprom size
#define EEPROM_24_C_04_PART_SIZE (256) // in bytes
#define EEPROM_24_C_04_PART1 (256)     // in bytes
#define EEPROM_24_C_04_PART2 (512)     // in bytes
#define EEPROM_24_C_04_SIZE (512)      // in bytes
#define EEPROM_24_C_04_ADDR_PART1 (0x50)
#define EEPROM_24_C_04_ADDR_PART2 (0x51)
#define EEPROM_24_C_04_READ_MASK (0x01)
#define EEPROM_24_C_04_PAGE_SIZE (0x10)   // in bytes
#define EEPROM_24_C_04_PAGE_MASK (0xFFF0) // in bytes
#define EEPROM_24_C_04_ADDR_A8(address) (((((address >> 8) & 0x01) ? 1 : 0) << 1))
#define EEPROM_24_C_04_ADDR_A7_A0(address) (address & 0x000000FF)
#define EEPROM_24_C_04_TMR_PERIOD_WRITING (5)
#define EEPROM_24_C_04_TMR_PERIOD_IDLE (100)

/// @brief  Functions for M24C04 devices
INTERFACE const tsDevMemFuncs devMemM24c04Funcs;
INTERFACE const tsDevIoFuncs devIoM24c04StatusFuncs;

/// @brief M24C04 process events
typedef enum
{
    eM24c04Idle = 1,
    eM24c04WriteCmd,
} tsM24c04Events;

/// @brief  Device specific parameters
typedef struct
{
    tTicket ticket;     ///< Ticket that will be used for SPI process queue
    tsTimerGeneric tge; ///< Timer Generic
    tsDeque *addrQueue; ///< Stores requested write address
    uint8_t ramCopy[EEPROM_24_C_04_SIZE];
    union
    {
        struct
        {
            uint8_t comErr : 1;
            uint8_t outOfQueueMemory : 1;
            uint8_t rsvd : 6;
        } bits;
        uint8_t byte;
    } regs;
} tsM24c04Params;

/// @brief  Device specific constants
typedef struct
{
    const tsDevCom *i2c; ///< Device connected communication device
    tsTarget *target;    ///< Device target information on communication device
    tsProcess *process;  ///< Process that handles the jobs for this device
} tsM24c04Consts;

/** @brief  Create a devMem
 *  @param  _name   Name of devMem object
 */
#define DEV_MEM_M24C04_CREATE(_name, _i2c, _e1, _e2)                         \
    tsProcess _name##Process;                                                \
    DEQUE_CREATE(_name##Deque, 100)                                          \
    tsTarget _name##Target = {                                               \
        .regAdrSize = 1,                                                     \
        .slave = EEPROM_24_C_04_ADDR_PART1,                                  \
    };                                                                       \
    tsM24c04Params _name##Params =                                           \
        {                                                                    \
            .ticket = -1,                                                    \
            .tge = TIMER_GENERIC_INIT(),                                     \
            .addrQueue = &_name##Deque,                                      \
            .ramCopy = {0x00},                                               \
            .regs = 0x00,                                                    \
    };                                                                       \
    const tsM24c04Consts _name##Consts =                                     \
        {                                                                    \
            .i2c = &_i2c,                                                    \
            .target = &_name##Target,                                        \
            .process = &_name##Process,                                      \
    };                                                                       \
    DEV_MEM_CREATE(_name, devMemM24c04Funcs, &_name##Params, &_name##Consts) \
    PROCESS_CREATE(_name##Process, m24c04ProInit, m24c04ProDeinit, PROCESS_NONE, &_name##Params, &_name)
/** @} */

/// @brief  Device specific constants
typedef struct
{
    const tsDevMem *mem;
} tsM24c04StatusConsts;

/** @brief  Create a devMem
 *  @param  _name   Name of devMem object
 */
#define DEV_IO_MC24C04_STATUS_CREATE(_name, _mainDevice) \
    const tsM24c04StatusConsts _name##Consts =           \
        {                                                \
            .mem = &_mainDevice,                         \
    };                                                   \
    DEV_IO_CREATE(_name, devIoM24c04StatusFuncs, NULL, &_name##Consts)

#undef INTERFACE // Should not let this roam free

#endif // FILE_M24C04_H

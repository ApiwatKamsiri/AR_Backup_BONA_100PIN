/** @file       otaprocess.h
 *  @brief      ota & listBoards
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     mertGemalmaz
 *  @date       20.12.2019
 */
#ifndef FILE_OTAPROCESS_H
#define FILE_OTAPROCESS_H

/**
 *  @defgroup   OTAPROCESS OTAPROCESS
 *  @ingroup    APP
 *  @brief      ota & listBoards
 *  @{
 */

/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"

/*********************************************************************
 *  EXPORTED TYPEDEFS
 */
#define PACK_TYPE_OTA 0x30

typedef enum
{
    OTA_CMD_LIST_BOARDS             = 0x10,
    OTA_CMD_OTA_TRANSFER            = 0x20,
    OTA_CMD_OTA_INSTALL             = 0x30,
    OTA_CMD_BOARD_LIST              = 0x15,
    OTA_CMD_OTA_TRANSFER_COMPLATED  = 0x25,
    OTA_CMD_OTA_TRANSFER_FAILED     = 0x2E,
    OTA_CMD_OTA_INSTALL_STARTED     = 0x35,
    OTA_CMD_OTA_INSTALL_FINISHED    = 0x36,
    OTA_CMD_OTA_INSTALL_FAILED      = 0x3E,
    OTA_CMD_OTA_JUMP_TO_APPLICATION = 0x40,
    OTA_CMD_OTA_ENTER_BOOTLOADER    = 0x50,
} teOtaCommands;

#define NUMBER_OF_BOARDS 2 // display board, dummyMainboard  TODO:add TFT board --> how???????

typedef struct
{
    unsigned char major;
    unsigned char minor;
    unsigned char build;
} st_swVersion;

typedef struct
{
    unsigned char major;
    unsigned char minor;
} st_hwID;

typedef struct PLATFORM_PACKED
{
    unsigned char typeID;
    st_swVersion version;
    st_hwID hardwareID;
    unsigned char otaAvailability;
} st_boardList;

typedef struct PLATFORM_PACKED
{
    unsigned char numberOfBoards;
    st_boardList listOfBoards[NUMBER_OF_BOARDS];
} st_boardListPackage;

typedef struct PLATFORM_PACKED
{
    uint8_t major;
    uint8_t minor;
    uint8_t build;
} tsVersionOta;

typedef enum
{
    otaProcessSet = 0x00,
    otaProcessSync,
    otaProcessSyncBit,
} teOtaProcessCheck;

typedef struct PLATFORM_PACKED
{
    teOtaProcessCheck command;
    uint8_t *address;
    uint8_t length;
    uint8_t *addressWrite;
} tsOtaCheckMap;

/// @brief  Parameters of a OtaProcess process
typedef struct
{
    const tsOtaCheckMap *otaCheckMap;
    uint8_t mapSize;
    // st_boardListPackage boardListPackage;
    uint8_t changeAttMaxFrameSizeProcess;
    uint8_t changeAttMaxFrameSizeEvent;
    //    const tsVersionOta *applianceVersion;
    uint8_t versionMajor;
    uint8_t versionMinor;
    uint8_t versionBuild;
    uint8_t versionHw;
    uint8_t extVersionMajor;
    uint8_t extVersionMinor;
    uint8_t extVersionBuild;
    tsTimerEvent jumpToBoot;
} tsOtaProcessParams;

/// @brief  Events of a OtaProcess process
typedef enum
{
    eListOfBoardsRecv,
    eListOfBoardsSend,
    eEnterBootloader,
    eJumpToBootloader,
    eInitListOfBoards,
} teOtaProcessEvents;

/*********************************************************************
 *  EXPORTED MACROS
 */
/** @brief  OtaProcess process object creation macro
 *  @param  _name   Name of process object
 *  @param  _enum   Process enumeration for this object
 */
#define PROCESS_OTAPROCESS_CREATE(_name, _enum, _otaCheckMap, _maxFrameSizeProcess, _maxFrameSizeEvent, \
                                  _versionMajor, _versionMinor, _versionBuild, _versionHw,              \
                                  _extVersionMajor, _extVersionMinor, _extVersionBuild)                 \
    tsOtaProcessParams _name##Params =                                                                  \
        {                                                                                               \
            _otaCheckMap,                                                                               \
            ARRAY_SIZE(_otaCheckMap),                                                                   \
            _maxFrameSizeProcess,                                                                       \
            _maxFrameSizeEvent,                                                                         \
            _versionMajor,                                                                              \
            _versionMinor,                                                                              \
            _versionBuild,                                                                              \
            _versionHw,                                                                                 \
            _extVersionMajor,                                                                          \
            _extVersionMinor,                                                                          \
            _extVersionBuild,                                                                          \
            TIMER_EVENT_INIT(_enum, _enum, eJumpToBootloader),                                          \
    };                                                                                                  \
    PROCESS_CREATE(_name, otaProcessInit, otaProcessDeinit, _enum, &_name##Params, NULL)

/*********************************************************************
 *  INTERFACES
 */

#ifndef FILE_OTAPROCESS_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

/*********************************************************************
 *  INTERFACES: CONSTANTS
 */

/*********************************************************************
 *  INTERFACES: VARIABLES
 */
extern st_boardListPackage boardListPackage;

/*********************************************************************
 *  INTERFACES: FUNCTIONS
 */

#undef INTERFACE // Should not let this roam free

/** @} */

#endif // FILE_OTAPROCESS_H

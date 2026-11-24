/** @file       otaprocess.c
 *  @brief      ota & listBoards
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     mertGemalmaz
 *  @date       20.12.2019
 */
#define FILE_OTAPROCESS_C

/**
 *  @addtogroup OTAPROCESS
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "rcos.h"
#include "otaprocess.h"
#include "app\refrigerator\rf_ram.h"
#include "app\refrigerator\rf_configs.h"
#include "../../config/metadata.h"
#include "../app/ui/components/ui_display.h"

/*********************************************************************
 *  LOCAL CONSTANTS
 */

/*********************************************************************
 *  LOCAL TYPEDEFS
 */
#define ADRESS_CHECKSUM_FLASH 0x3ffc0
typedef enum
{
    OTA_APPLIANCE_CARD_TYPE_UNKNOWN = 0,
    OTA_APPLIANCE_CARD_TYPE_MB,
    OTA_APPLIANCE_CARD_TYPE_DB,
    OTA_APPLIANCE_CARD_TYPE_WIFI,
    OTA_APPLIANCE_CARD_TYPE_BLE,
    OTA_APPLIANCE_CARD_TYPE_DB_NO_FLASH,
    OTA_APPLIANCE_CARD_TYPE_TFT,
    OTA_APPLIANCE_CARD_TYPE_TFT_EXT_FLASH,
    OTA_APPLIANCE_CARD_TYPE_MB_W_CONN = 11,
} enum_boardTypeID;

typedef enum
{
    OTA_DISABLED,
    OTA_ENABLED,
}enum_otaAvailability;

//TODO: version of the SW should be moved another location !!!!!!
#define _MAJOR_		0
#define _VERSION_	1
#define _REVISION_	0

#define OTA_FRAME_SIZE 266
#define OTA_FLASH_METADATA_ROW_NUM 0x3FF //0x3FE as?
#define MCU_ROW_SIZE (0x100) // this is fixed.

#ifdef RCOS_PLATFORM_PSOC4
#define MCU_SECTOR_SIZE (0x100)
#elif defined RCOS_PLATFORM_RA2
#define MCU_SECTOR_SIZE (BSP_FEATURE_FLASH_LP_CF_BLOCK_SIZE) // deletion can only be made by block size; therefore, our code will write by block size.
#endif // add other types

#define MCU_SECTOR_ROW_COUNT (MCU_SECTOR_SIZE / MCU_ROW_SIZE)
#define FLASH_GET_CODE_BYTE(addr) (*((uint8_t *)(addr)))
/*********************************************************************
 *  LOCAL MACROS
 */

/*********************************************************************
 *  LOCAL VARIABLES
 */
st_boardListPackage boardListPackage;

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */
static PROCESS_HANDLER_PROTO(otaProcessHandler);
// void listOfBoardsInit(void);
void clearInternalFlashMetadata(void);
uint8_t isWakingUpFromOta(void);
void afterOtaSync(tsProcess *process);
uint32_t writeToFlashRow(uint32_t rowNum, const uint8_t rowData[]);
void swReset(void);
unsigned char isOTAexistingflag = 0;
/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
PROCESS_INIT_PROTO(otaProcessInit)
{
    tsOtaProcessParams *params = process->parameters;
    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, otaProcessHandler);
    process->initialized = 1; // If process needs other checks, set this another time
#if (BOOTLOADER_AVAILABLE == COMP_AVAILABLE)
    if(isWakingUpFromOta())
    {
        afterOtaSync(process);
    }
#endif
    // listOfBoardsInit();
    eventPost(process->enumeration, eInitListOfBoards, NULL, 0); //init comes from wfaProcess
}

/* OTA Control Address&Variables -> Their location is fixed in Bootloader. This bootloader will be alive inside the machines sold to the market. Do not modify */
#define AFTER_OTA_APP_STATUS_ADDR_IDX (0x00)      // Status changes to LOADED after a successful OTA
#define AFTER_OTA_APP_EM_CHECKSUM_ADDR_IDX (0x08) // Calculated checksum of the loaded software via OTA
// #define AFTER_OTA_APP_FIRST_ROW_NUM_ADDR_IDX (0x10) // No need to add this control, because the beginning address of the app may change in the future for new control systems.
#define OTA_COMPLETED_SIGNATURE_ADDR_IDX (0xff) // Any data here means a successful OTA is just completed
#define AFTER_OTA_APP_STATUS_LOADED (0x4C)
/* OTA Control Address&Variables - end */

uint8_t isWakingUpFromOta(void)
{
#if (BOOTLOADER_AVAILABLE == COMP_AVAILABLE)
  
    t_st_metadata tempMetaData;
    //memset(tempData, 0, sizeof(tempData));
    // as? tempData[MCU_ROW_SIZE - 1] = 0x40;
    memcpy(&tempMetaData, &rf_app_meta, sizeof(tempMetaData));
    tempMetaData.is_ota_pending = 0x00;
    writeToFlashRow(OTA_FLASH_METADATA_ROW_NUM, (uint8_t *)&tempMetaData); // clear previous metadata section for new Ota
    
    /*
    uint8_t tempData[MCU_ROW_SIZE];

    memcpy(tempData, (uint8_t *)(OTA_FLASH_METADATA_ROW_NUM * (MCU_ROW_SIZE)), MCU_ROW_SIZE);

    if (((tempData[AFTER_OTA_APP_STATUS_ADDR_IDX] == AFTER_OTA_APP_STATUS_LOADED) &&
         // (tempData[AFTER_OTA_APP_FIRST_ROW_NUM_ADDR_IDX] == 0x4C) &&
         // (tempData[AFTER_OTA_APP_FIRST_ROW_NUM_ADDR_IDX + 1] == 0x00) &&
         (tempData[AFTER_OTA_APP_EM_CHECKSUM_ADDR_IDX] == FLASH_GET_CODE_BYTE(ADRESS_CHECKSUM_FLASH))) ||
        (tempData[OTA_COMPLETED_SIGNATURE_ADDR_IDX] != 0x00))
    {
        tempData[AFTER_OTA_APP_STATUS_ADDR_IDX] = 0x00;
        // tempData[AFTER_OTA_APP_FIRST_ROW_NUM_ADDR_IDX] = 0x00;
        // tempData[AFTER_OTA_APP_FIRST_ROW_NUM_ADDR_IDX + 1] = 0x00;
        tempData[AFTER_OTA_APP_EM_CHECKSUM_ADDR_IDX] = 0x00;
        tempData[OTA_COMPLETED_SIGNATURE_ADDR_IDX] = 0x00;
    }
    else
        return 0;
    writeToFlashRow(OTA_FLASH_METADATA_ROW_NUM, tempData);
*/

#endif
    return 1;
}


uint32_t writeToFlashRow(uint32_t rowNum, const uint8_t rowData[])
{
#ifdef RCOS_PLATFORM_PSOC4
    return CySysFlashWriteRow(rowNum, rowData);

#elif defined RCOS_PLATFORM_RA2
    static uint8_t bufferToWrite[MCU_SECTOR_SIZE];  // it can be moved to stack, but it requires a BIG size of stack.

    fsp_err_t err;

    globalIntDisable();
    
#ifdef DATA_FLASH_CGC_ACTIVE
    checkClockBeforeFlashOperation(params->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif

    err = R_FLASH_LP_Open(EEPROM.p_ctrl, EEPROM.p_cfg); 
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    if (FSP_SUCCESS != err)
    {
#ifdef DATA_FLASH_CGC_ACTIVE
        checkClockAfterFlashOperation(params->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif
        globalIntEnable();
        return EXIT_FAILURE;
    }
    
#ifdef DATA_FLASH_CGC_ACTIVE
    checkClockAfterFlashOperation(params->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif

    globalIntEnable();
    // end of open
    
    fsp_err_t result;
    
    uint32_t sectorBgn = (rowNum - rowNum % MCU_SECTOR_ROW_COUNT) << 8;
    memcpy(bufferToWrite, (void *)sectorBgn, MCU_SECTOR_SIZE);                // Get current sector data
    memcpy(&bufferToWrite[(rowNum << 8) - sectorBgn], rowData, MCU_ROW_SIZE); // Overwrite new data

    globalIntDisable();
    result = R_FLASH_LP_Erase(EEPROM.p_ctrl, sectorBgn, 1); // Erase sector
    globalIntEnable();
    if (result != FSP_SUCCESS)
    {
        return 0;
    }

    globalIntDisable();
    result = R_FLASH_LP_Write(EEPROM.p_ctrl, (uint32_t)bufferToWrite, sectorBgn, MCU_SECTOR_SIZE);
    globalIntEnable();
    if (result != FSP_SUCCESS)
    {
        return 0;
    }

    return FSP_SUCCESS;
#endif // add other types
}


void swReset(void)
{
#ifdef RCOS_PLATFORM_PSOC4
    Bootloadable_1_Load(); // CySoftwareReset(); // -> jumps to bootloader.
#else
    NVIC_SystemReset();
#endif // add other types
}

void afterOtaSync(tsProcess *process)
{
    tsOtaProcessParams *params = process->parameters;
    uint8_t compData[4] = {0};

    for (uint8_t i = 0; i < params->mapSize; i++)
    {
        switch (params->otaCheckMap[i].command)
        {
        case otaProcessSet:
        {
            memcpy(params->otaCheckMap[i].address, params->otaCheckMap[i].addressWrite, params->otaCheckMap[i].length);
        }
        break;

        case otaProcessSync:
        {
            if (memcmp(params->otaCheckMap[i].address, compData, params->otaCheckMap[i].length) == 0)
            {
                memcpy(params->otaCheckMap[i].address, params->otaCheckMap[i].addressWrite, params->otaCheckMap[i].length);
            }
        }
        break;

        case otaProcessSyncBit:
        {
            memcpy(compData, params->otaCheckMap[i].address, params->otaCheckMap[i].length);
            memcpy(compData + 2, params->otaCheckMap[i].addressWrite, params->otaCheckMap[i].length);

            if ((compData[0] & compData[2]) == 0)
            {
                memcpy(params->otaCheckMap[i].address, params->otaCheckMap[i].addressWrite, params->otaCheckMap[i].length);
            }
        }
        break;
        }
    }
}

PROCESS_DEINIT_PROTO(otaProcessDeinit)
{
    tsOtaProcessParams *params = process->parameters;
    UNUSED(params); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0; // If process needs other checks, clear this another time
}

static PROCESS_HANDLER_PROTO(otaProcessHandler)
{
    tsOtaProcessParams *params = process->parameters;

    UNUSED(params); // REMOVE IF USED

    switch (eventCurrent.event)
    {
        case eInitListOfBoards:
        {
            boardListPackage.numberOfBoards = NUMBER_OF_BOARDS;

            boardListPackage.listOfBoards[0].typeID           = OTA_APPLIANCE_CARD_TYPE_MB;
            boardListPackage.listOfBoards[0].otaAvailability  = OTA_DISABLED;
            boardListPackage.listOfBoards[0].hardwareID.major = params->versionHw; // gerber
            boardListPackage.listOfBoards[0].hardwareID.minor = 1; // bom
            boardListPackage.listOfBoards[0].version.major    = params->versionMajor;
            boardListPackage.listOfBoards[0].version.minor    = params->versionMinor;
            boardListPackage.listOfBoards[0].version.build    = params->versionBuild;

            
            boardListPackage.listOfBoards[1].typeID           = OTA_APPLIANCE_CARD_TYPE_MB_W_CONN;
            boardListPackage.listOfBoards[1].otaAvailability  = OTA_ENABLED;
            boardListPackage.listOfBoards[1].hardwareID.major = params->versionHw; // gerber
            boardListPackage.listOfBoards[1].hardwareID.minor = 1; // bom
            boardListPackage.listOfBoards[1].version.major    = params->extVersionMajor;
            boardListPackage.listOfBoards[1].version.minor    = refrigerator_F.Stock.Version;
            boardListPackage.listOfBoards[1].version.build    = refrigerator_F.Stock.Revision;
        }
        break;
        case eListOfBoardsRecv:
        {
            eventReply(eListOfBoardsSend, &boardListPackage, sizeof(boardListPackage));
        }
        break;

#if (BOOTLOADER_AVAILABLE == COMP_AVAILABLE)
        case eEnterBootloader:
        {
            uint16_t otaFrameSize;
            if(isOTAexistingflag==2) // if ota come then show animation display,we can continue flashing 
            {                           // if this flag equal to 2 , that means animation started
              otaFrameSize = OTA_FRAME_SIZE;
              clearInternalFlashMetadata();
              swReset(); // Bootloadable_1_Load(); //CySoftwareReset();
              eventPost(params->changeAttMaxFrameSizeProcess, params->changeAttMaxFrameSizeEvent, &otaFrameSize, sizeof(otaFrameSize));
              timerEventStart(&params->jumpToBoot, 1); //gdm Tx
            }
            
            isOTAexistingflag= 1; // make this when ota come
            
        }
        break;

        case eJumpToBootloader:
        {
            swReset(); //Reset and jump to bootloader
        }
        break;
#endif

        default:
            break;
    }
}

#if (BOOTLOADER_AVAILABLE == COMP_AVAILABLE)
void clearInternalFlashMetadata(void)
{
    t_st_metadata tempMetaData;
    //memset(tempData, 0, sizeof(tempData));
    // as? tempData[MCU_ROW_SIZE - 1] = 0x40;
    memcpy(&tempMetaData, &rf_app_meta, sizeof(tempMetaData));
    tempMetaData.is_ota_pending = 0x40;
    writeToFlashRow(OTA_FLASH_METADATA_ROW_NUM, (uint8_t *)&tempMetaData); // clear previous metadata section for new Ota
}
#endif
//void listOfBoardsInit(void)
//{
//     tsOtaProcessParams *params = process->parameters;
//
//    boardListPackage.numberOfBoards = NUMBER_OF_BOARDS;
//
//    boardListPackage.listOfBoards[0].typeID = OTA_APPLIANCE_CARD_TYPE_DB_NO_FLASH;//OTA_APPLIANCE_CARD_TYPE_DB_NO_FLASH;
//    boardListPackage.listOfBoards[0].otaAvailability = OTA_ENABLED;
//    boardListPackage.listOfBoards[0].hardwareID.major = 0x05; // gerber
//    boardListPackage.listOfBoards[0].hardwareID.minor = 0x06; // bom
//    boardListPackage.listOfBoards[0].version.major = params->versionMajor;
//    boardListPackage.listOfBoards[0].version.minor = params->versionMinor;
//    boardListPackage.listOfBoards[0].version.build = params->versionMinor;
//
//    boardListPackage.listOfBoards[1].typeID = OTA_APPLIANCE_CARD_TYPE_MB;
//    boardListPackage.listOfBoards[1].otaAvailability = OTA_DISABLED;
//    boardListPackage.listOfBoards[1].hardwareID.major = 0x29; // gerber
//    boardListPackage.listOfBoards[1].hardwareID.minor = 0x30; // bom
//    boardListPackage.listOfBoards[1].version.major = 0x31;    // from wifi array
//    boardListPackage.listOfBoards[1].version.minor = 0x32;    // from wifi array
//    boardListPackage.listOfBoards[1].version.build = 0x33;    // from wifi array
//}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @} */

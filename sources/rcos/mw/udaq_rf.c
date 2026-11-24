/** @file       udaq.c
 *  @brief      ARCELIK Universal Data Acquisition & Control
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ovunc Doruk Izgen
 *  @author     Deniz Fer
 *  @date       14.03.2019
 */
#define FILE_UDAQ_C

/**
 *  @addtogroup UDAQ
 *  @{
 */
/*********************************************************************
 *  INCLUDES
 */
#include "udaq_rf.h"

/*********************************************************************
 *  LOCAL CONSTANTS
 */

#define UDAQ_READ_COM_PERIOD (10)
#define UDAQ_PACKAGE_TIMEOUT (5000)

#define COMMAND_ENCRYPT_FLAG BIT(3)
#define REPLY_ACK (0xAA)
#define REPLY_NACK (0x5A)

enum
{
    eCaptureCommand,
    eCaptureHeader
};

enum
{
    eCommandBoundaryLow,
    eCommandGetInfo,
    eCommandRead,
    eCommandWrite,
    eCommandPublicRead,
    eCommandGetKey,

    eCommandGetRcosVersions,
    eCommandGetVersions,
    
    eCommandGetInfoSub = 65,
    eCommandReadSub,
    eCommandWriteSub,
    eCommandPublicReadSub,
    eCommandGetKeySub,

    eCommandBoundaryHigh,

    eCommandEncBoundaryLow = 127,
    eCommandRomRead,
    eCommandSetMode,
    eCommandRamRead,
    eCommandRamWrite,
    eCommandEepRead,
    eCommandEepWrite,
    eCommandTechRead,
    eCommandTechWrite,
    eCommandTechArrRead,

    eCommandEncBoundaryHigh
};

enum
{
    eInfoArraySize,
    eInfoMachineType,
    eInfoModelNo,
    eInfoUnique,
    eInfoRomVer,
    eInfoEepVer
};

/*********************************************************************
 *  LOCAL TYPEDEFS
 */

/*********************************************************************
 *  LOCAL MACROS
 */
#define NIBBLE_CHANGE(_byte) (((uint8_t)(_byte) << 4) | ((uint8_t)(_byte) >> 4))
#define DECODE_BYTE(_byte) (NIBBLE_CHANGE(_byte) ^ ((consts->cryptoKey & NIBBLE_CHANGE(~COMMAND_ENCRYPT_FLAG))))

/*********************************************************************
 *  LOCAL VARIABLES
 */

/*********************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 */
PROCESS_HANDLER_PROTO(udaqHandler);
static uint8_t checksumIsCorrect(tsUdaqPkg *package);
static PT_THREAD(commandParser);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */
static PT_THREAD(udaqOperation)
{
    tsUdaqParams *params       = process->parameters;
    const tsUdaqConsts *consts = process->constants;
    tsUdaqPkg *package         = &params->incoming;
    uint8_t failed             = 0;

    PT_BEGIN();

    // Continue to receive new packages
    for (;;)
    {
        // Capture command
        for (;;)
        {
            PT_WAIT_UNTIL_W_PERIOD(devComReceive(consts->com, package, sizeof(package->command)) == sizeof(package->command),
                                   UDAQ_READ_COM_PERIOD);

            if (package->command & COMMAND_ENCRYPT_FLAG)
            {
                uint8_t check = DECODE_BYTE(package->command);

                // Check if it is a valid encrypted command
                if ((check > eCommandEncBoundaryLow) &&
                    (check < eCommandEncBoundaryHigh))
                {
                    break; // out of capture command
                }
            }
            else
            {
                // Check if it is a valid command
                if ((package->command > eCommandBoundaryLow) &&
                    (package->command < eCommandBoundaryHigh))
                {
                    break; // out of capture command
                }
            }
        }

        // Capture rest of the package
        PT_WAIT_UNTIL_W_PERIOD_IF_TIMEOUT(devComReceive(consts->com, &package->address, sizeof(tsUdaqPkg) - sizeof(package->command)) == (sizeof(tsUdaqPkg) - sizeof(package->command)),
                                          UDAQ_READ_COM_PERIOD,
                                          UDAQ_PACKAGE_TIMEOUT)
        {
            // Rest of package is missing
            failed = 1;
        }

        if (!failed)
        {
            // Decode package
            if (package->command & COMMAND_ENCRYPT_FLAG)
            {
                uint8_t idx, *ptr;

                ptr = (void *)package;
                for (idx = 0; idx < sizeof(tsUdaqPkg); idx++)
                {
                    *ptr = DECODE_BYTE(*ptr);
                    ptr++;
                }
            }

            // Checksum
            if (checksumIsCorrect(package))
            {
                package->address = ntohs(package->address);
                PT_SPAWN(&params->child, commandParser);
            }
        }
    }

    PT_END();
}

PROCESS_INIT_PROTO(udaqInit)
{
    tsUdaqParams *params       = process->parameters;
    const tsUdaqConsts *consts = process->constants;
    const tsUdaqItem *array;
    const tsUdaqItem *arraySub;
    const tsUdaqItem *arrayFTC;
    uint8_t idx;

    ADD_RCOS_VERSION("udaq", "2.0.0")

    PROCESS_STATE_CHANGE(process, udaqHandler);

    params->totalSize = 0;
    array             = consts->itemArray;
    for (idx = 0; idx < consts->itemArraySize; array++, idx++)
    {
        params->totalSize += array->size;
    }

    params->totalSizeSub = 0;
    arraySub             = consts->itemArraySub;
    for (idx = 0; idx < consts->itemArraySizeSub; arraySub++, idx++)
    {
        params->totalSizeSub += arraySub->size;
    }

    params->totalSizeFTC = 0;
    arrayFTC             = consts->itemArrayFTC;
    for(idx = 0; idx < consts->itemArraySizeFTC; arrayFTC++, idx++)
    {
        params->totalSizeFTC += arrayFTC->size;
    }

    devComInit(consts->com);
    threadStart(process, udaqOperation);

    process->initialized = 1;
}

PROCESS_DEINIT_PROTO(udaqDeinit)
{
    tsUdaqParams *params = process->parameters;

    UNUSED(params); // REMOVE IF USED
    threadStop(process, udaqOperation);
    process->initialized = 0;
    PROCESS_STATE_CHANGE(process, NULL);
}

PROCESS_HANDLER_PROTO(udaqHandler)
{
    UNUSED(process);
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */
static uint8_t *memoryAddrFrom(const tsUdaqItem *array, uint8_t arrSize, uint16_t offset)
{
    uint8_t idx;

    for (idx = 0; idx < arrSize; array++, idx++)
    {
        if (offset >= array->size)
        {
            offset -= array->size;
        }
        else
        {
            break;
        }
    }

    return ((uint8_t *)array->address) + offset;
}

/** @brief  Checksum calculation result
 *  @retval TRUE checksum is correct
 *  @retval FALSE checksum is incorrect
 */
static uint8_t checksumIsCorrect(tsUdaqPkg *package)
{
    uint8_t result;

    result = package->command;
    result += package->address >> 8;
    result += package->address & 0xff;
    result += package->data;
    result += package->checksum + 1;

    if(package->command & 0x40)
        result -= 0x40;
    
    if(package->command & 0x20)
        result -= 0x20;

    return (result == 0);
}

/// @brief  Send a single byte response
static void replyWithByte(tsProcess *process, uint8_t txData)
{
    const tsUdaqConsts *consts = process->constants;
    uint8_t checksum           = 255;

    checksum -= txData;
    devComSend(consts->com, &txData, sizeof(txData));

    devComSend(consts->com, &checksum, sizeof(checksum));
}

static void replyWithArray(tsProcess *process, const tsUdaqItem *array, uint8_t arrSize)
{
    const tsUdaqConsts *consts = process->constants;
    uint8_t idx;
    uint8_t checksum = 255;

    for (idx = 0; idx < arrSize; array++, idx++)
    {
        uint8_t jdx;

        for (jdx = 0; jdx < array->size; jdx++)
        {
            checksum -= ((uint8_t *)array->address)[jdx];
        }
        devComSend(consts->com, array->address, array->size);
    }
    devComSend(consts->com, &checksum, sizeof(checksum));
}

static PT_THREAD(commandParser)
{
    tsUdaqParams *params       = process->parameters;
    const tsUdaqConsts *consts = process->constants;
    tsUdaqPkg *package         = &params->incoming;

    PT_BEGIN();

    if (package->command == eCommandGetInfo)
    {
        switch (package->address) // Warning do not use PT macros inside a "switch/case"
        {
            case eInfoArraySize:
                replyWithByte(process, params->totalSize + sizeof(uint8_t)); // Size of checksum that's in replyWithArray
                break;
            case eInfoMachineType:
                replyWithByte(process, consts->machineType);
                break;
            case eInfoModelNo:
                replyWithByte(process, consts->modelNo);
                break;
            case eInfoUnique:
                replyWithByte(process, consts->modelId);
                break;
            case eInfoRomVer:
                replyWithByte(process, consts->romVer);
                break;
            case eInfoEepVer:
                replyWithByte(process, consts->eepVer);
                break;
            default:
                break;
        }
    }
    else if (package->command == eCommandRead)
    {
        if (package->address < params->totalSize)
        {
            replyWithByte(process, *memoryAddrFrom(consts->itemArray, consts->itemArraySize, package->address));
        }
    }
    else if (package->command == eCommandWrite)
    {
        package->address &= 0x00FF; // high byte a 1 yaziyordu ve adress yanlis oldugundan deger kontrol edilemiyordu. ��z�ld�.
        if (package->address < params->totalSize)
        {
            *memoryAddrFrom(consts->itemArray, consts->itemArraySize, package->address) = package->data;
            replyWithByte(process, REPLY_ACK);
        }
    }
    else if (package->command == eCommandPublicRead)
    {
        replyWithArray(process, consts->itemArray, consts->itemArraySize);
    }
    else if (package->command == eCommandGetKey)
    {
        replyWithByte(process, consts->cryptoKey);
    }
/***************************************************************************************************************/
    else if (package->command == eCommandGetInfoSub)
    {
        switch (package->address) // Warning do not use PT macros inside a "switch/case"
        {
            case eInfoArraySize:
                replyWithByte(process, params->totalSizeSub + sizeof(uint8_t)); // Size of checksum that's in replyWithArray
                break;
            case eInfoMachineType:
                replyWithByte(process, consts->machineType);
                break;
            case eInfoModelNo:
                replyWithByte(process, consts->modelNo);
                break;
            case eInfoUnique:
                replyWithByte(process, consts->modelIdSub);
                break;
            case eInfoRomVer:
                replyWithByte(process, consts->romVer);
                break;
            case eInfoEepVer:
                replyWithByte(process, consts->eepVer);
                break;
            default:
                break;
        }
    }
    else if (package->command == eCommandReadSub)
    {
        if (package->address < params->totalSizeSub)
        {
            replyWithByte(process, *memoryAddrFrom(consts->itemArraySub, consts->itemArraySizeSub, package->address));
        }
    }
    else if (package->command == eCommandWriteSub)
    {
        package->address &= 0x00FF; // high byte a 1 yaziyordu ve adress yanlis oldugundan deger kontrol edilemiyordu. ��z�ld�.
        if (package->address < params->totalSizeSub)
        {
            *memoryAddrFrom(consts->itemArraySub, consts->itemArraySizeSub, package->address) = package->data;
            replyWithByte(process, REPLY_ACK);
        }     
    }
    else if (package->command == eCommandPublicReadSub)
    {
        replyWithArray(process, consts->itemArraySub, consts->itemArraySizeSub);
    }
    else if (package->command == eCommandGetKeySub)
    {
        replyWithByte(process, consts->cryptoKey);
    }
/***************************************************************************************************************/
    else if (package->command == eCommandRomRead)
    {
        // Not supported
    }
    else if (package->command == eCommandSetMode)
    {
        // Not supported
    }
    else if (package->command == eCommandRamRead)
    {
        // replyWithByte(process, *(uint8_t*)package->address);
    }
    else if (package->command == eCommandRamWrite)
    {
        // *(uint8_t*)package->address = package->data;
        replyWithByte(process, REPLY_ACK);
    }
    else if (package->command == eCommandEepRead)
    {
        // Not supported
    }
    else if (package->command == eCommandEepWrite)
    {
        // Not supported
    }
    else if (package->command == eCommandTechRead)
    {
        if (package->address < params->totalSizeFTC)
        {
            replyWithByte(process, *memoryAddrFrom(consts->itemArrayFTC, consts->itemArraySizeFTC, package->address));
        }
    }
    else if (package->command == eCommandTechWrite)
    {
        package->address &= 0x00FF; // high byte a 1 yaziyordu ve adress yanlis oldugundan deger kontrol edilemiyordu. ��z�ld�.
        if (package->address < params->totalSizeFTC)
        {
            *memoryAddrFrom(consts->itemArrayFTC, consts->itemArraySizeFTC, package->address) = package->data;
            replyWithByte(process, REPLY_ACK);
        }    
    }
    else if (package->command == eCommandTechArrRead)
    {
        // replyWithArray(process, consts->itemArray, consts->itemArraySize);
    }
    else if ((package->command == eCommandGetVersions) ||
             (package->command == eCommandGetRcosVersions))
    {
        // print version list
        if (package->command == eCommandGetVersions)
        {
            // user component versions
            extern tsLsl userVersionList;
            params->verList = &userVersionList;
        }
        else
        {
            // rcos component versions
            extern tsLsl rcosVersionList;
            params->verList = &rcosVersionList;
        }

        if (params->verList)
        {
            LIST_SL_FOREACH(params->verItem, params->verList)
            {
                devComPrint(consts->com, params->verItem->versionInfo->compName);
                PT_WAIT_UNTIL_W_PERIOD(devComSend(consts->com, NULL, 0) == 0, 10);
                devComPrint(consts->com, "_");
                devComPrint(consts->com, params->verItem->versionInfo->compVersion);
                PT_WAIT_UNTIL_W_PERIOD(devComSend(consts->com, NULL, 0) == 0, 10);
                devComPrint(consts->com, "\n");
            }
        }
    }
    else
    {
        // Unknown command
        replyWithByte(process, REPLY_NACK);
    }

    PT_END();
}

/** @} */

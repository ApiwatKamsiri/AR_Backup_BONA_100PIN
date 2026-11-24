/*
 *  @author     Cihan YILDIRIM
 *  @date       14.06.2023
*/

#include "rcos.h"
#include "rf_backup.h"
#include "board/board.h"
//#include "sh32f9xx_sb0_libcfg.h"
#include "app/refrigerator/rf_ram.h"

#define DATA_FLASH_INIT_ADDR 0x0
unsigned char ReadenCRC = 0;
static PROCESS_HANDLER_PROTO(rfBackupProcessHandler);

static uint8_t flashCrcCalculator(uint8_t *frame, uint16_t length)
{
    uint8_t retVal;
    uint16_t idx;

    retVal = crc8MaximInit();

    for (idx = 0; idx < length; idx++)
    {
        retVal = crc8MaximAddData(retVal, frame[idx]);
    }
    retVal = crc8MaximFinish(retVal);

    return retVal;
}

static uint8_t checkPreambleBytes(uint8_t *frame, uint8_t length)
{
    uint8_t retVal = 0;
    uint8_t preamble[4] = {0x53, 0x4C, 0x48, 0x2A}; //preamble bytes for controlling eeprom first write operation. do not change this bytes.

    if(length >= 3)
    {
        if((frame[0] == preamble[0]) && (frame[1] == preamble[1]) && (frame[2] == preamble[2]) && (frame[length-1] == preamble[3]))
        {
            retVal = 1;
        }
    }

    return retVal;
}

/// @brief  Initialization function of testProcess
PROCESS_INIT_PROTO(backupE2Init)
{
    tsE2Params *params       = process->parameters;
    const tsE2Consts *consts = process->constants;
    uint8_t isEepromValid    = 0;

    devMemInit(consts->devEeprom);

    /***********************/
    /*Read All Backup Array if there is valid record in flash*/

//    uint32_t StartAddr;
//    uint8_t e2psector, sectorNumIndex;
    uint16_t len = params->backupArrSize;
    tsBackupItem *bpArr = (tsBackupItem *)params->backupArr;
    uint16_t arrCnt, arrCnt2;
    uint16_t totalLength = 0;
    uint8_t paddingVal = 0;
    uint8_t crcVal;
    uint8_t tempSize;
    uint8_t arrRead[256] = 0;
    uint8_t readCrc = 0;
    for(arrCnt = 0; arrCnt < len; arrCnt++)
    {
        totalLength += bpArr[arrCnt].length;
    }
    tempSize = totalLength;
    //Padding to uint32_t
    paddingVal = (totalLength % 4);
    totalLength += (4 - paddingVal);
    //adding for crc data
    if(paddingVal)
    {
        totalLength += 4;
    }

    //uint8_t *arr = (uint8_t *)malloc(totalLength);
    
    memset(arrRead, 0, totalLength);

    //memcpy(arr, (const void *)(StartAddr), totalLength);
    isEepromValid = devMemRead(consts->devEeprom, DATA_FLASH_INIT_ADDR, arrRead, totalLength);


    crcVal = flashCrcCalculator(arrRead, (totalLength-4));

    //CRC tutuyor ise veriyi oku
    readCrc = arrRead[totalLength-4];
    ReadenCRC = readCrc;
    if((crcVal == readCrc) && (checkPreambleBytes(arrRead, tempSize) == 1))
    {
        for(arrCnt = 0, arrCnt2 = 0; arrCnt < len; arrCnt++)
        {
            memcpy((uint8_t *)bpArr[arrCnt].address, &arrRead[arrCnt2], bpArr[arrCnt].length);
            arrCnt2 += bpArr[arrCnt].length;
        }
    }

    timerEventStart(&params->tev, params->updatePeriod);
    PROCESS_STATE_CHANGE(process, rfBackupProcessHandler);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of testProcess
PROCESS_DEINIT_PROTO(backupE2Deinit)
{
    tsE2Params *params       = process->parameters;
    const tsE2Consts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);

    process->initialized = 0; // If process needs other checks, clear this another time
}

static PROCESS_HANDLER_PROTO(rfBackupProcessHandler)
{
    tsE2Params *params       = process->parameters;
    const tsE2Consts *consts = process->constants;
//    uint8_t isEepromValid    = 0;

    switch (eventCurrent.event)
    {
        case eBackupProcessEventPeriodic:
        {
            if(params->autoSave)
            {
                //Write fonksiyonu icinde ram ile flash karsilastirilip degisim varsa yazdiriliyor.
                eventPost(process->enumeration, eBackupProcessEventWriteAll, NULL, 0);
                timerEventStart(&params->tev, params->updatePeriod);
            }
        }
        break;
        case eBackupProcessEventWriteAll:
            uint16_t totalLength = 0;
            uint8_t paddingVal = 0;
            uint8_t crcVal;
            uint16_t len = params->backupArrSize;
            uint16_t arrCnt, arrCnt2, arrCnt3;
            tsBackupItem *bpArr = (tsBackupItem *)params->backupArr;
            uint8_t tempSize;
           
            for(arrCnt = 0; arrCnt < len; arrCnt++)
            {
                totalLength += bpArr[arrCnt].length;
            }
            tempSize = totalLength;
            //Padding to uint32_t
            paddingVal = (totalLength % 4);
            totalLength += (4 - paddingVal);
            //adding for crc data
            if(paddingVal)
            {
                totalLength += 4;
            }
            
            uint8_t arr[256] = 0;
            memset(arr, 0, totalLength);

            for(arrCnt = 0, arrCnt3 = 0, crcVal = 0; arrCnt < len; arrCnt++)
            {
                for(arrCnt2 = 0; arrCnt2 < bpArr[arrCnt].length; arrCnt2++, arrCnt3++)
                {
                    uint8_t *adr = (uint8_t *)bpArr[arrCnt].address;
                    arr[arrCnt3] = adr[arrCnt2];
                }
            }
            crcVal = flashCrcCalculator(arr, (totalLength-4));
            arr[totalLength-4] = crcVal;
            arrCnt = 0;

            // Burada Ram ve Flash Karsilastirmasi yapilacak
            //volatile uint32_t adrReadTemp;
            uint8_t tempReadVal;
            uint8_t say = 0;

            if(ReadenCRC != crcVal)
            {
                if(arrCnt < (totalLength-4))
                {
                    ReadenCRC = crcVal;
                    devMemWrite(consts->devEeprom, DATA_FLASH_INIT_ADDR, arr, totalLength);
                }
            }

    break;

        default:
            break;
    }
}

/** @} */
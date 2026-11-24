/** @file       SH32F9/emu_eeprom.c
 *  @brief      Emulator for an EEPROM
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Cihan YILDIRIM
 *  @date       14.06.2023
 */
#define FILE_EMU_EEPROM_C

/**
 *  @addtogroup SH32F9_EMU_EEPROM
 *  @{
 */
#include <stdio.h>
#include <stdlib.h>
#include "emu_eeprom.h"
#include "mw/backup.h"
#include "libs/crc.h"

static DEV_MEM_FUNC_INIT(emuEepromInit);
static DEV_MEM_FUNC_DEINIT(emuEepromDeinit);
static DEV_MEM_FUNC_READ(emuEepromRead);
static DEV_MEM_FUNC_WRITE(emuEepromWrite);
static DEV_MEM_FUNC_ERASE(emuEepromErase);

const tsDevMemFuncs devMemEmuEepromFuncs =
    {
        emuEepromInit,
        emuEepromDeinit,
        emuEepromRead,
        emuEepromWrite,
        emuEepromWrite, // Write function is capable of both
        emuEepromErase,
        NULL,
        NULL,
};

static DEV_MEM_FUNC_INIT(emuEepromInit)
{
    const tsEmuEepromConsts *consts = device->constants;
    ADD_RCOS_VERSION("SH32F9/emueeprom", "1.0.0")

    //consts->init(consts->startAddress);
    device->sys->initialized = 1;

    return EXIT_SUCCESS;
}

static DEV_MEM_FUNC_DEINIT(emuEepromDeinit)
{
    device->sys->initialized = 0;

    return EXIT_SUCCESS;
}



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
    uint8_t preamble[3] = {0x53, 0x4C, 0x48}; //preamble bytes for controlling eeprom first write operation. do not change this bytes.

    if(length >= 3)
    {
      if((frame[0] == preamble[0]) && (frame[1] == preamble[1]) && (frame[2] == preamble[2]))
      {
        retVal = 1;
      }
    }

    return retVal;
}

static DEV_MEM_FUNC_READ(emuEepromRead)
{
    const tsEmuEepromConsts *consts = device->constants;

    uint32_t StartAddr;
    uint8_t e2psector, sectorNumIndex;
    uint16_t len = length;
    tsBackupItem *bpArr = (tsBackupItem *)readData;
    uint16_t arrCnt, arrCnt2; 
    uint16_t totalLength = 0;
    uint8_t paddingVal = 0;
    uint8_t crcVal;

    sectorNumIndex = (E2_SECTOR_NUM_Type)(address / 1024);
    e2psector = consts->blockList[sectorNumIndex];
    StartAddr = address - (sectorNumIndex*1024);

    for(arrCnt = 0; arrCnt < len; arrCnt++)
    {
      totalLength += bpArr[arrCnt].length;
    }
    
    //Padding to uint32_t
    paddingVal = (totalLength % 4);
    totalLength += (4 - paddingVal);
    //adding for crc data
    if(paddingVal)
    {
      totalLength += 4;
    }

    uint8_t *arr = (uint8_t *)malloc(totalLength);   
    memset(arr, 0, totalLength);

    FLASH_E2_ReadByte((E2_SECTOR_NUM_Type)e2psector, StartAddr, totalLength, arr);

    crcVal = flashCrcCalculator(arr, (totalLength-4));

    //CRC tutuyor ise veriyi oku
    if((crcVal == arr[totalLength-4]) && checkPreambleBytes(arr, totalLength-4))
    {
        for(arrCnt = 0, arrCnt2 = 0; arrCnt < len; arrCnt++)
        {
            FLASH_E2_ReadByte((E2_SECTOR_NUM_Type)e2psector, (StartAddr + arrCnt2), bpArr[arrCnt].length, (uint8_t *)bpArr[arrCnt].address);
            arrCnt2 += bpArr[arrCnt].length;
        }
    }
    else
    {
        length = 0;
    }

    TICKET_REMOVE(device->sys->tvm);

    free(arr);

    return length;
}

static DEV_MEM_FUNC_WRITE(emuEepromWrite)
{
    const tsEmuEepromConsts *consts = device->constants;

    FLASH_Status status = FLASH_COMPLETE;
    uint32_t StartAddr;
    uint8_t sectorNum, sectorNumIndex;
    uint16_t len = length;
    tsBackupItem *bpArr = (tsBackupItem *)writeData;
    uint16_t arrCnt, arrCnt2, arrCnt3; 
    uint16_t totalLength = 0;
    uint8_t paddingVal = 0;
    uint8_t crcVal;
    uint8_t tempVal;

    // Find sector index
    sectorNumIndex = address / 1024;
        // Find number of the sector 
    sectorNum = consts->blockList[sectorNumIndex];
    StartAddr = address - (sectorNumIndex << 10);

    // Detect undefined sector
    if(!(sectorNumIndex < consts->blockListSize))
    {
        // Failed
        length = 0; 
        return length;
    }
 
    for(arrCnt = 0; arrCnt < len; arrCnt++)
    {
      totalLength += bpArr[arrCnt].length;
    }
    
    //Padding to uint32_t
    paddingVal = (totalLength % 4);
    totalLength += (4 - paddingVal);
    //adding for crc data
    if(paddingVal)
    {
      totalLength += 4;
    }

    uint8_t *arr = (uint8_t *)malloc(totalLength);   
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

    //Flash daki data ile ramdeki datayi karsilastir
    FLASH_E2_ReadByte((E2_SECTOR_NUM_Type)sectorNum, (StartAddr + totalLength - 4), 1, &tempVal);

    if(tempVal == crcVal)
    {
      for(; arrCnt < (totalLength-4); arrCnt++)
      {
        FLASH_E2_ReadByte((E2_SECTOR_NUM_Type)sectorNum, (StartAddr + arrCnt), 1, &tempVal);
        if(tempVal != arr[arrCnt])
        {
          break;
        }
      }
    }

    //Data Farkli cikarsa flash'a yaz
    if(arrCnt < (totalLength-4))
    {
      //Erase Flash
      status = FLASH_COMPLETE;
      for (uint8_t index = 0; index < consts->blockListSize; index++)
      {
          FLASH_E2_Unlock(SINGLE_OPERATION);
          status = FLASH_E2_EraseSector(consts->blockList[index]);
          FLASH_E2_Lock();

      }

      //Write Flash
      status = FLASH_COMPLETE;
      FLASH_E2_Unlock(MULTI_OPERATION);
      status = FLASH_E2_ProgramPage((E2_SECTOR_NUM_Type)sectorNum, StartAddr, totalLength, arr);
      FLASH_E2_Lock();
      
      if(status != FLASH_COMPLETE)
      {
        length = 0;
      }
    }

    free(arr);

    TICKET_REMOVE(device->sys->tvm);
    return length;
}

static DEV_MEM_FUNC_ERASE(emuEepromErase)
{
    const tsEmuEepromConsts *consts = device->constants;

    UNUSED(address);

    FLASH_Status status = FLASH_COMPLETE;

    for (uint8_t index = 0; index < consts->blockListSize; index++)
    {
        FLASH_E2_Unlock(SINGLE_OPERATION);
        status = FLASH_E2_EraseSector(consts->blockList[index]);
        FLASH_E2_Lock();

        if (status != FLASH_COMPLETE)
        {
            // Failed
            size = 0;
        }
    }

    TICKET_REMOVE(device->sys->tvm);

    return size;
}

/** @} */

/** @file       h_backup.c
*  @brief      BBackup data in external eeprom after every 2 hours 
*  @copyright  (c) 2019-Arcelik - All Rights Reserved
*              Permission to use, reproduce, copy, prepare derivative works,
*              modify, distribute, perform, display or sell this software and/or
*              its documentation for any purpose is prohibited without the express
*              written consent of Arcelik A.S.
*  @author     saqib sohail
*  @date       01.08.2024
*/
#define FILE_H_BACKUP_C

/**
*  @addtogroup BACKUP
*  @{
*/
/*********************************************************************
*  INCLUDES
*/
#include "h_backup.h"
//#include <string.h>
/* Reset record function */
#ifdef ResetRecord
#include "reset_record.h"
#endif
tsProcess *rcosBackup = NULL;
extern stEEPROM_Ram EEPROM_R;
extern stRefrigerator_Ram refrigerator_R;
/*********************************************************************
*  LOCAL CONSTANTS
*/
#define COPY_CNT (1)     // Number of copies stored in memory
#define BURST_SIZE (32) // Maximum number of bytes that will be read or written
#define ITEM_SIZE_MAX (sizeof(uint32_t))
#define MAXLENGTH 252
#define PERIOD_2_HOURS 1000*60*60*2 //1000*60*2
#define PERIOD_1_Min 1000*60        //APW : Update I2C driver for add CT state (every min)
#define Index_Item_ClearCT 21+1     //APW : Write field force (21 index)
uint8_t indexBackupTimer1 = 0;
uint8_t indexBackupTimer2 = 0;
uint16_t buffer_b16[MAXLENGTH+1];
bool ResultCheckStock1NotClear = false;
bool ResultCheckStock2NotClear = false;
bool ResultCheckStock3NotClear = false;
bool ResultCheckParaVerNotClear = false;
bool ResultCheckParaRevNotClear = false;
bool ResultCheckCoreVerNotClear = false;
bool ResultCheckCoreRevNotClear = false;
/// @brief  Events of a Backup process
typedef enum
{
  eBackupPeriodic100Ms,
  eBackupPeriodic2Hrs,
  eBackupPeriodic1Min,  //APW : Update I2C driver for add CT state
  eBackupPeriodic20Ms   //APW : Update I2C driver for add CT state
} teBackupEvents;


/*void   updatevalues(void)
{
  tsHBackupParams *params = rcosBackup->parameters;
   if(params->eepr->flag1.Operation_24h == 1) //When the recording time of 24 hours is reached, write 0 to buffer.
  {
    params->eepr->eep_uc_Log_FCompVal_Buf.byte4 = 0;
    params->eepr->eep_uc_Log_RCompVal_Buf.byte4 = 0;
  }
}*/


void clearBackupTimer(void)
{
  
  tsHBackupParams *params = rcosBackup->parameters;
  
  switch(indexBackupTimer2)
  {
  case 0:
    if(refrigerator_R.Cabin[0].values.finishDefrostFlag)
    {
      buffer_b16[0] = 0;
      devMemWrite(params->nvm, eep_us_Backup_RRealTimer_Address,buffer_b16, 2 * (eep_us_Backup_RRealTimer_Length));
      //refrigerator_R.Cabin[0].values.finishDefrostFlag = false;
    }
    indexBackupTimer2++;
    break;
    
  case 1:
    if(refrigerator_R.Cabin[0].values.finishDefrostFlag)
    {
      buffer_b16[0] = 0;
      devMemWrite(params->nvm, eep_us_Backup_RDoorOpenTimer_Address,buffer_b16, 2 * (eep_us_Backup_RDoorOpenTimer_Length));
      refrigerator_R.Cabin[0].values.finishDefrostFlag = false;
    }
    indexBackupTimer2++;
    break;
    
  case 2:
    if(refrigerator_R.Cabin[1].values.finishDefrostFlag)
    {
      buffer_b16[0] = 0;
      devMemWrite(params->nvm, eep_us_Backup_FRealTimer_Address,buffer_b16, 2 * (eep_us_Backup_FRealTimer_Length));
      //refrigerator_R.Cabin[1].values.finishDefrostFlag = false;
    }
    indexBackupTimer2++;
    break;
    
  case 3:
    if(refrigerator_R.Cabin[1].values.finishDefrostFlag)
    {
      buffer_b16[0] = 0;
      devMemWrite(params->nvm, eep_us_Backup_FDoorOpenTimer_Address,buffer_b16, 2 * (eep_us_Backup_FDoorOpenTimer_Length));
      refrigerator_R.Cabin[1].values.finishDefrostFlag = false;
    }
    indexBackupTimer2++;
    break;
    
  default:
    indexBackupTimer2 = 0;
    break;
    
  }
}

void updateTimerBackup(void) // 
{
  tsHBackupParams *params = rcosBackup->parameters;

  switch(indexBackupTimer1)
  {

  case 0:
    buffer_b16[0] = params->eepr->eep_us_Backup_RRealTimer;
    if (!(refrigerator_R.Cabin[0].values.realTimer % 15))
      devMemWrite(params->nvm, eep_us_Backup_RRealTimer_Address,buffer_b16, 2 * (eep_us_Backup_RRealTimer_Length));
    indexBackupTimer1++;
    break;
    
  case 1:
    buffer_b16[0] = params->eepr->eep_us_Backup_FRealTimer;
    if (!(refrigerator_R.Cabin[1].values.realTimer % 15))
      devMemWrite(params->nvm, eep_us_Backup_FRealTimer_Address,buffer_b16, 2 * (eep_us_Backup_FRealTimer_Length));
    indexBackupTimer1++;
    break;
    
  case 2:
    buffer_b16[0] = params->eepr->eep_us_Backup_RDoorOpenTimer;
    if (!(refrigerator_R.Cabin[0].values.realTimer % 15))
      devMemWrite(params->nvm, eep_us_Backup_RDoorOpenTimer_Address,buffer_b16, 2 * (eep_us_Backup_RDoorOpenTimer_Length));
    indexBackupTimer1++;
    break;
    
  case 3:
    buffer_b16[0] = params->eepr->eep_us_Backup_FDoorOpenTimer;
    if (!(refrigerator_R.Cabin[1].values.realTimer % 15))
      devMemWrite(params->nvm, eep_us_Backup_FDoorOpenTimer_Address,buffer_b16, 2 * (eep_us_Backup_FDoorOpenTimer_Length));
    indexBackupTimer1++;
    break;
    
  default:
    clearBackupTimer();
    break;
  } 
}

  
void updateField(uint8_t index) // Update required field in EEPROM first read then write
{
  
  tsHBackupParams *params = rcosBackup->parameters;
  if(index>=params->itemArrSize)//out of bounds
    return;
  
  
  const tsBackupItemEE *item = &params->itemArr[index];
  
  if (!item->ptr || item->length == 0 || item->length > MAXLENGTH  ) {
    
    return;
  }
  
  if (item->size == sizeof(uint8_t)) {
    uint8_t value = *(uint8_t *)(item->ptr);
    uint8_t *buffer_b8 = (uint8_t *)buffer_b16;             // use array as 8 bit to reduce memory
    
    while (item->length != devMemRead(params->nvm, item->mem_adress,(buffer_b8+1),item->length));
    *buffer_b8 = value;
    devMemWrite(params->nvm, item->mem_adress,buffer_b8,item->length);
  }
  else if (item->size == sizeof(uint16_t)) {
    uint16_t value = *(uint16_t *)(item->ptr);
    
    while (2*(item->length) != devMemRead(params->nvm, item->mem_adress,(buffer_b16+1),2*(item->length)));
    buffer_b16[0] = value;
    devMemWrite(params->nvm, item->mem_adress,buffer_b16,2*(item->length));
  }
    
  else if (item->size == sizeof(uint32_t)) {
    uint32_t value = *(uint32_t *)(item->ptr);
    uint32_t *buffer_b32 = (uint32_t *)buffer_b16; 
    
     if((item->mem_adress == eep_uc_Log_FCompValT_Address) || (item->mem_adress == eep_uc_Log_RCompValT_Address)) 
         {
           if(params->eepr->flag1.Operation_24h == 1) // Record every 24 hr 
           {
            while (4*(item->length) != devMemRead(params->nvm, item->mem_adress,(buffer_b32+1),4*(item->length)));
            buffer_b32[0] = value;
            devMemWrite(params->nvm, item->mem_adress,buffer_b32,4*(item->length));
           }
         }
         else{ 
                  while (4*(item->length) != devMemRead(params->nvm, item->mem_adress,(buffer_b32+1),4*(item->length)));
                  buffer_b32[0] = value;
                  devMemWrite(params->nvm, item->mem_adress,buffer_b32,4*(item->length));
         }
  }
}


/*** APW : Update I2C driver for add CT state ***/
 void WriteFieldForce(uint8_t indexClearCT) //Write function when force from commercial
{
  tsHBackupParams *params = rcosBackup->parameters;
  
  uint8_t *buffer_b8 = (uint8_t *)buffer_b16; 
  *buffer_b8 = 0;
  buffer_b16[0] = 0;
  
  switch(indexClearCT)
  {
  case 0:
      break;
  case 1: //CT state log (Fix in case 1 only) : 1 byte
        *buffer_b8 = params->rf->ui.states.commercial.FlagCommercialEnable;
        devMemWrite(params->nvm, eep_uc_Log_TestMd_Address,buffer_b8, eep_uc_Log_TestMd_Length);
      break;
  case 2: //Clear Data Count : 1 byte
        devMemWrite(params->nvm, eep_uc_Log_DataCnt_Address,buffer_b8, eep_uc_Log_DataCnt_Length);
        params->eepr->eep_uc_Log_DataCnt = *buffer_b8;
      break;
  case 3: //Failure Data 1 : 1 byte
        devMemWrite(params->nvm, eep_uc_ErrLog_01_Address,buffer_b8, eep_uc_ErrLog_01_Length);
        params->eepr->eep_uc_ErrLog[0] = *buffer_b8;
      break;
  case 4: //Failure Data 2 : 1 byte
        devMemWrite(params->nvm, eep_uc_ErrLog_02_Address,buffer_b8, eep_uc_ErrLog_02_Length);
        params->eepr->eep_uc_ErrLog[1] = *buffer_b8;
      break;
  case 5: //Failure Data 3 : 1 byte
        devMemWrite(params->nvm, eep_uc_ErrLog_03_Address,buffer_b8, eep_uc_ErrLog_03_Length);
         params->eepr->eep_uc_ErrLog[2] = *buffer_b8;
      break;
  case 6: //Failure Data 4 : 1 byte
        devMemWrite(params->nvm, eep_uc_ErrLog_04_Address,buffer_b8, eep_uc_ErrLog_04_Length);
         params->eepr->eep_uc_ErrLog[3] = *buffer_b8;
      break;
  case 7: //Failure Day 1 : 1 byte
        devMemWrite(params->nvm, eep_uc_ErrOfDay_01_Address,buffer_b8, eep_uc_ErrOfDay_01_Length);
        params->eepr->eep_uc_ErrOfDay[0] = *buffer_b8;
      break;
  case 8: //Failure Day 2 : 1 byte
        devMemWrite(params->nvm, eep_uc_ErrOfDay_02_Address,buffer_b8, eep_uc_ErrOfDay_02_Length);
        params->eepr->eep_uc_ErrOfDay[1] = *buffer_b8;
      break;
  case 9: //Failure Day 3 : 1 byte
        devMemWrite(params->nvm, eep_uc_ErrOfDay_03_Address,buffer_b8, eep_uc_ErrOfDay_03_Length);
        params->eepr->eep_uc_ErrOfDay[2] = *buffer_b8;
      break;
  case 10: //Failure Day 4 : 1 byte
        devMemWrite(params->nvm, eep_uc_ErrOfDay_04_Address,buffer_b8, eep_uc_ErrOfDay_04_Length);
        params->eepr->eep_uc_ErrOfDay[3] = *buffer_b8;
      break;
  case 11: //F door Sum is clear : 2 byte
        devMemWrite(params->nvm, eep_us_LogSum_Fdoor_Address,buffer_b16,2*(eep_us_LogSum_Fdoor_Length));
        params->eepr->eep_us_LogSum_Fdoor = buffer_b16[0];
      break;
  case 12: //R door Sum is clear : 2 byte
        devMemWrite(params->nvm, eep_us_LogSum_Rdoor_Address,buffer_b16,2*(eep_us_LogSum_Rdoor_Length));
        params->eepr->eep_us_LogSum_Rdoor = buffer_b16[0];
      break;
  case 13: //F defrost Sum is clear : 2 byte
        devMemWrite(params->nvm, eep_us_LogSum_FDefCount_Address,buffer_b16,2*(eep_us_LogSum_FDefCount_Length));
        params->eepr->eep_us_LogSum_FDefCount = buffer_b16[0];
      break;
  case 14: //R defrost Sum is clear : 2 byte
        devMemWrite(params->nvm, eep_us_LogSum_RDefCount_Address,buffer_b16,2*(eep_us_LogSum_RDefCount_Length));
        params->eepr->eep_us_LogSum_RDefCount = buffer_b16[0];
      break;
  case 15: //Working time is clear : 2 byte
        devMemWrite(params->nvm, eep_us_LogSum_PwrOnTm_Address,buffer_b16,2*(eep_us_LogSum_PwrOnTm_Length));
        params->eepr->eep_us_LogSum_PwrOnTm = buffer_b16[0];
      break;
  case 16: //Working time is clear : 2 byte
        devMemWrite(params->nvm, eep_us_LogSum_Valve_Address,buffer_b16,2*(eep_us_LogSum_Valve_Length));
        params->eepr->eep_us_LogSum_Valve = buffer_b16[0];
      break;
  case 17: //Working time is clear : 1 byte
        devMemWrite(params->nvm, eep_uc_Log_hourcnt_Address,buffer_b8,eep_uc_Log_hourcnt_Length);
        params->eepr->eep_uc_Log_hourcnt = *buffer_b8;
      break;
  case 18:
        devMemWrite(params->nvm, eep_us_Backup_FRealTimer_Address,buffer_b16, 2 * (eep_us_Backup_FRealTimer_Length));
        params->eepr->eep_us_Backup_FRealTimer = buffer_b16[0];
        refrigerator_R.Cabin[1].E2.realTimer = 0;
        refrigerator_R.Cabin[1].values.realTimer = 0;
    break;
  case 19:
        devMemWrite(params->nvm, eep_us_Backup_RRealTimer_Address,buffer_b16, 2 * (eep_us_Backup_RRealTimer_Length));
        params->eepr->eep_us_Backup_RRealTimer = buffer_b16[0];
        refrigerator_R.Cabin[0].E2.realTimer = 0;
        refrigerator_R.Cabin[0].values.realTimer = 0;
    break;
  case 20:
        devMemWrite(params->nvm, eep_us_Backup_FDoorOpenTimer_Address,buffer_b16, 2 * (eep_us_Backup_FDoorOpenTimer_Length));
        params->eepr->eep_us_Backup_FDoorOpenTimer = buffer_b16[0];
    break;
  case 21:
        devMemWrite(params->nvm, eep_us_Backup_RDoorOpenTimer_Address,buffer_b16, 2 * (eep_us_Backup_RDoorOpenTimer_Length));
        params->eepr->eep_us_Backup_RDoorOpenTimer = buffer_b16[0];  
    break;  
  }
}
/*** APW : Update I2C driver for add CT state ***/

/******************* Clear EEPROM data to 00 when initial ********************/
void ClearEEPROMdataWhenInit(void)
{
    tsHBackupParams *params = rcosBackup->parameters;
   
    /************ Failure history clear to 00 ************/
    /****(Clear when read stock main / version to FF) ****/
    for(uint8_t i=0;i<4;i++){
      params->eepr->eep_uc_ErrLog[i] = 0;
      params->eepr->eep_uc_ErrOfDay[i] = 0;
    }
    /************ Failure log sum clear to 00 ************/
    /****(Clear when read stock main / version to FF) ****/
    params->eepr->eep_us_LogSum_Fdoor = 0;
    params->eepr->eep_us_LogSum_Rdoor = 0;
    params->eepr->eep_us_LogSum_FDefCount = 0;
    params->eepr->eep_us_LogSum_RDefCount = 0;
    params->eepr->eep_us_LogSum_PwrOnTm = 0;
   //params->eepr->eep_uc_Log_hourcnt = 0;      //This function special cannot clear in this process,it clear when read initial
    /************* Error history clear to 00 *************/
    /****(Clear when read stock main / version to FF) ****/
    for(uint8_t i=0;i<29;i++){
      params->eepr->eep_us_ErrHis_Old[i] = 0;
      params->eepr->eep_us_ErrHis_New[i] = 0;
      params->eepr->eep_us_ErrHis_Cnt[i] = 0;
    }
    /** Valve/Gear/WD/IC/DIS operation counter clear to 00 **/
    /***** (Clear when read stock main / version to FF) *****/
    params->eepr->eep_us_LogSum_Valve = 0;
    params->eepr->eep_us_LogSum_GireMOnTm = 0;
    params->eepr->eep_us_LogSum_WDPOnTm = 0;
    params->eepr->eep_us_LogSum_IDSolOnTm = 0;
    params->eepr->eep_us_LogSum_DDSolOnTm = 0;
    /*********** CT state and DataCnt clear to 00 ***********/
    /***** (Clear when read stock main / version to FF) *****/
    //params->eepr->eep_uc_Log_TestMd.byte = 0;   //This function special cannot clear in this process,it clear when read initial
    //params->eepr->eep_uc_Log_DataCnt = 0;       //This function special cannot clear in this process,it clear when read initial
    /********* Stock software and version clear to 00 ********/
    /***** (Clear when read stock main / version to FF) *****/
    params->eepr->eep_ui_Log_MainStockNo[0] = 0;
    params->eepr->eep_ui_Log_MainStockNo[1] = 0;
    params->eepr->eep_ui_Log_MainStockNo[2] = 0;
    params->eepr->eep_uc_Log_MainParaVer = 0;
    params->eepr->eep_uc_Log_MainParaRev = 0;
    params->eepr->eep_uc_Log_MainCoreVer = 0;
    params->eepr->eep_uc_Log_MainCoreRev = 0; 
    
    params->eepr->eep_ui_Log_CompStockNo[0] = 0;  
    params->eepr->eep_ui_Log_CompStockNo[1] = 0;
    params->eepr->eep_ui_Log_CompStockNo[2] = 0;
    params->eepr->eep_uc_Log_CompParaVer = 0;
    params->eepr->eep_uc_Log_CompParaRev = 0;
    params->eepr->eep_uc_Log_CompCoreVer = 0;
    params->eepr->eep_uc_Log_CompCoreRev = 0;
}

void ReadFieldInit(void) //Read function when initial
{
   tsHBackupParams *params = rcosBackup->parameters;
  
        // use array as 8 bit to reduce memory
    
/*************************** For Read 1 byte data *****************************************/
   /*** APW : Update I2C driver for add CT state ***/
   uint8_t *buffer_b8 = (uint8_t *)buffer_b16;     
   while (eep_uc_Log_TestMd_Length != devMemRead(params->nvm, eep_uc_Log_TestMd_Address,buffer_b8, eep_uc_Log_TestMd_Length));
    params->eepr->eep_uc_Log_TestMd.byte = *buffer_b8;
    if(params->eepr->eep_uc_Log_TestMd.byte == 0xFF)
    {
      params->eepr->eep_uc_Log_TestMd.byte = 0;
      params->rf->ui.states.commercial.FlagCommercialEnable = 0;
    }
    else 
      params->rf->ui.states.commercial.FlagCommercialEnable = params->eepr->eep_uc_Log_TestMd.CT_Mode;
   /*** APW : Update I2C driver for add CT state ***/
   
   while (eep_uc_Log_hourcnt_Length != devMemRead(params->nvm, eep_uc_Log_hourcnt_Address,buffer_b8, eep_uc_Log_hourcnt_Length)); //Read data from eep_uc_Log_hourcnt
   if(*buffer_b8 == 0xFF)
     *buffer_b8 = 0;
   params->eepr->eep_uc_Log_hourcnt = *buffer_b8 + 1;
   while (eep_uc_ErrLog_01_Length != devMemRead(params->nvm, eep_uc_ErrLog_01_Address,buffer_b8, eep_uc_ErrLog_01_Length));
   params->eepr->eep_uc_ErrLog[0] = *buffer_b8;
   while (eep_uc_ErrLog_02_Length != devMemRead(params->nvm, eep_uc_ErrLog_02_Address,buffer_b8, eep_uc_ErrLog_02_Length)); 
   params->eepr->eep_uc_ErrLog[1] = *buffer_b8;
   while (eep_uc_ErrLog_03_Length != devMemRead(params->nvm, eep_uc_ErrLog_03_Address,buffer_b8, eep_uc_ErrLog_03_Length)); 
   params->eepr->eep_uc_ErrLog[2] = *buffer_b8;
   while (eep_uc_ErrLog_04_Length != devMemRead(params->nvm, eep_uc_ErrLog_04_Address,buffer_b8, eep_uc_ErrLog_04_Length)); 
   params->eepr->eep_uc_ErrLog[3] = *buffer_b8;
   while (eep_uc_ErrOfDay_01_Length != devMemRead(params->nvm, eep_uc_ErrOfDay_01_Address,buffer_b8, eep_uc_ErrOfDay_01_Length)); 
   params->eepr->eep_uc_ErrOfDay[0] = *buffer_b8;
   while (eep_uc_ErrOfDay_02_Length != devMemRead(params->nvm, eep_uc_ErrOfDay_02_Address,buffer_b8, eep_uc_ErrOfDay_02_Length)); 
   params->eepr->eep_uc_ErrOfDay[1] = *buffer_b8;
   while (eep_uc_ErrOfDay_03_Length != devMemRead(params->nvm, eep_uc_ErrOfDay_03_Address,buffer_b8, eep_uc_ErrOfDay_03_Length)); 
   params->eepr->eep_uc_ErrOfDay[2] = *buffer_b8;
   while (eep_uc_ErrOfDay_04_Length != devMemRead(params->nvm, eep_uc_ErrOfDay_04_Address,buffer_b8, eep_uc_ErrOfDay_04_Length)); 
   params->eepr->eep_uc_ErrOfDay[3] = *buffer_b8;   
   while (eep_uc_Log_DataCnt_Length != devMemRead(params->nvm, eep_uc_Log_DataCnt_Address,buffer_b8, eep_uc_Log_DataCnt_Length));
   if(*buffer_b8 == 0xFF)
     *buffer_b8 = 0;
   if(*buffer_b8 < 0xFC) //Max 252 value of params->eepr->eep_uc_Log_DataCnt
    params->eepr->eep_uc_Log_DataCnt = *buffer_b8 + 1;
   else
    params->eepr->eep_uc_Log_DataCnt = *buffer_b8;

    /*************************** For Read 2 byte data *****************************************/
    while (2*(eep_us_LogSum_Fdoor_Length) != devMemRead(params->nvm, eep_us_LogSum_Fdoor_Address , buffer_b16,2*(eep_us_LogSum_Fdoor_Length))); //Read data from eep_us_LogSum_Fdoor
    params->eepr->eep_us_LogSum_Fdoor = *buffer_b16;
    while (2*(eep_us_LogSum_Rdoor_Length) != devMemRead(params->nvm, eep_us_LogSum_Rdoor_Address , buffer_b16,2*(eep_us_LogSum_Rdoor_Length))); //Read data from eep_us_LogSum_Fdoor
    params->eepr->eep_us_LogSum_Rdoor = *buffer_b16;
    while (2*(eep_us_LogSum_FDefCount_Length) != devMemRead(params->nvm, eep_us_LogSum_FDefCount_Address , buffer_b16,2*(eep_us_LogSum_FDefCount_Length))); //Read data from eep_us_LogSum_FDefCount 
    params->eepr->eep_us_LogSum_FDefCount = *buffer_b16;
    while (2*(eep_us_LogSum_RDefCount_Length) != devMemRead(params->nvm, eep_us_LogSum_RDefCount_Address , buffer_b16,2*(eep_us_LogSum_RDefCount_Length))); //Read data from eep_us_LogSum_RDefCount 
    params->eepr->eep_us_LogSum_RDefCount = *buffer_b16;
    while (2*(eep_us_LogSum_PwrOnTm_Length) != devMemRead(params->nvm, eep_us_LogSum_PwrOnTm_Address , buffer_b16,2*(eep_us_LogSum_PwrOnTm_Length))); //Read data from eep_us_LogSum_PwrOnTm
    params->eepr->eep_us_LogSum_PwrOnTm = *buffer_b16;   
    while (2*(eep_us_LogSum_Valve_Length) != devMemRead(params->nvm, eep_us_LogSum_Valve_Address , buffer_b16,2*(eep_us_LogSum_Valve_Length))); //Read data from eep_us_LogSum_Valve
    params->eepr->eep_us_LogSum_Valve = *buffer_b16;
    while (2*(eep_us_Backup_FRealTimer_Length) != devMemRead(params->nvm, eep_us_Backup_FRealTimer_Address , buffer_b16,2*(eep_us_Backup_FRealTimer_Length))); //Read data from eep_us_Backup_FRealTimer 
    params->eepr->eep_us_Backup_FRealTimer = *buffer_b16;
    while (2*(eep_us_Backup_RRealTimer_Length) != devMemRead(params->nvm, eep_us_Backup_RRealTimer_Address , buffer_b16,2*(eep_us_Backup_RRealTimer_Length))); //Read data from eep_us_Backup_RRealTimer 
    params->eepr->eep_us_Backup_RRealTimer = *buffer_b16;
    while (2*(eep_us_Backup_FDoorOpenTimer_Length) != devMemRead(params->nvm, eep_us_Backup_FDoorOpenTimer_Address , buffer_b16,2*(eep_us_Backup_FDoorOpenTimer_Length))); //Read data from eep_us_Backup_FDoorOpenTimer 
    params->eepr->eep_us_Backup_FDoorOpenTimer = *buffer_b16;
    while (2*(eep_us_Backup_RDoorOpenTimer_Length) != devMemRead(params->nvm, eep_us_Backup_RDoorOpenTimer_Address , buffer_b16,2*(eep_us_Backup_RDoorOpenTimer_Length))); //Read data from eep_us_Backup_RDoorOpenTimer 
    params->eepr->eep_us_Backup_RDoorOpenTimer = *buffer_b16;
    
    uint16_t ErrHis_Old_StartAddress = eep_us_ErrHis_Old_type_01_Address; //0x0CEF
    uint16_t ErrHis_New_StartAddress = eep_us_ErrHis_New_type_01_Address; //0x0D29
    uint16_t ErrHis_Cnt_StartAddress = eep_us_ErrHis_Cnt_type_01_Address; //0x0D63
    uint8_t Length = eep_us_ErrHis_Old_type_01_Length; //0x01
    for(uint8_t i=0;i<29;i++){
    while (2*(Length) != devMemRead(params->nvm, ErrHis_Old_StartAddress, buffer_b16,2*(Length))); //Read data from eep_us_LogSum_Valve
    params->eepr->eep_us_ErrHis_Old[i] = *buffer_b16;
    while (2*(Length) != devMemRead(params->nvm, ErrHis_New_StartAddress, buffer_b16,2*(Length))); //Read data from eep_us_LogSum_Valve
    params->eepr->eep_us_ErrHis_New[i] = *buffer_b16;
    while (2*(Length) != devMemRead(params->nvm, ErrHis_Cnt_StartAddress, buffer_b16,2*(Length))); //Read data from eep_us_LogSum_Valve
    params->eepr->eep_us_ErrHis_Cnt[i] = *buffer_b16;
    ErrHis_Old_StartAddress +=2;
    ErrHis_New_StartAddress +=2;
    ErrHis_Cnt_StartAddress +=2;
    }
    
    /*************************** For Read 4 byte data *****************************************/
    uint32_t *buffer_b32 = (uint32_t *)buffer_b16; 
    while (4*(eep_uc_Log_FCompVal_Buf_Length) != devMemRead(params->nvm, eep_uc_Log_FCompVal_Buf_Address,buffer_b32,4*(eep_uc_Log_FCompVal_Buf_Length))); //Read data from eep_uc_Log_FCompVal_Buf
    params->eepr->eep_uc_Log_FCompVal_Buf.byte4 = *buffer_b32;
    while (4*(eep_uc_Log_RCompVal_Buf_Length) != devMemRead(params->nvm, eep_uc_Log_RCompVal_Buf_Address,buffer_b32,4*(eep_uc_Log_RCompVal_Buf_Length))); //Read data from eep_uc_Log_FCompVal_Buf
    params->eepr->eep_uc_Log_RCompVal_Buf.byte4 = *buffer_b32;
    
    
    //Converse data to 00 when the data are FF (We will not reset the data above).
    while (eep_ui_Log_MainStockNo01_Length != devMemRead(params->nvm, eep_ui_Log_MainStockNo01_Address,buffer_b16, eep_ui_Log_MainStockNo01_Length));
      if(*buffer_b16 == 0xFFFF)
        ResultCheckStock1NotClear = true;
      else
        ResultCheckStock1NotClear = false;
    while (eep_ui_Log_MainStockNo02_Length != devMemRead(params->nvm, eep_ui_Log_MainStockNo02_Address,buffer_b16, eep_ui_Log_MainStockNo02_Length));
      if(*buffer_b16 == 0xFFFF)
        ResultCheckStock2NotClear = true;
      else
        ResultCheckStock2NotClear = false;
    while (eep_ui_Log_MainStockNo03_Length != devMemRead(params->nvm, eep_ui_Log_MainStockNo03_Address,buffer_b16, eep_ui_Log_MainStockNo03_Length));
      if(*buffer_b16 == 0xFFFF)
        ResultCheckStock3NotClear = true;
      else
        ResultCheckStock3NotClear = false;
    while (eep_uc_Log_MainParaVer_Length != devMemRead(params->nvm, eep_uc_Log_MainParaVer_Address,buffer_b8, eep_uc_Log_MainParaVer_Length));
      if(*buffer_b8 == 0xFF)
        ResultCheckParaVerNotClear = true;
      else
        ResultCheckParaVerNotClear = false;
    while (eep_uc_Log_MainParaRev_Length != devMemRead(params->nvm, eep_uc_Log_MainParaRev_Address,buffer_b8, eep_uc_Log_MainParaRev_Length));
      if(*buffer_b8 == 0xFF)
        ResultCheckParaRevNotClear = true;
      else
        ResultCheckParaRevNotClear = false;
    while (eep_uc_Log_MainCoreVer_Length != devMemRead(params->nvm, eep_uc_Log_MainCoreVer_Address,buffer_b8, eep_uc_Log_MainCoreVer_Length));
      if(*buffer_b8 == 0xFF)
        ResultCheckCoreVerNotClear = true;
      else
        ResultCheckCoreVerNotClear = false;
    while (eep_uc_Log_MainCoreRev_Length != devMemRead(params->nvm, eep_uc_Log_MainCoreRev_Address,buffer_b8, eep_uc_Log_MainCoreRev_Length));
      if(*buffer_b8 == 0xFF)
        ResultCheckCoreRevNotClear = true;
      else
        ResultCheckCoreRevNotClear = false;

    if(ResultCheckStock1NotClear && ResultCheckStock2NotClear && ResultCheckStock3NotClear &&
        ResultCheckParaVerNotClear && ResultCheckParaRevNotClear && ResultCheckCoreVerNotClear &&
          ResultCheckCoreRevNotClear)
            ClearEEPROMdataWhenInit();

}
   
static PROCESS_HANDLER_PROTO(backupPeriodicBackup)
{
  tsHBackupParams *params = process->parameters;
  static  uint8_t indexx = 0;
  static  uint8_t indexxClearCT = 0;      
  
  if (eventCurrent.source == eventCurrent.destination)
  {
    switch (eventCurrent.event)
    {
    /*** APW : Update I2C driver for add CT state ***/
    case eBackupPeriodic20Ms:
      params->periodic20Ms.post.destination = process->enumeration;
      params->periodic20Ms.post.source      = process->enumeration;
      params->periodic20Ms.post.event       = eBackupPeriodic20Ms;

      timerEventStart(&params->periodic20Ms, 20); // Periodic check
      
      updateTimerBackup();
#ifdef ResetRecord
      RSTTick20ms((indexBackupTimer1 > 3u) && (indexx == 0u));  //Update function and check status of normal EEPROM update
#endif
      
      if(params->rf->ui.states.commercial.ForceWriteFlageCommercial)
      {
        if(indexxClearCT < Index_Item_ClearCT)
        {
        indexxClearCT++;
        
          if(params->rf->ui.states.commercial.ClearStatusCommercial)
          {
            if(indexxClearCT >= Index_Item_ClearCT)
              params->rf->ui.states.commercial.ClearStatusCommercial = 0;
            else
              WriteFieldForce(indexxClearCT);
          }
          else
          {
            if(indexxClearCT <= 1)
              WriteFieldForce(indexxClearCT);
          }
         
        }
        else{
        params->rf->ui.states.commercial.FlagCommercialEnable = 0;
        params->rf->ui.states.commercial.ProhibitionCommercialUpdateEEP = 0;
        params->rf->ui.states.commercial.ForceWriteFlageCommercial = 0;
        indexxClearCT = 0;
        }
      }
      
      break;
    /*** APW : Update I2C driver for add CT state ***/
    case eBackupPeriodic100Ms:
      
 //     updatevalues();
      updateField(indexx);
      indexx++;  
      if(indexx < params->itemArrSize)
      {
        params->periodic100Ms.post.destination = process->enumeration;
        params->periodic100Ms.post.source      = process->enumeration;
        params->periodic100Ms.post.event       = eBackupPeriodic100Ms;
        if (params->period)
        {
          timerEventStart(&params->periodic100Ms, params->period); // Periodic check
        }
      }
      else      //when all elements have been updated
      {
       indexx = 0;
       eventPost(eProcessEEprom, eEEPROMEventUpdateComplete, NULL, 0); // post event to notify other process
      }
      break;
    /*** APW : Update I2C driver for add CT state ***/
    case eBackupPeriodic1Min:
      
      if((!(refrigerator_R.Cabin[0].values.realTimer % 15)) || (!(refrigerator_R.Cabin[1].values.realTimer % 15)))
        indexBackupTimer1 = 0;
      
      if(params->rf->ui.states.commercial.status == _UI_Commercial_Test &&
         params->rf->ui.states.commercial.ProhibitionCommercialUpdateEEP == 0)
      {
          params->periodic100Ms.post.destination = process->enumeration;
          params->periodic100Ms.post.source      = process->enumeration;
          params->periodic100Ms.post.event       = eBackupPeriodic100Ms;
          if (params->period)
          {
          timerEventStart(&params->periodic100Ms, params->period); // Periodic check
          }   
      }
      
      params->periodic1Min.post.destination = process->enumeration;
      params->periodic1Min.post.source      = process->enumeration;
      params->periodic1Min.post.event       = eBackupPeriodic1Min;

      timerEventStart(&params->periodic1Min, PERIOD_1_Min); // Periodic check
      
      break;
    /*** APW : Update I2C driver for add CT state ***/
    case eBackupPeriodic2Hrs:
      
      /*** APW : Update I2C driver for add CT state ***/
      /*params->periodic100Ms.post.destination = process->enumeration;
      params->periodic100Ms.post.source      = process->enumeration;
      params->periodic100Ms.post.event       = eBackupPeriodic100Ms;
      if (params->period)
      {
        timerEventStart(&params->periodic100Ms, params->period); // Periodic check
      }*/    
      
      if(params->rf->ui.states.commercial.status == _UI_Commercial_None)
      {
          params->periodic100Ms.post.destination = process->enumeration;
          params->periodic100Ms.post.source      = process->enumeration;
          params->periodic100Ms.post.event       = eBackupPeriodic100Ms;
          if (params->period)
          {
          timerEventStart(&params->periodic100Ms, params->period); // Periodic check
          }   
      }
      /*** APW : Update I2C driver for add CT state ***/
      
      params->periodic2Hrs.post.destination = process->enumeration;
      params->periodic2Hrs.post.source      = process->enumeration;
      params->periodic2Hrs.post.event       = eBackupPeriodic2Hrs;
      
      timerEventStart(&params->periodic2Hrs, PERIOD_2_HOURS); // Periodic check
      
      break;
      
    default:
      break;
    }
  }
}

void backupUpdateTrigger(void)
{
  tsHBackupParams *params = rcosBackup->parameters;
  
  // eventPost(rcosBackup->enumeration, eBackupPeriodic, NULL, 0); // Backup is closed to outside interaction
  timerEventStart(&params->periodic100Ms, params->period); // Periodic check is triggered after 1ms
}

PROCESS_INIT_PROTO(backupInit)
{
  tsHBackupParams *params = process->parameters;
  //  tsBackupHeader dummyHeader;
  //  uint16_t encBuff[sizeof(dummyHeader)];
  //  ADD_RCOS_VERSION("backup", "2.0.1")
  
  devMemInit(params->nvm); // Did it finish?
  
  rcosBackup = process;
  /*
  // Calculate array identifier
  //   params->header.signature = BACKUP_SIGNATURE;
  //  params->header.itemArrId = crc16CcittInit();
  //  params->header.totalSize = 0;
  for (int idx = 0; idx < params->itemArrSize; idx++)
  {
  params->header.itemArrId = crc16CcittAddData(params->header.itemArrId, params->itemArr[idx].length); // Use lengths of each item for crc calculation
  params->header.totalSize += params->itemArr[idx].length;                                             // Calculate total size of items
  params->crcBuffer[idx] = calculateItemCrc(idx);                                                      // Remember CRC value of each item
}
  params->header.itemArrId = crc16CcittFinish(params->header.itemArrId);
  
  // Get header from NVM
  devMemReadBlocking(params->start, encBuff, sizeof(encBuff));
  // hammingDecodeArray((uint8_t *)&dummyHeader, encBuff, ARRAY_SIZE(encBuff));
  
  if ((dummyHeader.signature == params->header.signature) &&
  (dummyHeader.itemArrId == params->header.itemArrId) &&
  (dummyHeader.totalSize == params->header.totalSize))
  {
  backupReadAll();
}
    else
  {
  // Backup is changed or something...
  backupWriteAll();
  // Write our header
  hammingEncodeArray(encBuff, (uint8_t *)&params->header, ARRAY_SIZE(encBuff));
  devMemWriteBlocking(params->start, encBuff, sizeof(encBuff));
}
  */
  ReadFieldInit();
#ifdef ResetRecord
  RSTCommit(params->nvm, htons(params->eepr->eep_us_LogSum_PwrOnTm));  // stored swapped, see WorkingDays()
#endif
  params->eepr->flag1.Operation_Reset = 1; //Reset flag = 1 when power on only.
  params->eepr->eep_uc_Log_VDMax = 0x00;
  params->eepr->eep_uc_Log_VDMin = 0xFF;
  // TODO: confirm with team if at start needs to check whether 2 hours have passed
  PROCESS_STATE_CHANGE(process, backupPeriodicBackup);
  /*
  params->periodic100Ms.post.destination = process->enumeration;
  params->periodic100Ms.post.source      = process->enumeration;
  params->periodic100Ms.post.event       = eBackupPeriodic100Ms;
  if (params->period)
  {
    timerEventStart(&params->periodic100Ms, params->period); // Periodic check
  }
  */
/*** APW : Update I2C driver for add CT state ***/
  params->periodic20Ms.post.destination = process->enumeration;
  params->periodic20Ms.post.source      = process->enumeration;
  params->periodic20Ms.post.event       = eBackupPeriodic20Ms;
  timerEventStart(&params->periodic20Ms, 20); // Periodic check

  params->periodic1Min.post.destination = process->enumeration;
  params->periodic1Min.post.source      = process->enumeration;
  params->periodic1Min.post.event       = eBackupPeriodic1Min;
  timerEventStart(&params->periodic1Min, PERIOD_1_Min); // Periodic check
/*** APW : Update I2C driver for add CT state ***/
  
  params->periodic2Hrs.post.destination = process->enumeration;
  params->periodic2Hrs.post.source      = process->enumeration;
  params->periodic2Hrs.post.event       = eBackupPeriodic2Hrs;
  timerEventStart(&params->periodic2Hrs, PERIOD_2_HOURS); // Periodic check
  
  
}

PROCESS_DEINIT_PROTO(backupDeinit)
{
  tsHBackupParams *params = process->parameters;
  
  timerEventStop(&params->periodic100Ms);
  PROCESS_STATE_CHANGE(process, NULL);
  process->initialized = 0;
}

/** @} */

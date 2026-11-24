#ifndef FILE_EE_DATA_H
#define FILE_EE_DATA_H


#include "app/refrigerator/rf_ram.h"
/*
#define INTERFACE extern
#else
#define INTERFACE
#endif
*/

// length of each member in EEPROM
#define eep_uc_TraceData_Length         16
#define eep_uc_ErrLog_01_Length            1
#define eep_uc_ErrLog_02_Length           1
#define eep_uc_ErrLog_03_Length           1
#define eep_uc_ErrLog_04_Length            1
#define eep_uc_ErrOfDay_01_Length          1
#define eep_uc_ErrOfDay_02_Length         1
#define eep_uc_ErrOfDay_03_Length          1
#define eep_uc_ErrOfDay_04_Length         1
#define eep_uc_Log_Ftemp_Length         252              //252
#define eep_uc_Log_Rtemp_Length         252            //252
#define eep_uc_Log_Tctemp_Length        252
#define  eep_uc_Log_Imtemp_Length       252
#define  eep_uc_Log_FDtemp_Length       252
#define  eep_uc_Log_RDtemp_Length       252
#define  eep_uc_Log_HUMtemp_Length      252
#define  eep_uc_Log_FRDoor_Length       252
#define  eep_uc_Log_Flags_1_Length      252
#define  eep_uc_Log_Flags_2_Length      252
#define  eep_uc_Log_Flags_3_Length      252
#define  eep_uc_Log_VDMax_Length        252
#define  eep_uc_Log_VDMin_Length        252
#define  eep_us_LogSum_Fdoor_Length     1               //2 bytes
#define  eep_us_LogSum_Rdoor_Length     1
#define  eep_us_LogSum_FDefCount_Length 1         
#define  eep_us_LogSum_RDefCount_Length 1         
#define  eep_us_LogSum_PwrOnTm_Length   1
#define  eep_uc_Log_hourcnt_Length      1               // 1 byte field

#define  eep_us_ErrHis_Old_type_01_Length 1          // 2 byte fields
#define  eep_us_ErrHis_Old_type_02_Length 1        
#define  eep_us_ErrHis_Old_type_03_Length 1  
#define  eep_us_ErrHis_Old_type_04_Length 1  
#define  eep_us_ErrHis_Old_type_05_Length 1  
#define  eep_us_ErrHis_Old_type_06_Length 1  
#define  eep_us_ErrHis_Old_type_07_Length 1  //There is no error E6
#define  eep_us_ErrHis_Old_type_08_Length 1  //There is no error E7
#define  eep_us_ErrHis_Old_type_09_Length 1  
#define  eep_us_ErrHis_Old_type_10_Length 1 
#define  eep_us_ErrHis_Old_type_11_Length 1  
#define  eep_us_ErrHis_Old_type_12_Length 1  
#define  eep_us_ErrHis_Old_type_13_Length 1  
#define  eep_us_ErrHis_Old_type_14_Length 1  
#define  eep_us_ErrHis_Old_type_15_Length 1  
#define  eep_us_ErrHis_Old_type_16_Length 1  
#define  eep_us_ErrHis_Old_type_17_Length 1  
#define  eep_us_ErrHis_Old_type_18_Length 1  
#define  eep_us_ErrHis_Old_type_19_Length 1  
#define  eep_us_ErrHis_Old_type_20_Length 1  
#define  eep_us_ErrHis_Old_type_21_Length 1  
#define  eep_us_ErrHis_Old_type_22_Length 1  
#define  eep_us_ErrHis_Old_type_23_Length 1  
#define  eep_us_ErrHis_Old_type_24_Length 1  
#define  eep_us_ErrHis_Old_type_25_Length 1  
#define  eep_us_ErrHis_Old_type_26_Length 1  
#define  eep_us_ErrHis_Old_type_27_Length 1  
#define  eep_us_ErrHis_Old_type_28_Length 1  
#define  eep_us_ErrHis_Old_type_29_Length 1  

#define  eep_us_ErrHis_New_type_01_Length 1          // 2 byte fields
#define  eep_us_ErrHis_New_type_02_Length 1        
#define  eep_us_ErrHis_New_type_03_Length 1  
#define  eep_us_ErrHis_New_type_04_Length 1  
#define  eep_us_ErrHis_New_type_05_Length 1  
#define  eep_us_ErrHis_New_type_06_Length 1  
#define  eep_us_ErrHis_New_type_07_Length 1  //There is no error E6
#define  eep_us_ErrHis_New_type_08_Length 1  //There is no error E7
#define  eep_us_ErrHis_New_type_09_Length 1  
#define  eep_us_ErrHis_New_type_10_Length 1 
#define  eep_us_ErrHis_New_type_11_Length 1  
#define  eep_us_ErrHis_New_type_12_Length 1  
#define  eep_us_ErrHis_New_type_13_Length 1  
#define  eep_us_ErrHis_New_type_14_Length 1  
#define  eep_us_ErrHis_New_type_15_Length 1  
#define  eep_us_ErrHis_New_type_16_Length 1  
#define  eep_us_ErrHis_New_type_17_Length 1  
#define  eep_us_ErrHis_New_type_18_Length 1  
#define  eep_us_ErrHis_New_type_19_Length 1  
#define  eep_us_ErrHis_New_type_20_Length 1  
#define  eep_us_ErrHis_New_type_21_Length 1  
#define  eep_us_ErrHis_New_type_22_Length 1  
#define  eep_us_ErrHis_New_type_23_Length 1  
#define  eep_us_ErrHis_New_type_24_Length 1  
#define  eep_us_ErrHis_New_type_25_Length 1  
#define  eep_us_ErrHis_New_type_26_Length 1  
#define  eep_us_ErrHis_New_type_27_Length 1  
#define  eep_us_ErrHis_New_type_28_Length 1  
#define  eep_us_ErrHis_New_type_29_Length 1 

#define  eep_us_ErrHis_Cnt_type_01_Length 1          // 2 byte fields
#define  eep_us_ErrHis_Cnt_type_02_Length 1        
#define  eep_us_ErrHis_Cnt_type_03_Length 1  
#define  eep_us_ErrHis_Cnt_type_04_Length 1  
#define  eep_us_ErrHis_Cnt_type_05_Length 1  
#define  eep_us_ErrHis_Cnt_type_06_Length 1  
#define  eep_us_ErrHis_Cnt_type_07_Length 1  //There is no error E6   
#define  eep_us_ErrHis_Cnt_type_08_Length 1  //There is no error E7
#define  eep_us_ErrHis_Cnt_type_09_Length 1  
#define  eep_us_ErrHis_Cnt_type_10_Length 1 
#define  eep_us_ErrHis_Cnt_type_11_Length 1  
#define  eep_us_ErrHis_Cnt_type_12_Length 1  
#define  eep_us_ErrHis_Cnt_type_13_Length 1  
#define  eep_us_ErrHis_Cnt_type_14_Length 1  
#define  eep_us_ErrHis_Cnt_type_15_Length 1  
#define  eep_us_ErrHis_Cnt_type_16_Length 1  
#define  eep_us_ErrHis_Cnt_type_17_Length 1  
#define  eep_us_ErrHis_Cnt_type_18_Length 1  
#define  eep_us_ErrHis_Cnt_type_19_Length 1  
#define  eep_us_ErrHis_Cnt_type_20_Length 1  
#define  eep_us_ErrHis_Cnt_type_21_Length 1  
#define  eep_us_ErrHis_Cnt_type_22_Length 1  
#define  eep_us_ErrHis_Cnt_type_23_Length 1  
#define  eep_us_ErrHis_Cnt_type_24_Length 1  
#define  eep_us_ErrHis_Cnt_type_25_Length 1  
#define  eep_us_ErrHis_Cnt_type_26_Length 1  
#define  eep_us_ErrHis_Cnt_type_27_Length 1  
#define  eep_us_ErrHis_Cnt_type_28_Length 1  
#define  eep_us_ErrHis_Cnt_type_29_Length 1 

#define eep_uc_Log_FCompValT_Length    21              // its a 4 byte field
#define eep_uc_Log_RCompValT_Length    21              // its a 4 byte field
#define eep_uc_Log_FCompVal_Buf_Length    1              // its a 4 byte field
#define eep_uc_Log_RCompVal_Buf_Length    1              // its a 4 byte field    
#define eep_uc_Log_ValveFlag_Length    252
#define eep_us_LogSum_Valve_Length      1                 // 2 byte fields
#define eep_us_LogSum_GireMOnTm_Length  1
#define eep_us_LogSum_WDPOnTm_Length    1
#define eep_us_LogSum_IDSolOnTm_Length  1
#define eep_us_LogSum_DDSolOnTm_Length  1
#define eep_uc_Log_TestMd_Length        1
#define eep_uc_Log_DataCnt_Length       1
#define eep_ui_Log_MainStockNo01_Length   1
#define eep_ui_Log_MainStockNo02_Length   1
#define eep_ui_Log_MainStockNo03_Length   1
#define eep_uc_Log_MainParaVer_Length   1
#define eep_uc_Log_MainParaRev_Length   1
#define eep_uc_Log_MainCoreVer_Length   1
#define eep_uc_Log_MainCoreRev_Length   1
#define eep_ui_Log_CompStockNo01_Length   1
#define eep_ui_Log_CompStockNo02_Length   1
#define eep_ui_Log_CompStockNo03_Length   1
#define eep_uc_Log_CompParaVer_Length   1
#define eep_uc_Log_CompParaRev_Length   1
#define eep_uc_Log_CompCoreVer_Length   1
#define eep_uc_Log_CompCoreRev_Length   1
#define eep_us_Backup_FRealTimer_Length   1
#define eep_us_Backup_RRealTimer_Length   1
#define eep_us_Backup_FDoorOpenTimer_Length   1
#define eep_us_Backup_RDoorOpenTimer_Length   1
#define eep_uc_RstCnt_Length            8       //counter per cause: WDT CORE STACK HARDFAULT
#define eep_uc_RstHdr_Length            8       //LOG_VER, header CRC, 6 B reserve
#define eep_uc_RstRec_01_Length         16      //record 1 = newest
#define eep_uc_RstRec_02_Length         16      //record 2
#define eep_uc_RstRec_03_Length         16      //record 3 = oldest

// strating addresses of each member in EEPROM
#define EEPROM_START_ADDRESS 0
// #define field_Address                 (Previous_field_Address + Previous_field_Length * sizeof(Previous_field_type)   some fields are 1 or 2 or 4 bytes
#define eep_uc_TraceData_Address         EEPROM_START_ADDRESS
#define eep_uc_ErrLog_01_Address         (eep_uc_TraceData_Address + eep_uc_TraceData_Length)
#define eep_uc_ErrLog_02_Address         (eep_uc_ErrLog_01_Address + eep_uc_ErrLog_01_Length)
#define eep_uc_ErrLog_03_Address         (eep_uc_ErrLog_02_Address + eep_uc_ErrLog_02_Length)
#define eep_uc_ErrLog_04_Address         (eep_uc_ErrLog_03_Address + eep_uc_ErrLog_03_Length)
#define eep_uc_ErrOfDay_01_Address       (eep_uc_ErrLog_04_Address + eep_uc_ErrLog_04_Length)
#define eep_uc_ErrOfDay_02_Address       (eep_uc_ErrOfDay_01_Address + eep_uc_ErrOfDay_01_Length)
#define eep_uc_ErrOfDay_03_Address       (eep_uc_ErrOfDay_02_Address + eep_uc_ErrOfDay_02_Length)
#define eep_uc_ErrOfDay_04_Address       (eep_uc_ErrOfDay_03_Address + eep_uc_ErrOfDay_03_Length)
#define eep_uc_Log_Ftemp_Address         (eep_uc_ErrOfDay_04_Address + eep_uc_ErrOfDay_04_Length)
#define eep_uc_Log_Rtemp_Address         (eep_uc_Log_Ftemp_Address +  eep_uc_Log_Ftemp_Length)
#define eep_uc_Log_Tctemp_Address        (eep_uc_Log_Rtemp_Address + eep_uc_Log_Rtemp_Length)
#define eep_uc_Log_Imtemp_Address        (eep_uc_Log_Tctemp_Address +  eep_uc_Log_Tctemp_Length)
#define eep_uc_Log_FDtemp_Address        (eep_uc_Log_Imtemp_Address +  eep_uc_Log_Imtemp_Length)
#define eep_uc_Log_RDtemp_Address        (eep_uc_Log_FDtemp_Address +  eep_uc_Log_FDtemp_Length)
#define eep_uc_Log_HUMtemp_Address       (eep_uc_Log_RDtemp_Address +  eep_uc_Log_RDtemp_Length)
#define eep_uc_Log_FRDoor_Address        (eep_uc_Log_HUMtemp_Address +  eep_uc_Log_HUMtemp_Length)
#define eep_uc_Log_Flags_1_Address       (eep_uc_Log_FRDoor_Address + eep_uc_Log_FRDoor_Length)
#define eep_uc_Log_Flags_2_Address       (eep_uc_Log_Flags_1_Address + eep_uc_Log_Flags_1_Length)
#define eep_uc_Log_Flags_3_Address       (eep_uc_Log_Flags_2_Address + eep_uc_Log_Flags_2_Length)
#define eep_uc_Log_VDMax_Address         (eep_uc_Log_Flags_3_Address + eep_uc_Log_Flags_3_Length)
#define eep_uc_Log_VDMin_Address         (eep_uc_Log_VDMax_Address + eep_uc_Log_VDMax_Length)
#define eep_us_LogSum_Fdoor_Address      (eep_uc_Log_VDMin_Address + eep_uc_Log_VDMin_Length)
#define eep_us_LogSum_Rdoor_Address      (eep_us_LogSum_Fdoor_Address + eep_us_LogSum_Fdoor_Length*2)
#define eep_us_LogSum_FDefCount_Address  (eep_us_LogSum_Rdoor_Address + eep_us_LogSum_Rdoor_Length*2)
#define eep_us_LogSum_RDefCount_Address  (eep_us_LogSum_FDefCount_Address + eep_us_LogSum_FDefCount_Length*2)
#define eep_us_LogSum_PwrOnTm_Address    (eep_us_LogSum_RDefCount_Address + eep_us_LogSum_RDefCount_Length*2)
#define eep_uc_Log_hourcnt_Address       (eep_us_LogSum_PwrOnTm_Address + eep_us_LogSum_PwrOnTm_Length*2)

#define eep_us_ErrHis_Old_type_01_Address (eep_uc_Log_hourcnt_Address + eep_uc_Log_hourcnt_Length)
#define eep_us_ErrHis_Old_type_02_Address (eep_us_ErrHis_Old_type_01_Address + eep_us_ErrHis_Old_type_01_Length*2)
#define eep_us_ErrHis_Old_type_03_Address (eep_us_ErrHis_Old_type_02_Address + eep_us_ErrHis_Old_type_02_Length*2)
#define eep_us_ErrHis_Old_type_04_Address (eep_us_ErrHis_Old_type_03_Address + eep_us_ErrHis_Old_type_03_Length*2)
#define eep_us_ErrHis_Old_type_05_Address (eep_us_ErrHis_Old_type_04_Address + eep_us_ErrHis_Old_type_04_Length*2)
#define eep_us_ErrHis_Old_type_06_Address (eep_us_ErrHis_Old_type_05_Address + eep_us_ErrHis_Old_type_05_Length*2)
#define eep_us_ErrHis_Old_type_07_Address (eep_us_ErrHis_Old_type_06_Address + eep_us_ErrHis_Old_type_06_Length*2)//There is no error E6
#define eep_us_ErrHis_Old_type_08_Address (eep_us_ErrHis_Old_type_07_Address + eep_us_ErrHis_Old_type_07_Length*2)//There is no error E7
#define eep_us_ErrHis_Old_type_09_Address (eep_us_ErrHis_Old_type_08_Address + eep_us_ErrHis_Old_type_08_Length*2)
#define eep_us_ErrHis_Old_type_10_Address (eep_us_ErrHis_Old_type_09_Address + eep_us_ErrHis_Old_type_09_Length*2)
#define eep_us_ErrHis_Old_type_11_Address (eep_us_ErrHis_Old_type_10_Address + eep_us_ErrHis_Old_type_10_Length*2)
#define eep_us_ErrHis_Old_type_12_Address (eep_us_ErrHis_Old_type_11_Address + eep_us_ErrHis_Old_type_11_Length*2)
#define eep_us_ErrHis_Old_type_13_Address (eep_us_ErrHis_Old_type_12_Address + eep_us_ErrHis_Old_type_12_Length*2)
#define eep_us_ErrHis_Old_type_14_Address (eep_us_ErrHis_Old_type_13_Address + eep_us_ErrHis_Old_type_13_Length*2)
#define eep_us_ErrHis_Old_type_15_Address (eep_us_ErrHis_Old_type_14_Address + eep_us_ErrHis_Old_type_14_Length*2)
#define eep_us_ErrHis_Old_type_16_Address (eep_us_ErrHis_Old_type_15_Address + eep_us_ErrHis_Old_type_15_Length*2)
#define eep_us_ErrHis_Old_type_17_Address (eep_us_ErrHis_Old_type_16_Address + eep_us_ErrHis_Old_type_16_Length*2)
#define eep_us_ErrHis_Old_type_18_Address (eep_us_ErrHis_Old_type_17_Address + eep_us_ErrHis_Old_type_17_Length*2)
#define eep_us_ErrHis_Old_type_19_Address (eep_us_ErrHis_Old_type_18_Address + eep_us_ErrHis_Old_type_18_Length*2)
#define eep_us_ErrHis_Old_type_20_Address (eep_us_ErrHis_Old_type_19_Address + eep_us_ErrHis_Old_type_19_Length*2)
#define eep_us_ErrHis_Old_type_21_Address (eep_us_ErrHis_Old_type_20_Address + eep_us_ErrHis_Old_type_20_Length*2)
#define eep_us_ErrHis_Old_type_22_Address (eep_us_ErrHis_Old_type_21_Address + eep_us_ErrHis_Old_type_21_Length*2)
#define eep_us_ErrHis_Old_type_23_Address (eep_us_ErrHis_Old_type_22_Address + eep_us_ErrHis_Old_type_22_Length*2)
#define eep_us_ErrHis_Old_type_24_Address (eep_us_ErrHis_Old_type_23_Address + eep_us_ErrHis_Old_type_23_Length*2)
#define eep_us_ErrHis_Old_type_25_Address (eep_us_ErrHis_Old_type_24_Address + eep_us_ErrHis_Old_type_24_Length*2)
#define eep_us_ErrHis_Old_type_26_Address (eep_us_ErrHis_Old_type_25_Address + eep_us_ErrHis_Old_type_25_Length*2)
#define eep_us_ErrHis_Old_type_27_Address (eep_us_ErrHis_Old_type_26_Address + eep_us_ErrHis_Old_type_26_Length*2)
#define eep_us_ErrHis_Old_type_28_Address (eep_us_ErrHis_Old_type_27_Address + eep_us_ErrHis_Old_type_27_Length*2)
#define eep_us_ErrHis_Old_type_29_Address (eep_us_ErrHis_Old_type_28_Address + eep_us_ErrHis_Old_type_28_Length*2)

#define eep_us_ErrHis_New_type_01_Address (eep_us_ErrHis_Old_type_29_Address + eep_us_ErrHis_Old_type_29_Length*2)
#define eep_us_ErrHis_New_type_02_Address (eep_us_ErrHis_New_type_01_Address + eep_us_ErrHis_New_type_01_Length*2)
#define eep_us_ErrHis_New_type_03_Address (eep_us_ErrHis_New_type_02_Address + eep_us_ErrHis_New_type_02_Length*2)
#define eep_us_ErrHis_New_type_04_Address (eep_us_ErrHis_New_type_03_Address + eep_us_ErrHis_New_type_03_Length*2)
#define eep_us_ErrHis_New_type_05_Address (eep_us_ErrHis_New_type_04_Address + eep_us_ErrHis_New_type_04_Length*2)
#define eep_us_ErrHis_New_type_06_Address (eep_us_ErrHis_New_type_05_Address + eep_us_ErrHis_New_type_05_Length*2)
#define eep_us_ErrHis_New_type_07_Address (eep_us_ErrHis_New_type_06_Address + eep_us_ErrHis_New_type_06_Length*2)//There is no error E6
#define eep_us_ErrHis_New_type_08_Address (eep_us_ErrHis_New_type_07_Address + eep_us_ErrHis_New_type_07_Length*2)//There is no error E7
#define eep_us_ErrHis_New_type_09_Address (eep_us_ErrHis_New_type_08_Address + eep_us_ErrHis_New_type_08_Length*2)
#define eep_us_ErrHis_New_type_10_Address (eep_us_ErrHis_New_type_09_Address + eep_us_ErrHis_New_type_09_Length*2)
#define eep_us_ErrHis_New_type_11_Address (eep_us_ErrHis_New_type_10_Address + eep_us_ErrHis_New_type_10_Length*2)
#define eep_us_ErrHis_New_type_12_Address (eep_us_ErrHis_New_type_11_Address + eep_us_ErrHis_New_type_11_Length*2)
#define eep_us_ErrHis_New_type_13_Address (eep_us_ErrHis_New_type_12_Address + eep_us_ErrHis_New_type_12_Length*2)
#define eep_us_ErrHis_New_type_14_Address (eep_us_ErrHis_New_type_13_Address + eep_us_ErrHis_New_type_13_Length*2)
#define eep_us_ErrHis_New_type_15_Address (eep_us_ErrHis_New_type_14_Address + eep_us_ErrHis_New_type_14_Length*2)
#define eep_us_ErrHis_New_type_16_Address (eep_us_ErrHis_New_type_15_Address + eep_us_ErrHis_New_type_15_Length*2)
#define eep_us_ErrHis_New_type_17_Address (eep_us_ErrHis_New_type_16_Address + eep_us_ErrHis_New_type_16_Length*2)
#define eep_us_ErrHis_New_type_18_Address (eep_us_ErrHis_New_type_17_Address + eep_us_ErrHis_New_type_17_Length*2)
#define eep_us_ErrHis_New_type_19_Address (eep_us_ErrHis_New_type_18_Address + eep_us_ErrHis_New_type_18_Length*2)
#define eep_us_ErrHis_New_type_20_Address (eep_us_ErrHis_New_type_19_Address + eep_us_ErrHis_New_type_19_Length*2)
#define eep_us_ErrHis_New_type_21_Address (eep_us_ErrHis_New_type_20_Address + eep_us_ErrHis_New_type_20_Length*2)
#define eep_us_ErrHis_New_type_22_Address (eep_us_ErrHis_New_type_21_Address + eep_us_ErrHis_New_type_21_Length*2)
#define eep_us_ErrHis_New_type_23_Address (eep_us_ErrHis_New_type_22_Address + eep_us_ErrHis_New_type_22_Length*2)
#define eep_us_ErrHis_New_type_24_Address (eep_us_ErrHis_New_type_23_Address + eep_us_ErrHis_New_type_23_Length*2)
#define eep_us_ErrHis_New_type_25_Address (eep_us_ErrHis_New_type_24_Address + eep_us_ErrHis_New_type_24_Length*2)
#define eep_us_ErrHis_New_type_26_Address (eep_us_ErrHis_New_type_25_Address + eep_us_ErrHis_New_type_25_Length*2)
#define eep_us_ErrHis_New_type_27_Address (eep_us_ErrHis_New_type_26_Address + eep_us_ErrHis_New_type_26_Length*2)
#define eep_us_ErrHis_New_type_28_Address (eep_us_ErrHis_New_type_27_Address + eep_us_ErrHis_New_type_27_Length*2)
#define eep_us_ErrHis_New_type_29_Address (eep_us_ErrHis_New_type_28_Address + eep_us_ErrHis_New_type_28_Length*2)

#define eep_us_ErrHis_Cnt_type_01_Address (eep_us_ErrHis_New_type_29_Address + eep_us_ErrHis_New_type_29_Length*2)
#define eep_us_ErrHis_Cnt_type_02_Address (eep_us_ErrHis_Cnt_type_01_Address + eep_us_ErrHis_Cnt_type_01_Length*2)
#define eep_us_ErrHis_Cnt_type_03_Address (eep_us_ErrHis_Cnt_type_02_Address + eep_us_ErrHis_Cnt_type_02_Length*2)
#define eep_us_ErrHis_Cnt_type_04_Address (eep_us_ErrHis_Cnt_type_03_Address + eep_us_ErrHis_Cnt_type_03_Length*2)
#define eep_us_ErrHis_Cnt_type_05_Address (eep_us_ErrHis_Cnt_type_04_Address + eep_us_ErrHis_Cnt_type_04_Length*2)
#define eep_us_ErrHis_Cnt_type_06_Address (eep_us_ErrHis_Cnt_type_05_Address + eep_us_ErrHis_Cnt_type_05_Length*2)
#define eep_us_ErrHis_Cnt_type_07_Address (eep_us_ErrHis_Cnt_type_06_Address + eep_us_ErrHis_Cnt_type_06_Length*2)//There is no error E6
#define eep_us_ErrHis_Cnt_type_08_Address (eep_us_ErrHis_Cnt_type_07_Address + eep_us_ErrHis_Cnt_type_07_Length*2)//There is no error E7
#define eep_us_ErrHis_Cnt_type_09_Address (eep_us_ErrHis_Cnt_type_08_Address + eep_us_ErrHis_Cnt_type_08_Length*2)
#define eep_us_ErrHis_Cnt_type_10_Address (eep_us_ErrHis_Cnt_type_09_Address + eep_us_ErrHis_Cnt_type_09_Length*2)
#define eep_us_ErrHis_Cnt_type_11_Address (eep_us_ErrHis_Cnt_type_10_Address + eep_us_ErrHis_Cnt_type_10_Length*2)
#define eep_us_ErrHis_Cnt_type_12_Address (eep_us_ErrHis_Cnt_type_11_Address + eep_us_ErrHis_Cnt_type_11_Length*2)
#define eep_us_ErrHis_Cnt_type_13_Address (eep_us_ErrHis_Cnt_type_12_Address + eep_us_ErrHis_Cnt_type_12_Length*2)
#define eep_us_ErrHis_Cnt_type_14_Address (eep_us_ErrHis_Cnt_type_13_Address + eep_us_ErrHis_Cnt_type_13_Length*2)
#define eep_us_ErrHis_Cnt_type_15_Address (eep_us_ErrHis_Cnt_type_14_Address + eep_us_ErrHis_Cnt_type_14_Length*2)
#define eep_us_ErrHis_Cnt_type_16_Address (eep_us_ErrHis_Cnt_type_15_Address + eep_us_ErrHis_Cnt_type_15_Length*2)
#define eep_us_ErrHis_Cnt_type_17_Address (eep_us_ErrHis_Cnt_type_16_Address + eep_us_ErrHis_Cnt_type_16_Length*2)
#define eep_us_ErrHis_Cnt_type_18_Address (eep_us_ErrHis_Cnt_type_17_Address + eep_us_ErrHis_Cnt_type_17_Length*2)
#define eep_us_ErrHis_Cnt_type_19_Address (eep_us_ErrHis_Cnt_type_18_Address + eep_us_ErrHis_Cnt_type_18_Length*2)
#define eep_us_ErrHis_Cnt_type_20_Address (eep_us_ErrHis_Cnt_type_19_Address + eep_us_ErrHis_Cnt_type_19_Length*2)
#define eep_us_ErrHis_Cnt_type_21_Address (eep_us_ErrHis_Cnt_type_20_Address + eep_us_ErrHis_Cnt_type_20_Length*2)
#define eep_us_ErrHis_Cnt_type_22_Address (eep_us_ErrHis_Cnt_type_21_Address + eep_us_ErrHis_Cnt_type_21_Length*2)
#define eep_us_ErrHis_Cnt_type_23_Address (eep_us_ErrHis_Cnt_type_22_Address + eep_us_ErrHis_Cnt_type_22_Length*2)
#define eep_us_ErrHis_Cnt_type_24_Address (eep_us_ErrHis_Cnt_type_23_Address + eep_us_ErrHis_Cnt_type_23_Length*2)
#define eep_us_ErrHis_Cnt_type_25_Address (eep_us_ErrHis_Cnt_type_24_Address + eep_us_ErrHis_Cnt_type_24_Length*2)
#define eep_us_ErrHis_Cnt_type_26_Address (eep_us_ErrHis_Cnt_type_25_Address + eep_us_ErrHis_Cnt_type_25_Length*2)
#define eep_us_ErrHis_Cnt_type_27_Address (eep_us_ErrHis_Cnt_type_26_Address + eep_us_ErrHis_Cnt_type_26_Length*2)
#define eep_us_ErrHis_Cnt_type_28_Address (eep_us_ErrHis_Cnt_type_27_Address + eep_us_ErrHis_Cnt_type_27_Length*2)
#define eep_us_ErrHis_Cnt_type_29_Address (eep_us_ErrHis_Cnt_type_28_Address + eep_us_ErrHis_Cnt_type_28_Length*2)

#define eep_uc_Log_FCompValT_Address      (eep_us_ErrHis_Cnt_type_29_Address + eep_us_ErrHis_Cnt_type_29_Length*2)
#define eep_uc_Log_RCompValT_Address      (eep_uc_Log_FCompValT_Address + eep_uc_Log_FCompValT_Length*4)
#define eep_uc_Log_FCompVal_Buf_Address   (eep_uc_Log_RCompValT_Address + eep_uc_Log_RCompValT_Length*4)
#define eep_uc_Log_RCompVal_Buf_Address   (eep_uc_Log_FCompVal_Buf_Address + eep_uc_Log_FCompVal_Buf_Length*4)
#define eep_uc_Log_ValveFlag_Address      (eep_uc_Log_RCompVal_Buf_Address + eep_uc_Log_RCompVal_Buf_Length*4)
#define eep_us_LogSum_Valve_Address       (eep_uc_Log_ValveFlag_Address + eep_uc_Log_ValveFlag_Length)
#define eep_us_LogSum_GireMOnTm_Address   (eep_us_LogSum_Valve_Address + eep_us_LogSum_Valve_Length*2)
#define eep_us_LogSum_WDPOnTm_Address     (eep_us_LogSum_GireMOnTm_Address + eep_us_LogSum_GireMOnTm_Length*2)
#define eep_us_LogSum_IDSolOnTm_Address   (eep_us_LogSum_WDPOnTm_Address + eep_us_LogSum_WDPOnTm_Length*2)
#define eep_us_LogSum_DDSolOnTm_Address   (eep_us_LogSum_IDSolOnTm_Address + eep_us_LogSum_IDSolOnTm_Length*2)
#define eep_uc_Log_TestMd_Address         (eep_us_LogSum_DDSolOnTm_Address + eep_us_LogSum_DDSolOnTm_Length*2)
#define eep_uc_Log_DataCnt_Address        (eep_uc_Log_TestMd_Address + eep_uc_Log_TestMd_Length)
#define eep_ui_Log_MainStockNo01_Address  (eep_uc_Log_DataCnt_Address + eep_uc_Log_DataCnt_Length)
#define eep_ui_Log_MainStockNo02_Address  (eep_ui_Log_MainStockNo01_Address + eep_ui_Log_MainStockNo01_Length*2)
#define eep_ui_Log_MainStockNo03_Address  (eep_ui_Log_MainStockNo02_Address + eep_ui_Log_MainStockNo02_Length*2)
#define eep_uc_Log_MainParaVer_Address    (eep_ui_Log_MainStockNo03_Address + eep_ui_Log_MainStockNo03_Length*2)
#define eep_uc_Log_MainParaRev_Address    (eep_uc_Log_MainParaVer_Address + eep_uc_Log_MainParaVer_Length)
#define eep_uc_Log_MainCoreVer_Address    (eep_uc_Log_MainParaRev_Address + eep_uc_Log_MainParaRev_Length)
#define eep_uc_Log_MainCoreRev_Address    (eep_uc_Log_MainCoreVer_Address + eep_uc_Log_MainCoreVer_Length)
#define eep_ui_Log_CompStockNo01_Address  (eep_uc_Log_MainCoreRev_Address + eep_uc_Log_MainCoreRev_Length)
#define eep_ui_Log_CompStockNo02_Address  (eep_ui_Log_CompStockNo01_Address + eep_ui_Log_CompStockNo01_Length*2)
#define eep_ui_Log_CompStockNo03_Address  (eep_ui_Log_CompStockNo02_Address + eep_ui_Log_CompStockNo02_Length*2)
#define eep_uc_Log_CompParaVer_Address    (eep_ui_Log_CompStockNo03_Address + eep_ui_Log_CompStockNo03_Length*2)
#define eep_uc_Log_CompParaRev_Address    (eep_uc_Log_CompParaVer_Address + eep_uc_Log_CompParaVer_Length)
#define eep_uc_Log_CompCoreVer_Address    (eep_uc_Log_CompParaRev_Address + eep_uc_Log_CompParaRev_Length)
#define eep_uc_Log_CompCoreRev_Address    (eep_uc_Log_CompCoreVer_Address + eep_uc_Log_CompCoreVer_Length)
#define eep_us_Backup_FRealTimer_Address    (eep_uc_Log_CompCoreRev_Address + eep_uc_Log_CompCoreRev_Length)
#define eep_us_Backup_RRealTimer_Address    (eep_us_Backup_FRealTimer_Address + eep_us_Backup_FRealTimer_Length*2)
#define eep_us_Backup_FDoorOpenTimer_Address    (eep_us_Backup_RRealTimer_Address + eep_us_Backup_RRealTimer_Length*2)
#define eep_us_Backup_RDoorOpenTimer_Address    (eep_us_Backup_FDoorOpenTimer_Address + eep_us_Backup_FDoorOpenTimer_Length*2)
/* Reset log: fixed address, NOT part of the chain above [0x0FC0 - 0xFFF]*/
#define EEPROM_RSTRECORD_START_ADDRESS 0x0FC0
#define eep_uc_RstCnt_Address           (EEPROM_RSTRECORD_START_ADDRESS)
#define eep_uc_RstHdr_Address           (eep_uc_RstCnt_Address + eep_uc_RstCnt_Length)
#define eep_uc_RstRec_01_Address        (eep_uc_RstHdr_Address + eep_uc_RstHdr_Length)
#define eep_uc_RstRec_02_Address        (eep_uc_RstRec_01_Address + eep_uc_RstRec_01_Length)
#define eep_uc_RstRec_03_Address        (eep_uc_RstRec_02_Address + eep_uc_RstRec_02_Length)

/// @brief 

typedef struct 
{
  struct 
  {
    unsigned char Year;
    unsigned char Month;
    unsigned char Day;
  }Date;
  unsigned char PCB_type;
  unsigned char SW_Version;
  struct 
  {
    unsigned char Byte_1;
    unsigned char Byte_2;
    unsigned char Byte_3;
  }PCB_No;
  unsigned char Circuit_Group;

  unsigned char Reserved[6];
 
}stTraceability;

typedef struct
{
    union {
        struct {
            unsigned char FDoor_open:1;     // 1 bit         
            unsigned char eep_uc_Log_Ftemp:7; // -64 to 63 range    
        };
        unsigned char byte; // Access the whole byte directly
   };
} stTemperatureF;


typedef struct
{
    union {
        struct {
            unsigned char RDoor_open:1;
            unsigned char eep_uc_Log_Rtemp:7; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stTemperatureR;

typedef struct
{
    union {
        struct {
            //unsigned char ES_operation:1; //BONA don't have Energy saving but there's similar function is Eco mode.
            unsigned char Eco_flag:1;
            unsigned char eep_uc_Log_Tctemp:7; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stTemperatureTc;

typedef struct
{
    union {
        struct {
            unsigned char Human_Detect:1;
            unsigned char eep_uc_Log_Imtemp:7; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stTemperatureIM;

typedef struct
{
    union {
        struct {
           unsigned char F_Defrost_flag:1;
           unsigned char eep_uc_Log_FDtemp:7; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stTemperatureFEva;

typedef struct
{
    union {
        struct {
            unsigned char R_Defrost_flag:1;
            unsigned char eep_uc_Log_RDtemp:7; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stTemperatureREva;

typedef struct
{
    union {
        struct {
            unsigned char Shabbat_flag:1;
            unsigned char eep_uc_Log_HUMtemp:7; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stHumiditySh;

typedef struct
{
    union {
        struct {
            unsigned char eep_uc_Log_RDoor:4;
            unsigned char eep_uc_Log_FDoor:4; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stDooropen_Cumulativetime;

typedef struct
{
    union {
        struct {
            unsigned char Operation_IM_Mode:1;
            unsigned char Operation_IM_Operation:1;
            unsigned char Operation_IM_Fullice:1;
            unsigned char Operation_IM_Water:1;
            unsigned char Operation_Reset:1;
            unsigned char Operation_Wifimode:1; 
            unsigned char Operation_24h:1; 
            unsigned char Operation_Failure:1; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stOperation_Flag1;

typedef struct
{
    union {
        struct {
            unsigned char Operation_RTemperaturecontrol:4;
            unsigned char Operation_WifiRoutno:1; 
            unsigned char Operation_QuickFreeze:1; 
            unsigned char Operation_QuickCool:1; 
            unsigned char Operation_Mute:1; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stOperation_Flag2;

typedef struct
{
    union {
        struct {
            unsigned char Operation_FTemperaturecontrol:4;
            unsigned char Operation_WifiCardNoConnect:1; 
            unsigned char Operation_VacuumTemp:1; 
            unsigned char Operation_VacuumMotor:1; 
            unsigned char Operation_VacuumMode:1; 
        };
        unsigned char byte; // Access the whole byte directly
    };
} stOperation_Flag3;

typedef struct
{
    union {
        struct {
            unsigned char eep_uc_Log_FValve_time:4; 
            unsigned char eep_uc_Log_RValve_time:4;
        };
        unsigned char byte; // Access the whole byte directly
    };
} stValve_position_Cumulativetime;



typedef struct //Add by STW
{
    union {
        struct {
            unsigned long FCompVal_1999rpm:8; 
            unsigned long FCompVal_2999rpm:8;
            unsigned long FCompVal_3999rpm:8;
            unsigned long FCompVal_4000rpm:8;
        };
        unsigned long byte4; 
    };
} stFComVal_buffer;

typedef struct //Add by STW
{
    union {
        struct {
            unsigned long RCompVal_1999rpm:8; 
            unsigned long RCompVal_2999rpm:8;
            unsigned long RCompVal_3999rpm:8;
            unsigned long RCompVal_4000rpm:8;
        };
        unsigned long byte4; 
    };
} stRComVal_buffer;

typedef struct //Add by STW for apiwat test about CT
{
    union {
        struct {
            unsigned char CT_Mode:1; 
            unsigned char Reserve:6;
            unsigned char ProhibitionUse:1;     //Don't use for confirm CT state (If data FF, maker don't write data)
        };
        unsigned char byte; 
    };
} stTestMode;


typedef struct //Add by APW for reset record
{
    unsigned char CNT_WDT; 
    unsigned char CNT_CORE;
    unsigned char CNT_STACK;
    unsigned char CNT_HARDFAULT;
    unsigned char CNT_PARITY;
    unsigned char CNT_BUS;
    unsigned char CNT_LVD;
    unsigned char CNT_OTHER;
}stRSTRecordCNTMode;

typedef struct //Add by APW for reset record
{
    unsigned char LOG_VER; 
    unsigned char CNT_CRC;
    unsigned char Reserve[6];
} stRSTRecordLOGMode;

typedef struct //Add by APW for reset record
{
    unsigned char seq;              // +0
    unsigned char cause;            // +1  bit 0-6 cause, bit 7 CWSF
    unsigned char RSTSR0;           // +2
    unsigned char RSTSR1[2];        // +3  little endian
    unsigned char FW_VER;           // +5
    unsigned char DAY[2];           // +6  little endian
    unsigned char REPEAT;           // +8
    unsigned char DETAIL[6];        // +9  meaning depends on cause:
                                    //     WDT       [0] proc, [1] event
                                    //     CORE      [0..1] reason, [2..5] data
                                    //     STACK     [0..2] SP, [3] proc, [4] event
                                    //     HARDFAULT [0..2] PC, [3..5] LR
                                    //     other     all 0
    unsigned char CRC;              // +15
} stRSTRecordLASTMode;

///-------------------------------------------------------------------------------------------------------------------------/////
///-------------------------------------------------------------------------------------------------------------------------/////

typedef struct
{
    // General error log information
    unsigned char eep_uc_ErrLog[4];
    unsigned char eep_uc_ErrOfDay[4];

    // Temperature and Humidity Logs
    stTemperatureF tempF;
    stTemperatureR tempR;
    stTemperatureTc tempTc;
    stTemperatureIM tempIM;
    stTemperatureFEva tempFD;
    stTemperatureREva tempRD;
    stHumiditySh humidity;

    // Door Open Cumulative Time Log
    stDooropen_Cumulativetime DOpenTime;

    // Operation Flags
    stOperation_Flag1 flag1;
    stOperation_Flag2 flag2;
    stOperation_Flag3 flag3;

    // Voltage Difference Logs
    unsigned char eep_uc_Log_VDMax; 
    unsigned char eep_uc_Log_VDMin;

    // Cumulative Summation Logs
    unsigned short eep_us_LogSum_Fdoor;
    unsigned short eep_us_LogSum_Rdoor;
    
    unsigned short eep_us_LogSum_FDefCount;
    
    unsigned short eep_us_LogSum_RDefCount;
    
    unsigned short eep_us_LogSum_PwrOnTm;
    
    unsigned short eep_us_Backup_FRealTimer;
    unsigned short eep_us_Backup_RRealTimer;
    unsigned short eep_us_Backup_FDoorOpenTimer;
    unsigned short eep_us_Backup_RDoorOpenTimer;
    // Hour Counter Log
    unsigned char eep_uc_Log_hourcnt;

    // Error History (Old, New, Count)
    unsigned short eep_us_ErrHis_Old[29];
    unsigned short eep_us_ErrHis_New[29];
    unsigned short eep_us_ErrHis_Cnt[29];
    
    // Valve position times
    unsigned long eep_uc_Log_FCompValT;
    unsigned long eep_uc_Log_RCompValT;
    
    //unsigned long  eep_uc_Log_FCompVal_Buf;
   // unsigned long  eep_uc_Log_RCompVal_Buf;
    
    stFComVal_buffer  eep_uc_Log_FCompVal_Buf;
    stRComVal_buffer  eep_uc_Log_RCompVal_Buf;
    
    stValve_position_Cumulativetime eep_uc_Log_ValveFlag;
    
    unsigned short   eep_us_LogSum_Valve;
    unsigned short   eep_us_LogSum_GireMOnTm;
    unsigned short   eep_us_LogSum_WDPOnTm;
    unsigned short   eep_us_LogSum_IDSolOnTm;
    unsigned short   eep_us_LogSum_DDSolOnTm;
          
    stTestMode eep_uc_Log_TestMd;
    unsigned char  eep_uc_Log_DataCnt;
    
    unsigned short  eep_ui_Log_MainStockNo[3];
    unsigned char   eep_uc_Log_MainParaVer;
    unsigned char   eep_uc_Log_MainParaRev;
    unsigned char   eep_uc_Log_MainCoreVer;
    unsigned char   eep_uc_Log_MainCoreRev;
    unsigned short  eep_ui_Log_CompStockNo[3];
    unsigned char   eep_uc_Log_CompParaVer;
    unsigned char   eep_uc_Log_CompParaRev;
    unsigned char   eep_uc_Log_CompCoreVer;
    unsigned char   eep_uc_Log_CompCoreRev;
    
    stRSTRecordCNTMode eep_uc_RstCnt;           // 0x0FC0
    stRSTRecordLOGMode eep_uc_RstHdr;           // 0x0FC8
    stRSTRecordLASTMode eep_uc_RstRec[3];       // 0x0FD0 / 0x0FE0 / 0x0FF0
    
} stEEPROM_Ram;

extern stEEPROM_Ram EEPROM_R;

typedef struct
{
  void *ptr;                      ///< Address of item
  uint8_t size;                   ///< Length of item
  const  uint16_t mem_adress;     ///    starting adress of item in eeprom memory
  const  uint8_t length;          /// length of stored last stored items
} tsBackupItemEE;

#define BACKUP_ITEM_EE(_variable, _varName)        \
    {                                 \
      &_variable, sizeof(_variable), _varName##_Address, _varName##_Length \
    }


//#undef INTERFACE // Should not let this roam free

#endif // FILE_EEPROM_LOG_MAP_H
 

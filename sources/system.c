#include "system.h"
#include "board/board.h"
#include "dev/display_spi.h"
#include "app/refrigerator/rf_configs.h"
#include "dev/synchronizer.h"
#include "app/refrigerator/rf_configs.h"
#include "app/refrigerator/rf_ram.h"
#include "app/refrigerator/refrigerator.h"
#include "app/mw/rf_backup.h"
#include "app/mw/rf_udaq.h"
#include "app/cooler/cooler.h"
#include "app/cabin/cabin.h"
#include "app/ui/ui_proc.h"
//#include "mw/backup.h"
#include "app/mw/h_backup.h" // MJI
#include "dev/inverteruartcomm.h"
#include "app/ice_machine/ice_machine.h"
#include "app/water_source/water_source.h"
#include "app/ice_source/ice_source.h"
#include "app/dispenser/dispenser.h"
#ifdef ENERGYMONITORING
 #include "sources/dev/energymonitoring.h"
#endif

#include "dev/ra2/data_flash.h"
#include "dev/ra2/uart.h"

/* Reset record function */
#ifdef ResetRecord
#include "app/mw/reset_record.h"
#endif

#ifdef HOSHI_H30
unsigned char User_MuteBuzzer = 1;
#endif //HOSHI_H30

stRefrigerator_Ram refrigerator_R;

//cadd cihan note: Bence BACKUP_ITEM olarak degil duz array olarak duzenle asagisini


tsBackupItem backupArray [] = {
    BACKUP_ITEM(refrigerator_R.workMode.E2.preambleBytes[0]),
    BACKUP_ITEM(refrigerator_R.workMode.E2.preambleBytes[1]),
    BACKUP_ITEM(refrigerator_R.workMode.E2.preambleBytes[2]),
    //2

    BACKUP_ITEM(refrigerator_R.workMode.E2.workMode),
    BACKUP_ITEM(refrigerator_R.workMode.E2.fairModeFlag),
    //4
    BACKUP_ITEM(refrigerator_R.E2),
    /*BACKUP_ITEM(refrigerator_R.E2.pingTestState),
    BACKUP_ITEM(refrigerator_R.E2.Sabbath),
    BACKUP_ITEM(refrigerator_R.E2.EcoExtra),
    BACKUP_ITEM(refrigerator_R.E2.keyLock),
    BACKUP_ITEM(refrigerator_R.E2.dispOff),
    BACKUP_ITEM(refrigerator_R.E2.doorsClosedBlocked),
    BACKUP_ITEM(refrigerator_R.E2.Silent),
    BACKUP_ITEM(refrigerator_R.E2.Violed),
    BACKUP_ITEM(refrigerator_R.E2.WifiStatus),
    BACKUP_ITEM(refrigerator_R.E2.Convertible),
    BACKUP_ITEM(refrigerator_R.E2.CabinOff),
    BACKUP_ITEM(refrigerator_R.E2.wifiErrors),
    BACKUP_ITEM(refrigerator_R.E2.tempType),
    BACKUP_ITEM(refrigerator_R.E2.temperatureConvPointer[0]),
    BACKUP_ITEM(refrigerator_R.E2.temperatureConvPointer[1]),
    BACKUP_ITEM(refrigerator_R.E2.temperatureConvPointer[2]),
    BACKUP_ITEM(refrigerator_R.E2.temperatureConvPointer[3]),
    BACKUP_ITEM(refrigerator_R.E2.sabbathTimer),
    BACKUP_ITEM(refrigerator_R.E2.silentTimer),
    BACKUP_ITEM(refrigerator_R.E2.onVTimer),
    BACKUP_ITEM(refrigerator_R.E2.offVTimer),*/
    //37

    BACKUP_ITEM(refrigerator_R.WaterSource.E2.filterTimer),
    BACKUP_ITEM(refrigerator_R.WaterSource.E2.errFilterEnabled),
    //42
    BACKUP_ITEM(refrigerator_R.Dispenser.E2.state),
    BACKUP_ITEM(refrigerator_R.Dispenser.E2.lightTime),
    //44

    /*BACKUP_ITEM(refrigerator_R.IceCreamMachine.E2.iceCream),
    BACKUP_ITEM(refrigerator_R.IceCreamMachine.E2.state),
    BACKUP_ITEM(refrigerator_R.IceCreamMachine.E2.timer),
    BACKUP_ITEM(refrigerator_R.IceCreamMachine.E2.counter),*/
    //48

    BACKUP_ITEM(refrigerator_R.IceMachine.E2.iceOFF),
    BACKUP_ITEM(refrigerator_R.IceMachine.E2.waterPumpTime),
    BACKUP_ITEM(refrigerator_R.IceMachine.E2.QuickIce60),
    //50
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2),
    /*BACKUP_ITEM(refrigerator_R.Cabin[0].E2.Mode),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.FF),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.FRZ),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.Convertible),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.Vacation),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.Rice),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.errDefrostHeaterCounter),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.highTempStatus),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.HighTempIsactive),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.FirstCutoutDoneF),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.workTimer),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.realTimer),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.meanCoolTime),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.meanIdleTime),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.maxFRZTempInCext),
    BACKUP_ITEM(refrigerator_R.Cabin[0].E2.AlgorithmActivationTime),*/
    //84

    BACKUP_ITEM(refrigerator_R.Cabin[1].E2),
   /* BACKUP_ITEM(refrigerator_R.Cabin[1].E2.Mode),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.FF),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.FRZ),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.Convertible),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.Vacation),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.Rice),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.errDefrostHeaterCounter),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.highTempStatus),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.HighTempIsactive),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.FirstCutoutDoneF),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.workTimer),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.realTimer),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.meanCoolTime),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.meanIdleTime),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.maxFRZTempInCext),
    BACKUP_ITEM(refrigerator_R.Cabin[1].E2.AlgorithmActivationTime),*/
    //118
    /* 2024.06.13 4480 Asagidaki degiskenler adresi belli olmadigi icin bu veri/degerler calisma sirasinda surekli degisiyor.
     *
    */
#if _NumOfCabin>2
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.Mode),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.FF),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.FRZ),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.Convertible),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.Vacation),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.Rice),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.errDefrostHeaterCounter),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.highTempStatus),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.HighTempIsactive),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.FirstCutoutDoneF),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.workTimer),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.realTimer),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.meanCoolTime),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.meanIdleTime),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.maxFRZTempInCext),
    BACKUP_ITEM(refrigerator_R.Cabin[2].E2.AlgorithmActivationTime),
    //152
    /* 2024.06.13 4480 Asagidaki degiskenler adresi belli olmadigi icin bu veri/degerler calisma sirasinda surekli degisiyor.
     *
    */
#if _NumOfCabin>3
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.Mode),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.FF),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.FRZ),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.Convertible),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.Vacation),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.Rice),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.errDefrostHeaterCounter),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.highTempStatus),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.HighTempIsactive),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.FirstCutoutDoneF),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.workTimer),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.realTimer),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.meanCoolTime),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.meanIdleTime),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.maxFRZTempInCext),
    BACKUP_ITEM(refrigerator_R.Cabin[3].E2.AlgorithmActivationTime),
#endif
#endif
    //186 if Cabin Num >=3
    //152 if Cabin Num < 3
    BACKUP_ITEM(refrigerator_R.workMode.E2.preambleBytes[3]),
};



DEV_COM_UART_CREATE(compUart, COMP_UART_BUS, comp_callback, 48, 10)   // 600 Baud for compressor inverter card
//PROCESS_INVERTER_UART_COMM_CREATE(inverterCommProc,eProcessInverterCom, compUart, vccDriveEn)   //APW : Disable for new MCU
PROCESS_INVERTER_UART_COMM_CREATE(inverterCommProc,eProcessInverterCom, compUart)
#ifdef ENERGYMONITORING
PROCESS_ENERGYMONITORING_CREATE(energyMonitoringProc, eProcessEnergyMonitoring, &inverterCommProcParams.compressorComm)
#endif


DEV_CPX_SYNCHRONIZER_CREATE(sync, 75, 20)
PROCESS_REFRIGERATOR_CREATE(rfProc, eProcessRefrigerator, &refrigerator_R, &refrigerator_F, &sync)
PROCESS_COOLER_CREATE(coolerProc, eProcessCooler, &refrigerator_R.Cooler, &refrigerator_F.Cooler, &sync)
PROCESS_CABIN_CREATE(cabin0Proc, eProcessCabin0, 0, &refrigerator_R.Cabin[0], &refrigerator_F.Cabin[0], &sync)
PROCESS_CABIN_CREATE(cabin1Proc, eProcessCabin1, 1, &refrigerator_R.Cabin[1], &refrigerator_F.Cabin[1], &sync)
//PROCESS_CABIN_CREATE(cabin2Proc, eProcessCabin2, 2, &refrigerator_R.Cabin[2], &refrigerator_F.Cabin[2], &sync)
//PROCESS_CABIN_CREATE(cabin3Proc, eProcessCabin3, 3, &refrigerator_R.Cabin[3], &refrigerator_F.Cabin[3], &sync)
PROCESS_ICE_MACHINE_CREATE(IceMachineProc, eProcessIceMachine, &refrigerator_R.IceMachine, &refrigerator_F.IceMachine, &sync) 
PROCESS_ICE_SOURCE_CREATE(IceSourceProc, eProcessIceSource,&refrigerator_R.IceSource, &refrigerator_F.IceSource, &sync) 
PROCESS_WATERSOURCE_CREATE(WaterSourceProc, eProcessWaterSource, &refrigerator_R.WaterSource, &refrigerator_F.WaterSource, &sync)
PROCESS_DISPENSER_CREATE(DispenserProc, eProcessDispenser, &refrigerator_R.Dispenser, &refrigerator_F.Dispenser, &sync) 
PROCESS_UI_CREATE(uiProc, eProcessUi, &refrigerator_R.ui, &refrigerator_R, &refrigerator_F, display)    

stEEPROM_Ram EEPROM_R;
const tsBackupItemEE backupList[] = {
    
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_ErrLog[0] ,eep_uc_ErrLog_01),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_ErrLog[1] ,eep_uc_ErrLog_02),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_ErrLog[2] ,eep_uc_ErrLog_03),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_ErrLog[3] ,eep_uc_ErrLog_04),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_ErrOfDay[0] ,eep_uc_ErrOfDay_01),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_ErrOfDay[1] ,eep_uc_ErrOfDay_02),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_ErrOfDay[2] ,eep_uc_ErrOfDay_03),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_ErrOfDay[3] ,eep_uc_ErrOfDay_04),
    BACKUP_ITEM_EE(EEPROM_R.tempF ,eep_uc_Log_Ftemp),
    BACKUP_ITEM_EE(EEPROM_R.tempR ,eep_uc_Log_Rtemp),
    BACKUP_ITEM_EE(EEPROM_R.tempTc ,eep_uc_Log_Tctemp),
    BACKUP_ITEM_EE(EEPROM_R.tempIM ,eep_uc_Log_Imtemp),  
    BACKUP_ITEM_EE(EEPROM_R.tempFD, eep_uc_Log_FDtemp),
    BACKUP_ITEM_EE(EEPROM_R.tempRD, eep_uc_Log_RDtemp),
    BACKUP_ITEM_EE(EEPROM_R.humidity, eep_uc_Log_HUMtemp),
    BACKUP_ITEM_EE(EEPROM_R.DOpenTime, eep_uc_Log_FRDoor),
    BACKUP_ITEM_EE(EEPROM_R.flag1, eep_uc_Log_Flags_1),
    BACKUP_ITEM_EE(EEPROM_R.flag2, eep_uc_Log_Flags_2),
    BACKUP_ITEM_EE(EEPROM_R.flag3, eep_uc_Log_Flags_3),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_VDMax, eep_uc_Log_VDMax),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_VDMin, eep_uc_Log_VDMin),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_Fdoor, eep_us_LogSum_Fdoor),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_Rdoor, eep_us_LogSum_Rdoor),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_FDefCount, eep_us_LogSum_FDefCount),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_RDefCount, eep_us_LogSum_RDefCount), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_PwrOnTm, eep_us_LogSum_PwrOnTm),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_hourcnt, eep_uc_Log_hourcnt), 
    
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[0]  ,eep_us_ErrHis_Old_type_01), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[1]  ,eep_us_ErrHis_Old_type_02), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[2]  ,eep_us_ErrHis_Old_type_03), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[3]  ,eep_us_ErrHis_Old_type_04), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[4]  ,eep_us_ErrHis_Old_type_05), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[5]  ,eep_us_ErrHis_Old_type_06), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[6]  ,eep_us_ErrHis_Old_type_07), //There is no error E6
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[7]  ,eep_us_ErrHis_Old_type_08), //There is no error E7 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[8]  ,eep_us_ErrHis_Old_type_09), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[9]  ,eep_us_ErrHis_Old_type_10), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[10] ,eep_us_ErrHis_Old_type_11), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[11] ,eep_us_ErrHis_Old_type_12), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[12] ,eep_us_ErrHis_Old_type_13), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[13] ,eep_us_ErrHis_Old_type_14), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[14] ,eep_us_ErrHis_Old_type_15), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[15] ,eep_us_ErrHis_Old_type_16), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[16] ,eep_us_ErrHis_Old_type_17), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[17] ,eep_us_ErrHis_Old_type_18), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[18] ,eep_us_ErrHis_Old_type_19), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[19] ,eep_us_ErrHis_Old_type_20), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[20] ,eep_us_ErrHis_Old_type_21), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[21] ,eep_us_ErrHis_Old_type_22), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[22] ,eep_us_ErrHis_Old_type_23), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[23] ,eep_us_ErrHis_Old_type_24), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[24] ,eep_us_ErrHis_Old_type_25), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[25] ,eep_us_ErrHis_Old_type_26), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[26] ,eep_us_ErrHis_Old_type_27), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[27] ,eep_us_ErrHis_Old_type_28), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Old[28] ,eep_us_ErrHis_Old_type_29),
    
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[0]  ,eep_us_ErrHis_New_type_01), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[1]  ,eep_us_ErrHis_New_type_02), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[2]  ,eep_us_ErrHis_New_type_03), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[3]  ,eep_us_ErrHis_New_type_04), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[4]  ,eep_us_ErrHis_New_type_05), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[5]  ,eep_us_ErrHis_New_type_06), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[6]  ,eep_us_ErrHis_New_type_07), //There is no error E6
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[7]  ,eep_us_ErrHis_New_type_08), //There is no error E7
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[8]  ,eep_us_ErrHis_New_type_09), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[9]  ,eep_us_ErrHis_New_type_10), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[10] ,eep_us_ErrHis_New_type_11), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[11] ,eep_us_ErrHis_New_type_12), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[12] ,eep_us_ErrHis_New_type_13), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[13] ,eep_us_ErrHis_New_type_14), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[14] ,eep_us_ErrHis_New_type_15), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[15] ,eep_us_ErrHis_New_type_16), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[16] ,eep_us_ErrHis_New_type_17), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[17] ,eep_us_ErrHis_New_type_18), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[18] ,eep_us_ErrHis_New_type_19), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[19] ,eep_us_ErrHis_New_type_20), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[20] ,eep_us_ErrHis_New_type_21), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[21] ,eep_us_ErrHis_New_type_22), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[22] ,eep_us_ErrHis_New_type_23), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[23] ,eep_us_ErrHis_New_type_24), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[24] ,eep_us_ErrHis_New_type_25), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[25] ,eep_us_ErrHis_New_type_26), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[26] ,eep_us_ErrHis_New_type_27), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[27] ,eep_us_ErrHis_New_type_28), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_New[28] ,eep_us_ErrHis_New_type_29),  
   
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[0]  ,eep_us_ErrHis_Cnt_type_01), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[1]  ,eep_us_ErrHis_Cnt_type_02), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[2]  ,eep_us_ErrHis_Cnt_type_03), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[3]  ,eep_us_ErrHis_Cnt_type_04), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[4]  ,eep_us_ErrHis_Cnt_type_05), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[5]  ,eep_us_ErrHis_Cnt_type_06), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[6]  ,eep_us_ErrHis_Cnt_type_07), //There is no error E6
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[7]  ,eep_us_ErrHis_Cnt_type_08), //There is no error E7
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[8]  ,eep_us_ErrHis_Cnt_type_09), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[9]  ,eep_us_ErrHis_Cnt_type_10), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[10] ,eep_us_ErrHis_Cnt_type_11), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[11] ,eep_us_ErrHis_Cnt_type_12), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[12] ,eep_us_ErrHis_Cnt_type_13), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[13] ,eep_us_ErrHis_Cnt_type_14), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[14] ,eep_us_ErrHis_Cnt_type_15), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[15] ,eep_us_ErrHis_Cnt_type_16), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[16] ,eep_us_ErrHis_Cnt_type_17), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[17] ,eep_us_ErrHis_Cnt_type_18), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[18] ,eep_us_ErrHis_Cnt_type_19), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[19] ,eep_us_ErrHis_Cnt_type_20), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[20] ,eep_us_ErrHis_Cnt_type_21), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[21] ,eep_us_ErrHis_Cnt_type_22), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[22] ,eep_us_ErrHis_Cnt_type_23), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[23] ,eep_us_ErrHis_Cnt_type_24), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[24] ,eep_us_ErrHis_Cnt_type_25), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[25] ,eep_us_ErrHis_Cnt_type_26), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[26] ,eep_us_ErrHis_Cnt_type_27), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[27] ,eep_us_ErrHis_Cnt_type_28), 
    BACKUP_ITEM_EE(EEPROM_R.eep_us_ErrHis_Cnt[28] ,eep_us_ErrHis_Cnt_type_29), 
      
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_FCompValT ,eep_uc_Log_FCompValT),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_RCompValT ,eep_uc_Log_RCompValT),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_FCompVal_Buf ,eep_uc_Log_FCompVal_Buf),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_RCompVal_Buf ,eep_uc_Log_RCompVal_Buf),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_ValveFlag ,eep_uc_Log_ValveFlag),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_Valve ,eep_us_LogSum_Valve),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_GireMOnTm ,eep_us_LogSum_GireMOnTm),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_WDPOnTm ,eep_us_LogSum_WDPOnTm),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_IDSolOnTm ,eep_us_LogSum_IDSolOnTm),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_LogSum_DDSolOnTm ,eep_us_LogSum_DDSolOnTm),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_TestMd ,eep_uc_Log_TestMd),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_DataCnt ,eep_uc_Log_DataCnt),
    BACKUP_ITEM_EE(EEPROM_R.eep_ui_Log_MainStockNo[0] ,eep_ui_Log_MainStockNo01),
    BACKUP_ITEM_EE(EEPROM_R.eep_ui_Log_MainStockNo[1] ,eep_ui_Log_MainStockNo02),
    BACKUP_ITEM_EE(EEPROM_R.eep_ui_Log_MainStockNo[2] ,eep_ui_Log_MainStockNo03),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_MainParaVer ,eep_uc_Log_MainParaVer),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_MainParaRev ,eep_uc_Log_MainParaRev),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_MainCoreVer ,eep_uc_Log_MainCoreVer),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_MainCoreRev ,eep_uc_Log_MainCoreRev),
    BACKUP_ITEM_EE(EEPROM_R.eep_ui_Log_CompStockNo[0] ,eep_ui_Log_CompStockNo01),
    BACKUP_ITEM_EE(EEPROM_R.eep_ui_Log_CompStockNo[1] ,eep_ui_Log_CompStockNo02),
    BACKUP_ITEM_EE(EEPROM_R.eep_ui_Log_CompStockNo[2] ,eep_ui_Log_CompStockNo03),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_CompParaVer ,eep_uc_Log_CompParaVer),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_CompParaRev ,eep_uc_Log_CompParaRev),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_CompCoreVer ,eep_uc_Log_CompCoreVer),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_Log_CompCoreRev ,eep_uc_Log_CompCoreRev),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_Backup_FRealTimer ,eep_us_Backup_FRealTimer),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_Backup_RRealTimer ,eep_us_Backup_RRealTimer),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_Backup_FDoorOpenTimer ,eep_us_Backup_FDoorOpenTimer),
    BACKUP_ITEM_EE(EEPROM_R.eep_us_Backup_RDoorOpenTimer ,eep_us_Backup_RDoorOpenTimer),
    
    /* Reset log (fixed 0x0FC0-0x0FFF): listed for the map only. Arrays are skipped by
     * updateField(); reset_record.c reads them in RSTCommit() and writes them in RSTTick20ms(). */
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_RstCnt    ,eep_uc_RstCnt),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_RstHdr    ,eep_uc_RstHdr),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_RstRec[0] ,eep_uc_RstRec_01),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_RstRec[1] ,eep_uc_RstRec_02),
    BACKUP_ITEM_EE(EEPROM_R.eep_uc_RstRec[2] ,eep_uc_RstRec_03),
};

PROCESS_BACKUP_CREATE(eepromFlashBackup, eepromDev, backupList, 100, &EEPROM_R, &refrigerator_R) // MJI
PROCESS_EEPROM_FILEDS_CREATE(eepromFields, eProcessEEprom, &EEPROM_R, &inverterCommProcParams, &refrigerator_R, &filteredAdcRSEN, &filteredAdcRDSEN, &filteredAdcFSEN, &filteredAdcFDSEN, &filteredAdcIMSEN, &filteredAdcTCSEN ,&filteredAdcHSEN)
DEV_CPX_FLASH_CREATE(flashCoreDevice, EEPROM)
#pragma location = 0x40100000U
DEV_MEM_SECTION_CREATE(emulatedEepromSection, ".emulated_eeprom_1_section", 256, flashCoreDevice)
PROCESS_RF_BACKUP_CREATE(rfBackupProc, eProcessBackup, &emulatedEepromSection, backupArray, 500, 1)      //x, 1) -> x ms de bir tum ram'i eeproma yazar. x, 0)-> periyodik yazma kapanir


tsProcess *sysProcesses[] = {
    NULL,
    &rfProc,
    &uiProc,
    &rfBackupProc, // New eeprom implementation should be add
    &inverterCommProc,
#ifdef ENERGYMONITORING
    &energyMonitoringProc,
#endif
    &coolerProc,
    &IceMachineProc,
    &IceSourceProc,
    &WaterSourceProc,
    &DispenserProc,
    &cabin0Proc,
    &cabin1Proc,
    //&cabin2Proc,
    //&cabin3Proc,
};



/**
 * @brief Common function used by processes to change their states.
 *        Changes process's state handler and posts eSysEventEnterState
 *        event to this process.
 *
 * @param process pointer to process that its state will be changed
 * @param stateHandler pointer to new the state handler function
 */
void sysProcChangeState(tsProcess *process, tfProcessHandler stateHandler)
{
    if (PROCESS_STATE_CURRENT(process) != stateHandler)
    {
        PROCESS_STATE_CHANGE(process, stateHandler);
        eventPost(process->enumeration, eSysEventEnterState, NULL, 0);
    }
}

/**
 * @brief Asks all system buzzers to perform a command (play a melody)
 *
 * @param cmd command or melody from teSystemBuzzerCommands enum
 */
void sysBuzzerCmd(teSystemBuzzerCommands cmd)
{
    devDspSendCommand(&display, (teMainBoardCommand)cmd);     
#ifdef HOSHI_H30
    if (refrigerator_F.Parameters.onBoardBuzzer && User_MuteBuzzer)
#else      
    if (refrigerator_F.Parameters.onBoardBuzzer)  
#endif //HOSHI_H30      
    {

        if (cmd == eSysBuzzerPlayDoorsAlertMelody)
            cmd = eSysBuzzerPlayWarningMelody;
        devIoPut(&buzzer, cmd);
    }
}



/// @brief Starts booting the system
void sysStart(void)
{
#ifdef ResetRecord
    RSTBootCapture();        // Read RSTSR0/1/2 and the no-init RAM before anything runs
#endif
    boardInitBoard();
#ifdef ResetRecord
    RSTInstallHooks();       // stack monitor NMI callback setting
#endif
    devCpxInit(&sync, NULL);
    processStart(&rfProc);
    processStart(&inverterCommProc);
    processStart(&rfBackupProc);
    processStart(&rfUdaqProc);
    processStart(&eepromFlashBackup);
    processStart(&eepromFields);
    // TODO start connectivity process
    
}

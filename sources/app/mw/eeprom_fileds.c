/** @file       eeprom_fileds.c
 *  @brief      EEPROM_FILEDS application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Saqib Sohail
 *  @date       9/4/2024
 */
#define FILE_EEPROM_FILEDS_C

#include "eeprom_fileds.h"
#include "app/mw/connectivity/remote_control.h"

#define DEBUG_FILE_NAME ""

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup EEPROM_FILEDS
 *  @{
 */
/*
static PT_THREAD(EEPROMFiledsThread)
{
    tsEEPROMFiledsParams *params       = process->parameters;
    const tsEEPROMFiledsConsts *consts = process->constants;

    PT_BEGIN();

    for (;;)
    {
        PT_WHILE();
        PT_YIELD();
    }

    PT_END();
}
*/
extern const stRefrigerator_Flash refrigerator_F;

static PROCESS_HANDLER_PROTO(EEPROMFiledsDefaultStateHandler);
static PROCESS_HANDLER_PROTO(EEPROM1SecTask);
static PROCESS_HANDLER_PROTO(EEPROM1MinTask);
static PROCESS_HANDLER_PROTO(EEPROM2HrTask);
static void FailureHistoryUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void FTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void RTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void TCTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void IMTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);           //H20
static void FDTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void RDTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void HumTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);          //H30
static void DoorOpenTimeUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void OpFlag1Update(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void LogTestMdAndDataCntUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);        //APW : Update I2C driver CT code to global
static void OpFlag2Update(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void OpFlag3Update(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void SupplyMaxMin(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void FRDoorOpeningSum(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void DefrostTimes(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void WorkingDays(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void CountEvery2hr(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void CompVolPerDays(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void ValvePosition2hr(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void FRDoorCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);  //add in 0000000000_V00R00_V00R12
static void ValveOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void GearMotorOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void WaterDispenserOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void IceCrusheOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void IceCubeOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void FailureFrequencyUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void ClearFlags(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void SoftVersion(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);
static void RealTimerBackup(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params);

void push_new_error(tsError *errorList, uint8_t size, tsError newError);
tsError pop_error(tsError *errorList, uint8_t size);
tsError below_error(tsError *errorList, uint8_t size);
void update_time(tsError *errorList, uint8_t size);
/// @brief  Initialization function of EEPROMFileds
PROCESS_INIT_PROTO(EEPROMFiledsInit)
{
    tsEEPROMFiledsParams *params       = process->parameters;
    const tsEEPROMFiledsConsts *consts = process->constants;

  //  UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    timerEventStart(&params->te.periodic20Ms, 20);
    timerEventStart(&params->te.periodic1Sec, 1000);
    timerEventStart(&params->te.periodic1Min, 60 * 1000);
    PROCESS_STATE_CHANGE(process, EEPROMFiledsDefaultStateHandler);
    
    if(refrigerator_F.Sets.Model_Select_EEPROM_Record == 0)
      params->MODEL_SEL = 0; //FUJIGEN2
    else if(refrigerator_F.Sets.Model_Select_EEPROM_Record == 1)
      params->MODEL_SEL = 1; //HOSHIGEN2_H10
    else if(refrigerator_F.Sets.Model_Select_EEPROM_Record == 2)
      params->MODEL_SEL = 2; //HOSHIGEN2_H30
    else if(refrigerator_F.Sets.Model_Select_EEPROM_Record == 3)
      params->MODEL_SEL = 3; //BONA
    else if(refrigerator_F.Sets.Model_Select_EEPROM_Record == 4)
      params->MODEL_SEL = 4; //ALPS
    else
      params->MODEL_SEL = 5; //Non
    
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of EEPROMFileds
PROCESS_DEINIT_PROTO(EEPROMFiledsDeinit)
{
    tsEEPROMFiledsParams *params       = process->parameters;
    const tsEEPROMFiledsConsts *consts = process->constants;

   // UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    timerEventStop(&params->te.periodic20Ms);
    timerEventStop(&params->te.periodic1Sec);
    timerEventStop(&params->te.periodic1Min);
    PROCESS_STATE_CHANGE(process, NULL);
   
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of EEPROMFileds
static PROCESS_HANDLER_PROTO(EEPROMFiledsDefaultStateHandler)
{
    tsEEPROMFiledsParams *params       = process->parameters;
    const tsEEPROMFiledsConsts *consts = process->constants;

   //UNUSED(params); // REMOVE IF USED
   UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
    case eEEPROMEventPeriodic20Ms:
      //  EEPROM20MsTask(consts, params);
        timerEventStart(&params->te.periodic20Ms, 20);
        break;

    case eEEPROMEventPeriodic1Sec:
        EEPROM1SecTask(process);
        timerEventStart(&params->te.periodic1Sec, 1000);
        break;

    case eEEPROMEventPeriodic1Min:
        EEPROM1MinTask(process);
        timerEventStart(&params->te.periodic1Min, 60 * 1000);
        break;
        
    case eEEPROMEventUpdateComplete:    // Event generated by H-backup process when 2-hourly update is complete
        EEPROM2HrTask(process);
        break;

        default:
            break;
    }


}

static PROCESS_HANDLER_PROTO(EEPROM1SecTask)
{
    tsEEPROMFiledsParams *params       = process->parameters;
    const tsEEPROMFiledsConsts *consts = process->constants;

  //  UNUSED(params); // REMOVE IF USED
  //  UNUSED(consts); // REMOVE IF USED

  FTempUpdate(consts, params);
  RTempUpdate(consts, params);
  TCTempUpdate(consts, params);
  IMTempUpdate(consts, params); //H20
  FDTempUpdate(consts, params);
  RDTempUpdate(consts, params);
  HumTempUpdate(consts, params); //H30
  DoorOpenTimeUpdate(consts, params);
  OpFlag1Update(consts, params);
  LogTestMdAndDataCntUpdate(consts, params);                                    //APW : Update I2C driver CT code to global
  OpFlag2Update(consts, params);
  OpFlag3Update(consts, params);
  SupplyMaxMin(consts, params);
  FRDoorOpeningSum(consts, params);
  DefrostTimes(consts, params);
  WorkingDays(consts, params);
  ValvePosition2hr(consts, params);
  FRDoorCnt(consts, params);  //add in 0000000000_V00R00_V00R12
  /*** APW : Update I2C driver CT code to global ***/
  if(consts->rf->ui.states.commercial.status == _UI_Commercial_None)
  {
    CompVolPerDays(consts, params);
    FailureFrequencyUpdate(consts, params);
  }
  /*** APW : Update I2C driver CT code to global ***/
  ValveOperatCnt(consts, params);
  GearMotorOperatCnt(consts, params);
  WaterDispenserOperatCnt(consts, params);
  IceCrusheOperatCnt(consts, params);
  IceCubeOperatCnt(consts, params);
  SoftVersion(consts, params);
  RealTimerBackup(consts, params);
}
static PROCESS_HANDLER_PROTO(EEPROM1MinTask)
{
    tsEEPROMFiledsParams *params       = process->parameters;
    const tsEEPROMFiledsConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
        case EVENT_NONE: // FILL WITH NEW EVENTS
            break;

        default:
            break;
    }
}
static PROCESS_HANDLER_PROTO(EEPROM2HrTask)
{
    tsEEPROMFiledsParams *params       = process->parameters;
    const tsEEPROMFiledsConsts *consts = process->constants;
//    UNUSED(params); // REMOVE IF USED
//    UNUSED(consts); // REMOVE IF USED
// call function to clear all 2-hourly flags
    CountEvery2hr(consts, params);
    FailureHistoryUpdate(consts, params);
    ClearFlags(consts, params);
    
   
}
static void FailureHistoryUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  
  update_time(params->errorQueue,TOTAL_ERRORS);
  update_time(params->EEpromErrorImage,4);
  //tsError NewError = pop_error(params->errorQueue,TOTAL_ERRORS);
  tsError NewError = below_error(params->errorQueue,TOTAL_ERRORS);
  if (NewError.errorNumber)
    push_new_error(params->EEpromErrorImage,4, NewError);
    for (uint8_t i=0; i<4; i++)
    {
      params->eepr->eep_uc_ErrLog[i] = params->EEpromErrorImage[i].errorNumber;
      if(params->eepr->eep_uc_ErrOfDay[i] < 0xFF)
      params->eepr->eep_uc_ErrOfDay[i] = params->EEpromErrorImage[i].daysPassed;
    }
}
/********************************* F temperature and F door flag *********************************/
static void FTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{ 
  uint8_t temp = ((uint8_t)(devIoGet(params->dev.filteredAdcFSEN)/4));
  temp = temp>>1;
  params->eepr->tempF.eep_uc_Log_Ftemp = temp;
  params->eepr->tempF.FDoor_open |= consts->rf->Cabin[1].components.doorSwitch[0].value;//Set F door flag
}
/********************************* R temperature and R door flag *********************************/
static void RTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  uint8_t temp = ((uint8_t)(devIoGet(params->dev.filteredAdcRSEN)/4));
  temp = temp>>1;
  params->eepr->tempR.eep_uc_Log_Rtemp = temp;
  params->eepr->tempR.RDoor_open |= consts->rf->Cabin[0].components.doorSwitch[0].value; //Set R door flag
}
/********************************* TC temperature and Eco flag *********************************/
static void TCTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  uint8_t temp = ((uint8_t)(devIoGet(params->dev.filteredAdcTCSEN)/4));
  temp = temp>>1;
  params->eepr->tempTc.eep_uc_Log_Tctemp = temp;
  params->eepr->tempTc.Eco_flag |= consts->rf->Sets.user.functions.EcoExtra; //Set Energy Saving door flag
}
/********************************* IM temperature and HUMAN detected flag *********************************/
static void IMTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params) //Start H20
{
  uint8_t temp = ((uint8_t)(devIoGet(params->dev.filteredAdcIMSEN)/4));
  temp = temp>>1;
  params->eepr->tempIM.eep_uc_Log_Imtemp = temp;
  params->eepr->tempIM.Human_Detect |= consts->rf->Cabin[0].components.humanSensor.value; //Set Human sensor detected H50
}
/********************************* F defrost temperature and F defrost flag *******************************/
static void FDTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
   uint8_t temp = ((uint8_t)(devIoGet(params->dev.filteredAdcFDSEN)/4));
   temp = temp>>1;
   params->eepr->tempFD.eep_uc_Log_FDtemp = temp;
   
   //F defrost flag see in DeforstTimes(consts, params) function
}
/********************************* R defrost temperature and R defrost flag *******************************/
static void RDTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{

  if(params->MODEL_SEL == 3)    //BONA
  {
   uint8_t temp = ((uint8_t)(devIoGet(params->dev.filteredAdcRDSEN)/4));
   temp = temp>>1;
   params->eepr->tempRD.eep_uc_Log_RDtemp = temp;
    //R defrost flag see in DeforstTimes(consts, params) function
   }
   if(params->MODEL_SEL == 2 || params->MODEL_SEL == 4)   //HOSHI_H30 OR ALPS
   { //There is no RDTempUpdate in Hoshi model.
     return;
   }
}

/********************************* Humidity and Shabbath flag *******************************/
static void HumTempUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  if(params->MODEL_SEL == 3)    //BONA H30-50
  {
   uint8_t temp = ((uint8_t)(devIoGet(params->dev.filteredAdcHSEN)/4));
   temp = temp>>1;
   params->eepr->humidity.eep_uc_Log_HUMtemp = temp;
  }
  params->eepr->humidity.Shabbat_flag |= consts->rf->Sets.user.functions.Sabbath;
}

/********************************* Door Open Time Update *********************************/
static void DoorOpenTimeUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params) 
{
  if(consts->rf->Cabin[1].components.doorSwitch[0].value == _SwitchValue_OPEN && 
     consts->rf->ui.states.commercial.status == _UI_Commercial_None) //F foor is open (APW add state confirm Normal or CT)
  {
    if(params->FDoorOpenCntTimer1s < 900) //F door open under 15 min
    params->FDoorOpenCntTimer1s++;
    params->eepr->DOpenTime.eep_uc_Log_FDoor = (params->FDoorOpenCntTimer1s + 30)/60 ; //over 30 second rounded off  
  }
  
  if(consts->rf->Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN && 
     consts->rf->ui.states.commercial.status == _UI_Commercial_None) //R foor is open (APW add state confirm Normal or CT)
  {
   if(params->RDoorOpenCntTimer1s < 900) //R door open under 15 min
    params->RDoorOpenCntTimer1s ++;
    params->eepr->DOpenTime.eep_uc_Log_RDoor = (params->RDoorOpenCntTimer1s + 30)/60 ; //over 30 second rounded off  
  }
  
/*** APW : Update I2C driver for add CT state ***/
  if(consts->rf->Cabin[1].components.doorSwitch[0].value == _SwitchValue_OPEN && 
     consts->rf->ui.states.commercial.status == _UI_Commercial_Test) //F foor is open
  {
    if(params->FDoorOpenCntTimer1s*60 < 900) //F door open under 15 sec
    params->FDoorOpenCntTimer1s++;
    params->eepr->DOpenTime.eep_uc_Log_FDoor = (params->FDoorOpenCntTimer1s*60 + 30)/60 ; 
  }
  
  if(consts->rf->Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN && 
     consts->rf->ui.states.commercial.status == _UI_Commercial_Test) //R foor is open
  {
   if(params->RDoorOpenCntTimer1s*60 < 900) //R door open under 15 sec
    params->RDoorOpenCntTimer1s ++;
    params->eepr->DOpenTime.eep_uc_Log_RDoor = (params->RDoorOpenCntTimer1s*60 + 30)/60 ; 
  }
/*** APW : Update I2C driver for add CT state ***/
}
/********************************* Operation Flag 1 *********************************/
static void OpFlag1Update(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)//Operation flag 1
{
  //Bit 7 : Failure flag
/* for (unsigned char i = 0; i < sizeof(consts->rf->Values.errors.errorsCheckTable); i++) 
 { 
  if (consts->rf->Values.errors.errorsCheckTable[i] > 0) //Check there is error.
    params->eepr->flag1.Operation_Failure = 1;
 }
*/
  //Bit 6 : 24hr flag
   if(params->eepr->eep_uc_Log_hourcnt >= 11) // 22 hr flag is 1 for waiting wtire when 24 hr reached
    params->eepr->flag1.Operation_24h = 1;
  //Bit 5 : Wifi mode flag  ---> H30-50
   /* if(params->MODEL_SEL == 3 || params->MODEL_SEL == 4)      //BONA and ALPS
    {
      if(consts->rf->E2.WifiStatus)
        params->eepr->flag1.Operation_Wifimode = 1;
    }*/
  //Bit 4 : Reset flag 
         // See in h_backup.c as initial function.
  //Bit 3 : IM water empty flag  ---> H20
    if(consts->rf->WaterSource.values.waterTankEmpty)
      params->eepr->flag1.Operation_IM_Water = 1;
  //Bit 2 : IM full ice flag  ---> H20
    if(consts->rf->IceMachine.values.state == _IMch_State_BoxFull)
      params->eepr->flag1.Operation_IM_Fullice = 1;
  //Bit 1 : IM operation flag  ---> H20
    if(consts->rf->IceMachine.values.state == _IMch_State_PureIce)
      params->eepr->flag1.Operation_IM_Operation = 1;
  //Bot 0 : IM mode flag  ---> H20
    if(refrigerator_F.IceMachine.type == _IceMachineType_DEFAULT)
    {
      if(consts->rf->IceMachine.sets.iceOFF == 0x00)
      params->eepr->flag1.Operation_IM_Mode = 1;
    }
    
}

/*** APW : Update I2C driver for add CT state ***/
static void LogTestMdAndDataCntUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params){
  params->eepr->eep_uc_Log_TestMd.CT_Mode = consts->rf->ui.states.commercial.FlagCommercialEnable;
}
/*** APW : Update I2C driver for add CT state ***/

/********************************* Operation Flag 2 *********************************/
static void OpFlag2Update(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)//Operation flag 2
{
  //Bit 7 : Mute -> There is no function in arcelik software.
  //Bit 6 : Quick Cool
   if(consts->rf->Cabin[0].sets.user.functions.Quick == 0x01)
     params->eepr->flag2.Operation_QuickCool = 1;
  //Bit 5 : Quick Freeze
   if(consts->rf->Cabin[1].sets.user.functions.Quick == 0x01)
     params->eepr->flag2.Operation_QuickFreeze = 1;
  //Bit 4 : Wifi No Connect with Router   ---> H30-50
  /* if(params->MODEL_SEL == 3 || params->MODEL_SEL == 4)         //BONA
   {
   if((consts->rf->E2.WifiStatus) && (connectionState == EV_WIFI_MODE_GET_CONNECTING))
    {
      if(params->timeCheckRouter++ > 20)
      { // Get connect more than 20 sec -> No Wifi router
       params->eepr->flag2.Operation_WifiRoutno = 1;
       params->timeCheckRouter = 0;
      }
    }
   }*/
  //Bit 0-3 : R temperature control
   if(params->MODEL_SEL == 3)           //BONA
   {
   switch(consts->rf->Cabin[0].sets.user.FF)
   {
     case 0x06 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x01; break; // 1C
     case 0x07 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x02; break; // 2C
     case 0x00 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x03; break; // 3C
     case 0x01 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x04; break; // 4C
     case 0x02 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x05; break; // 5C
     case 0x03 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x06; break; // 6C  
     case 0x04 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x07; break; // 7C
     case 0x05 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x08; break; // 8C
   }
   }
   if(params->MODEL_SEL == 2)           //HOSHI_H30
   {
   switch(consts->rf->Cabin[0].sets.user.FF)
   {
     case 0x05 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x01; break; // MAX 
     case 0x06 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x02; break; 
     case 0x07 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x03; break; 
     case 0x08 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x04; break; 
     case 0x00 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x05; break; // MID
     case 0x01 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x06; break; 
     case 0x02 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x07; break;  
     case 0x03 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x08; break; 
     case 0x04 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x09; break; // MIN
   }
   }
   
   if(params->MODEL_SEL == 4)           //ALPS
   {
   switch(consts->rf->Cabin[0].sets.user.FF)
   {
     case 0x05 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x02; break; // MAX        2  
     case 0x06 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x03; break; //            3
     case 0x00 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x04; break; // Max-Mid    4
     case 0x01 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x05; break; //            5
     case 0x02 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x06; break; // Mid-Min    6
     case 0x03 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x07; break; //            7
     case 0x04 : params->eepr->flag2.Operation_RTemperaturecontrol = 0x08; break; // MIN        8
   }
   }
}

/********************************* Operation Flag 3 *********************************/
static void OpFlag3Update(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)//Operation flag 3
{
   if(params->MODEL_SEL == 3)           //BONA
   {
  //Bit 7 : Vacuum mode  --> H50
   if(consts->rf->Cabin[0].values.VacuumBox.sets != _VacuumSet_Off)
    params->eepr->flag3.Operation_VacuumMode = 1;
  //Bit 6 : Vacuum motor  --> H50
   if(consts->rf->Cabin[0].values.VacuumBox.state == _VacuumState_ON ||
        consts->rf->Cabin[1].values.VacuumBox.state == _VacuumState_ON)
    params->eepr->flag3.Operation_VacuumMotor = 1;
  //Bit 5 : Vacuum temp (-1C/1C)  --> H50
   if(consts->rf->Cabin[0].values.VacuumBox.sets == _VacuumSet_Off)
    params->eepr->flag3.Operation_VacuumTemp = 1;       //When vacuum not active, defualt to 1 (is +1)
   else
   {
      if(consts->rf->Cabin[0].values.VacuumBox.sets == _VacuumSet_MinusOne)
        params->eepr->flag3.Operation_VacuumTemp = 0;   //-1 is 0
      else
        params->eepr->flag3.Operation_VacuumTemp = 1;   //+1 is 1
   }
  //Bit 4 : Wifi card no connect with PCB  -->H30-50
  /* if((consts->rf->E2.WifiStatus) && (connectionState == EV_WIFI_MODE_NOT_CONNECTED)) //Wifi on but wifi not connect
   {
     if(params->WifiConnectimer++ > 20) //check connection 20 sec refer  : Connectivity_T1sec(void)
     {
       params->eepr->flag3.Operation_WifiCardNoConnect = 1;
       params->WifiConnectimer = 0;
     }
   }*/
  //Bit 0-3 : F temperature control
   switch(consts->rf->Cabin[1].sets.user.FRZ)
   {
     case 0x04 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x01; break; // -22C //-24
     case 0x05 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x02; break; // -21C //-23
     case 0x06 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x03; break; // -20C //-22
     case 0x00 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x04; break; // -19C //-21
     case 0x01 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x05; break; // -18C //-20
     case 0x02 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x06; break; // -17C //-19
     case 0x03 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x07; break; // -16C //-18
   }
   }
   
   if(params->MODEL_SEL == 2)           //HOSHI_H30
   {
   //Bit 7-5 : N have  --> HOSHI
   //Bit 0-3 : F temperature control
   switch(consts->rf->Cabin[1].sets.user.FRZ)
   {
     case 0x05 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x01; break; // MAX
     case 0x06 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x02; break;      
     case 0x07 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x03; break;      
     case 0x08 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x04; break;      
     case 0x00 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x05; break; // MID     
     case 0x01 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x06; break;      
     case 0x02 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x07; break;       
     case 0x03 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x08; break;      
     case 0x04 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x09; break; // MIN     
   }
   }
   
   if(params->MODEL_SEL == 4)           //ALPS
   {
   //Bit 7-5 : N have  --> ALPS
   //Bit 4 : Wifi card no connect with PCB  --> ALPS
   /*if((consts->rf->E2.WifiStatus) && (connectionState == EV_WIFI_MODE_NOT_CONNECTED)) //Wifi on but wifi not connect
   {
     if(params->WifiConnectimer++ > 20) //check connection 20 sec refer  : Connectivity_T1sec(void)
     {
       params->eepr->flag3.Operation_WifiCardNoConnect = 1;
       params->WifiConnectimer = 0;
     }
   }*/
   //Bit 0-3 : F temperature control
   switch(consts->rf->Cabin[1].sets.user.FRZ)
   {
     case 0x05 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x01; break; // MAX        -24 
     case 0x06 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x02; break; //            -22
     case 0x00 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x03; break; // Max-Mid    -20  
     case 0x01 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x04; break; //            -19
     case 0x02 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x05; break; // Mid-Min    -18 
     case 0x03 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x06; break; //            -17
     case 0x04 : params->eepr->flag3.Operation_FTemperaturecontrol = 0x07; break; // MIN        -16     
   }
   }
}

/********************************* Power supply voltage Max/Min *********************************/
static void SupplyMaxMin(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params) // Later : There is no compressor MCU in debug board
{
    static bool isCompressorRunning = false; 
    static uint8_t invalidtime = 0; 
    static bool invalidRecordStatus = true; 
    static uint8_t voltage=0; 

    // Check compressor stop
    if (consts->rf->Cooler.components.compressor.Readvalue == 0) { //(consts->rf->Cooler.components.compressor.value == 0) {
        isCompressorRunning = false; //Compressor is stop
    }

    // Check compressor is stared
    if (!isCompressorRunning && consts->rf->Cooler.components.compressor.Readvalue > 0) { //(!isCompressorRunning && consts->rf->Cooler.components.compressor.value > 0) { 
        isCompressorRunning = true; // Compressor started
        invalidtime = 0; // reset timer invalid time
    }

    // If the compressor is running, check the first 5 seconds. 
    if (isCompressorRunning) {
        if (invalidtime < 5) {
            invalidtime++; //add time 
            invalidRecordStatus = false; // not store in 5 second
        } else {
            invalidRecordStatus = true; // After 5 seconds, it can be store.
        }
    }
    
     voltage = (uint8_t)((params->comp->compressorComm.EKIPackets.InverterDCbusVoltage*0.005656566*255)/5);  
     
     if(refrigerator_F.Sets.voltageInput == 110) //adc value for 110V
      voltage >>= 1;  // power/2
    
    // If it is in a state where it can be saved
    if ((invalidRecordStatus) && (voltage > 0)) {
        // Record Voltage
       if(voltage > params->eepr->eep_uc_Log_VDMax)
       {
           params->eepr->eep_uc_Log_VDMax = voltage; //Max_voltage
       }
       if(voltage < params->eepr->eep_uc_Log_VDMin)
       {
           params->eepr->eep_uc_Log_VDMin = voltage;  //Min_voltage
       }  
       
    }
  

}

/********************************* Number of F/R door opening times *********************************/
static void FRDoorOpeningSum(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  //F door open
  static bool FdoorCloseSts = false;
  
   params->CumulativeFdoorOpen = htons(params->eepr->eep_us_LogSum_Fdoor);
  
    if(consts->rf->Cabin[1].components.doorSwitch[0].value == _SwitchValue_CLOSE )
     FdoorCloseSts = true;
    
    if((FdoorCloseSts) && (consts->rf->Cabin[1].components.doorSwitch[0].value == _SwitchValue_OPEN))
    {
       if(++params->FDoorOpenSum >= 10 ) //F door open 10 times = 1 hex in EEPROM
        { 
          if(params->CumulativeFdoorOpen < 0xFFFF) //Max value
               params->CumulativeFdoorOpen ++;
               params->eepr->eep_us_LogSum_Fdoor = htons(params->CumulativeFdoorOpen);
           params->FDoorOpenSum = 0;
        }
       FdoorCloseSts = false;
    }
  //R door open
    static bool RdoorCloseSts = false;
    
    params->CumulativeRdoorOpen = htons(params->eepr->eep_us_LogSum_Rdoor);
    
    if(consts->rf->Cabin[0].components.doorSwitch[0].value == _SwitchValue_CLOSE )
    RdoorCloseSts = true;
    
    if((RdoorCloseSts) && (consts->rf->Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN) )
    {
        if(++params->RDoorOpenSum >= 10 ) //R door open 10 times = 1 hex in EEPROM
        { 
          if(params->CumulativeRdoorOpen < 0xFFFF) //Max value
            params->CumulativeRdoorOpen ++;
            params->eepr->eep_us_LogSum_Rdoor = htons(params->CumulativeRdoorOpen);
          params->RDoorOpenSum = 0;
        }
        RdoorCloseSts = false;
    }
    
}

/********************************* Number of Defrost Times *********************************/
static void DefrostTimes(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
 //F defrost times
  static bool FDefStat = false;
  
  params->CumulativeFDefost = htons(params->eepr->eep_us_LogSum_FDefCount);
    
  if(consts->rf->Cabin[1].states.Work != _Work_Defrost) //if no defrost
    FDefStat = true;
  
  if((FDefStat == true) && (consts->rf->Cabin[1].states.Work == _Work_Defrost)) //when defrost strat (change Off to On)
  {
      params->eepr->tempFD.F_Defrost_flag |= 1;
      if(params->CumulativeFDefost < 0xFFFF) //Max value
      params->CumulativeFDefost++;
      params->eepr->eep_us_LogSum_FDefCount = htons(params->CumulativeFDefost);
      FDefStat = false; //
  }
  
  if(params->MODEL_SEL == 3)            //BONA
  {
 //R defrost times
  static bool RDefStat = false;
  
  params->CumulativeRDefost = htons(params->eepr->eep_us_LogSum_RDefCount);
  
  if(consts->rf->Cabin[0].states.Work != _Work_Defrost) //if no defrost
  RDefStat = true;
    
  if((RDefStat == true) && (consts->rf->Cabin[0].states.Work == _Work_Defrost)) //when defrost strat (change Off to On)
  {
    params->eepr->tempRD.R_Defrost_flag |= 1;
    if(params->CumulativeRDefost < 0xFFFF) //Max value
    params->CumulativeRDefost++;
    params->eepr->eep_us_LogSum_RDefCount = htons(params->CumulativeRDefost);
    RDefStat = false;
  }
  }
}

/************************************* Working days ***********************************/
static void WorkingDays(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  static bool No24hr = false;
  
  if(params->eepr->flag1.Operation_24h != 1)
     No24hr = true;
  
  params->CumulativeWorkingDay = htons(params->eepr->eep_us_LogSum_PwrOnTm);
  if( No24hr && params->eepr->flag1.Operation_24h == 1)
  {
    if(params->CumulativeWorkingDay < 0xFFFF )
    params->CumulativeWorkingDay++; 
    params->eepr->eep_us_LogSum_PwrOnTm = htons(params->CumulativeWorkingDay);
   // params->eepr->eep_uc_Log_hourcnt = 0;
    No24hr = false;
  }
  
}

/************** Compressor and Valve position time per days **********************/
static void CompVolPerDays(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
    static bool PowerInit = true;

   if((PowerInit) && (params->eepr->flag1.Operation_Reset == 1))  //Convert min to sec for continue counter.
   {  //convert F
      params->accumulated_CompFVal[0] = (params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_1999rpm * 300) - 150;
      params->accumulated_CompFVal[1] = (params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_2999rpm * 300) - 150;
      params->accumulated_CompFVal[2] = (params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_3999rpm * 300) - 150;
      params->accumulated_CompFVal[3] = (params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_4000rpm * 300) - 150;
      //Convert R
      if(params->MODEL_SEL == 3){       //BONA               
      params->accumulated_CompRVal[0] = (params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_1999rpm * 300) - 150;
      params->accumulated_CompRVal[1] = (params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_2999rpm * 300) - 150;
      params->accumulated_CompRVal[2] = (params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_3999rpm * 300) - 150;
      params->accumulated_CompRVal[3] = (params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_4000rpm * 300) - 150;
      }
      for(uint8_t i=0;i<4;i++)
      {
       if(params->accumulated_CompFVal[i]<0) 
          params->accumulated_CompFVal[i] = 0;
       if((params->accumulated_CompRVal[i]<0) && (params->MODEL_SEL == 3))      //BONA
          params->accumulated_CompRVal[i] = 0;
       
      }
      PowerInit = false;
   }
  
  params->CompSpeed = consts->rf->Cooler.components.compressor.Readvalue * 30 ;//consts->rf->Cooler.components.compressor.value * 30 ;

  if(params->CompSpeed > 0) // Compressor on
  {
     if(params->MODEL_SEL == 3){        //BONA
     //Valve is F open or all open--------------------------------------------
     if((consts->rf->Cooler.components.solenoidValve[0].positionIndex == 0x02) || (consts->rf->Cooler.components.solenoidValve[0].positionIndex == 0x03))
     {
       if(params->CompSpeed < 2000)
       {
         if(params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_1999rpm < 0xFF)
         params->accumulated_CompFVal[0]++;
       }else{
           if((params->CompSpeed >= 2000) && (params->CompSpeed < 3000) ) 
            {
              if(params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_2999rpm < 0xFF)
              params->accumulated_CompFVal[1]++;
             }else{
                   if((params->CompSpeed >= 3000) && (params->CompSpeed < 4000) ) 
                   {
                     if(params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_3999rpm < 0xFF)
                     params->accumulated_CompFVal[2]++;
                   }else {
                          if(params->CompSpeed >= 4000)
                          {
                           if(params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_4000rpm < 0xFF)
                           params->accumulated_CompFVal[3]++;
                          }
                   }
              }
             }
      
     }
     //Valve is R open or all open---------------------------------------------
     if((consts->rf->Cooler.components.solenoidValve[0].positionIndex == 0x01) || (consts->rf->Cooler.components.solenoidValve[0].positionIndex == 0x03)) 
     {
        if(params->CompSpeed < 2000)
       {
         if(params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_1999rpm < 0xFF)
         params->accumulated_CompRVal[0]++;
       }else{
           if((params->CompSpeed >= 2000) && (params->CompSpeed < 3000) ) 
            {
              if(params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_2999rpm < 0xFF)
              params->accumulated_CompRVal[1]++;
             }else{
                   if((params->CompSpeed >= 3000) && (params->CompSpeed < 4000) ) 
                   {
                     if(params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_3999rpm < 0xFF)
                     params->accumulated_CompRVal[2]++;
                   }else {
                          if(params->CompSpeed >= 4000)
                          {
                           if(params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_4000rpm < 0xFF)
                           params->accumulated_CompRVal[3]++;
                          }
                   }
              }
             }
     }
     }
     
     if(params->MODEL_SEL == 2 || params->MODEL_SEL == 4)         //HOSHI_H30 OR ALPS
     {
       if(params->CompSpeed < 2000)
       {
         if(params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_1999rpm < 0xFF)
         params->accumulated_CompFVal[0]++;
       }else{
              if((params->CompSpeed >= 2000) && (params->CompSpeed < 3000) ) 
              {
                if(params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_2999rpm < 0xFF)
                params->accumulated_CompFVal[1]++;
              }else{
                    if((params->CompSpeed >= 3000) && (params->CompSpeed < 4000) ) 
                    {
                     if(params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_3999rpm < 0xFF)
                     params->accumulated_CompFVal[2]++;
                    }else{
                           if(params->CompSpeed >= 4000)
                          {
                           if(params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_4000rpm < 0xFF)
                           params->accumulated_CompFVal[3]++;
                          }
                         }
                   }
            }
     }
  }  
   params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_1999rpm = (params->accumulated_CompFVal[0]+150)/300;
   params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_2999rpm = (params->accumulated_CompFVal[1]+150)/300;
   params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_3999rpm = (params->accumulated_CompFVal[2]+150)/300;
   params->eepr->eep_uc_Log_FCompVal_Buf.FCompVal_4000rpm = (params->accumulated_CompFVal[3]+150)/300;
   params->eepr->eep_uc_Log_FCompValT = params->eepr->eep_uc_Log_FCompVal_Buf.byte4; //move buffer to Display data
   if(params->MODEL_SEL == 3){          //BONA
   params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_1999rpm = (params->accumulated_CompRVal[0]+150)/300;
   params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_2999rpm = (params->accumulated_CompRVal[1]+150)/300;
   params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_3999rpm = (params->accumulated_CompRVal[2]+150)/300;
   params->eepr->eep_uc_Log_RCompVal_Buf.RCompVal_4000rpm = (params->accumulated_CompRVal[3]+150)/300;
   params->eepr->eep_uc_Log_RCompValT = params->eepr->eep_uc_Log_RCompVal_Buf.byte4; //move buffer to Display data  
   }
}

/********************************* Valve positon time per 2hr. *********************************/
static void ValvePosition2hr(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  if(params->MODEL_SEL == 3){           //BONA
  //F or entire is open.
  if((consts->rf->Cooler.components.solenoidValve[0].positionIndex == 0x02) || (consts->rf->Cooler.components.solenoidValve[0].positionIndex == 0x03))
  {
    if(params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_FValve_time < 0x0C) //Maximum 120 min
    {
       params->FValveCont1s++;
       if(consts->rf->ui.states.commercial.status == _UI_Commercial_None)       //APW : Update I2C driver CT code to global
        params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_FValve_time = (params->FValveCont1s + 300)/600; //1 hex ~ 10 min and 5 min rounded off 
       if(consts->rf->ui.states.commercial.status == _UI_Commercial_Test)       //APW : Update I2C driver CT code to global
        params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_FValve_time = (params->FValveCont1s*60 + 300)/600; //1 hex ~ 10 sec and 5 sec rounded off   
    }
  }
   //R or entire is open.
  if((consts->rf->Cooler.components.solenoidValve[0].positionIndex == 0x01) || (consts->rf->Cooler.components.solenoidValve[0].positionIndex == 0x03))
  {
    if(params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_RValve_time < 0x0C) // 1 hex ~ 10 min and Maximum 120 min
    {
       params->RValveCont1s++;
       if(consts->rf->ui.states.commercial.status == _UI_Commercial_None)       //APW : Update I2C driver CT code to global
        params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_RValve_time = (params->RValveCont1s + 300)/600; //1 hex ~ 10 min and 5 min rounded off 
       if(consts->rf->ui.states.commercial.status == _UI_Commercial_Test)       //APW : Update I2C driver CT code to global
        params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_RValve_time = (params->RValveCont1s*60 + 300)/600; //1 hex ~ 10 min and 5 min rounded off 
    }
  }
  }
}

/*********************************************************************
 function  = F door and R door open count(maximum 30 time = F hex).
 author    = D.Sutiwas					
 history   = 2025.04.24/D.Sutiwas  :New function release  -> 0000000000_V00R00_V00R12			
*********************************************************************/
static void FRDoorCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  if(params->MODEL_SEL == 2 || params->MODEL_SEL == 4) { //HOSHI H30 OR ALPS
  static bool FdoorCloseSts = false;
  static bool RdoorCloseSts = false;
  static uint8_t F_Door_Cnt = 0;
  static uint8_t R_Door_Cnt = 0;
  //F door
   if(consts->rf->Cabin[1].components.doorSwitch[0].value == _SwitchValue_CLOSE )
     FdoorCloseSts = true;
    
    if((FdoorCloseSts) && (consts->rf->Cabin[1].components.doorSwitch[0].value == _SwitchValue_OPEN))
    {
       if(++F_Door_Cnt >= 2 ) 
        { //F door open 2 times = 1 hex in EEPROM
          if(params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_FValve_time < 0xF) 
          {
            params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_FValve_time ++;//Used eeprom address of valve record every 2 hr that why I used this variable(see in EEPROM MAP).
          }
          F_Door_Cnt = 0;
        }
       FdoorCloseSts = false;
    }
    
  //R door
     if(consts->rf->Cabin[0].components.doorSwitch[0].value == _SwitchValue_CLOSE )
     RdoorCloseSts = true;
    
    if((RdoorCloseSts) && (consts->rf->Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN))
    {
       if(++R_Door_Cnt >= 2 ) 
        { //F door open 2 times = 1 hex in EEPROM
          if(params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_RValve_time < 0xF)
          {
            params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_RValve_time ++;//Used eeprom address of valve record every 2 hr that why I used this variable(see in EEPROM MAP).
          }
          R_Door_Cnt = 0;
        }
       RdoorCloseSts = false;
    }
  }
}

/********************************* Valve operation counter 20times = 1 hex *********************************/
static void ValveOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  if(params->MODEL_SEL == 3){           //BONA
    static bool ValvePosiCheck1times = true; 
    if(ValvePosiCheck1times) //set valve position after power on.
    {
      params->PreviousValvePosition = consts->rf->Cooler.components.solenoidValve[0].positionIndex;
      ValvePosiCheck1times = false;
    }
    
    params->CumulativeValveOper = htons(params->eepr->eep_us_LogSum_Valve);
    
    if(consts->rf->Cooler.components.solenoidValve[0].positionIndex != params->PreviousValvePosition)
    {
      if(++params->ValeOperSum >= 20)
      {//20 times = 1 hex
         if(params->CumulativeValveOper < 0xFFFF) //Max value
          params->CumulativeValveOper++;
         
         params->eepr->eep_us_LogSum_Valve = htons(params->CumulativeValveOper);
         params->ValeOperSum = 0;
      }
     params->PreviousValvePosition = consts->rf->Cooler.components.solenoidValve[0].positionIndex;
    }
  }
  
}

/********************************* Gear Motor operation counter 15s = 1 hex *********************************/
static void GearMotorOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  if(params->MODEL_SEL == 3){           //BONA
 
    params->CumulativeGearMoOper = htons(params->eepr->eep_us_LogSum_GireMOnTm);       //htons is meean htons(1000Hex) = 0010Hex

    if(consts->rf->IceSource.components.pusher.value == _PusherValue_PUSH)
    {
      if(++params->GearMoOperSum >= 15)
      {//15s = 1 hex (This loop stay in 1s counting)          
         if(params->CumulativeGearMoOper < 0xFFFF) //Max value
          params->CumulativeGearMoOper++;
         
         params->eepr->eep_us_LogSum_GireMOnTm = htons(params->CumulativeGearMoOper);  
         params->GearMoOperSum = 0;
      }
    }
  }
}
/****************************** Water Dispenser operation counter 15s = 1 hex *******************************/
static void WaterDispenserOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  if(params->MODEL_SEL == 3){           //BONA
 
    params->CumulativeWaterDisOper = htons(params->eepr->eep_us_LogSum_WDPOnTm);       //htons is meean htons(1000Hex) = 0010Hex

    if(consts->rf->Dispenser.sets.state == _DispenserState_WaterPump &&
        consts->rf->WaterSource.components.dispenserPump.value != _PumpValue_OFF)
    {
      if(++params->WaterDisOperSum >= 15)
      {//15s = 1 hex (This loop stay in 1s counting)          
         if(params->CumulativeWaterDisOper < 0xFFFF) //Max value
          params->CumulativeWaterDisOper++;
         
         params->eepr->eep_us_LogSum_WDPOnTm = htons(params->CumulativeWaterDisOper);  
         params->WaterDisOperSum = 0;
      }
    }
  }
}
/******************************* Ice Crushe operation counter 1 times = 1 hex *******************************/
static void IceCrusheOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  if(params->MODEL_SEL == 3){           //BONA
    static bool IceCrusheCloseSts = false;
    params->CumulativeIceCrusheOper = htons(params->eepr->eep_us_LogSum_IDSolOnTm);       //htons is meean htons(1000Hex) = 0010Hex

    if(consts->rf->Dispenser.sets.state == _DispenserState_IceCrash &&
         consts->rf->IceSource.components.shutter.value != _ShutterValue_OPENING && 
          consts->rf->IceSource.components.selector.value == _SelectorValue_OFF)
      IceCrusheCloseSts = true;
    else if(consts->rf->Dispenser.sets.state == _DispenserState_IceCrash &&
              consts->rf->IceSource.components.shutter.value == _ShutterValue_OPENING && 
                consts->rf->IceSource.components.selector.value == _SelectorValue_OFF)
    {
      if((IceCrusheCloseSts) && (params->CumulativeIceCrusheOper < 0xFFFF))
      {
        params->CumulativeIceCrusheOper++;
        params->eepr->eep_us_LogSum_IDSolOnTm = htons(params->CumulativeIceCrusheOper);
      }
      IceCrusheCloseSts = false;
    }
  }
}
/******************************** Ice Cube operation counter 1 times = 1 hex ********************************/
static void IceCubeOperatCnt(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  if(params->MODEL_SEL == 3){           //BONA
    static bool IceCubeCloseSts = false;
    params->CumulativeIceCubeOper = htons(params->eepr->eep_us_LogSum_DDSolOnTm);       //htons is meean htons(1000Hex) = 0010Hex

    if(consts->rf->Dispenser.sets.state == _DispenserState_IceCube &&
         consts->rf->IceSource.components.shutter.value != _ShutterValue_OPENING && 
          consts->rf->IceSource.components.selector.value == _SelectorValue_OFF)
      IceCubeCloseSts = true;
    else if(consts->rf->Dispenser.sets.state == _DispenserState_IceCube &&
              consts->rf->IceSource.components.shutter.value == _ShutterValue_OPENING && 
                consts->rf->IceSource.components.selector.value == _SelectorValue_SELECTING_A)
    {
      if((IceCubeCloseSts) && (params->CumulativeIceCubeOper < 0xFFFF))
      {
        params->CumulativeIceCubeOper++;
        params->eepr->eep_us_LogSum_DDSolOnTm = htons(params->CumulativeIceCubeOper);
      }
      IceCubeCloseSts = false;
    }
  }
}

/********************************* Count every 2 hr *********************************/
static void CountEvery2hr(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)//Backup Count every 2 hr
{
   // Count every 2 hr------------------------------------
  if(params->eepr->eep_uc_Log_DataCnt < 0xFC) //Max 252 value
  params->eepr->eep_uc_Log_DataCnt++;
  
  if (params->eepr->eep_uc_Log_hourcnt >= 11)
    params->eepr->eep_uc_Log_hourcnt = 0;
  else
    params->eepr->eep_uc_Log_hourcnt++;
  
   // for FailureFrequencyUpdate every days---------------
   uint16_t ConvDatHis_Old;//Convert data His_Old before record.
   uint16_t ConvDatHis_New;//Convert data His_New before record.
   for (int i = 0; i < TOTAL_ERRORS; i++)
    { 
      if(params->eepr->eep_us_ErrHis_Cnt[i])
      {//ErrHis_Old
        if(params->TimerErrHis_Old[i]++ >= 11)
        {
          ConvDatHis_Old = htons(params->eepr->eep_us_ErrHis_Old[i]);
           if(params->eepr->eep_us_ErrHis_Old[i] < 0xFFFF)
           { 
             ConvDatHis_Old++;
             params->eepr->eep_us_ErrHis_Old[i] = htons(ConvDatHis_Old);
           }
        params->TimerErrHis_Old[i] = 0;
        }

        //ErrHis_New
        if(params->TimerErrHis_New[i]++ >= 11)
        {
          ConvDatHis_New = htons(params->eepr->eep_us_ErrHis_New[i]);
           if(params->eepr->eep_us_ErrHis_New[i] < 0xFFFF)
           {
             ConvDatHis_New++;
             params->eepr->eep_us_ErrHis_New[i] = htons(ConvDatHis_New);
           }
        params->TimerErrHis_New[i] = 0;
        }
        
      }
    }
}

static void FailureFrequencyUpdate(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  
   static bool AfterPowerOn = true;
  if(AfterPowerOn)
  {//After power on of failure history update.
     for (uint8_t k=0; k<4; k++)
     {
      params->EEpromErrorImage[k].errorNumber = params->eepr->eep_uc_ErrLog[k];
      params->EEpromErrorImage[k].daysPassed = params->eepr->eep_uc_ErrOfDay[k];
     }
    AfterPowerOn = false;
  }
  
  // update these three fields at startup of MCU eep_us_ErrHis_Old[29],  eep_us_ErrHis_New[29], eep_us_ErrHis_Cnt[29]
  uint16_t ConvDatHis_Cnt;//Convert data His_New before record.
  uint8_t err_skip_address;
  for (uint8_t i = 0; i < sizeof(consts->rf->Values.errors.errorsCheckTable) ; i++)       //size table more than error 1 point
  {
      if(i == 6 || i == 7 || i == 20 || i == 21)       //error skip
        err_skip_address = 0xFF;
      else if(i <= 5)                           //error 0 - 5 not skip
        err_skip_address = 0;
      else if(i > 7 && i <= 19)                 //error 8 - 19 skip 2 point
        err_skip_address = 2;
      else                                      //error 22 - end 2 points but we must set 4 because we have skip 2 point before
        err_skip_address = 4;
        
      if (consts->rf->Values.errors.errorsCheckTable[i])//There is error
      {
        if (params->errorCheckPrev[i - err_skip_address] == 0)
        {//When there is an error but the previous error value is 0.It means that a new error has occurred.
          params->eepr->flag1.Operation_Failure = 1;
          tsError newError = {i + 1, 0, 0};
          push_new_error(params->errorQueue,TOTAL_ERRORS, newError);
          ConvDatHis_Cnt = htons(params->eepr->eep_us_ErrHis_Cnt[i - err_skip_address]);
          ConvDatHis_Cnt++;
          params->eepr->eep_us_ErrHis_Cnt[i - err_skip_address] = htons(ConvDatHis_Cnt);
          params->eepr->eep_us_ErrHis_New[i - err_skip_address] = 0;
          params->TimerErrHis_New[i - err_skip_address] = 0;
        }
      }
      if(err_skip_address != 0xFF)
        params->errorCheckPrev[i - err_skip_address] = consts->rf->Values.errors.errorsCheckTable[i];
  }
}

/******************* Software version recorded ********************/
static void SoftVersion(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  params->eepr->eep_ui_Log_MainStockNo[0] = htons(consts->rf->Stock.No[0]);
  params->eepr->eep_ui_Log_MainStockNo[1] = htons(consts->rf->Stock.No[1]);
  params->eepr->eep_ui_Log_MainStockNo[2] = htons(consts->rf->Stock.No[2]);
  params->eepr->eep_uc_Log_MainParaVer = consts->rf->Stock.Version;
  params->eepr->eep_uc_Log_MainParaRev = consts->rf->Stock.Revision;
  params->eepr->eep_uc_Log_MainCoreVer = consts->rf->Stock.CoreVersion;
  params->eepr->eep_uc_Log_MainCoreRev = consts->rf->Stock.CoreRevision; 
  
  params->eepr->eep_ui_Log_CompStockNo[0] = htons(refrigerator_R.Cooler.components.compressor.Stock.No[0]);//htons(params->comp->compressorComm.EKIPackets.BoardSkuID3); 
  params->eepr->eep_ui_Log_CompStockNo[1] = htons(refrigerator_R.Cooler.components.compressor.Stock.No[1]);//htons(params->comp->compressorComm.EKIPackets.BoardSkuID2);
  params->eepr->eep_ui_Log_CompStockNo[2] = htons(refrigerator_R.Cooler.components.compressor.Stock.No[2]);//htons(params->comp->compressorComm.EKIPackets.BoardSkuID1);
  params->eepr->eep_uc_Log_CompParaVer = refrigerator_R.Cooler.components.compressor.Stock.Version;//params->comp->compressorComm.EKIPackets.BoardVerRev0;
  params->eepr->eep_uc_Log_CompParaRev = refrigerator_R.Cooler.components.compressor.Stock.Revision;//params->comp->compressorComm.EKIPackets.BoardVerRev1;
  params->eepr->eep_uc_Log_CompCoreVer = refrigerator_R.Cooler.components.compressor.Stock.CoreVersion;//params->comp->compressorComm.EKIPackets.BoardVerRev2;
  params->eepr->eep_uc_Log_CompCoreRev = refrigerator_R.Cooler.components.compressor.Stock.CoreRevision;//params->comp->compressorComm.EKIPackets.BoardVerRev3;

}

/******************* RealTimer backup recorded ********************/
static void RealTimerBackup(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
  params->eepr->eep_us_Backup_RRealTimer = refrigerator_R.Cabin[0].values.realTimer;  
  params->eepr->eep_us_Backup_FRealTimer = refrigerator_R.Cabin[1].values.realTimer;
  
  params->eepr->eep_us_Backup_RDoorOpenTimer = refrigerator_R.Cabin[0].values.defrostDoorOpenedTimer;  
  params->eepr->eep_us_Backup_FDoorOpenTimer = refrigerator_R.Cabin[1].values.defrostDoorOpenedTimer;
}

/******************* Clear status or any flag after 2 hr passed ********************/
static void ClearFlags(const tsEEPROMFiledsConsts *consts, tsEEPROMFiledsParams *params)
{
   
  params->eepr->tempF.FDoor_open = 0; 
  params->eepr->tempR.RDoor_open = 0; 
  params->eepr->tempFD.F_Defrost_flag = 0;
  params->eepr->tempRD.R_Defrost_flag = 0;
  params->eepr->tempIM.Human_Detect = 0;
  params->eepr->tempTc.Eco_flag = 0;
  params->eepr->humidity.Shabbat_flag = 0;
    
   params->FDoorOpenCntTimer1s = 0;
   params->RDoorOpenCntTimer1s = 0;
   params->eepr->DOpenTime.byte = 0x00;
    
    if(params->eepr->flag1.Operation_24h == 1)
   {
      params->accumulated_CompFVal[0] = 0;
      params->accumulated_CompFVal[1] = 0;
      params->accumulated_CompFVal[2] = 0;
      params->accumulated_CompFVal[3] = 0;
      params->accumulated_CompRVal[0] = 0;
      params->accumulated_CompRVal[1] = 0;
      params->accumulated_CompRVal[2] = 0;
      params->accumulated_CompRVal[3] = 0;
      params->eepr->flag1.Operation_24h    = 0;
  } 
  //flag1
  params->eepr->flag1.Operation_IM_Mode = 0;
  params->eepr->flag1.Operation_IM_Operation = 0;
  params->eepr->flag1.Operation_IM_Water  = 0;
  params->eepr->flag1.Operation_IM_Fullice  = 0;
  params->eepr->flag1.Operation_Reset   = 0;
  params->eepr->flag1.Operation_Wifimode = 0;   // need to confirm this
  params->eepr->flag1.Operation_Failure  = 0;
  //params->eepr->flag1.Operation_24h    = 0;
  
  //flag 2
  //params->eepr->flag2.Operation_Mute = 0;   // Currently, There is no Mute function
  params->eepr->flag2.Operation_QuickCool = 0;
  params->eepr->flag2.Operation_QuickFreeze = 0;
  params->eepr->flag2.Operation_WifiRoutno = 0;  // need to confirm this
  
  // flag 3 
  params->eepr->flag3.Operation_VacuumMode = 0;
  params->eepr->flag3.Operation_VacuumMotor = 0;
  params->eepr->flag3.Operation_WifiCardNoConnect = 0;
   //params->eepr->flag3.byte = 0; // All flags are reset at two hours interval
   params->eepr->eep_uc_Log_VDMax = 0x00;
   params->eepr->eep_uc_Log_VDMin = 0xFF;
  params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_FValve_time = 0;
  params->eepr->eep_uc_Log_ValveFlag.eep_uc_Log_RValve_time = 0;
  params->FValveCont1s = 0;
  params->RValveCont1s = 0; 
}
//------------------------------------------------------------------------------------------------------------------//
//---------------------------------Helper Functions ----------------------------------------------------------------//
// Function to add a new error
void push_new_error(tsError *errorList, uint8_t size, tsError newError)
{
  if(!newError.errorNumber) // check if correct error data is pushed
    return;
  // Shift elements down, discarding the last if the array is full
  for (uint8_t i = size - 1; i > 0; i--)
  {
    errorList[i] = errorList[i - 1];
  }
  // Add new error at the top
  *errorList = newError;
}

// Function to remove the top error
tsError pop_error(tsError *errorList, uint8_t size)
{
  if (errorList[0].errorNumber)
  { // Check if there's an error at the top
    tsError topError = errorList[0];
    // Shift elements up
    for (uint8_t i = 0; i < size - 1; i++)
    {
     errorList[i] = errorList[i + 1];
    }
    // Clear the last element after shifting
   errorList[size - 1] = (tsError){0}; // Optional: Clear the last element
    return topError;
  }
  else
    return (tsError){0}; // Return a default error
}

// Function to remove the below error
tsError below_error(tsError *errorList, uint8_t size)
{
  // Start from the last index and search for the last non-zero error
  for (int8_t i = size - 1; i >= 0; i--)
  {
      if (errorList[i].errorNumber != 0)
      {
          tsError lastError = errorList[i];
          errorList[i] = (tsError){0}; // Clear this slot
          return lastError;
      }
  }
  return (tsError){0}; // No error found
}
    
// Function to update hours timer
void update_time(tsError *errorList,  uint8_t size)
{
  for (uint8_t i = 0; i < size; i++)
  {
    if (errorList[i].errorNumber)
    { // Only update non-zero error elements
      errorList[i].hoursPassed ++;
      if (errorList[i].hoursPassed >= 12)
      {
        errorList[i].hoursPassed = 0;
        if(errorList[i].daysPassed < 0xFF)
        errorList[i].daysPassed++;
      }
    }
  }
}

/** @} */

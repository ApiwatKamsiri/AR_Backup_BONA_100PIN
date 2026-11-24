/*
 * rcos_connectivity.h
 *
 *  Created on: 28 Ağu 2024
 *      Author: 26104480 26037659
 */

#ifndef SOURCES_APP_MW_RCOS_CONNECTIVITY_H_
#define SOURCES_APP_MW_RCOS_CONNECTIVITY_H_


#include "rcos.h"
#include "system.h"
#include "..\..\rcos\mw\Connectivity\wfa.h"
#include "..\..\rcos\mw\Connectivity\gdm.h"
#include "..\..\rcos\mw\Connectivity\ccm.h"
#include "..\..\rcos\mw\Connectivity\pcm.h"
#include "..\..\rcos\mw\Connectivity\ccm_safir.h"
#include "..\..\rcos\mw\Connectivity\consumption.h"
#include "connectivity\remote_control.h"
#include "connectivity\remote_control_driver.h"
#include "..\..\rcos\mw\Connectivity\feature.h"
#include "..\..\board\board.h"
#include "..\..\dev\otaprocess.h"
#include "..\..\dev\energymonitoring.h"

extern tsEnergyMonitoringParams energyMonitoringProcParams;

#define MERCEK_VERSION_MAJOR 3
#define MERCEK_VERSION_MINOR 2 

#define FEATURE_COUNT (1)
#define MERCEK_SIZE (187)  // V03R22: COMP_STOCK_NO 6 bytes

#if (CONNECTIVITY != _NON_CONNECTED)
const tsGdmMapItem gdmMap[] = {
    // Pack Type                Command                             Process                Event
    {PACK_TYPE_WFA, WFA_READ_BYTE, eProcessWifiArray, eWfaReadByte},
    {PACK_TYPE_WFA, WFA_WRITE_BYTE, eProcessWifiArray, eWfaWriteByte},
    {PACK_TYPE_WFA, WFA_READ_DYNAMIC_BYTES, eProcessWifiArray, eWfaReadDynamicBytes},
    {PACK_TYPE_WFA, WFA_READ_ALL_BYTES, eProcessWifiArray, eWfaReadAllBytes},
    {PACK_TYPE_WFA, WFA_READ_MERCEK_ARRAY, eProcessWifiArray, eWfaReadMercek},
    {PACK_TYPE_WFA, WFA_BYTE_READ, eProcessWifiArray, eWfaByteRead},
    {PACK_TYPE_WFA, WFA_BYTE_WRITTEN, eProcessWifiArray, eWfaByteWritten},
    {PACK_TYPE_WFA, WFA_DYNAMIC_BYTES_READ, eProcessWifiArray, eWfaDynamicBytesRead},
    {PACK_TYPE_WFA, WFA_ALL_BYTES_READ, eProcessWifiArray, eWfaAllBytesRead},
    {PACK_TYPE_WFA, WFA_MERCEK_ARRAY_READ, eProcessWifiArray, eWfaMercekRead},
    {PACK_TYPE_WFA, WFA_READ_MERCEK_VERSION, eProcessWifiArray, eWfaReadMercekVersion},
    {PACK_TYPE_WFA, WFA_MERCEK_VERSION_READ, eProcessWifiArray, eWfaMercekVersionRead},
    {PACK_TYPE_WFA, WFA_READ_SECURE_MERCEK, eProcessWifiArray, eWfaReadSecureMercek},
    {PACK_TYPE_WFA, WFA_SECURE_MERCEK_READ, eProcessWifiArray, eWfaSecureMercekRead},
    {PACK_TYPE_CONSUMPTION, CONSUMPTION_READ_DATA, eProcessConsumption, eConsumptionEventsReadConsumptionData},
    {PACK_TYPE_CONSUMPTION, CONSUMPTION_EMIT_DATA, eProcessConsumption, eConsumptionEventsEmitConsumptionData},
    {PACK_TYPE_CONSUMPTION, CONSUMPTION_DATA_PUBLISHED, eProcessConsumption, eConsumptionEventsConsumptionDataPublished},
    {PACK_TYPE_CONFIG, CCM_CMD_READ_ATTRIBUTE, eProcessCcm, eCcmRecievedReadAttribute},
    {PACK_TYPE_CONFIG, CCM_CMD_WRITE_ATTRIBUTE, eProcessCcm, eCcmRecievedWriteAttribute},
    {PACK_TYPE_CONFIG, CCM_CMD_ATTRIBUTE_READ, eProcessCcm, eCcmRecievedAttributeRead},
    {PACK_TYPE_CONFIG, CCM_CMD_ATTRIBUTE_WRITTEN, eProcessCcm, eCcmRecievedAttributeWritten},
    {PACK_TYPE_CONFIG, CCM_CMD_ATTRIBUTE_EMITTED, eProcessCcm, eCcmRecievedAttributeEmitted},
    {PACK_TYPE_OTA, OTA_CMD_LIST_BOARDS, eProcessOta, eListOfBoardsRecv},
    {PACK_TYPE_OTA, OTA_CMD_BOARD_LIST, eProcessOta, eListOfBoardsSend},
    {PACK_TYPE_OTA, OTA_CMD_OTA_ENTER_BOOTLOADER, eProcessOta, eEnterBootloader},
//    {PACK_TYPE_SMART, SMART_COM_UPDATE, eProcessSmartCom, eSmartComUpdate},
    {PACK_TYPE_FEATURE,         FEATURE_LIST,                       eProcessFeature,       eFeatureEventsListFeatures},
    {PACK_TYPE_FEATURE,         FEATURE_LISTED,                     eProcessFeature,       eFeatureEventsFeatureListed},
//#if (CONNECTIVITY == _BLE_CONNECTED) // bluetooth'lu dualcore islemcilerde IPC uzerinden veri transferi icin acilan kanal
//    {PACK_TYPE_IPC_LOCAL, IPC_LOCAL_M4_IS_ALIVE, eProcessRemoteControlDriver, eRCDKeepAlive},
//#endif
};

const tsAttrbuteRooter attrbuteMap[] = {
  //     Attr Id                         r   w   e   Event Handler                             Length                            Attr
  /*0*/ {MAX_FRAME_SIZE, FALSE, TRUE, FALSE, AttrHandlerMaxFrameSize, sizeof(maxFrameSizeApplience), &maxFrameSizeApplience},
  /*1*/ {DEVICE_DESCRIPTION, TRUE, TRUE, FALSE, AttrHandlerDeviceDescription, sizeof(dummyAttribute), &dummyAttribute},
  /*2*/ {SUPPORTED_CONNECTION_TYPES, TRUE, FALSE, FALSE, AttrHandlerSupportedConnTypes, sizeof(suppConnTypes), &suppConnTypes},
  /*3*/ {COMMUNICATION_MODE, TRUE, TRUE, FALSE, AttrHandlerCommMode, sizeof(dummyAttribute), &dummyAttribute},
  /*4*/ {MAC_ADDRESS, TRUE, FALSE, FALSE, AttrHandlerMacAddress, sizeof(dummyAttribute), &dummyAttribute},
  /*5*/ {COMM_PROTOCOL_VERSION, TRUE, FALSE, FALSE, AttrHandlerCommProtocolVersion, sizeof(dummyAttribute), &dummyAttribute},
  /*6*/ {CONNECTION_STATE_WIFI, TRUE, TRUE, TRUE, AttrHandlerConnStateWifi, sizeof(connStateWifi), &connStateWifi},
  /*7*/ {STORED_SSID, TRUE, FALSE, TRUE, AttrHandlerStoredSSID, sizeof(storedSSID), &storedSSID},
  /*8*/ {CONNECTION_SUBSTATE, TRUE, FALSE, TRUE, AttrHandlerConnSubStateWifi, sizeof(connSubStateWifi), &connSubStateWifi},
  /*9*/ {ONBOARDING_TIMEOUT_DUR_IN_SEC, TRUE, TRUE, FALSE, AttrHandlerOnboardingTimeOutDurationSec, sizeof(dummyAttribute), &dummyAttribute},
  /*A*/ {CUSTOMIZED_NAME, FALSE, TRUE, FALSE, AttrHandlerCustomizedName, sizeof(dummyAttribute), &dummyAttribute},
  /*B*/ {CONNECTIVITY_BOARD_ID, TRUE, TRUE, FALSE, AttrHandlerConnBoardId, sizeof(dummyAttribute), &dummyAttribute},
  /*C*/ {OTA_STATE, TRUE, FALSE, TRUE, AttrHandlerOtaState, sizeof(dummyAttribute), &dummyAttribute},
  /*D*/ {NETWORK_CARD_SW_VERSION, TRUE, FALSE, FALSE, AttrHandlerNetworkCardSwVersion, sizeof(nbSwVersion), &nbSwVersion},

};

const tsWfaMap wifiArrayMap[] = {
	/*0*/    {&WiFiVariables.ServiceButton, sizeof(WiFiVariables.ServiceButton), eProcessRemoteControlDriver, eRCDEventServiceButton},
    /*1-5*/  {WiFiVariables.StockCode, sizeof(WiFiVariables.StockCode), 0, 0},
	/*6*/    {&WiFiVariables.MachineID, sizeof(WiFiVariables.MachineID), 0, 0},
//	/*7*/    {&WiFiVariables.ServiceButtonShortTimeOut, sizeof(WiFiVariables.ServiceButtonShortTimeOut), eProcessRemoteControlDriver, eRCDEventServiceButtonShortTimeOut},
//	/*8*/    {&WiFiVariables.ServiceButtonLongTimeOut, sizeof(WiFiVariables.ServiceButtonLongTimeOut), eProcessRemoteControlDriver, eRCDEventServiceButtonLongTimeOut},
	/*9*/    {&WiFiVariables.MeasurementInformation, sizeof(WiFiVariables.MeasurementInformation), 0, 0},
	/*10-14*/{WiFiVariables.Password, sizeof(WiFiVariables.Password), 0, 0},
//	/*15*/   {&WiFiVariables.NBVersion, sizeof(WiFiVariables.NBVersion), 0, 0},
//	/*16*/   {&WiFiVariables.NBRevision, sizeof(WiFiVariables.NBRevision), 0, 0},
	/*17*/   {&WiFiVariables.WGVersion, sizeof(WiFiVariables.WGVersion), 0, 0},
	/*18*/   {&WiFiVariables.DateMinute, sizeof(WiFiVariables.DateMinute), 0, eRCDEventDateMinute},
	/*19*/   {&WiFiVariables.DateHour, sizeof(WiFiVariables.DateHour), 0, eRCDEventDateHour},
	/*20*/   {&WiFiVariables.DateDay, sizeof(WiFiVariables.DateDay), 0, eRCDEventDateDay},
	/*21*/   {&WiFiVariables.DateMonth, sizeof(WiFiVariables.DateMonth), 0, eRCDEventDateMonth},
//	/*22*/   {&WiFiVariables.DateYear, sizeof(WiFiVariables.DateYear), 0, eRCDEventDateYear},
	/*23*/   {&MercekData.SendData, sizeof(MercekData.SendData), eProcessRemoteControlDriver, eRCDEventMercekReadDone},
	/*24*/   {&WiFiVariables.MercekSize, sizeof(WiFiVariables.MercekSize), 0, 0},
	/*25*/   {&WiFiVariables.ArraySize, sizeof(WiFiVariables.ArraySize), 0, 0},
	//// dynamic region starts /////
	/*26*/   {&WiFiVariables.Situation, sizeof(WiFiVariables.Situation), eProcessRemoteControlDriver, eRCDEventSituation},
	/*27*/   {&WiFiVariables.NBErrors0, sizeof(WiFiVariables.NBErrors0), 0, 0},
	/*28*/   {&WiFiVariables.MercekErrorLenght, sizeof(WiFiVariables.MercekErrorLenght), 0, 0},
	/*29*/   {&WiFiVariables.TestMode, sizeof(WiFiVariables.TestMode), 0, 0},
	/*30*/   {&WiFiVariables.EndMesg, sizeof(WiFiVariables.EndMesg), 0, eRCDEventEndMesg},
	/*31*/   {&WiFiVariables.ResetControl, sizeof(WiFiVariables.ResetControl), eProcessRemoteControlDriver, eRCDEventResetControl},
	/*32*/   {&WiFiVariables.Aux2, sizeof(WiFiVariables.Aux2), eProcessRemoteControlDriver, eRCDEventAux2},
	/*33*/   {&WiFiVariables.OTA, sizeof(WiFiVariables.OTA), eProcessRemoteControlDriver, eRCDEventOTA},
	/*34*/   {&WiFiVariables.CabinTempType, sizeof(WiFiVariables.CabinTempType), eProcessRemoteControlDriver, eRCDEventCabinTempType},
//	/*35-39*/{WiFiVariables.CabinTempSet, sizeof(WiFiVariables.CabinTempSet), eProcessRemoteControlDriver, eRCDEventCabinTempSet},
	/*35-39*/{&WiFiVariables.CabinTempSet[0], sizeof(WiFiVariables.CabinTempSet[0]), eProcessRemoteControlDriver, eRCDEventCabinTempSet0},
	/*35-39*/{&WiFiVariables.CabinTempSet[1], sizeof(WiFiVariables.CabinTempSet[1]), eProcessRemoteControlDriver, eRCDEventCabinTempSet1},
	/*35-39*/{&WiFiVariables.CabinTempSet[2], sizeof(WiFiVariables.CabinTempSet[2]), eProcessRemoteControlDriver, eRCDEventCabinTempSet2},
	/*35-39*/{&WiFiVariables.CabinTempSet[3], sizeof(WiFiVariables.CabinTempSet[3]), eProcessRemoteControlDriver, eRCDEventCabinTempSet3},
	/*35-39*/{&WiFiVariables.CabinTempSet[4], sizeof(WiFiVariables.CabinTempSet[4]), eProcessRemoteControlDriver, eRCDEventCabinTempSet4},
	
//        /*40-44*/{WiFiVariables.CabinFunc, sizeof(WiFiVariables.CabinFunc), eProcessRemoteControlDriver, eRCDEventCabinFunc},
	/*40-44*/{&WiFiVariables.CabinFunc[0], sizeof(WiFiVariables.CabinFunc[0]), eProcessRemoteControlDriver, eRCDEventCabinFunc0},
	/*40-44*/{&WiFiVariables.CabinFunc[1], sizeof(WiFiVariables.CabinFunc[1]), eProcessRemoteControlDriver, eRCDEventCabinFunc1},
	/*40-44*/{&WiFiVariables.CabinFunc[2], sizeof(WiFiVariables.CabinFunc[2]), eProcessRemoteControlDriver, eRCDEventCabinFunc2},
	/*40-44*/{&WiFiVariables.CabinFunc[3], sizeof(WiFiVariables.CabinFunc[3]), eProcessRemoteControlDriver, eRCDEventCabinFunc3},
	/*40-44*/{&WiFiVariables.CabinFunc[4], sizeof(WiFiVariables.CabinFunc[4]), eProcessRemoteControlDriver, eRCDEventCabinFunc4},
	/*45*/   {&WiFiVariables.IceMachineSet, sizeof(WiFiVariables.IceMachineSet), eProcessRemoteControlDriver, eRCDEventIceMachineSet},
	/*46*/   {&WiFiVariables.Dispenser, sizeof(WiFiVariables.Dispenser), eProcessRemoteControlDriver, eRCDEventDispenser},
	/*47*/   {&WiFiVariables.IceSource, sizeof(WiFiVariables.IceSource), eProcessRemoteControlDriver, eRCDEventIceSource},
	/*48*/   {&WiFiVariables.ServiceButtonCopy, sizeof(WiFiVariables.ServiceButtonCopy), eProcessRemoteControlDriver, eRCDEventServiceButtonCopy},
	/*49*/   {&WiFiVariables.IceMachine, sizeof(WiFiVariables.IceMachine), eProcessRemoteControlDriver, eRCDEventIceMachine},
	/*50*/   {&WiFiVariables.DisplaySet, sizeof(WiFiVariables.DisplaySet), eProcessRemoteControlDriver, eRCDEventDisplaySet},
	/*51*/   {&WiFiVariables.Vacation, sizeof(WiFiVariables.Vacation), eProcessRemoteControlDriver, eRCDEventVacation},
	/*52*/   {&WiFiVariables.EcoFuzzy, sizeof(WiFiVariables.EcoFuzzy), eProcessRemoteControlDriver, eRCDEventEcoFuzzy},
	/*53*/   {&WiFiVariables.Sabbath, sizeof(WiFiVariables.Sabbath), eProcessRemoteControlDriver, eRCDEventSabbath},
	/*54*/   {&WiFiVariables.ConnectedFeatures, sizeof(WiFiVariables.ConnectedFeatures), 0, 0},
	/*55*/   {&WiFiVariables.Violed, sizeof(WiFiVariables.Violed), eProcessRemoteControlDriver, eRCDEventVioled},
	/*56*/   {&WiFiVariables.Warnings, sizeof(WiFiVariables.Warnings), eProcessRemoteControlDriver, eRCDEventWarnings},
	/*57*/   {&WiFiVariables.Error, sizeof(WiFiVariables.Error), eProcessRemoteControlDriver, eRCDEventError},
	/*58*/   {&WiFiVariables.Filter, sizeof(WiFiVariables.Filter), eProcessRemoteControlDriver, eRCDEventFilter},
	/*59*/   {&WiFiVariables.AlarmLevel, sizeof(WiFiVariables.AlarmLevel), eProcessRemoteControlDriver, eRCDEventAlarmLevel},
        
        /*60*/   {&WiFiVariables.RefrigeratorAux3, sizeof(WiFiVariables.RefrigeratorAux3), 0, 0},
	/*61*/   {&WiFiVariables.RefrigeratorAux4, sizeof(WiFiVariables.RefrigeratorAux4), 0, 0},
	/*62*/   {&WiFiVariables.Components1, sizeof(WiFiVariables.Components1), eProcessRemoteControlDriver, eRCDEventComponents1},
	/*63*/   {&WiFiVariables.Components2, sizeof(WiFiVariables.Components2), eProcessRemoteControlDriver, eRCDEventComponents2},
	
        /*64*/   {&WiFiVariables.DailyHour, sizeof(WiFiVariables.DailyHour), 0, 0},
	/*65*/   {&WiFiVariables.DailyMin, sizeof(WiFiVariables.DailyMin), 0, 0},
	/*66*/   {&WiFiVariables.SpecialTimes_1, sizeof(WiFiVariables.SpecialTimes_1), 0, 0},
	/*67*/   {&WiFiVariables.ShoppingMode, sizeof(WiFiVariables.ShoppingMode), eProcessRemoteControlDriver, eRCDEventShoppingMode},
	/*68*/   {&WiFiVariables.SpecialTimes_3, sizeof(WiFiVariables.SpecialTimes_3), 0, 0},
	/*69*/   {&WiFiVariables.SpecialFunctions, sizeof(WiFiVariables.SpecialFunctions), eProcessRemoteControlDriver, eRCDEventSpecialFunctions},
	/*70*/   {&WiFiVariables.HandleLight, sizeof(WiFiVariables.HandleLight), eProcessRemoteControlDriver, eRCDEventHandleLight},
	/*71*/   {&WiFiVariables.DoorAlarmTime, sizeof(WiFiVariables.DoorAlarmTime), eProcessRemoteControlDriver, eRCDEventDoorAlarmTime},
	/*72*/   {&WiFiVariables.AIMode, sizeof(WiFiVariables.AIMode), eProcessRemoteControlDriver, eRCDEventAIMode},
	/*73*/   {&WiFiVariables.CheapSelectMin_3, sizeof(WiFiVariables.CheapSelectMin_3), eProcessRemoteControlDriver, eRCDEventCheapSelectMin_3},
	/*74*/   {&WiFiVariables.CheapSelectDuration_3, sizeof(WiFiVariables.CheapSelectDuration_3), eProcessRemoteControlDriver, eRCDEventCheapSelectDuration_3},
	/*75*/   {&WiFiVariables.CheapSelectFlag, sizeof(WiFiVariables.CheapSelectFlag), eProcessRemoteControlDriver, eRCDEventCheapSelectFlag},
	/*76*/   {&WiFiVariables.CheapSelectConfig, sizeof(WiFiVariables.CheapSelectConfig), eProcessRemoteControlDriver, eRCDEventCheapSelectConfig},
	/*77*/   {&WiFiVariables.CheapSelectCountDown, sizeof(WiFiVariables.CheapSelectCountDown), eProcessRemoteControlDriver, eRCDEventCheapSelectCountDown},
	
        /*78*/   {&WiFiVariables.DailyDay, sizeof(WiFiVariables.DailyDay), 0, 0},
	/*79*/   {&WiFiVariables.DailyMonth, sizeof(WiFiVariables.DailyMonth), 0, 0},
	/*80*/   {&WiFiVariables.DailyYear, sizeof(WiFiVariables.DailyYear), 0, 0},
	/*81*/   {&WiFiVariables.hTE_HighestTempRead, sizeof(WiFiVariables.hTE_HighestTempRead), 0, 0},
	/*82*/   {&WiFiVariables.hTE_WarningReason, sizeof(WiFiVariables.hTE_WarningReason), 0, 0},
	/*83*/   {&WiFiVariables.eE_EnergyEfficiencyDutyFF, sizeof(WiFiVariables.eE_EnergyEfficiencyDutyFF), eProcessRemoteControlDriver, eRCDEventeE_EnergyEfficiencyDutyFF}, 
	/*84*/   {&WiFiVariables.MercekErrorShadow1, sizeof(WiFiVariables.MercekErrorShadow1), 0, 0},
	/*85*/   {&WiFiVariables.MercekErrorShadow2, sizeof(WiFiVariables.MercekErrorShadow2), 0, 0},
        /*86*/   {&WiFiVariables.QuickIce60, sizeof(WiFiVariables.QuickIce60), eProcessRemoteControlDriver, eRCDEventQuickIce60}, 
        /*87*/   {&WiFiVariables.HandleLightIndex, sizeof(WiFiVariables.HandleLightIndex), eProcessRemoteControlDriver, eRCDEventRGBHandleLight},
        /*88*/   {&WiFiVariables.VacuumBoxSet, sizeof(WiFiVariables.VacuumBoxSet), eProcessRemoteControlDriver, eRCDEventVacuumBox},
        /*89*/   {&WiFiVariables.KeylockDspr, sizeof(WiFiVariables.KeylockDspr), eProcessRemoteControlDriver, eRCDEventKeylockDspr},
};   

const tsMercekMap mercekArrayMap[] = {
	/* 00 */  {&MercekData.SendingReason, sizeof(MercekData.SendingReason)},
	/* 01 */  {MercekData.MBStock, sizeof(MercekData.MBStock)},
	/* 05 */  {&MercekData.MBVersion, sizeof(MercekData.MBVersion)},
	/* 06 */  {&MercekData.MBRevision, sizeof(MercekData.MBRevision)},
	/* 07 */  {&MercekData.WorkMode, sizeof(MercekData.WorkMode)},
	/* 08 */  {&MercekData.CabinTypeMode, sizeof(MercekData.CabinTypeMode)},
	/* 09 */  {&MercekData.CabinSets, sizeof(MercekData.CabinSets)},
	/* 10 */  {MercekData.DoorOpenCounter, sizeof(MercekData.DoorOpenCounter)},
	/* 12 */  {MercekData.CabinDoorOpenTimer, sizeof(MercekData.CabinDoorOpenTimer)},
	/* 14 */  {&MercekData.HumanSensorCounter, sizeof(MercekData.HumanSensorCounter)},
	/* 15 */  {&MercekData.Functions, sizeof(MercekData.Functions)},
	/* 16 */  {&MercekData.Functions2, sizeof(MercekData.Functions2)},
	/* 17 */  {&MercekData.Functions3, sizeof(MercekData.Functions3)},
	/* 18 */  {MercekData.IceMachine, sizeof(MercekData.IceMachine)},
	/* 20 */  {(uint8_t*)MercekData.IceMachineTemp, sizeof(MercekData.IceMachineTemp)},
	/* 30 */  {&MercekData.IceMachineWaterHeater, sizeof(MercekData.IceMachineWaterHeater)},
	/* 31 */  {MercekData.Dispenser, sizeof(MercekData.Dispenser)},
	/* 33 */  {MercekData.Cabin0Components, sizeof(MercekData.Cabin0Components)},
	/* 37 */  {MercekData.Cabin1Components, sizeof(MercekData.Cabin1Components)},
	/* 41 */  {&MercekData.VacuumHeater, sizeof(MercekData.VacuumHeater)},
	/* 42 */  {&MercekData.CoolerType, sizeof(MercekData.CoolerType)},
	/* 43 */  {MercekData.SelenoidValve, sizeof(MercekData.SelenoidValve)},
	/* 47 */  {MercekData.SelenoidValveDetailed, sizeof(MercekData.SelenoidValveDetailed)},
	/* 77 */  {&MercekData.EnergyMonitor, sizeof(MercekData.EnergyMonitor)},
	/* 78 */  {MercekData.Compresor, sizeof(MercekData.Compresor)},
	/* 82 */  {MercekData.CompresorDetailed, sizeof(MercekData.CompresorDetailed)},
	/* 90 */  {MercekData.CompStock, sizeof(MercekData.CompStock)},
	/* 96 */  {&MercekData.CompVersion, sizeof(MercekData.CompVersion)},
	/* 97 */  {&MercekData.CompRevision, sizeof(MercekData.CompRevision)},
	/* 98 */  {&MercekData.CompCoreVersion, sizeof(MercekData.CompCoreVersion)},
	/* 99 */  {&MercekData.CompCoreRevision, sizeof(MercekData.CompCoreRevision)},
	/* 100 */ {&MercekData.CompType, sizeof(MercekData.CompType)},
	/* 101 */ {MercekData.CompError, sizeof(MercekData.CompError)},
	/* 103 */ {MercekData.CompPower, sizeof(MercekData.CompPower)},
	/* 105 */ {MercekData.CompDcbus, sizeof(MercekData.CompDcbus)},
	/* 107 */ {MercekData.CompRealSpeed, sizeof(MercekData.CompRealSpeed)},
	/* 108 */ {MercekData.Cabin0Defrost, sizeof(MercekData.Cabin0Defrost)},
	/* 110 */ {MercekData.Cabin1Defrost, sizeof(MercekData.Cabin1Defrost)},
	/* 112 */ {(uint8_t*)MercekData.Cabin0TempSensor, sizeof(MercekData.Cabin0TempSensor)},
	/* 132 */ {(uint8_t*)MercekData.Cabin1TempSensor, sizeof(MercekData.Cabin1TempSensor)},
	/* 152 */ {MercekData.Ambient, sizeof(MercekData.Ambient)},
	/* 156 */ {MercekData.Connection, sizeof(MercekData.Connection)},
	/* 158 */ {MercekData.ErrorCode, sizeof(MercekData.ErrorCode)},
	/* 166 */ {&MercekData.DataCount, sizeof(MercekData.DataCount)},
	/* 167 */ {&MercekData.MercekCount, sizeof(MercekData.MercekCount)},
};

const tsMercekMap secureMercekArrayMap[] = {
	/* 00 */ {&secureMercekVars.data1,  sizeof(secureMercekVars.data1)},
	/* 01 */ {&secureMercekVars.data2,  sizeof(secureMercekVars.data2)},
	/* 02 */ {&secureMercekVars.data3,  sizeof(secureMercekVars.data3)},
	/* 03 */ {&secureMercekVars.data4,  sizeof(secureMercekVars.data4)},
	/* 04 */ {&secureMercekVars.data5,  sizeof(secureMercekVars.data5)},
	/* 05 */ {&secureMercekVars.data6,  sizeof(secureMercekVars.data6)},
	/* 06 */ {&secureMercekVars.data7,  sizeof(secureMercekVars.data7)},
	/* 07 */ {&secureMercekVars.data8,  sizeof(secureMercekVars.data8)},
	/* 08 */ {&secureMercekVars.data9,  sizeof(secureMercekVars.data9)},
	/* 09 */ {&secureMercekVars.data10, sizeof(secureMercekVars.data10)},
	/* 10 */ {&secureMercekVars.data11, sizeof(secureMercekVars.data11)},
	/* 11 */ {&secureMercekVars.data12, sizeof(secureMercekVars.data12)},
	/* 12 */ {&secureMercekVars.data13, sizeof(secureMercekVars.data13)},
	/* 13 */ {&secureMercekVars.data14, sizeof(secureMercekVars.data14)},
	/* 14 */ {&secureMercekVars.data15, sizeof(secureMercekVars.data15)},
	/* 15 */ {&secureMercekVars.data16, sizeof(secureMercekVars.data16)},
	/* 16 */ {&secureMercekVars.data17, sizeof(secureMercekVars.data17)},
	/* 17 */ {&secureMercekVars.data18, sizeof(secureMercekVars.data18)},
	/* 18 */ {&secureMercekVars.data19, sizeof(secureMercekVars.data19)},
};

tsFeatureElement featureArray[FEATURE_COUNT]=
{
    FEATURE_ELEMENT(ENERGYMANAGEMENT, 0),
};

PROCESS_APPWFA_CREATE_WITH_SECURE_MERCEK(processWifiArray, eProcessWifiArray, wifiArrayMap, mercekArrayMap, secureMercekArrayMap, MERCEK_VERSION_MAJOR, MERCEK_VERSION_MINOR, eProcessRemoteControlDriver, eRCDEventMercekReadDone)
PROCESS_REMOTE_CONTROL_DRIVER_CREATE(remoteControlDriver, eProcessRemoteControlDriver, connSubStateWifi, WFA_SIZE, MERCEK_SIZE)
PROCESS_GDM_CREATE(processGdm, eProcessGdm, gdmMap, wifiUart, 1024, 190, eProcessCcmManager, eCcmArrangeMaxFrameSize, eProcessCcmManager, eCcmSafirGetCommStatus)

#if (CONNECTIVITY == _WIFI_CONNECTED)
PROCESS_CCM_CREATE(processCcm, eProcessCcm, attrbuteMap, eProcessCcmManager, eProcessGdm)
PROCESS_CCMSAFIR_CREATE(processCcmManager, eProcessCcmManager, attrbuteMap, eProcessCcm, eProcessRemoteControlDriver, eRCDEventOnboardingState, eProcessRemoteControlDriver, eRCDEventSafirComState)

#if (ENERGY_MONITORING == DEF_AVAILABLE)
tsConsumptionElement consumptionArray[] =
{
    // consumption elements - such as Energy etc.                                                                  200*60*100
    CONSUMPTION_ELEMENT(eConsumptionType_Energy_1, &energyMonitoringProcParams.energy.T1sec.instantValue , 0/*(ENERGY_READY_MEASUREMENT_LIMIT/600)*/, ENERGY_READY_TIME_LIMIT, 0, ENERGY_CONSUMPTION_SAVE_E2_PERIOD, NULL, 0, 60*60*100, eSendingOptionsAuto, (teMeasurementUnits)(MEASUREMENT_UNIT_ENERGY)),
};
PROCESS_CONSUMPTION_CREATE(processConsumption, eProcessConsumption, consumptionArray, eProcessGdm, eProcessRemoteControlDriver, eRCDEventConsumptionSent); // REV_GDM
#endif

uint8_t onboardingDoneControl = 0x10, setToZero = 0x00, setToSelectionModeActive = 0x02;//, calibCompleteControl = LD_CALIB_COMPLETE_FLAG;
const tsOtaCheckMap afterOtaMap[] = {
  NULL,
//	{otaProcessSet, &washerPermFlags.bytes.f1, sizeof(washerPermFlags.bytes.f1), &setToZero},
//	{otaProcessSet, &washerPermFlags.bytes.f2, sizeof(washerPermFlags.bytes.f2), &setToSelectionModeActive}, // setting only BFSW_SELECTION_MODE_ACTIVE
//	{otaProcessSyncBit, &programFlag.all, sizeof(programFlag.all), &onboardingDoneControl},					 // setting only Onboarding_done
//	{otaProcessSync, (uint8_t *)(&Wash_Cycle_Cnt), sizeof(Wash_Cycle_Cnt), (uint8_t *)(&washCycleCountE)},
//	{otaProcessSync, &Service_Mode_Comp_Cnt, sizeof(Service_Mode_Comp_Cnt), &serviceModeCompCntE},
//	{otaProcessSync, &Test_Mode_1_Comp_Cnt, sizeof(Test_Mode_1_Comp_Cnt), &testModeCompCntE},
//	{otaProcessSync, (uint8_t *)(&motorCalibrationValues.calibratedInertia), sizeof(motorCalibrationValues.calibratedInertia), (uint8_t *)(&calibratedInertiaE)},
//	{otaProcessSync, (uint8_t *)(&motorCalibrationValues.calibratedFriction), sizeof(motorCalibrationValues.calibratedFriction), (uint8_t *)(&calibratedFrictionE)},
//	{otaProcessSync, (uint8_t *)(&motorCalibrationValues.calibrationCompleted), sizeof(motorCalibrationValues.calibrationCompleted), (uint8_t *)(&calibCompleteControl)},
//	{otaProcessSync, (uint8_t *)(&dryCycleCount), sizeof(dryCycleCount), (uint8_t *)(&dryCycleCountE)},
};
unsigned char tempValue1 = 0;
PROCESS_OTAPROCESS_CREATE(processOta, eProcessOta, afterOtaMap, eProcessCcmManager, eCcmwriteAttMaxFrameSizeOta,
						  0, 0, 0, 0,
						  0, 0, 0)


PROCESS_FEATURE_CREATE(processFeature, eProcessFeature, featureArray, eProcessGdm, 14)


void setFeatures()
{
//    if (getEnergyMonitoringAvailability())
 //   {
        featureArray[0].featureID = ENERGYMANAGEMENT;
        featureArray[0].featureVersion = 1;
//    }
}

#endif

void safirStart()
{
    devComInit(&wifiUart);

    processStart(&remoteControlDriver);
    processStart(&processGdm);
    processStart(&processWifiArray);
#if (CONNECTIVITY == _WIFI_CONNECTED)
#if (ENERGY_MONITORING == DEF_AVAILABLE)
    processStart(&processConsumption);
#endif
    processStart(&processCcm);
    processStart(&processCcmManager);
    processStart(&processOta);
    setFeatures();
    processStart(&processFeature);
#endif // CONNECTIVITY
}
#endif
#endif /* SOURCES_APP_MW_RCOS_CONNECTIVITY_H_ */

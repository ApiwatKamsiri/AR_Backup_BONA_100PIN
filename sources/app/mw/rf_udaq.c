#include "rf_udaq.h"
#include "mw/udaq_rf.h"
#include "app/refrigerator/rf_ram.h"
#include "board/board.h"

#define _CabinUdaq(_no)                                                                                                  \
                UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.fan.speedOrPWM),                                                       \
		{(uint8_t *)&refrigerator_R.Cabin[_no].components.flap.value, 2 /*TODO in old uxPure only one byte is used*/},     \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.heaterA.value),                                                    \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.heaterB.value),                                                    \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.heaterC.value),                                                    \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].components.VacuumBox.value), 1 /*TODO in old uxPure only one byte is used*/}, \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.innerLight.value),                                                 \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.blueLight.value),                                                  \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.doorSwitch[0]),                                                      \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.doorSwitch[1]),                                                      \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].components.airTempSensor.valueInCExt) + 1, 1},                             \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].components.airTempSensor.valueInCExt), 1},                                 \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.airTempSensor.error),                                              \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].components.evaTempSensor.valueInCExt) + 1, 1},                             \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].components.evaTempSensor.valueInCExt), 1},                                 \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].components.evaTempSensor.error),                                              \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].sets.datas.display),                                                          \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.coolCutInTemp) + 1, 1},                                         \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.coolCutInTemp), 1},                                             \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.coolCutOutTemp) + 1, 1},                                        \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.coolCutOutTemp), 1},                                            \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.fanCutInTemp) + 1, 1},                                          \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.fanCutInTemp), 1},                                              \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.fanCutOutTemp) + 1, 1},                                         \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.fanCutOutTemp), 1},                                             \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.lowTempHeaterCutInTemp) + 1, 1},                                \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.lowTempHeaterCutInTemp), 1},                                    \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.lowTempHeaterCutOutTemp) + 1, 1},                               \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].sets.datas.lowTempHeaterCutOutTemp), 1},                                   \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].states.Sensor),                                                               \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].states.Work),                                                                 \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].states.Cool),                                                                 \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].states.Defrost),                                                              \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].states.Fan),                                                                  \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].states.HeaterA),                                                              \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].states.HeaterB),                                                              \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].states.HeaterC),                                                              \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.gasDesired),                                                           \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.gasApplied),                                                           \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].stateTimers.Cool),                                                            \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].stateTimers.Idle),                                                            \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].stateTimers.Defrost),                                                         \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].stateTimers.Drainage),                                                        \
		{(uint8_t *)(&refrigerator_R.Cabin[_no].stateTimers.HotStart), 1},                                                 \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.duty.mean),                                                            \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.cycleCounter),                                                         \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.workTimer),                                                               \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.realTimer),                                                           \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.quickTimer),                                                           \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.VacuumBox.onTimer),                                                            \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.VacuumBox.offTimer),                                                     \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.defrostDoorOpenedTimer),                                               \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.errDefrostHeaterCounter),                                              \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.HighTemErr.activationTimer),                                           \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.highTempStayTimer),                                                    \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.fanLevel),                                                             \
		UDAQ_ITEM(&refrigerator_R.Cabin[_no].values.fanLevelStayTimer)

const tsUdaqItem udaqMap[] = {
	/********************************* _Definitions 8 *********************************/
	UDAQ_ITEM(&refrigerator_R.Stock.No[2]),
	UDAQ_ITEM(&refrigerator_R.Stock.No[1]),
	UDAQ_ITEM(&refrigerator_R.Stock.No[0]),
	UDAQ_ITEM(&refrigerator_R.Stock.Version),
	UDAQ_ITEM(&refrigerator_R.Stock.Revision),

	/********************************* _ProductionTest 3 *********************************/
	UDAQ_ITEM(&refrigerator_R.workMode.values.prodTest.state),
	UDAQ_ITEM(&refrigerator_R.workMode.values.prodTest.minTimer),

	/********************************* Dispenser 4 *********************************/
	UDAQ_ITEM(&refrigerator_R.Dispenser.sets.state),
	UDAQ_ITEM(&refrigerator_R.Dispenser.components.tactSwitch.value),
	UDAQ_ITEM(&refrigerator_R.Dispenser.components.light.value),      

	/********************************* _IceCreamMachine 8 *********************************/
	{(uint8_t *)(&refrigerator_R.Cooler.components.ambientTempSensor.valueInCExt) + 1, 1}, // UDAQ_ITEM macro cannot be used because UDAQ wants this data in big endian
	{(uint8_t *)(&refrigerator_R.Cooler.components.ambientTempSensor.valueInCExt), 1},
	UDAQ_ITEM(&refrigerator_R.Cooler.components.ambientTempSensor.error),
#if defined(AI_COOL_V1_5)
	UDAQ_ITEM(&refrigerator_R.AI.predictedPersona),
#else
	UDAQ_ITEM(&refrigerator_R.Cooler.parameters.ambientType),   
#endif        
	UDAQ_ITEM(&refrigerator_R.Cooler.parameters.ambientType),
	UDAQ_ITEM(&refrigerator_R.Cooler.components.humiditySensor.humidityValue),
	UDAQ_ITEM(&refrigerator_R.Cooler.components.humiditySensor.error),
	UDAQ_ITEM(&refrigerator_R.Cooler.parameters.humidityType),

	/********************************* _IceMachine 21 *********************************/
	{(uint8_t *)(&refrigerator_R.IceMachine.components.caseTempSensor.valueInCExt) + 1, 1},
	{(uint8_t *)(&refrigerator_R.IceMachine.components.caseTempSensor.valueInCExt), 1},
	UDAQ_ITEM(&refrigerator_R.IceMachine.components.caseTempSensor.error),
	UDAQ_ITEM(&refrigerator_R.IceMachine.components.caseRotator.value),
	UDAQ_ITEM(&refrigerator_R.IceMachine.components.caseTactSwitch.value),
	UDAQ_ITEM(&refrigerator_R.IceMachine.components.boxTactSwitch.value),
	{(uint8_t *)(&refrigerator_R.IceMachine.sets), 1}, // bit fields cannot be addressed or used with sizeof
	{(uint8_t *)(&refrigerator_R.IceMachine.values), 1},
	UDAQ_ITEM(&refrigerator_R.IceMachine.values.state),
	UDAQ_ITEM(&refrigerator_R.IceMachine.values.step),
	{(uint8_t *)(&refrigerator_R.IceMachine.values.waterPumpTimer),1},
	UDAQ_ITEM(&refrigerator_R.IceMachine.values.waterVacumTimer),
	{(uint16_t *)(&refrigerator_R.IceMachine.values.stepTimer),1},//UDAQ_ITEM(&refrigerator_R.Cabin[0].stateTimers.PumpOut),
        //UDAQ_ITEM(&refrigerator_R.Cabin[0].stateTimers.PumpOut+1),
	UDAQ_ITEM(&refrigerator_R.IceMachine.values.makeIceTimer),
        UDAQ_ITEM(&refrigerator_R.IceMachine.values.checkWaterTimer),//{(uint8_t *)&refrigerator_R.Cooler.parameters.valveStateStayTimer, 1},
        //{(uint8_t *)&refrigerator_R.Cooler.parameters.valveStateStayTimer+1, 1},
	UDAQ_ITEM(&refrigerator_R.IceMachine.values.checkWaterTempTimer),
	UDAQ_ITEM(&refrigerator_R.IceMachine.values.checkBoxTimer),//UDAQ_ITEM(&refrigerator_R.ui.serviceButton.state),
	UDAQ_ITEM(&refrigerator_R.IceMachine.values.caseTactSWErrorTimer),//UDAQ_ITEM(&refrigerator_R.ui.serviceButton.statePrev),
	UDAQ_ITEM(&refrigerator_R.IceMachine.values.caseTactSWErrorReTryTimer),//UDAQ_ITEM(&refrigerator_R.ui.serviceButton.timeOut.timer),

	/********************************* _WaterSource 9 *********************************/
	UDAQ_ITEM(&refrigerator_R.WaterSource.components.mainPump.value),
	UDAQ_ITEM(&refrigerator_R.WaterSource.components.mainValve.value),
	UDAQ_ITEM(&refrigerator_R.WaterSource.components.dispenserPump.value),
	UDAQ_ITEM(&refrigerator_R.WaterSource.components.iceMachinePump.value),
	UDAQ_ITEM(&refrigerator_R.WaterSource.components.waterVentilHeater.value),
	UDAQ_ITEM(&refrigerator_R.WaterSource.values.heaterTimer),
	UDAQ_ITEM(&refrigerator_R.WaterSource.values.filterTimer),

	/********************************* _IceSource 3 *********************************/
	UDAQ_ITEM(&refrigerator_R.IceSource.components.selector.value),
	UDAQ_ITEM(&refrigerator_R.IceSource.components.shutter.value),
	UDAQ_ITEM(&refrigerator_R.IceSource.components.pusher.value),
        
	/********************************* _Cooler 15 *********************************/
	UDAQ_ITEM(&refrigerator_R.Cooler.components.compressor.value),
	UDAQ_ITEM(&refrigerator_R.Cooler.components.condenserFan.speedOrPWM),
	UDAQ_ITEM(&refrigerator_R.Cooler.components.solenoidValve[0].positionIndex),
	{(uint8_t *)&refrigerator_R.Cooler.components.solenoidValve[0].position,1},
	{(uint8_t *)&refrigerator_R.Cooler.parameters, 1}, // bit fields cannot be addressed or used with sizeof
	UDAQ_ITEM(&refrigerator_R.Cooler.parameters.state),
	UDAQ_ITEM(&refrigerator_R.Cooler.parameters.priority),
	UDAQ_ITEM(&refrigerator_R.Cooler.parameters.gasLevel),
        {(uint8_t *)&refrigerator_R.Cooler.parameters.gasLevelStayTimerFRZ, 1}, //{(uint8_t *)&refrigerator_R.Cooler.parameters.gasLevelStayTimer, 1}, 
	UDAQ_ITEM(&refrigerator_R.Cooler.parameters.compressorOnTimer),
	UDAQ_ITEM(&refrigerator_R.Cooler.parameters.compressorOffTimer),
        {(uint8_t *)&refrigerator_R.Cooler.parameters.solenoidValvePositionTimer, 1},

	/********************************* _Common 13 *********************************/
	UDAQ_ITEM(&refrigerator_R.Sets.user.functions),
	UDAQ_ITEM(&refrigerator_R.Sets.work.functions),
	{(uint8_t *)&refrigerator_R.Values, 1},
	UDAQ_ITEM(&refrigerator_R.Values.doorsOpenedCounter),
	UDAQ_ITEM(&refrigerator_R.Values.doorsClosedTimer),
	UDAQ_ITEM(&refrigerator_R.Values.doorsClosedBlockTimer),
	{(uint8_t *)&refrigerator_R.Values.doorsAlertTimer, 1}, // TODO in old uxPure only one byte is used
	UDAQ_ITEM(&refrigerator_R.Cooler.parameters.gasLevelStayTimerFF),//UDAQ_ITEM(&refrigerator_R.Values.sabbathTimer),
	UDAQ_ITEM(&refrigerator_R.Values.ecoExtraTimer),

	/********************************* _Cabin 70 * 2 = 140 *********************************/
	_CabinUdaq(0),
	_CabinUdaq(1),

	/********************************* _Extras 3 *********************************/
	UDAQ_ITEM(&refrigerator_R.Values.doorsClosedStateStayTimer),
	UDAQ_ITEM(&refrigerator_R.workMode.values.workMode),
};

const tsUdaqItem udaqMapSub[] = {
        /********************************* Connected 28 *********************************/
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                   
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.Cooler.parameters.gasLevel),1},                                                                
        {(uint8_t *)(&refrigerator_R.ConnectedApp.dailyTime.cDailyHour),1},                      
        {(uint8_t *)(&refrigerator_R.ConnectedApp.dailyTime.cDailyMin),1},                       
        {(uint8_t *)(&refrigerator_R.ConnectedApp.fSGrid.DeDefrost.cCheapSelectFlag.byte),1},    
        {(uint8_t *)(&refrigerator_R.ConnectedApp.fSGrid.DeDefrost.cCheapSelectConfig.byte),1},  
        {(uint8_t *)(&refrigerator_R.ConnectedApp.fSGrid.DeDefrost.cCheapSelectCountDown),1},    
        {(uint8_t *)(&refrigerator_R.Cabin[1].values.realTimer),1},                              
        {(uint8_t *)(&refrigerator_R.Cabin[1].values.realTimer) + 1,1},                          
        {(uint8_t *)(&refrigerator_R.Cabin[1].values.activeRealTimer),1},                        
        {(uint8_t *)(&refrigerator_R.Cabin[1].values.activeRealTimer) + 1,1},                    
        {(uint8_t *)(&refrigerator_R.ConnectedApp.dailyTime.cDailyDay),1},                       
        {(uint8_t *)(&refrigerator_R.ConnectedApp.dailyTime.cDailyMonth),1},                     
        {(uint8_t *)(&refrigerator_R.ConnectedApp.dailyTime.cDailyYear),1},
        
        /********************************* _Cabin 70 * 2 = 140 *********************************/
	_CabinUdaq(2),
	_CabinUdaq(3),
        
        /********************************* Extra 7 *********************************/
        {(uint8_t *)(&refrigerator_R.workMode.values.workModeTimer),1},                                       
        {(uint8_t *)(&refrigerator_R.Cabin[1].values.HighTemErr.algorithmActivationTime),1},     
        {(uint8_t *)(&refrigerator_R.Cabin[1].values.HighTemErr.algorithmActivationTime) + 1,1}, 
        {(uint8_t *)(&refrigerator_R.Cabin[1].values.HighTemErr.delayTime),1},                   
        {(uint8_t *)(&refrigerator_R.Cabin[1].values.HighTemErr.delayTime) + 1,1},               
        {(uint8_t *)(&refrigerator_R.IceMachine.components.fan.speedOrPWM),1},                   
        {(uint8_t *)(&refrigerator_R.IceMachine.components.fan.speedOrPWM) + 1,1},
};

const tsUdaqItem udaqMapFTC[] = {
    {(uint8_t *)(&refrigerator_R.workMode.values.workMode),1},// 0
    {(uint8_t *)(&refrigerator_R.Stock.Version),1},   // 1
    {(uint8_t *)(&refrigerator_R.Stock.Revision),1},  // 2
    {(uint8_t *)(&FTC.Tests.ICT),1},                  // 3
    {(uint8_t *)(&FTC.Tests.FTC),1},                  // 4
    {(uint8_t *)(&FTC.ACOut.index),1},                // 5
    {(uint8_t *)(&FTC.ACOut.value),1},                // 6
    {(uint8_t *)(&FTC.ACSwitch.index),1},             // 7
    {(uint8_t *)(&FTC.ACSwitch.value),1},             // 8
    {(uint8_t *)(&FTC.ADC_.index),1},                 // 9
    {(uint8_t *)(&FTC.ADC_.value),1},                 // 10
    {(uint8_t *)(&FTC.ADC_.value) + 1,1},             // 11
    {(uint8_t *)(&FTC.BDCMotor.index),1},             // 12
    {(uint8_t *)(&FTC.BDCMotor.value),1},             // 13
    {(uint8_t *)(&FTC.Buzzer.value),1},               // 14
    {(uint8_t *)(&FTC.CSDCMotor.index),1},            // 15
    {(uint8_t *)(&FTC.CSDCMotor.value),1},            // 16
    {(uint8_t *)(&FTC.DCOut.index),1},                // 17
    {(uint8_t *)(&FTC.DCOut.value),1},                // 18
    {(uint8_t *)(&FTC.DCSwitch.index),1},             // 19
    {(uint8_t *)(&FTC.DCSwitch.value),1},             // 20
    {(uint8_t *)(&FTC.PWMOut.index),1},               // 21
    {(uint8_t *)(&FTC.PWMOut.value),1},               // 22
    {(uint8_t *)(&FTC.PWMOut.value) + 1,1},           // 23
    {(uint8_t *)(&FTC.StepMotor.index),1},            // 24
    {(uint8_t *)(&FTC.StepMotor.value),1},            // 25
    {(uint8_t *)(&FTC.StepMotor.value) + 1,1},        // 26
    {(uint8_t *)(&FTC.VCC.index),1},                  // 27
    {(uint8_t *)(&FTC.VCC.value),1},                  // 28
    {(uint8_t *)(&FTC.VSDCMotor.index),1},            // 29
    {(uint8_t *)(&FTC.VSDCMotor.value),1},            // 30
    {(uint8_t *)(&FTC.VSDCMotor.value) + 1,1},        // 31
    {(uint8_t *)(&refrigerator_R.Stock.No[0]),1},     // 32
    {(uint8_t *)(&refrigerator_R.Stock.No[0]) + 1,1}, // 33
    {(uint8_t *)(&refrigerator_R.Stock.No[1]),1},     // 34
    {(uint8_t *)(&refrigerator_R.Stock.No[1]) + 1,1}, // 35
    {(uint8_t *)(&refrigerator_R.Stock.No[2]),1},     // 36
    {(uint8_t *)(&refrigerator_R.Stock.No[2]) + 1,1}, // 37
    {(uint8_t *)(&FTC.VSDCMotor.speed),1},            // 38
    {(uint8_t *)(&FTC.VSDCMotor.speed) + 1,1},        // 39
    {(uint8_t *)(&refrigerator_R.ui.dspPacket.displayInfo.id),1},                       // 40
    {(uint8_t *)(&refrigerator_R.ui.dspPacket.displayInfo.swVer),1},                    // 41
    {(uint8_t *)(&refrigerator_R.ui.dspPacket.displayInfo.swRev),1},                    // 42
    {(uint8_t *)(&FTC.Eeprom.index),1},                                                 // 43
    {(uint8_t *)(&FTC.Eeprom.value),1},                                                 // 44
    {(uint8_t *)(&FTC.Eeprom.feedback),1},                                              // 45
    {(uint8_t *)(&FTC.WiFiUart.index),1},                                               // 46
    {(uint8_t *)(&FTC.WiFiUart.value),1},                                               // 47
    {(uint8_t *)(&FTC.WiFiUart.feedback),1},                                            // 48
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.No[0]),1},          // 49
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.No[0]) + 1,1},      // 50
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.No[1]),1},          // 51
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.No[1]) + 1,1},      // 52
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.No[2]),1},          // 53
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.No[2]) + 1,1},      // 54
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.Version),1},        // 55
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.Revision),1},       // 56
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.CoreVersion),1},    // 57
    {(uint8_t *)(&refrigerator_R.Cooler.components.compressor.Stock.CoreRevision),1},   // 58
#ifdef ALPS_H50
    {(uint8_t *)(&FTC.DUTY.index),1},                  // 59
    {(uint8_t *)(&FTC.DUTY.value),1},                  // 60
#endif
};

tsTarget udaqTarget = {
	.config = NULL,
};

PROCESS_UDAQ_RF_CREATE(rfUdaqProc, udaqUart, udaqTarget, udaqMap, udaqMapSub, udaqMapFTC, 2, 55, 18, 19, 1, 1, 0)
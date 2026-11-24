#include "rf_errors.h"
#include "app/libs/temp_funcs.h"
#include "../../mw/connectivity/remote_control.h"

static unsigned char sensorErrorsTable[] =
	{
		_Error_FRZ_Air,
		_Error_FRZ_Eva,
		_Error_FF_Eva,
		_Error_FF_Air,
		_Error_Cooler_Ambient,
		_Error_IceMachine_CaseTemp,
		_Error_JKR_Air,
		_Error_JKR_Eva,
		_Error_FLAP_Air,
		//_Error_BioFRESH_Air,
};

static unsigned char fanErrorsTable[] =
	{
		_Error_FRZ_Fan,
		_Error_JKR_Fan,
		_Error_Cooler_CondanserFan,
		_Error_FF_Fan,
		_Error_FLAP_Fan,
		//_Error_BioFRESH_Fan,
};

#define _SensorErrorTableSize sizeof(sensorErrorsTable) / sizeof(sensorErrorsTable[0])
#define _FanErrorTableSize sizeof(fanErrorsTable) / sizeof(fanErrorsTable[0])

static void rfErrorsCheckCooler(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP, stRefrigeratorErrors *er);
static void rfErrorsCheckCabin(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP, unsigned char cabin_Count, stRefrigeratorErrors *er);
static void rfErrorsCheckIceMachine(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP, stRefrigeratorErrors *er);
static void ErrorsCheckWifi(void);

void rfErrorsCheckAll_123(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
	UNUSED(rfC);
	UNUSED(rfP);
}

void rfErrorsCheckAll(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP)
{
	stRefrigeratorErrors *er = &rfP->Values.errors;
	er->highTempErrNumber = 51;

	er->errorsNumber = 0;
	er->sensorErrorsNumber = 0;
	er->fanErrorsNumber = 0;

	for (unsigned char i = 0; i < _SizeOfErrorsCheckTable; i++)
		er->errorsCheckTable[i] = 0;

	// ErrorsCheckHighTemp
	if (rfP->Values.errHighTemp)
		er->errorsCheckTable[_Error_HighTemperature] |= 0x01;

	rfErrorsCheckCabin(rfC, rfP, _NumOfCabin, er);
	rfErrorsCheckIceMachine(rfC, rfP, er);
	rfErrorsCheckCooler(rfC, rfP, er);
	ErrorsCheckWifi();

	for (unsigned char i = 0; i < _SizeOfErrorsCheckTable; i++)
	{
		if (er->errorsCheckTable[i])
		{
			if (i == _Error_HighTemperature)
			{
				if (rfP->Values.tempType == _TempType_Celcius)
					er->errorsTable[er->errorsNumber] = (unsigned char)celciusExtToCelcius(rfP->Cabin[1].values.HighTemErr.maxFRZTempInCext);
				else if (rfP->Values.tempType == _TempType_Fahrenheit)
					er->errorsTable[er->errorsNumber] = (unsigned char)celciusToFahnrenheight(celciusExtToCelcius(rfP->Cabin[1].values.HighTemErr.maxFRZTempInCext));
				er->highTempErrNumber = er->errorsNumber;
			}
			else
				er->errorsTable[er->errorsNumber] = i;
			er->errorsNumber++;
		}
	}

	for (unsigned char i = 0; i < _SensorErrorTableSize; i++)
	{
		if (er->errorsCheckTable[sensorErrorsTable[i]])
		{
			er->sensorErrorsNumber++;
		}
	}

	for (unsigned char i = 0; i < _FanErrorTableSize; i++)
	{
		if (er->errorsCheckTable[fanErrorsTable[i]])
		{
			er->fanErrorsNumber++;
		}
	}
}

static void rfErrorsCheckCooler(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP, stRefrigeratorErrors *er)
{
	if (rfP->Cooler.components.condenserFan.error)
		er->errorsCheckTable[_Error_Cooler_CondanserFan] |= 0x01;

    if ((rfC->Cooler.components.ambientTempSensor.type == _TempSensorType_NTCwithADC || rfC->Cooler.components.ambientTempSensor.type == _TempSensorType_NTC_Ambient_ADC) &&
		rfP->Cooler.components.ambientTempSensor.error)
	{
		if (!rfC->Cooler.components.ambientTempSensor.hideError ||
			(rfP->workMode.values.workMode == _WM_Service)
			|| (rfP->workMode.values.prodTest.minTimer < 1))
			er->errorsCheckTable[_Error_Cooler_Ambient] |= 0x01;
	}
	else if (rfC->Cooler.components.ambientTempSensor.type == _TempSensorType_SHT3X &&
			 rfP->Cooler.components.ambientTempSensor.error &&
			 ((rfP->workMode.values.workMode == _WM_Service)
			  || (rfP->workMode.values.prodTest.minTimer < 1)))
		er->errorsCheckTable[_Error_Cooler_Ambient] |= 0x01;
	else
		er->errorsCheckTable[_Error_Cooler_Ambient] = 0x00;
}

static void rfErrorsCheckCabin(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP,
							   unsigned char cabin_Count, stRefrigeratorErrors *er)
{
	unsigned char errAirIndex;
	unsigned char errEvaIndex;
	unsigned char errFanIndex;
	unsigned char errDefrostHeaterIndex;

	for (unsigned char i = 0; i < cabin_Count; i++)
	{
		errAirIndex = 0xFF;
		errEvaIndex = 0xFF;
		errFanIndex = 0xFF;
		errDefrostHeaterIndex = 0xFF;

		switch (rfC->Cabin[i].type)
		{
		case _CabinType_FF:
			errAirIndex = _Error_FF_Air;
			errEvaIndex = _Error_FF_Eva;
			errFanIndex = _Error_FF_Fan;
			break;

		case _CabinType_FRZ:
			errAirIndex = _Error_FRZ_Air;
			errEvaIndex = _Error_FRZ_Eva;
			errFanIndex = _Error_FRZ_Fan;
			errDefrostHeaterIndex = _Error_FRZ_DefrostHeater;
			break;

		case _CabinType_JKR:
			errAirIndex = _Error_JKR_Air;
			errEvaIndex = _Error_JKR_Eva;
			errFanIndex = _Error_JKR_Fan;
			errDefrostHeaterIndex = _Error_JKR_DefrostHeater;
			break;
		case _CabinType_FLAP:
			errAirIndex = _Error_FLAP_Air;
			errFanIndex = _Error_FLAP_Fan;
			break;
			/*case _CabinType_BioFRESH:
				errAirIndex = _Error_BioFRESH_Air;
				errFanIndex = _Error_BioFRESH_Fan;
			break;*/

		case _CabinType_NONE:
		default:;
			break;
		}

		if (errAirIndex < _SizeOfErrorsCheckTable && rfP->Cabin[i].components.airTempSensor.error)
			if (!rfC->Cabin[i].components.airTempSensor.hideError ||
				(rfP->workMode.values.workMode == _WM_Service)
				|| (rfP->workMode.values.prodTest.minTimer < 1))
				er->errorsCheckTable[errAirIndex] |= 0x01 << i;

		if ((errEvaIndex < _SizeOfErrorsCheckTable) &&
			(rfP->Cabin[i].components.evaTempSensor.error))
			if (!rfC->Cabin[i].components.evaTempSensor.hideError ||
				(rfP->workMode.values.workMode == _WM_Service)
				|| (rfP->workMode.values.prodTest.minTimer < 1))
				er->errorsCheckTable[errEvaIndex] |= 0x01 << i;

		if ((errFanIndex < _SizeOfErrorsCheckTable) &&
			(rfP->Cabin[i].components.fan.error))
			er->errorsCheckTable[errFanIndex] |= 0x01 << i;

		if ((errDefrostHeaterIndex < _SizeOfErrorsCheckTable) &&
			(rfP->Cabin[i].values.errDefrostHeater))
			er->errorsCheckTable[errDefrostHeaterIndex] |= 0x01 << i;
	}
}

static void rfErrorsCheckIceMachine(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP, stRefrigeratorErrors *er)
{
	if ((rfC->IceMachine.type != _IceMachineType_NONE) &&
		(!rfP->IceMachine.sets.iceOFF))
	{
		if (rfP->IceMachine.components.caseTempSensor.error)
			er->errorsCheckTable[_Error_IceMachine_CaseTemp] = 1;

		if (rfP->IceMachine.values.errCaseTactSW)
			er->errorsCheckTable[_Error_IceMachine_CaseTactSW] = 1;
	}
}

static void ErrorsCheckWifi(void)
{
#if (CONNECTIVITY != _NON_CONNECTED)
    /* if(wifiError_B1==1) 
     refrigerator_R.Values.errors.errorsCheckTable[ _Error_Wifi_B1 ] |= 1;*/
     if (wifiError_B2 == 1)
         refrigerator_R.Values.errors.errorsCheckTable[_Error_Wifi_B2] = 1;
     else if (wifiError_B2 == 0)
         refrigerator_R.Values.errors.errorsCheckTable[_Error_Wifi_B2] = 0;
#endif
}

void rfCheckHighTempError(stRefrigerator_Ram *rfP)
{
	bool errHighTemp = false;

	for (unsigned char i = 0; i < _NumOfCabin; i++)
		errHighTemp |= rfP->Cabin[i].values.HighTemErr.isactive;

	if (rfP->workMode.values.workMode != _WM_Distributer /*|| WorkMode != _WM_FairMode*/)
		rfP->Values.errHighTemp = errHighTemp;
	else
		rfP->Values.errHighTemp = 0;
}
/*** APW : Error ARM mode ***/
void rfInitARMModeError(stRefrigerator_Ram *rfP)
{
#ifdef HOSHI_H30
    rfP->workMode.values.ARMModeError.FRZSensorAbnormal = false;                //For HOSHI
    rfP->workMode.values.ARMModeError.FFSensorAbnormal = false;                 //For HOSHI
    rfP->workMode.values.ARMModeError.AmbientSensorAbnormal = false;            //For HOSHI
    rfP->workMode.values.ARMModeError.FRZEVASensorAbnormal = false;             //For HOSHI
    rfP->workMode.values.ARMModeError.EEPCommunicationError = false;            //For HOSHI
    rfP->workMode.values.ARMModeError.CompressorError = false;                  //For HOSHI
    rfP->workMode.values.ARMModeError.CompressorCommunicationError = false;     //For HOSHI
    rfP->workMode.values.ARMModeError.FRZFanAbnormal = false;                   //For HOSHI
    rfP->workMode.values.ARMModeError.CondensorFanAbnormal = false;             //For HOSHI
    rfP->workMode.values.ARMModeError.FFFanAbnormal = false;                    //For HOSHI
    rfP->workMode.values.ARMModeError.IMUnitAbnormal = false;                   //For HOSHI
    rfP->workMode.values.ARMModeError.IMSensorAbnormal = false;                 //For HOSHI
    rfP->workMode.values.ARMModeError.DefrostCircuitAbnormal = false;           //For HOSHI
    rfP->workMode.values.ARMModeError.PowerSupplyVoltageAbnormal = false;       //For HOSHI
    rfP->workMode.values.ARMModeError.FreezerNotCool = false;                   //For HOSHI
    rfP->workMode.values.ARMModeError.FreshfoodNotCool = false;                 //For HOSHI
    rfP->workMode.values.ARMModeError.HuminitySensorAbnormal = false;           //For HOSHI
    rfP->workMode.values.ARMModeError.IceInspectionAbnormal = false;            //For HOSHI
    rfP->workMode.values.ARMModeError.HighTemperatureAbnormal = false;          //For HOSHI
    rfP->workMode.values.ARMModeError.ControlPanelCommunicationError = false;   //For HOSHI
    rfP->workMode.values.ARMModeError.FSSensorAbnormal = false;                 //For HOSHI
    rfP->workMode.values.ARMModeError.PingTestError = false;                    //APW : Initial PingTestError to false only
    rfP->workMode.values.ARMModeError.ValveARM = false;                         //APW : Add CT heater software 
#else
    rfP->workMode.values.ARMModeError.PingTestError = false;                    //APW : Initial PingTestError to false only
    rfP->workMode.values.ARMModeError.ValveARM = false;                         //APW : Add CT heater software 
#endif //HOSHI_H30
}

void rfARMModeErrorOutput20MsTask(stRefrigerator_Ram *rfP)
{
#ifdef HOSHI_H30
    stRefrigeratorErrors *er = &rfP->Values.errors;
//Priority high ^
    //error E0 : Freezer room Sensor Error
    if(er->errorsCheckTable[0])                                                 
      rfP->workMode.values.ARMModeError.FRZSensorAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.FRZSensorAbnormal = false;
    //error E3 : Refrigerator Sensor Error, E18 : Flap Air Sensor Error
    if(er->errorsCheckTable[3] || er->errorsCheckTable[18])                
      rfP->workMode.values.ARMModeError.FFSensorAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.FFSensorAbnormal = false;
    //error E5 : Ambient Sensor Error
    if(er->errorsCheckTable[5])                                            
      rfP->workMode.values.ARMModeError.AmbientSensorAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.AmbientSensorAbnormal = false;
    //error E1 : Freezer Evaporator Sensor Error
    if(er->errorsCheckTable[1])                                            
      rfP->workMode.values.ARMModeError.FRZEVASensorAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.FRZEVASensorAbnormal = false;
    //error E13 : Freezer Fan Error
    if(er->errorsCheckTable[13])                                           
      rfP->workMode.values.ARMModeError.FRZFanAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.FRZFanAbnormal = false;
    //error E15 : K-Fan Error (Condansor Fan)
    if(er->errorsCheckTable[15])                                           
      rfP->workMode.values.ARMModeError.CondensorFanAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.CondensorFanAbnormal = false;
    //error E16 : Refrigerator Fan Error, E17 : Flap Fan Error
    if(er->errorsCheckTable[16] || er->errorsCheckTable[17])               
      rfP->workMode.values.ARMModeError.FFFanAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.FFFanAbnormal = false;
    //error E9 : Ice Maker Motor Error
    if(er->errorsCheckTable[9])                                            
      rfP->workMode.values.ARMModeError.IMUnitAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.IMUnitAbnormal = false;
    //error E8 : Ice Maker Sensor Error
    if(er->errorsCheckTable[8])                                            
      rfP->workMode.values.ARMModeError.IMSensorAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.IMSensorAbnormal = false;
    //error E4 : Freezer Defrost System Error
    if(er->errorsCheckTable[4])                                            
      rfP->workMode.values.ARMModeError.DefrostCircuitAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.DefrostCircuitAbnormal = false;
    //error E19 : High Temperature
    if(er->errorsCheckTable[19])                                           
      rfP->workMode.values.ARMModeError.HighTemperatureAbnormal = true;
    else
      rfP->workMode.values.ARMModeError.HighTemperatureAbnormal = false;
//Priority low
#endif
}
/*** APW : Error ARM mode ***/
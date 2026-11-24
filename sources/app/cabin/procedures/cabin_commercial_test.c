/*** APW : Add CT heater software ***/
#include "cabin_commercial_test.h"
#include "cabin_gas_level.h"

#include "../components/cabin_switch.h"
#include "../components/cabin_inner_light.h"
#include "../components/cabin_blue_light.h"

#include "app/libs/temp_funcs.h"
#include "app/libs/component_test.h"

void cabinCheckAirSensorCommercial(stCabin_Flash const *cabinC,
						 stCabin_Ram *cabinP,
						 unsigned short testTimer);

void cabinCheckEvaSensorCommercial(stCabin_Flash const *cabinC,
						 stCabin_Ram *cabinP,
						 unsigned short testTimer);

void cabinCheckALLCommercial(stCabin_Flash const *cabinC,
				   stCabin_Ram *cabinP,
				   unsigned short testTimer);

void cabinCloseAllComponentsCommercial(stCabin_Ram *cabinP)
{
	cabinP->components.fan.speedOrPWM = 0;
	cabinP->components.flap.value = 0;
        cabinP->components.heaterA.value = _HeaterValue_OFF;
        cabinP->components.heaterB.value = _HeaterValue_OFF;
        cabinP->components.heaterC.value = _HeaterValue_OFF;
	cabinP->components.ionizer.value = _IonizerValue_OFF;
	cabinP->components.violed.value = _VioLedValue_OFF;
	cabinP->components.blueLight.value = 0;
	cabinP->components.innerLight.value = 0;
	// cabinP->components.blueLight.step   = BLUE_LIGHT_STEP;
	// cabinP->components.innerLight.step  = INNER_LIGHT_STEP;
}

void cabinCommercialTestsFailedTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;

	cabinCloseAllComponentsCommercial(cabinP);
	// CabinSwitchTask( cabinP );
	cabinBlueLightTask(consts, params);
	cabinInnerLightTask(consts, params);

	cabinP->values.gasDesired.type = _GT_Failed;
	cabinP->values.gasDesired.level = 0;
}

void cabinPerformanceCommercialTestInit(stCabin_Flash const *cabinC,
							  stCabin_Ram *cabinP)
{
	if (cabinC->type)
		cabinP->states.Work = _Work_CommercialTestMode;

	cabinP->values.performance.airChecked = false;
	cabinP->values.performance.evaChecked = false;
	cabinP->values.performance.allChecked = false;
	cabinP->values.performance.Fail = false;
	cabinP->values.performance.airTempAtStart =
		cabinP->components.airTempSensor.valueInCExt;

}

void cabinPerformanceCommercialTestTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	stCabin_Flash const *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	unsigned short testTimer = params->rf->workMode.values.CommercialTest.componentTestTimer100Ms;
        
	cabinCloseAllComponentsCommercial(cabinP);
        
	switch (cabinP->sets.user.Mode)
	{
	case _CabinMode_FF:
		break;
	case _CabinMode_FRZ:
		break;
	default:
		break;
	}  
        
	if (consts->rf->Cooler.components.ambientTempSensor.type == _TempSensorType_NONE)
		cabinUpdateGasLevel(consts, params);

	// CabinSwitchTask( cabinP );
	cabinBlueLightTask(consts, params);
	cabinInnerLightTask(consts, params);

	cabinCheckALLCommercial(cabinC, cabinP, testTimer);
}

void cabinCheckALLCommercial(stCabin_Flash const *cabinC,
				   stCabin_Ram *cabinP,
				   unsigned short testTimer)
{
	cabinCheckAirSensorCommercial(cabinC, cabinP, testTimer);
	cabinCheckEvaSensorCommercial(cabinC, cabinP, testTimer);

	if (cabinP->values.performance.airChecked &&
		cabinP->values.performance.evaChecked)
		cabinP->values.performance.allChecked = true;
	else
		cabinP->values.performance.allChecked = false;
}

void cabinCheckAirSensorCommercial(stCabin_Flash const *cabinC,
						 stCabin_Ram *cabinP,
						 unsigned short testTimer)
{
	if ((cabinC->type == _CabinType_NONE) &&
		!cabinC->components.airTempSensor.type)
		cabinP->values.performance.airChecked = true;

	if (!cabinP->values.performance.Fail &&
		!cabinP->values.performance.airChecked)
	{
		if (testTimer == cabinC->parameters.Performance.timeAir)
		{
			if ((cabinP->values.performance.airTempAtStart -
				 cabinP->components.airTempSensor.valueInCExt) <
				cabinC->parameters.Performance.deltaTempAir)
			{
				cabinP->values.performance.Fail = true;
			}
			else
			{
				cabinP->values.performance.airChecked = true;
			}
		}
	}
}

void cabinCheckEvaSensorCommercial(stCabin_Flash const *cabinC,
						 stCabin_Ram *cabinP,
						 unsigned short testTimer)
{
	if ((cabinC->type == _CabinType_NONE) &&
		!cabinC->components.evaTempSensor.type)
		cabinP->values.performance.evaChecked = true;

	if (!cabinP->values.performance.Fail &&
		!cabinP->values.performance.evaChecked)
	{
		if (testTimer > cabinC->parameters.Performance.timeEva2)
		{
			cabinP->values.performance.evaChecked = true;
		}
		else
		{
			if (testTimer > cabinC->parameters.Performance.timeEva1)
			{
				if ((cabinP->components.evaTempSensor.valueInCExt > cabinC->parameters.Performance.tempEva1) ||
					(cabinP->components.evaTempSensor.valueInCExt < cabinC->parameters.Performance.tempEva2))
				{
					cabinP->values.performance.Fail = true;
				}
			}
		}
	}
}
/*** APW : Add CT heater software ***/

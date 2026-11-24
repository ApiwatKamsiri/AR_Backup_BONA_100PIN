#include "cabin_production_test.h"
#include "cabin_gas_level.h"

#include "../components/cabin_switch.h"
#include "../components/cabin_inner_light.h"
#include "../components/cabin_blue_light.h"
#include "../components/cabin_handle_light.h"

#include "app/libs/temp_funcs.h"
#include "app/libs/component_test.h"

void cabinCheckAirSensor(stCabin_Flash const *cabinC,
						 stCabin_Ram *cabinP,
						 unsigned short testTimer);

void cabinCheckEvaSensor(stCabin_Flash const *cabinC,
						 stCabin_Ram *cabinP,
						 unsigned short testTimer);

void cabinCheckALL(stCabin_Flash const *cabinC,
				   stCabin_Ram *cabinP,
				   unsigned short testTimer);

void cabinCloseAllComponents(stCabin_Ram *cabinP)
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
        cabinP->components.handleLight.R_value = 0;
        cabinP->components.handleLight.G_value = 0;
        cabinP->components.handleLight.B_value = 0;
}

void cabinTestsFailedTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;

	cabinCloseAllComponents(cabinP);
	// CabinSwitchTask( cabinP );
	cabinBlueLightTask(consts, params);
	cabinInnerLightTask(consts, params);
        cabinHandleLightTask(consts, params);

	cabinP->values.gasDesired.type = _GT_Failed;
	cabinP->values.gasDesired.level = 0;
}

void cabinPerformanceTestInit(stCabin_Flash const *cabinC,
							  stCabin_Ram *cabinP)
{
	if (cabinC->type)
		cabinP->states.Work = _Work_HotStart;

	cabinP->values.performance.airChecked = false;
	cabinP->values.performance.evaChecked = false;
	cabinP->values.performance.allChecked = false;
	cabinP->values.performance.Fail = false;
	cabinP->values.performance.airTempAtStart =
		cabinP->components.airTempSensor.valueInCExt;
}

void isProductionTestCondition(stCabin_Flash const *cabinC,
                              stCabin_Ram *cabinP)
{
  if (cabinC->type)
  {
    if(cabinC->components.airTempSensor.type)
    {
      if(!cabinP->components.airTempSensor.error && cabinP->components.airTempSensor.valueInCExt < 390)
        cabinP->states.Work = _Work_PerformanceTest;   
    }
  }
}

void cabinPerformanceTestTask(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	stCabin_Flash const *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	unsigned short testTimer = params->rf->workMode.values.prodTest.componentTestTimer100Ms;

	cabinCloseAllComponents(cabinP);

	if ((cabinC->components.fan.type) &&
		(cabinP->values.doorClosedTimer > _Fan_ONDelayTime))
		cabinP->components.fan.speedOrPWM = 100;

	if (cabinC->components.flap.type)
		cabinP->components.flap.value = 1800;

	switch (cabinP->sets.user.Mode)
	{
	case _CabinMode_FF:
		cabinP->values.gasDesired.type = _GT_Cold_FF;
		break;

	case _CabinMode_FRZ:
		cabinP->values.gasDesired.type = _GT_Cold_FRZ;
		break;

	default:
		cabinP->values.gasDesired.type = _GT_Cold;
		break;
	}
	if (consts->rf->Cooler.components.ambientTempSensor.type == _TempSensorType_NONE)
		cabinUpdateGasLevel(consts, params);

	// CabinSwitchTask( cabinP );
	cabinBlueLightTask(consts, params);
	cabinInnerLightTask(consts, params);
        cabinHandleLightTask(consts, params);
        
	cabinCheckALL(cabinC, cabinP, testTimer);
}

void cabinCheckALL(stCabin_Flash const *cabinC,
				   stCabin_Ram *cabinP,
				   unsigned short testTimer)
{
	cabinCheckAirSensor(cabinC, cabinP, testTimer);
	cabinCheckEvaSensor(cabinC, cabinP, testTimer);

	if (cabinP->values.performance.airChecked &&
		cabinP->values.performance.evaChecked)
		cabinP->values.performance.allChecked = true;
	else
		cabinP->values.performance.allChecked = false;
}

void cabinCheckAirSensor(stCabin_Flash const *cabinC,
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

void cabinCheckEvaSensor(stCabin_Flash const *cabinC,
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

void cabinComponentsTest(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	stCabin_Flash const *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	tsCabinProcDevIos *devs = &params->dev;
	const stCabinComponents_Flash *compC = &cabinC->components;
	stCabinComponents_Ram *compP = &cabinP->components;

	// component test timer x 100 ms
	uint32_t tt = params->rf->workMode.values.prodTest.componentTestTimer100Ms;

	cabinCloseAllComponents(cabinP);
	cabinSwitchTask(consts, params);

	if (cabinC->type)
	{
		bool done = true;
		
		if (cabinP->values.doorClosedTimer)
		{
			done &= compTest(devs->fan, &compP->fan.speedOrPWM, &compC->fan.test, tt, 100);
			done &= compTest(devs->innerLight, &compP->innerLight.value, &compC->innerLight.test, tt, 100);
			done &= compTest(devs->blueLight, &compP->blueLight.value, &compC->blueLight.test, tt, 100);
			done &= compTest(devs->handleLight_R, &compP->handleLight.R_value, &compC->handleLight.test, tt, 100);
			done &= compTest(devs->handleLight_G, &compP->handleLight.G_value, &compC->handleLight.test, tt, 100);
			done &= compTest(devs->handleLight_B, &compP->handleLight.B_value, &compC->handleLight.test, tt, 100);
		}
		else
		{
			//  Continue fan test when knob is available
			if (cabinC->components.knob.type != _KnobType_NONE)
				compTest(devs->fan, &compP->fan.speedOrPWM, &compC->fan.test, tt, 100);
			done &= compTestDone(devs->fan, &compC->fan.test, tt);
			done &= compTestDone(devs->innerLight, &compC->innerLight.test, tt);
			done &= compTestDone(devs->blueLight, &compC->blueLight.test, tt);
			done &= compTest(devs->handleLight_R, &compP->handleLight.R_value, &compC->handleLight.test, tt, 100);
			done &= compTest(devs->handleLight_G, &compP->handleLight.G_value, &compC->handleLight.test, tt, 100);
			done &= compTest(devs->handleLight_B, &compP->handleLight.B_value, &compC->handleLight.test, tt, 100);

			cabinInnerLightTask(consts, params);
			cabinBlueLightTask(consts, params); // bb
			cabinHandleLightTask(consts, params);
		}
		
		if (params->rf->Values.doorsClosed)
			done &= compTest(devs->heaterA, &compP->heaterA.value, &compC->heaterA.test, tt, _HeaterValue_ON);
		else
			done &= compTestDone(devs->heaterA, &compC->heaterA.test, tt);

		done &= compTest(devs->heaterB, &compP->heaterB.value, &compC->heaterB.test, tt, _HeaterValue_ON);
		done &= compTest(devs->heaterC, &compP->heaterC.value, &compC->heaterC.test, tt, _HeaterValue_ON);
		done &= compTest(devs->ionizer, &compP->ionizer.value, &compC->ionizer.test, tt, _IonizerValue_ON);
		done &= compTest(devs->violed, &compP->violed.value, &compC->violed.test, tt, _VioLedValue_ON);
		
		cabinP->values.componentsTestDone = done;
	}
}


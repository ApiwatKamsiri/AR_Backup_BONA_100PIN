#include "cabin_shared_funcs.h"

void cabinQuickFf(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);

	bool quickFF = false;

	for (uint8_t i = 0; i < _NumOfCabin; i++)
	{
		if ((consts->rf->Cabin[i].type != _CabinType_NONE) &&
			(params->rf->Cabin[i].sets.work.Mode == _CabinMode_FF) &&
			(params->rf->Cabin[i].sets.work.functions.Quick))
		{
			quickFF = true;
			break;
		}
	}
	cabinP->values.quickFF = quickFF; // quickFF;
}
void cabinQuickFrz(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);

	bool quickFRZ = false;

	for (uint8_t i = 0; i < _NumOfCabin; i++)
	{
		if ((consts->rf->Cabin[i].type != _CabinType_NONE) &&
			(params->rf->Cabin[i].sets.work.Mode == _CabinMode_FRZ) &&
			(params->rf->Cabin[i].sets.work.functions.Quick))
		{
			quickFRZ = true;
			break;
		}
	}

	cabinP->values.quickFRZ = quickFRZ; // quickFRZ    if
}

void cabinQuickIce(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
  //Quick Ice cannot cause a quick freeze functions 
	/*const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);*/

	//cabinP->values.quickFRZ = params->rf->IceMachine.values.quickIce;
        /*** APW : Add Ice making operation test in CT ***/
        /*if(params->rf->ui.states.commercial.status == _UI_Commercial_None)
            cabinP->values.quickFRZ = params->rf->IceMachine.values.quickIce;*/
        /*** APW : Add Ice making operation test in CT ***/
}

/*void cabinQuickIceCream(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);

	bool quickIceCream = false;

	if ((consts->rf->IceCreamMachine.type != _IceCreamMachineType_NONE) &&
		(params->rf->IceCreamMachine.values.state == _IceCreamMachineState_MakingIceCream))
		quickIceCream = true;

	if (consts->rf->IceCreamMachine.parameters.relatedCabin == consts->cabinNo)
	{
		if (quickIceCream)
		{
			cabinP->sets.user.functions.Quick = true;
			cabinP->values.quickIceCream = true;
		}
		else
		{
			if (cabinP->values.quickIceCream)
			{
				cabinP->sets.user.functions.Quick = false;
				cabinP->values.quickIceCream = false;
			}
		}
	}
}*/

void cabinAirSupply(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

	unsigned char airSupplier = cabinC->parameters.Fan.AirSupplier;
	if (airSupplier < _NumOfCabin &&
		consts->rf->Cabin[airSupplier].type != _CabinType_NONE &&
		params->rf->Cabin[airSupplier].components.fan.speedOrPWM)
		cabinP->values.airSupply = true;
	else
		cabinP->values.airSupply = false;
}

void cabinCheckFrzDefrost(const tsCabinProcConsts *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	//stCabin_Ram *cabinP = params->cabin;
	UNUSED(cabinC);
	
	bool frzDefrost = false;

	for (uint8_t i = 0; i < _NumOfCabin; i++)
	{
		if ((consts->rf->Cabin[i].type != _CabinType_NONE) &&
			(params->rf->Cabin[i].sets.work.Mode == _CabinMode_FRZ) &&
			(params->rf->Cabin[i].states.Work == _Work_Defrost))
		{
			frzDefrost = true;
			params->rf->Cooler.parameters.frzDefrost = true;
			break;
		}
	}
	
	for (uint8_t i = 0; i < _NumOfCabin; i++)
		params->rf->Cabin[i].values.frzDefrost = frzDefrost;
}
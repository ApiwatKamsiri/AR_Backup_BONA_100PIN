#include "ce_refrigerator.h"
#include "app/refrigerator/procedures/rf_sabbath.h"
#include "app/refrigerator/procedures/rf_silent.h"
#include "app/refrigerator/procedures/rf_eco_extra.h"

static void CabinRice(stCabin_Flash const *cabinC, stCabin_Ram *cabinP, bool Rice);

void ceRfTempUnitChange(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived)
{
	tsUiRam *uiP = params->ui;
	stRefrigerator_Ram *rfP = params->rf;
	stRefrigerator_Flash const *rfC = consts->rf;

	if (!cmdReceived)
		return;

	enTempType prvTempType = rfP->Values.tempType;

	prvTempType++;

	switch (prvTempType)
	{
	case _TempType_Celcius:
	case _TempType_Fahrenheit:
		if (!rfC->Parameters.fahrenheit)
			prvTempType++;
	default:
		break;
	}

	if (prvTempType >= _TempType_END)
		prvTempType = _TempType_Celcius;

	if (rfP->Values.tempType != prvTempType)
	{
		rfP->Values.tempType = prvTempType;
		rfP->E2.tempType = prvTempType;
		sysBuzzerCmd(eSysBuzzerPlayStartMelody);

                /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
                if(rfP->workMode.values.workMode == _WM_CommercialTest){
                   uiP->states.commercial.state = _UI_CommercialState_TempUnit_Init;
                }
                else
                {
                   uiP->states.normal.state = _UI_NormalState_TempUnit_Init;
                }
                /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
                
		uiP->states.distributer.state = _UI_DistributerState_TempUnit_Init;
		// UI_FairModeState = _UI_FairModeState_TempUnit_Init;
	}

	if (rfP->Values.tempType == _TempType_Celcius)
	{
		// uiP->dspPacket.funcs.ledFahr_Cabin1 = true;
		uiP->dspPacket.funcs.ledCelsius = true;
		// uiP->dspPacket.funcs.ledFahr_Cabin0 = false;
		uiP->dspPacket.funcs.ledFahrenheit = false;
	}
	else
	{
		for (unsigned char i = 0; i < _NumOfCabin; i++)
		{
			rfP->Values.temperatureConvPointer[i] = rfP->E2.temperatureConvPointer[i];
		}

		// uiP->dspPacket.funcs.ledFahr_Cabin1 = false;
		uiP->dspPacket.funcs.ledCelsius = false;
		// uiP->dspPacket.funcs.ledFahr_Cabin0 = true;
		uiP->dspPacket.funcs.ledFahrenheit = true;
	}
}

void ceRfSabbath(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived)
{
	if (!cmdReceived)
		return;

	tsUiRam *uiP = params->ui;
	stRefrigerator_Ram *rfP = params->rf;
	stRefrigerator_Flash const *rfC = consts->rf;

	if (rfC->Sets.configs.functions.Sabbath)
	{
                /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
                if(rfP->workMode.values.workMode == _WM_CommercialTest){
                uiP->states.commercial.state = _UI_CommercialState_Sabbath;
                }
                else
                {
		uiP->states.normal.state = _UI_NormalState_Sabbath;
                }
                /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
                
		rfSabbathToggle(rfC, rfP);
		rfSabbathStore(rfC, rfP);
		if (rfP->Sets.user.functions.Sabbath)
			sysBuzzerCmd(eSysBuzzerPlayStartMelody);
		else
			sysBuzzerCmd(eSysBuzzerPlayEndMelody);
	}

	uiResetSetChangeTimer(uiP);
}

void ceRfResetSabbath(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived)
{
	if (!cmdReceived)
		return;

	stRefrigerator_Flash const *rfC = consts->rf;
	stRefrigerator_Ram *rfP = params->rf;

	if (rfC->Sets.configs.functions.Sabbath)
	{
		rfSabbathDisable(rfC, rfP);
		rfSabbathStore(rfC, rfP);
		if (!rfP->Sets.user.functions.Sabbath)
			sysBuzzerCmd(eSysBuzzerPlayEndMelody);
	}

	uiResetSetChangeTimer(params->ui);
}

void ceRfSilent(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived)
{
	if (!cmdReceived)
		return;

	stRefrigerator_Ram *rfP = params->rf;
	stRefrigerator_Flash const *rfC = consts->rf;

	if (rfC->Sets.configs.functions.Silent)
	{
		rfSilentToggle(rfC, rfP);
		rfSilentStore(rfC, rfP);
		if (rfP->Sets.user.functions.Silent)
			sysBuzzerCmd(eSysBuzzerPlayStartMelody);
		else
			sysBuzzerCmd(eSysBuzzerPlayEndMelody);
	}

	uiResetSetChangeTimer(params->ui);
}

void ceRfEcoExtra(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived)
{
	if (!cmdReceived)
		return;

	stRefrigerator_Ram *rfP = params->rf;
	stRefrigerator_Flash const *rfC = consts->rf;

	if (rfC->Sets.configs.functions.EcoExtra &&
		!rfP->Cabin[1].sets.user.functions.Convertible &&
		!rfP->Cabin[1].sets.user.functions.CabinOff)
	{
		rfEcoExtraToggle(rfC, rfP);
		rfEcoExtraStore(rfC, rfP);

		if (rfP->Sets.user.functions.EcoExtra)
			sysBuzzerCmd(eSysBuzzerPlayStartMelody);
		else
			sysBuzzerCmd(eSysBuzzerPlayEndMelody);
	}

	uiResetSetChangeTimer(params->ui);
}

		  
#ifdef HOSHI_H30	  			  
void ceRfTempSettingFine(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived)
{
	if (!cmdReceived)
		return;

	stRefrigerator_Ram *rfP = params->rf;
	//stRefrigerator_Flash const *rfC = consts->rf;

        if((refrigerator_R.ui.states.commercial.status == _UI_Commercial_None && 
              refrigerator_R.Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN && 
                refrigerator_R.Cabin[1].components.doorSwitch[0].value == _SwitchValue_CLOSE) ||
                  refrigerator_R.ui.states.commercial.status != _UI_Commercial_None)
        {
          if (!rfP->Sets.user.functions.TempSettingFine)
          {
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
            rfP->Sets.user.functions.TempSettingFine = true;
            rfP->Cabin[0].sets.user.functions.TempSettingFine = true;
            rfP->Cabin[1].sets.user.functions.TempSettingFine = true;
            rfP->E2.TempSettingFine = rfP->Sets.user.functions.TempSettingFine;
            rfP->Cabin[0].E2.TempSettingFine = rfP->Cabin[0].sets.user.functions.TempSettingFine;
            rfP->Cabin[1].E2.TempSettingFine = rfP->Cabin[1].sets.user.functions.TempSettingFine;
          }
          else
          {
            sysBuzzerCmd(eSysBuzzerPlayEndMelody);
            rfP->Sets.user.functions.TempSettingFine = false;
            rfP->Cabin[0].sets.user.functions.TempSettingFine = false;
            rfP->Cabin[1].sets.user.functions.TempSettingFine = false;
            
            rfP->Cabin[0].sets.user.FF = 0;
            rfP->Cabin[1].sets.user.FRZ = 0;
            rfP->E2.TempSettingFine = rfP->Sets.user.functions.TempSettingFine;
            rfP->Cabin[0].E2.TempSettingFine = rfP->Cabin[0].sets.user.functions.TempSettingFine;
            rfP->Cabin[1].E2.TempSettingFine = rfP->Cabin[1].sets.user.functions.TempSettingFine;
          }
        }
        uiResetSetChangeTimer(params->ui);
}
void ceRfDemoMode(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived)
{
  if (!cmdReceived)
    return;
  
  if((refrigerator_R.ui.states.commercial.status == _UI_Commercial_None && 
        refrigerator_R.Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN && 
          refrigerator_R.Cabin[1].components.doorSwitch[0].value == _SwitchValue_OPEN) ||
            refrigerator_R.ui.states.commercial.status != _UI_Commercial_None)
  {
    if(refrigerator_R.workMode.values.prodTest.minTimer < 5)
    {
      eventPost(eProcessRefrigerator, eSysEventStartDistributerMode, NULL, 0);
      uiResetSetChangeTimer(params->ui);
    }
  }
}
void ceRfMuteBuzzer(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived)
{
	if (!cmdReceived)
		return;
        
        if((refrigerator_R.ui.states.commercial.status == _UI_Commercial_None && 
              refrigerator_R.Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN && 
                refrigerator_R.Cabin[1].components.doorSwitch[0].value == _SwitchValue_CLOSE) ||
                  refrigerator_R.ui.states.commercial.status != _UI_Commercial_None)
        {
          if (User_MuteBuzzer)
          {
            sysBuzzerCmd(eSysBuzzerPlayEndMelody);
            User_MuteBuzzer = 0;
          }
          else
          {
            User_MuteBuzzer = 1;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
          }
        }
	uiResetSetChangeTimer(params->ui);
}
#endif //HOSHI_H30   
void ceRfVacation(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived)
{
	if (!cmdReceived)
		return;

	stRefrigerator_Ram *rfP = params->rf;
	stRefrigerator_Flash const *rfC = consts->rf;
	tsUiRam *uiP = params->ui;

	if (rfP->Values.errors.sensorErrorsNumber)
	{
		rfP->Sets.user.functions.Vacation = false;
		sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
	}
	else
	{
		rfP->Sets.user.functions.Vacation =
			!rfP->Sets.user.functions.Vacation;

		if (rfP->Sets.user.functions.Vacation)
			sysBuzzerCmd(eSysBuzzerPlayStartMelody);
		else
			sysBuzzerCmd(eSysBuzzerPlayEndMelody);
	}

	for (unsigned char i = 0; i < _NumOfCabin; i++)
	{
		CabinVacation(consts, params,
					  &rfC->Cabin[i], &rfP->Cabin[i], rfP->Sets.user.functions.Vacation);
	}

	uiResetSetChangeTimer(uiP);
        
        /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
        if(rfP->workMode.values.workMode == _WM_CommercialTest){
          uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
        }
        else
        {
          uiP->states.normal.state = _UI_NormalState_Sets_Init;              
        }
        /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
        
	uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
	// UI_FairModeState = _UI_FairModeState_Sets_Init;
}

void CabinVacation(const tsUiProcConsts *consts, tsUiProcParams *params,
				   stCabin_Flash const *cabinC, stCabin_Ram *cabinP, bool Vacation)
{
	if (cabinC->type != _CabinType_NONE)
	{
		switch (cabinP->sets.user.Mode)
		{
		case _CabinMode_FF:
			if (cabinC->sets.configs.functions.Vacation)
			{
				if (Vacation)
				{
					cabinP->sets.user.functions.Quick = false;
					cabinP->sets.user.functions.Rice = false;
				}
				cabinP->sets.user.functions.Vacation = Vacation;
			}
			break;
		}
	}

	cabinP->E2.Rice = cabinP->sets.user.functions.Rice;
	cabinP->E2.Vacation = cabinP->sets.user.functions.Vacation;

	uiResetSetChangeTimer(params->ui);
}

void ceRfRice(const tsUiProcConsts *consts, tsUiProcParams *params,
			  bool cmdReceived)
{
	if (!cmdReceived)
		return;

	stRefrigerator_Ram *rfP = params->rf;
	const stRefrigerator_Flash *rfC = consts->rf;
	tsUiRam *uiP = params->ui;

	if (rfP->Values.errors.sensorErrorsNumber)
	{
		rfP->Sets.user.functions.Rice = false;
		sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
	}
	else
	{
		rfP->Sets.user.functions.Rice =
			!rfP->Sets.user.functions.Rice;

		if (rfP->Sets.user.functions.Rice)
			sysBuzzerCmd(eSysBuzzerPlayStartMelody);
		else
			sysBuzzerCmd(eSysBuzzerPlayEndMelody);
	}

	for (unsigned char i = 0; i < _NumOfCabin; i++)
	{
		CabinRice(&rfC->Cabin[i],
				  &rfP->Cabin[i],
				  !rfP->Sets.user.functions.Rice);
	}

	uiResetSetChangeTimer(uiP);
        
        /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
        if(rfP->workMode.values.workMode == _WM_CommercialTest){
          uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
        }
        else
        {
          uiP->states.normal.state = _UI_NormalState_Sets_Init;              
        }
        /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
        
	uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
	// UI_FairModeState = _UI_FairModeState_Sets_Init;
}

static void CabinRice(stCabin_Flash const *cabinC, stCabin_Ram *cabinP, bool Rice)
{
	if (cabinC->type != _CabinType_NONE)
	{
		switch (cabinP->sets.user.Mode)
		{
		case _CabinMode_FF:
			if (cabinC->sets.configs.functions.Rice)
			{
				if (Rice)
				{
					cabinP->sets.user.functions.Quick = false;
					cabinP->sets.user.functions.Vacation = false;
				}
				cabinP->sets.user.functions.Rice = Rice;
			}
			break;
		}
	}

	cabinP->E2.Rice = cabinP->sets.user.functions.Rice;
	cabinP->E2.Vacation = cabinP->sets.user.functions.Vacation;
}

void ceRfKnobsCheck(const tsUiProcConsts *consts, tsUiProcParams *params)
{

	stRefrigerator_Ram *rfP = params->rf;
	stRefrigerator_Flash const *rfC = consts->rf;
	tsUiRam *uiP = params->ui;

	const stCabin_Flash *cabinC;
	stCabin_Ram *cabinP;
        
        static uint8_t knobPositionFF = 0;
        static uint8_t knobPositionFRZ = 0;

	for (unsigned char i = 0; i < _NumOfCabin; i++)
	{
		cabinC = &rfC->Cabin[i];
		cabinP = &rfP->Cabin[i];

		if ((cabinC->type != _CabinType_NONE) &&
			(cabinC->components.knob.type != _KnobType_NONE))
		{
			cabinP->sets.user.functions.Rice = false;

			if (cabinP->sets.user.Mode == _CabinMode_FF)
			{
                          if(knobPositionFF!=cabinP->components.knob.position)
                          {
                            knobPositionFF = cabinP->components.knob.position;
                            cabinP->components.knob.positionChanged = true;
                          }
				cabinP->sets.user.FF = cabinP->components.knob.position;
			}
			else if (cabinP->sets.user.Mode == _CabinMode_FRZ)
			{
                          if(knobPositionFRZ!=cabinP->components.knob.position)
                          {
                            knobPositionFRZ = cabinP->components.knob.position;
                            cabinP->components.knob.positionChanged = true;
                          }
				cabinP->sets.user.FRZ = cabinP->components.knob.position;
			}
			else
			{
				cabinP->sets.user.FF = cabinP->components.knob.position;
				cabinP->sets.user.FRZ = cabinP->components.knob.position;
			}

			switch (rfP->Cabin[i].components.knob.position)
			{
			case 0:
				// Sabbath
				if (cabinP->components.knob.positionChanged)
				{
					if (rfC->Sets.configs.functions.Sabbath)
					{
						if (!rfP->Sets.user.functions.Sabbath)
						{
							rfSabbathEnable(rfC, rfP);
							rfSabbathStore(rfC, rfP);
						}
					}
				}
				// Vacation
				if (cabinP->sets.user.Mode == _CabinMode_FF)
				{
					cabinP->sets.user.functions.Vacation =
						cabinC->sets.configs.functions.Vacation;
				}
				// Quick
				cabinP->sets.user.functions.Quick = false;
				// Buzzer
				if (cabinP->components.knob.positionChanged)
				{
					if (rfP->Sets.user.functions.Sabbath ||
						cabinC->sets.configs.functions.Vacation)
						sysBuzzerCmd(eSysBuzzerPlayStartMelody);
					else
					   sysBuzzerCmd( eSysBuzzerPlayButtonMelody );
				}
				break;
			case 5:
				// Sabbath
				if (rfP->Sets.user.functions.Sabbath)
				{
					rfSabbathDisable(rfC, rfP);
					rfSabbathStore(rfC, rfP);
				}
				// Vacation
				cabinP->sets.user.functions.Vacation = false;
				// Quick
				if (cabinP->components.knob.positionChanged)
				{
					if (cabinP->sets.user.Mode == _CabinMode_FF)
						cabinP->sets.user.functions.Quick = cabinC->sets.configs.functions.QuickFF;
					else if (cabinP->sets.user.Mode == _CabinMode_FRZ)
						cabinP->sets.user.functions.Quick = cabinC->sets.configs.functions.QuickFRZ;
					else
						cabinP->sets.user.functions.Quick = false;
				}
				// Buzzer
				if (cabinP->components.knob.positionChanged)
				{
					if (cabinP->sets.user.functions.Quick)
						sysBuzzerCmd(eSysBuzzerPlayStartMelody);
					else
					   sysBuzzerCmd( eSysBuzzerPlayButtonMelody );
				}
				break;
			default:
				// Buzzer
				if (cabinP->components.knob.positionChanged)
				{
					if (rfP->Sets.user.functions.Sabbath ||
						cabinP->sets.user.functions.Vacation ||
						cabinP->sets.user.functions.Quick)
						sysBuzzerCmd(eSysBuzzerPlayEndMelody);
					else
					   sysBuzzerCmd( eSysBuzzerPlayButtonMelody );
				}
				// Sabbath
				if (rfP->Sets.user.functions.Sabbath)
				{
					rfSabbathDisable(rfC, rfP);
					rfSabbathStore(rfC, rfP);
				}
				// Vacation
				cabinP->sets.user.functions.Vacation = false;
				// Quick
				cabinP->sets.user.functions.Quick = false;
				break;
			}

			if (cabinP->components.knob.positionChanged)
			{
				uiResetSetChangeTimer(uiP);
                                /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
                                if(rfP->workMode.values.workMode == _WM_CommercialTest){
                                  uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
                                }
                                else
                                {
                                  uiP->states.normal.state = _UI_NormalState_Sets_Init;
                                }
                                /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
				uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
				// UI_FairModeState = _UI_FairModeState_Sets_Init;
			}

			cabinP->components.knob.positionChanged = false;
			cabinP->E2.FF = cabinP->sets.user.FF;
			cabinP->E2.FRZ = cabinP->sets.user.FRZ;
			cabinP->E2.Rice = cabinP->sets.user.functions.Rice;
			cabinP->E2.Vacation = cabinP->sets.user.functions.Vacation;
		}
	}
}

void ceRfApplianceOff(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived)
{
	if (!cmdReceived)
		return;

	stRefrigerator_Ram *rfP = params->rf;
	tsUiRam *uiP = params->ui;

	if (rfP->workMode.values.workMode == _WM_ApplianceOff)
	{
		uiP->dspPacket.funcs.ledApplianceOff = false;
		// CHECKPOINT
		eventPost(eProcessRefrigerator, eSysEventReinit, NULL, 0);
		eventPost(eProcessRefrigerator, eSysEventStartProductionTestMode, NULL, 0);
	}
	else
	{
		uiP->dspPacket.funcs.ledApplianceOff = true;
		eventPost(eProcessRefrigerator, eSysEventStartApplianceOffMode, NULL, 0);
	}
}

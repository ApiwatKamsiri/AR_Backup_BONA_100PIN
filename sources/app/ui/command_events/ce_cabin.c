#include "system.h"
#include "ce_cabin.h"
#include "app/libs/temp_funcs.h"
#include "app/refrigerator/procedures/rf_eco_extra.h"
#include "../components/ui_display.h"
#include "app/cabin/cabin.h"  // For AI_COOL_V1 function prototypes
#include "app/refrigerator/refrigerator.h"

static void resetHighTempParameters(stRefrigerator_Ram *rfP, stCabin_Ram *cabinP);

void ceHandleLight(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                        bool cmdReceived)
{
  if(!cmdReceived)
    return;
  
  if(!cabinC->components.handleLight.type)
    return;
      
    params->rf->Values.handleLightState = !params->rf->Values.handleLightState;  
    params->rf->E2.handleLightState = params->rf->Values.handleLightState;
  
  if(params->rf->Values.handleLightState)
    sysBuzzerCmd(eSysBuzzerPlayStartMelody);
  else
    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
}

void ceHandleLightOnOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                        bool cmdReceived)
{
  if(!cmdReceived)
    return;
  
  if(!cabinC->components.handleLight.type)
    return;

    if(cabinP->E2.selectedColorIndex)
      cabinP->E2.selectedColorIndex = 0;
    else
      cabinP->E2.selectedColorIndex = 1;
    
    cabinP->values.selectedColorIndex = cabinP->E2.selectedColorIndex;
  
    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
} 
void ceHandleLightSet(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                        bool cmdReceived)
{
  if(!cmdReceived)
    return;
  
  if(!cabinC->components.handleLight.type)
    return;
  
  if(!cabinP->E2.selectedColorIndex)
    return;

//      refrigerator_R.Cabin[0].values.selectedColorIndex = rcCommand;
//      refrigerator_R.Cabin[0].E2.selectedColorIndex = rcCommand;
//      refrigerator_R.Cabin[1].values.selectedColorIndex = rcCommand;
//      refrigerator_R.Cabin[1].E2.selectedColorIndex = rcCommand;
    if(cabinP->E2.selectedColorIndex >= 11)
      cabinP->E2.selectedColorIndex = 0;
    params->ui->dspPacket.funcs.ledWifi = false;
    cabinP->values.selectedColorIndex = cabinP->E2.selectedColorIndex + 1;
    cabinP->E2.selectedColorIndex = cabinP->values.selectedColorIndex;
    cabinP->values.selectedColorIndexSetTime = 25;
  
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
} 
void ceVacuumBox(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                        bool cmdReceived)
{
  
    if (!cmdReceived)
      return;
    
    if (cabinC->components.vacuumBox.type) 
    {
      if(cabinP->values.VacuumBox.sets < _VacuumSet_One)
        cabinP->values.VacuumBox.sets++;
      else
        cabinP->values.VacuumBox.sets = _VacuumSet_Off;     
      
      sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
      
    }
    cabinP->E2.VacuumSet = cabinP->values.VacuumBox.sets;
    uiResetSetChangeTimer(params->ui);
    
}


void ceCabinConvertible(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                        bool cmdReceived)
{
    if (!cmdReceived)
        return;

    tsUiRam *uiP = params->ui;

    if (cabinP->sets.user.functions.CabinOff)
        ceCabinFRZOff(consts, params, &consts->rf->Cabin[1], &params->rf->Cabin[1], true); // return;

    if (cabinC->sets.configs.functions.Convertible)
    {

        cabinP->sets.user.functions.Convertible = !cabinP->sets.user.functions.Convertible;

        params->rf->E2.Convertible = cabinP->sets.user.functions.Convertible;

        if (cabinP->sets.user.functions.Convertible)
        {
            cabinP->sets.user.functions.Quick = false;
            // uiDspCabinData(consts, params,  eDisplayEventALL_Converter, 1, eDisplayCabinDataTypeSpecial );
            resetHighTempParameters(params->rf, cabinP);
            params->rf->E2.EcoExtra = false;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
        }
        else
        {
            params->rf->Values.doorsClosedTimer = consts->rf->Parameters.doorsClosedTime;
            resetHighTempParameters(params->rf, cabinP);
            sysBuzzerCmd(eSysBuzzerPlayEndMelody);
        }

        rfEcoExtraDisable(consts->rf, params->rf);
    }
    resetTemperaturePointerF(consts->rf, params->rf, 1);
    params->rf->E2.temperatureConvPointer[1] = params->rf->Values.temperatureConvPointer[1];
    uiResetSetChangeTimer(uiP);
    
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    if(params->rf->workMode.values.workMode == _WM_CommercialTest){
       uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
    }
    else
    {
       uiP->states.normal.state = _UI_NormalState_Sets_Init;
    }
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    
    uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
}

void ceCabinFRZOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                   stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                   bool cmdReceived)
{
    if (!cmdReceived)
        return;

    tsUiRam *uiP = params->ui;

    if (cabinP->sets.user.functions.Convertible)
        ceCabinConvertible(consts, params, &consts->rf->Cabin[1], &params->rf->Cabin[1], true); // return;

    if (cabinC->type != _CabinType_NONE)
    {
        switch (cabinP->sets.user.Mode)
        {
        case _CabinMode_FRZ:

            if (cabinC->sets.configs.functions.CabinOff)
            {
                cabinP->sets.user.functions.CabinOff = !cabinP->sets.user.functions.CabinOff;
                params->rf->E2.CabinOff = cabinP->sets.user.functions.CabinOff;

                if (cabinP->sets.user.functions.CabinOff)
                {
                    cabinP->sets.user.functions.Quick = false;
                    uiDspCabinData(consts, params, eDisplaySpecialsMidLine, 1, eDisplayCabinDataTypeSpecial);
                    resetHighTempParameters(params->rf, cabinP);
                    params->rf->E2.EcoExtra = false;
                    sysBuzzerCmd(eSysBuzzerPlayStartMelody);
                }
                else
                {
                    resetHighTempParameters(params->rf, cabinP);
                    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
                }

                rfEcoExtraDisable(consts->rf, params->rf);
            }
            break;
        }
    }

    uiResetSetChangeTimer(uiP);
    
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    if(params->rf->workMode.values.workMode == _WM_CommercialTest){
       uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
    }
    else
    {
       uiP->states.normal.state = _UI_NormalState_Sets_Init;
    }
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    
    uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
}

void ceCabinChangeFRZMode(const tsUiProcConsts *consts, tsUiProcParams *params,
                          stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP,
                          bool cmdReceived)
{

    if (!cmdReceived)
        return;

    if (params->rf->Cabin[1].sets.user.functions.Convertible)                                   // ff
        ceCabinFRZOff(consts, params, &consts->rf->Cabin[1], &params->rf->Cabin[1], true);      // change to off
    else if (params->rf->Cabin[1].sets.user.functions.CabinOff)                                 // off
        ceCabinFRZOff(consts, params, &consts->rf->Cabin[1], &params->rf->Cabin[1], true);      // change to frz
    else                                                                                        // frz
        ceCabinConvertible(consts, params, &consts->rf->Cabin[1], &params->rf->Cabin[1], true); // chanfe to ff
}

void ceCabinSetDown(const tsUiProcConsts *consts, tsUiProcParams *params,
                    stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                    bool cmdReceived)
{
    if (!cmdReceived)
        return;

    tsUiRam *uiP = params->ui;

    bool FFsetCanChange = false;
    bool FRZsetCanChange = false;

    FFsetCanChange = checkSetValueStatus(params->rf, 0);
    FRZsetCanChange = checkSetValueStatus(params->rf, 1);

    if (cabinC->type != _CabinType_NONE)
    {
        switch (cabinP->sets.user.Mode)
        {
        case _CabinMode_FF:
            if (cabinP->sets.user.functions.Quick ||
                cabinP->sets.user.functions.Vacation ||
                cabinP->sets.user.functions.Rice)
            {
                cabinP->sets.user.functions.Quick = false;
                cabinP->sets.user.functions.Vacation = false;
                cabinP->sets.user.functions.Rice = false;
            }
            else
            {
                params->rf->Values.temperatureConvPointer[0]++;
                if (FFsetCanChange)
                {
#ifdef HOSHI_H30
                  if(!cabinP->sets.user.functions.TempSettingFine)
                  {
#ifdef TempSetting4Lv
                    switch(cabinP->sets.user.FF)
                    {
                    case 0:
                      cabinP->sets.user.FF = 5;
                      break;
                    case 2:
                      cabinP->sets.user.FF = 0;
                      break;
                    case 4:
                      cabinP->sets.user.FF = 2;
                      break;
                    case 5:
                      cabinP->sets.user.FF = 4;
                      break;  
                    default:
                      cabinP->sets.user.FF = 0;
                      break;
                    }
#else                    
                    switch(cabinP->sets.user.FF)
                    {
                    case 0:
                      cabinP->sets.user.FF = 5;
                      break;
                    case 4:
                      cabinP->sets.user.FF = 0;
                      break;
                    case 5:
                      cabinP->sets.user.FF = 4;
                      break;
                    default:
                      cabinP->sets.user.FF = 0;
                      break;
                    }
#endif //TempSetting4Lv          
                  }
                  else
                  {  
                    cabinP->sets.user.FF--;
                    if (cabinP->sets.user.FF >= cabinC->sets.configs.FF)
                    {
                      cabinP->sets.user.FF = cabinC->sets.configs.FF - 1;
                    }
                  }
#else
                    cabinP->sets.user.FF--;
                    if (cabinP->sets.user.FF >= cabinC->sets.configs.FF)
                    {
                        cabinP->sets.user.FF = cabinC->sets.configs.FF - 1;
                    }
#endif //HOSHI_H30
                    resetTemperaturePointerF(consts->rf, params->rf, 0);
                }
            }
            break;

        case _CabinMode_FRZ:
            if (!cabinP->values.quickIceCream)
            {
                if (cabinP->sets.user.functions.Quick ||
                    cabinP->sets.user.functions.Vacation ||
                    cabinP->sets.user.functions.Rice)
                {
                    cabinP->sets.user.functions.Quick = false;
                    cabinP->sets.user.functions.Vacation = false;
                    cabinP->sets.user.functions.Rice = false;
                }
                else if (cabinP->sets.user.functions.Convertible /*||
                         cabinP->sets.user.functions.CabinOff*/
                )
                {
                    params->rf->Values.temperatureConvPointer[1]++;
                    if (FRZsetCanChange)
                    {
                        cabinP->sets.user.Convertible--;
                        if (cabinP->sets.user.Convertible >= cabinC->sets.configs.functions.Convertible)
                        {
                            cabinP->sets.user.Convertible = cabinC->sets.configs.functions.Convertible - 1;
                        }
                        resetTemperaturePointerF(consts->rf, params->rf, 1);
                    }
                }
                else if (cabinP->sets.user.functions.CabinOff)
                {
                }
                else
                {
                    params->rf->Values.temperatureConvPointer[1]++;
                    if (FRZsetCanChange)
                    {
#ifdef HOSHI_H30                    	
                      if(!cabinP->sets.user.functions.TempSettingFine)
                      {
                        
#ifdef TempSetting4Lv
                        switch(cabinP->sets.user.FRZ)
                        {
                        case 0:
                          cabinP->sets.user.FRZ = 5;
                          break;
                        case 2:
                          cabinP->sets.user.FRZ = 0;
                          break;
                        case 4:
                          cabinP->sets.user.FRZ = 2;
                          break;
                        case 5:
                          cabinP->sets.user.FRZ = 4;
                          break;  
                        default:
                          cabinP->sets.user.FRZ = 0;
                          break;
                        }
#else                    
                        switch(cabinP->sets.user.FRZ)
                        {
                        case 0:
                          cabinP->sets.user.FRZ = 5;
                          break;
                        case 4:
                          cabinP->sets.user.FRZ = 0;
                          break;
                        case 5:
                          cabinP->sets.user.FRZ = 4;
                          break;
                        default:
                          cabinP->sets.user.FRZ = 0;
                          break;
                        }
#endif //TempSetting4Lv     
                      }
                      else
                      {     
                        cabinP->sets.user.FRZ--;
                        if (cabinP->sets.user.FRZ >= cabinC->sets.configs.FRZ)
                        {
                          cabinP->sets.user.FRZ = cabinC->sets.configs.FRZ - 1;
                        }   
                      }
#else
                        cabinP->sets.user.FRZ--;
                        if (cabinP->sets.user.FRZ >= cabinC->sets.configs.FRZ)
                        {
                            cabinP->sets.user.FRZ = cabinC->sets.configs.FRZ - 1;
                        }
#endif //HOSHI_H30
                        resetTemperaturePointerF(consts->rf, params->rf, 1);
                    }
                }
            }
            break;
        }
    }
  
    cabinP->E2.FRZ = cabinP->sets.user.FRZ;
    cabinP->E2.FF = cabinP->sets.user.FF;
    cabinP->E2.Convertible = cabinP->sets.user.Convertible;
    params->rf->E2.temperatureConvPointer[0] = params->rf->Values.temperatureConvPointer[0];
    params->rf->E2.temperatureConvPointer[1] = params->rf->Values.temperatureConvPointer[1];
    cabinP->E2.Rice = cabinP->sets.user.functions.Rice;
    cabinP->E2.Vacation = cabinP->sets.user.functions.Vacation;

    uiResetSetChangeTimer(uiP);
    
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    if(params->rf->workMode.values.workMode == _WM_CommercialTest){
       uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
    }
    else
    {
       uiP->states.normal.state = _UI_NormalState_Sets_Init;
    }
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    
    uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
    // UI_FairModeState = _UI_FairModeState_Sets_Init;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
}

void ceCabinSetUp(const tsUiProcConsts *consts, tsUiProcParams *params,
                  stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                  bool cmdReceived)
{

    if (!cmdReceived)
        return;
    bool FFsetCanChange = false;
    bool FRZsetCanChange = false;

    tsUiRam *uiP = params->ui;

    FFsetCanChange = checkSetValueStatusUp(params->rf, 0);
    FRZsetCanChange = checkSetValueStatusUp(params->rf, 1);

    if (cabinC->type != _CabinType_NONE)
    {
        switch (cabinP->sets.user.Mode)
        {
        case _CabinMode_FF:
            if (cabinP->sets.user.functions.Quick ||
                cabinP->sets.user.functions.Vacation ||
                cabinP->sets.user.functions.Rice)
            {
                cabinP->sets.user.functions.Quick = false;
                cabinP->sets.user.functions.Vacation = false;
                cabinP->sets.user.functions.Rice = false;
            }
            else
            {
                params->rf->Values.temperatureConvPointer[0]--;
                if (FFsetCanChange)
                {
                    cabinP->sets.user.FF++;
                    if (cabinP->sets.user.FF >= cabinC->sets.configs.FF)
                    {
                        cabinP->sets.user.FF = 0;
                    }
                    resetTemperaturePointerFup(consts->rf, params->rf, 0);
                }
            }
            break;
        case _CabinMode_FRZ:
            if (!cabinP->values.quickIceCream)
            {
                if (cabinP->sets.user.functions.Quick ||
                    cabinP->sets.user.functions.Vacation ||
                    cabinP->sets.user.functions.Rice)
                {
                    cabinP->sets.user.functions.Quick = false;
                    cabinP->sets.user.functions.Vacation = false;
                    cabinP->sets.user.functions.Rice = false;
                }
                else
                {
                    params->rf->Values.temperatureConvPointer[1]--;
                    if (FRZsetCanChange)
                    {
                        cabinP->sets.user.FRZ++;
                        if (cabinP->sets.user.FRZ >= cabinC->sets.configs.FRZ)
                        {
                            cabinP->sets.user.FRZ = 0;
                        }
                        resetTemperaturePointerFup(consts->rf, params->rf, 1);
                    }
                }
            }
            break;
        }
    }
    
    cabinP->E2.FRZ = cabinP->sets.user.FRZ;
    cabinP->E2.FF = cabinP->sets.user.FF;
    params->rf->E2.temperatureConvPointer[0] = params->rf->Values.temperatureConvPointer[0];
    params->rf->E2.temperatureConvPointer[1] = params->rf->Values.temperatureConvPointer[1];
    cabinP->E2.Rice = cabinP->sets.user.functions.Rice;
    cabinP->E2.Vacation = cabinP->sets.user.functions.Vacation;

    uiResetSetChangeTimer(uiP);
    
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    if(params->rf->workMode.values.workMode == _WM_CommercialTest){
       uiP->states.commercial.state = _UI_CommercialState_Sets_Init;
    }
    else
    {
       uiP->states.normal.state = _UI_NormalState_Sets_Init;
    }
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    
    uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
    // UI_FairModeState = _UI_FairModeState_Sets_Init;
    sysBuzzerCmd(eSysBuzzerPlayButtonMelody); // SET degeri degistiginde ses gelir.
}

void ceCabinMode(const tsUiProcConsts *consts, tsUiProcParams *params,
                 stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                 bool cmdReceived)
{
    if (!cmdReceived)
        return;

    tsUiRam *uiP = params->ui;

    enCabinModes Mode;

    switch (cabinC->type)
    {
    case _CabinType_JKR:
    case _CabinType_FF:
        Mode = cabinP->sets.user.Mode;
        switch (Mode)
        {
        case _CabinMode_FF:
            Mode = _CabinMode_FRZ;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
            break;
        case _CabinMode_FRZ:
            Mode = _CabinMode_FF;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
            break;
        }

        cabinP->sets.user.Mode = Mode;
        cabinP->sets.user.functions.Quick = false;
        cabinP->sets.user.functions.Vacation = false;
        cabinP->sets.user.functions.Rice = false;
        break;
    case _CabinType_FRZ:
        Mode = cabinP->sets.user.Mode;
        switch (Mode)
        {
        case _CabinMode_FF:
            Mode = _CabinMode_FRZ;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
            break;
        case _CabinMode_FRZ:
            Mode = _CabinMode_FF;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
            break;
        }

        cabinP->sets.user.Mode = Mode;
        cabinP->sets.user.functions.Quick = false;
        cabinP->sets.user.functions.Vacation = false;
        cabinP->sets.user.functions.Rice = false;
        break;
    case _CabinType_FLAP:
        Mode = cabinP->sets.user.Mode;
        switch (Mode)
        {
        case _CabinMode_FF:
            Mode = _CabinMode_FRZ;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
            break;
        case _CabinMode_FRZ:
            Mode = _CabinMode_FF;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
            break;
        }

        cabinP->sets.user.Mode = Mode;
        cabinP->sets.user.functions.Quick = false;
        cabinP->sets.user.functions.Vacation = false;
        cabinP->sets.user.functions.Rice = false;
        break;
    }

    cabinP->E2.Mode = cabinP->sets.user.Mode;
    cabinP->E2.Rice = cabinP->sets.user.functions.Rice;
    cabinP->E2.Vacation = cabinP->sets.user.functions.Vacation;

    uiResetSetChangeTimer(uiP);
    
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    if(params->rf->workMode.values.workMode == _WM_CommercialTest){
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

void ceCabinQuick(const tsUiProcConsts *consts, tsUiProcParams *params,
                  stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                  bool cmdReceived)
{
    if (!cmdReceived)
        return;

    tsUiRam *uiP = params->ui;
    enCabinModes Mode;

    switch (cabinC->type)
    {
    case _CabinType_FLAP:
    case _CabinType_FF:
        if (cabinC->sets.configs.functions.QuickFF)
        {
            if (cabinP->sets.user.functions.Vacation ||
                cabinP->sets.user.functions.Rice)
            {
                cabinP->sets.user.functions.Vacation = false;
                cabinP->sets.user.functions.Rice = false;
                sysBuzzerCmd(eSysBuzzerPlayEndMelody);
            }
            else
            {
#ifdef HOSHI_H30
              if((refrigerator_R.ui.states.commercial.status == _UI_Commercial_None && 
                    refrigerator_R.Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN && 
                      refrigerator_R.Cabin[1].components.doorSwitch[0].value == _SwitchValue_CLOSE) ||
                        refrigerator_R.ui.states.commercial.status != _UI_Commercial_None)
#endif
              {
                cabinP->sets.user.functions.Quick =
                    !cabinP->sets.user.functions.Quick;
                if (cabinP->sets.user.functions.Quick)
                {
                    sysBuzzerCmd(eSysBuzzerPlayStartMelody);
                }
                else
                {
                    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
                }
              }
            }
        }
        break;
    case _CabinType_FRZ:
        if (cabinC->sets.configs.functions.QuickFRZ &&
            !cabinP->values.quickIceCream)
        {
            if (cabinP->sets.user.functions.Vacation ||
                cabinP->sets.user.functions.Rice ||
                cabinP->sets.user.functions.Convertible ||
                cabinP->sets.user.functions.CabinOff)
            {
                cabinP->sets.user.functions.Vacation = false;
                cabinP->sets.user.functions.Rice = false;

                if (!cabinP->sets.user.functions.Convertible &&
                    !cabinP->sets.user.functions.CabinOff)
                    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
            }
            else
            {
#ifdef HOSHI_H30
              if((refrigerator_R.ui.states.commercial.status == _UI_Commercial_None && 
                    refrigerator_R.Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN && 
                      refrigerator_R.Cabin[1].components.doorSwitch[0].value == _SwitchValue_CLOSE) ||
                        refrigerator_R.ui.states.commercial.status != _UI_Commercial_None)
#endif
              {

                cabinP->sets.user.functions.Quick =
                    !cabinP->sets.user.functions.Quick;
                if (cabinP->sets.user.functions.Quick)
                    sysBuzzerCmd(eSysBuzzerPlayStartMelody);
                else
                    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
              }
            }
        }
        break;

    case _CabinType_JKR:
        Mode = cabinP->sets.user.Mode;
        switch (Mode)
        {
        case _CabinMode_FF:
            if (cabinC->sets.configs.functions.QuickFF)
            {
                if (cabinP->sets.user.functions.Vacation ||
                    cabinP->sets.user.functions.Rice)
                {
                    cabinP->sets.user.functions.Vacation = false;
                    cabinP->sets.user.functions.Rice = false;
                    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
                }
                else
                {
#ifdef HOSHI_H30
                  if((refrigerator_R.ui.states.commercial.status == _UI_Commercial_None && 
                        refrigerator_R.Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN && 
                          refrigerator_R.Cabin[1].components.doorSwitch[0].value == _SwitchValue_CLOSE) ||
                            refrigerator_R.ui.states.commercial.status != _UI_Commercial_None)
#endif
                  {
                    cabinP->sets.user.functions.Quick =
                        !cabinP->sets.user.functions.Quick;
                    if (cabinP->sets.user.functions.Quick)
                    {
                        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
                    }
                    else
                    {
                        sysBuzzerCmd(eSysBuzzerPlayEndMelody);
                    }
                  }
                }
            }
            break;
        case _CabinMode_FRZ:
             if (cabinC->sets.configs.functions.QuickFRZ)
            {
                if (cabinP->sets.user.functions.Vacation ||
                    cabinP->sets.user.functions.Rice)
                {
                    cabinP->sets.user.functions.Vacation = false;
                    cabinP->sets.user.functions.Rice = false;
                    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
                }
                else
                {
#ifdef HOSHI_H30
                  if((refrigerator_R.ui.states.commercial.status == _UI_Commercial_None && 
                        refrigerator_R.Cabin[0].components.doorSwitch[0].value == _SwitchValue_OPEN && 
                          refrigerator_R.Cabin[1].components.doorSwitch[0].value == _SwitchValue_CLOSE) ||
                            refrigerator_R.ui.states.commercial.status != _UI_Commercial_None)
#endif
                  {
                    cabinP->sets.user.functions.Quick =
                        !cabinP->sets.user.functions.Quick;
                    if (cabinP->sets.user.functions.Quick)
                    {
                        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
                    }
                    else
                    {
                        sysBuzzerCmd(eSysBuzzerPlayEndMelody);
                    }
                  }
                }
            }
            break;
        }

        
        break;
    }

    cabinP->E2.Rice = cabinP->sets.user.functions.Rice;
    cabinP->E2.Vacation = cabinP->sets.user.functions.Vacation;
    uiResetSetChangeTimer(uiP);
    
    /*** APW : Update command CtmodeState and display in CT and others BUG about display ***/
    if(params->rf->workMode.values.workMode == _WM_CommercialTest){
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

void ceCabinChangeCabin(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP,
                        bool cmdReceived)
{
    if (!cmdReceived)
        return;

    tsUiRam *uiP = params->ui;

    if (uiP->dspValues.dspSelector < DSP_USED_CABINS_COUNT)
        uiP->dspValues.dspSelector++;
    if (rfC->Cabin[uiP->dspValues.dspSelector].type == _CabinType_NONE)
        uiP->dspValues.dspSelector = 0;

    sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    uiResetSetChangeTimer(uiP);
}

/*
void ceCabinKnob( stCabin_Flash const* cabinC,
              stCabin_Ram *cabinP ){

  if( cabinC->components.knob.type == _KnobType_NONE ) return;

  if( cabinC->type != _CabinType_NONE ){
    bool refreshDisplay = false;

    switch( cabinP->sets.user.Mode ){
    case _CabinMode_FF:
      cabinP->sets.user.functions.Rice = false;

      switch( cabinP->components.knob.valueInPosition ){
      case 0:
        cabinP->sets.user.functions.Quick = false;

        if( cabinC->sets.configs.functions.Vacation ){
          if( !cabinP->sets.user.functions.Vacation ){
            cabinP->sets.user.functions.Vacation = true;
            sysBuzzerCmd( eSysBuzzerPlayStartMelody );
            refreshDisplay = true;
          }
        }else
            cabinP->sets.user.functions.Vacation = false;
        break;
      case 5:
        cabinP->sets.user.functions.Vacation = false;

        if( cabinC->sets.configs.functions.QuickFF ){
          if( cabinP->components.knob.positionChanged ){
            cabinP->components.knob.positionChanged = false;
            cabinP->sets.user.functions.Quick = true;
            sysBuzzerCmd( eSysBuzzerPlayStartMelody );
            refreshDisplay = true;
          }
        }else
          cabinP->sets.user.functions.Quick = false;
        break;
      default:
        if( cabinP->sets.user.functions.Quick ||
            cabinP->sets.user.functions.Vacation ){
          sysBuzzerCmd( eSysBuzzerPlayEndMelody );
          cabinP->sets.user.functions.Quick = false;
          cabinP->sets.user.functions.Vacation = false;
          refreshDisplay = true;
        }
        break;
      }

      if( cabinP->sets.user.FF  != cabinP->components.knob.valueInPosition ){
        cabinP->sets.user.FF = cabinP->components.knob.valueInPosition;
        if( DisplayData.MBCommand == eMainBoardCommandNone )
          sysBuzzerCmd( _MBCommands_Buzzer_mButton );
        refreshDisplay = true;
      }
    break;
    case _CabinMode_FRZ:
      cabinP->sets.user.functions.Rice = false;
      cabinP->sets.user.functions.Vacation = false;

      switch( cabinP->components.knob.valueInPosition ){
      case 5:
        if( cabinC->sets.configs.functions.QuickFRZ ){
          if( cabinP->components.knob.positionChanged ){
            cabinP->components.knob.positionChanged = false;
            cabinP->sets.user.functions.Quick = true;
            sysBuzzerCmd( eSysBuzzerPlayStartMelody );
            refreshDisplay = true;
          }
        }else
          cabinP->sets.user.functions.Quick = false;
        break;
      default:
        if( cabinP->sets.user.functions.Quick ){
          sysBuzzerCmd( eSysBuzzerPlayEndMelody );
          cabinP->sets.user.functions.Quick = false;
          refreshDisplay = true;
        }
        break;
      }

      if( cabinP->sets.user.FRZ  != cabinP->components.knob.valueInPosition ){
        cabinP->sets.user.FRZ = cabinP->components.knob.valueInPosition;
        if( DisplayData.MBCommand == eMainBoardCommandNone )
          sysBuzzerCmd( _MBCommands_Buzzer_mButton );
        refreshDisplay = true;
      }
      break;
    }

    if( refreshDisplay ){
      uiResetSetChangeTimer(params->ui);
      uiP->states.normal.state = _UI_NormalState_Sets_Init;
      uiP->states.distributer.state = _UI_DistributerState_Sets_Init;
    }

    cabinP->E2.FF = cabinP->sets.user.FF;
    cabinP->E2.FRZ = cabinP->sets.user.FRZ;
    cabinP->E2.Rice = cabinP->sets.user.functions.Rice;
    cabinP->E2.Vacation = cabinP->sets.user.functions.Vacation;
  }
}
*/

void ceCabinWorkSets(const stCabin_Flash *cabinC, stCabin_Ram *cabinP, stIceMachine_Ram *iceMachineP, const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{
    unsigned char workIndex;

    cabinP->sets.work.Mode = cabinP->sets.user.Mode;
    cabinP->sets.work.FF = cabinP->sets.user.FF;
    cabinP->sets.work.FRZ = cabinP->sets.user.FRZ;
    cabinP->sets.work.Convertible = cabinP->sets.user.Convertible;

    if (cabinP->values.quickBlock)
    {
        cabinP->sets.work.functions.Quick = false;
        cabinP->sets.user.functions.Quick = false;
    }
    else
        cabinP->sets.work.functions.Quick = cabinP->sets.user.functions.Quick;

    cabinP->sets.work.functions.Vacation = cabinP->sets.user.functions.Vacation;
    cabinP->sets.work.functions.Rice = cabinP->sets.user.functions.Rice;

    switch (cabinP->sets.work.Mode)
    {
    case _CabinMode_FF:
        if(iceMachineP->values.QuickIce60)
            workIndex = cabinC->sets.indexes.FF[5];    //<<QuickIce60 Minimum Position 
        else if (cabinP->sets.work.functions.Vacation)
            workIndex = cabinC->sets.indexes.functions.Vacation;
        else if (cabinP->sets.work.functions.Quick)
            workIndex = cabinC->sets.indexes.functions.QuickFF;
        else if (cabinP->sets.work.functions.Rice)
            workIndex = cabinC->sets.indexes.functions.Rice;
        else
            workIndex = cabinC->sets.indexes.FF[cabinP->sets.work.FF];
        break;

    case _CabinMode_FRZ:
        if(iceMachineP->values.QuickIce60)
            workIndex = cabinC->sets.indexes.functions.QuickFRZ;    //<<QuickIce60
        else if (cabinP->sets.work.functions.Quick)
            workIndex = cabinC->sets.indexes.functions.QuickFRZ;
        else if (cabinP->sets.work.functions.EcoExtra)
            workIndex = cabinC->sets.indexes.FRZ[3];    //<< [3] Minimum Position 
        else if (cabinP->sets.user.functions.Convertible)
            workIndex = cabinC->sets.indexes.functions.Convertible[cabinP->sets.work.Convertible];
        else if (cabinP->sets.user.functions.CabinOff)
            workIndex = cabinC->sets.indexes.functions.CabinOff;
        else
            workIndex = cabinC->sets.indexes.FRZ[cabinP->sets.work.FRZ];
        break;

    default:
        workIndex = 0;
        break;
    }
    if(cabinP->values.defrost.preCoolActive) 
    {
      cabinP->values.CoolPreCoolOffset = cabinC->parameters.Cool.PreCoolOffsets[cabinP->values.ambientType];
      cabinP->values.FanPreCoolOffset = cabinC->parameters.Fan.PreCoolOffsets[cabinP->values.ambientType];
    } 
    else if(cabinP->values.defrost.afterDefrostActive) 
    {
      cabinP->values.CoolPreCoolOffset = cabinC->parameters.Cool.AfterDefrostOffsets[cabinP->values.ambientType];
    } 
    else 
    {
      cabinP->values.CoolPreCoolOffset = 0; 
      cabinP->values.FanPreCoolOffset = 0;
    }
    if ((!iceMachineP->sets.iceOFF) && (iceMachineP->values.quickIce) && (!cabinP->values.doorsClosedState) && (!iceMachineP->sets.quickIceOnDisplay))
    {
        if (cabinC->sets.configs.functions.QuickIce && iceMachineP->values.quickIce && (!cabinP->values.doorsClosedState))
        {
            if ((cabinC->type == _CabinType_FRZ) && (cabinP->sets.work.Mode == _CabinMode_FRZ))
            {
                cabinP->sets.datas.coolCutInTemp = ((cabinSetsDatasTable_F[workIndex].coolCutInTemp) - iceMachineP->values.HBY_SetDecrease + cabinP->values.CoolPreCoolOffset);
                cabinP->sets.datas.coolCutOutTemp = ((cabinSetsDatasTable_F[workIndex].coolCutOutTemp) - iceMachineP->values.HBY_SetDecrease + cabinP->values.CoolPreCoolOffset);
                cabinP->sets.datas.fanCutInTemp = ((cabinSetsDatasTable_F[workIndex].fanCutInTemp) - iceMachineP->values.HBY_SetDecrease + cabinP->values.FanPreCoolOffset);
                cabinP->sets.datas.fanCutOutTemp = ((cabinSetsDatasTable_F[workIndex].fanCutOutTemp) - iceMachineP->values.HBY_SetDecrease + cabinP->values.FanPreCoolOffset);
            }

            if ((cabinC->type == _CabinType_FF) && (cabinP->sets.work.Mode == _CabinMode_FF))
            {
                workIndex = cabinC->sets.indexes.functions.QuickIce;
                cabinP->sets.datas.coolCutInTemp = cabinSetsDatasTable_F[workIndex].coolCutInTemp + cabinP->values.CoolPreCoolOffset;
                cabinP->sets.datas.coolCutOutTemp = cabinSetsDatasTable_F[workIndex].coolCutOutTemp + cabinP->values.CoolPreCoolOffset;
                cabinP->sets.datas.fanCutInTemp = cabinSetsDatasTable_F[workIndex].fanCutInTemp + cabinP->values.FanPreCoolOffset;
                cabinP->sets.datas.fanCutOutTemp = cabinSetsDatasTable_F[workIndex].fanCutOutTemp + cabinP->values.FanPreCoolOffset;
            }
        }
    }
    else
    {
        if (cabinP->values.doorsClosedState)
        { // KK  mod
//            cabinP->sets.datas.coolCutInTemp = (cabinSetsDatasTable_F[workIndex].coolCutInTemp + cabinC->parameters.Cool.Offsets[cabinP->values.ambientType] + cabinP->values.CoolPreCoolOffset);
//            cabinP->sets.datas.coolCutOutTemp = (cabinSetsDatasTable_F[workIndex].coolCutOutTemp + cabinC->parameters.Cool.Offsets[cabinP->values.ambientType] + cabinP->values.CoolPreCoolOffset);
            cabinP->sets.datas.coolCutInTemp = (cabinSetsDatasTable_F[workIndex].coolCutInTemp + cabinC->parameters.Cool.Offsets[cabinP->values.ambientType] + cabinP->values.CoolPreCoolOffset + cabinC->parameters.Cool.OffsetsShiftCutIn[cabinP->values.ambientType]);
            cabinP->sets.datas.coolCutOutTemp = (cabinSetsDatasTable_F[workIndex].coolCutOutTemp + cabinC->parameters.Cool.Offsets[cabinP->values.ambientType] + cabinP->values.CoolPreCoolOffset + cabinC->parameters.Cool.OffsetsShiftCutOut[cabinP->values.ambientType]);
            cabinP->sets.datas.fanCutInTemp = (cabinSetsDatasTable_F[workIndex].fanCutInTemp + cabinC->parameters.Fan.Offsets[cabinP->values.ambientType] + cabinP->values.FanPreCoolOffset);
            cabinP->sets.datas.fanCutOutTemp = (cabinSetsDatasTable_F[workIndex].fanCutOutTemp + cabinC->parameters.Fan.Offsets[cabinP->values.ambientType] + cabinP->values.FanPreCoolOffset);
        }
        else
        { // KA mod
//            cabinP->sets.datas.coolCutInTemp = (cabinSetsDatasTable_F[workIndex].coolCutInTemp + cabinC->parameters.Cool.Offsets[cabinP->values.ambientType] + cabinP->values.CoolPreCoolOffset);
//            cabinP->sets.datas.coolCutOutTemp = (cabinSetsDatasTable_F[workIndex].coolCutOutTemp + cabinC->parameters.Cool.Offsets[cabinP->values.ambientType] + cabinP->values.CoolPreCoolOffset);
            cabinP->sets.datas.coolCutInTemp = (cabinSetsDatasTable_F[workIndex].coolCutInTemp + cabinC->parameters.Cool.Offsets[cabinP->values.ambientType] + cabinP->values.CoolPreCoolOffset + cabinC->parameters.Cool.OffsetsShiftCutIn[cabinP->values.ambientType]);
            cabinP->sets.datas.coolCutOutTemp = (cabinSetsDatasTable_F[workIndex].coolCutOutTemp + cabinC->parameters.Cool.Offsets[cabinP->values.ambientType] + cabinP->values.CoolPreCoolOffset + cabinC->parameters.Cool.OffsetsShiftCutOut[cabinP->values.ambientType]);
            cabinP->sets.datas.fanCutInTemp = (cabinSetsDatasTable_F[workIndex].fanCutInTemp + cabinC->parameters.Fan.Offsets[cabinP->values.ambientType] + cabinP->values.FanPreCoolOffset);
            cabinP->sets.datas.fanCutOutTemp = (cabinSetsDatasTable_F[workIndex].fanCutOutTemp + cabinC->parameters.Fan.Offsets[cabinP->values.ambientType] + cabinP->values.FanPreCoolOffset);
        }
    }
    if (cabinP->values.ambientType == _Ambient_NONE)
    {
        if (cabinP->values.doorsClosedState)
        {
            cabinP->sets.datas.lowTempHeaterCutInTemp =
                cabinSetsDatasTable_F[workIndex].lowTempHeaterDCCutInTemp;
            cabinP->sets.datas.lowTempHeaterCutOutTemp =
                cabinSetsDatasTable_F[workIndex].lowTempHeaterDCCutOutTemp;
        }
        else
        {
            cabinP->sets.datas.lowTempHeaterCutInTemp =
                cabinSetsDatasTable_F[workIndex].lowTempHeaterDOCutInTemp;
            cabinP->sets.datas.lowTempHeaterCutOutTemp =
                cabinSetsDatasTable_F[workIndex].lowTempHeaterDOCutOutTemp;
        }
    }
    else
    {
        if (cabinP->values.doorsClosedState)
        {
            cabinP->sets.datas.lowTempHeaterCutInTemp =
                cabinSetsDatasTable_F[workIndex].lowTempHeaterDCCutInTemp + cabinC->parameters.HeaterA.LTHeaterAmbientOffSet[cabinP->values.ambientType];
            cabinP->sets.datas.lowTempHeaterCutOutTemp =
                cabinSetsDatasTable_F[workIndex].lowTempHeaterDCCutOutTemp + cabinC->parameters.HeaterA.LTHeaterAmbientOffSet[cabinP->values.ambientType];
        }
        else
        {
            cabinP->sets.datas.lowTempHeaterCutInTemp =
                cabinSetsDatasTable_F[workIndex].lowTempHeaterDOCutInTemp + cabinC->parameters.HeaterA.LTHeaterAmbientOffSet[cabinP->values.ambientType];
            cabinP->sets.datas.lowTempHeaterCutOutTemp =
                cabinSetsDatasTable_F[workIndex].lowTempHeaterDOCutOutTemp + cabinC->parameters.HeaterA.LTHeaterAmbientOffSet[cabinP->values.ambientType];
        }
    }
    
#if defined (AI_COOL_V1)
    CabinAIDecreaseCabinSetIfRequired(cabinC, cabinP);
#endif
    
#if defined (AI_COOL_V1_5)  
    RefrigeratorAIDecreaseCabinSetsIfRequired(rfC, rfP);
#endif
}

void cabinWorkCompSpeedUp(const stCabin_Flash *cabinC, stCabin_Ram *cabinP, const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP)
{   
    switch (cabinP->sets.user.Mode)
    {
        case _CabinMode_FF:
            if(cabinP->sets.user.MaxDialUserSet == cabinP->sets.datas.display)
              cabinP->sets.work.functions.CompSpeedLevelNeedUpLH = true;
            else
              cabinP->sets.work.functions.CompSpeedLevelNeedUpLH = false;
          break;
          
        case _CabinMode_FRZ:
            if(cabinP->sets.user.MaxDialUserSet == cabinP->sets.datas.display)
              cabinP->sets.work.functions.CompSpeedLevelNeedUpLH = true;
            else
              cabinP->sets.work.functions.CompSpeedLevelNeedUpLH = false;
          break;
        default:
          break;
    }  
}

static void resetHighTempParameters(stRefrigerator_Ram *rfP, stCabin_Ram *cabinP)
{

    rfP->Values.errHighTemp = 0;
    cabinP->values.HighTemErr.algorithmActivationTime = 0;
    cabinP->values.HighTemErr.activationTimer = 0;
    cabinP->values.HighTemErr.highTempStatus = _idle;
    cabinP->values.HighTemErr.isactive = 0;
    cabinP->values.HighTemErr.FirstCutoutDoneF = 0;
    cabinP->E2.FirstCutoutDoneF = 0;
    cabinP->E2.HighTempIsactive = 0;
    cabinP->E2.highTempStatus = _idle;
    cabinP->E2.AlgorithmActivationTime = 0;
}

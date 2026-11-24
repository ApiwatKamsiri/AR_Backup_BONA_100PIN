#include "ce_dispenser.h"


void ceDispenserNextState(const tsUiProcConsts *consts, tsUiProcParams *params,
                          stDispenser_Flash const *dispenserC, stDispenser_Ram *dispenserP,
                          bool cmdReceived , unsigned char *displayEvent)
{    
    
    if ((!cmdReceived) ||
        (!(dispenserC->waterType || dispenserC->iceType)))
      return;
    if( !(dispenserP->components.tactSwitch.value))
    {
      DispenserNextState(dispenserC, dispenserP,cmdReceived, displayEvent);
      dispenserP->values.dispenserTactFlag = 0;            
      uiResetSetChangeTimer(params->ui);
      if(params->rf->IceSource.values.dispanserRepushTimer)
      {
        params->rf->IceSource.values.iceDispenserWaitTime=150;
        params->rf->IceSource.values.dispanserRepushTimer = 0;
      } 
    /*  if(dispenserP->sets.state == _DispenserState_Measured_CalibrateCustom)
        sysBuzzerCmd(eSysBuzzerPlayOpenMelody);
      else
      {*/
        if(!dispenserP->sets.commandIsNotAllowedF)
          sysBuzzerCmd(eSysBuzzerPlayButtonMelody);
        else
          sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
     /* }*/
      
    }
    else
      sysBuzzerCmd(eSysBuzzerPlayWarningMelody);
    
    dispenserP->sets.commandIsNotAllowedF = false;
          
        
}

void ceDispenserOnOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                      stDispenser_Flash const *dispenserC, stDispenser_Ram *dispenserP,
                      bool cmdReceived)
{
    if ((!cmdReceived) ||
        (!(dispenserC->waterType || dispenserC->iceType)))
        return;

    DispenserOnOff(dispenserC, dispenserP);

    if (dispenserP->sets.state)
        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    else
        sysBuzzerCmd(eSysBuzzerPlayEndMelody);

    uiResetSetChangeTimer(params->ui);
}
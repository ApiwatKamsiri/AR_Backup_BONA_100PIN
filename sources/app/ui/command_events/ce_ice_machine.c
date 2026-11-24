#include "ce_ice_machine.h"

void ceIceMachineOnOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                       stIceMachine_Flash const *iceMachineC, stIceMachine_Ram *iceMachineP,
                       bool cmdReceived)
{
    if (!cmdReceived)
        return;

    if (iceMachineC->type)
    {
        iceMachineP->sets.iceOFF = !iceMachineP->sets.iceOFF;

        if (!iceMachineP->sets.iceOFF)
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
        else
        {
            sysBuzzerCmd(eSysBuzzerPlayEndMelody);
            iceMachineP->values.quickIce60MaximumTimer = 0;
            iceMachineP->values.quickIce60MaximumTimesCount = 0;
        }

    }

    iceMachineP->E2.iceOFF = iceMachineP->sets.iceOFF;
    uiResetSetChangeTimer(params->ui);
    if(iceMachineP->sets.iceOFF 
      /*APW*/&& params->ui->states.commercial.status == _UI_Commercial_None)
    {
      iceMachineP->values.QuickIce60 = false;
      iceMachineP->E2.QuickIce60 = iceMachineP->values.QuickIce60;
    }
    uiResetSetChangeTimer(params->ui);
}

void ceQuickIce60OnOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                       stIceMachine_Flash const *iceMachineC, stIceMachine_Ram *iceMachineP,
                       bool cmdReceived)
{
    if (!cmdReceived)
      return;
    
    if (iceMachineC->type)
    {
      if(!iceMachineP->sets.iceOFF
        /* APW */ && params->ui->states.commercial.status == _UI_Commercial_None)
      {
        iceMachineP->values.QuickIce60 = !iceMachineP->values.QuickIce60;
        if(iceMachineP->values.QuickIce60)
        {
          sysBuzzerCmd(eSysBuzzerPlayStartMelody);
          iceMachineP->values.quickIce60MaximumTimer = iceMachineP->sets.quickIce60MaximumTime;
          iceMachineP->values.quickIce60MaximumTimesCount = iceMachineP->sets.quickIce60MaximumTimes * 2;
        }        
        else
        {
          sysBuzzerCmd(eSysBuzzerPlayEndMelody);
          iceMachineP->values.quickIce60MaximumTimer = 0;
          iceMachineP->values.quickIce60MaximumTimesCount = 0;
        }
      }
      else
          //sysBuzzerCmd(eSysBuzzerPlayEndMelody);
      /*** APW ***/
      {
        if(params->ui->states.commercial.status == _UI_Commercial_None)
          sysBuzzerCmd(eSysBuzzerPlayEndMelody);
        else
        {
          if(!iceMachineP->values.QuickIce60)
          {
            iceMachineP->values.QuickIce60 = 1;
            sysBuzzerCmd(eSysBuzzerPlayStartMelody);
          }
        }
      }
      /*** APW ***/
    }
    if(params->ui->states.commercial.status == _UI_Commercial_None)
      iceMachineP->E2.QuickIce60 = iceMachineP->values.QuickIce60;
    uiResetSetChangeTimer(params->ui);
}

/*** APW : Add Ice making component test in CT for HOSHI ***/
void ceIceComponentTest(const tsUiProcConsts *consts, tsUiProcParams *params,
                       stIceMachine_Flash const *iceMachineC, stIceMachine_Ram *iceMachineP,
                       bool cmdReceived)
{
    if (!cmdReceived)
      return;
    
    if (iceMachineC->type)
    {
      iceMachineP->values.IceComponentTest = !iceMachineP->values.IceComponentTest;
      
      if(iceMachineP->values.IceComponentTest)
        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
      else
        sysBuzzerCmd(eSysBuzzerPlayEndMelody);
    }

    uiResetSetChangeTimer(params->ui);
}
/*** APW : Add Ice making component test in CT for HOSHI ***/
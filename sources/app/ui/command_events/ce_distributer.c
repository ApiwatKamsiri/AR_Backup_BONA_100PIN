#include "ce_distributer.h"

void ceDistributerSetState(tsUiRam *uiP, enUI_DistributerStates to,
                               bool cmdReceived)
{
    if (!cmdReceived)
        return;

    switch (to)
    {
    case _UI_DistributerState_Sets:
        uiP->states.distributer.timer = _UI_Distributer_Sets_Time;
        break;
    case _UI_DistributerState_Password:
        if (uiP->password.stayTimer)
            uiP->states.distributer.state = to;
        break;
    default:
        uiP->states.distributer.state = to;
        break;
    }
}


void ceDistributerStart(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    eventPost(eProcessRefrigerator, eSysEventStartDistributerMode, NULL, 0);
    uiP->password.stayTimer = 0;
}

/*
Fair Mode and Distributer Mode are exactly the same but LED_Alert.
For saving program memory, Fair Mode is driven from Distributer Mode routins.
Only difference is fairModeFlag. if this flag is set, Fair mode is active.
*/
void ceFairModeStart(tsUiRam *uiP, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    // CHECKPOINT
    //params->rf->workMode.E2.fairModeFlag = 1;
    eventPost(eProcessRefrigerator, eSysEventStartFairMode, NULL, 0);
    uiP->password.stayTimer = 0;
}

void ceDistributerExit(bool cmdReceived)
{
    if (!cmdReceived)
        return;

    // CHECKPOINT
    eventPost(eProcessRefrigerator, eSysEventStartProductionTestMode, NULL, 0);
}
#ifdef HOSHI_H30
//Demo Display
void ceDemoModeDisplay(tsUiRam *uiP)
{
  if(!uiP->timers.dspOffTimer)
  {
    if(!uiP->states.distributer.DemoDisplaytimer)
    {
      uiP->states.distributer.DemoDisplaytimer = 50;
      switch(uiP->states.distributer.DemoDisplayStep)
      {
      case 0:

        uiP->dspPacket.funcs.ledIceOn = true; 
        uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
        uiP->dspPacket.cabinData[1] = 0x00;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
        uiP->dspPacket.cabinData[0] = 0x00;
        uiP->dspPacket.funcs.ledEcoExtra = false;

        uiP->states.distributer.DemoDisplayStep = 1;
        break;
      case 1:
        
        uiP->dspPacket.funcs.ledIceOn = false; 
        uiP->dspPacket.funcs.ledCabin1Quick =  0x03;
        uiP->dspPacket.cabinData[1] = 0xD8;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
        uiP->dspPacket.cabinData[0] = 0x00;
        uiP->dspPacket.funcs.ledEcoExtra = false;

        uiP->states.distributer.DemoDisplayStep = 2;
        break;
      case 2:
        uiP->dspPacket.funcs.ledIceOn = false; 
        uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
        uiP->dspPacket.cabinData[1] = 0x00;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x03;
        uiP->dspPacket.cabinData[0] = 0x28;
        uiP->dspPacket.funcs.ledEcoExtra = false;

        uiP->states.distributer.DemoDisplayStep = 3;
        break;
      case 3:
        uiP->dspPacket.funcs.ledIceOn = false; 
        uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
        uiP->dspPacket.cabinData[1] = 0x00;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
        uiP->dspPacket.cabinData[0] = 0x00;
        uiP->dspPacket.funcs.ledEcoExtra = true;
        
        uiP->states.distributer.DemoDisplayStep = 4;
        break;
      case 4:
        uiP->dspPacket.funcs.ledIceOn = false; 
        uiP->dspPacket.funcs.ledCabin1Quick = 0x00;
        uiP->dspPacket.cabinData[1] = 0x00;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x03;
        uiP->dspPacket.cabinData[0] = 0x28;
        uiP->dspPacket.funcs.ledEcoExtra = false;

        uiP->states.distributer.DemoDisplayStep = 5;
        break;
      case 5:
        uiP->dspPacket.funcs.ledIceOn = false; 
        uiP->dspPacket.funcs.ledCabin1Quick =  0x03;
        uiP->dspPacket.cabinData[1] = 0xD8;
        uiP->dspPacket.funcs.ledCabin0Quick = 0x00;
        uiP->dspPacket.cabinData[0] = 0x00;
        uiP->dspPacket.funcs.ledEcoExtra = false;

        uiP->states.distributer.DemoDisplayStep = 0;
        break;
      default:
        uiP->states.distributer.DemoDisplayStep = 0;
        break;
      }
    }
  }
}
#endif //HOSHI_H30
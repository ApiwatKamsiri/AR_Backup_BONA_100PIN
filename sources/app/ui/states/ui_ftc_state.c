#include "ui_ftc_state.h"
#include "../components/ui_display.h"
#include "../command_events/cmd_check.h"
#include "board/board.h"
#include "../command_events/ce_buzzer.h"

// implement ui FTC tests inside board_ux40 file
// use callback timer instead of state.ftc.timer to blind all leds on, off
// get new event from display, check it, then use sysBuzzerCmd if it's eDisplayEventBuzzer

void uiFTCState20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  tsUiRam *uiP = params->ui;
  UNUSED(consts);

  if (uiP->states.ftc.timer)
    uiP->states.ftc.timer--;
}

void uiFTCStateTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  tsUiRam *uiP = params->ui;

  uiP->dspPacket.funcs.displayOff = false;

  switch (uiP->states.ftc.state)
  {
  case _UI_FTCState_BlinkLed:
  default:
    if (!uiP->states.ftc.timer)
      uiP->states.ftc.timer = 100;
    if (uiP->states.ftc.timer > 25)
    { // Wait for 1.5 second (is it elapsed?)
      uiDspAllOn(consts, params);
    }
    else
    {
      uiDspAllOff(consts, params);
    }
    break;
  }
  params->rf->E2.FTCState = FTC.Tests.FTC;
  params->rf->E2.ICTState = FTC.Tests.ICT;

}

void uiFTCStateKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params)
{
  tsUiRam *uiP = params->ui;

  switch (uiP->states.ftc.state)
  {
  case _UI_FTCState_BlinkLed:
  default:
    break;
  }

  ceBuzzer(isEvent(eDisplayEventBuzzer));
  clearEvent();
}
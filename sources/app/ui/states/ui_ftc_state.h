#ifndef _UI_FTC_H
#define _UI_FTC_H

// implement ui FTC tests inside board_ux40 file
// use callback timer instead of state.ftc.timer to blind all leds on, off
// get new event from display, check it, then use sysBuzzerCmd if it's eDisplayEventBuzzer

#include "../ui_proc.h"

void UI_FTC_Init(void);

void uiFTCState20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void UI_FTC_1MinTask(void);

void uiFTCStateTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiFTCStateKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params);
#endif


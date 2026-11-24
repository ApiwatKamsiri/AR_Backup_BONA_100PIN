#ifndef _CE_Distributer_H
#define _CE_Distributer_H

#include <stdbool.h>
#include "../ui_proc.h"

void ceDistributerSetState(tsUiRam *uiP, enUI_DistributerStates to, bool cmdReceived);
void ceDistributerStart(tsUiRam *uiP, bool cmdReceived);
void ceFairModeStart(tsUiRam *uiP, bool cmdReceived);
void ceDistributerExit(bool cmdReceived);
#ifdef HOSHI_H30
void ceDemoModeDisplay(tsUiRam *uiP);//Demo Display
#endif //HOSHI_H30
#endif

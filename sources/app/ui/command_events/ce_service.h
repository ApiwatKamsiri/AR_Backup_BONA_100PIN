#ifndef _CE_Service_H
#define _CE_Service_H

#include <stdbool.h>
#include "../ui_ram.h"

void ceServiceSetState(tsUiRam* uiP, enUI_ServiceStates to, bool cmdReceived);

void ceServiceIncreaseSlideIndex(tsUiRam* uiP, bool cmdReceived);
void ceServiceDecreaseSlideIndex(tsUiRam* uiP, bool cmdReceived);
void ceServiceIncreaseComponentIndex(tsUiRam* uiP, bool cmdReceived);
void ceServiceDecreaseComponentIndex(tsUiRam* uiP, bool cmdReceived);
void ceServiceIncreaseSensorIndex(tsUiRam* uiP, bool cmdReceived);
void ceServiceDecreaseSensorIndex(tsUiRam* uiP, bool cmdReceived);
void ceServiceIncreaseParameterIndex(tsUiRam* uiP, bool cmdReceived);
void ceServiceDecreaseParameterIndex(tsUiRam* uiP, bool cmdReceived);
void ceServiceSelectedChange(tsUiRam* uiP, bool cmdReceived);
void ceServiceSpecialsChange(tsUiRam* uiP, bool cmdReceived);
void ceServiceAllOff(tsUiRam* uiP,bool cmdReceived);
void ceServiceStart(tsUiRam *uiP, bool cmdReceived);
void ceServiceExit(tsUiRam* uiP, bool cmdReceived);

#endif

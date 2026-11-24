#ifndef _CE_IceMachine_H
#define _CE_IceMachine_H

#include <stdbool.h>
#include "../ui_proc.h"

void ceIceMachineOnOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                       stIceMachine_Flash const *iceMachineC, stIceMachine_Ram *iceMachineP,
                       bool cmdReceived);
void ceQuickIce60OnOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                       stIceMachine_Flash const *iceMachineC, stIceMachine_Ram *iceMachineP,
                       bool cmdReceived);

//APW
void ceIceComponentTest(const tsUiProcConsts *consts, tsUiProcParams *params,
                       stIceMachine_Flash const *iceMachineC, stIceMachine_Ram *iceMachineP,
                       bool cmdReceived);

#endif

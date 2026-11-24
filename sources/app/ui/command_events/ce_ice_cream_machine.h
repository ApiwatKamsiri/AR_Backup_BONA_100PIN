#ifndef _CE_IceCreamMachine_H
#define _CE_IceCreamMachine_H

#include <stdbool.h>
#include "../ui_proc.h"

void ceIceCreamMachineOnOff(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC,
                            stIceCreamMachine_Ram *iceCreamMachineP, bool cmdReceived);

void ceIceCreamMachineSet(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC,
                          stIceCreamMachine_Ram *iceCreamMachineP,
                          bool cmdReceived);

void ceIceCreamMachineStart(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC,
                            stIceCreamMachine_Ram *iceCreamMachineP,
                            bool cmdReceived);

void ceIceCreamMachineEnd(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC,
                          stIceCreamMachine_Ram *iceCreamMachineP,
                          bool cmdReceived);

#endif

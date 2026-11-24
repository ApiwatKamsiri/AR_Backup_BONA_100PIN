#ifndef _CE_Dispenser_H
#define _CE_Dispenser_H

#include <stdbool.h>
#include "../ui_proc.h"
#include "../../dispenser/dispenser_procedures.h" 

void ceDispenserNextState(const tsUiProcConsts *consts, tsUiProcParams *params,
                          stDispenser_Flash const *dispenserC, stDispenser_Ram *dispenserP,
                          bool cmdReceived , unsigned char *displayEvent);

void ceDispenserOnOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                      stDispenser_Flash const *dispenserC, stDispenser_Ram *dispenserP,
                      bool cmdReceived);

#endif
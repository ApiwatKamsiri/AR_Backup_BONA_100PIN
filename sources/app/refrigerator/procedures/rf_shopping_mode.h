#ifndef _RefrigeratorShoppingMode_H
#define _RefrigeratorShoppingMode_H

#include "../rf_ram.h"
#include "../rf_configs.h"

void rfShoppingModeInit(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfShoppingMode1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

#endif

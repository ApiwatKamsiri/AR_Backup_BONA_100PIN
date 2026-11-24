#ifndef _RefrigeratorDoorsClosed_H
#define _RefrigeratorDoorsClosed_H

#include "../rf_ram.h"
#include "../rf_configs.h"

void rfDoorsClosedInit(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfDoorsClosed1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfDoorsClosedTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfDoorsClosedDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

#endif

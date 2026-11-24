#ifndef _RefrigeratorSilent_H
#define _RefrigeratorSilent_H

#include "../rf_ram.h"
#include "../rf_configs.h"

void rfSilent1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSilentTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

void rfSilentEnable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSilentDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSilentToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSilentToWork(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSilentStore(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

#endif

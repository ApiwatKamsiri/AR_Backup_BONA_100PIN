#ifndef _RefrigeratorRice_H
#define _RefrigeratorRice_H

#include "../rf_ram.h"
#include "../rf_configs.h"

void rfRiceTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfRiceEnable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfRiceDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfRiceToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

#endif

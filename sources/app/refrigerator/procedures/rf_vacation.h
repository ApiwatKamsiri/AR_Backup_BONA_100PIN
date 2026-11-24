#ifndef _RefrigeratorVacation_H
#define _RefrigeratorVacation_H

#include "../rf_ram.h"
#include "../rf_configs.h"

void rfVacationEnable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfVacationDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfVacationToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfVacationTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

#endif

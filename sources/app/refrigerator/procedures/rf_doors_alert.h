#ifndef _RefrigeratorDoorsAlert_H
#define _RefrigeratorDoorsAlert_H

#include "../rf_ram.h"
#include "../rf_configs.h"

void rfDoorsAlertInit(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfDoorsAlert1SecTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfDoorsAlert1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfDoorsAlertTask(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);

#endif

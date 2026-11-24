#ifndef _RefrigeratorEcoExtra_H
#define _RefrigeratorEcoExtra_H

#include "../rf_ram.h"
#include "../rf_configs.h"

void rfEcoExtraInit(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfEcoExtra1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfEcoExtraTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

void rfEcoExtraEnable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfEcoExtraDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfEcoExtraToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

void rfEcoExtraResetTimer(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
// void rfEcoExtraToWork( const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfEcoExtraStore(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

#endif

#ifndef _rfSabbath_H
#define _rfSabbath_H

#include "../rf_ram.h"
#include "../rf_configs.h"

void rfSabbathTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSabbath1MinTask(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSabbathEnable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSabbathDisable(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSabbathToggle(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSabbathToWork(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void rfSabbathStore(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);

#endif

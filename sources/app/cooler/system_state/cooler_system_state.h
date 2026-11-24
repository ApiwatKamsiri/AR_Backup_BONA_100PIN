#ifndef COOLER_SYSTEM_STATE_H
#define COOLER_SYSTEM_STATE_H

#include "../cooler_configs.h"
#include "../cooler_ram.h"

unsigned char coolerGetSystemState(const stCooler_Flash *coolerC, stCooler_Ram *coolerP);							// cooler types
unsigned char coolerGetStatePriority(stCooler_Ram *coolerP, const stCoolerSystemState *systemState); // cooler types
#endif
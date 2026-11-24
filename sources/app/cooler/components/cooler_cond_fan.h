#ifndef COOLER_COND_FAN_H
#define COOLER_COND_FAN_H

#include "../cooler_configs.h"
#include "../cooler_ram.h"

void coolerUpdateCondFanSpeed(const stCooler_Flash *coolerC, stCooler_Ram *coolerP, unsigned char level);

#endif
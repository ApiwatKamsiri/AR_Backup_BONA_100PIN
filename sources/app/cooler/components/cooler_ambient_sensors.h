#ifndef COOLER_AMBIENT_SENSORS_H
#define COOLER_AMBIENT_SENSORS_H

#include "../cooler_configs.h"
#include "../cooler_ram.h"


void coolerUpdateAmbientType(const stCooler_Flash *coolerC, stCooler_Ram *coolerP);
void coolerUpdateHumidityType(const stCooler_Flash *coolerC, stCooler_Ram *coolerP);

#endif
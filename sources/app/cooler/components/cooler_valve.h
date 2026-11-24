#ifndef COOLER_VALVE_H
#define COOLER_VALVE_H

#include "../cooler_configs.h"
#include "../cooler_ram.h"

typedef enum
{
    _ALL_CLOSE = 0,
    _FF_OPEN,
    _FRZ_OPEN,
    _ALL_OPEN,
} enSolenoidValvePosition;

void coolerInitValveState(stCooler_Ram *coolerP);
void coolerUpdateValveState(const stCooler_Flash *coolerC, stCooler_Ram *coolerP,
                       unsigned char state,
                       unsigned char gasType,
                       unsigned char gasLevel);

#endif
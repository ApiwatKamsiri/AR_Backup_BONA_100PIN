#ifndef TEMP_FUNCS_H
#define TEMP_FUNCS_H

#include <stdbool.h>
#include "app/refrigerator/rf_ram.h"
#include "app/refrigerator/rf_configs.h"

unsigned char getCurrentPointerC(signed char CelciusValue);
signed char celciusToFahnrenheight(signed char valueInC);
// signed int NTC_10BitADCtoCDegree( unsigned int ADCValue );
signed char celciusExtToCelcius(signed short valueInCExt);
bool checkSetValueStatus(stRefrigerator_Ram *rfP, unsigned char cabinIndex);
bool checkSetValueStatusUp(stRefrigerator_Ram *rfP, unsigned char cabinIndex);
unsigned char resetTemperaturePointerF(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP, unsigned char cabinIndex);
signed char getFahrenheitValue(unsigned char pointer);
signed char getCelciusValue(unsigned char pointer);
void refreshTemperaturePointer(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP);
void defaultTemperaturePointerF(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP, unsigned char cabinIndex);
unsigned char getSetValueFromPointer(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP, unsigned char ind);
void setpointerValue(stRefrigerator_Ram *rfP, unsigned char cabinIndex, signed char savedVal);
unsigned char resetTemperaturePointerFup(const stRefrigerator_Flash *rfC, stRefrigerator_Ram *rfP, unsigned char cabinIndex);

#endif

#ifndef _CE_WaterSource_H
#define _CE_WaterSource_H

#include <stdbool.h>
#include "app/water_source/water_source_configs.h"
#include "app/water_source/water_source_ram.h"

void ceWaterSourceRstFilter(stWaterSource_Flash const *waterSourceC, stWaterSource_Ram *waterSourceP,
                            bool cmdReceived);

void ceWaterSourceErrFilter(stWaterSource_Flash const *waterSourceC, stWaterSource_Ram *waterSourceP,
                            bool cmdReceived);
#endif

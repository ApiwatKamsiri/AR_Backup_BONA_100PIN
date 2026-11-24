#ifndef CABIN_GAS_LEVEL_H
#define CABIN_GAS_LEVEL_H

#include "../cabin.h"

void cabinUpdateGasLevel(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinUpdateGasLevelAmbient(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

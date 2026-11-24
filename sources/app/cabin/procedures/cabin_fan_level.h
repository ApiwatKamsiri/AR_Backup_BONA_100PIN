#ifndef CABIN_FAN_LEVEL_H
#define CABIN_FAN_LEVEL_H

#include "../cabin.h"

void cabinUpdateFanLevel(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinClearFanLevel(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinUpdateFanLevelAmbient(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

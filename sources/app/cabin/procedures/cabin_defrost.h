#ifndef CABIN_DEFROST_H
#define CABIN_DEFROST_H

#include "../cabin.h"

void cabinDefrostMeanTimeInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinDefrostMeanTimeCTRL(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinDefrostUpdateInterval(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

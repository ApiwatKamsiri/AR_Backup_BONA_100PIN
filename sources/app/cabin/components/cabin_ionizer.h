#ifndef _CabinALL_Ionizer_H
#define _CabinALL_Ionizer_H

#include "../cabin.h"

void cabinIonizerInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinIonizer1SecTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinIonizer1MinTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinIonizerTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

#ifndef _CabinALL_Flap_H
#define _CabinALL_Flap_H
#include "../cabin.h"

void cabinFlap1SecTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFlapStates cabinFlapState(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinFlapControl(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

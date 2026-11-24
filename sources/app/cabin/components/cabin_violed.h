#ifndef _CabinALL_VioLed_H
#define _CabinALL_VioLed_H

#include "../cabin.h"

void cabinVioLedInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinVioLed1SecTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinVioLed1MinTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinVioLedTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

#ifndef _CabinFRZ_H
#define _CabinFRZ_H

#include "../cabin.h"

void cabinFrzInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinFrzTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
#endif

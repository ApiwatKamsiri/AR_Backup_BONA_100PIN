#ifdef JKRavailable

#ifndef _CabinJKR_H
#define _CabinJKR_H

#include "../cabin.h"

void cabinJkrInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinJkrTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif
#endif
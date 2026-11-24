#ifndef _CabinALL_VacuumBox_H
#define _CabinALL_VacuumBox_H

#include "../cabin.h"

void cabinVacuumBoxInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinVacuumBox1SecTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinVacuumBox1MinTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinVacuumBoxTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

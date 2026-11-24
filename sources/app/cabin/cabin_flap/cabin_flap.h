#ifndef _CabinFLAP_H
#define _CabinFLAP_H

#include "../cabin.h"

void cabinFlapInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinFlapTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

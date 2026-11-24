#ifndef _CabinALL_InnerLight_H
#define _CabinALL_InnerLight_H

#include "../cabin.h"

#define INNER_LIGHT_STEP 5
#define BLUE_LIGHT_STEP 5

void cabinInnerLight20MsTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinInnerLightInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinInnerLightTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

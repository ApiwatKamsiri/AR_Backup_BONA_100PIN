#ifndef _CabinALL_Switch_H
#define _CabinALL_Switch_H

#define _DOOR_CLOSED_DELAY 3 * 50 // 3*50*20 ms

#include "../cabin.h"

void cabinSwitchInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinSwitch20MsTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinSwitchTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

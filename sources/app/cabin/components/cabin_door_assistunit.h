#ifndef _CabinALL_DoorAssistUnit_H
#define _CabinALL_DoorAssistUnit_H

#include "../cabin.h"

void cabinDoorAssistUnit20MsTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinDoorAssistUnitT1sec(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinDoorAssistUnitInit(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinDoorAssistUnitTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif
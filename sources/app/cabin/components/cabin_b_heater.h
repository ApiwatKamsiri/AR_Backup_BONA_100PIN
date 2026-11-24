#ifndef _CabinALL_HeaterB_H
#define _CabinALL_HeaterB_H

#include "../cabin.h"

enHeaterBStates cabinBHeater(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinBHeaterControl(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

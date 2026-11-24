#ifndef _CabinALL_HeaterA_H
#define _CabinALL_HeaterA_H

#include "../cabin.h"

enHeaterAStates cabinAHeater(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinAHeaterControl(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterAStates cabinAHeaterHumidity(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enHeaterAStates cabinFfzHeaterACommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterAStates cabinFfzHeaterACommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);
#endif

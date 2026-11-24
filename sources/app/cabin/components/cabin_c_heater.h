#ifndef _CabinALL_HeaterCH
#define _CabinALL_HeaterCH

#include "../cabin.h"

enHeaterCStates cabinCHeater(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterCStates cabinCHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterCStates cabinCHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterCStates cabinCHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterCStates cabinCHeaterDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterCStates cabinCHeaterHumidity(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinCHeaterControl(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enHeaterCStates cabinFfHeaterCCommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterCStates cabinFfHeaterCCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterCStates cabinFfzHeaterCCommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterCStates cabinFfzHeaterCCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

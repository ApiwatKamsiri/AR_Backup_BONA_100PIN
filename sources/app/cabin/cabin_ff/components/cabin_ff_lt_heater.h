#ifndef _CabinFF_LTHeater_H
#define _CabinFF_LTHeater_H

#include "../../cabin.h"

enHeaterAStates cabinFfLTHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterAStates cabinFfLTHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterAStates cabinFfLTHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enHeaterAStates cabinFfHeaterACommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterAStates cabinFfHeaterACommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

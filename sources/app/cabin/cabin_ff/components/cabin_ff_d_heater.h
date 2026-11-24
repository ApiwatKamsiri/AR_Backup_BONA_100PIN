#ifndef _CabinFF_DHeater_H
#define _CabinFF_DHeater_H

#include "../../cabin.h"

enHeaterBStates cabinFfDHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinFfDHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinFfDHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinFfDHeaterDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enHeaterBStates cabinFfHeaterBCommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinFfHeaterBCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif



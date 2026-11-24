#ifndef _CabinFRZ_DHeater_H
#define _CabinFRZ_DHeater_H

#include "../../cabin.h"

enHeaterBStates cabinFrzDHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinFrzDHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinFrzDHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinFrzDHeaterDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enHeaterBStates cabinFfzHeaterBCommercialTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinFfzHeaterBCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);


#endif

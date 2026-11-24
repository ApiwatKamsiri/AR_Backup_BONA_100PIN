#ifdef JKRavailable

#ifndef _CabinJKR_DHeater_H
#define _CabinJKR_DHeater_H

#include "../../cabin.h"

enHeaterBStates cabinJkrDHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinJkrDHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinJkrDHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinJkrDHeaterDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif
#endif
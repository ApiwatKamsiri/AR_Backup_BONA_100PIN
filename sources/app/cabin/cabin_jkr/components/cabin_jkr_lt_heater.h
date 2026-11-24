#ifdef JKRavailable

#ifndef _CabinJKR_LTHeater_H
#define _CabinJKR_LTHeater_H

#include "../../cabin.h"

enHeaterAStates cabinJkrLTHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterAStates cabinJkrLTHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterAStates cabinJkrLTHeaterDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterAStates cabinJkrLTHeaterDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif
#endif
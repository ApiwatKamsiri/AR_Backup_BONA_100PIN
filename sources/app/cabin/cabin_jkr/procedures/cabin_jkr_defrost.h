#ifdef JKRavailable

#ifndef _CabinJKR_Defrost_H
#define _CabinJKR_Defrost_H

#include "../../cabin.h"

enDefrostStates cabinJkrDefrostIn(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enDefrostStates cabinJkrDefrostOut(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinJkrDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif
#endif
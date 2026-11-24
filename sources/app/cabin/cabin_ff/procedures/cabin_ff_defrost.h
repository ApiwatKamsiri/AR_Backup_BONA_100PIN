#ifndef _CabinFF_Defrost_H
#define _CabinFF_Defrost_H

#include "../../cabin.h"

enDefrostStates cabinFfDefrostIn(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enDefrostStates cabinFfDefrostOut(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinFfDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

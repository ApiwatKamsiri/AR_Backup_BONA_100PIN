#ifndef _CabinFRZ_Defrost_H
#define _CabinFRZ_Defrost_H

#include "../../cabin.h"

enDefrostStates cabinFrzDefrostIn(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enDefrostStates cabinFrzDefrostOut(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinFrzDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

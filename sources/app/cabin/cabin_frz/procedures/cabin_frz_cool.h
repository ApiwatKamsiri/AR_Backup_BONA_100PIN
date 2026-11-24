#ifndef _CabinFRZ_Cool_H
#define _CabinFRZ_Cool_H

#include "../../cabin.h"

enCoolStates cabinFrzCoolIn(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enCoolStates cabinFrzCoolOut(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

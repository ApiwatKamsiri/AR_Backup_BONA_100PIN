#ifdef JKRavailable

#ifndef _CabinJKR_Fan_H
#define _CabinJKR_Fan_H

#include "../../cabin.h"

enFanStates cabinJkrFanCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFanStates cabinJkrFanIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFanStates cabinJkrFanDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFanStates cabinJkrFanDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif
#endif
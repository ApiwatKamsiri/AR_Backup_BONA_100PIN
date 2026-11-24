#ifndef _CabinFF_Fan_H
#define _CabinFF_Fan_H

#include "../../cabin.h"

enFanStates cabinFfFanCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFanStates cabinFfFanIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFanStates cabinFfFanDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enFanStates cabinFfFanCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params);   //APW : Add Fan on operation in CT

#endif











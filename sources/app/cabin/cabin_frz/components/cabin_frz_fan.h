#ifndef _CabinFRZ_Fan_H
#define _CabinFRZ_Fan_H

#include "../../cabin.h"

enFanStates cabinFrzFanCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFanStates cabinFrzFanIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFanStates cabinFrzFanDefrost(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFanStates cabinFrzFanDrainage(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enFanStates cabinFrzFanCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params);    //APW : Add CT heater software
#endif

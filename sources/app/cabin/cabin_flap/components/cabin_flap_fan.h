#ifndef _CabinFLAP_Fan_H
#define _CabinFLAP_Fan_H

#include "../../cabin.h"

enFanStates cabinFlapFanCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFanStates cabinFlapFanIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enFanStates cabinFlapFanCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params);
#endif








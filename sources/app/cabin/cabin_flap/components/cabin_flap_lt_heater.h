#ifndef _CabinFLAP_LTH_H
#define _CabinFLAP_LTH_H

#include "../../cabin.h"

enHeaterAStates cabinFlapLTHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterAStates cabinFlapLTHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enHeaterAStates cabinFlapLTHeaterCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params);                // APW : Add heater on operation in CT
enHeaterAStates cabinFlapLTHeaterCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);     // APW : Add heater on operation in CT
#endif
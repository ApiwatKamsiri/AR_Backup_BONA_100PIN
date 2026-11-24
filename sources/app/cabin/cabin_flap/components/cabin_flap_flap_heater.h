#ifndef _CabinFLAP_FLPHeater_H
#define _CabinFLAP_FLPHeater_H

#include "../../cabin.h"

enHeaterBStates cabinFlapFlapHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterBStates cabinFlapFlapHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enHeaterBStates cabinFlapFlapHeaterCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params);                 // APW : Add heater on operation in CT
enHeaterBStates cabinFlapFlapHeaterCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);      // APW : Add heater on operation in CT
#endif
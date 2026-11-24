#ifndef _CabinFLAP_CHLHeater_H
#define _CabinFLAP_CHLHeater_H

#include "../../cabin.h"

enHeaterCStates cabinFlapCHLHeaterCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enHeaterCStates cabinFlapCHLHeaterIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);

enHeaterCStates cabinFlapCHLHeaterCommercial(tsCabinProcConsts const *consts, tsCabinProcParams *params);               //APW : Add heater on operation in CT
enHeaterCStates cabinFlapCHLHeaterCommercialDefrostTest(tsCabinProcConsts const *consts, tsCabinProcParams *params);    //APW : Add heater on operation in CT
#endif
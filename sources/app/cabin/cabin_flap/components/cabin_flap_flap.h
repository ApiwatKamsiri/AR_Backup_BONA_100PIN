#ifndef _CabinFLAP_Flap_H
#define _CabinFLAP_Flap_H

#include "../../cabin.h"

enFlapStates cabinFlapFlapCool(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enFlapStates cabinFlapFlapIdle(tsCabinProcConsts const *consts, tsCabinProcParams *params);
/*** APW : Add damper on operation in CT ***/
enFlapStates cabinFlapFlapCommercialState(tsCabinProcConsts const *consts, tsCabinProcParams *params);
/*** APW : Add damper on operation in CT ***/

#endif









#ifndef FILE_CABIN_FF_COOL_H
#define FILE_CABIN_FF_COOL_H

#include "../../cabin.h"

enCoolStates cabinFfCoolIn(tsCabinProcConsts const *consts, tsCabinProcParams *params);
enCoolStates cabinFfCoolOut(tsCabinProcConsts const *consts, tsCabinProcParams *params);

// extern unsigned char compressorOffForThermalCutOutTimer;
// extern unsigned char compressorOffForQFrzTimer;

#endif

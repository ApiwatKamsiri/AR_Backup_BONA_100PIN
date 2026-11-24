#ifndef CABIN_ERRORS_H
#define CABIN_ERRORS_H

#include "../cabin.h"

void cabinCheckHighTempErr(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinCheckHighTempErr1MinTask(tsCabinProcConsts const *consts, tsCabinProcParams *params);
signed short cabinResetMaxTempInCext(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinCheckDefrostHeaterErr(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinIncDefrostHeaterErrCounter(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinClearDefrostHeaterErrCounter(tsCabinProcConsts const *consts, tsCabinProcParams *params);
void cabinSensorsErr(tsCabinProcConsts const *consts, tsCabinProcParams *params);

#endif

#ifndef CABIN_SHARED_FUNCS_H
#define CABIN_SHARED_FUNCS_H

#include "../cabin.h"

// TODO call this functions
void cabinQuickFf(const tsCabinProcConsts *consts, tsCabinProcParams *params);
void cabinQuickFrz(const tsCabinProcConsts *consts, tsCabinProcParams *params);
void cabinQuickIce(const tsCabinProcConsts *consts, tsCabinProcParams *params);
//void cabinQuickIceCream(const tsCabinProcConsts *consts, tsCabinProcParams *params);
void cabinAirSupply(const tsCabinProcConsts *consts, tsCabinProcParams *params);
void cabinCheckFrzDefrost(const tsCabinProcConsts *consts, tsCabinProcParams *params);

#endif

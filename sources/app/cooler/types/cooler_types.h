#ifndef COOLER_TYPES_H
#define COOLER_TYPES_H

#include "app/cooler/cooler.h"

void coolerTypeDefault(const tsCoolerProcConsts * consts, tsCoolerProcParams * params);
void coolerTypeDefault2(const tsCoolerProcConsts * consts, tsCoolerProcParams * params);
void coolerTypeSemiParallel(const tsCoolerProcConsts * consts, tsCoolerProcParams * params);
void coolerTypeParallel(const tsCoolerProcConsts * consts, tsCoolerProcParams * params);

#endif
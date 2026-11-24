/*** APW : Add CT heater software ***/
#ifndef CABIN_COMMERCIAL_TEST_H
#define CABIN_COMMERCIAL_TEST_H

#include "../cabin.h"

void cabinCloseAllComponentsCommercial(stCabin_Ram *cabinP);
void cabinPerformanceCommercialTestInit(stCabin_Flash const *cabinC, stCabin_Ram *cabinP);
void cabinPerformanceCommercialTestTask(const tsCabinProcConsts *consts, tsCabinProcParams *params);
void cabinCommercialTestsFailedTask(const tsCabinProcConsts *consts, tsCabinProcParams *params);

#endif
/*** APW : Add CT heater software ***/
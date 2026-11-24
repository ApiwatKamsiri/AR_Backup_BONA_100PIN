#ifndef CABIN_PRODUCTION_TEST_H
#define CABIN_PRODUCTION_TEST_H

#include "../cabin.h"

void cabinCloseAllComponents(stCabin_Ram *cabinP);
void cabinPerformanceTestInit(stCabin_Flash const *cabinC, stCabin_Ram *cabinP);
void isProductionTestCondition(stCabin_Flash const *cabinC, stCabin_Ram *cabinP);
void cabinPerformanceTestTask(const tsCabinProcConsts *consts, tsCabinProcParams *params);
void cabinComponentsTest(const tsCabinProcConsts *consts, tsCabinProcParams *params);
void cabinTestsFailedTask(const tsCabinProcConsts *consts, tsCabinProcParams *params);
//bool cabinIsHotStart(stCabin_Flash const *cabinC, stCabin_Ram *cabinP);

#endif

#ifndef COMPONENT_TEST_H
#define COMPONENT_TEST_H

#include "rcos.h"
#include "app/refrigerator/components_configs.h"

bool compTestDone(const tsDevIo *dev, const tsCompTestData *testData, uint32_t testTimer);
bool compTestWSize(const tsDevIo *dev, void *devRam, size_t devRamSize, const tsCompTestData *testData, uint32_t testTimer, uint32_t testValue);

#define compTest(_dev, _devRam, _testData, _testTimer, _testValue) \
		compTestWSize(_dev, _devRam, sizeof(*(_devRam)), _testData, _testTimer, _testValue)

#endif

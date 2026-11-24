#include "component_test.h"

/**
 * @brief Checks if production line test is done for a component
 *
 * @param dev pointer to the devIo of the component
 * @param testData pointer to test data struct that holds start & stop time
 * @param testTimer current test timer value (passed test time in 100ms)
 * @return true if component test is finished or component is not in use
 * @return false if component is used and its test isn't done yet
 */
bool compTestDone(const tsDevIo *dev, const tsCompTestData *testData, uint32_t testTimer)
{
	return (dev == NULL || testTimer >= testData->endTime);
}

/**
 * @brief Tests a component by writing a test value to its ram variable during
 * its test time and returns true if the test is done.
 *
 * @param dev pointer to the devIo of the component
 * @param devRam pointer to ram value that is attached to this device 
 * @param devRamSize size if the ram value in bytes
 * @param testData pointer to test data struct that holds start & stop time
 * @param testTimer current test timer value (passed test time in 100ms)
 * @param testValue value will be written devRam during component test 
 * @return true if component test is finished or component is not in use
 * @return false if component is used and its test isn't done yet
 */
bool compTestWSize(const tsDevIo *dev, void *devRam, size_t devRamSize,
				   const tsCompTestData *testData, uint32_t testTimer, uint32_t testValue)
{
	if (dev == NULL || testTimer >= testData->endTime)
		return true;
	if (testTimer >= testData->startTime)
	{
		switch (devRamSize)
		{
		case sizeof(uint8_t):
			*((uint8_t *)devRam) = testValue;
			break;
		case sizeof(uint16_t):
			*((uint16_t *)devRam) = testValue;
			break;
		case sizeof(uint32_t):
			*((uint32_t *)devRam) = testValue;
			break;
		}
	}
	return false;
}
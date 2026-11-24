#include "cooler_system_state.h"

static bool hasGasLevel(stGasSpecs const *gasSpecs);							  // state only
static bool isGasLevelRight(unsigned char gasLevel, stGasSpecs const *gasSpecs); // state only
static bool hasGasType(stGasSpecs const *gasSpecs);							  // state only
static bool isGasTypeRight(enGasType gasType, stGasSpecs const *gasSpecs);		  // state

static bool isStateEmpty(stCoolerSystemState const *systemState);					  // cooler system state
static bool isStateBroken(stCoolerSystemState const *systemState);					  // cooler system state
static bool isStateRight(stCooler_Ram *coolerP, stCoolerSystemState const *systemState); // cooler system state

unsigned char coolerGetSystemState(const stCooler_Flash *coolerC, stCooler_Ram *coolerP)
{
	stCoolerSystemState const *systemState;
	unsigned char state = 0;

	for (state = 1; state < _CoolerStateSize; state++)
	{
		systemState = &coolerC->parameters.sysStates[state];
		if (!isStateEmpty(systemState) &&
			!isStateBroken(systemState))
		{
			if (isStateRight(coolerP, systemState))
			{
				// stay in state ( normal state ) ;
				if (coolerC->parameters.sysStates[state].queue)
				{
					coolerP->parameters.queue = true;

					if (state > coolerP->parameters.queueState)
					{
						coolerP->parameters.queueState = state;
						break;
					}

					if (state == coolerP->parameters.queueState)
						break;
				}
				else
				{
					coolerP->parameters.queue = false;
					coolerP->parameters.queueState = 0;
					break;
				}
			}
		}
		else
		{
			// If state empty or broken go to default state;
			state = 0;
			coolerP->parameters.queue = false;
			coolerP->parameters.queueState = 0;
			break;
		}
	}

	if (state >= _CoolerStateSize)
	{
		// go to default state
		state = 0;
		coolerP->parameters.queue = false;
		coolerP->parameters.queueState = 0;
	}

	return state;
}

unsigned char coolerGetStatePriority(stCooler_Ram *coolerP, const stCoolerSystemState *systemState)
{
	stGasSpecs const *gasSpecs;
    unsigned char priority = 0;
    bool gasResult;

    for (unsigned char i = 0; i < _CoolerGasSize; i++)
    {
        gasResult = false;
        gasSpecs = &systemState->gasSpecs[i];
        if (hasGasType(gasSpecs))
        {
            if (isGasTypeRight((enGasType)coolerP->parameters.gasDesired[i].type, gasSpecs))
            {
                if (hasGasLevel(gasSpecs))
                {
                    if (isGasLevelRight(coolerP->parameters.gasDesired[i].level, gasSpecs))
                    {
                        gasResult = true;
                    }
                }
                else
                {
                    gasResult = true;
                }
            }
        }

        if (gasResult)
        {
            if (systemState->gasSpecs[i].priority)
            {
                if ((!systemState->gasSpecs[priority].priority) ||
                    (systemState->gasSpecs[i].priority > systemState->gasSpecs[priority].priority))
                    priority = i;
            }
        }
    }
    return priority;
}

static bool hasGasLevel(stGasSpecs const *gasSpecs)
{
	if (gasSpecs->lowLevel || gasSpecs->hghLevel)
        return true;
    else
        return false;
}

static bool isGasLevelRight(unsigned char gasLevel, stGasSpecs const *gasSpecs)
{
	bool level = false;

    if (gasSpecs->lowLevel && gasSpecs->hghLevel)
    {
        if (gasSpecs->lowLevel < gasSpecs->hghLevel)
        {
            if ((gasLevel >= gasSpecs->lowLevel) && (gasLevel <= gasSpecs->hghLevel))
                level = true;
        }
        else if (gasSpecs->lowLevel > gasSpecs->hghLevel)
        {
            if ((gasLevel >= gasSpecs->lowLevel) || (gasLevel <= gasSpecs->hghLevel))
                level = true;
        }
        else
        {
            if ((gasLevel == gasSpecs->lowLevel) || (gasLevel == gasSpecs->hghLevel))
                level = true;
        }
    }
    else if (gasSpecs->lowLevel)
    {
        if (gasLevel >= gasSpecs->lowLevel)
            level = true;
    }
    else if (gasSpecs->hghLevel)
    {
        if (gasLevel <= gasSpecs->lowLevel)
            level = true;
    }

    return level;
}

static bool hasGasType(stGasSpecs const *gasSpecs)
{
	if (gasSpecs->type != _GT_None)
        return true;
    else
        return false;
}

static bool isGasTypeRight(enGasType gasType, stGasSpecs const *gasSpecs)
{
	bool type = false;

    switch (gasSpecs->type)
    {
        case _GT_No:
        case _GT_Hot:
        case _GT_Failed:
        case _GT_Forced:
        case _GT_Cold_FF:
        case _GT_Cold_FRZ:
        case _GT_None:
        case _GT_HotStart:
        case _GT_PerformanceTest:   
        case _GT_PerformanceTest2:           
        case _GT_HotStart2:
            if (gasType == gasSpecs->type)
                type = true;
        break;

        case _GT_Cold:
            if ((gasType == gasSpecs->type) || (gasType == _GT_Cold_FF) || (gasType == _GT_Cold_FRZ))
                type = true;
        break;
    }

    return type;
}

static bool isStateEmpty(stCoolerSystemState const *systemState)
{
	// Check Logic
    switch (systemState->logic)
    {
        case _L_OR:
        case _L_AND:
            ;
        break;

        default:
            return true;
        // break; since state is unreachable
    }

    // Check Type
    for (unsigned char i = 0; i < _CoolerGasSize; i++)
    {
        if (systemState->gasSpecs[i].type)
            return false;
    }

    return true;
}

static bool isStateBroken(stCoolerSystemState const *systemState)
{
	switch (systemState->logic)
    {
        case _L_OR:
            for (unsigned char i = 0; i < _CoolerGasSize; i++)
            {
                if (systemState->gasSpecs[i].type &&
                    !systemState->gasSpecs[i].priority)
                    return true;
            }

            return false;

        case _L_AND:
            for (unsigned char i = 0; i < _CoolerGasSize; i++)
            {
                if (systemState->gasSpecs[i].type &&
                    systemState->gasSpecs[i].priority)
                    return false;
            }

            return true;

        default:
            return true;

    }
}

static bool isStateRight(stCooler_Ram *coolerP, stCoolerSystemState const *systemState)
{
	stGasSpecs const *gasSpecs;

    switch (systemState->logic)
    {
        case _L_OR:
            for (unsigned char i = 0; i < _CoolerGasSize; i++)
            {
                gasSpecs = &systemState->gasSpecs[i];

                if (hasGasType(gasSpecs))
                {
                    if (isGasTypeRight((enGasType)coolerP->parameters.gasDesired[i].type, gasSpecs))
                    {
                        if (hasGasLevel(gasSpecs))
                        {
                            if (isGasLevelRight(coolerP->parameters.gasDesired[i].level, gasSpecs))
                                return true;
                        }
                        else
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        case _L_AND:
            for (unsigned char i = 0; i < _CoolerGasSize; i++)
            {
                gasSpecs = &systemState->gasSpecs[i];

                if (hasGasType(gasSpecs))
                {
                    if (isGasTypeRight((enGasType)coolerP->parameters.gasDesired[i].type, gasSpecs))
                    {
                        if (hasGasLevel(gasSpecs))
                        {
                            if (!isGasLevelRight(coolerP->parameters.gasDesired[i].level, gasSpecs))
                                return false;
                        }
                    }
                    else
                        return false;
                }
            }
            return true;

        default:
            return false;
    }
}
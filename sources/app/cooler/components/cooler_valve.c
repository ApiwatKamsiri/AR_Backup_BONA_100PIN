#include "cooler_valve.h"

static bool isValvePositionChanged(stCooler_Ram *coolerP);

void coolerInitValveState(stCooler_Ram *coolerP)
{
	coolerP->parameters.valveDefaultPosition = true;
	coolerP->parameters.valvePrevState = 0xff; // dummy value
	coolerP->parameters.valvePrevPosition = 0xff;
}

void coolerUpdateValveState(const stCooler_Flash *coolerC, stCooler_Ram *coolerP, 
								unsigned char state,
								unsigned char gasType,
								unsigned char gasLevel)
{
	unsigned char gas;
	unsigned char enough;

	// Solenoid Valve
	if (state != coolerP->parameters.valvePrevState)
	{
		coolerP->parameters.valveDefaultPosition = true;
	}
	else
	{
		if (coolerC->parameters.sysStates[state].valvesAltPositionTime)
		{
			if (!coolerP->parameters.solenoidValvePositionTimer)
			{
				if (coolerP->parameters.valveDefaultPosition)
				{
					coolerP->parameters.solenoidValvePositionTimer =
						coolerC->parameters.sysStates[state].valvesAltPositionTime;
					coolerP->parameters.valveDefaultPosition = false;
				}
				else
				{
					coolerP->parameters.solenoidValvePositionTimer =
						coolerC->parameters.sysStates[state].valvesDefPositionTime;
					coolerP->parameters.valveDefaultPosition = true;
				}
			}
		}
	}

	if (coolerP->parameters.valveDefaultPosition)
	{
		coolerP->components.solenoidValve[0].positionIndex =
			coolerC->parameters.valveGasStates[coolerC->parameters.sysStates[state].valvesDefPosition].valve0Position;
		coolerP->components.solenoidValve[1].positionIndex =
			coolerC->parameters.valveGasStates[coolerC->parameters.sysStates[state].valvesDefPosition].valve1Position;
		gas = coolerC->parameters.valveGasStates[coolerC->parameters.sysStates[state].valvesDefPosition].gas;
		enough = coolerC->parameters.valveGasStates[coolerC->parameters.sysStates[state].valvesDefPosition].enough;
	}
	else
	{
		coolerP->components.solenoidValve[0].positionIndex =
			coolerC->parameters.valveGasStates[coolerC->parameters.sysStates[state].valvesAltPosition].valve0Position;
		coolerP->components.solenoidValve[1].positionIndex =
			coolerC->parameters.valveGasStates[coolerC->parameters.sysStates[state].valvesAltPosition].valve1Position;
		gas = coolerC->parameters.valveGasStates[coolerC->parameters.sysStates[state].valvesAltPosition].gas;
		enough = coolerC->parameters.valveGasStates[coolerC->parameters.sysStates[state].valvesAltPosition].enough;
	}

	coolerP->components.solenoidValve[0].testMode = 0;
	coolerP->components.solenoidValve[1].testMode = 0;

	coolerP->parameters.valvePositionChanged = isValvePositionChanged(coolerP);

	if (state != coolerP->parameters.valvePrevState)
	{
		if (!coolerP->parameters.valvePositionChanged)
		{
			if (coolerP->parameters.valveStateStayTimer < coolerC->parameters.sysStates[state].valvesDefPositionTime)
				coolerP->parameters.solenoidValvePositionTimer =
					coolerC->parameters.sysStates[state].valvesDefPositionTime - coolerP->parameters.valveStateStayTimer;
			else
				coolerP->parameters.solenoidValvePositionTimer = 0;
		}
		else
			coolerP->parameters.solenoidValvePositionTimer =
				coolerC->parameters.sysStates[state].valvesDefPositionTime;

		coolerP->parameters.valvePrevState = state;
		coolerP->parameters.valveStateStayTimer = 0;
	}

	for (unsigned char i = 0; i < 5; i++)
	{
		if (gas & (0x01 << i))
		{
			coolerP->parameters.gasApplied[i].type = gasType;
                        
                        if(!coolerP->parameters.valvePositionChanged)
                          coolerP->parameters.gasAppliedPrev[i].type = gasType;

			if (enough & (0x01 << i))
			{
				coolerP->parameters.gasApplied[i].level = gasLevel;
                                
                                if(!coolerP->parameters.valvePositionChanged)
                                  coolerP->parameters.gasAppliedPrev[i].level = gasLevel;
			}
			else
			{
				coolerP->parameters.gasApplied[i].level = 0;
			}
		}
		else
		{
			coolerP->parameters.gasApplied[i].type = _GT_No;
			coolerP->parameters.gasApplied[i].level = 0;
		}
	}
}

static bool isValvePositionChanged(stCooler_Ram *coolerP)
{
	if (coolerP->parameters.valvePrevPosition != coolerP->components.solenoidValve[0].positionIndex)
	{
		coolerP->parameters.valvePrevPosition = coolerP->components.solenoidValve[0].positionIndex;
		coolerP->parameters.gasLevelStayTimer = 0;
		coolerP->parameters.valveStateStayTimer = 0;

		return true;
	}
	else
		return false;
}
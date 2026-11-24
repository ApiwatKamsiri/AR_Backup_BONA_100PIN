#include "cooler_cond_fan.h"
#include "cooler_valve.h"

void coolerUpdateCondFanSpeed(const stCooler_Flash *coolerC, stCooler_Ram *coolerP, unsigned char level)
{
	unsigned short speed = 0;

	enAmbientType ambientType = coolerP->parameters.ambientType;

	if (coolerP->components.compressor.value)
	{
		if (!coolerP->parameters.condenserFanSpeedChecked &&
			(coolerP->parameters.compressorOnTimer >=
			 coolerC->parameters.condenserFan.speedCheckTime))
		{
			coolerP->parameters.condenserFanSpeedChecked = true;
		}

		if (coolerP->parameters.ambientType == _Ambient_NONE)
		{
			if (coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
				speed = (unsigned short)((signed short)coolerC->parameters.condenserFan.speedLevelFF[level - 1] +
									   coolerC->parameters.condenserFan.speedCheckCorrection);
			else if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
				speed = (unsigned short)((signed short)coolerC->parameters.condenserFan.speedLevelFRZ[level - 1] +
									   coolerC->parameters.condenserFan.speedCheckCorrection);
			else
				speed = (unsigned short)((signed short)coolerC->parameters.condenserFan.speedLevelFF[level - 1] +
									   coolerC->parameters.condenserFan.speedCheckCorrection);
		}
		else
		{
			if (coolerP->components.solenoidValve[0].positionIndex == _FF_OPEN)
				speed = (unsigned short)((signed short)coolerC->parameters.condenserFan.FF.speedLevel[ambientType][level - 1] +
									   coolerC->parameters.condenserFan.speedCheckCorrection);
			else if (coolerP->components.solenoidValve[0].positionIndex == _FRZ_OPEN)
				speed = (unsigned short)((signed short)coolerC->parameters.condenserFan.FRZ.speedLevel[ambientType][level - 1] +
									   coolerC->parameters.condenserFan.speedCheckCorrection);
			else
				speed = (unsigned short)((signed short)coolerC->parameters.condenserFan.FF.speedLevel[ambientType][level - 1] +
									   coolerC->parameters.condenserFan.speedCheckCorrection);
		}
	}
	else
	{
		coolerP->parameters.condenserFanSpeedChecked = false;
		coolerP->parameters.condenserFanSpeedChanged = false;
	}

	coolerP->components.condenserFan.speedOrPWM = speed;
}
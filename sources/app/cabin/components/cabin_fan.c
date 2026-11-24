#include "cabin_fan.h"
#include "app/cabin/procedures/cabin_fan_level.h"

void cabinFanControl(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
	const stCabin_Flash *cabinC = consts->cabin;
	stCabin_Ram *cabinP = params->cabin;

	enAmbientType ambientType = cabinP->values.ambientType;
      
        
        /*** APW : Add CT heater software ***/
        if (params->rf->Cabin[0].states.Work == _Work_CommercialTestMode ||
            params->rf->Cabin[1].states.Work == _Work_CommercialTestMode)
        {
          return;
        }
        /*** APW : Add CT heater software ***/
        
        
	if (params->rf->Cabin[0].states.Work == _Work_PumpOut)
	{
		if (params->rf->Cabin[1].states.Work != _Work_Defrost)
			cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.SpeedPumpOut;
		return;
	}

	if ((!cabinC->components.fan.type) ||
		((cabinP->values.doorClosedTimer < _Fan_ONDelayTime) && (!cabinP->sets.work.functions.Sabbath)) ||
		isFanOff(cabinP->states.Fan) ||
		isFanNone(cabinP->states.Fan))
	{
		cabinP->components.fan.speedOrPWM = 0;
		cabinClearFanLevel(consts, params);
	}
	else if (cabinP->states.Fan == _FanLevel_FRZ_ConvertibleMode)
	{
		cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.SpeedConvertible;
	}
	else if (isFanLevel(cabinP->states.Fan))
	{
		if (cabinP->values.ambientType == _Ambient_NONE)
			cabinUpdateFanLevel(consts, params);
		else
			cabinUpdateFanLevelAmbient(consts, params);

		if (cabinP->values.fanLevel)
		{
			if (cabinP->values.ambientType == _Ambient_NONE)
			{
				cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.SpeedLevel[cabinP->values.fanLevel - 1];
			}
			else
			{
				cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.AmbientSpeedLevel[ambientType][cabinP->values.fanLevel - 1];
			}
                        if (cabinP->states.Fan == _FanLevel_FF_AtCompressorONFRZAmbientLevel || cabinP->states.Fan == _FanLevel_FRZ_AtCompressorONFFAmbientLevel)
                        {
                          cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.SpeedIdle;
                        }
		}
		else
		{
			cabinP->components.fan.speedOrPWM = 0;
		}
	}
	else if (isFanHotStart(cabinP->states.Fan))
	{
		if (cabinP->values.ambientType == _Ambient_NONE)
			cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.SpeedHotStart;
		else
			cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.AmbientSpeedLevel[ambientType][_compressorHotStartLevel];
		cabinClearFanLevel(consts, params);
	}
	else if (isFanError(cabinP->states.Fan))
	{
		cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.SpeedError;
		cabinClearFanLevel(consts, params);
	}
	else if (isFanQuick(cabinP->states.Fan))
	{
		cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.SpeedQuick;
		cabinClearFanLevel(consts, params);
	}
	else
	{
		cabinP->components.fan.speedOrPWM = 0;
		cabinClearFanLevel(consts, params);
	}

	if (cabinP->sets.work.functions.Silent)
	{
		if (cabinP->components.fan.speedOrPWM > (cabinC->parameters.Fan.SpeedSilent))
			cabinP->components.fan.speedOrPWM = cabinC->parameters.Fan.SpeedSilent;
	}
}
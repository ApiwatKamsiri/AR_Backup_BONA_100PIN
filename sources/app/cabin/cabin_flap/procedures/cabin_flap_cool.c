#include "cabin_flap_cool.h"

enCoolStates cabinFlapCoolIn(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;
    
    if (cabinP->sets.work.functions.Sabbath)
    {
        if (cabinP->stateTimers.Idle >=
            cabinP->values.duty.meanIdleTime)
            return _CoolIn_FLAP_MeanIdleTimeElapsed;
        if (cabinP->stateTimers.Idle >=
            cabinC->parameters.Cool.SabbathMaxIdleTime)
            return _CoolIn_FLAP_SabbathMaxIdleTimeElapsed;
        return _CoolNone_FLAP;
    }

    switch (cabinP->states.Sensor)
    {
        case _Sensor_AirP:
#ifdef HOSHI_H30
            if ((params->rf->Cabin[0].components.airTempSensor.valueInCExt >= params->rf->Cabin[0].sets.datas.coolCutInTemp)            
               &&(params->rf->Cabin[1].components.airTempSensor.valueInCExt >= params->rf->Cabin[1].sets.datas.coolCutInTemp))          //HOSHI Damper Open when FRZ Hot and FF Cold
#else
            if (cabinP->components.airTempSensor.valueInCExt >=
                cabinP->sets.datas.coolCutInTemp)
#endif	//HOSHI_H30
                return _CoolIn_FLAP_AirHigherThanCoolCutIn;
            if (cabinP->stateTimers.Idle >=
                cabinC->parameters.Cool.MaxIdleTime)
                return _CoolIn_FLAP_MaxIdleTimeElapsed;
        break;
        case _Sensor_AirF:
            if (cabinP->stateTimers.Idle >=
                cabinC->parameters.Cool.ErrorMaxIdleTime)
                return _CoolIn_FLAP_ErrorMaxIdleTimeElapsed;
        break;
    }
    return _CoolNone_FLAP;
}

enCoolStates cabinFlapCoolOut(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    switch (cabinP->states.Sensor)
    {
        case _Sensor_AirP:
#ifdef HOSHI_H30
            if ((params->rf->Cabin[0].components.airTempSensor.valueInCExt <= params->rf->Cabin[0].sets.datas.coolCutOutTemp)           
              || (params->rf->Cabin[1].components.airTempSensor.valueInCExt <= params->rf->Cabin[1].sets.datas.coolCutOutTemp))         //HOSHI Damper Close when FRZ Cold
#else
            if (cabinP->components.airTempSensor.valueInCExt <=
                cabinP->sets.datas.coolCutOutTemp)
#endif	//HOSHI_H30
                return _CoolOut_FLAP_AirLowerThanCoolCutOut;
            if ((cabinP->sets.work.Mode == _CabinMode_FF) &&
                (cabinP->values.quickFRZ) &&
                (cabinP->components.airTempSensor.valueInCExt <=
                cabinC->parameters.Cool.QuickFRZCutOutTemp))
                return _CoolOut_FLAP_AirIsTooColdAtQuickFRZ;
        break;

        case _Sensor_AirF:
            if (cabinP->sets.work.functions.Sabbath)
            {
                if (cabinP->stateTimers.Cool >=
                    cabinP->values.duty.meanCoolTime)
                    return _CoolOut_FLAP_MeanCoolTimeElapsed;
            }
            if (cabinP->stateTimers.Cool >=
                cabinC->parameters.Cool.ErrorMaxCoolTime)
                return _CoolOut_FLAP_ErrorMaxCoolTimeElapsed;
        break;
    }
    return _CoolNone_FLAP;
}
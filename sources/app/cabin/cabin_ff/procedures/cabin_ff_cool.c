#include "cabin_ff_cool.h"
#include "../../defaults/cabin_defaults_cool.h"

enCoolStates cabinFfCoolIn(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    if (cabinP->sets.work.functions.Sabbath)
    {
        if (cabinP->stateTimers.Idle >=
            cabinP->values.duty.meanIdleTime)
            return _CoolIn_FF_MeanIdleTimeElapsed;
        if (cabinP->stateTimers.Idle >=
            cabinC->parameters.Cool.SabbathMaxIdleTime)
            return _CoolIn_FF_SabbathMaxIdleTimeElapsed;
        return _CoolNone_FF;
    }

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirP | _Sensor_EvaP:

        if (cabinP->values.quickFRZ &&
            (cabinP->components.airTempSensor.valueInCExt <=
             cabinC->parameters.Cool.QuickFRZCutOutTemp))
            return _CoolNone_FF;

        if (cabinC->parameters.Cool.isEvaCutInActive)
        {
            if (cabinP->components.airTempSensor.valueInCExt >= cabinP->sets.datas.coolCutInTemp &&
                cabinP->components.evaTempSensor.valueInCExt >= cabinC->parameters.Cool.EvaCutInTemp &&
                cabinP->components.evaTempSensor.valueInCExt >= _Cool_FFAirEva_CutInTemp)
                return _CoolIn_FF_AirHigherThanCoolCutIn;
        }
        else
        {
            if (cabinP->components.airTempSensor.valueInCExt >= cabinP->sets.datas.coolCutInTemp)
                return _CoolIn_FF_AirHigherThanCoolCutIn;
        }

        if (cabinP->stateTimers.Idle >= cabinC->parameters.Cool.MaxIdleTime)
            return _CoolIn_FF_MaxIdleTimeElapsed;
        break;

    case _Sensor_AirP | _Sensor_EvaF:

        if (cabinP->values.quickFRZ &&
            (cabinP->components.airTempSensor.valueInCExt <=
             cabinC->parameters.Cool.QuickFRZCutOutTemp))
            return _CoolNone_FF;

        if (cabinP->components.airTempSensor.valueInCExt >= cabinP->sets.datas.coolCutInTemp)
            return _CoolIn_FF_AirHigherThanCoolCutIn;

        if (cabinP->stateTimers.Idle >= cabinC->parameters.Cool.MaxIdleTime)
            return _CoolIn_FF_MaxIdleTimeElapsed;
        break;

    case _Sensor_AirF:
    case _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        if (cabinP->stateTimers.Idle >=
            cabinC->parameters.Cool.ErrorMaxIdleTime)
            return _CoolIn_FF_ErrorMaxIdleTimeElapsed;
        break;

    case _Sensor_EvaP:
        if (cabinP->components.evaTempSensor.valueInCExt >=
            cabinP->sets.datas.coolCutInTemp)
            return _CoolIn_FF_EvaHigherThanCoolCutIn;

        if (cabinP->stateTimers.Idle >=
            cabinC->parameters.Cool.MaxIdleTime)
            return _CoolIn_FF_MaxIdleTimeElapsed;
        /*
        if( (cabinP->stateTimers.Defrost + cabinP->stateTimers.Idle) >=
        cabinC->parameters.Cool.MaxIdleTime )
        return _CoolIn_FF_MaxIdleTimeElapsed;
        */
        break;

    case _Sensor_AirF | _Sensor_EvaP:
        if (cabinP->components.evaTempSensor.valueInCExt >=
            cabinC->parameters.Cool.ErrorCutInTemp)
            return _CoolIn_FF_EvaHigherThanErrorCutInTemp;
        break;
    }
    return _CoolNone_FF;
}

enCoolStates cabinFfCoolOut(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    if ((cabinP->states.Cool == _CoolIn_FF_MaxIdleTimeElapsed) &&
        (cabinP->stateTimers.Cool < cabinC->parameters.Cool.MaxIdleTimeElapsedCoolTime))
        return _CoolNone_FF;

    switch (cabinP->states.Sensor)
    {
    case _Sensor_AirP:
    case _Sensor_AirP | _Sensor_EvaP:
    case _Sensor_AirP | _Sensor_EvaF:
        if (cabinP->values.quickFRZ &&
            (cabinP->components.airTempSensor.valueInCExt <=
             cabinC->parameters.Cool.QuickFRZCutOutTemp))
            return _CoolOut_FF_AirIsTooColdAtQuickFRZ; // priority is high...

        /* ozgur - there is a problem about quick ice logic.
        if(cabinP->values.quickIce){
        if(params->rf->Cooler.components.compressor.value &&
            params->rf->Cabin[0].values.quickIce &&
            cabinP->components.airTempSensor.valueInCExt <= cabinC->parameters.Performance.thermalCutOut )
        {
            params->rf->Values.quickIceTermalCutOutTimer = consts->rf->Parameters.quickIceTermalCutOutTimer; // 40; ozgur
            params->rf->Cooler.parameters.compressorOffTimer = 0;
            return _CoolOut_FF_AirLowerThanThermalCutOut;
        }
        }
        if( params->rf->Cooler.components.compressor.value &&
            cabinP->values.quickFRZ &&
            cabinP->components.airTempSensor.valueInCExt <= cabinC->parameters.Cool.QuickFRZCutOutTemp )
        {
        params->rf->Values.quickFrzTermalCutOutTimer = consts->rf->Parameters.quickFrzTermalCutOutTimer;    // 40; ozgur
        params->rf->Cooler.parameters.compressorOffTimer = 0;
        return _CoolOut_FF_AirIsTooColdAtQuickFRZ; // priority is high...
        }
        */
        if (cabinP->components.airTempSensor.valueInCExt <=
            cabinP->sets.datas.coolCutOutTemp)
            return _CoolOut_FF_AirLowerThanCoolCutOut;
        break;

    case _Sensor_AirF:
    case _Sensor_EvaF:
    case _Sensor_AirF | _Sensor_EvaF:
        if (cabinP->sets.work.functions.Sabbath)
        {
            if (cabinP->stateTimers.Cool >=
                cabinP->values.duty.meanCoolTime)
                return _CoolOut_FF_MeanCoolTimeElapsed;
        }

        if (cabinP->stateTimers.Cool >=
            cabinC->parameters.Cool.ErrorMaxCoolTime)
            return _CoolOut_FF_ErrorMaxCoolTimeElapsed;
        break;

    case _Sensor_EvaP:
        if (cabinP->components.evaTempSensor.valueInCExt <=
            cabinP->sets.datas.coolCutOutTemp)
            return _CoolOut_FF_EvaLowerThanCoolCutOut;
        break;

    case _Sensor_AirF | _Sensor_EvaP:
        if (cabinP->components.evaTempSensor.valueInCExt <=
            cabinC->parameters.Cool.ErrorCutOutTemp)
            return _CoolOut_FF_EvaLowerThanErrorCutOutTemp;
        break;
    }
    return _CoolNone_FF;
}

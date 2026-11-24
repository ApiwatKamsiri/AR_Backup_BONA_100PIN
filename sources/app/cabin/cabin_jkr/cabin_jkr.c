#ifdef JKRavailable

#include "cabin_jkr.h"
#include "components/cabin_jkr_d_heater.h"
#include "components/cabin_jkr_lt_heater.h"
#include "components/cabin_jkr_fan.h"
#include "procedures/cabin_jkr_cool.h"
#include "procedures/cabin_jkr_defrost.h"
#include "app/cabin/procedures/cabin_duty.h"
#include "app/cabin/procedures/cabin_errors.h"
#include "app/cabin/procedures/cabin_defrost.h"
#include "app/cabin/procedures/cabin_drainage.h"
#include "app/cabin/procedures/cabin_gas_level.h"
#include "app/cabin/components/cabin_c_heater.h"
#include "app/cabin/components/cabin_comp_flap.h"
#include "app/cabin/components/cabin_c_heater.h"

static void cabinJkrUpdateSensorState(stCabin_Flash const *cabinC, stCabin_Ram *cabinP);

void cabinJkrInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    cabinP->states.Cool = _CoolNone_JKR;
    cabinP->states.Defrost = _DefrostNone_JKR;
    cabinP->states.Fan = _FanNone_JKR;
    cabinP->states.Flap = _FlapNone_ALL;
    cabinP->states.HeaterA = _HeaterANone_JKRLowTemp;
    cabinP->states.HeaterB = _HeaterBNone_JKRDefrost;
    cabinP->states.HeaterC = _HeaterCNone_ALL;
    cabinP->states.Work = _Work_Idle;
    cabinP->values.gasDesired.type = _GT_No;
    cabinP->values.gasDesired.level = 0;
}

void cabinJkrTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    enCoolStates CoolState;
    enDefrostStates DefrostState;
    enFanStates FanState;
    enFlapStates FlapState;
    enHeaterAStates HeaterAState;
    enHeaterBStates HeaterBState;
    enHeaterCStates HeaterCState;

    cabinJkrUpdateSensorState(cabinC, cabinP);
    cabinJkrDefrost(consts, params);
    switch (cabinP->states.Work)
    {
    case _Work_Idle:
        CoolState = cabinJkrCoolIn(consts, params);
        DefrostState = cabinJkrDefrostIn(consts, params);
        FanState = cabinJkrFanIdle(consts, params);
        HeaterAState = cabinJkrLTHeaterIdle(consts, params);
        HeaterBState = cabinJkrDHeaterIdle(consts, params);
        HeaterCState = cabinCHeaterIdle(consts, params);

        if (isDefrostIn(DefrostState))
        {
            cabinP->states.Defrost = DefrostState;
            cabinP->values.gasDesired.type = _GT_Hot;
            cabinP->values.gasDesired.level = 5; // dummy value
        }
        else if (isCoolIn(CoolState))
        {
            cabinP->states.Cool = CoolState;
            switch (cabinP->sets.user.Mode)
            {
            case _CabinMode_FF:
                cabinP->values.gasDesired.type = _GT_Cold_FF;
                break;
            case _CabinMode_FRZ:
                cabinP->values.gasDesired.type = _GT_Cold_FRZ;
                break;
            default:
                cabinP->values.gasDesired.type = _GT_Cold;
                break;
            }
            cabinUpdateGasLevel(consts, params);
        }
        else
        {
            cabinP->values.gasDesired.type = _GT_No;
            cabinP->values.gasDesired.level = 0;
        }

        if (cabinP->values.gasApplied.type == _GT_Hot)
        {
            cabinP->states.Work = _Work_Defrost;
        }
        else if ((cabinP->values.gasApplied.type == _GT_Cold) &&
                 (cabinP->values.gasApplied.level))
        {
            cabinP->states.Work = _Work_Cool;
        }
        else
        {
            if (isDefrostIn(DefrostState))
                cabinP->states.Work = _Work_Defrost;
        }
        break;

    case _Work_Cool:
    case _Work_CoolWait:
        CoolState = cabinJkrCoolOut(consts, params);
        DefrostState = cabinJkrDefrostIn(consts, params);

        if (cabinP->values.gasApplied.type == _GT_Cold)
            cabinP->states.Work = _Work_Cool;

        if (cabinP->states.Work == _Work_Cool)
        {
            FanState = cabinJkrFanCool(consts, params);
            HeaterAState = cabinJkrLTHeaterCool(consts, params);
            HeaterBState = cabinJkrDHeaterCool(consts, params);
            HeaterCState = cabinCHeaterCool(consts, params);
        }
        else
        { // _Work_CoolWait
            FanState = cabinJkrFanIdle(consts, params);
            HeaterAState = cabinJkrLTHeaterIdle(consts, params);
            HeaterBState = cabinJkrDHeaterIdle(consts, params);
            HeaterCState = cabinCHeaterIdle(consts, params);
        }

        if (isDefrostIn(DefrostState))
        {
            cabinP->states.Defrost = DefrostState;
            cabinP->values.gasDesired.type = _GT_Hot;
            cabinP->values.gasDesired.level = 5; // dummy value
        }
        else if (isCoolOut(CoolState))
        {
            cabinP->states.Cool = CoolState;
            cabinP->values.gasDesired.type = _GT_No;
            cabinP->values.gasDesired.level = 0;
        }
        else
        {
            switch (cabinP->sets.user.Mode)
            {
            case _CabinMode_FF:
                cabinP->values.gasDesired.type = _GT_Cold_FF;
                break;

            case _CabinMode_FRZ:
                cabinP->values.gasDesired.type = _GT_Cold_FRZ;
                break;

            default:
                cabinP->values.gasDesired.type = _GT_Cold;
                break;
            }
            cabinUpdateGasLevel(consts, params);
        }

        if (cabinP->values.gasApplied.type == _GT_Hot)
        {
            cabinP->states.Work = _Work_Defrost;
            cabinDutyTask(consts, params);
        }
        else if (cabinP->values.gasApplied.type == _GT_No)
        {
            if (isDefrostIn(DefrostState))
            {
                cabinP->states.Work = _Work_Defrost;
            }
            else if (isCoolOut(CoolState))
            {
                cabinP->states.Work = _Work_Idle;
            }
            else
            {
                cabinP->states.Work = _Work_CoolWait;
            }

            if (cabinP->states.Work != _Work_CoolWait)
                cabinDutyTask(consts, params);
        }
        break;

    case _Work_Defrost:
        if (cabinP->states.Defrost == _DefrostIn_JKR_MaxQuickTimeElapsed)
            cabinP->values.quickDefrost = true;

        DefrostState = cabinJkrDefrostOut(consts, params);
        FanState = cabinJkrFanDefrost(consts, params);
        HeaterAState = cabinJkrLTHeaterDefrost(consts, params);
        HeaterBState = cabinJkrDHeaterDefrost(consts, params);
        HeaterCState = cabinCHeaterDefrost(consts, params);
        CoolState = cabinJkrCoolOut(consts, params);

        if (isDefrostOut(DefrostState))
        {
            cabinP->states.Defrost = DefrostState;
            if (CoolState == _CoolOut_JKR_AirIsTooColdAtQuickFRZ)
            {
                cabinP->values.gasDesired.type = _GT_Forced;
                cabinP->values.gasDesired.level = 0;
            }
            else
            {
                cabinP->values.gasDesired.type = _GT_No;
                cabinP->values.gasDesired.level = 0;
            }
        }
        else
        {
            cabinP->values.gasDesired.type = _GT_Hot;
            cabinP->values.gasDesired.level = 5; // dummy value
        }

        if (cabinP->values.gasApplied.type == _GT_Cold)
        {
            cabinP->states.Work = _Work_Cool;
        }
        else if (cabinP->values.gasApplied.type == _GT_No)
        {
            if (isDefrostOut(DefrostState))
            {
                if (DefrostState == _DefrostOut_JKR_MaxDefrostTimeElapsed_DO)
                {
                    cabinIncDefrostHeaterErrCounter(consts, params);
                }
                else
                    cabinClearDefrostHeaterErrCounter(consts, params);

                cabinP->states.Work = _Work_Drainage;
            }
        }

        if (cabinP->states.Work != _Work_Defrost)
        {
            if (DefrostState != _DefrostOut_JKR_ForIceCream)
            {
                /*cabinP->values.defrostDelayTimerForIceCream =
                    cabinC->parameters.Defrost.MaxDelayTimeForIceCream;*/
                cabinP->values.cycleCounter = 0;
                cabinP->values.workTimer = 0;
                cabinP->values.realTimer = 0;
                cabinP->values.duty.delay = 2;
                // cabinP->stateTimers.Defrost = 0;
                cabinDefrostMeanTimeCTRL(consts, params);
                cabinP->stateTimers.Drainage = 0;
            }
        }
        break;

    case _Work_Drainage:
        CoolState = cabinJkrCoolIn(consts, params);
        FanState = cabinJkrFanDrainage(consts, params);
        HeaterAState = cabinJkrLTHeaterDrainage(consts, params);
        HeaterBState = cabinJkrDHeaterDrainage(consts, params);
        HeaterCState = cabinCHeaterDrainage(consts, params);

        if (cabinDrainageOut(consts, params))
        {
            if (isCoolIn(CoolState))
            {
                cabinP->states.Cool = CoolState;
                switch (cabinP->sets.user.Mode)
                {
                case _CabinMode_FF:
                    cabinP->values.gasDesired.type = _GT_Cold_FF;
                    break;
                case _CabinMode_FRZ:
                    cabinP->values.gasDesired.type = _GT_Cold_FRZ;
                    break;
                default:
                    cabinP->values.gasDesired.type = _GT_Cold;
                    break;
                }
                cabinUpdateGasLevel(consts, params);
            }
            else
            {
                cabinP->values.gasDesired.type = _GT_No;
                cabinP->values.gasDesired.level = 0;
            }
        }
        else
        {
            cabinP->values.gasDesired.type = _GT_Forced;
            cabinP->values.gasDesired.level = 0;
        }

        if (cabinP->values.gasApplied.type == _GT_Hot)
        {
            cabinP->states.Work = _Work_Defrost;
        }
        else if (cabinP->values.gasApplied.type == _GT_Cold)
        {
            cabinP->states.Work = _Work_Cool;
        }
        else
        {
            if (cabinDrainageOut(consts, params))
            {
                cabinP->states.Work = _Work_Idle;
            }
        }

        /*if (cabinP->states.Work != _Work_Drainage)
        {
            if (!cabinP->values.quickDefrost)
                cabinP->values.quickTimer = 0;
        }*/
        break;

    case _Work_HotStart:
        FanState = _FanHotStart_JKR;
        HeaterAState = _HeaterAOff_HotStart;
        HeaterBState = _HeaterBOff_HotStart;
        HeaterCState = _HeaterCOff_HotStart;

        cabinP->values.gasDesired.type = _GT_HotStart;
        cabinP->values.gasDesired.level = 5;
        if (cabinP->stateTimers.HotStart >= 7)
            cabinP->values.gasDesired.level = 7;

        if (cabinP->stateTimers.HotStart > 70)
        {
            cabinP->states.Work = _Work_Cool;
        }
        break;

    default:
        cabinP->states.Work = _Work_Idle;
        break;
    }
    FlapState = cabinFlapState(consts, params);
    if (!isFanNone(FanState))
        cabinP->states.Fan = FanState;
    if (!isFlapNone(FlapState))
      cabinP->states.Flap = FlapState;
    if (!isHeaterANone(HeaterAState))
        cabinP->states.HeaterA = HeaterAState;
    if (!isHeaterBNone(HeaterBState))
        cabinP->states.HeaterB = HeaterBState;
    if (!isHeaterCNone(HeaterCState))
        cabinP->states.HeaterC = HeaterCState;
}

static void cabinJkrUpdateSensorState(stCabin_Flash const *cabinC, stCabin_Ram *cabinP)
{
    cabinP->states.Sensor = (enSensorStates)0;
    if (cabinC->components.airTempSensor.type != _TempSensorType_NONE)
    {
        if (cabinP->components.airTempSensor.error)
            cabinP->states.Sensor |= _Sensor_AirF;
        else
            cabinP->states.Sensor |= _Sensor_AirP;
    }

    if (cabinC->components.evaTempSensor.type != _TempSensorType_NONE)
    {
        if (cabinP->components.evaTempSensor.error)
            cabinP->states.Sensor |= _Sensor_EvaF;
        else
            cabinP->states.Sensor |= _Sensor_EvaP;
    }
}
#endif
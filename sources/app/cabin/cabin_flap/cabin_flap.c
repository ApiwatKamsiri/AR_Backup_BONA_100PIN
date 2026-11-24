#include "cabin_flap.h"
#include "app/cabin/procedures/cabin_errors.h"
#include "components/cabin_flap_chl_heater.h"
#include "components/cabin_flap_fan.h"
#include "components/cabin_flap_flap_heater.h"
#include "components/cabin_flap_flap.h"
#include "components/cabin_flap_lt_heater.h"
#include "procedures/cabin_flap_cool.h"
#include "app/cabin/procedures/cabin_gas_level.h"
#include "app/cabin/procedures/cabin_duty.h"

static void cabinFlapUpdateSensorState(stCabin_Flash const *cabinC, stCabin_Ram *cabinP);

void cabinFlapInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;
    UNUSED(cabinC);

    cabinP->states.Cool = _CoolNone_FLAP;
    cabinP->states.Defrost = _DefrostNone_FLAP;
    cabinP->states.Fan = _FanNone_FLAP;
    cabinP->states.Flap = _FlapNone_FLAP;
    cabinP->states.HeaterA = _HeaterANone_FLAPLowTemp;
    cabinP->states.HeaterB = _HeaterBNone_FLAPFlap;
    cabinP->states.HeaterC = _HeaterCNone_FLAPChannel;
    cabinP->states.Work = _Work_Idle;
    cabinP->values.gasDesired.type = _GT_No;
    cabinP->values.gasDesired.level = 0;
}

void cabinFlapTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;

    enCoolStates CoolState;
    //enDefrostStates DefrostState;
    enFanStates FanState;
    enFlapStates FlapState;
    enHeaterAStates HeaterAState;
    enHeaterBStates HeaterBState;
    enHeaterCStates HeaterCState;

    cabinFlapUpdateSensorState(cabinC, cabinP);
    cabinP->states.Defrost = _DefrostNone_FLAP;
    cabinClearDefrostHeaterErrCounter(consts, params);

    switch (cabinP->states.Work)
    {
    case _Work_Idle:
        CoolState = cabinFlapCoolIn(consts, params);
        FanState = cabinFlapFanIdle(consts, params);
        FlapState = cabinFlapFlapIdle(consts, params);
        HeaterAState = cabinFlapLTHeaterIdle(consts, params);
        HeaterBState = cabinFlapFlapHeaterIdle(consts, params);
        HeaterCState = cabinFlapCHLHeaterIdle(consts, params);

        if (isCoolIn(CoolState))
        {
            cabinP->states.Cool = CoolState;
            cabinP->values.gasDesired.type = _GT_Cold_FF;
#ifdef HOSHI_H30
            if (cabinP->values.ambientType == _Ambient_NONE)
              cabinUpdateGasLevel(consts, params);
            else
              cabinUpdateGasLevelAmbient(consts, params);
#else
            cabinUpdateGasLevel(consts, params);
#endif //HOSHI_H30      
        }
        else
        {
            cabinP->values.gasDesired.type = _GT_No;
            cabinP->values.gasDesired.level = 0;
        }

        if (cabinP->values.gasApplied.type == _GT_Cold)
        {
            cabinP->states.Work = _Work_Cool;
        }
        break;

    case _Work_Cool:
    case _Work_CoolWait:
        CoolState = cabinFlapCoolOut(consts, params);

        if (cabinP->values.gasApplied.type == _GT_Cold)
            cabinP->states.Work = _Work_Cool;

        if (cabinP->states.Work == _Work_Cool)
        {
            FanState = cabinFlapFanCool(consts, params);
            FlapState = cabinFlapFlapCool(consts, params);
            HeaterAState = cabinFlapLTHeaterCool(consts, params);
            HeaterBState = cabinFlapFlapHeaterCool(consts, params);
            HeaterCState = cabinFlapCHLHeaterCool(consts, params);
        }
        else
        { // _Work_CoolWait
            FanState = cabinFlapFanIdle(consts, params);
            FlapState = cabinFlapFlapIdle(consts, params);
            HeaterAState = cabinFlapLTHeaterIdle(consts, params);
            HeaterBState = cabinFlapFlapHeaterIdle(consts, params);
            HeaterCState = cabinFlapCHLHeaterIdle(consts, params);
        }

        if (isCoolOut(CoolState))
        {
            cabinP->states.Cool = CoolState;
            if (CoolState == _CoolOut_FLAP_AirIsTooColdAtQuickFRZ)
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
            cabinP->values.gasDesired.type = _GT_Cold_FF;
#ifdef HOSHI_H30
            if (cabinP->values.ambientType == _Ambient_NONE)
              cabinUpdateGasLevel(consts, params);
            else
              cabinUpdateGasLevelAmbient(consts, params);
#else
            cabinUpdateGasLevel(consts, params);
#endif
        }

        if ((cabinP->values.gasApplied.type == _GT_Hot) ||
            (cabinP->values.gasApplied.type == _GT_No))
        {
            if (isCoolOut(CoolState))
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

    case _Work_HotStart:
        FanState = _FanHotStart_FLAP;
        HeaterAState = _HeaterAOff_HotStart;
        HeaterBState = _HeaterBOff_HotStart;
        HeaterCState = _HeaterCOff_HotStart;

        FlapState = cabinFlapFlapCool(consts, params);
        cabinP->values.gasDesired.type = _GT_HotStart;
        cabinP->values.gasDesired.level = 5;
        if (cabinP->stateTimers.HotStart > 7)
            cabinP->values.gasDesired.level = 7;

        if (cabinP->stateTimers.HotStart > 70)
            cabinP->states.Work = _Work_Cool;
        break;

    /*** APW : Add compernant operation Flap in CT ***/
    case _Work_CommercialTestMode:            
        //Create fan and damper for flap cabin
        FlapState = cabinFlapFlapCommercialState(consts, params);
        FanState = cabinFlapFanCommercial(consts, params);
        HeaterAState = cabinFlapLTHeaterCommercial(consts, params);
        HeaterBState = cabinFlapFlapHeaterCommercial(consts, params);
        HeaterCState = cabinFlapCHLHeaterCommercial(consts, params);
        //Now don't add  flap fan when we need to use please add.
            
            if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
            {
               if(cabinP->components.flap.flapFeed == 0 && 
                   cabinP->values.FlapWaitingTimeCommercial_sec >= 2 &&
                     cabinP->states.Flap == _FlapPositionA_START)
               {
                  cabinP->components.flap.value =
                    cabinC->parameters.Flap.PositionA *10;
                  cabinP->values.FlapWaitingTimeCommercial_sec = 0;
               }
               else if(cabinP->components.flap.flapFeed == cabinC->parameters.Flap.PositionA *10 && 
                   cabinP->values.FlapWaitingTimeCommercial_sec >= 2 &&
                     cabinP->states.Flap == _FlapPositionA_START)
               {
                  cabinP->components.flap.value = 0;
                  cabinP->values.FlapWaitingTimeCommercial_sec = 0;
               }  
               else if(cabinP->components.flap.flapFeed != 0 &&
                        cabinP->states.Flap == _FlapNone_START)
               {
                  cabinP->components.flap.value = 0;
                  cabinP->values.FlapWaitingTimeCommercial_sec = 0;
               }
               else
                  cabinP->components.flap.value = cabinP->components.flap.value;
               
                if(cabinP->states.Fan == _FanCommercialStart_FLAP)
                  cabinP->components.fan.speedOrPWM = 100;
                else  
                  cabinP->components.fan.speedOrPWM = 0;
            }
            
        if(FanState == _FanOff_FLAP_Always)
          cabinP->components.fan.speedOrPWM = 0;
            
        if(HeaterAState == _HeaterAOff_START)
          cabinP->components.heaterA.value = _HeaterValue_OFF;
        if(HeaterBState == _HeaterBOff_START)
          cabinP->components.heaterB.value = _HeaterValue_OFF;
        if(HeaterCState == _HeaterCOff_START)
          cabinP->components.heaterC.value = _HeaterValue_OFF;
            
        break;
        
    case _Work_CommercialDefrostTestMode:   
      //Create heater for flap cabin
        HeaterAState = cabinFlapLTHeaterCommercialDefrostTest(consts, params);
        HeaterBState = cabinFlapFlapHeaterCommercialDefrostTest(consts, params);
        HeaterCState = cabinFlapCHLHeaterCommercialDefrostTest(consts, params);
        
        if(HeaterAState == _HeaterAOn_START)
          cabinP->components.heaterA.value = _HeaterValue_ON;
        else
          cabinP->components.heaterA.value = _HeaterValue_OFF;
        
        if(HeaterBState == _HeaterBOn_START)
          cabinP->components.heaterB.value = _HeaterValue_ON;
        else
          cabinP->components.heaterB.value = _HeaterValue_OFF;
        
        if(HeaterCState == _HeaterCOn_START)
          cabinP->components.heaterC.value = _HeaterValue_ON;
        else
          cabinP->components.heaterC.value = _HeaterValue_OFF;
            
        if(cabinP->components.flap.value == 0 &&
            cabinP->components.flap.flapFeed == 0)
          FlapState = _FlapNone_START;
        else
          FlapState = _FlapPositionA_START;
        
        FanState = _FanOff_FLAP_Always;
        
        break;
    /*** APW : Add compernant operation Flap in CT ***/
        
    default:
        cabinP->states.Work = _Work_Idle;
        break;
    }

    cabinP->states.Defrost = _DefrostNone_FLAP;
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

static void cabinFlapUpdateSensorState(stCabin_Flash const *cabinC, stCabin_Ram *cabinP)
{
    cabinP->states.Sensor = (enSensorStates)0;
    if (cabinC->components.airTempSensor.type != _TempSensorType_NONE)
    {
        if (cabinP->components.airTempSensor.error)
            cabinP->states.Sensor |= _Sensor_AirF;
        else
            cabinP->states.Sensor |= _Sensor_AirP;
    }
}
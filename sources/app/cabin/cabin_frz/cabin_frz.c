#include "cabin_frz.h"
#include "procedures/cabin_frz_defrost.h"
#include "procedures/cabin_frz_cool.h"
#include "components/cabin_frz_d_heater.h"
#include "components/cabin_frz_fan.h"
#include "app/cabin/components/cabin_c_heater.h"
#include "app/cabin/components/cabin_a_heater.h"
#include "app/cabin/components/cabin_comp_flap.h"
#include "app/cabin/procedures/cabin_gas_level.h"
#include "app/cabin/procedures/cabin_shared_funcs.h"
#include "app/cabin/procedures/cabin_duty.h"
#include "app/cabin/procedures/cabin_errors.h"
#include "app/cabin/procedures/cabin_defrost.h"
#include "app/cabin/procedures/cabin_drainage.h"
#include "app/cooler/cooler.h"

static void cabinFrzUpdateSensorState(stCabin_Flash const *cabinC, stCabin_Ram *cabinP);

void cabinFrzInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;
    UNUSED(cabinC);

    cabinP->states.Cool = _CoolNone_FRZ;
    cabinP->states.Defrost = _DefrostNone_FRZ;
    cabinP->states.Fan = _FanNone_FRZ;
    cabinP->states.Flap = _FlapNone_ALL;
    cabinP->states.HeaterA = _HeaterANone_ALL;
    cabinP->states.HeaterB = _HeaterBNone_FRZDefrost;
    cabinP->states.HeaterC = _HeaterCNone_ALL;
    cabinP->states.Work = _Work_Idle;
    cabinP->states.WorkPrev = _Work_Idle;
    cabinP->values.gasDesired.type = _GT_No;
    cabinP->values.gasDesired.level = 0;
}

static void cabinFrzUpdateSensorState(stCabin_Flash const *cabinC, stCabin_Ram *cabinP)
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

void cabinFrzTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    stCabin_Ram *cabinP = params->cabin;
    const stCabin_Flash *cabinC = consts->cabin;

    enCoolStates CoolState;
    enDefrostStates DefrostState;
    enFanStates FanState;
    enFlapStates FlapState;
    enHeaterAStates HeaterAState;
    enHeaterBStates HeaterBState;
    enHeaterCStates HeaterCState;

    cabinFrzUpdateSensorState(cabinC, cabinP);
    cabinFrzDefrost(consts, params);
    switch (cabinP->states.Work)
    {
    case _Work_Idle:
        CoolState = cabinFrzCoolIn(consts, params);
        DefrostState = cabinFrzDefrostIn(consts, params);
        FanState = cabinFrzFanIdle(consts, params);
        HeaterBState = cabinFrzDHeaterIdle(consts, params);
        HeaterCState = _HeaterCOff_ALL_DuringOffTime;

        cabinP->values.workTimer = 0;

        if (isDefrostIn(DefrostState))
        {
            cabinP->states.Defrost = DefrostState;
            cabinP->values.gasDesired.type = _GT_Hot;
            cabinP->values.gasDesired.level = 5; // dummy value
        }
        else if (isCoolIn(CoolState))
        {
            cabinP->states.Cool = CoolState;
            cabinP->values.gasDesired.type = _GT_Cold_FRZ;

            if (cabinP->values.ambientType == _Ambient_NONE)
                cabinUpdateGasLevel(consts, params);
            else
                cabinUpdateGasLevelAmbient(consts, params);
        }
        else
        {
            cabinP->values.gasDesired.type = _GT_No;
            cabinP->values.gasDesired.level = 0;
        }

        if (cabinP->values.gasApplied.type == _GT_Hot)
        {
          if(cabinC->parameters.Defrost.RecoveryModeActive && (params->rf->Cabin[0].states.Work != _Work_Defrost))
          {
            cabinP->values.defrost.recoveryModeActive = 1;
            cabinP->stateTimers.PumpOut = cabinC->parameters.Cool.pumpOutTime;
            cabinP->states.Work = _Work_PumpOut;
            cabinP->states.WorkPrev = _Work_Cool;
          }
          else
          {
            cabinP->states.Work = _Work_Defrost;
            cabinP->states.WorkPrev = _Work_Idle;
            //cabinP->values.realTimer = 0;
            cabinCheckFrzDefrost(consts, params);
          }
        }
        else if ((cabinP->values.gasApplied.type == _GT_Cold) &&
                 (cabinP->values.gasApplied.level))
        {
            if (consts->rf->Sets.configs.functions.PumpOut.isActive)
            {
                cabinP->stateTimers.PumpOut = cabinC->parameters.Cool.pumpOutTime;
                cabinP->states.Work = _Work_PumpOut;
                cabinP->states.WorkPrev = _Work_Idle;
            }
            else
            {
                cabinP->states.Work = _Work_Cool;
                cabinP->states.WorkPrev = _Work_Idle;
            }
        }
        else
        {
            if (isDefrostIn(DefrostState))
            {
              if(cabinC->parameters.Defrost.RecoveryModeActive && (params->rf->Cabin[0].states.Work != _Work_Defrost))
              {
                cabinP->values.defrost.recoveryModeActive = 1;
                cabinP->stateTimers.PumpOut = cabinC->parameters.Cool.pumpOutTime;
                cabinP->states.Work = _Work_PumpOut;
                cabinP->states.WorkPrev = _Work_Cool;
              }
              else
              {
                cabinP->states.Work = _Work_Defrost;
                cabinP->states.WorkPrev = _Work_Idle;
                //cabinP->values.realTimer = 0;
                cabinCheckFrzDefrost(consts, params);
              }
            }
            else if (isCoolIn(CoolState))
            {
                cabinP->states.Work = _Work_CoolWait;
                cabinP->states.WorkPrev = _Work_Idle;
            }
            else
                cabinP->states.Work = _Work_Idle;
        }
        break;
    case _Work_PumpOut:
      HeaterBState = cabinFrzDHeaterIdle(consts, params);
      HeaterCState = _HeaterCOff_ALL_DuringOffTime;
      
      if(cabinP->values.defrost.recoveryModeActive)
      {
        if (!cabinP->stateTimers.PumpOut || 
            (params->rf->Cabin[0].states.Work == _Work_PerformanceTest) ||
               (params->rf->Cabin[0].states.Work == _Work_HotStart))
        {
          cabinP->values.defrost.recoveryModeActive = 0;
          cabinP->states.WorkPrev = _Work_PumpOut;
          cabinP->states.Work = _Work_Defrost;
          cabinCheckFrzDefrost(consts, params);
          //cabinP->values.realTimer = 0;
          if((cabinP->values.gasApplied.type == _GT_Hot) &&  (cabinP->states.WorkPrev == _Work_Cool))
            cabinDutyTask(consts, params);
        }
      }
      else if (!cabinP->stateTimers.PumpOut || 
                (params->rf->Cabin[0].states.Work == _Work_PerformanceTest) ||
                  (params->rf->Cabin[0].states.Work == _Work_HotStart))
      {
        if(cabinP->states.WorkPrev == _Work_HotStart)
          cabinP->states.Work = _Work_HotStart;
        else
          cabinP->states.Work = _Work_Cool;
        
        cabinP->states.WorkPrev = _Work_PumpOut;
      }   
      break;
      
    case _Work_Cool:
    case _Work_CoolWait:
        CoolState = cabinFrzCoolOut(consts, params);
        DefrostState = cabinFrzDefrostIn(consts, params);
        
        if (cabinP->values.gasApplied.type == _GT_Cold)
        {
          if (consts->rf->Sets.configs.functions.PumpOut.isActive && (cabinP->states.WorkPrev == _Work_CoolWait || pumpOut))
          {
            cabinP->stateTimers.PumpOut = cabinC->parameters.Cool.pumpOutTime;
            cabinP->states.WorkPrev = _Work_CoolWait;
            cabinP->states.Work = _Work_PumpOut;
            pumpOut = 0;
          }
          else
          {
            if (cabinP->states.WorkPrev != _Work_PumpOut)
              cabinP->states.WorkPrev = _Work_CoolWait;
            cabinP->states.Work = _Work_Cool;
          }
        }
        
        if (cabinP->states.Work == _Work_Cool)
        {
            FanState = cabinFrzFanCool(consts, params);
            HeaterBState = cabinFrzDHeaterCool(consts, params);
            HeaterCState = _HeaterCOff_ALL_DuringOffTime;
        }
        else
        { // _Work_CoolWait
            FanState = cabinFrzFanIdle(consts, params);
            HeaterBState = cabinFrzDHeaterIdle(consts, params);
            HeaterCState = _HeaterCOff_ALL_DuringOffTime;
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
            cabinP->values.gasDesired.type = _GT_Cold_FRZ;

            if (cabinP->values.ambientType == _Ambient_NONE)
                cabinUpdateGasLevel(consts, params);
            else
                cabinUpdateGasLevelAmbient(consts, params);
        }

        if (cabinP->values.gasApplied.type == _GT_Hot)
        {
          if(cabinC->parameters.Defrost.RecoveryModeActive && (params->rf->Cabin[0].states.Work != _Work_Defrost))
          {
            cabinP->values.defrost.recoveryModeActive = 1;
            cabinP->stateTimers.PumpOut = cabinC->parameters.Cool.pumpOutTime;
            cabinP->states.Work = _Work_PumpOut;
            cabinP->states.WorkPrev = _Work_Cool;
          }
          else
          {
            cabinP->states.Work = _Work_Defrost;
            cabinP->states.WorkPrev = _Work_Cool;
            //cabinP->values.realTimer = 0;
            cabinDutyTask(consts, params);
            cabinCheckFrzDefrost(consts, params);
          }
        }
        else if (cabinP->values.gasApplied.type == _GT_No)
        {
            if (isDefrostIn(DefrostState))
            {
              if(cabinC->parameters.Defrost.RecoveryModeActive && (params->rf->Cabin[0].states.Work != _Work_Defrost))
              {
                cabinP->values.defrost.recoveryModeActive = 1;
                cabinP->stateTimers.PumpOut = cabinC->parameters.Cool.pumpOutTime;
                cabinP->states.Work = _Work_PumpOut;
                cabinP->states.WorkPrev = _Work_Cool;
              }
              else
              {
                cabinP->states.Work = _Work_Defrost;
                cabinP->states.WorkPrev = _Work_Cool;
                //cabinP->values.realTimer = 0;
                cabinCheckFrzDefrost(consts, params);
              }
            }
            else if (isCoolOut(CoolState))
            {
                cabinP->states.Work = _Work_Idle;
                cabinP->states.WorkPrev = _Work_Cool;
            }
            else
            {
                cabinP->states.Work = _Work_CoolWait;

                if (cabinP->states.WorkPrev == _Work_Drainage)
                    cabinP->states.WorkPrev = _Work_Drainage;
                else
                    cabinP->states.WorkPrev = _Work_Cool;
            }

            if (cabinP->states.Work != _Work_CoolWait)
                cabinDutyTask(consts, params);
        }
        break;

    case _Work_Defrost:
        if (cabinP->states.Defrost == _DefrostIn_FRZ_MaxQuickTimeElapsed)
            cabinP->values.quickDefrost = true;

        CoolState = cabinFrzCoolIn(consts, params);
        DefrostState = cabinFrzDefrostOut(consts, params);
        FanState = cabinFrzFanDefrost(consts, params);
        HeaterBState = cabinFrzDHeaterDefrost(consts, params);
        HeaterCState = cabinCHeaterDefrost(consts, params); 

        cabinP->values.workTimer = 0;

        if (isDefrostOut(DefrostState))
        {
            cabinP->states.Defrost = DefrostState;
            // cabinP->values.gasDesired.type  = _GT_Forced; GT_Forced Direkman Kompressor'� durdurur. Tam Paralel i�in Problem.
            // cabinP->values.gasDesired.level = 0;
        }
        else
        {
            cabinP->values.gasDesired.type = _GT_Hot;
            cabinP->values.gasDesired.level = 5; // dummy value
        }

        if (cabinP->values.gasApplied.type == _GT_Cold)
        {
            cabinP->states.Work = _Work_Cool;
            cabinP->states.WorkPrev = _Work_Defrost;
            cabinP->values.firstDefrostFlag = false;
            cabinP->values.finishDefrostFlag = true;
            cabinP->values.realTimer = 0;
            cabinP->E2.realTimer = 0;
            cabinP->values.defrostDoorOpenedTimer = 0;
            cabinCheckFrzDefrost(consts, params);
        }
        else if (cabinP->values.gasApplied.type == _GT_No)
        {
            if (isDefrostOut(DefrostState))
            {
                if (DefrostState == _DefrostOut_FRZ_MaxDefrostTimeElapsed_DO)
                {
                    cabinIncDefrostHeaterErrCounter(consts, params);
                }
                else
                    cabinClearDefrostHeaterErrCounter(consts, params);

                cabinP->states.Work = _Work_Drainage;
                cabinP->states.WorkPrev = _Work_Defrost;
                cabinP->values.firstDefrostFlag = false;
                cabinP->values.finishDefrostFlag = true;
                cabinP->values.realTimer = 0;
                cabinP->E2.realTimer = 0;
                cabinP->values.afterDefrostTimer = cabinC->parameters.Defrost.AfterDefrostTime;
                params->rf->Cabin[0].values.defrost.afterDefrostActive = 1;
                params->rf->Cabin[1].values.defrost.afterDefrostActive = 1;
                cabinP->values.defrostDoorOpenedTimer = 0;
                cabinCheckFrzDefrost(consts, params);
            }
        }

        if (cabinP->states.Work != _Work_Defrost)
        {
            if (DefrostState != _DefrostOut_FRZ_ForIceCream)
            {
               /* cabinP->values.defrostDelayTimerForIceCream =
                    cabinC->parameters.Defrost.MaxDelayTimeForIceCream;*/
                cabinP->values.cycleCounter = 0;
                cabinP->values.workTimer = 0;
                // cabinP->values.realTimer    = 0;
                cabinP->values.duty.delay = 2;
                // cabinP->stateTimers.Defrost = 0;
                cabinDefrostMeanTimeCTRL(consts, params);
                cabinP->stateTimers.Drainage = 0;
            }
        }
        break;

    case _Work_Drainage:
        CoolState = cabinFrzCoolIn(consts, params);
        FanState = cabinFrzFanDrainage(consts, params);
        HeaterBState = cabinFrzDHeaterDrainage(consts, params);
        HeaterCState = cabinCHeaterDrainage(consts, params); 
        
        if(cabinP->values.defrost.defrostHighTempActive)
            cabinP->values.defrost.defrostHighTempActive = false;

        cabinCheckFrzDefrost(consts, params);

        if (cabinDrainageOut(consts, params))
        {
            if (isCoolIn(CoolState))
            {
                cabinP->states.Cool = CoolState;
                cabinP->values.gasDesired.type = _GT_Cold_FRZ;

                if (cabinP->values.ambientType == _Ambient_NONE)
                    cabinUpdateGasLevel(consts, params);
                else
                    cabinP->values.gasDesired.level = _compressorDefrostLevel + 1;
            }
            else
            {
                cabinP->values.gasDesired.type = _GT_No;
                cabinP->values.gasDesired.level = 0;
            }
        }
        else
        {
            // cabinP->values.gasDesired.type      = _GT_Forced; GT_Forced Direkman Kompressor'� durdurur. Tam Paralel i�in Problem.
            // cabinP->values.gasDesired.level     = 0;
        }

        if (cabinP->values.gasApplied.type == _GT_Hot)
        {
          if(cabinC->parameters.Defrost.RecoveryModeActive && (params->rf->Cabin[0].states.Work != _Work_Defrost))
          {
            cabinP->values.defrost.recoveryModeActive = 1;
            cabinP->stateTimers.PumpOut = cabinC->parameters.Cool.pumpOutTime;
            cabinP->states.Work = _Work_PumpOut;
            cabinP->states.WorkPrev = _Work_Cool;
          }
          else
          {
            cabinP->states.Work = _Work_Defrost;
            cabinP->states.WorkPrev = _Work_Drainage;
            //cabinP->values.realTimer = 0;
            cabinCheckFrzDefrost(consts, params);
          }
        }
        else if (cabinP->values.gasApplied.type == _GT_Cold)
        {
            cabinP->states.Work = _Work_Cool;
            cabinP->states.WorkPrev = _Work_Drainage;
        }
        else
        {
            if (cabinDrainageOut(consts, params))
            {
                if (isCoolIn(CoolState))
                    cabinP->states.Work = _Work_Cool;
                else
                    cabinP->states.Work = _Work_Idle;

                cabinP->states.WorkPrev = _Work_Drainage;
            }
        }

        /*if (cabinP->states.Work != _Work_Drainage)
        {
            if (!cabinP->values.quickDefrost)
                cabinP->values.quickTimer = 0;
        }*/
        break;
    case _Work_HotStart:
        if(cabinP->stateTimers.HotStart > cabinC->parameters.Fan.HotStartDelayTime)
            {
              if (consts->rf->Cooler.type == _CoolerType_PARALLEL)
              {
                if (cabinP->values.gasApplied.type == _GT_Cold ) 
                  FanState = _FanHotStart_FF;
                else
                  FanState = _FanOff_FF_Always;
              }
              else
              {
                  FanState = _FanHotStart_FF;
              }
            }
            else
              FanState = _FanOff_FRZ_Always;

              HeaterAState = _HeaterAOff_HotStart;
              HeaterBState = _HeaterBOff_HotStart;
              HeaterCState = _HeaterCOff_HotStart;
              
              CoolState =  cabinFrzCoolOut(consts, params);

              if (!cabinP->values.doorClosedTimer)
              {
                if(cabinP->stateTimers.HotStart >= cabinC->parameters.Cool.HotStartTime)
                  cabinP->values.gasDesired.type  = _GT_HotStart2;
                else
                  cabinP->values.gasDesired.type  = _GT_HotStart;
                  
                if(cabinP->values.ambientType == _Ambient_NONE)
                {
                  cabinP->values.gasDesired.level = 5;
                  if (cabinP->stateTimers.HotStart > 7)
                    cabinP->values.gasDesired.level = 6;
                }
                else
                {
                  cabinP->values.gasDesired.level = _compressorHotStartLevel + 1;
                }
              }
              else
              {
                if(cabinP->stateTimers.HotStart >= cabinC->parameters.Cool.HotStartTime)
                  cabinP->values.gasDesired.type  = _GT_HotStart2;
                else
                  cabinP->values.gasDesired.type  = _GT_HotStart;
                  
                if(cabinP->values.ambientType == _Ambient_NONE)
                {
                  cabinP->values.gasDesired.level = 5;
                  if (cabinP->stateTimers.HotStart > 7)
                    cabinP->values.gasDesired.level = 8;
                } 
                else
                {
                  cabinP->values.gasDesired.level = _compressorHotStartLevel + 1;
                }
              }

              if ((cabinP->stateTimers.HotStart >= (cabinC->parameters.Cool.HotStartTime + cabinC->parameters.Cool.HotStart2Time))
                  || isCoolOut(CoolState))
              {
                if (cabinP->values.gasApplied.type == _GT_Cold )
                {
                  cabinP->states.Work = _Work_Cool;
                  cabinP->states.WorkPrev = _Work_HotStart;
                }
                else
                {
                    cabinP->states.Work = _Work_Idle;
                    cabinP->states.WorkPrev = _Work_HotStart;
                }   
              }
          break;
      case _Work_PerformanceTest:
          if(cabinP->stateTimers.PerformanceTest > cabinC->parameters.Fan.HotStartDelayTime)
          {
            if (refrigerator_F.Cooler.type == _CoolerType_SEMIPARALLEL)
            {
              if (cabinP->values.gasApplied.type == _GT_Cold ) 
                FanState = _FanHotStart_FRZ;
              else
                FanState = _FanOff_FRZ_Always;
            }
            else
            {
                FanState = _FanHotStart_FRZ;
            }
          }
          else
            FanState = _FanOff_FRZ_Always;
          
            HeaterAState = _HeaterAOff_HotStart;
            HeaterBState = _HeaterBOff_HotStart;
            HeaterCState = _HeaterCOff_HotStart;
            
            CoolState = cabinFrzCoolOut(consts, params);

            if (!cabinP->values.doorClosedTimer)
            {
                if(cabinP->stateTimers.PerformanceTest >= cabinC->parameters.Cool.PerformanceTestTime)
                  cabinP->values.gasDesired.type = _GT_PerformanceTest2;
                else  
                  cabinP->values.gasDesired.type = _GT_PerformanceTest;
                
                if(cabinP->values.ambientType == _Ambient_NONE)
                {
                    cabinP->values.gasDesired.level = 8;
                    if (cabinP->stateTimers.HotStart > 7)
                        cabinP->values.gasDesired.level = 9;
                }
                else
                {
                    cabinP->values.gasDesired.level = _compressorHotStartLevel + 1;
                }
            }
            else
            {
                if(cabinP->stateTimers.PerformanceTest >= cabinC->parameters.Cool.PerformanceTestTime)
                  cabinP->values.gasDesired.type = _GT_PerformanceTest2;
                else  
                  cabinP->values.gasDesired.type = _GT_PerformanceTest;
                
                if(cabinP->values.ambientType == _Ambient_NONE)
                {
                    cabinP->values.gasDesired.level = 5;
                    if (cabinP->stateTimers.HotStart > 7)
                        cabinP->values.gasDesired.level = 7;
                }
                else
                {
                    cabinP->values.gasDesired.level = _compressorHotStartLevel + 1;
                }
            }
            if ((cabinP->stateTimers.PerformanceTest >= (cabinC->parameters.Cool.PerformanceTestTime + cabinC->parameters.Cool.PerformanceTest2Time))
                 || isCoolOut(CoolState))
            {
              if (cabinP->values.gasApplied.type == _GT_Cold )
              {
                cabinP->states.Work = _Work_Cool;
                cabinP->states.WorkPrev = _Work_PerformanceTest; 
              }
              else
              {
                cabinP->states.Work = _Work_Idle;
                cabinP->states.WorkPrev = _Work_PerformanceTest;
              }
            }
        break;
        
    /*** APW : Add CT heater software ***/  
    case _Work_CommercialTestMode:
     
            FanState = cabinFrzFanCommercial(consts, params);
            HeaterAState = cabinFfzHeaterACommercialTest(consts, params);
            HeaterBState = cabinFfzHeaterBCommercialTest(consts, params);
            HeaterCState = cabinFfzHeaterCCommercialTest(consts, params);
            
            if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_FirstTest)
            {
                if(cabinP->states.Fan == _FanCommercialStart_FRZ)
                  cabinP->components.fan.speedOrPWM = 100;
                else  
                  cabinP->components.fan.speedOrPWM = 0;
            }
            
            else if(params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_ValveTest ||
                      params->rf->workMode.values.CommercialTest.state == _WM_CommercialTest_State_CoolingTest)
            {
                if(cabinP->states.Fan == _FanCommercialStart_FRZ)
                  cabinP->components.fan.speedOrPWM = 100;
                else  
                  cabinP->components.fan.speedOrPWM = 0;
            }
            
            else
            {
               //(Other condition will not operation, if open) cabinP->components.fan.speedOrPWM = 0;
            }

        if(FanState == _FanOff_FRZ_Always)
          cabinP->components.fan.speedOrPWM = 0;
            
        /*** APW : Disable All heater in CT test function ***/
        if(HeaterAState == _HeaterAOff_START)
          cabinP->components.heaterA.value = _HeaterValue_OFF;
        if(HeaterBState == _HeaterBOff_START)
          cabinP->components.heaterB.value = _HeaterValue_OFF;
        if(HeaterCState == _HeaterCOff_START)
          cabinP->components.heaterC.value = _HeaterValue_OFF;
        /*** APW : Disable All heater in CT test function ***/
      
        break;
        
    case _Work_CommercialDefrostTestMode:   
      
        HeaterAState = cabinFfzHeaterACommercialDefrostTest(consts, params);
        HeaterBState = cabinFfzHeaterBCommercialDefrostTest(consts, params);
        HeaterCState = cabinFfzHeaterCCommercialDefrostTest(consts, params); 
        
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
        
        FanState = _FanOff_FRZ_Always;
            
        break;
    /*** APW : Add CT heater software ***/
        
    default:
        cabinP->states.Work = _Work_Idle;
        break;
    }
  
    /*** APW: Commercial mode don't need ***/
    if(cabinP->states.Work != _Work_CommercialTestMode && 
        cabinP->states.Work != _Work_CommercialDefrostTestMode)
    {
      FlapState = cabinFlapState(consts, params);   
      
      if (cabinP->values.humidityType != _Humidity_NONE)
        HeaterAState = cabinAHeaterHumidity(consts, params);
      else
        HeaterAState = cabinAHeater(consts, params);
        
	  if(!cabinP->values.afterDefrostTimer)
      {
	    params->rf->Cabin[0].values.defrost.afterDefrostActive = 0;
	    params->rf->Cabin[1].values.defrost.afterDefrostActive = 0;
      }
    }
    /*** APW: Commercial mode don't need ***/
    
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
    
    if(cabinP->values.firstDefrostFlag)
      cabinP->values.defrost.delayTimer = 0;
}

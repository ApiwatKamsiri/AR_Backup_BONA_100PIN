/** @file       energymonitoring.c
 *  @brief      ENERGYMONITORING application program file
 *  @copyright  (c) 2024-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Baris KARA
 *  @date       26.08.2024
 */
#define FILE_ENERGYMONITORING_C

#include "energymonitoring.h"
#include "../app/refrigerator/rf_common.h"
#include "app/refrigerator/rf_ram.h"
#include "app/refrigerator/rf_configs.h"

#define DEBUG_FILE_NAME "energy"

// #define DEBUG_PORT_
// DEBUG_START(0)

#define _MAINBOARD_CONSUMPTION_VALUE 3*100 //Watt
#define _MAINBOARD_IDLE_CONSUMPTION_VALUE 1.3*100 //Watt

/**
 *  @addtogroup ENERGYMONITORING
 *  @{
 */
static unsigned short getDcBusVoltage(tsEnergyMonitoringParams *params)
{
  if((params->compressor->compressorType == COMPRESSOR_TYPE_EMBRACO) ||
  (params->compressor->compressorType == COMPRESSOR_TYPE_JIAXIPERA) ||
  (params->compressor->compressorType == COMPRESSOR_TYPE_TWOINONE))
    return params->compressor->Embc_Jiax_Packets.BusVoltage;

  if(params->compressor->compressorType == COMPRESSOR_TYPE_EKI)
    return params->compressor->EKIPackets.InverterDCbusVoltage;

    return 0;
}

static unsigned int getInstantPowerValue(tsEnergyMonitoringParams *params, unsigned short resistance)
{
  unsigned int instantPower = 0;
  unsigned short dcBusVoltage = getDcBusVoltage(params);
  unsigned int acVoltage = (unsigned int)((float)(dcBusVoltage)/1.414);

  if(resistance) 
  {
    if(!acVoltage)
      instantPower = (230*230) / resistance;
    else
      instantPower = (acVoltage*acVoltage) / resistance; 
  }

  return instantPower;
}



static void RefrigeratorCoolerEnergyCalculate(tsEnergyMonitoringParams *params, const tsEnergyMonitoringConsts *consts)
{
    tsT1 *time = &params->energy.T1sec;

    if(refrigerator_F.Cooler.components.compressor.type)
    {
        if((params->compressor->compressorType == COMPRESSOR_TYPE_EMBRACO) ||
        (params->compressor->compressorType == COMPRESSOR_TYPE_JIAXIPERA) ||
        (params->compressor->compressorType == COMPRESSOR_TYPE_TWOINONE))
            time->instantValue += (unsigned int)(params->compressor->Embc_Jiax_Packets.Power)*100;
        else if(params->compressor->compressorType == COMPRESSOR_TYPE_EKI)
            time->instantValue += (unsigned int)(params->compressor->EKIPackets.PowerDQActual)*100;
        else
            time->instantValue += 0; // TODO ????
    }

    if(refrigerator_F.Cooler.components.condenserFan.type)
      if(refrigerator_R.Cooler.components.condenserFan.speedOrPWM)
        time->instantValue += refrigerator_F.Cooler.components.condenserFan.maxPower;/**refrigerator_R.Cooler.components.condenserFan.speedOrPWM;*/
}

static void RefrigeratorCabinEnergyCalculate(tsEnergyMonitoringParams *params, const tsEnergyMonitoringConsts *consts)
{
    tsT1 *time = &params->energy.T1sec;
    //?? time->instantValue = 0;
    
    for (unsigned char i = 0; i < _NumOfCabin; i++)
    {
        if (refrigerator_F.Cabin[i].type)
        {
           if(refrigerator_F.Cabin[i].components.fan.type)
             if(refrigerator_R.Cabin[i].components.fan.speedOrPWM)
             time->instantValue += refrigerator_F.Cabin[i].components.fan.maxPower;/**refrigerator_R.Cabin[i].components.fan.speedOrPWM;*/

           if(refrigerator_F.Cabin[i].components.heaterA.type)
             if(refrigerator_R.Cabin[i].components.heaterA.value)
             time->instantValue += getInstantPowerValue(params, refrigerator_F.Cabin[i].components.heaterA.resistanceForPower)*100;

           if(refrigerator_F.Cabin[i].components.heaterB.type)
             if(refrigerator_R.Cabin[i].components.heaterB.value)
             time->instantValue += getInstantPowerValue(params, refrigerator_F.Cabin[i].components.heaterB.resistanceForPower)*100;

           if(refrigerator_F.Cabin[i].components.heaterC.type)
             if(refrigerator_R.Cabin[i].components.heaterC.value)
             time->instantValue += getInstantPowerValue(params, refrigerator_F.Cabin[i].components.heaterC.resistanceForPower)*100;

           if(refrigerator_F.Cabin[i].components.ionizer.type)
             if(refrigerator_R.Cabin[i].components.ionizer.value)
             time->instantValue += refrigerator_F.Cabin[i].components.ionizer.maxPower;

           if(refrigerator_F.Cabin[i].components.innerLight.type)
             if(refrigerator_R.Cabin[i].components.innerLight.value)
             time->instantValue += refrigerator_F.Cabin[i].components.innerLight.maxPower;

           if(refrigerator_F.Cabin[i].components.blueLight.type)
             if(refrigerator_R.Cabin[i].components.blueLight.value)
             time->instantValue += refrigerator_F.Cabin[i].components.blueLight.maxPower;
           

           if(refrigerator_F.Cabin[i].components.handleLight.type)
           {
             if(refrigerator_R.Cabin[i].components.handleLight.R_value)
               time->instantValue += refrigerator_F.Cabin[i].components.handleLight.maxPower * refrigerator_R.Cabin[i].components.handleLight.R_value / 100;
             if(refrigerator_R.Cabin[i].components.handleLight.G_value)
               time->instantValue += refrigerator_F.Cabin[i].components.handleLight.maxPower * refrigerator_R.Cabin[i].components.handleLight.G_value / 100;
             if(refrigerator_R.Cabin[i].components.handleLight.B_value)
               time->instantValue += refrigerator_F.Cabin[i].components.handleLight.maxPower * refrigerator_R.Cabin[i].components.handleLight.B_value / 100;
           }

          if(refrigerator_F.Cabin[i].components.violed.type)
            if(refrigerator_R.Cabin[i].components.violed.value)
             time->instantValue += refrigerator_F.Cabin[i].components.violed.maxPower;
        }
    }
}

static void RefrigeratorOtherComponenetsEnergyCalculate(tsEnergyMonitoringParams *params, const tsEnergyMonitoringConsts *consts)
{
    tsT1 *time = &params->energy.T1sec;
    if(refrigerator_R.Cooler.components.compressor.value)
      time->instantValue += _MAINBOARD_CONSUMPTION_VALUE;
    else
      time->instantValue += _MAINBOARD_IDLE_CONSUMPTION_VALUE;
}

void EnergyMonitoring1SecTask(tsEnergyMonitoringParams *params, const tsEnergyMonitoringConsts *consts)
{
    //static tsRefrigeratorEnergy energy;

    params->energy.T1sec.instantValue = 0;

    RefrigeratorCoolerEnergyCalculate(params, consts);
    RefrigeratorCabinEnergyCalculate(params, consts);
    RefrigeratorOtherComponenetsEnergyCalculate(params, consts);
    
    if(params->energy.stateTimerT1sec)
        params->energy.stateTimerT1sec--;
}
void EnergyMonitoring1MinTask(tsEnergyMonitoringParams *params, const tsEnergyMonitoringConsts *consts)
{
    params->energy.T1min.instantValue = params->energy.T1sec.averageValue;
}

static PT_THREAD(energyMonitoringThread)
{
    tsEnergyMonitoringParams *params       = process->parameters;
    const tsEnergyMonitoringConsts *consts = process->constants;
    UNUSED(params);
    UNUSED(consts);
    PT_BEGIN();

    for (;;)
    {
        PT_YIELD();
    }

    PT_END();
}

static PROCESS_HANDLER_PROTO(energyMonitoringHandler);

/// @brief  Initialization function of energyMonitoring
PROCESS_INIT_PROTO(energyMonitoringInit)
{
    tsEnergyMonitoringParams *params       = process->parameters;
    const tsEnergyMonitoringConsts *consts = process->constants;

    UNUSED(consts); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, energyMonitoringHandler);

    timerEventStart(&params->te.periodic1Sec, ENERGYMONITORING_TIME1SEC);
    timerEventStart(&params->te.periodic1Min, ENERGYMONITORING_TIME1MIN);

    threadStart(process, energyMonitoringThread);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of energyMonitoring
PROCESS_DEINIT_PROTO(energyMonitoringDeinit)
{
    tsEnergyMonitoringParams *params       = process->parameters;
    const tsEnergyMonitoringConsts *consts = process->constants;

    UNUSED(consts); // REMOVE IF USED

    timerEventStop(&params->te.periodic1Sec);
    timerEventStop(&params->te.periodic1Min);

    PROCESS_STATE_CHANGE(process, NULL);
    threadStop(process, process->threadFunction);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of energyMonitoring
static PROCESS_HANDLER_PROTO(energyMonitoringHandler)
{
    tsEnergyMonitoringParams *params       = process->parameters;
    const tsEnergyMonitoringConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
        case eEmPeriodic1Sec:
            EnergyMonitoring1SecTask(params,consts);
            timerEventStart(&params->te.periodic1Sec, ENERGYMONITORING_TIME1SEC);
            break;
        case eEmPeriodic1Min:
            EnergyMonitoring1MinTask(params,consts);
            timerEventStart(&params->te.periodic1Min, ENERGYMONITORING_TIME1MIN);
            break;

        default:
            break;
    }
}

/** @} */

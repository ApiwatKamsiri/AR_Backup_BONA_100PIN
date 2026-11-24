#include "cabin_switch.h"

void cabinSwitchInit(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    cabinP->values.doorClosedTimer = 0;
    cabinP->values.doorOpenedTimer = 0;
}

void cabinSwitch20MsTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    if (cabinP->values.doorClosedTimer)
        if (cabinP->values.doorClosedDelay)
            cabinP->values.doorClosedDelay--;

    if (cabinP->values.doorClosedTimer < 0xFF)
        cabinP->values.doorClosedTimer++;
    
    if ((cabinP->components.doorSwitch[0].value == _SwitchValue_OPEN) ||
        (cabinP->components.doorSwitch[1].value == _SwitchValue_OPEN))
    {
      if (cabinP->values.doorOpenedTimer < 0xFF)
        cabinP->values.doorOpenedTimer++;
    }
    //TODO: Arif, buraya bakilmasi lazim. Kapi switch'ine gore artirmak daha iyi olabilir.
}

void cabinSwitchTask(tsCabinProcConsts const *consts, tsCabinProcParams *params)
{
    const stCabin_Flash *cabinC = consts->cabin;
    stCabin_Ram *cabinP = params->cabin;
    UNUSED(cabinC);

    if ((cabinP->components.doorSwitch[0].value == _SwitchValue_OPEN) ||
        (cabinP->components.doorSwitch[1].value == _SwitchValue_OPEN))
    {
        cabinP->values.doorClosedTimer = 0;
        cabinP->values.doorClosedDelay = _DOOR_CLOSED_DELAY;
        //APW : Add function Empty Clear by open and Clear flag first ice for fix Ice clogged pipes
        if(refrigerator_R.WaterSource.values.waterTankEmpty)
          refrigerator_R.WaterSource.values.EmptyDoorOpenClear = true;
    }
    else
    {
        cabinP->values.doorOpenedTimer = 0;
        //APW : Add function Empty Clear by open and Clear flag first ice for fix Ice clogged pipes
        if(refrigerator_R.WaterSource.values.waterTankEmpty && 
           (refrigerator_R.Cabin[0].components.doorSwitch[0].value == _SwitchValue_CLOSE && refrigerator_R.Cabin[1].components.doorSwitch[0].value == _SwitchValue_CLOSE))
              if(refrigerator_R.WaterSource.values.EmptyDoorOpenClear)
                refrigerator_R.WaterSource.values.EmptyDoorCloseClear = true;
    }
}
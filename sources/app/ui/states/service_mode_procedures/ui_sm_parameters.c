#include "ui_sm_parameters.h"
#include "../../procedures/ui_slides.h"

#ifdef SERVICEMODEavailable

static void ParametersCheckIceMachine(tsUiSmParametersData *smParams, stIceMachine_Flash const *iceMachineC);
static unsigned char ControlIceMachinePumpWaterTime(unsigned char value, bool change);
static unsigned char ControlDispenserLightTime(unsigned char value, bool change);
static void ParametersCheckDispenser(tsUiSmParametersData *smParams, stDispenser_Flash const *dispenserC);
void uiSmParamsCheckAll(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiSmParametersData *smParams = &params->ui->states.service.params;
    smParams->number = 0;
    for (unsigned char i = 0; i < _SizeOfParametersCheckTable; i++)
        smParams->checkTable[i] = 0;
    ParametersCheckDispenser(smParams, &consts->rf->Dispenser);

    ParametersCheckIceMachine(smParams, &consts->rf->IceMachine);
    for (unsigned char i = 0; i < _SizeOfParametersCheckTable; i++)
    {
        if (smParams->checkTable[i])
        {
            for (unsigned char j = 0; j < 7; j++)
            {
                if (smParams->checkTable[i] & (0x01 << j))
                {
                    smParams->table[smParams->number][0] = i;
                    smParams->table[smParams->number][1] = j;
                    smParams->number++;
                }
            }
        }
    }
}

void uiSmParamsControlSelected(const tsUiProcConsts *consts, tsUiProcParams *params, bool change)
{
    tsUiSmParametersData *smParams = &params->ui->states.service.params;
    tsUiRam *uiP = params->ui;

    if (smParams->index >= smParams->number)
    {

        params->ui->slides.slideNumber = 0;
        return;
    }

    switch (smParams->table[smParams->index][0])
    {
    case _Par_IceMachinePumpWaterTime:
        params->rf->IceMachine.sets.waterPumpTime =
            ControlIceMachinePumpWaterTime(params->rf->IceMachine.sets.waterPumpTime, change);

        params->rf->IceMachine.E2.waterPumpTime =
            params->rf->IceMachine.sets.waterPumpTime;

        uiSlideData(&uiP->slides, eDisplaySpecialsParametersIceMachineWaterPumpTime, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, params->rf->IceMachine.sets.waterPumpTime, 1, eDisplayCabinDataTypeNumber);
        params->ui->slides.slideNumber = 2;
        break;

    case _Par_DispenserLightTime:
        params->rf->Dispenser.sets.lightTime =
            ControlDispenserLightTime(params->rf->Dispenser.sets.lightTime, change);

        params->rf->Dispenser.E2.lightTime =
            params->rf->Dispenser.sets.lightTime;

        uiSlideData(&uiP->slides, eDisplaySpecialsParametersDispenserLightTime, 0, eDisplayCabinDataTypeSpecial);
        uiSlideData(&uiP->slides, params->rf->Dispenser.sets.lightTime, 1, eDisplayCabinDataTypeNumber);       
        params->ui->slides.slideNumber = 2;
        break;
    }
    change = 0;
}

static void ParametersCheckIceMachine(tsUiSmParametersData *smParams, stIceMachine_Flash const *iceMachineC)
{
    if (iceMachineC->type != _IceMachineType_NONE)
        smParams->checkTable[_Par_IceMachinePumpWaterTime] = 1;
}
static void ParametersCheckDispenser(tsUiSmParametersData *smParams, stDispenser_Flash const *dispenserC)
{
    if ((dispenserC->waterType != _DispenserWaterType_NONE) ||
        (dispenserC->iceType != _DispenserIceType_NONE))
    {
        if (dispenserC->components.light.type != _LightType_NONE)
            smParams->checkTable[_Par_DispenserLightTime] = 1;
    }
} 
static unsigned char ControlIceMachinePumpWaterTime(unsigned char value, bool change)
{
    if (change)
    {
        value++;
        if ((value > 99) || (value < 10))
            value = 10;
    }

    return value;
}
static unsigned char ControlDispenserLightTime(unsigned char value, bool change)
{
    if (change)
    {
        value++;
        if ((value > 99) || (value < 10))
            value = 10;
    }

    return value;
}

#endif //SERVICEMODEavailable

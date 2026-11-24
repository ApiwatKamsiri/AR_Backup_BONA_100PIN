#include "ui_ice_cream_machine.h"
#include "ui_buzzer.h"
#include "../command_events/cmd_check.h"
#include "../command_events/ce_ice_cream_machine.h"
#include "ui_display.h"

void uiIceCreamMachineInit(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    UNUSED(consts);
    UNUSED(params);
}

void uiIceCreamMachine20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    UNUSED(consts);
    tsUiIceMachineData *im = &params->ui->iceMachine;
    if (im->timer)
        im->timer--;
}

void uiIceCreamMachine1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    UNUSED(consts);
    tsUiIceMachineData *im = &params->ui->iceMachine;
    if (im->stateTimer)
        im->stateTimer--;
}

void uiIceCreamMachineTask(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    tsUiIceMachineData *im = &params->ui->iceMachine;

    switch (params->rf->IceCreamMachine.values.state)
    {
    case _IceCreamMachineState_Idle:
        if (im->timer > (_UI_IceCreamMachine_BlinkSet_Time >> 1))
        {
            uiDspIceCreamData(consts, params, params->rf->IceCreamMachine.sets.datas.display,
                              eDisplayCabinDataTypeNumber);
            uiDspIceCreamLED(consts, params, eDisplayLedStateOn);
        }
        else
        {
            uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
            uiDspIceCreamLED(consts, params, eDisplayLedStateOff);
        }

        if (!im->timer)
            im->timer = _UI_IceCreamMachine_BlinkSet_Time;

        if (!im->stateTimer)
        {
            params->rf->IceCreamMachine.values.state = _IceCreamMachineState_OFF;
            sysBuzzerCmd(eSysBuzzerPlayCloseMelody);
        }
        break;

    case _IceCreamMachineState_Init:
    case _IceCreamMachineState_ErrTemp:
        if (params->rf->IceCreamMachine.values.errTemp)
        {
            if (im->timer > (_UI_IceCreamMachine_BlinkSet_Time >> 1))
            {
                uiDspIceCreamData(consts, params, eDisplaySpecialsError, eDisplayCabinDataTypeSpecial);
                uiDspIceCreamLED(consts, params, eDisplayLedStateOn);
            }
            else
            {
                uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
                uiDspIceCreamLED(consts, params, eDisplayLedStateOff);
            }

            if (!im->timer)
                im->timer = _UI_IceCreamMachine_BlinkSet_Time;
        }
        else
        {
            uiDspIceCreamData(consts, params, params->rf->IceCreamMachine.sets.datas.display,
                              eDisplayCabinDataTypeNumber);
            uiDspIceCreamLED(consts, params, eDisplayLedStateAnimation);
        }

        if (!im->stateTimer)
            params->rf->IceCreamMachine.values.state = _IceCreamMachineState_Idle;
        break;

    case _IceCreamMachineState_MakingIceCream:
        if (params->rf->IceCreamMachine.values.errTemp)
        {
            if (im->timer > (_UI_IceCreamMachine_BlinkSet_Time >> 1))
            {
                uiDspIceCreamData(consts, params, eDisplaySpecialsError, eDisplayCabinDataTypeSpecial);
                uiDspIceCreamLED(consts, params, eDisplayLedStateOn);
            }
            else
            {
                uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
                uiDspIceCreamLED(consts, params, eDisplayLedStateOff);
            }

            if (!im->timer)
                im->timer = _UI_IceCreamMachine_BlinkSet_Time;
        }
        else
        {
            uiDspIceCreamData(consts, params, params->rf->IceCreamMachine.sets.datas.display,
                              eDisplayCabinDataTypeNumber);
            uiDspIceCreamLED(consts, params, eDisplayLedStateAnimation);
        }

        im->stateTimer = _UI_IceCreamMachine_StateIceCreamIsMade_Time;
        break;

    case _IceCreamMachineState_IceCreamIsMade:
        if (im->timer > (_UI_IceCreamMachine_BlinkSet_Time >> 1))
            uiDspIceCreamLED(consts, params, eDisplayLedStateOn);
        else
            uiDspIceCreamLED(consts, params, eDisplayLedStateOff);

        if (!im->timer)
            im->timer = _UI_IceCreamMachine_BlinkSet_Time;

        uiDspIceCreamData(consts, params, params->rf->IceCreamMachine.sets.datas.display,
                          eDisplayCabinDataTypeNumber);

        uiResetDisplayOffTimer(params->ui);
        uiBuzzerWarning(consts, params, true);

        if (!im->stateTimer)
        {
            params->rf->IceCreamMachine.values.state = _IceCreamMachineState_Idle;
        }
        break;

    case _IceCreamMachineState_OFF:
        uiDspIceCreamData(consts, params, eDisplaySpecialsAllOff, eDisplayCabinDataTypeSpecial);
        uiDspIceCreamLED(consts, params, eDisplayLedStateOff);
        break;
    }
}

void uiIceCreamMachineKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params)
{
    switch (params->rf->IceCreamMachine.values.state)
    {
    case _IceCreamMachineState_Idle:
        ceIceCreamMachineSet(params, &consts->rf->IceCreamMachine,
                             &params->rf->IceCreamMachine,
                             isEvent(eDisplayEventIceCreamMchSet));
        ceIceCreamMachineStart(params, &consts->rf->IceCreamMachine,
                               &params->rf->IceCreamMachine,
                               isEvent(eDisplayEventIceCreamMchOnOff));
        break;

    case _IceCreamMachineState_Init:
    case _IceCreamMachineState_MakingIceCream:
        ceIceCreamMachineEnd(params, &consts->rf->IceCreamMachine,
                             &params->rf->IceCreamMachine,
                             isEvent(eDisplayEventIceCreamMchOnOff));
        break;

    case _IceCreamMachineState_ErrTemp:
    case _IceCreamMachineState_IceCreamIsMade:
        ceIceCreamMachineEnd(params, &consts->rf->IceCreamMachine,
                             &params->rf->IceCreamMachine,
                             isEvent(eDisplayEventIceCreamMchSet));
        break;

    case _IceCreamMachineState_OFF:
        ceIceCreamMachineOnOff(params, &consts->rf->IceCreamMachine,
                               &params->rf->IceCreamMachine,
                               isEvent(eDisplayEventIceCreamMchOnOff));
        break;
    }
}

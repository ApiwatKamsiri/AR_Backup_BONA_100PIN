#include "ce_ice_cream_machine.h"

static void IceCreamMachineOnOff(stIceCreamMachine_Ram *iceCreamMachineP);
static void IceCreamMachineSet(stIceCreamMachine_Flash const *iceCreamMachineC, stIceCreamMachine_Ram *iceCreamMachineP);
static void IceCreamMachineStart(stIceCreamMachine_Ram *iceCreamMachineP);
static void IceCreamMachineEnd(stIceCreamMachine_Ram *iceCreamMachineP);

void ceIceCreamMachineOnOff(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC,
                            stIceCreamMachine_Ram *iceCreamMachineP, bool cmdReceived)
{
    if ((!cmdReceived) || (!iceCreamMachineC->type))
        return;

    IceCreamMachineOnOff(iceCreamMachineP);
    params->ui->iceMachine.stateTimer = _UI_IceCreamMachine_StateIdle_Time;
}

void ceIceCreamMachineSet(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC,
                          stIceCreamMachine_Ram *iceCreamMachineP,
                          bool cmdReceived)
{
    if ((!cmdReceived) || (!iceCreamMachineC->type))
        return;

    IceCreamMachineSet(iceCreamMachineC, iceCreamMachineP);
    params->ui->iceMachine.timer = _UI_IceCreamMachine_BlinkSet_Time;
    params->ui->iceMachine.stateTimer = _UI_IceCreamMachine_StateIdle_Time;
}

void ceIceCreamMachineStart(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC,
                            stIceCreamMachine_Ram *iceCreamMachineP,
                            bool cmdReceived)
{
    if ((!cmdReceived) || (!iceCreamMachineC->type))
        return;

    IceCreamMachineStart(iceCreamMachineP);
    params->ui->iceMachine.stateTimer = _UI_IceCreamMachine_StateErrTemp_Time;
}

void ceIceCreamMachineEnd(tsUiProcParams *params, stIceCreamMachine_Flash const *iceCreamMachineC,
                          stIceCreamMachine_Ram *iceCreamMachineP,
                          bool cmdReceived)
{
    if ((!cmdReceived) || (!iceCreamMachineC->type))
        return;

    IceCreamMachineEnd(iceCreamMachineP);
    params->ui->iceMachine.stateTimer = _UI_IceCreamMachine_StateIdle_Time;
}

static void IceCreamMachineOnOff(stIceCreamMachine_Ram *iceCreamMachineP)
{
    switch (iceCreamMachineP->values.state)
    {
    case _IceCreamMachineState_OFF:
        iceCreamMachineP->values.state = _IceCreamMachineState_Idle;
        sysBuzzerCmd(eSysBuzzerPlayOpenMelody);
        break;
    default:
        iceCreamMachineP->values.state = _IceCreamMachineState_OFF;
        sysBuzzerCmd(eSysBuzzerPlayCloseMelody);
        break;
    }
}

static void IceCreamMachineSet(stIceCreamMachine_Flash const *iceCreamMachineC, stIceCreamMachine_Ram *iceCreamMachineP)
{
    switch (iceCreamMachineP->sets.set)
    {
    case _IceCreamMachineSet_iceCream:
    default:
        if (iceCreamMachineP->values.state == _IceCreamMachineState_Idle)
        {
            iceCreamMachineP->sets.values.iceCream++;

            if (iceCreamMachineP->sets.values.iceCream >=
                iceCreamMachineC->sets.sizes.iceCream)
                iceCreamMachineP->sets.values.iceCream = 0;
        }
        break;
    }
}

static void IceCreamMachineStart(stIceCreamMachine_Ram *iceCreamMachineP)
{
    if (iceCreamMachineP->values.state == _IceCreamMachineState_Idle)
    {
        iceCreamMachineP->values.state = _IceCreamMachineState_Init;
        sysBuzzerCmd(eSysBuzzerPlayStartMelody);
    }
}

static void IceCreamMachineEnd(stIceCreamMachine_Ram *iceCreamMachineP)
{
    iceCreamMachineP->values.state = _IceCreamMachineState_Idle;
    iceCreamMachineP->E2.state = iceCreamMachineP->values.state;
    sysBuzzerCmd(eSysBuzzerPlayEndMelody);
}

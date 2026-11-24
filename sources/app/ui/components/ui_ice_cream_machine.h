#ifndef _UI_IceCreamMachine_H
#define _UI_IceCreamMachine_H

#include "../ui_proc.h"
void uiIceCreamMachineInit(const tsUiProcConsts *consts, tsUiProcParams *params);
extern void uiIceCreamMachine20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params);
extern void uiIceCreamMachine1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiIceCreamMachineTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiIceCreamMachineKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params);

#endif

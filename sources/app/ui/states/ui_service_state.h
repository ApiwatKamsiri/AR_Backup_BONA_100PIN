#ifndef _UI_Service_H
#define _UI_Service_H
#include "./service_mode_procedures/ui_sm_components.h"
#include "./service_mode_procedures/ui_sm_parameters.h"
#include "./service_mode_procedures/ui_sm_sensors.h"

#include "../ui_proc.h"

#ifdef SERVICEMODEavailable

void uiServiceStateInit(const tsUiProcConsts *consts, tsUiProcParams *params);

void uiServiceState20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiServiceState1MinTask(const tsUiProcConsts *consts, tsUiProcParams *params);

void uiServiceStateTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiServiceStateKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params);

#endif
#endif //SERVICEMODEavailable

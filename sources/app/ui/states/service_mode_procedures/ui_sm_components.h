#ifndef _Components_H
#define _Components_H
//#include "Display.h"
#include <stdbool.h>

#include "../../ui_proc.h"

#ifdef SERVICEMODEavailable

void uiSmCompsInit(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmComps20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmCompsCheckAll(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmCompsCloseAll(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmCompsSecureAll(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmCompsControlSelected(const tsUiProcConsts *consts, tsUiProcParams *params, bool change);
void uiSmCompsControlSpecials(const tsUiProcConsts *consts, tsUiProcParams *params, bool changeSpecials);

void uiSmCompsTestInit(const tsUiProcConsts *consts, tsUiProcParams *params);
// bool uiSmCompsTest(void);

void uiSmCompsSetFansCurrentIndexToInit(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmCompsSetFansCurrentIndexToLastIndex(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmCompsInitFanTables(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmCompsInitVCCFreqTable(const tsUiProcConsts *consts, tsUiProcParams *params);

#endif
#endif //SERVICEMODEavailable

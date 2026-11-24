#ifndef _UI_Distributer_H
#define _UI_Distributer_H
#define _UI_Distributer_AllLedsOn_Time          50  //  50 * 20ms = 1 sec   //Ezgi Kutlu: Versiyon revizyon s�resi ka�masin diye �nceki zaman arttirildi.
#define _UI_Distributer_VersionRevision_Time    50  //  75 * 20ms = 1,5 sec //Ezgi Kutlu 1s
#define _UI_Distributer_TempUnit_Time           50  //  50 * 20ms = 1 sec

#include "../ui_proc.h"

void uiDistributerState20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDistributerStateInit(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDistributerStateTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDistributerStateKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params);

#endif


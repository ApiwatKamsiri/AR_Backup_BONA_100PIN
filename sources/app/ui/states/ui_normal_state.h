#ifndef _UI_Normal_H
#define _UI_Normal_H

#define _UI_Normal_Error_Time 50           // 50  * 20ms = 1 sec
#define _UI_Normal_AllLedsOn_Time 100      // 100 * 20ms = 1,5 sec
#ifdef ALPS_H10H30
#define _UI_Normal_VersionRevision_Time 100 // 50  * 20ms = 1 sec //Ezgi Kutlu 1s
#else
#define _UI_Normal_VersionRevision_Time 50 // 50  * 20ms = 1 sec //Ezgi Kutlu 1s
#endif
#define _UI_Normal_TempUnit_Time 50        // 50  * 20ms = 1 sec
#define _UI_Normal_Sets_Time 250           // 250 * 20ms = 5 sec

#include "../ui_proc.h"

void uiNormalState20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiNormalStateInit(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiNormalStateTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiNormalStateKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params);
#ifdef HOSHI_H30
extern unsigned char User_MuteBuzzer;
#endif //HOSHI_H30
#endif

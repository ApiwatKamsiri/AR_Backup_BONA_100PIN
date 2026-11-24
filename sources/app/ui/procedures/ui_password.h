#ifndef _Password_H_
#define _Password_H_

#include <stdbool.h>

#include "../ui_proc.h"

void uiPassword1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiPasswordInit(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiPasswordTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiPasswordKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiPasswordResetStayTimer(const tsUiProcConsts *consts, tsUiProcParams *params);

void uiPasswordIncreaseKey(tsUiProcParams *params);
void uiPasswordDecreaseKey(tsUiProcParams *params);
void uiPasswordAcceptKey(tsUiProcParams *params);
bool uiIsPassword(tsUiProcParams *params, unsigned short password);

#endif

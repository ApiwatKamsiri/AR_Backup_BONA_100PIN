#ifndef _CE_Display_H
#define _CE_Display_H

#include <stdbool.h>
#include "../ui_proc.h"

void ceDisplayOff(tsUiProcParams *params, bool cmdReceived);
void ceDisplayKeyLock(tsUiProcParams *params, bool cmdReceived);
void ceDisplayKeyLockDspr(tsUiProcParams *params, bool cmdReceived);
void ceDisplaySetDim(tsUiProcParams *params, bool cmdReceived);
void ceDisplayHighTempAlarmOff(tsUiProcParams *params, bool cmdReceived);
void ceDisplayServiceButton(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived);

#endif

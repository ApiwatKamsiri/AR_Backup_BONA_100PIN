#ifndef _CE_Password_H
#define _CE_Password_H

#include <stdbool.h>
#include "../ui_proc.h"

void cePasswordIncreaseKey(tsUiProcParams *params, bool cmdReceived);
void cePasswordDecreaseKey(tsUiProcParams *params, bool cmdReceived);
void cePasswordAcceptKey(tsUiProcParams *params, bool cmdReceived);

#endif

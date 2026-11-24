#include "ce_password.h"
#include "../procedures/ui_password.h"

void cePasswordIncreaseKey(tsUiProcParams *params, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiPasswordIncreaseKey(params);
}

void cePasswordDecreaseKey(tsUiProcParams *params, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiPasswordDecreaseKey(params);
}

void cePasswordAcceptKey(tsUiProcParams *params, bool cmdReceived)
{
    if (!cmdReceived)
        return;
    uiPasswordAcceptKey(params);
}


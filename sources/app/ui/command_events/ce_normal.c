
#include "ce_normal.h"

void ceNormalSetState(tsUiProcParams *params, enUI_NormalStates to, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    tsUiRam *uiP = params->ui;

    // uiP->states.normal.state=_UI_NormalState_Sets_Init;
    // to=_UI_NormalState_Sets_Init;
    switch (to)
    {
    case _UI_NormalState_Password:
        if (uiP->password.stayTimer)
            uiP->states.normal.state = to;
        break;

    default:
        uiP->states.normal.state = to;
        break;
    }
}

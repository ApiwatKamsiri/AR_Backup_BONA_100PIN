
#include "ce_commercial.h"

void ceCommercialSetState(tsUiProcParams *params, enUI_CommercialStates to, bool cmdReceived)
{
    if (!cmdReceived)
        return;

    tsUiRam *uiP = params->ui;

     //uiP->states.commercial.state=_UI_CommercialState_Sets_Init;
     //to=_UI_CommercialState_Sets_Init;
    switch (to)
    {
    case _UI_CommercialState_Password:
        if (uiP->password.stayTimer)
            uiP->states.commercial.state = to;
        break;

    default:
        uiP->states.commercial.state = to;
        break;
    }
}

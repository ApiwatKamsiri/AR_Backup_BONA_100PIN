#include "cmd_check.h"

void getNewEvent(const tsUiProcConsts* consts, tsUiProcParams* params)
{
    params->ui->dspValues.receivedEvent = devDspGetEvent(consts->dsp);
}
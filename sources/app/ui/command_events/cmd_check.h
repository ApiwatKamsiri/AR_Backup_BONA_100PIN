#ifndef _CMDCheck_H
#define _CMDCheck_H

#include "../ui_proc.h"
#include "dev/display_spi.h"

#define isEvent(_cmd)    (params->ui->dspValues.receivedEvent == (_cmd))
#define clearEvent()    (params->ui->dspValues.receivedEvent = eDisplayEventNone)

void getNewEvent(const tsUiProcConsts* consts, tsUiProcParams* params);


#endif
      
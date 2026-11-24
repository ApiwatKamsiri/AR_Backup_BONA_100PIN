#ifndef UI_BUZZER_H
#define UI_BUZZER_H


#include "../ui_proc.h"
#include "dev/display_spi.h"

void uiBuzzerWarning(const tsUiProcConsts *consts, tsUiProcParams *params, bool isThereWarning);
void uiBuzzerDoorAlarmAHTH(const tsUiProcConsts *consts, tsUiProcParams *params, bool isThereWarning);  //APW : Create pattern door alaem for FUJI/HOSHI Gen2(AHTH need to common Gen1)
//void uiBuzzerCmd(const tsUiProcConsts *consts, teMainBoardCommand command);

#endif

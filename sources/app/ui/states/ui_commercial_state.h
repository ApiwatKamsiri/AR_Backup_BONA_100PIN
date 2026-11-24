/*** APW : Add CT heater software ***/
#ifndef _UI_Commercial_H
#define _UI_Commercial_H

#include "../ui_proc.h"

void uiCommercialStatusInit(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiCommercialStateInit(const tsUiProcConsts *consts, tsUiProcParams *params);       //APW : Update command CtmodeState and display in CT and others BUG about display

void uiCommercialState20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiCommercialState1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiCommercialState1MinTask(const tsUiProcConsts *consts, tsUiProcParams *params);   //APW : Update I2C driver for add CT state

void uiCommercialStateTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiCommercialStateKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params);

#endif
/*** APW : Add CT heater software ***/
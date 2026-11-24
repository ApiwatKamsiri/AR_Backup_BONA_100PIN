#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H

#include "../ui_proc.h"
#include "dev/display_spi.h"

#define _UI_KeyLockWarningCount 3

unsigned char uiDspCheckTemp(const tsUiProcConsts *consts, tsUiProcParams *params, signed char NTC_Value);

void uiDspVacuumBox(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspKeyLock(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspIceOff(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspIceCabinsSetsOff(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspIceCabinsSetsOn(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspFunctionsAtOffMode(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspFunctionsOff(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspFunctionsOn(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspAllOff(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspAllOn(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspCabinsQuicks(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspCabinsSets(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspCabinsSetsInCommercial(const tsUiProcConsts *consts, tsUiProcParams *params);     //APW : Add display setting in CT
void uiDspCabinsRealTemps(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspError(const tsUiProcConsts *consts, tsUiProcParams *params);

void uiDspCabinIcon(const tsUiProcConsts *consts, tsUiProcParams *params);

void uiDspCabinQuickLED(const tsUiProcConsts *consts, tsUiProcParams *params, unsigned char cabinIndex, teDisplayLedState ledx);
void uiDspIceCreamLED(const tsUiProcConsts *consts, tsUiProcParams *params, teDisplayLedState ledx);

void uiDspCabinData(const tsUiProcConsts *consts, tsUiProcParams *params, unsigned char data, unsigned char cabinIndex, teDisplayCabinDataTypes dataType);
void uiDspIceCreamData(const tsUiProcConsts *consts, tsUiProcParams *params, unsigned char data, teDisplayCabinDataTypes dataType);
signed char uiDspTemperature(const tsUiProcConsts *consts, tsUiProcParams *params, signed char ValuInC, unsigned char cabinIndex);
void uiDspTempUnit(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspDispenser(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiDspDoorsAtQuick(const tsUiProcConsts *consts, tsUiProcParams *params);

//#define SMALL_SINGLE_CABIN_DISPLAY

#endif

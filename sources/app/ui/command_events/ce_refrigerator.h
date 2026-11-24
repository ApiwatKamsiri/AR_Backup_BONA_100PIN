#ifndef CE_REFRIGERATOR_H
#define CE_REFRIGERATOR_H

#include "../ui_proc.h"

void ceRfTempUnitChange(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived);

void ceRfSabbath(const tsUiProcConsts *consts, tsUiProcParams *params,
				 bool cmdReceived);

void ceRfResetSabbath(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived);

void ceRfEcoExtra(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived);

void ceRfVacation(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived);

void ceRfRice(const tsUiProcConsts *consts, tsUiProcParams *params,
			  bool cmdReceived);

void ceRfApplianceOff(const tsUiProcConsts *consts, tsUiProcParams *params, bool cmdReceived);

extern void ceRfSilent(const tsUiProcConsts *consts, tsUiProcParams *params,
					   bool cmdReceived);

void ceRfKnobsCheck(const tsUiProcConsts *consts, tsUiProcParams *params);

void CabinVacation(const tsUiProcConsts *consts, tsUiProcParams *params,
				   stCabin_Flash const *cabinC, stCabin_Ram *cabinP, bool Vacation);
#ifdef HOSHI_H30
void ceRfTempSettingFine(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived);

void ceRfMuteBuzzer(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived);

void ceRfDemoMode(const tsUiProcConsts *consts, tsUiProcParams *params,
				  bool cmdReceived);

extern unsigned char User_MuteBuzzer;
#endif //HOSHI_H30   

#endif

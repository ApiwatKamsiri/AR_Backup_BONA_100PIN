#ifndef _CE_Cabin_H
#define _CE_Cabin_H

#include "../ui_proc.h"

void ceHandleLight(const tsUiProcConsts *consts, tsUiProcParams *params,
                   stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                   bool cmdReceived);

void ceHandleLightOnOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                   stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                   bool cmdReceived);
                   
void ceHandleLightSet(const tsUiProcConsts *consts, tsUiProcParams *params,
                   stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                   bool cmdReceived);

void ceVacuumBox(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                        bool cmdReceived);

void ceCabinConvertible(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                        bool cmdReceived);

void ceCabinFRZOff(const tsUiProcConsts *consts, tsUiProcParams *params,
                   stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                   bool cmdReceived);

void ceCabinChangeFRZMode(const tsUiProcConsts *consts, tsUiProcParams *params,
                          stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP,
                          bool cmdReceived);

void ceCabinSetUp(const tsUiProcConsts *consts, tsUiProcParams *params,
                    stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                    bool cmdReceived);

void ceCabinSetDown(const tsUiProcConsts *consts, tsUiProcParams *params,
                    stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                    bool cmdReceived);

void ceCabinMode(const tsUiProcConsts *consts, tsUiProcParams *params,
                 stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                 bool cmdReceived);

void ceCabinQuick(const tsUiProcConsts *consts, tsUiProcParams *params,
                  stCabin_Flash const *cabinC, stCabin_Ram *cabinP,
                  bool cmdReceived);

void ceCabinChangeCabin(const tsUiProcConsts *consts, tsUiProcParams *params,
                        stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP,
                        bool cmdReceived);

void ceCabinWorkSets(const stCabin_Flash *cabinC, stCabin_Ram *cabinP, stIceMachine_Ram *iceMachineP, stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);

void cabinWorkCompSpeedUp(const stCabin_Flash *cabinC, stCabin_Ram *cabinP, stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);

#endif

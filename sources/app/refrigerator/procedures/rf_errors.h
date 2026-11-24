#ifndef RF_ERRORS_H
#define RF_ERRORS_H


#include "../rf_ram.h"
#include "../rf_configs.h"

void rfErrorsCheckAll(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
void rfErrorsCheckAll_123(stRefrigerator_Flash const *rfC, stRefrigerator_Ram *rfP);
void rfCheckHighTempError(stRefrigerator_Ram *rfP);
void rfInitARMModeError(stRefrigerator_Ram *rfP);     //APW : Initial ARM mode
void rfARMModeErrorOutput20MsTask(stRefrigerator_Ram *rfP);     //APW : ARM mode output
#endif

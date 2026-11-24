#ifndef _Sensors_H
#define _Sensors_H
#include <stdbool.h>

#include "../../ui_proc.h"

#ifdef SERVICEMODEavailable

void uiSmSensCheckAll(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmSensControlSelected(const tsUiProcConsts *consts, tsUiProcParams *params);

#endif
#endif //SERVICEMODEavailable

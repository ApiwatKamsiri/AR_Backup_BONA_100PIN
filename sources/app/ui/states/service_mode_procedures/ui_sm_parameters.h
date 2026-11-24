#ifndef _CTRL_Parameters_H
#define _CTRL_Parameters_H
#include <stdbool.h>

#include "../../ui_proc.h"

#ifdef SERVICEMODEavailable

void uiSmParamsCheckAll(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiSmParamsControlSelected(const tsUiProcConsts *consts, tsUiProcParams *params, bool change);

#endif
#endif //SERVICEMODEavailable

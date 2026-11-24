/**
 ******************************************************************************
 * @file    uiP->serviceButton.h
 * @author  ECM Software Team - EBI
 * @version V01R00
 * @date    27/04/2020
 * @brief   This file contains all function prototypes, structures and
 * variables for the serviceButton project.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SERVICE_BUTTON_H_
#define SERVICE_BUTTON_H_

#include <stdbool.h>

/* Exported Variables ------------------------------------------------------- */
/** @addtogroup ServiceButton Exported Variables
 * @{
 */

/* Exported Functions ------------------------------------------------------- */
/** @addtogroup ServiceButton Exported Functions
 * @{
 */

#include "../ui_proc.h"

void uiServiceButton20MsTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiServiceButton1SecTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiServiceButton1MinTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiServiceButtonInit(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiServiceButtonTask(const tsUiProcConsts *consts, tsUiProcParams *params);
void uiServiceButtonKeyHandler(const tsUiProcConsts *consts, tsUiProcParams *params);

#endif
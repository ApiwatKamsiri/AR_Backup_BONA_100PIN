/** @file       rcos.c
 *  @brief      RCoS+ main file
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Umit Sahin
 *  @date       21.01.2020
 */
#define FILE_RCOS_C

#include "system.h"
#include "rcos.h"
#include "app/mw/rcos_connectivity.h"

#include "init/arch/renesas_ra2_init.h"
CORE_EVENTQUEUE_SIZE(512)
CORE_TICK_DEFAULT(1000)
CORE_WDT_DEFAULT(g_wdt0, 0)


void rcosMainInit(void);


void rcosMain(void)
{  
  platformInit();
  coreInit();
#if (CONNECTIVITY != NON_CONNECTED)
  safirStart();
#endif
  sysStart();           //cadd
  coreRun();
}


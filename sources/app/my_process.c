/** @file       my_process.c
 *  @brief      MY_PROCESS application program file
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     muhammed
 *  @date       2022-03-01
 */
#define FILE_MY_PROCESS_C

#include "my_process.h"
#include "fsl_port.h"
#include "board.h"
#include "dev/synchronizer.h"
#include "app/refrigerator/rf_configs.h"
#include "app/refrigerator/rf_ram.h"
#include "app/cooler/cooler.h"
#include "app/cabin/cabin.h"
#include "app/ui/ui_proc.h"
#include "app/mw/rf_backup.h"


//PROCESS_CABIN_CREATE(cabin1, 5, 0, &refrigerator_R.Cabin[0], &refrigerator_F.Cabin[0])
PROCESS_COOLER_CREATE(coolerProcess, 14, &refrigerator_R.Cooler, &refrigerator_F.Cooler, &sync)
PROCESS_CABIN_CREATE(cabinFfProcess, 15, 0, &refrigerator_R.Cabin[0], &refrigerator_F.Cabin[0], &sync)
PROCESS_CABIN_CREATE(cabinFrzProcess, 16, 0, &refrigerator_R.Cabin[1], &refrigerator_F.Cabin[1], &sync)
PROCESS_CABIN_CREATE(cabinJkrProcess, 17, 0, &refrigerator_R.Cabin[2], &refrigerator_F.Cabin[2], &sync)
PROCESS_CABIN_CREATE(cabinFlapProcess, 18, 0, &refrigerator_R.Cabin[3], &refrigerator_F.Cabin[3], &sync)
PROCESS_UI_CREATE(uiProcess, eProcessUi, &refrigerator_R.ui, &refrigerator_R, &refrigerator_F, display)

#define DEBUG_FILE_NAME ""

// #define DEBUG_PORT_
// DEBUG_START(0)


/**
 *  @addtogroup MY_PROCESS
 *  @{
 */



//
static int32_t temp[2];
static uint32_t ntcError[2];
static uint32_t ac_outputs[2];

static PT_THREAD(myProcessThread)
{
    tsMyProcessParams *params = process->parameters;
    const tsMyProcessConsts *consts = process->constants;
    UNUSED(params);
    UNUSED(consts);

    PT_BEGIN();

    PT_YIELD_TIMEOUT(2000);

    for (;;)
    {
        PT_YIELD_TIMEOUT(500);
        ac_outputs[0] = refrigerator_F.Cabin[0].components.blueLight.index;
        PT_YIELD_TIMEOUT(1000);
        ac_outputs[1] = refrigerator_R.Cabin[1].components.airTempSensor.valueInCExt;
        PT_YIELD_TIMEOUT(1000);
        ac_outputs[1] = 0;
        PT_YIELD_TIMEOUT(1000);
        ac_outputs[0] = 0;
    }

    PT_END();
}

// static const tsFanConfig fanConfig = {.fgPinUsed = true, .powerPinUsed = true, .pwmUsed = true, .rpm = 2100, .ppr = 2};

/// @brief  Initialization function of myProcess
PROCESS_INIT_PROTO(myProcessInit)
{
    devCpxInit(&sync, NULL);

    //processStart(&rfProcess);
    processStart(&coolerProcess);
    processStart(&cabinFfProcess);
    processStart(&cabinFrzProcess);
    processStart(&cabinFlapProcess);
    processStart(&cabinJkrProcess);
    processStart(&uiProcess);
    
    
    const tsDevIo *dev;

    dev = boardInitDevice(eDeviceTypeNTC, 0, NULL);
    devSyncAttach(&sync, eSyncTypeDeviceToRam, dev, &temp[0]);

    dev = boardInitDevice(eDeviceTypeNTC, 1, NULL);
    devSyncAttach(&sync, eSyncTypeDeviceToRam, dev, &temp[1]);

    dev = boardInitDevice(eDeviceTypeNTCErrorDetector, 0, NULL);
    devSyncAttach(&sync, eSyncTypeDeviceToRam, dev, &ntcError[0]);

    dev = boardInitDevice(eDeviceTypeNTCErrorDetector, 1, NULL);
    devSyncAttach(&sync, eSyncTypeDeviceToRam, dev, &ntcError[1]);

    dev = boardInitDevice(eDeviceTypeACOutputDelayable, 0, NULL);
    devSyncAttach(&sync, eSyncTypeRamToDevice, dev, &ac_outputs[0]);

    dev = boardInitDevice(eDeviceTypeACOutputDelayable, 1, NULL);
    devSyncAttach(&sync, eSyncTypeRamToDevice, dev, &ac_outputs[1]);

    //processStart(&rfBackupProc);
    process->initialized = 1; // If process needs other checks, set this another time
    
    threadStart(process, myProcessThread);
}

/// @brief  Deinitialization function of myProcess
PROCESS_DEINIT_PROTO(myProcessDeinit)
{
    tsMyProcessParams *params = process->parameters;
    const tsMyProcessConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    threadStop(process, process->threadFunction);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of myProcess
/*
static PROCESS_HANDLER_PROTO(myProcessHandler)
{
    tsMyProcessParams *params = process->parameters;
    const tsMyProcessConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
    case EVENT_NONE: // FILL WITH NEW EVENTS
        break;

    default:
        break;
    }
}
*/
/** @} */

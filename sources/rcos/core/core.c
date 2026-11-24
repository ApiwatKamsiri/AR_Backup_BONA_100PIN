/** @file       core.c
 *  @brief      RCoS+ main functions
 *  @copyright  (c) 2018-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Mehmet Baris
 *  @author     Deniz Fer
 *  @date       08.03.2016
 */
#define FILE_CORE_C

#include "rcos.h"
#include "porting.h"
#include "board/board.h"
#include "dev/ra2/spi_slave.h"
/* Reset record function*/
#ifdef ResetRecord
#include "app/mw/reset_record.h"
#endif
#define DEBUG_FILE_NAME "core"
#define DEBUG_PORT_EVENT BIT(0)
#define DEBUG_PORT_PROCESS BIT(1)
#define DEBUG_PORT_TIMER_CB BIT(2)
#define DEBUG_PORT_TIMER_EV BIT(3)
DEBUG_START(DEBUG_PORT_EVENT + DEBUG_PORT_PROCESS + DEBUG_PORT_TIMER_CB + DEBUG_PORT_TIMER_EV)

#if 0
#define CRITICAL_SECTION_ENTER() rcosTick.disable()
#define CRITICAL_SECTION_EXIT() rcosTick.enable()
#else
#define CRITICAL_SECTION_ENTER() globalIntDisable()
#define CRITICAL_SECTION_EXIT() globalIntEnable()
#endif

extern volatile tsDeque eventQueue; ///< This object must be created in main.c with CORE_EVENTQUEUE_SIZE macro

tProcessEnum processHiddenMin = PROCESS_NONE; ///< Minimum value that has been assigned to a hidden process,
                                              ///< this value changes with each hidden process started.

LIST_DL_CREATE(timerGenericList);  ///< List of generic sw timers
LIST_DL_CREATE(timerCallBackList); ///< List of callback sw timers
LIST_DL_CREATE(timerEventList);    ///< List of event sw timers
LIST_SL_CREATE(rcosVersionList);   ///< List of rcos modules
LIST_SL_CREATE(userVersionList);   ///< List of rcos modules

uint32_t timerTickCount = 0;
uint32_t timerStopWatch = 0;

LIST_DL_CREATE(coreProcessPresent); ///< List of processes that are on this cpu

const tsDevCom *rcosDebugDev __attribute__((weak)) = NULL;

/// @brief  System reset information structure
typedef struct
{
    uint16_t fresh;
    uint16_t reason; ///< Reason of last reset
    uint32_t data;   ///< Data related to last reset reason
} tsCoreSystemReset;

tsCoreSystemReset coreSysResetInfo; ///< System reset information

static inline void coreWdtRefresh(void);
static void coreSysReset(uint16_t reason, uint32_t data);
static uint8_t eventPostRaw(tProcessEnum source, tProcessEnum destination, tEventEnum event, const void *data, uint8_t length, teBool fifo);

/*********************************************************************
 *  INTERFACE FUNCTIONS
 */

uint8_t timerGenericStart(tsTimerGeneric *obj, uint32_t period)
{
    CRITICAL_SECTION_ENTER();

    obj->_cnt = period;
    ldlInsertTail(&timerGenericList, obj);

    CRITICAL_SECTION_EXIT();

    return EXIT_SUCCESS;
}

uint8_t timerGenericStop(tsTimerGeneric *obj)
{
    CRITICAL_SECTION_ENTER();

    obj->_cnt = 0;
    ldlDelete(&timerGenericList, obj);

    CRITICAL_SECTION_EXIT();

    return EXIT_SUCCESS;
}

uint8_t timerCallBackStart(tsTimerCallBack *obj, uint32_t period)
{
    if (!obj->callBack)
    {
        // Input parameters check
        coreSysReset(__LINE__, (uint32_t)obj);

        return EXIT_FAILURE;
    }

    CRITICAL_SECTION_ENTER();

    obj->_cnt = period;
    ldlInsertTail(&timerCallBackList, obj);

    CRITICAL_SECTION_EXIT();

    return EXIT_SUCCESS;
}

uint8_t timerCallBackStop(tsTimerCallBack *obj)
{
    CRITICAL_SECTION_ENTER();

    obj->_cnt = 0;
    ldlDelete(&timerCallBackList, obj);

    CRITICAL_SECTION_EXIT();

    return EXIT_SUCCESS;
}

uint8_t timerEventStart(tsTimerEvent *obj, uint32_t duration)
{
    CRITICAL_SECTION_ENTER();

    if (PROCESS_NONE == obj->post.destination)
    {
        CRITICAL_SECTION_EXIT();
        return EXIT_FAILURE;
    }

    obj->_cnt = duration;
    ldlInsertTail(&timerEventList, obj);

    CRITICAL_SECTION_EXIT();

    return EXIT_SUCCESS;
}

uint8_t timerEventStop(tsTimerEvent *obj)
{
    CRITICAL_SECTION_ENTER();

    obj->_cnt = 0;
    ldlDelete(&timerEventList, obj);

    CRITICAL_SECTION_EXIT();

    return EXIT_SUCCESS;
}

/** @brief      Reset stopwatch counter and enable stopwatch
 *  @warning    Stopwatch should be used only for reliability functions!
 *              Do not use for any other purposes!
 */
static inline void stopWatchStart(void)
{
    rcosTick.disable();

    timerStopWatch = timerTickCount;

    rcosTick.enable();
}

/** @brief      Disable stopwatch
 *  @warning    Stopwatch should be used only for reliability functions!
 *              Do not use for any other purposes!
 *  @return     Stopwatch count value
 */
static inline uint32_t stopWatchStop(void)
{
    rcosTick.disable();

    timerStopWatch = timerTickCount - timerStopWatch;

    rcosTick.enable();

    return timerStopWatch;
}

/** @brief      Process every generic timer in the list */
static inline void timerGenericListProcess(void)
{
    tsTimerGeneric *timer;
    tsTimerGeneric *nextTimer;

    for (timer = (void *)timerGenericList.head; timer != NULL; timer = nextTimer) /// Process each timer in list
    {
        nextTimer = (void *)timer->_li.tail;

        if (timer->_cnt)
        {
            timer->_cnt--;
        }
        if (!timer->_cnt) /// Timer ends
        {
            ldlDelete(&timerGenericList, timer);
        }
    }
}

/** @brief      Process every callback timer in the list */
static inline void timerCallBackListProcess(void)
{
    tsTimerCallBack *timer = (void *)timerCallBackList.head;

    while (timer)
    {
        tsTimerCallBack *next; // = (void *)timer->_li.tail;

        if (timer->_cnt)
        {
            timer->_cnt--;
        }
        if (!timer->_cnt) // Timer ends
        {
            if (timer->callBack)
            {
                timer->_cnt = timer->callBack(timer->parameter); // Function call
            }
            next = (void *)timer->_li.tail;
            if (0 == timer->_cnt)
            {
                // Called for the last time
                ldlDelete(&timerCallBackList, timer);
            }
            else
            {
                // Restart with new _cnt
            }
        }
        else
        {
            next = (void *)timer->_li.tail;
        }
        timer = next;
    }
}

/** @brief      Process every event timer in the list */
static inline void timerEventListProcess(void)
{
    tsTimerEvent *timer;
    tsTimerEvent *nextTimer;

    for (timer = (void *)timerEventList.head; timer != NULL; timer = nextTimer) /// Process each timer in list
    {
        nextTimer = (void *)timer->_li.tail;

        if (timer->_cnt)
        {
            timer->_cnt--;
        }
        if (!timer->_cnt) /// Timer ends
        {
            if (eventPostRaw(timer->post.source, timer->post.destination, timer->post.event, NULL, 0, TRUE) == EXIT_SUCCESS)
            {
                ldlDelete(&timerEventList, timer);
            }
        }
    }
}

/** @brief      1 ms timer interrupt subroutine
 */
CORE_TICK_PROTO(rcosTickIsr)
{
    timerTickCount++;

    /// Process generic timers
    timerGenericListProcess();

    /// Process function callbacks
    timerCallBackListProcess();

    /// Process event posts
    timerEventListProcess();
}

uint8_t processStart(tsProcess *process)
{
    if (!process->initialized)
    {
        CRITICAL_SECTION_ENTER();

        if (PROCESS_NONE == process->enumeration) /// Fix process enumeration for hidden processes
        {
            process->enumeration = --processHiddenMin;
            DEBUG_PRINT(DEBUG_PORT_PROCESS, "pid:%i hidden", process->enumeration);
        }

        ldlInsertTail(&coreProcessPresent, process); /// Remember processes that are on this cpu
        
        CRITICAL_SECTION_EXIT();
        
        process->init(process); /// Initialize process

        DEBUG_PRINT(DEBUG_PORT_PROCESS, "pid:%i start", process->enumeration);

        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

uint8_t processStop(tsProcess *process)
{
    if (process->initialized)
    {
        CRITICAL_SECTION_ENTER();

        process->deinit(process); /// Deinitialize process

        CRITICAL_SECTION_EXIT();

        DEBUG_PRINT(DEBUG_PORT_PROCESS, "pid:%i stop", process->enumeration);

        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

tsProcess *processFind(tProcessEnum enumeration)
{
    tsProcess *process;

    LIST_DL_FOREACH(process, &coreProcessPresent)
    {
        if (process->enumeration == enumeration)
        {
            return process;
        }
    }

    return NULL;
}

uint8_t threadStart(tsProcess *process, PT_THREAD((*threadFunction))) //
{
    if (!threadFunction)
    {
        // That's not a thread
        return EXIT_FAILURE;
    }

    if (process->threadFunction != 0)
    {
        // A thread is already running
        return EXIT_FAILURE;
    }

    process->tlc.post.source      = process->enumeration;
    process->tlc.post.destination = process->enumeration;
    process->tlc.post.event       = EVENT_PT;
    process->tlc.post.length      = 0;
    process->threadFunction       = threadFunction;
    process->thread.lc            = 0;

    eventPostRaw(process->enumeration, process->enumeration, EVENT_PT, NULL, 0, TRUE);

    return EXIT_SUCCESS;
}

uint8_t threadStop(tsProcess *process, PT_THREAD((*threadFunction)))
{
    if (!threadFunction)
    {
        // That's not a thread
        return EXIT_FAILURE;
    }

    if (process->threadFunction != threadFunction)
    {
        // This is not the thread you're looking for
        return EXIT_FAILURE;
    }

    // timerEventStop(&process->tlc); // do not stop timer event to kill thread itself
    process->thread.lc = 1; // Possibility of this local continuation to exist is close to none.
                            // Hence this will force the thread to continue from PT_END
    //process->threadFunction(process, &process->thread);

    return EXIT_SUCCESS;
}

#if CORE_INTERCOM
TIMER_EVENT_CREATE(rcomTev, PROCESS_NONE, PROCESS_NONE, EVENT_NONE)

/// @brief  Event handler for RCOM process
static PROCESS_HANDLER_PROTO(rcomHandler)
{
    tsRcomParams *params = process->parameters;

    if (eventCurrent.source == eventCurrent.destination)
    {
        uint8_t idx;

        for (idx = 0; idx < params->devCount; idx++)
        {
            if (0 == params->incoming[idx].destination)
            {
                // We're looking for an tsEventPost
                if (devComReceive(params->devList[idx], NULL, 0) >= sizeof(params->incoming[0]))
                {
                    devComReceive(params->devList[idx], &params->incoming[idx], sizeof(params->incoming[0]));
                }
            }
            else // if (0 == params->incoming[idx].destination)
            {
                // We're looking for the data of last tsEventPost
                if (devComReceive(params->devList[idx], eventData, params->incoming[idx].length)) // >= params->incoming[idx].length)
                {
                    if ((params->incoming[idx].destination < processHiddenMin) && // Block incoming events to pass into hidden process
                        (processFind(params->incoming[idx].destination)))
                    {
                        // Target process is here
                        eventPostRaw(params->incoming[idx].source, params->incoming[idx].destination, params->incoming[idx].event, eventData, params->incoming[idx].length, TRUE);
                    }
                    else
                    {
                        // Target process is not here
                        uint8_t jdx;

                        for (jdx = 0; jdx < params->mapCount; jdx++)
                        {
                            devComSend(params->devList[params->map[jdx].idx], &params->incoming[idx], sizeof(params->incoming[0]));
                            devComSend(params->devList[params->map[jdx].idx], eventData, params->incoming[idx].length);
                            break;
                        }

                        if (PROCESS_NONE == params->map[jdx].process)
                        {
                            // Target process is not in the map
                        }
                    }
                    memset(&params->incoming[idx], 0, sizeof(params->incoming[0]));
                } // if (devComReceive(params->devList[idx], eventData, params->incoming[idx].length))
            }     // else // if (0 == params->incoming[idx].destination)
        }         // for (idx = 0; params->devList[idx] != NULL; idx++)
        timerEventStart(&rcomTev, 10);
    }
    else if (PROCESS_NONE == eventCurrent.source)
    {
        uint8_t jdx;

        for (jdx = 0; jdx < params->mapCount; jdx++)
        {
            devComSend(params->devList[params->map[jdx].idx], eventData, sizeof(tsEventPost)); // tsEventPost as header
            devComSend(params->devList[params->map[jdx].idx], &eventData[sizeof(tsEventPost)],
                       eventCurrent.length - sizeof(tsEventPost)); // The rest is data
            break;
        }
    }
    else
    {
        // No-one else should send something here
    }
}

/// @brief  Init function for RCOM process
PROCESS_INIT_PROTO(rcomInit)
{
    tsRcomParams *params = process->parameters;

    rcomTev.post.destination = process->enumeration;
    rcomTev.post.source      = process->enumeration;

    PROCESS_STATE_CHANGE(process, rcomHandler);
    process->initialized = 1;

    memset(params->incoming, 0, sizeof(params->incoming[0]) * params->devCount); // Assumes process is initialized if devCount is bigger than 0
    timerEventStart(&rcomTev, 10);
}

/// @brief  Deinit function for RCOM process
PROCESS_DEINIT_PROTO(rcomDeinit)
{
    tsRcomParams *params = process->parameters;

    UNUSED(params);
    PROCESS_STATE_CHANGE(process, NULL);
    process->initialized = 0;

    timerEventStop(&rcomTev);
}

PROCESS_CREATE(rcomProcess, rcomInit, rcomDeinit, PROCESS_NONE, (void *)&rcosRcom, NULL)
#endif // #if CORE_INTERCOM

/** @brief  Post an event in raw format to be used by internal core functions
 *  @param  source      Enumeration of source event process
 *  @param  destination Enumeration of destination event process
 *  @param  event       Enumeration of event
 *  @param  data        Pointer to location of data that will accompany event
 *  @param  length      Length of data
 *  @param  fifo        TRUE: insert as FIFO, FALSE: insert as LIFO
 *  @retval EXIT_SUCCESS
 *  @retval EXIT_FAILURE
 */
static uint8_t eventPostRaw(tProcessEnum source, tProcessEnum destination, tEventEnum event, const void *data, uint8_t length, teBool fifo)
{
    tsProcess *process;
    tsEventPost eventObj;

    if (EVENT_PT == event) // if it is a Protothread event, there're some rules
    {
        if (source != destination)
        {
            return EXIT_FAILURE;
        }
        if (length)
        {
            return EXIT_FAILURE;
        }
    }

    eventObj.source      = source;
    eventObj.destination = destination;
    eventObj.event       = event;
    eventObj.length      = length;

    process = processFind(destination); // Find process object

    if (process)
    {
        // Process is here
        if (process->handlerCurrent ||
            (EVENT_PT == event))
        {
            CRITICAL_SECTION_ENTER();
            // It has an event handler
            if (DEQUE_FREE((tsDeque *)&eventQueue) >= (sizeof(tsEventPost) + length))
            {
                // There's enough space in eventQueue to hold the event
                if (fifo)
                {
                    FIFO_PUSH((tsDeque *)&eventQueue, &eventObj, sizeof(eventObj));
                    if (length)
                    {
                        FIFO_PUSH((tsDeque *)&eventQueue, data, length);
                    }
                }
                else
                {
                    eventObj.length = 0; // XXX: not fully tested
                    if (length)
                    {
                        LIFO_PUSH((tsDeque *)&eventQueue, data, length);
                    }
                    LIFO_PUSH((tsDeque *)&eventQueue, &eventObj, sizeof(eventObj));
                }

                // Update @ref _tsProcess::evCntLoad and @ref _tsProcess::evCntMax values for target process
                process->evCntLoad++;
                process->evCntMax = MAX(process->evCntLoad, process->evCntMax);
            }
            else // if (DEQUE_FREE((tsDeque *)&eventQueue) >= (sizeof(tsEventPost) + length))
            {
                // not enough space to insert event
                coreSysReset(__LINE__, ((uint32_t)destination << 16) | event); // Cause a system reset if eventQueue is full

                CRITICAL_SECTION_EXIT();
                return EXIT_FAILURE;
            }
            CRITICAL_SECTION_EXIT();
            return EXIT_SUCCESS;
        }
        else // if (process->handlerCurrent)
        {
            // process currently does not have an event handler
            return EXIT_FAILURE;
        }
    }
    else // if (process)
    {
        // Process is not here, distribute event to another RCoS on the system
        /// @todo Event distribution over system
#if CORE_INTERCOM
        tProcessEnum targetProcess;

        if (PROCESS_NONE != rcosRcom.extProcess)
        {
            targetProcess = rcosRcom.extProcess;
        }
        else
        {
            targetProcess = rcomProcess.enumeration;
        }

        if (PROCESS_NONE != targetProcess)
        {
            tsEventPost carrier;

            carrier.source      = PROCESS_NONE;
            carrier.destination = targetProcess;
            carrier.event       = EVENT_NONE; // RCOM event
            carrier.length      = length + sizeof(tsEventPost);

            CRITICAL_SECTION_ENTER();
            // It has an event handler
            if (DEQUE_FREE((tsDeque *)&eventQueue) >= (sizeof(tsEventPost) + sizeof(tsEventPost) + length))
            {
                // There's enough space in eventQueue to hold the event
                FIFO_PUSH((tsDeque *)&eventQueue, &carrier, sizeof(carrier));
                FIFO_PUSH((tsDeque *)&eventQueue, &eventObj, sizeof(eventObj));
                if (length)
                {
                    FIFO_PUSH((tsDeque *)&eventQueue, data, length);
                }
            }
            else // if (DEQUE_FREE((tsDeque *)&eventQueue) >= (sizeof(tsEventPost) + length))
            {
                // not enough space to insert event
                coreSysReset(__LINE__, ((uint32_t)destination << 16) | event); // Cause a system reset if eventQueue is full

                CRITICAL_SECTION_EXIT();

                return EXIT_FAILURE;
            }
            CRITICAL_SECTION_EXIT();

        } // if (PROCESS_NONE != targetProcess)
#endif    // #if CORE_INTERCOM
    }

    return EXIT_SUCCESS;
}

uint8_t eventPost(tProcessEnum destination, tEventEnum event, const void *data, uint8_t length)
{
    if (TRUE == isIsrActive())
    {
        return EXIT_FAILURE; /// You cannot use eventPost in ISR
    }

    return eventPostRaw(eventCurrent.destination, destination, event, data, length, TRUE);
}

uint8_t eventPostPone(void)
{
    if (TRUE == isIsrActive())
    {
        return EXIT_FAILURE; /// You cannot use eventPostPone in ISR
    }

    if (EVENT_PT == eventCurrent.event)
    {
        return EXIT_FAILURE; /// You cannot use eventPostPone in a thread
    }

    return eventPostRaw(eventCurrent.source, eventCurrent.destination, eventCurrent.event, eventData, eventCurrent.length, TRUE);
}

uint8_t eventPostInIsr(tProcessEnum destination, tEventEnum event)
{
    //if (TRUE == isIsrActive())
    {
        return eventPostRaw(PROCESS_NONE, destination, event, NULL, 0, FALSE);
    }

    return EXIT_FAILURE; /// eventPostInIsr returns EXIT_FAILURE either it is not supported for the selected platform
                         /// or it is not called inside an ISR
}

uint8_t eventReply(tEventEnum event, const void *data, uint8_t length)
{
    if (TRUE == isIsrActive())
    {
        return EXIT_FAILURE; /// You cannot use eventReply in ISR
    }

    if (EVENT_PT == eventCurrent.event)
    {
        return EXIT_FAILURE; /// You cannot use eventReply in a thread
    }

    if (PROCESS_NONE != eventCurrent.source)
    {
        return eventPostRaw(eventCurrent.destination, eventCurrent.source, event, data, length, TRUE);
    }

    return EXIT_FAILURE;
}

void coreInit(void)
{
    ADD_RCOS_VERSION("RCoSCore", "2.0.1")
    /// Watchdog timer init
    rcosWdt.refresh();
    rcosWdt.stop();

    /// rcos (event queue) init
    FIFO_FLUSH((tsDeque *)&eventQueue);

    devComInit(rcosDebugDev);
}

void coreRun(void)
{
#if CORE_INTERCOM
    if (PROCESS_NONE == rcosRcom.extProcess)
    {
        // No external process is defined for RCOM
        if (rcosRcom.devCount)
        {
            // Array has at least 1 devCom except the NULL termination
            uint8_t idx;

            for (idx = 0; idx < rcosRcom.devCount; idx++)
            {
                devComInit(rcosRcom.devList[idx]);
            }

            processStart(&rcomProcess); // Our own process to handle rcosRcom.devList
        }
    }
#endif // #if CORE_INTERCOM

    rcosWdt.start();
    rcosTick.start();

    for (;;)
    {
        coreWdtRefresh();
       /* while(1)
        {
        }*/
        // Process events
         if (!FIFO_IS_EMPTY(&eventQueue))
        {
            tsProcess *process;

            CRITICAL_SECTION_ENTER();
            FIFO_POP((tsDeque *)&eventQueue, &eventCurrent, sizeof(eventCurrent)); // Get event from queue
            if (eventCurrent.length)
            {
                FIFO_POP((tsDeque *)&eventQueue, &eventData, eventCurrent.length); // Get data from queue
            }
            CRITICAL_SECTION_EXIT();

            LIST_DL_FOREACH(process, &coreProcessPresent) // Find event process
            {
                if (process->enumeration == eventCurrent.destination)
                {
                    if ((process->enumeration == eventCurrent.source) &&
                        (EVENT_PT == eventCurrent.event))
                    {
                        if (process->threadFunction)
                        {
#ifdef ResetRecord
                            RST_CHECKPOINT(process->enumeration, EVENT_PT);                     //Record current event before call the threadFunction event
#endif
                            uint32_t period = process->threadFunction(process, &process->thread);
                            
#ifdef ResetRecord
                            RST_CHECKPOINT(0, 0);                                               //Clear Record current event after called the event
#endif

                            if ((PT_ENDED == period) ||
                                (PT_EXITED == period))
                            {
                                process->threadFunction = NULL;
                            }
                            else if ((PT_WAITING == period) ||
                                     (PT_YIELDED == period))
                            {
                                eventPost(process->enumeration, EVENT_PT, NULL, 0);
                            }
                            else
                            {
                                timerEventStart(&process->tlc, period);
                            }
                        }
                    }
                    else if (process->handlerCurrent)
                    {
                        uint16_t processTime; // An event processing should not take longer than a second

                        stopWatchStart();
#ifdef ResetRecord
                        RST_CHECKPOINT(process->enumeration, eventCurrent.event);               //Record current event before call the event
#endif
                        process->handlerCurrent(process); // Call event process handler
#ifdef ResetRecord
                        RST_CHECKPOINT(0, 0);                                                   //Clear Record current event after called the event
#endif
                        processTime = stopWatchStop();

                        /// @todo for SECURITY reasons eventData should be cleared
                        //memset(eventData, 0, sizeof(eventData)); // eventCurrent.length

                        // Remember the event that took the longest time to process
                        if (process->timeMax < processTime)
                        {
                            process->timeMax      = processTime;
                            process->timeMaxEvent = eventCurrent.event;
                            DEBUG_PRINT(DEBUG_PORT_PROCESS, "pid:%i timeMax:%i event:%i", process->enumeration, process->timeMax, eventCurrent.event);
                        }
                    }
                    process->evCntLoad--;
                    // if (processTime >= g_u16EventDurationMax)
                    // {
                    //     coreSysReset(__LINE__, ((uint32_t)process->enumeration << 16) | eventCurrent.event); // + processTime
                    // }

                    break;
                } // if (process->enumeration == eventCurrent.destination)
            }     // LIST_DL_FOREACH(process, &coreProcessPresent)
        }         // if (!FIFO_IS_EMPTY(&eventQueue))
    }             // for (;;)
}

int rcosDebugPrint(const char *format, ...)
{
    int length = 0;

    if (rcosDebugDev != NULL)
    {
        if (rcosDebugDev->sys->initialized)
        {
            va_list args;

            va_start(args, format);

            length = _devComVPrint(rcosDebugDev, format, args);

            va_end(args);
        }
    }

    return length;
}

/*********************************************************************
 *  LOCAL FUNCTIONS
 */

/** @brief      Check timer health, and refresh watchdog if timer is running properly or not if it's not
 *              NOTE: WDT period is set to 500ms (accuracy: +/-60%)
 */
static inline void coreWdtRefresh(void)
{
    if (!(~rcosWdtVars.flags & rcosWdtVars.flagMask))
    {
        /// All flags are set, everything is OK
        rcosWdt.refresh();
        rcosWdtVars.flags = 0;
    }
    else
    {
        // coreSysReset(__LINE__, rcosWdtVars.flags);
    }
}

/** @brief      Save reset reason and reset the entire system
 *  @param[in]  reason why we need a system reset
 *  @param[in]  data information about the reason of reset
 */
static void coreSysReset(uint16_t reason, uint32_t data)
{
    CRITICAL_SECTION_ENTER();

#ifdef ResetRecord
    RSTCoreReset(reason, data);  // coreSysResetInfo is a normal global and is zeroed at boot
#endif
    rcosWdt.start();
    rcosWdt.refresh();

    // Save reset reason and data here, and do other necessary flash operations as increasing counters etc. if needed
    // Warning: If it takes a long time, do not forget to refresh watchdog timer again and again until all data saved and flash operations completed
    coreSysResetInfo.reason = reason;
    coreSysResetInfo.data   = data;

    /// Wait for watchdog reset
    for (;;)
    {
        continue;
    }

    // CRITICAL_SECTION_EXIT();
}

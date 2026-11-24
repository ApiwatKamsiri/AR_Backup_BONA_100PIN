/** @file       consumption.c
 *  @brief      CONSUMPTION application program file
 *  @copyright  (c) 2023-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ozkan AKGUL
 *  @date       23.01.2023
 */
#define FILE_CONSUMPTION_C

#include "consumption.h"

#define DEBUG_FILE_NAME "consumption"

// #define DEBUG_PORT_
// DEBUG_START(0)

/**
 *  @addtogroup CONSUMPTION
 *  @{
 */
unsigned char timeOutErrorNB, errorNBtimes = 0;

static PT_THREAD(consumptionThread)
{
    tsConsumptionParams *params       = process->parameters;
    const tsConsumptionConsts *consts = process->constants;
    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED
    PT_BEGIN();

    for (;;)
    {
        PT_YIELD();
    }

    PT_END();
}

static PROCESS_HANDLER_PROTO(consumptionHandler);

/// @brief  Initialization function of consumption
PROCESS_INIT_PROTO(consumptionInit)
{
    tsConsumptionParams *params       = process->parameters;
    const tsConsumptionConsts *consts = process->constants;
    ADD_RCOS_VERSION("consumption", "1.0.0")

    UNUSED(consts); // REMOVE IF USED

    params->timerPeriodic.post.source = process->enumeration;
    params->timerPeriodic.post.destination = process->enumeration;
    params->timerPeriodic.post.event = eConsumptionEventsSendPeriodicData;

    timerEventStart(&params->timerPeriodic, 1000); /// calculate total consumption

    PROCESS_STATE_CHANGE(process, consumptionHandler);
    threadStart(process, consumptionThread);
    process->initialized = 1; // If process needs other checks, set this another time
}

/// @brief  Deinitialization function of consumption
PROCESS_DEINIT_PROTO(consumptionDeinit)
{
    tsConsumptionParams *params       = process->parameters;
    const tsConsumptionConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    PROCESS_STATE_CHANGE(process, NULL);
    threadStop(process, process->threadFunction);
    process->initialized = 0; // If process needs other checks, clear this another time
}

/// @brief  Event handler function of consumption
static PROCESS_HANDLER_PROTO(consumptionHandler)
{
    tsConsumptionParams *params       = process->parameters;
    const tsConsumptionConsts *consts = process->constants;

    UNUSED(params); // REMOVE IF USED
    UNUSED(consts); // REMOVE IF USED

    switch (eventCurrent.event)
    {
        case EVENT_NONE: // FILL WITH NEW EVENTS
            break;

        case eConsumptionEventsEmitConsumptionData:
        {
            ///< Input data structure acording to protocol
            ///<  Type[1] + Consumption[4] + Duration[4] + Unit[2] total size  = 11 byte
            tsConsumptionData data;
            memcpy(&data, eventData, sizeof(tsConsumptionData));
            eventPost(consts->gdmProcessEnum, eConsumptionEventsEmitConsumptionData, &data, sizeof(tsConsumptionData));
        }
        break;

        case eConsumptionEventsSendPeriodicData:
        {
            tsConsumptionElement * cElement;
            uint16_t index = 0;

            for (index = 0; index < consts->consumptionArraySize; index++)
            {
                cElement = (tsConsumptionElement *)&params->consumptionArray[index];
                if (cElement->sendingStatus != eSendingOptionsAuto) break;

                if (cElement->sendingState == eSendingStateDefault)
                {
                    cElement->timerCnt++;
                    cElement->totalConsumption += *(cElement->instantConsumption);
                    if (cElement->sendingLimit != 0)
                    {
                        ///< use sending limit
                        if ((cElement->totalConsumption / cElement->convConstant) >= cElement->sendingLimit)
                        {
                            ///< send to network board with its timer counter and reset all values (after getting cloud sent event)
                            tsConsumptionData data;
                            data.type = cElement->type;
                            data.totalConsumption = htonl(cElement->totalConsumption / cElement->convConstant);     ///< conver total value acordingly
                            data.totalDuration = htonl(5);//cElement->timerCnt / 60;   ///<    convert into minutes from seconds.
                            data.measurementUnit = cElement->measurementUnit;

                            eventPost(process->enumeration, eConsumptionEventsEmitConsumptionData, &data, sizeof(data));
                            cElement->sendingState = eSendingStateSentToNB;       ///<    data sent to NB - wait for response
                        }
                    }
                    else if (cElement->sendingPeriod != 0)
                    {
                        ///< use sending period for periodic send
                        if ((cElement->timerCnt / 60) >= cElement->sendingPeriod)
                        {
                            ///< send to network board with its timer counter and reset all values (after geting cloud sent event)
                            tsConsumptionData data;
                            data.type = cElement->type;
                            data.totalConsumption = cElement->totalConsumption / cElement->convConstant;     ///< conver total value acordingly
                            data.totalDuration = cElement->timerCnt / 60;   ///<    convert into minutes from seconds.
                            data.measurementUnit = cElement->measurementUnit;

                            eventPost(process->enumeration, eConsumptionEventsEmitConsumptionData, &data, sizeof(data));
                            cElement->sendingState = eSendingStateSentToNB;       ///<    data sent to NB - wait for response
                        }
                    }

                }
                else if (cElement->sendingState == eSendingStateSentToCloud)
                {
                    cElement->timerCnt = 0;     ///<    reset counter
                    cElement->totalConsumption = 0;     ///< reset total consumption value
                    timeOutErrorNB = 0; // reset
                    errorNBtimes = 0;
                    cElement->sendingState = eSendingStateDefault;                    
                }
                else if (cElement->sendingState == eSendingStateSentToNB)
                {
                  timeOutErrorNB++;
                  if(timeOutErrorNB > 60) // second
                  {
                    errorNBtimes++;
                    if(errorNBtimes > 5) // if error contiune along 5*60=300 second , 
                    {
                      cElement->timerCnt = 0; // then continue to collect data
                      errorNBtimes = 0;
                    }
          
                    cElement->sendingState = eSendingStateDefault;
                    timeOutErrorNB = 0;
                    break;
                  }
                    ///<    TODO : add timeout value here if there is no response from NB as successfully sent to cloud.
                    ///<    TODO : add NVM functions to this part
                }
            }

            timerEventStart(&params->timerPeriodic, 1000);      ///< every 1 second periodic
        }
        break;
        case eConsumptionEventsConsumptionDataPublished:
        {
            ///< this event called by GDM - NB complated to send data to cloud
            tsPublishResultData response;
            response.type = eventData[0];               ///< get type
            response.publishResult = eventData[1];      ///< get publish result

            tsConsumptionElement * cElement;
            uint16_t index = 0;

            for (index = 0; index < consts->consumptionArraySize; index++)
            {
                cElement = (tsConsumptionElement *)&params->consumptionArray[index];
                if (cElement->type == response.type)
                {
                    if (cElement->sendingStatus == eSendingOptionsManual)
                    {
                        ///<    Continue with application process
                        ///<    Send cloud sent notification to app
                        eventPost(consts->targetProcessEnum, consts->targetEvent, &response, sizeof(response));
                    }
                    
                    ///<    continue to send next - before that reset all in periodic event - cloud sent ok
                    cElement->sendingState = eSendingStateSentToCloud;
                    break;
                }
            }
        }
        break;

        case eConsumptionEventsSendConsumptionDataManual:
        {
            ///<    This message is coming from application - send it to NB 
            tsConsumptionData data;
            memcpy(&data, eventData, sizeof(tsConsumptionData));
            eventPost(consts->gdmProcessEnum, eConsumptionEventsEmitConsumptionData, &data, sizeof(data));
        }
        break;

        case eConsumptionEventsReadConsumptionData:
        {
            ///<    This message is coming from application - response it.
            ///<    If it is not assigned as auto do not response.
            teConsumptionTypes type = eventData[0];

            tsConsumptionElement * cElement;
            uint16_t index = 0;

            for (index = 0; index < consts->consumptionArraySize; index++)
            {
                cElement = (tsConsumptionElement *)&params->consumptionArray[index];
                if (cElement->type == type)
                {
                    if (cElement->sendingStatus == eSendingOptionsAuto)
                    {
                        ///< send to network board with its timer counter and reset all values (after geting cloud sent event)
                        tsConsumptionData data;
                        data.type = cElement->type;
                        data.totalConsumption = cElement->totalConsumption / cElement->convConstant;     ///< conver total value acordingly
                        data.totalDuration = cElement->timerCnt / 60;   ///<    convert into minutes from seconds.
                        data.measurementUnit = cElement->measurementUnit;

                        eventPost(process->enumeration, eConsumptionEventsEmitConsumptionData, &data, sizeof(data));

                        cElement->sendingState = eSendingStateSentToNB;       ///<    data sent to NB - wait for response

                        break;
                    }
                }
            }
        }
        break;

        default:
            break;
    }
}

/** @} */

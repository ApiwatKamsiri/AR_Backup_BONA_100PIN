/** @file       rassert.c
 *  @brief      Assert macros
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Burak OZAN
 *  @date       06.06.2022
 */
#define FILE_RASSERT_C

/** INCLUDES ******************************************************************/
#include "rassert.h"

/** CONSTANTS *****************************************************************/

/** TYPEDEFS ******************************************************************/

/** MACROS ********************************************************************/

/** VARIABLES *****************************************************************/
DEQUE_CREATE(assertLogQueue, ASSERT_LOG_QUEUE_SIZE * sizeof(tsAssertLog)) //Stores assert logs, not sorted by priority

/** LOCAL FUNCTION DECLARATIONS ***********************************************/

/** INTERFACE FUNCTION DEFINITIONS ********************************************/
/// @brief  add Rcos version
void addRassertVersion(void) //(BO) Bu fonksiyon nerede çağırılacak?
{
    // add version of the assert component
    ADD_RCOS_VERSION("rassert", "1.0.1");
}

/// @brief  dummy function returns directly
void rcosAssertDummy(uint8_t condition)
{
    return;
}

/// @brief  If it will be an assert errors, it stores the source file's name and line number
void rcosAssertLog(uint8_t condition, teAssertPriority priority, const char *file, uint16_t line)
{
    if (!condition)
    {
        tsAssertLog tempAssertLog = {{0}};
        uint8_t logIndex = 0, lowestPriorityIndex = 0;
        teAssertPriority lowestPriorityInQueue = RASSERT_PRI_LOW;

        /***Save the assertLog********************************************************************************************/
        tempAssertLog.priority = priority; //priority
        //
        if (strlen(file) > ASSERT_FILE_NAME_SIZE)
        {
            strncpy(tempAssertLog.info.fileName, file + strlen(file) - strlen(".c") - ASSERT_FILE_NAME_SIZE, ASSERT_FILE_NAME_SIZE);
        }
        else
        {
            strncpy(tempAssertLog.info.fileName, file, ASSERT_FILE_NAME_SIZE);
        }
        //
        tempAssertLog.info.lineNumber = line < UINT16_MAX ? line : UINT16_MAX;
        /***Check if the assert exist or not and find the lowest priority log*********************************************/
        tsAssertLog *tempOldAssertLogPtr = (tsAssertLog *)(assertLogQueue.buffer);
        lowestPriorityInQueue            = tempOldAssertLogPtr->priority;
        for (logIndex = 0, lowestPriorityIndex = logIndex; logIndex < DEQUE_LOAD(&assertLogQueue) / sizeof(tsAssertLog); logIndex++, tempOldAssertLogPtr++)
        {
            if (memcmp(tempOldAssertLogPtr, &tempAssertLog, sizeof(tsAssertLog)) == 0)
            {
                return;
            }
            if (tempOldAssertLogPtr->priority < lowestPriorityInQueue)
            {
                lowestPriorityInQueue = tempOldAssertLogPtr->priority;
                lowestPriorityIndex   = logIndex;
            }
        }
        /***Add assertLog to queue considering the priority***************************************************************/
        if (!DEQUE_IS_FULL(&assertLogQueue))
        {
            dequePushBack(&assertLogQueue, &tempAssertLog, sizeof(tempAssertLog)); // Add a new one
        }
        else
        {
            if (tempAssertLog.priority > lowestPriorityInQueue) //If new assert has higher priority than an assert at queue
            {
                tsAssertLog *tempOldAssertLogPtr = ((tsAssertLog *)(assertLogQueue.buffer)) + lowestPriorityIndex;
                memcpy(tempOldAssertLogPtr, &tempAssertLog, sizeof(tsAssertLog)); //Update log with new
            }
        }
    }
}

/** LOCAL FUNCTION DEFINITIONS ************************************************/

/** @file       version.c
 *  @brief      RCoS versioning
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Ozkan AKGUL
 *  @date       07.07.2020
 */
#define FILE_VERSION_C

/** INCLUDES ******************************************************************/
#include "version.h"

/** CONSTANTS *****************************************************************/

/** TYPEDEFS ******************************************************************/

/** MACROS ********************************************************************/

/** VARIABLES *****************************************************************/

/** LOCAL FUNCTION DECLARATIONS ***********************************************/

/** INTERFACE FUNCTION DEFINITIONS ********************************************/
void addVerVersion()
{
    // add version of the version component
    ADD_RCOS_VERSION("version", "2.0.0");
}

void addVersionItem(tsLsl *versionList, tsVersionListItem *newCompVersion)
{
    // check version items
    if (newCompVersion->_li.next)
    {
        // already in list
        return;
    }
    else if (versionList->next == NULL)
    {
        // list is empty, add to list as first item
        lslInsertTail(versionList, newCompVersion);
    }
    else if (lslTailFrom(versionList->next) == newCompVersion)
    {
        // already in list as tail item
        return;
    }
    else
    {
        // add to list
        lslInsertTail(versionList, newCompVersion);
    }
}

/** LOCAL FUNCTION DEFINITIONS ************************************************/

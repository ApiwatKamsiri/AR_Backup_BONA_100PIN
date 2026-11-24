/**
 *  @file       rassert.h
 *  @brief      Assert macros
 *  @copyright  (c) 2022-Arcelik - All Rights Reserved
 *  @author     Burak OZAN
 *  @date       06.06.2022
 */

//If you want to use assert you should define RASSERT_LEVEL, ASSERT_FILE_NAME_SIZE, ASSERT_LOG_QUEUE_SIZE at proj_config.h
//!!! calling a function in RCOS_ASSERT macro is not recommended !!!

/** Example proj config 
#define RASSERT_LEVEL   (RASSERT_LEVEL_1_LOG)
#ifdef FILE_ADC_C
    #define RASSERT_LEVEL   (RASSERT_LEVEL_0_OFF)
#endif
#ifdef FILE_DATA_FLASH_C
    #define RASSERT_LEVEL   (RASSERT_LEVEL_0_OFF)
#endif    
#ifdef FILE_PWT_C
    #define RASSERT_LEVEL   (RASSERT_LEVEL_2_DEBUG)
#endif   
#define ASSERT_FILE_NAME_SIZE	(8)
#define ASSERT_LOG_QUEUE_SIZE	(3) **/

#ifndef FILE_RASSERT_H
#define FILE_RASSERT_H

/** @addtogroup LIBS
 *  @{
 *      @defgroup   Assert
 *      @brief      Assert macro
 *  @}
 *
 *  @addtogroup VERSION
 *  @{
 */

/** INCLUDES ******************************************************************/
#include "libs.h"
#include <string.h>
#include <assert.h>
#include "deque.h"
//#include "proj_config.h"

#ifndef FILE_RASSERT_C
#define INTERFACE extern
#else
#define INTERFACE
#endif
/** CONSTANTS *****************************************************************/
#define RASSERT_LEVEL_0_OFF (0)   //Call a dummy function has no affect
#define RASSERT_LEVEL_1_LOG (1)   //If a condition is not correct saves the file and line info considering the priority
#define RASSERT_LEVEL_2_DEBUG (2) //!!! Not recommended at production time because if an assert error occurs the standart assert function blocks the code

typedef enum
{
    RASSERT_PRI_LOW = 0,
    RASSERT_PRI_HIGH,
    RASSERT_PRI_RESET,
} teAssertPriority;

#ifndef RASSERT_LEVEL
#define RASSERT_LEVEL (RASSERT_LEVEL_0_OFF)
#endif

#ifndef ASSERT_FILE_NAME_SIZE
#define ASSERT_FILE_NAME_SIZE (8)
#endif

#ifndef ASSERT_LOG_QUEUE_SIZE
#define ASSERT_LOG_QUEUE_SIZE (3)
#endif

/** TYPEDEFS ******************************************************************/
typedef struct
{
    char fileName[ASSERT_FILE_NAME_SIZE];
    uint16_t lineNumber;
} tsAssertInfo;

typedef struct
{
    tsAssertInfo info;
    teAssertPriority priority;
    uint8_t spareChar;
} tsAssertLog;

/** MACROS ********************************************************************/
#if RASSERT_LEVEL == RASSERT_LEVEL_0_OFF
//#warning assert has level 0
#define RCOS_ASSERT(_condition, _pritority) \
    rcosAssertDummy(_condition) //!!! calling a function in RCOS_ASSERT macro is not recommended !!!

#elif RASSERT_LEVEL == RASSERT_LEVEL_1_LOG
//#warning assert has level 1
/* 	"--no_path_in_file_macros" command should be added to 
	"Project > Options > C/C++ Compiler > Extra Options" on IAR.
	Otherwise __FILE__ macro will include all path on your
	computer and waste data space!!!*/
#if 0
/* 	If __FILE__ includes full path __FILENAME__ can be used, 
	but it take space from code memory*/
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#define RCOS_ASSERT(_condition, _pritority) \
    rcosAssertLog(_condition, _pritority, __FILE__, __LINE__) //!!! calling a function in RCOS_ASSERT macro is not recommended !!!

#elif RASSERT_LEVEL == RASSERT_LEVEL_2_DEBUG
//#warning assert has level 2
/* 	Assert yapilan yerlerde debug mesaji ciksin
	veya wdt reset atsin isteniyorsa bu makro aktif edilebilir.
	Uretim yazilimlari icin bu makro kapatilmali!!!*/
#define RCOS_ASSERT(_condition, _pritority) \
    assert(_condition) //!!! calling a function in RCOS_ASSERT macro is not recommended !!!

#endif

/** VARIABLES *****************************************************************/
extern tsDeque assertLogQueue;

/** INTERFACE FUNCTION DECLARATIONS ********************************************/
INTERFACE void addRassertVersion(void);
INTERFACE void rcosAssertDummy(uint8_t condition);
INTERFACE void rcosAssertLog(uint8_t condition, teAssertPriority priority, const char *file, uint16_t line);

#undef INTERFACE

#endif // FILE_RASSERT_H

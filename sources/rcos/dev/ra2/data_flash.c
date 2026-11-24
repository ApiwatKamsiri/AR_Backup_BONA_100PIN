/** @file       data_flash.c
 *  @brief      C file of FLASH device
 *  @copyright  (c) 2020-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Vehbi Umur Cabuk
 *  @date       05.05.2021
 *  @author     Burak OZAN
 *  @date       05.05.2022
 */
#define FILE_DATA_FLASH_C

#include "data_flash.h"
#include <string.h>

#define FLASH_DF_BLOCK_0               0x40100000U /*   1 KB: 0x40100000 - 0x401003FF */ // total 4KB // 1 BYTE programming max 356 us
#define FLASH_DATA_BLOCK_SIZE          (1024)

static volatile _Bool g_b_flash_event_not_blank = false;
static volatile _Bool g_b_flash_event_blank = false;
static volatile _Bool g_b_flash_event_erase_complete = false;
static volatile _Bool g_b_flash_event_write_complete = false;

/*******************************************************************************************************************//**
 * @brief       This functions checks the flag for blank check.
 * @param[IN]   block_num				Block number of Data-Flash.
 * @retval      FLASH_RESULT_BLANK		If block is blank.
 * @retval		FLASH_RESULT_NOT_BLANK	If block is not blank.
 **********************************************************************************************************************/
flash_result_t flash_lp_wait_for_blankcheck_flag(void)
{
    /* Wait for callback function to set flag */
    while (!(g_b_flash_event_not_blank||g_b_flash_event_blank))
    {
        ;
    }
    if (g_b_flash_event_not_blank)
    {
        /* Reset flag */
        return FLASH_RESULT_NOT_BLANK;
    }
    else
    {
        /* Reset flag */
        return FLASH_RESULT_BLANK;
    }
}

#ifdef DATA_FLASH_CGC_ACTIVE

/// @brief  To write or open flash module, switch the clock to internal if it is external
static uint8_t checkClockBeforeFlashOperation(const cgc_instance_t  * g_cgc,
                                             cgc_clock_t * const    p_clock_source)
{
    cgc_divider_cfg_t temp_p_divider_cfg;
    fsp_err_t err = FSP_SUCCESS;

    if(((cgc_instance_ctrl_t *)(g_cgc->p_ctrl))->open == 0)
    {//Open cgc module if it is not
        err = g_cgc->p_api->open(g_cgc->p_ctrl, g_cgc->p_cfg);
        RCOS_ASSERT(FSP_ERR_ASSERTION != err, RASSERT_PRI_LOW);
    }
    //Get system clock source       
                                        
    err = g_cgc->p_api->systemClockGet(g_cgc->p_ctrl, p_clock_source, /*(cgc_divider_cfg_t * const )*/(&temp_p_divider_cfg));
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    //If prev clock source is HOCO, there is no need for clock set 
    if(*p_clock_source != CGC_CLOCK_HOCO)
    {//IF it is not HOCO, switch to HOCO to able to write data flash
        err = g_cgc->p_api->systemClockSet(g_cgc->p_ctrl, CGC_CLOCK_HOCO, /*(cgc_divider_cfg_t * const )*/(&temp_p_divider_cfg));
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    }

    return EXIT_SUCCESS;
}

/// @brief  Set back to previous clcok if it has been changed
static uint8_t checkClockAfterFlashOperation(const cgc_instance_t  * g_cgc,
                                             cgc_clock_t * const     prev_p_clock_source)
{
    cgc_divider_cfg_t temp_p_divider_cfg;
    cgc_clock_t temp_p_clock_source;
    fsp_err_t err = FSP_SUCCESS;
    
    //If prev clock source is HOCO, there is no need for clock set 
    if(*prev_p_clock_source != CGC_CLOCK_HOCO)
    {
        if(((cgc_instance_ctrl_t *)(g_cgc->p_ctrl))->open == 0)
        {//Open cgc module if it is not
            err = g_cgc->p_api->open(g_cgc->p_ctrl, g_cgc->p_cfg);
            RCOS_ASSERT(FSP_ERR_ASSERTION != err, RASSERT_PRI_LOW);
        }
        //Getting clock divider configuration first
        err = g_cgc->p_api->systemClockGet(g_cgc->p_ctrl, (cgc_clock_t * const)(&temp_p_clock_source), (cgc_divider_cfg_t * const )(&temp_p_divider_cfg));
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
        //Set back the previous clock source
        err = g_cgc->p_api->systemClockSet(g_cgc->p_ctrl, *prev_p_clock_source, (cgc_divider_cfg_t * const )(&temp_p_divider_cfg));
        RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    }

    return EXIT_SUCCESS;
}

#endif

/**
 *  @addtogroup KE04_FLASH
 *  @{
 */

static DEV_CPX_FUNC_INIT(flashInit);
static DEV_CPX_FUNC_DEINIT(flashDeinit);

/// @brief  Structure that defines the functions for flash
const tsDevCpxFuncs devCpxFlashFuncs =
    {
        flashInit,
        flashDeinit,
};


#define MCU_SECTOR_SIZE (BSP_FEATURE_FLASH_LP_CF_BLOCK_SIZE) 
#define OTA_FRAME_SIZE 266
#define OTA_FLASH_METADATA_ROW_NUM 0x3FE
#define MCU_ROW_SIZE (0x100) // this is fixed.
#define MCU_SECTOR_ROW_COUNT (MCU_SECTOR_SIZE / MCU_ROW_SIZE)

/// @brief  Init function for a flash
static DEV_CPX_FUNC_INIT(flashInit)
{
    tsFlashParams *params = device->parameters;
#ifdef DATA_FLASH_CGC_ACTIVE
    cgc_clock_t temp_p_clock_source;
#endif
    fsp_err_t err = FSP_SUCCESS;
    ADD_RCOS_VERSION("ra2/flash", "2.0.1")

    UNUSED(config);
    
    globalIntDisable();
    
#ifdef DATA_FLASH_CGC_ACTIVE
    checkClockBeforeFlashOperation(params->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif

    err = R_FLASH_LP_Open(params->config->p_ctrl, params->config->p_cfg); 
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    if (FSP_SUCCESS != err)
    {
#ifdef DATA_FLASH_CGC_ACTIVE
        checkClockAfterFlashOperation(params->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif
        globalIntEnable();
        return EXIT_FAILURE;
    }
    
#ifdef DATA_FLASH_CGC_ACTIVE
    checkClockAfterFlashOperation(params->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif

    globalIntEnable();
    device->sys->initialized = TRUE;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a flash
static DEV_CPX_FUNC_DEINIT(flashDeinit)
{
    tsFlashParams *params = device->parameters;
    fsp_err_t err = FSP_SUCCESS;

    err = R_FLASH_LP_Close(params->config->p_ctrl);
    RCOS_ASSERT(FSP_SUCCESS == err, RASSERT_PRI_LOW);
    if (FSP_SUCCESS != err)
    {
        return EXIT_FAILURE;
    }

    device->sys->initialized = FALSE;
    UNUSED(params); // REMOVE IF USED

    return EXIT_SUCCESS;
}

/**
 *  @addtogroup RA2_SECTION
 *  @{
 */

static DEV_MEM_FUNC_INIT(sectionInit);
static DEV_MEM_FUNC_DEINIT(sectionDeinit);
static DEV_MEM_FUNC_READ(sectionRead);
static DEV_MEM_FUNC_WRITE(sectionWrite);
static DEV_MEM_FUNC_PROGRAM(sectionProgram);
static DEV_MEM_FUNC_ERASE(sectionErase);
static DEV_MEM_FUNC_TICKET_GET(sectionTicketGet);
static DEV_MEM_FUNC_TICKET_VALID(sectionTicketValid);

/// @brief  Structure that defines the functions for section
const tsDevMemFuncs devMemSectionFuncs =
    {
        sectionInit,
        sectionDeinit,
        sectionRead,
        sectionWrite,
        sectionProgram,
        sectionErase,
        sectionTicketGet,
        sectionTicketValid,
};

/// @brief  Init function for a section
static DEV_MEM_FUNC_INIT(sectionInit)
{
    tsSectionParams *params = device->parameters;

    if (!params->flash->sys->initialized)
    {
        if (devCpxInit(params->flash, NULL) != EXIT_SUCCESS)
        {
            return EXIT_FAILURE;
        }
    }

    device->sys->initialized = TRUE;

    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a section
static DEV_MEM_FUNC_DEINIT(sectionDeinit)
{
    tsSectionParams *params = device->parameters;

    if (params->flash->sys->initialized)
    {
        if (devCpxDeinit(params->flash) != EXIT_SUCCESS)
        {
            return EXIT_FAILURE;
        }
    }

    device->sys->initialized = FALSE;

    return EXIT_SUCCESS;
}

/// @brief  Read function for a section
static DEV_MEM_FUNC_READ(sectionRead)
{
    tsSectionParams *params = device->parameters;
    tsDevMemSystem *sys     = device->sys;

    if (!sys->initialized)
    {
        return 0;
    }

    // This is fail. Out of range of page.
    if ((address + length) >= params->areaSize)
    {
        return 0;
    }

    memcpy(readData, params->area + address, length);

    TICKET_REMOVE(params->flash->sys->tvm);

    return length;
}

/// @brief  Write function for a section
static DEV_MEM_FUNC_WRITE(sectionWrite)
{
    tsSectionParams *params = device->parameters;
    tsDevMemSystem *sys     = device->sys;
    fsp_err_t result;

    if (!sys->initialized)
    {
        return 0;
    }

    // This is fail. Out of range of page.
    if ((address + length) >= params->areaSize)
    {
        return 0;
    }

    {
        const uint8_t *srcPtr = writeData;
        uint32_t sectorBgn    = (((uint32_t)params->area + address) / params->sectorSize) * params->sectorSize; // Beginning of the sector of starting physical address
        uint16_t total        = length;
#ifdef DATA_FLASH_CGC_ACTIVE
        cgc_clock_t temp_p_clock_source;
#endif

        address = (uint32_t)params->area + address; // Update address variable to hold the physical address

        while (total)
        {
            uint16_t opLength = (params->sectorSize - (address % params->sectorSize)); // Length until end of sector
            opLength          = MIN(total, opLength);                                  // Operation length for this sector

            memcpy(params->sectorBuffer, (void *)sectorBgn, params->sectorSize);  // Get current sector data
            memcpy(&params->sectorBuffer[address - sectorBgn], srcPtr, opLength); // Overwrite new data

//            globalIntDisable();

#ifdef DATA_FLASH_CGC_ACTIVE
            checkClockBeforeFlashOperation(params->fparams->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif

            //result = R_FLASH_LP_Erase(params->fparams->config->p_ctrl, sectorBgn, 1); // Erase sector
            result = R_FLASH_LP_Erase(params->fparams->config->p_ctrl, sectorBgn, 1); // Erase sector

            RCOS_ASSERT(FSP_SUCCESS == result, RASSERT_PRI_LOW);
            if (FSP_SUCCESS != result)
            {
#ifdef DATA_FLASH_CGC_ACTIVE
                checkClockAfterFlashOperation(params->fparams->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif
//                globalIntEnable();
                return 0;
            }

            flash_status_t status;
            /* Wait until the current flash operation completes. */
            do
            {
                result = R_FLASH_LP_StatusGet(params->fparams->config->p_ctrl, &status);
            } while ((FSP_SUCCESS == result) && (FLASH_STATUS_BUSY == status));

            //globalIntDisable();
            result = R_FLASH_LP_Write(params->fparams->config->p_ctrl, (uint32_t)params->sectorBuffer, sectorBgn, params->sectorSize); // Program sector
            
#ifdef DATA_FLASH_CGC_ACTIVE
            checkClockAfterFlashOperation(params->fparams->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif
            
//            globalIntEnable();

            RCOS_ASSERT(FSP_SUCCESS == result, RASSERT_PRI_LOW);
            if (FSP_SUCCESS != result)
            {
                return 0;
            }
            
            /* Wait until the current flash operation completes. */
            do
            {
                result = R_FLASH_LP_StatusGet(params->fparams->config->p_ctrl, &status);
            } while ((FSP_SUCCESS == result) && (FLASH_STATUS_BUSY == status));

            sectorBgn += params->sectorSize;
            total -= opLength;
            address += opLength;
            srcPtr += opLength;
        }
    }

    TICKET_REMOVE(params->flash->sys->tvm);

    return length;
}

/// @brief  Program function for a section
static DEV_MEM_FUNC_PROGRAM(sectionProgram)
{
    tsSectionParams *params = device->parameters;
    tsDevMemSystem *sys     = device->sys;
    fsp_err_t result;
#ifdef DATA_FLASH_CGC_ACTIVE
    cgc_clock_t temp_p_clock_source;
#endif

    if (!sys->initialized)
    {
        return 0;
    }

    // This is fail. Out of range of page.
    if ((address + length) >= params->areaSize)
    {
        return 0;
    }

    globalIntDisable();
    
#ifdef DATA_FLASH_CGC_ACTIVE
    checkClockBeforeFlashOperation(params->fparams->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif

    result = R_FLASH_LP_Write(params->fparams->config->p_ctrl, (uint32_t)progData, (uint32_t)(params->area + address), length); // Program sector
    
#ifdef DATA_FLASH_CGC_ACTIVE
    checkClockAfterFlashOperation(params->fparams->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif

    globalIntEnable();
	
    RCOS_ASSERT(FSP_SUCCESS == result, RASSERT_PRI_LOW);
    if (FSP_SUCCESS != result)
    {
        return 0;
    }

    TICKET_REMOVE(params->flash->sys->tvm);

    return length;
}

/// @brief  Erase function for a section
static DEV_MEM_FUNC_ERASE(sectionErase)
{
    tsSectionParams *params = device->parameters;
    tsDevMemSystem *sys     = device->sys;
    fsp_err_t result;
#ifdef DATA_FLASH_CGC_ACTIVE
    cgc_clock_t temp_p_clock_source;
#endif
    
    if (!sys->initialized)
    {
        return 0;
    }

    // This is fail. Out of range of page.
    if ((address + size) >= params->areaSize)
    {
        return 0;
    }

    globalIntDisable();
    
#ifdef DATA_FLASH_CGC_ACTIVE
    checkClockBeforeFlashOperation(params->fparams->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif

    result = R_FLASH_LP_Erase(params->fparams->config->p_ctrl, (uint32_t)(params->area + address), 1); // Erase sector
    
#ifdef DATA_FLASH_CGC_ACTIVE
    checkClockAfterFlashOperation(params->fparams->cgc, (cgc_clock_t * const)(&temp_p_clock_source));
#endif

    globalIntEnable();
    if (result != FSP_SUCCESS)
    {
        return 0;
    }

    TICKET_REMOVE(params->flash->sys->tvm);

    return size;
}

/// @brief  Ticket get function for a section
static DEV_MEM_FUNC_TICKET_GET(sectionTicketGet)
{
    tsSectionParams *params = device->parameters;

    return TICKET_GET(params->flash->sys->tvm);
}

/// @brief  Ticket valid function for a section
static DEV_MEM_FUNC_TICKET_VALID(sectionTicketValid)
{
    tsSectionParams *params = device->parameters;

    if (params->flash->sys->initialized)
    {
        return TICKET_IS_UP(params->flash->sys->tvm, ticket);
    }
    else
    {
        return FALSE;
    }
}

/* Callback function for FLASH LP HAL. */
void bgo_callback(flash_callback_args_t * p_args)
{
    switch(p_args->event)
    {
        case FLASH_EVENT_NOT_BLANK:
            g_b_flash_event_not_blank = true;
            break;
        case FLASH_EVENT_BLANK:
            g_b_flash_event_blank = true;
            break;
        case FLASH_EVENT_ERASE_COMPLETE:
            g_b_flash_event_erase_complete = true;
            break;
        case FLASH_EVENT_WRITE_COMPLETE:
            g_b_flash_event_write_complete = true;
            break;
        default:
            /* Do nothing */
            break;
    }
}

/** @} */

/** @file       reset_record.h
 *  @brief      Production reset log in the external EEPROM (24C32, 0x0FC0-0x0FFF, 64 B)
 *  @copyright  -
 *  @author     Apiwat Kamsiri
 *  @date       11/11/2026
 */

#ifndef RESET_RECORD_H
#define RESET_RECORD_H

#ifdef ResetRecord

#include "rcos.h"

/* EEPROM layout (24C32, page = 32 B). Address and RAM mirror are in EE_data.h:
 * eep_uc_RstCnt / eep_uc_RstHdr / eep_uc_RstRec[3] = EEPROM_R, 0x0FC0-0x0FFF */
#define RST_ADDR_SIZE           (64u)           // 0x0FC0-0x0FFF, 2 pages
#define RST_AREA_CNT            (0u)            // 0x0FC0-0x0FC7: 8 counters, 1 B each, saturate at 255
#define RST_AREA_VER            (8u)            // 0x0FC8: LOG_VER
#define RST_AREA_CRC            (9u)            // 0x0FC9: CRC-8 of 0x0FC0-0x0FC8
#define RST_AREA_RESERVE        (10u)           // 0x0FCA - 0x0FCF: Reserve
#define RST_AREA_RECORD         (16u)           // 0x0FD0: REC_1, 0x0FE0: REC_2, 0x0FF0: REC_3
#define RST_RECORD_SIZE         (16u)
#define RST_RECORD_NUM          (3u)
#define RST_LOG_VER             (0x01u)

/* Record offsets */
#define RST_RECORD_SEQ          (0u)
#define RST_RECORD_CAUSE        (1u)            // bit 0-6 cause, bit 7 CWSF
#define RST_RECORD_RSTSR0       (2u)
#define RST_RECORD_RSTSR1       (3u)            // 2 B, little endian
#define RST_RECORD_FWVER        (5u)
#define RST_RECORD_DAY          (6u)            // 2 B, little endian
#define RST_RECORD_REPEAT       (8u)            // 1-255, 255 = 255 or more
#define RST_RECORD_DETAIL       (9u)            // 6 B, meaning depends on cause
#define RST_RECORD_CRC          (15u)           // CRC-8 of +0..+14
#define RST_DETAIL_SIZE         (6u)

/* Writes the pending REPEAT after this long without a new reset */
#define RST_STABLE_TICKS        (30000u)        // 30000 x 20 ms = 10 min

/* Cause = counter index + 1. Order matches CNT_xxx at 0x0FC0-0x0FC7. */
typedef enum
{
    RST_CAUSE_NONE = 0,      // cold start (power-on): not logged
    RST_CAUSE_WDT,           // DETAIL: proc, event of the handler that hung
    RST_CAUSE_CORE,          // DETAIL: reason (2 B), data (4 B) of coreSysReset()
    RST_CAUSE_STACK,         // DETAIL: SP (3 B), proc, event
    RST_CAUSE_HARDFAULT,     // DETAIL: stacked PC (3 B), stacked LR (3 B)
    RST_CAUSE_PARITY,        // RSTSR1.RPERF, DETAIL empty
    RST_CAUSE_BUS,           // RSTSR1.BUSMRF / BUSSRF, DETAIL empty
    RST_CAUSE_LVD,           // RSTSR0.LVDnRF (LVD0 reset is disabled in OFS1 today)
    RST_CAUSE_OTHER,         // SW reset, RES pin, debugger
    RST_CAUSE_NUM
} teRstCause;


void RSTBootCapture(void);
void RSTInstallHooks(void);
void RSTCoreReset(uint16_t reason, uint32_t data);
void RSTCommit(const tsDevMem *nvm, uint16_t day);
void RSTTick20ms(uint8_t eeIdle);

/* Main loop checkpoint: which handler is running (no-init RAM) */
extern volatile uint8_t RSTCpProc;
extern volatile uint8_t RSTCpEvent;
#define RST_CHECKPOINT(_proc, _event)           \
    do {                                        \
          RSTCpProc = (uint8_t)(_proc);         \
          RSTCpEvent = (uint8_t)(_event);       \
    } while (0)


#endif //ResetRecord

#endif

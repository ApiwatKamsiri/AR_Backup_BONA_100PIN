/** @file       reset_record.c
 *  @brief      Production reset log in the external EEPROM (see reset_record.h)
 *  @copyright  -
 *  @author     Apiwat Kamsiri
 *  @date       11/11/2026
 */

#include "reset_record.h"

#ifdef ResetRecord

#include <string.h>
#include <stddef.h>
#include "bsp_api.h"
#include "app/refrigerator/rf_configs.h"
#include "app/mw/EE_data.h"

/* The log lives in the EEPROM_R mirror (EE_data.h). The byte offsets below must match it. */
#define RST_ADDR_START          (eep_uc_RstCnt_Address)                 // 0x0FC0
static_assert(sizeof(stRSTRecordCNTMode)  == 8u,  "reset log: counters must be 8 B");
static_assert(sizeof(stRSTRecordLOGMode)  == 8u,  "reset log: header must be 8 B");
static_assert(sizeof(stRSTRecordLASTMode) == RST_RECORD_SIZE, "reset log: record must be 16 B");
static_assert(offsetof(stEEPROM_Ram, eep_uc_RstHdr) - offsetof(stEEPROM_Ram, eep_uc_RstCnt) == RST_AREA_VER,
              "reset log: header must follow the counters");
static_assert(offsetof(stEEPROM_Ram, eep_uc_RstRec) - offsetof(stEEPROM_Ram, eep_uc_RstCnt) == RST_AREA_RECORD,
              "reset log: records must follow the header");
static_assert(offsetof(stRSTRecordLASTMode, DETAIL) == RST_RECORD_DETAIL, "reset log: DETAIL offset");
static_assert(offsetof(stRSTRecordLASTMode, CRC)    == RST_RECORD_CRC,    "reset log: CRC offset");
static_assert(eep_uc_RstRec_03_Address + eep_uc_RstRec_03_Length == RST_ADDR_START + RST_ADDR_SIZE,
              "reset log: EE_data.h addresses must cover 64 B");

#define RST_FAULT_MAGIC  (0x5A3Cu)

/* No-init RAM: survives WDT / SW / NMI resets, lost at power-on */
typedef struct
{
    /* Set by the fault hooks. */
    volatile uint8_t  faultType;                        // RST_CAUSE_CORE / STACK / HARDFAULT
    volatile uint8_t  faultDetail[RST_DETAIL_SIZE];
    volatile uint16_t faultMagic;                       // RST_FAULT_MAGIC ^ faultType

    /* Open series for the repeat guard. Normal context only, protected by seriesCrc. */
    uint8_t open;                                       // 1 = the record in 'slot' still collects repeats
    uint8_t slot;                                       // always 0 (REC_1 = newest)
    uint8_t key[1u + RST_DETAIL_SIZE];                  // cause + DETAIL
    uint8_t count;                                      // occurrences so far, 1..255
    uint8_t written;                                    // REPEAT value already in EEPROM
    uint8_t stored;                                     // 1 = the record was handed to the EEPROM driver
    uint8_t seriesCrc;
} tsRstBox;

__no_init static tsRstBox RSTBox;
__no_init volatile uint8_t RSTCpProc;
__no_init volatile uint8_t RSTCpEvent;

typedef enum
{
    RST_ACT_NONE = 0,                                   // power-on: nothing to do
    RST_ACT_HOLD,                                       // repeat counted in RAM only
    RST_ACT_NEW,                                        // new record (+ counter)
    RST_ACT_FLUSH,                                      // repeat reached 255: write REPEAT once
} teRstAct;

typedef struct
{
    uint8_t slot;
    uint8_t key[1u + RST_DETAIL_SIZE];
    uint8_t count;
    uint8_t written;
} tsRstSeries;

static teRstAct    RSTAct;        
static uint8_t     RSTFlushPrev;                        // 1 = write the old series' REPEAT before the new record
static tsRstSeries RSTPrev;                             // the old series
static uint8_t     RSTNew[RST_RECORD_SIZE];             // record built at boot
/* RAM image of 0x0FC0-0x0FFF = EEPROM_R.eep_uc_RstCnt + eep_uc_RstHdr + eep_uc_RstRec[3] (64 B in a row) */
static uint8_t *const RSTImg = (uint8_t *)&EEPROM_R.eep_uc_RstCnt;
static uint8_t     RSTTx[RST_ADDR_SIZE];                // write buffer: the 24cxx thread keeps this pointer until done
static uint8_t     RSTTxPending;
static uint8_t     RSTImgValid;
static uint16_t    RSTTicks;
static const tsDevMem *RSTNvm;

/* Helpers */
static uint8_t RSTCrc8(const uint8_t *p, uint8_t n)
{
    uint8_t crc = 0xFFu;
    while (n--)
    {
        crc ^= *p++;
        for (uint8_t b = 0u; b < 8u; b++)
        {
            crc = (crc & 0x80u) ? (uint8_t)((crc << 1) ^ 0x31u) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

static uint8_t RSTSeriesCrc(void)
{
    return RSTCrc8(&RSTBox.open, (uint8_t)(offsetof(tsRstBox, seriesCrc) - offsetof(tsRstBox, open)));
}

static void RSTSeriesSave(void)
{
    RSTBox.seriesCrc = RSTSeriesCrc();
}

static uint8_t RSTSeriesValid(void)
{
    return (RSTBox.seriesCrc == RSTSeriesCrc()) && (RSTBox.open == 1u) && (RSTBox.slot < RST_RECORD_NUM);
}

static void RSTPut24(volatile uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
}

static uint8_t *RSTRec(uint8_t slot)
{
    return &RSTImg[RST_AREA_RECORD + (uint16_t)slot * RST_RECORD_SIZE];
}

static uint8_t RSTRecValid(const uint8_t *r)
{
    uint8_t cause = r[RST_RECORD_CAUSE] & 0x7Fu;
    return (r[RST_RECORD_CRC] == RSTCrc8(r, RST_RECORD_SIZE - 1u)) && (cause > RST_CAUSE_NONE) && (cause < RST_CAUSE_NUM);
}

static void RSTCntAdd(uint8_t cause, uint8_t n)
{
    uint8_t *c = &RSTImg[RST_AREA_CNT + cause - 1u];
    *c = ((uint16_t)*c + n > 0xFFu) ? 0xFFu : (uint8_t)(*c + n);
}

static void RSTSchedule(void)
{
    RSTImg[RST_AREA_CRC] = RSTCrc8(&RSTImg[RST_AREA_CNT], RST_AREA_CRC);
    memcpy(RSTTx, RSTImg, RST_ADDR_SIZE);     // writes are >= 10 min apart, the previous one is long finished
    RSTTxPending = 1u;
}

/* Write REPEAT of a series into its record and add the new repeats to the counter */
static void RSTApplyRepeat(const tsRstSeries *s)
{
    uint8_t *r = RSTRec(s->slot);

    if (!RSTRecValid(r) || ((r[RST_RECORD_CAUSE] & 0x7Fu) != s->key[0]) ||
        memcmp(&r[RST_RECORD_DETAIL], &s->key[1], RST_DETAIL_SIZE))
        return;                           // record was replaced or the area was re-initialised

    r[RST_RECORD_REPEAT] = s->count;
    r[RST_RECORD_CRC]    = RSTCrc8(r, RST_RECORD_SIZE - 1u);
    RSTCntAdd(s->key[0], (uint8_t)(s->count - s->written));
}

static void RSTBoxToSeries(tsRstSeries *s)
{
    s->slot    = RSTBox.slot;
    memcpy(s->key, RSTBox.key, sizeof(s->key));
    s->count   = RSTBox.count;
    s->written = RSTBox.written;
}

/* Fault hooks (record, then wait for the WDT reset) */
void RSTCoreReset(uint16_t reason, uint32_t data)
{
    if (RSTBox.faultType != RST_CAUSE_NONE)
        return;                           // keep the first fault
    RSTBox.faultDetail[0] = (uint8_t)reason;
    RSTBox.faultDetail[1] = (uint8_t)(reason >> 8);
    RSTBox.faultDetail[2] = (uint8_t)data;
    RSTBox.faultDetail[3] = (uint8_t)(data >> 8);
    RSTBox.faultDetail[4] = (uint8_t)(data >> 16);
    RSTBox.faultDetail[5] = (uint8_t)(data >> 24);
    RSTBox.faultType  = RST_CAUSE_CORE;
    RSTBox.faultMagic = (uint16_t)(RST_FAULT_MAGIC ^ RST_CAUSE_CORE);
}

__stackless void HardFault_Handler(void)
{
    const uint32_t *frame = (const uint32_t *)__get_MSP();

    if (RSTBox.faultType == RST_CAUSE_NONE)
    {
        RSTPut24(&RSTBox.faultDetail[0], frame[6]);   // stacked PC (flash is 256 KB, 3 B is enough)
        RSTPut24(&RSTBox.faultDetail[3], frame[5]);   // stacked LR
        RSTBox.faultType  = RST_CAUSE_HARDFAULT;
        RSTBox.faultMagic = (uint16_t)(RST_FAULT_MAGIC ^ RST_CAUSE_HARDFAULT);
    }
    for (;;)
    {
    }                                                // WDT resets the MCU
}

static void RSTStackNmi(bsp_grp_irq_t irq)
{
    (void)irq;
    if (RSTBox.faultType == RST_CAUSE_NONE)
    {
        RSTPut24(&RSTBox.faultDetail[0], __get_MSP());
        RSTBox.faultDetail[3] = RSTCpProc;
        RSTBox.faultDetail[4] = RSTCpEvent;
        RSTBox.faultDetail[5] = 0u;
        RSTBox.faultType  = RST_CAUSE_STACK;
        RSTBox.faultMagic = (uint16_t)(RST_FAULT_MAGIC ^ RST_CAUSE_STACK);
    }
    for (;;)
    {
    }                                                // WDT resets the MCU
}

void RSTInstallHooks(void)
{
    (void)R_BSP_GroupIrqWrite(BSP_GRP_IRQ_MPU_STACK, RSTStackNmi);
}

/* Boot */
void RSTBootCapture(void)
{
    uint8_t  rs0   = R_SYSTEM->RSTSR0;                                          //Power-on Reset, Voltage Monitor Reset
    uint16_t rs1   = R_SYSTEM->RSTSR1;                                          //Watchdog, Software Reset, RAM Parity Error
    uint8_t  cwsf  = (uint8_t)(R_SYSTEM->RSTSR2 & R_SYSTEM_RSTSR2_CWSF_Msk);    //First power on or Reset
    uint8_t  cold  = (uint8_t)((rs0 & R_SYSTEM_RSTSR0_PORF_Msk) || !cwsf);      //Check warm : 1 or cold start : 0
    
    /* After power-on the no-init RAM is random. After a parity reset it must not be read. */
    uint8_t  ramOk = (uint8_t)(!cold && !(rs1 & R_SYSTEM_RSTSR1_RPERF_Msk));    //Check ram 
    uint8_t  cause = RST_CAUSE_NONE;
    uint8_t  key[1u + RST_DETAIL_SIZE] = { 0u };      // cause + DETAIL
    uint8_t *detail = &key[1];

    RSTAct       = RST_ACT_NONE;
    RSTFlushPrev = 0u;

    if (!cold)
    {
        uint8_t  fault = RST_CAUSE_NONE;

        if (ramOk)                                      // never read no-init RAM after a parity reset
        {
            uint8_t  type  = RSTBox.faultType;          // read each volatile once
            uint16_t magic = RSTBox.faultMagic;

            if (((type == RST_CAUSE_CORE) || (type == RST_CAUSE_STACK) || (type == RST_CAUSE_HARDFAULT)) &&
                (magic == (uint16_t)(RST_FAULT_MAGIC ^ type)))
            {
                fault = type;
                for (uint8_t i = 0u; i < RST_DETAIL_SIZE; i++)
                    detail[i] = RSTBox.faultDetail[i];
            }
        }

        /* Every fault ends in a WDT reset, so the fault type comes first */
        if (fault != RST_CAUSE_NONE)
            cause = fault;
        else if (rs1 & R_SYSTEM_RSTSR1_RPERF_Msk)
            cause = RST_CAUSE_PARITY;
        else if (rs1 & (R_SYSTEM_RSTSR1_BUSMRF_Msk | R_SYSTEM_RSTSR1_BUSSRF_Msk))
            cause = RST_CAUSE_BUS;
        else if (rs1 & R_SYSTEM_RSTSR1_SPERF_Msk)
            cause = RST_CAUSE_STACK;
        else if (rs1 & (R_SYSTEM_RSTSR1_IWDTRF_Msk | R_SYSTEM_RSTSR1_WDTRF_Msk))
        {
            cause = RST_CAUSE_WDT;
            if (ramOk)
            {
                detail[0] = RSTCpProc;                // 0 / 0 = hang outside a handler (scheduler, ISR)
                detail[1] = RSTCpEvent;
            }
        }
        else if (rs1 & R_SYSTEM_RSTSR1_SWRF_Msk)
            cause = RST_CAUSE_OTHER;
        else if (rs0 & (R_SYSTEM_RSTSR0_LVD0RF_Msk | R_SYSTEM_RSTSR0_LVD1RF_Msk | R_SYSTEM_RSTSR0_LVD2RF_Msk))
            cause = RST_CAUSE_LVD;
        else
            cause = RST_CAUSE_OTHER;                  // RES pin, debugger
        key[0] = cause;
    }

    if (!ramOk)
        memset(&RSTBox.open, 0, offsetof(tsRstBox, seriesCrc) - offsetof(tsRstBox, open));

    if (cause != RST_CAUSE_NONE)
    {
        memset(RSTNew, 0, sizeof(RSTNew));
        RSTNew[RST_RECORD_CAUSE]     = (uint8_t)(cause | (cwsf ? 0x80u : 0u));
        RSTNew[RST_RECORD_RSTSR0]    = rs0;
        RSTNew[RST_RECORD_RSTSR1]    = (uint8_t)rs1;
        RSTNew[RST_RECORD_RSTSR1+1u] = (uint8_t)(rs1 >> 8);
        RSTNew[RST_RECORD_FWVER]     = refrigerator_F.Stock.Revision;
        RSTNew[RST_RECORD_REPEAT]    = 1u;
        memcpy(&RSTNew[RST_RECORD_DETAIL], detail, RST_DETAIL_SIZE);

        if (ramOk && RSTSeriesValid() && RSTBox.stored && !memcmp(key, RSTBox.key, sizeof(key)))
        {
            /* Same cause + DETAIL again before 10 min stable: count in RAM */
            if (RSTBox.count < 0xFFu)
            {
                RSTBox.count++;
                RSTAct = (RSTBox.count == 0xFFu) ? RST_ACT_FLUSH : RST_ACT_HOLD;
            }
            else
            {
                RSTAct = RST_ACT_HOLD;                 // already 255 in EEPROM: stop writing
            }
        }
        else
        {
            if (ramOk && RSTSeriesValid() && RSTBox.stored && (RSTBox.count > RSTBox.written))
            {
                RSTBoxToSeries(&RSTPrev);              // different cause: write the old REPEAT first
                RSTFlushPrev = 1u;
            }
            RSTBox.open = 0u;                         // the new series opens in RSTCommit()
            memcpy(RSTBox.key, key, sizeof(key));
            RSTAct = RST_ACT_NEW;
        }
    }
    else
    {
        RSTBox.open = 0u;
    }
    RSTSeriesSave();

    /* Ready for the next fault */
    RSTBox.faultMagic = 0u;
    RSTBox.faultType  = RST_CAUSE_NONE;
    RSTCpProc  = 0u;
    RSTCpEvent = 0u;

    /* Clear the flags so the next boot sees only its own cause, and set CWSF so the next
     * reset without power-on reads as warm. */
    R_SYSTEM->RSTSR1 = 0u;
    R_SYSTEM->RSTSR0 = 0u;
    R_SYSTEM->RSTSR2 = R_SYSTEM_RSTSR2_CWSF_Msk;
}

/* After backupInit (I2C ready) */
void RSTCommit(const tsDevMem *nvm, uint16_t day)
{
    RSTNvm = nvm;

    /* Blocking read, same style as ReadFieldInit() */
    while (RST_ADDR_SIZE != devMemRead(nvm, RST_ADDR_START, RSTImg, RST_ADDR_SIZE));

    if ((RSTImg[RST_AREA_VER] != RST_LOG_VER) ||
        (RSTImg[RST_AREA_CRC] != RSTCrc8(&RSTImg[RST_AREA_CNT], RST_AREA_CRC)))
    {
        memset(RSTImg, 0, RST_ADDR_SIZE);               // new unit (0xFF) or damaged header
        RSTImg[RST_AREA_VER] = RST_LOG_VER;
    }
    RSTImgValid = 1u;

    if ((RSTAct == RST_ACT_NONE) || (RSTAct == RST_ACT_HOLD))
        return;                                         // no EEPROM write

    if (RSTFlushPrev)
        RSTApplyRepeat(&RSTPrev);                       // Different cause: write the old REPEAT first

    if (RSTAct == RST_ACT_FLUSH)                        // When the count more than 255, write and clear
    {
        tsRstSeries s;                                  
        RSTBoxToSeries(&s);
        RSTApplyRepeat(&s);
        RSTBox.written = RSTBox.count;
    }
    else                                                // New cause
    {
        uint8_t *r     = RSTRec(0u);                    // REC_1 is always the newest

        RSTNew[RST_RECORD_SEQ]     = RSTRecValid(r) ? (uint8_t)(r[RST_RECORD_SEQ] + 1u) : 1u;
        RSTNew[RST_RECORD_DAY]     = (uint8_t)day;
        RSTNew[RST_RECORD_DAY+1u]  = (uint8_t)(day >> 8);
        RSTNew[RST_RECORD_CRC]     = RSTCrc8(RSTNew, RST_RECORD_SIZE - 1u);

        /* Shift down: REC_2 -> REC_3, REC_1 -> REC_2 (the old REC_3 is dropped) */
        memmove(RSTRec(1u), RSTRec(0u), (RST_RECORD_NUM - 1u) * RST_RECORD_SIZE);
        memcpy(r, RSTNew, RST_RECORD_SIZE);
        RSTCntAdd(RSTBox.key[0], 1u);
        
        RSTBox.open    = 1u;                          // key was set in RSTBootCapture()
        RSTBox.slot    = 0u;                          // the open series is always REC_1
        RSTBox.count   = 1u;
        RSTBox.written = 1u;
        RSTBox.stored  = 0u;                          // set when the driver accepts the write
    }
    RSTSeriesSave();
    RSTSchedule();
}

/* Runtime: h_backup eBackupPeriodic20Ms */
void RSTTick20ms(uint8_t eeIdle)
{
    if (RSTTxPending && RSTNvm && eeIdle)
    {
        /* One call, 64 B: the 24cxx driver splits it into the 2 pages. 0 = driver busy, retry */
        if (RST_ADDR_SIZE == devMemWrite(RSTNvm, RST_ADDR_START, RSTTx, RST_ADDR_SIZE))
        {
            RSTTxPending = 0u;
            /* A reset before this point leaves the series not stored: the next boot makes a new record */
            if (RSTSeriesValid() && !RSTBox.stored)
            {
                RSTBox.stored = 1u;
                RSTSeriesSave();
            }
        }
    }

    if (RSTTicks < RST_STABLE_TICKS)
    {
        if (++RSTTicks == RST_STABLE_TICKS)
        {
            /* 10 min without a reset: close the series, write the pending REPEAT once */
            if (RSTSeriesValid())
            {
                if ((RSTBox.count > RSTBox.written) && RSTImgValid)
                {
                    tsRstSeries s;
                    RSTBoxToSeries(&s);
                    RSTApplyRepeat(&s);
                    RSTSchedule();
                }
                RSTBox.open = 0u;
                RSTSeriesSave();
            }
        }
    }
}

#endif  //ResetRecord

/* generated HAL source file - do not edit */
#include "hal_data.h"

/* Macros to tie dynamic ELC links to ADC_TRIGGER_SYNC_ELC option in adc_trigger_t. */
#define ADC_TRIGGER_ADC0        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC0_B      ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1        ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1_B      ADC_TRIGGER_SYNC_ELC
wdt_instance_ctrl_t g_wdt0_ctrl;

const wdt_cfg_t g_wdt0_cfg =
{
    .timeout = WDT_TIMEOUT_16384,
    .clock_division = WDT_CLOCK_DIVISION_8192,
    .window_start = WDT_WINDOW_START_100,
    .window_end = WDT_WINDOW_END_0,
    .reset_control = WDT_RESET_CONTROL_RESET,
    .stop_control = WDT_STOP_CONTROL_ENABLE,
    .p_callback = NULL,
};

/* Instance structure to use this module. */
const wdt_instance_t g_wdt0 =
{
    .p_ctrl        = &g_wdt0_ctrl,
    .p_cfg         = &g_wdt0_cfg,
    .p_api         = &g_wdt_on_wdt
};
gpt_instance_ctrl_t FFAN_B_ctrl;
#if 0
const gpt_extended_pwm_cfg_t FFAN_B_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT6_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT6_COUNTER_UNDERFLOW,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      = (gpt_output_disable_t) ( GPT_OUTPUT_DISABLE_NONE),
    .adc_trigger         = (gpt_adc_trigger_t) ( GPT_ADC_TRIGGER_NONE),
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
};
#endif
const gpt_extended_cfg_t FFAN_B_extend =
{
    .gtioca = { .output_enabled = false,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_A,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT6_CAPTURE_COMPARE_B,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &FFAN_B_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) false,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif
};

const timer_cfg_t FFAN_B_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 0.0016666666666666668 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x4e20, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)2,
    .channel             = 6,
    .p_callback          = NULL,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = &NULL,
#endif
    .p_extend            = &FFAN_B_extend,
    .cycle_end_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT6_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT6_COUNTER_OVERFLOW,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t FFAN_B =
{
    .p_ctrl        = &FFAN_B_ctrl,
    .p_cfg         = &FFAN_B_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t RFAN_B_ctrl;
#if 0
const gpt_extended_pwm_cfg_t RFAN_B_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT8_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT8_COUNTER_UNDERFLOW,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      = (gpt_output_disable_t) ( GPT_OUTPUT_DISABLE_NONE),
    .adc_trigger         = (gpt_adc_trigger_t) ( GPT_ADC_TRIGGER_NONE),
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
};
#endif
const gpt_extended_cfg_t RFAN_B_extend =
{
    .gtioca = { .output_enabled = false,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT8_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT8_CAPTURE_COMPARE_A,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT8_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT8_CAPTURE_COMPARE_B,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &RFAN_B_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) false,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif
};

const timer_cfg_t RFAN_B_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 0.0016666666666666668 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x4e20, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)2,
    .channel             = 8,
    .p_callback          = NULL,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = &NULL,
#endif
    .p_extend            = &RFAN_B_extend,
    .cycle_end_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT8_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT8_COUNTER_OVERFLOW,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t RFAN_B =
{
    .p_ctrl        = &RFAN_B_ctrl,
    .p_cfg         = &RFAN_B_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t R_LED_A_ctrl;
#if 0
const gpt_extended_pwm_cfg_t R_LED_A_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT0_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT0_COUNTER_UNDERFLOW,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      = (gpt_output_disable_t) ( GPT_OUTPUT_DISABLE_NONE),
    .adc_trigger         = (gpt_adc_trigger_t) ( GPT_ADC_TRIGGER_NONE),
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
};
#endif
const gpt_extended_cfg_t R_LED_A_extend =
{
    .gtioca = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = false,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT0_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT0_CAPTURE_COMPARE_A,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT0_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT0_CAPTURE_COMPARE_B,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &R_LED_A_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) false,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif
};

const timer_cfg_t R_LED_A_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 0.0016666666666666668 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x13880, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = 0,
    .p_callback          = NULL,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = &NULL,
#endif
    .p_extend            = &R_LED_A_extend,
    .cycle_end_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT0_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT0_COUNTER_OVERFLOW,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t R_LED_A =
{
    .p_ctrl        = &R_LED_A_ctrl,
    .p_cfg         = &R_LED_A_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t B_LED_A_G_LED_B_ctrl;
#if 0
const gpt_extended_pwm_cfg_t B_LED_A_G_LED_B_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT5_COUNTER_UNDERFLOW,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      = (gpt_output_disable_t) ( GPT_OUTPUT_DISABLE_NONE),
    .adc_trigger         = (gpt_adc_trigger_t) ( GPT_ADC_TRIGGER_NONE),
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
};
#endif
const gpt_extended_cfg_t B_LED_A_G_LED_B_extend =
{
    .gtioca = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_A,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT5_CAPTURE_COMPARE_B,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &B_LED_A_G_LED_B_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif
};

const timer_cfg_t B_LED_A_G_LED_B_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 0.0016666666666666668 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x4e20, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)2,
    .channel             = 5,
    .p_callback          = NULL,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = &NULL,
#endif
    .p_extend            = &B_LED_A_G_LED_B_extend,
    .cycle_end_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT5_COUNTER_OVERFLOW,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t B_LED_A_G_LED_B =
{
    .p_ctrl        = &B_LED_A_G_LED_B_ctrl,
    .p_cfg         = &B_LED_A_G_LED_B_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t BTLED_A_XLED_B_ctrl;
#if 0
const gpt_extended_pwm_cfg_t BTLED_A_XLED_B_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT3_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT3_COUNTER_UNDERFLOW,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      = (gpt_output_disable_t) ( GPT_OUTPUT_DISABLE_NONE),
    .adc_trigger         = (gpt_adc_trigger_t) ( GPT_ADC_TRIGGER_NONE),
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
};
#endif
const gpt_extended_cfg_t BTLED_A_XLED_B_extend =
{
    .gtioca = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT3_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT3_CAPTURE_COMPARE_A,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT3_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT3_CAPTURE_COMPARE_B,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &BTLED_A_XLED_B_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif
};

const timer_cfg_t BTLED_A_XLED_B_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 0.0016666666666666668 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x13880, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = 3,
    .p_callback          = NULL,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = &NULL,
#endif
    .p_extend            = &BTLED_A_XLED_B_extend,
    .cycle_end_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT3_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT3_COUNTER_OVERFLOW,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t BTLED_A_XLED_B =
{
    .p_ctrl        = &BTLED_A_XLED_B_ctrl,
    .p_cfg         = &BTLED_A_XLED_B_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t LED_DRIVE_ctrl;
#if 0
const gpt_extended_pwm_cfg_t LED_DRIVE_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT2_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT2_COUNTER_UNDERFLOW,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      = (gpt_output_disable_t) ( GPT_OUTPUT_DISABLE_NONE),
    .adc_trigger         = (gpt_adc_trigger_t) ( GPT_ADC_TRIGGER_NONE),
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
};
#endif
const gpt_extended_cfg_t LED_DRIVE_extend =
{
    .gtioca = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT2_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT2_CAPTURE_COMPARE_A,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT2_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT2_CAPTURE_COMPARE_B,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &LED_DRIVE_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif
};

const timer_cfg_t LED_DRIVE_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 0.0016666666666666668 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x13880, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = 2,
    .p_callback          = NULL,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = &NULL,
#endif
    .p_extend            = &LED_DRIVE_extend,
    .cycle_end_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT2_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT2_COUNTER_OVERFLOW,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t LED_DRIVE =
{
    .p_ctrl        = &LED_DRIVE_ctrl,
    .p_cfg         = &LED_DRIVE_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t BUZ_BUZZER_ctrl;
#if 0
const gpt_extended_pwm_cfg_t BUZ_BUZZER_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT7_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT7_COUNTER_UNDERFLOW,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      = (gpt_output_disable_t) ( GPT_OUTPUT_DISABLE_NONE),
    .adc_trigger         = (gpt_adc_trigger_t) ( GPT_ADC_TRIGGER_NONE),
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
};
#endif
const gpt_extended_cfg_t BUZ_BUZZER_extend =
{
    .gtioca = { .output_enabled = false,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT7_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT7_CAPTURE_COMPARE_A,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT7_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT7_CAPTURE_COMPARE_B,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &BUZ_BUZZER_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 0
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) false,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) true,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif
};

const timer_cfg_t BUZ_BUZZER_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 0.001 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0xbb80, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = 7,
    .p_callback          = NULL,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = &NULL,
#endif
    .p_extend            = &BUZ_BUZZER_extend,
    .cycle_end_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT7_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT7_COUNTER_OVERFLOW,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t BUZ_BUZZER =
{
    .p_ctrl        = &BUZ_BUZZER_ctrl,
    .p_cfg         = &BUZ_BUZZER_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t COMP_OUT_T_ctrl;
#if 0
const gpt_extended_pwm_cfg_t COMP_OUT_T_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT1_COUNTER_UNDERFLOW)
    .trough_irq          = VECTOR_NUMBER_GPT1_COUNTER_UNDERFLOW,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      = (gpt_output_disable_t) ( GPT_OUTPUT_DISABLE_NONE),
    .adc_trigger         = (gpt_adc_trigger_t) ( GPT_ADC_TRIGGER_NONE),
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
};
#endif
const gpt_extended_cfg_t COMP_OUT_T_extend =
{
    .gtioca = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = false,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT1_CAPTURE_COMPARE_A)
    .capture_a_irq       = VECTOR_NUMBER_GPT1_CAPTURE_COMPARE_A,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT1_CAPTURE_COMPARE_B)
    .capture_b_irq       = VECTOR_NUMBER_GPT1_CAPTURE_COMPARE_B,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &COMP_OUT_T_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if 1
    .gtior_setting.gtior_b.gtioa  = (0U << 4U) | (1U << 2U) | (3U << 0U),
    .gtior_setting.gtior_b.oadflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.oahld  = 0U,
    .gtior_setting.gtior_b.oae    = (uint32_t) true,
    .gtior_setting.gtior_b.oadf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfaen  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsa  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
    .gtior_setting.gtior_b.gtiob  = (0U << 4U) | (0U << 2U) | (0U << 0U),
    .gtior_setting.gtior_b.obdflt = (uint32_t) GPT_PIN_LEVEL_LOW,
    .gtior_setting.gtior_b.obhld  = 0U,
    .gtior_setting.gtior_b.obe    = (uint32_t) false,
    .gtior_setting.gtior_b.obdf   = (uint32_t) GPT_GTIOC_DISABLE_PROHIBITED,
    .gtior_setting.gtior_b.nfben  = ((uint32_t) GPT_CAPTURE_FILTER_NONE & 1U),
    .gtior_setting.gtior_b.nfcsb  = ((uint32_t) GPT_CAPTURE_FILTER_NONE >> 1U),
#else
    .gtior_setting.gtior = 0U,
#endif
};

const timer_cfg_t COMP_OUT_T_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 0.03333333333333333 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x186a00, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = 1,
    .p_callback          = NULL,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = &NULL,
#endif
    .p_extend            = &COMP_OUT_T_extend,
    .cycle_end_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT1_COUNTER_OVERFLOW)
    .cycle_end_irq       = VECTOR_NUMBER_GPT1_COUNTER_OVERFLOW,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t COMP_OUT_T =
{
    .p_ctrl        = &COMP_OUT_T_ctrl,
    .p_cfg         = &COMP_OUT_T_cfg,
    .p_api         = &g_timer_on_gpt
};
flash_lp_instance_ctrl_t EEPROM_ctrl;
const flash_cfg_t EEPROM_cfg =
{
    .data_flash_bgo      = true,
    .p_callback          = bgo_callback,
    .p_context           = NULL,
    .ipl                 = (2),
#if defined(VECTOR_NUMBER_FCU_FRDYI)
    .irq                 = VECTOR_NUMBER_FCU_FRDYI,
#else
    .irq                 = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const flash_instance_t EEPROM =
{
    .p_ctrl        = &EEPROM_ctrl,
    .p_cfg         = &EEPROM_cfg,
    .p_api         = &g_flash_on_flash_lp
};
#define RA_NOT_DEFINED (UINT32_MAX)
#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_tx_dmac_callback(spi_instance_ctrl_t const * const p_ctrl);

void DOOR_SPI_BUS_tx_transfer_callback (dmac_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    spi_tx_dmac_callback(&DOOR_SPI_BUS_ctrl);
}
#endif

#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
#include "r_dmac.h"
extern void spi_rx_dmac_callback(spi_instance_ctrl_t const * const p_ctrl);

void DOOR_SPI_BUS_rx_transfer_callback (dmac_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    spi_rx_dmac_callback(&DOOR_SPI_BUS_ctrl);
}
#endif
#undef RA_NOT_DEFINED

spi_instance_ctrl_t DOOR_SPI_BUS_ctrl;

/** SPI extended configuration for SPI HAL driver */
const spi_extended_cfg_t DOOR_SPI_BUS_ext_cfg =
{
    .spi_clksyn         = SPI_SSL_MODE_SPI,
    .spi_comm           = SPI_COMMUNICATION_FULL_DUPLEX,
    .ssl_polarity        = SPI_SSLP_LOW,
    .ssl_select          = SPI_SSL_SELECT_SSL0,
    .mosi_idle           = SPI_MOSI_IDLE_VALUE_FIXING_DISABLE,
    .parity              = SPI_PARITY_MODE_DISABLE,
    .byte_swap           = SPI_BYTE_SWAP_DISABLE,
    .spck_div            = {
        /* Actual calculated bitrate: 40000. */ .spbr = 149, .brdv = 0
    },
    .spck_delay          = SPI_DELAY_COUNT_1,
    .ssl_negation_delay  = SPI_DELAY_COUNT_1,
    .next_access_delay   = SPI_DELAY_COUNT_1
 };

/** SPI configuration for SPI HAL driver */
const spi_cfg_t DOOR_SPI_BUS_cfg =
{
    .channel             = 0,

#if defined(VECTOR_NUMBER_SPI0_RXI)
    .rxi_irq             = VECTOR_NUMBER_SPI0_RXI,
#else
    .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SPI0_TXI)
    .txi_irq             = VECTOR_NUMBER_SPI0_TXI,
#else
    .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SPI0_TEI)
    .tei_irq             = VECTOR_NUMBER_SPI0_TEI,
#else
    .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SPI0_ERI)
    .eri_irq             = VECTOR_NUMBER_SPI0_ERI,
#else
    .eri_irq             = FSP_INVALID_VECTOR,
#endif

    .rxi_ipl             = (0),
    .txi_ipl             = (2),
    .tei_ipl             = (2),
    .eri_ipl             = (1),

    .operating_mode      = SPI_MODE_SLAVE,

    .clk_phase           = SPI_CLK_PHASE_EDGE_ODD,
    .clk_polarity        = SPI_CLK_POLARITY_LOW,

    .mode_fault          = SPI_MODE_FAULT_ERROR_ENABLE,
    .bit_order           = SPI_BIT_ORDER_LSB_FIRST,
    .p_transfer_tx       = DOOR_SPI_BUS_P_TRANSFER_TX,
    .p_transfer_rx       = DOOR_SPI_BUS_P_TRANSFER_RX,
    .p_callback          = spi_callback,

    .p_context           = NULL,
    .p_extend            = (void *)&DOOR_SPI_BUS_ext_cfg,
};

/* Instance structure to use this module. */
const spi_instance_t DOOR_SPI_BUS =
{
    .p_ctrl        = &DOOR_SPI_BUS_ctrl,
    .p_cfg         = &DOOR_SPI_BUS_cfg,
    .p_api         = &g_spi_on_spi
};
sci_uart_instance_ctrl_t     WIFI_UART_BUS_ctrl;

            baud_setting_t               WIFI_UART_BUS_baud_setting =
            {
                /* Baud rate calculated with 0.160% error. */ .semr_baudrate_bits_b.abcse = 1, .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 0, .cks = 0, .brr = 51, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme = false
            };

            /** UART extended configuration for UARTonSCI HAL driver */
            const sci_uart_extended_cfg_t WIFI_UART_BUS_cfg_extend =
            {
                .clock                = SCI_UART_CLOCK_INT,
                .rx_edge_start          = SCI_UART_START_BIT_FALLING_EDGE,
                .noise_cancel         = SCI_UART_NOISE_CANCELLATION_DISABLE,
                .rx_fifo_trigger        = SCI_UART_RX_FIFO_TRIGGER_MAX,
                .p_baud_setting         = &WIFI_UART_BUS_baud_setting,
                .flow_control           = SCI_UART_FLOW_CONTROL_RTS,
                #if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                .flow_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                .rs485_setting = {
                    .enable = SCI_UART_RS485_DISABLE,
                    .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
                #if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                    .de_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                },
            };

            /** UART interface configuration */
            const uart_cfg_t WIFI_UART_BUS_cfg =
            {
                .channel             = 0,
                .data_bits           = UART_DATA_BITS_8,
                .parity              = UART_PARITY_OFF,
                .stop_bits           = UART_STOP_BITS_1,
                .p_callback          = wifi_callback,
                .p_context           = NULL,
                .p_extend            = &WIFI_UART_BUS_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_tx       = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_rx       = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
                .rxi_ipl             = (2),
                .txi_ipl             = (2),
                .tei_ipl             = (2),
                .eri_ipl             = (2),
#if defined(VECTOR_NUMBER_SCI0_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI0_RXI,
#else
                .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI0_TXI,
#else
                .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI0_TEI,
#else
                .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI0_ERI,
#else
                .eri_irq             = FSP_INVALID_VECTOR,
#endif
            };

/* Instance structure to use this module. */
const uart_instance_t WIFI_UART_BUS =
{
    .p_ctrl        = &WIFI_UART_BUS_ctrl,
    .p_cfg         = &WIFI_UART_BUS_cfg,
    .p_api         = &g_uart_on_sci
};
sci_uart_instance_ctrl_t     UDAQ_UART_BUS_ctrl;

            baud_setting_t               UDAQ_UART_BUS_baud_setting =
            {
                /* Baud rate calculated with 0.160% error. */ .semr_baudrate_bits_b.abcse = 0, .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1, .cks = 0, .brr = 77, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme = false
            };

            /** UART extended configuration for UARTonSCI HAL driver */
            const sci_uart_extended_cfg_t UDAQ_UART_BUS_cfg_extend =
            {
                .clock                = SCI_UART_CLOCK_INT,
                .rx_edge_start          = SCI_UART_START_BIT_FALLING_EDGE,
                .noise_cancel         = SCI_UART_NOISE_CANCELLATION_DISABLE,
                .rx_fifo_trigger        = SCI_UART_RX_FIFO_TRIGGER_MAX,
                .p_baud_setting         = &UDAQ_UART_BUS_baud_setting,
                .flow_control           = SCI_UART_FLOW_CONTROL_RTS,
                #if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                .flow_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                .rs485_setting = {
                    .enable = SCI_UART_RS485_DISABLE,
                    .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
                #if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                    .de_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                },
            };

            /** UART interface configuration */
            const uart_cfg_t UDAQ_UART_BUS_cfg =
            {
                .channel             = 9,
                .data_bits           = UART_DATA_BITS_8,
                .parity              = UART_PARITY_OFF,
                .stop_bits           = UART_STOP_BITS_1,
                .p_callback          = udaq_callback,
                .p_context           = NULL,
                .p_extend            = &UDAQ_UART_BUS_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_tx       = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_rx       = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
                .rxi_ipl             = (2),
                .txi_ipl             = (2),
                .tei_ipl             = (2),
                .eri_ipl             = (2),
#if defined(VECTOR_NUMBER_SCI9_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI9_RXI,
#else
                .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI9_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI9_TXI,
#else
                .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI9_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI9_TEI,
#else
                .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI9_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI9_ERI,
#else
                .eri_irq             = FSP_INVALID_VECTOR,
#endif
            };

/* Instance structure to use this module. */
const uart_instance_t UDAQ_UART_BUS =
{
    .p_ctrl        = &UDAQ_UART_BUS_ctrl,
    .p_cfg         = &UDAQ_UART_BUS_cfg,
    .p_api         = &g_uart_on_sci
};
sci_uart_instance_ctrl_t     COMP_UART_BUS_ctrl;

            baud_setting_t               COMP_UART_BUS_baud_setting =
            {
                /* Baud rate calculated with 0.160% error. */ .semr_baudrate_bits_b.abcse = 0, .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 0, .cks = 1, .brr = 155, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme = false
            };

            /** UART extended configuration for UARTonSCI HAL driver */
            const sci_uart_extended_cfg_t COMP_UART_BUS_cfg_extend =
            {
                .clock                = SCI_UART_CLOCK_INT,
                .rx_edge_start          = SCI_UART_START_BIT_FALLING_EDGE,
                .noise_cancel         = SCI_UART_NOISE_CANCELLATION_DISABLE,
                .rx_fifo_trigger        = SCI_UART_RX_FIFO_TRIGGER_MAX,
                .p_baud_setting         = &COMP_UART_BUS_baud_setting,
                .flow_control           = SCI_UART_FLOW_CONTROL_RTS,
                #if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                .flow_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                .rs485_setting = {
                    .enable = SCI_UART_RS485_DISABLE,
                    .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
                #if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                    .de_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                },
            };

            /** UART interface configuration */
            const uart_cfg_t COMP_UART_BUS_cfg =
            {
                .channel             = 1,
                .data_bits           = UART_DATA_BITS_8,
                .parity              = UART_PARITY_OFF,
                .stop_bits           = UART_STOP_BITS_1,
                .p_callback          = comp_callback,
                .p_context           = NULL,
                .p_extend            = &COMP_UART_BUS_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_tx       = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_rx       = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
                .rxi_ipl             = (2),
                .txi_ipl             = (2),
                .tei_ipl             = (2),
                .eri_ipl             = (2),
#if defined(VECTOR_NUMBER_SCI1_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI1_RXI,
#else
                .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI1_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI1_TXI,
#else
                .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI1_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI1_TEI,
#else
                .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI1_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI1_ERI,
#else
                .eri_irq             = FSP_INVALID_VECTOR,
#endif
            };

/* Instance structure to use this module. */
const uart_instance_t COMP_UART_BUS =
{
    .p_ctrl        = &COMP_UART_BUS_ctrl,
    .p_cfg         = &COMP_UART_BUS_cfg,
    .p_api         = &g_uart_on_sci
};
iic_master_instance_ctrl_t EEPROM_I2C_BUS_ctrl;
const iic_master_extended_cfg_t EEPROM_I2C_BUS_extend =
{
    .timeout_mode             = IIC_MASTER_TIMEOUT_MODE_SHORT,
    .timeout_scl_low          = IIC_MASTER_TIMEOUT_SCL_LOW_ENABLED,
    /* Actual calculated bitrate: 99272. Actual calculated duty cycle: 49%. */ .clock_settings.brl_value = 27, .clock_settings.brh_value = 26, .clock_settings.cks_value = 1,
};
const i2c_master_cfg_t EEPROM_I2C_BUS_cfg =
{
    .channel             = 0,
    .rate                = I2C_MASTER_RATE_STANDARD,
    .slave               = 0x50,
    .addr_mode           = I2C_MASTER_ADDR_MODE_7BIT,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_tx       = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_rx       = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
    .p_callback          = EEPROM_i2c_callback,
    .p_context           = NULL,
#if defined(VECTOR_NUMBER_IIC0_RXI)
    .rxi_irq             = VECTOR_NUMBER_IIC0_RXI,
#else
    .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_IIC0_TXI)
    .txi_irq             = VECTOR_NUMBER_IIC0_TXI,
#else
    .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_IIC0_TEI)
    .tei_irq             = VECTOR_NUMBER_IIC0_TEI,
#else
    .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_IIC0_ERI)
    .eri_irq             = VECTOR_NUMBER_IIC0_ERI,
#else
    .eri_irq             = FSP_INVALID_VECTOR,
#endif
    .ipl                 = (2),
    .p_extend            = &EEPROM_I2C_BUS_extend,
};
/* Instance structure to use this module. */
const i2c_master_instance_t EEPROM_I2C_BUS =
{
    .p_ctrl        = &EEPROM_I2C_BUS_ctrl,
    .p_cfg         = &EEPROM_I2C_BUS_cfg,
    .p_api         = &g_i2c_master_on_iic
};
adc_instance_ctrl_t ADC_12_BUS_ctrl;
const adc_extended_cfg_t ADC_12_BUS_cfg_extend =
{
    .add_average_count   = ADC_ADD_OFF,
    .clearing            = ADC_CLEAR_AFTER_READ_ON,
    .trigger_group_b     = ADC_TRIGGER_SYNC_ELC,
    .double_trigger_mode = ADC_DOUBLE_TRIGGER_DISABLED,
    .adc_vref_control    = ADC_VREF_CONTROL_AVCC0_AVSS0,
    .enable_adbuf        = 0,
#if defined(VECTOR_NUMBER_ADC0_WINDOW_A)
    .window_a_irq        = VECTOR_NUMBER_ADC0_WINDOW_A,
#else
    .window_a_irq        = FSP_INVALID_VECTOR,
#endif
    .window_a_ipl        = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_ADC0_WINDOW_B)
    .window_b_irq      = VECTOR_NUMBER_ADC0_WINDOW_B,
#else
    .window_b_irq      = FSP_INVALID_VECTOR,
#endif
    .window_b_ipl      = (BSP_IRQ_DISABLED),
};
const adc_cfg_t ADC_12_BUS_cfg =
{
    .unit                = 0,
    .mode                = ADC_MODE_SINGLE_SCAN,
    .resolution          = ADC_RESOLUTION_12_BIT,
    .alignment           = (adc_alignment_t) ADC_ALIGNMENT_RIGHT,
    .trigger             = ADC_TRIGGER_SOFTWARE,
    .p_callback          = adcIsr,
    /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
    .p_context           = &NULL,
#endif
    .p_extend            = &ADC_12_BUS_cfg_extend,
#if defined(VECTOR_NUMBER_ADC0_SCAN_END)
    .scan_end_irq        = VECTOR_NUMBER_ADC0_SCAN_END,
#else
    .scan_end_irq        = FSP_INVALID_VECTOR,
#endif
    .scan_end_ipl        = (1),
#if defined(VECTOR_NUMBER_ADC0_SCAN_END_B)
    .scan_end_b_irq      = VECTOR_NUMBER_ADC0_SCAN_END_B,
#else
    .scan_end_b_irq      = FSP_INVALID_VECTOR,
#endif
    .scan_end_b_ipl      = (BSP_IRQ_DISABLED),
};
#if ((0) | (0))
const adc_window_cfg_t ADC_12_BUS_window_cfg =
{
    .compare_mask        =  0,
    .compare_mode_mask   =  0,
    .compare_cfg         = (0) | (0) | (0) | (ADC_COMPARE_CFG_EVENT_OUTPUT_OR),
    .compare_ref_low     = 0,
    .compare_ref_high    = 0,
    .compare_b_channel   = (ADC_WINDOW_B_CHANNEL_0),
    .compare_b_mode      = (ADC_WINDOW_B_MODE_LESS_THAN_OR_OUTSIDE),
    .compare_b_ref_low   = 0,
    .compare_b_ref_high  = 0,
};
#endif
const adc_channel_cfg_t ADC_12_BUS_channel_cfg =
{
    .scan_mask           = ADC_MASK_CHANNEL_0 | ADC_MASK_CHANNEL_1 | ADC_MASK_CHANNEL_2 | ADC_MASK_CHANNEL_3 | ADC_MASK_CHANNEL_4 | ADC_MASK_CHANNEL_5 | ADC_MASK_CHANNEL_6 | ADC_MASK_CHANNEL_7 | ADC_MASK_CHANNEL_8 | ADC_MASK_CHANNEL_9 | ADC_MASK_CHANNEL_11 | ADC_MASK_CHANNEL_12 | ADC_MASK_CHANNEL_13 | ADC_MASK_CHANNEL_14 |  0,
    .scan_mask_group_b   =  0,
    .priority_group_a    = ADC_GROUP_A_PRIORITY_OFF,
    .add_mask            =  0,
    .sample_hold_mask    =  0,
    .sample_hold_states  = 24,
#if ((0) | (0))
    .p_window_cfg        = (adc_window_cfg_t *) &ADC_12_BUS_window_cfg,
#else
    .p_window_cfg        = NULL,
#endif
};
/* Instance structure to use this module. */
const adc_instance_t ADC_12_BUS =
{
    .p_ctrl    = &ADC_12_BUS_ctrl,
    .p_cfg     = &ADC_12_BUS_cfg,
    .p_channel_cfg = &ADC_12_BUS_channel_cfg,
    .p_api     = &g_adc_on_adc
};
void g_hal_init(void) {
g_common_init();
}

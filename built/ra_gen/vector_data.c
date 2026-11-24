/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #pragma location = ".application_vectors"  // Dont forget to add this line when you generate new code using FSP RASC Program //OCE This is for OTA Update
        #if VECTOR_DATA_IRQ_COUNT > 0
        #if __has_include("r_ioport.h")
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI1 RXI (Received data full) */
            [1] = sci_uart_txi_isr, /* SCI1 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI1 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI1 ERI (Receive error) */
            [4] = sci_uart_rxi_isr, /* SCI9 RXI (Received data full) */
            [5] = sci_uart_txi_isr, /* SCI9 TXI (Transmit data empty) */
            [6] = sci_uart_tei_isr, /* SCI9 TEI (Transmit end) */
            [7] = sci_uart_eri_isr, /* SCI9 ERI (Receive error) */
            [8] = adc_scan_end_isr, /* ADC0 SCAN END (A/D scan end interrupt) */
            [9] = iic_master_txi_isr, /* IIC0 TXI (Transmit data empty) */
            [10] = iic_master_tei_isr, /* IIC0 TEI (Transmit end) */
            [11] = iic_master_eri_isr, /* IIC0 ERI (Transfer error) */
            [12] = iic_master_rxi_isr, /* IIC0 RXI (Receive data full) */
            [13] = sci_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [14] = sci_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [15] = sci_uart_eri_isr, /* SCI0 ERI (Receive error) */
            [16] = sci_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [17] = spi_txi_isr, /* SPI0 TXI (Transmit buffer empty) */
            [18] = spi_tei_isr, /* SPI0 TEI (Transmission complete event) */
            [19] = spi_eri_isr, /* SPI0 ERI (Error) */
            [20] = spi_rxi_isr, /* SPI0 RXI (Receive buffer full) */
            [22] = fcu_frdyi_isr, /* FCU FRDYI (Flash ready interrupt) */
        };
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [0] = BSP_PRV_IELS_ENUM(EVENT_SCI1_RXI), /* SCI1 RXI (Received data full) */
            [1] = BSP_PRV_IELS_ENUM(EVENT_SCI1_TXI), /* SCI1 TXI (Transmit data empty) */
            [2] = BSP_PRV_IELS_ENUM(EVENT_SCI1_TEI), /* SCI1 TEI (Transmit end) */
            [3] = BSP_PRV_IELS_ENUM(EVENT_SCI1_ERI), /* SCI1 ERI (Receive error) */
            [4] = BSP_PRV_IELS_ENUM(EVENT_SCI9_RXI), /* SCI9 RXI (Received data full) */
            [5] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TXI), /* SCI9 TXI (Transmit data empty) */
            [6] = BSP_PRV_IELS_ENUM(EVENT_SCI9_TEI), /* SCI9 TEI (Transmit end) */
            [7] = BSP_PRV_IELS_ENUM(EVENT_SCI9_ERI), /* SCI9 ERI (Receive error) */
            [8] = BSP_PRV_IELS_ENUM(EVENT_ADC0_SCAN_END), /* ADC0 SCAN END (A/D scan end interrupt) */
            [9] = BSP_PRV_IELS_ENUM(EVENT_IIC0_TXI), /* IIC0 TXI (Transmit data empty) */
            [10] = BSP_PRV_IELS_ENUM(EVENT_IIC0_TEI), /* IIC0 TEI (Transmit end) */
            [11] = BSP_PRV_IELS_ENUM(EVENT_IIC0_ERI), /* IIC0 ERI (Transfer error) */
            [12] = BSP_PRV_IELS_ENUM(EVENT_IIC0_RXI), /* IIC0 RXI (Receive data full) */
            [13] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TXI), /* SCI0 TXI (Transmit data empty) */
            [14] = BSP_PRV_IELS_ENUM(EVENT_SCI0_TEI), /* SCI0 TEI (Transmit end) */
            [15] = BSP_PRV_IELS_ENUM(EVENT_SCI0_ERI), /* SCI0 ERI (Receive error) */
            [16] = BSP_PRV_IELS_ENUM(EVENT_SCI0_RXI), /* SCI0 RXI (Receive data full) */
            [17] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TXI), /* SPI0 TXI (Transmit buffer empty) */
            [18] = BSP_PRV_IELS_ENUM(EVENT_SPI0_TEI), /* SPI0 TEI (Transmission complete event) */
            [19] = BSP_PRV_IELS_ENUM(EVENT_SPI0_ERI), /* SPI0 ERI (Error) */
            [20] = BSP_PRV_IELS_ENUM(EVENT_SPI0_RXI), /* SPI0 RXI (Receive buffer full) */
            [22] = BSP_PRV_IELS_ENUM(EVENT_FCU_FRDYI), /* FCU FRDYI (Flash ready interrupt) */
        };
        #elif __has_include("r_ioport_b.h")
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_IRQ_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
            [BSP_PRV_IELS_ENUM(SCI1_RXI)] = sci_uart_rxi_isr, /* SCI1 RXI (Received data full) */
            [BSP_PRV_IELS_ENUM(SCI1_TXI)] = sci_uart_txi_isr, /* SCI1 TXI (Transmit data empty) */
            [BSP_PRV_IELS_ENUM(SCI1_TEI)] = sci_uart_tei_isr, /* SCI1 TEI (Transmit end) */
            [BSP_PRV_IELS_ENUM(SCI1_ERI)] = sci_uart_eri_isr, /* SCI1 ERI (Receive error) */
            [BSP_PRV_IELS_ENUM(SCI9_RXI)] = sci_uart_rxi_isr, /* SCI9 RXI (Received data full) */
            [BSP_PRV_IELS_ENUM(SCI9_TXI)] = sci_uart_txi_isr, /* SCI9 TXI (Transmit data empty) */
            [BSP_PRV_IELS_ENUM(SCI9_TEI)] = sci_uart_tei_isr, /* SCI9 TEI (Transmit end) */
            [BSP_PRV_IELS_ENUM(SCI9_ERI)] = sci_uart_eri_isr, /* SCI9 ERI (Receive error) */
            [BSP_PRV_IELS_ENUM(ADC0_SCAN_END)] = adc_scan_end_isr, /* ADC0 SCAN END (A/D scan end interrupt) */
            [BSP_PRV_IELS_ENUM(IIC0_TXI)] = iic_master_txi_isr, /* IIC0 TXI (Transmit data empty) */
            [BSP_PRV_IELS_ENUM(IIC0_TEI)] = iic_master_tei_isr, /* IIC0 TEI (Transmit end) */
            [BSP_PRV_IELS_ENUM(IIC0_ERI)] = iic_master_eri_isr, /* IIC0 ERI (Transfer error) */
            [BSP_PRV_IELS_ENUM(IIC0_RXI)] = iic_master_rxi_isr, /* IIC0 RXI (Receive data full) */
            [BSP_PRV_IELS_ENUM(SCI0_TXI)] = sci_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [BSP_PRV_IELS_ENUM(SCI0_TEI)] = sci_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [BSP_PRV_IELS_ENUM(SCI0_ERI)] = sci_uart_eri_isr, /* SCI0 ERI (Receive error) */
            [BSP_PRV_IELS_ENUM(SCI0_RXI)] = sci_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [BSP_PRV_IELS_ENUM(SPI0_TXI)] = spi_txi_isr, /* SPI0 TXI (Transmit buffer empty) */
            [BSP_PRV_IELS_ENUM(SPI0_TEI)] = spi_tei_isr, /* SPI0 TEI (Transmission complete event) */
            [BSP_PRV_IELS_ENUM(SPI0_ERI)] = spi_eri_isr, /* SPI0 ERI (Error) */
            [BSP_PRV_IELS_ENUM(SPI0_RXI)] = spi_rxi_isr, /* SPI0 RXI (Receive buffer full) */
            [BSP_PRV_IELS_ENUM(FCU_FRDYI)] = fcu_frdyi_isr, /* FCU FRDYI (Flash ready interrupt) */
        };
        #endif
        #endif
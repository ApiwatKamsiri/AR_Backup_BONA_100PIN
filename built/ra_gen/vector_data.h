/* generated vector header file - do not edit */
        #ifndef VECTOR_DATA_H
        #define VECTOR_DATA_H
        #ifdef __cplusplus
        extern "C" {
        #endif
                /* Number of interrupts allocated */
        #ifndef VECTOR_DATA_IRQ_COUNT
        #define VECTOR_DATA_IRQ_COUNT    (22)
        #endif
        /* ISR prototypes */
        void sci_uart_rxi_isr(void);
        void sci_uart_txi_isr(void);
        void sci_uart_tei_isr(void);
        void sci_uart_eri_isr(void);
        void adc_scan_end_isr(void);
        void iic_master_txi_isr(void);
        void iic_master_tei_isr(void);
        void iic_master_eri_isr(void);
        void iic_master_rxi_isr(void);
        void spi_txi_isr(void);
        void spi_tei_isr(void);
        void spi_eri_isr(void);
        void spi_rxi_isr(void);
        void fcu_frdyi_isr(void);

        #if __has_include("r_ioport.h")
        /* Vector table allocations */
        #define VECTOR_NUMBER_SCI1_RXI ((IRQn_Type) 0) /* SCI1 RXI (Received data full) */
        #define SCI1_RXI_IRQn          ((IRQn_Type) 0) /* SCI1 RXI (Received data full) */
        #define VECTOR_NUMBER_SCI1_TXI ((IRQn_Type) 1) /* SCI1 TXI (Transmit data empty) */
        #define SCI1_TXI_IRQn          ((IRQn_Type) 1) /* SCI1 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI1_TEI ((IRQn_Type) 2) /* SCI1 TEI (Transmit end) */
        #define SCI1_TEI_IRQn          ((IRQn_Type) 2) /* SCI1 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI1_ERI ((IRQn_Type) 3) /* SCI1 ERI (Receive error) */
        #define SCI1_ERI_IRQn          ((IRQn_Type) 3) /* SCI1 ERI (Receive error) */
        #define VECTOR_NUMBER_SCI9_RXI ((IRQn_Type) 4) /* SCI9 RXI (Received data full) */
        #define SCI9_RXI_IRQn          ((IRQn_Type) 4) /* SCI9 RXI (Received data full) */
        #define VECTOR_NUMBER_SCI9_TXI ((IRQn_Type) 5) /* SCI9 TXI (Transmit data empty) */
        #define SCI9_TXI_IRQn          ((IRQn_Type) 5) /* SCI9 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI9_TEI ((IRQn_Type) 6) /* SCI9 TEI (Transmit end) */
        #define SCI9_TEI_IRQn          ((IRQn_Type) 6) /* SCI9 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI9_ERI ((IRQn_Type) 7) /* SCI9 ERI (Receive error) */
        #define SCI9_ERI_IRQn          ((IRQn_Type) 7) /* SCI9 ERI (Receive error) */
        #define VECTOR_NUMBER_ADC0_SCAN_END ((IRQn_Type) 8) /* ADC0 SCAN END (A/D scan end interrupt) */
        #define ADC0_SCAN_END_IRQn          ((IRQn_Type) 8) /* ADC0 SCAN END (A/D scan end interrupt) */
        #define VECTOR_NUMBER_IIC0_TXI ((IRQn_Type) 9) /* IIC0 TXI (Transmit data empty) */
        #define IIC0_TXI_IRQn          ((IRQn_Type) 9) /* IIC0 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_IIC0_TEI ((IRQn_Type) 10) /* IIC0 TEI (Transmit end) */
        #define IIC0_TEI_IRQn          ((IRQn_Type) 10) /* IIC0 TEI (Transmit end) */
        #define VECTOR_NUMBER_IIC0_ERI ((IRQn_Type) 11) /* IIC0 ERI (Transfer error) */
        #define IIC0_ERI_IRQn          ((IRQn_Type) 11) /* IIC0 ERI (Transfer error) */
        #define VECTOR_NUMBER_IIC0_RXI ((IRQn_Type) 12) /* IIC0 RXI (Receive data full) */
        #define IIC0_RXI_IRQn          ((IRQn_Type) 12) /* IIC0 RXI (Receive data full) */
        #define VECTOR_NUMBER_SCI0_TXI ((IRQn_Type) 13) /* SCI0 TXI (Transmit data empty) */
        #define SCI0_TXI_IRQn          ((IRQn_Type) 13) /* SCI0 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI0_TEI ((IRQn_Type) 14) /* SCI0 TEI (Transmit end) */
        #define SCI0_TEI_IRQn          ((IRQn_Type) 14) /* SCI0 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI0_ERI ((IRQn_Type) 15) /* SCI0 ERI (Receive error) */
        #define SCI0_ERI_IRQn          ((IRQn_Type) 15) /* SCI0 ERI (Receive error) */
        #define VECTOR_NUMBER_SCI0_RXI ((IRQn_Type) 16) /* SCI0 RXI (Receive data full) */
        #define SCI0_RXI_IRQn          ((IRQn_Type) 16) /* SCI0 RXI (Receive data full) */
        #define VECTOR_NUMBER_SPI0_TXI ((IRQn_Type) 17) /* SPI0 TXI (Transmit buffer empty) */
        #define SPI0_TXI_IRQn          ((IRQn_Type) 17) /* SPI0 TXI (Transmit buffer empty) */
        #define VECTOR_NUMBER_SPI0_TEI ((IRQn_Type) 18) /* SPI0 TEI (Transmission complete event) */
        #define SPI0_TEI_IRQn          ((IRQn_Type) 18) /* SPI0 TEI (Transmission complete event) */
        #define VECTOR_NUMBER_SPI0_ERI ((IRQn_Type) 19) /* SPI0 ERI (Error) */
        #define SPI0_ERI_IRQn          ((IRQn_Type) 19) /* SPI0 ERI (Error) */
        #define VECTOR_NUMBER_SPI0_RXI ((IRQn_Type) 20) /* SPI0 RXI (Receive buffer full) */
        #define SPI0_RXI_IRQn          ((IRQn_Type) 20) /* SPI0 RXI (Receive buffer full) */
        #define VECTOR_NUMBER_FCU_FRDYI ((IRQn_Type) 22) /* FCU FRDYI (Flash ready interrupt) */
        #define FCU_FRDYI_IRQn          ((IRQn_Type) 22) /* FCU FRDYI (Flash ready interrupt) */
        #endif

        #ifdef __cplusplus
        }
        #endif
        #endif /* VECTOR_DATA_H */
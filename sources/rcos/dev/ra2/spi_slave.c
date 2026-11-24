/** @file       ra2/spi_slave.c
 *  @brief      RA2 Serial Peripheral Interface
 *  @copyright  (c) 2019-Arcelik - All Rights Reserved
 *              Permission to use, reproduce, copy, prepare derivative works,
 *              modify, distribute, perform, display or sell this software and/or
 *              its documentation for any purpose is prohibited without the express
 *              written consent of Arcelik A.S.
 *  @author     Hamed Mohammadi
 *  @date       4/14/2021
 */
#define FILE_SPI_C

/**
 *  @addtogroup RA2_SPI
 *  @{
 */
#include "spi_slave.h"

//r_spi_write_read_common(consts->spi->p_ctrl, NULL, buffer, size, SPI_BIT_WIDTH_8_BITS);

//#define SPI_READ(buffer, size) consts->spi->p_api->read(consts->spi->p_ctrl, buffer, size, SPI_BIT_WIDTH_8_BITS)
#define SPI_READ(buffer, size)  R_SPI_Read(consts->spi->p_ctrl, buffer, size, SPI_BIT_WIDTH_8_BITS)
#define SPI_WRITE(buffer, size) R_SPI_Write(consts->spi->p_ctrl, buffer, size, SPI_BIT_WIDTH_8_BITS)
#define SPI_WRITEREAD(src, dst, size) R_SPI_WriteRead(consts->spi->p_ctrl, src, dst, size, SPI_BIT_WIDTH_8_BITS)

/* Max wait count for time-out operation */
#define MAX_COUNT 0xFFFF


/* Min wait count for time-out operation */
#define MIN_COUNT (0)
#define RESET_VALUE             (0x00)
static volatile uint32_t g_wait_count = MAX_COUNT;

#define SPRX0() { ctrl->p_regs->SPDCR &= ~(1 << R_SPI0_SPDCR_SPRDTD_Pos); }
#define SPTX0() { ctrl->p_regs->SPDCR |=  (1 << R_SPI0_SPDCR_SPRDTD_Pos); }
uint8_t readByte;
uint8_t writeByte;
uint8_t a;
uint8_t temp[26];

    uint32_t den;
void const* buffer1;
static DEV_COM_FUNC_INIT(spiInit);
static DEV_COM_FUNC_DEINIT(spiDeinit);
static DEV_COM_FUNC_OPEN(spiOpen);
static DEV_COM_FUNC_CLOSE(spiClose);
static DEV_COM_FUNC_SEND(spiSend);
static DEV_COM_FUNC_RECEIVE(spiReceive);

static volatile spi_event_t g_slave_event_flag;
static volatile uint8_t spiEventState;

volatile uint8_t spi_transfer_done,spi_transfer_abort,spi_fault_flag = 0;

const tsDevComFuncs devComSpiSlaveFuncs =
    {
        spiInit,
        spiDeinit,
        spiOpen,
        spiClose,
        spiSend,
        spiReceive,
        NULL,
        NULL,
};

void isReadyToConversion(void)
{
          while((SPI_EVENT_TRANSFER_COMPLETE != g_slave_event_flag))
        {
          /* Timer out operation if SPI operation fails to complete */
          g_wait_count--;
          if (MIN_COUNT >= g_wait_count)
          {
            spiEventState = 1;
              break;
          }
          else if ( (SPI_EVENT_TRANSFER_ABORTED == g_slave_event_flag))
          {
            spiEventState = 2;
            break;
          }
          else
          {
            spiEventState = 3;
            /* Do nothing */
          }
        }
        g_wait_count = MAX_COUNT;
        g_slave_event_flag  = (spi_event_t) RESET_VALUE;  // Reseting slave_event flag


}

void displayComProcess(const tsDevCom *device)
{
  
   /* tsSpiSlaveParams *params       = device->parameters;
    const tsSpiSlaveConsts *consts = device->constants;*/
    
   /* SPI_READ(temp, 1);
    isReadyToConversion();
    writeByte=0x02;
    SPI_WRITE(&writeByte, 1);
    isReadyToConversion();
    SPI_READ(temp+1, 1);
    isReadyToConversion();
    writeByte=0x02;
    SPI_WRITE(&writeByte, 1);
    isReadyToConversion();*/
/*            
    int s = 0;
     // uint8_t writearray [27] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x01,0x02,0x03,0x4F,0x4F,0x4F,0x4F,0x4F,0x4F,0x3F,0x01,0x88,0x00,0x00,0x41,0x00,0x00,0x00,0xC5};
     //   uint8_t writearray [27] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x01,0x02,0x03,0x51,0x01,0x4E,0x4E,0x4E,0x4E,0x3D,0x11,0x88,0x00,0x00,0x41,0x00,0x00,0x00,0x99};

    uint8_t writearray [27] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x01,0x02,0x03,0x4F,0x4F,0x4F,0x4F,0x4F,0x4F,0x00,0x01,0x88,0x00,0x00,0x41,0x00,0x00,0x00,0xC5};
    for (a=0; a<27; a++)
    {
      if(!g_slave_event_flag )
        SPI_WRITEREAD(writearray+a,temp+a, 1);
      isReadyToConversion();
      if(temp[a] == 0xAA && !s)
      {
        s++;
        temp[0] = temp[a];
        a = 0;
      }   
    }
*/
  /*  if(a<26)
      a++;
    else
      a=0;
       
    SPI_WRITEREAD(NULL,&readByte, 1);
    isReadyToConversion();
    params->rxbuffer[a]=readByte;*/
    /*switch (params->eSPIState){  
    case  _SPIState_Start:
      if(readByte == 0xAA)
      {
        params->eSPIState = _SPIState_Command;
        writeByte = eResponse_Byte_1;
        SPI_WRITEREAD(&writeByte,NULL,1);
        isReadyToConversion();
        params->rxbuffer[0]= readByte;
        crc=  crc8MaximInit();
        crc=  crc8MaximAddData(crc, readByte);
        index= 0;
      }
      else
      {
        writeByte = eResponse_Byte_0;
        SPI_WRITEREAD(&writeByte,NULL,1);
        isReadyToConversion();
      }
      break;
    case  _SPIState_Command: 
      params->rxbuffer[1]= readByte;
      params->eSPIState = _SPIState_Adress; 
      writeByte = eResponse_Byte_2;
      SPI_WRITEREAD(&writeByte,NULL,1);
      isReadyToConversion();
      crc= crc8MaximAddData(crc, readByte);
      break;
      
    case  _SPIState_Adress:
      params->rxbuffer[2]=readByte;
      params->eSPIState = _SPIState_Size;
      writeByte = eResponse_Byte_3;
      SPI_WRITEREAD(&writeByte,NULL,1);
      isReadyToConversion();
      crc= crc8MaximAddData(crc, readByte);
      break;
      
    case  _SPIState_Size:
      params->rxbuffer[3]= readByte;
      writeByte = params->txbuffer[0];
      SPI_WRITEREAD(&writeByte,NULL,1);
      isReadyToConversion();
      crc= crc8MaximAddData(crc, readByte);  
      if( params->rxbuffer[1] == 0x02)
      { 
        params->eSPIState = _SPIState_Command_RX_Data;
      }
      else 
      {
        crc= crc8MaximAddData(crc, params->txbuffer[0]);
        params->eSPIState = _SPIState_Command_TX_Data;
      }
      index++;
      break;
      
    case  _SPIState_Command_RX_Data:
      crc= crc8MaximAddData(crc, readByte); 
      params->rxbuffer[index-1+ params->rxbuffer[2]] = readByte;
      if(index>= params->rxbuffer[3])
        params->eSPIState = _SPIState_Command_RX_Checksum;
      index++;      
      break;
      
    case  _SPIState_Command_TX_Data:

      writeByte = params->txbuffer[index] ;
      SPI_WRITEREAD(&writeByte,NULL,1);
      isReadyToConversion();
      crc= crc8MaximAddData(crc, params->txbuffer[index]);
      index++;
      if(index>=params->rxbuffer[3]) 
        params->eSPIState = _SPIState_Command_TX_Checksum;         
      break;
      
    case  _SPIState_Command_RX_Checksum:
      params->rxbuffer[26] = readByte;
      if(crc==readByte && params->rxbuffer[15]!=0)
      {          
        dequePushBack(params->eventsQueue, (void const*)&params->rxbuffer[15], 1); //push the event received from dislay
      }
      params->eSPIState = _SPIState_Start;
      
      writeByte = 0x00;
      SPI_WRITEREAD(&writeByte,NULL,1);
      isReadyToConversion();
      index= 0; 
      crc= 0;

      break;
          
    case  _SPIState_Command_TX_Checksum:

      params->eSPIState = _SPIState_Start;
      writeByte = crc;
      SPI_WRITEREAD(&writeByte,NULL,1);
      isReadyToConversion();
      index= 0;
      crc=0;
      break;
      
      
    default:
      params->eSPIState = _SPIState_Start;
      writeByte = eResponse_Byte_0;
      SPI_WRITEREAD(&writeByte,NULL,1);
      isReadyToConversion();
      index= 0;
      break;
    }  */
} 


void spiIsr(const tsDevCom *device, spi_callback_args_t *p_args)
{
  //tsSpiSlaveParams *params       = device->parameters;
  
 /* static uint8_t index = 0;
  static uint8_t crc= 0; 
  IRQn_Type irq                  = R_FSP_CurrentIrqGet();
  spi_instance_ctrl_t *ctrl      = (spi_instance_ctrl_t *)R_FSP_IsrContextGet(irq);
  R_BSP_IrqStatusClear(irq);*/
  
  const tsSpiSlaveConsts *consts = device->constants;
  
  
   switch (p_args->event)
   {
       case SPI_EVENT_TRANSFER_COMPLETE:
       {
           spi_transfer_done = 1;
           break;
       }
       case SPI_EVENT_TRANSFER_ABORTED:
       {
           spi_transfer_abort = 1;
           break;
       }
       case SPI_EVENT_ERR_MODE_FAULT:
       {
           spi_fault_flag = 1;
           break;
       }
       case SPI_EVENT_ERR_READ_OVERFLOW:
       case SPI_EVENT_ERR_MODE_UNDERRUN:
       case SPI_EVENT_ERR_PARITY:
       case SPI_EVENT_ERR_OVERRUN: 
       case SPI_EVENT_ERR_FRAMING: 
       default:
       {
           spi_fault_flag = 1;
           break;
       } 
   }
   SPI_READ(NULL, 1); 
  
   /* if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
      g_slave_event_flag = SPI_EVENT_TRANSFER_COMPLETE;
    }
    else
    {
      g_slave_event_flag = SPI_EVENT_TRANSFER_ABORTED;
    }

    SPI_READ(NULL, 1); 


    isReadyToConversion();


    /*SPI_READ(&readByte, 1); 
    if(readByte==0xAA)
      ctrl->p_regs->SPDR_BY = 0x22;
    else if(readByte==0x01)
      ctrl->p_regs->SPDR_BY = 0x33;
    else if(readByte==0x02)
      ctrl->p_regs->SPDR_BY = 0x44;
    else if(readByte==0x0F)
      ctrl->p_regs->SPDR_BY = 0x55;
    else
      ctrl->p_regs->SPDR_BY = 0x66;
    isReadyToConversion();*/
  
 /* if(a<26)
    a++;
  else
    a=0;
  
  SPI_WRITEREAD(NULL,&readByte, 1);
  isReadyToConversion();
  params->rxbuffer[a]=readByte;*/
  /*    writeByte=0x88;
    if (irq == ctrl->p_cfg->rxi_irq) // SPI RXI interrupt
    {
         crc= 1;
    }
    else if (irq == ctrl->p_cfg->txi_irq) // SPI TXI interrupt
    {
        crc= 2;
    }
    else if (irq == ctrl->p_cfg->eri_irq)
    {
        ctrl->p_regs->SPSR;
        ctrl->p_regs->SPSR = 0;
    }




  isReadyToConversion();*/


  
    

  /*   SPI_WRITEREAD(NULL,&readByte, 1); 

    switch (params->eSPIState){  
    case  _SPIState_Start:
      if(readByte == 0xAA)
      {
        params->eSPIState = _SPIState_Command;
        //writeByte = eResponse_Byte_1;
        //SPI_WRITEREAD(&writeByte,NULL,1);
        //SPI_WRITE(&writeByte,1);
        ctrl->p_regs->SPDR_BY = eResponse_Byte_1;
        //isReadyToConversion();
        params->rxbuffer[0]= readByte;
        crc=  crc8MaximInit();
        crc=  crc8MaximAddData(crc, readByte);
        index= 0;
      }
      else
      {
        //writeByte = eResponse_Byte_0;
        //SPI_WRITEREAD(&writeByte,NULL,1);
        //SPI_WRITE(&writeByte,1);
        ctrl->p_regs->SPDR_BY = eResponse_Byte_0;
        //isReadyToConversion();
      }
      break;
    case  _SPIState_Command: 
      params->rxbuffer[1]= readByte;
      params->eSPIState = _SPIState_Adress; 
      //writeByte = eResponse_Byte_2;
      //SPI_WRITEREAD(&writeByte,NULL,1);
      //SPI_WRITE(&writeByte,1);
      ctrl->p_regs->SPDR_BY = eResponse_Byte_2;
      //isReadyToConversion();
      crc= crc8MaximAddData(crc, readByte);
      break;
      
    case  _SPIState_Adress:
      params->rxbuffer[2]=readByte;
      params->eSPIState = _SPIState_Size;
      //writeByte = eResponse_Byte_3;
      //SPI_WRITEREAD(&writeByte,NULL,1);
      //SPI_WRITE(&writeByte,1);
      //isReadyToConversion();
      ctrl->p_regs->SPDR_BY = eResponse_Byte_3;
      crc= crc8MaximAddData(crc, readByte);
      break;
      
    case  _SPIState_Size:
      params->rxbuffer[3]= readByte;
      //writeByte = params->txbuffer[0];
      //SPI_WRITEREAD(&writeByte,NULL,1);
      //SPI_WRITE(&writeByte,1);
      //isReadyToConversion();
      ctrl->p_regs->SPDR_BY = params->txbuffer[0];
      crc= crc8MaximAddData(crc, readByte);  
      if( params->rxbuffer[1] == 0x02)
      { 
        params->eSPIState = _SPIState_Command_RX_Data;
      }
      else 
      {
        crc= crc8MaximAddData(crc, params->txbuffer[0]);
        params->eSPIState = _SPIState_Command_TX_Data;
      }
      index++;
      break;
      
    case  _SPIState_Command_RX_Data:
      crc= crc8MaximAddData(crc, readByte); 
      params->rxbuffer[index-1+ params->rxbuffer[2]] = readByte;
      if(index>= params->rxbuffer[3])
        params->eSPIState = _SPIState_Command_RX_Checksum;
      index++;      
      break;
      
    case  _SPIState_Command_TX_Data:

      //writeByte = params->txbuffer[index] ;
      //SPI_WRITEREAD(&writeByte,NULL,1);
      //SPI_WRITE(&writeByte,1);
      //isReadyToConversion();
      ctrl->p_regs->SPDR_BY = params->txbuffer[index] ;
      crc= crc8MaximAddData(crc, params->txbuffer[index]);
      index++;
      if(index>=params->rxbuffer[3]) 
        params->eSPIState = _SPIState_Command_TX_Checksum;         
      break;
      
    case  _SPIState_Command_RX_Checksum:
      params->rxbuffer[26] = readByte;
      if(crc==readByte && params->rxbuffer[15]!=0)
      {          
        dequePushBack(params->eventsQueue, (void const*)&params->rxbuffer[15], 1); //push the event received from dislay
      }
      params->eSPIState = _SPIState_Start;
      
      //writeByte = 0x00;
      //SPI_WRITEREAD(&writeByte,NULL,1);
      //SPI_WRITE(&writeByte,1);
      ctrl->p_regs->SPDR_BY = 0x00;
      //isReadyToConversion();
      index= 0; 
      crc= 0;

      break;
          
    case  _SPIState_Command_TX_Checksum:

      params->eSPIState = _SPIState_Start;
      //writeByte = crc;
      //SPI_WRITEREAD(&writeByte,NULL,1);
      //SPI_WRITE(&writeByte,1);
      //isReadyToConversion();
      ctrl->p_regs->SPDR_BY = crc;
      index= 0;
      crc=0;
      break;
      
      
    default:
      params->eSPIState = _SPIState_Start;
      //writeByte = eResponse_Byte_0;
      //SPI_WRITEREAD(&writeByte,NULL,1);
      //SPI_WRITE(&writeByte,1);
      //isReadyToConversion();
      ctrl->p_regs->SPDR_BY = eResponse_Byte_0;
      index= 0;
      break;
    }  */
  
 /* if (irq == ctrl->p_cfg->eri_irq)
    {
        ctrl->p_regs->SPSR;
        ctrl->p_regs->SPSR = 0;
    }*/
  
  
//    tsSpiSlaveParams *params       = device->parameters;
//    const tsSpiSlaveConsts *consts = device->constants;
//    IRQn_Type irq                  = R_FSP_CurrentIrqGet();
//    spi_instance_ctrl_t *ctrl      = (spi_instance_ctrl_t *)R_FSP_IsrContextGet(irq);
//
//    R_BSP_IrqStatusClear(irq);
//    
//     static uint8_t index = 0;
//    static uint8_t crc= 0; 
//
//    if ((SPI_EVENT_TRANSFER_COMPLETE == p_args->event) &&((irq == ctrl->p_cfg->rxi_irq) || (irq == ctrl->p_cfg->tei_irq)))   
//    {
//        /*SPRX0();
//        uint8_t   &readByte = ctrl->p_regs->SPDR_BY;*/
//        //readByte = 0;
//        writeByte = 0;
//          SPI_READ(&readByte, 1);
//          //readByte = ctrl->p_regs->SPDR_BY;
//        switch (params->eSPIState){  
//    case  _SPIState_Start:
//         if(readByte == 0xAA)
//          {
//             params->eSPIState = _SPIState_Command;
//             /*SPTX0()
//             ctrl->p_regs->SPDR_BY = eResponse_Byte_1; */
//             writeByte = 7;
//             SPI_WRITE(&writeByte, 1);
//             params->rxbuffer[0]= readByte;
//             crc=  crc8MaximInit();
//             crc=  crc8MaximAddData(crc, readByte);
//             index= 0;
//           }
//          else
//          {
//            /*SPTX0()
//            ctrl->p_regs->SPDR_BY = eResponse_Byte_0;*/ //0
//            writeByte = eResponse_Byte_0;
//            SPI_WRITE(&writeByte, 1);
//          }
//     break;
//     
//     case  _SPIState_Command: 
//            params->rxbuffer[1]= readByte;
//            params->eSPIState = _SPIState_Adress; 
//            /*SPTX0()
//            ctrl->p_regs->SPDR_BY = eResponse_Byte_2; //2*/
//            writeByte = 6;
//            SPI_WRITE(&writeByte, 1);
//            crc= crc8MaximAddData(crc, readByte);
//     break;
//        
//     case  _SPIState_Adress:
//            params->rxbuffer[2]=readByte;
//            params->eSPIState = _SPIState_Size;
//            /*SPTX0()
//            ctrl->p_regs->SPDR_BY =eResponse_Byte_3;  //3*/
//            writeByte = 5;
//            SPI_WRITE(&writeByte, 1);
//            crc= crc8MaximAddData(crc, readByte);
//     break;
//          
//     case  _SPIState_Size:
//           // rxbuffer[3]= &readByte; 
//            params->rxbuffer[3]= readByte;
//            /*SPTX0()
//            ctrl->p_regs->SPDR_BY = params->txbuffer[0];*/
//            writeByte = 4;//params->txbuffer[0];
//            SPI_WRITE(&writeByte, 1);
//            crc= crc8MaximAddData(crc, readByte);  
//           if( params->rxbuffer[1] == 0x02)
//            { 
//              params->eSPIState = _SPIState_Command_RX_Data;
//             }
//           else 
//            {
//             crc= crc8MaximAddData(crc, params->txbuffer[0]);
//             params->eSPIState = _SPIState_Command_TX_Data;
//             }
//           index++;
//     break;
//          
//     case  _SPIState_Command_RX_Data:
//           crc= crc8MaximAddData(crc, readByte); 
//           params->rxbuffer[index-1+ params->rxbuffer[2]] = readByte;
//           if(index>= params->rxbuffer[3])
//             params->eSPIState = _SPIState_Command_RX_Checksum;
//           index++;      
//     break;
//          
//    case  _SPIState_Command_TX_Data:
//           /*SPTX0()
//           ctrl->p_regs->SPDR_BY = params->txbuffer[index];*/
//            writeByte = 3 ;//params->txbuffer[index];
//            SPI_WRITE(&writeByte, 1);
//           crc= crc8MaximAddData(crc, params->txbuffer[index]);
//           index++;
//           if(index>=params->rxbuffer[3]) 
//             params->eSPIState = _SPIState_Command_TX_Checksum;         
//    break;
//    
//    case  _SPIState_Command_RX_Checksum:
//          params->rxbuffer[26] = readByte;
//          if(crc==readByte && params->rxbuffer[15]!=0)
//           {          
//             dequePushBack(params->eventsQueue, (void const*)&params->rxbuffer[15], 1); //push the event received from dislay
//           }
//          params->eSPIState = _SPIState_Start;
//         /* SPTX0()
//          ctrl->p_regs->SPDR_BY =0x00;*/
//          writeByte = 0x00;
//          SPI_WRITE(&writeByte, 1);
//          index= 0; 
//          crc= 0;
//          break;
//          
//    case  _SPIState_Command_TX_Checksum:
//          params->eSPIState = _SPIState_Start;
//         /* SPTX0()
//          ctrl->p_regs->SPDR_BY =crc;*/
//          writeByte = 2;//crc;
//          SPI_WRITE(&writeByte, 1);
//          index= 0;
//          crc=0;
//          break;
//          
//    default:
//          params->eSPIState = _SPIState_Start;
//         /* SPTX0()
//          ctrl->p_regs->SPDR_BY =eResponse_Byte_0;  */
//          writeByte = 9;
//          SPI_WRITE(&writeByte, 1);
//          index= 0;
//          break;
//         }
//        if (irq == ctrl->p_cfg->eri_irq)
//        {
//          ctrl->p_regs->SPSR;
//          ctrl->p_regs->SPSR = 0;
//        }
//        
//   }
    
///////////////////////////////////////////////////////////////////////////////////7    

//    if (irq == ctrl->p_cfg->rxi_irq) // SPI RXI interrupt
//    {
//        uint8_t data = ctrl->p_regs->SPDR_BY;
//
//        if (FIFO_IS_EMPTY(params->txQue) && params->bytesTo&writeByte) // &writeByteing is finished
//        {
//            params->bytesTo&writeByte     = 0;
//            params->&writeByteingLastByte = FALSE;
//        }
//        else if (FIFO_IS_EMPTY(params->txQue)) // Don't fill receive buffer while &writeByteing
//        {
//            FIFO_PUSH(params->rxQue, (const void *)&data, 1);
//        }
//    }
//    else if (irq == ctrl->p_cfg->txi_irq) // SPI TXI interrupt
//    {
//        if (FIFO_LOAD(params->txQue))
//        {
//            FIFO_POP(params->txQue, (void *)&ctrl->p_regs-> , 1);
//            params->bytesTo&writeByte == 1 ? 1 : params->bytesTo&writeByte--;
//
//            if (FIFO_IS_EMPTY(params->txQue) && !params->&writeByteingLastByte) // FIFO is empty but there is still data in transmit register
//            {
//                uint8_t data            = 0;
//                params->&writeByteingLastByte = TRUE;
//                FIFO_PUSH(params->txQue, &data, sizeof(data));
//            }
//        }
//        else
//        {
//            ctrl->p_regs->SPDR = 0;
//        }
//    }
//    else if (irq == ctrl->p_cfg->eri_irq)
//    {
//        ctrl->p_regs->SPSR;
//        ctrl->p_regs->SPSR = 0;
//    }
}

/// @brief  Init function for a spi
static DEV_COM_FUNC_INIT(spiInit)
{
      const tsSpiSlaveConsts *consts = device->constants;
    ADD_RCOS_VERSION("ra2/spi_slave", "1.0.0")

    device->sys->initialized = TRUE;
    // Open SPI interface
    //consts->spi->p_api->open(consts->spi->p_ctrl, consts->spi->p_cfg);
    R_SPI_Open(consts->spi->p_ctrl, consts->spi->p_cfg);
    // Start receiving data
    SPI_READ(NULL, 1);
   /* buffer1="1";
    SPI_WRITE(buffer1,1);*/
    return EXIT_SUCCESS;
}

/// @brief  Deinit function for a spi
static DEV_COM_FUNC_DEINIT(spiDeinit)
{
    const tsSpiSlaveConsts *consts = device->constants;
//    tsSpiSlaveParams *params       = device->parameters;

    // Close the SPI interface
    consts->spi->p_api->close(consts->spi->p_ctrl);

    // Clear RX and TX queues
//    FIFO_CLEAR(params->txQue);
//    FIFO_CLEAR(params->rxQue);

    device->sys->initialized = FALSE;

    return EXIT_SUCCESS;
}

/// @brief  Open function for a spi
static DEV_COM_FUNC_OPEN(spiOpen)
{
//    tsSpiSlaveParams *params       = device->parameters;
    //const tsSpiSlaveConsts *consts = device->constants;

    if (!device->sys->initialized)
    {
        return EXIT_FAILURE;
    }

    // Clear RX and TX queues
//    FIFO_FLUSH(params->rxQue);
//    FIFO_FLUSH(params->txQue);

    // Open SPI interface
   // consts->spi->p_api->open(consts->spi->p_ctrl, consts->spi->p_cfg);

    // Start receiving data
   // SPI_READ(NULL, 1);

    return EXIT_SUCCESS;
}
/// @brief  Close function for a spi
static DEV_COM_FUNC_CLOSE(spiClose)
{
//   const tsSpiSlaveConsts *consts = device->constants;
//    tsDevComSystem *sys            = device->sys;

//    if (!sys->initialized)
//    {
//        return EXIT_FAILURE;
//    }
//
//    // Close the SPI interface
//    consts->spi->p_api->close(consts->spi->p_ctrl);
//
    return EXIT_SUCCESS;
}

/// @brief  &writeByte function for a spi
static DEV_COM_FUNC_SEND(spiSend)
{
//    tsSpiSlaveParams *params       = device->parameters;
//    const tsSpiSlaveConsts *consts = device->constants;

    if (!device->sys->initialized)
    {
        return EXIT_FAILURE;
    }

//    if (!length)
//    {
//        // Return data count in transmit buffer
//        length = params->bytesTo&writeByte;
//    }
//    else // if (!length)
//    {
//        if (txb == NULL)
//        {
//            // No operation
//        }
//        else // if (txb == NULL)
//        {
//            if (FIFO_FREE(params->txQue) >= length)
//            {
//                globalIntDisable();
//                // Place data into transmit buffer
//                FIFO_PUSH(params->txQue, txb, length);
//                params->bytesTo&writeByte = FIFO_LOAD(params->txQue);
//                globalIntEnable();
//            }
//            else
//            {
//                length = 0;
//            }
//        } // else // if (txb == NULL)
//    }     // else // if (!length)

    return length;
}

static DEV_COM_FUNC_RECEIVE(spiReceive)
{
//    tsDevComSystem *sys      = device->sys;
//    tsSpiSlaveParams *params = device->parameters;
//    tsSpiSlaveConsts *consts = device->constants;

//    if (!sys->initialized)
//    {
//        EXIT_FAILURE;
//    }
//
//    if (!length)
//    {
//        length = FIFO_LOAD(params->rxQue);
//    }
//    else // if (!length)
//    {
//        if (rxb == NULL)
//        {
//            // Dismiss received data
//            FIFO_FLUSH(params->rxQue); // Empty RX buffer
//        }
//        else // if (rxb == NULL)
//        {
//            if (FIFO_LOAD(params->rxQue) >= length)
//            {
//                globalIntDisable();
//                FIFO_POP(params->rxQue, rxb, length);
//                globalIntEnable();
//            }
//            else
//            {
//                length = 0;
//            }
//        } // else // if (rxb == NULL)
//    }     // else // if (!length)

    return length;
}

/** @} */

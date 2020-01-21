 /****************************************************************************
 Company: Dinamica y Desarrollo
 Designed by: Juan Sebastian Rincón Manrique 
 Date: 10/11/2017
 Description: This code allows to perform and manage the auto wake up unit
 by an interruption each 30 seconds and sending a flag to perform a specific
 function.
 Dependencies: AWU.h
 ****************************************************************************/

#include "AWU.h"
#include "ookrxtx.h"

#pragma vector = AWU_vector
__interrupt void AWU_IRQHandler(void)
{
  PE_ODR_ODR5 = 1;
  volatile unsigned char reg;
  AWUflag = 1;
  transmitFlag = 1;
  OOK_TX_PIN = 1;                //Toggle for stabilizing RX and as an preamble
  for (unsigned int i = 15000;i>0;i--){asm("nop"); }//delay
  OOK_TX_PIN = 0;                //Toggle for stabilizing RX and as an preamble
  for (unsigned int i = 10000;i>0;i--){asm("nop"); }//delay
   OOK_TX_PIN = 1;                //Toggle for stabilizing RX and as an preamble
  for (unsigned int i = 15000;i>0;i--){asm("nop"); }//delay
  OOK_TX_PIN = 0;                //Toggle for stabilizing RX and as an preamble
  for (unsigned int i = 10000;i>0;i--){asm("nop"); }//delay
  HeartBeat();
  PE_ODR_ODR5 = 0;
  reg = AWU_CSR1;       // Reading AWU_CSR1 register clears the interrupt flag
}

void AWU_Init() // Auto wake up each 30 seconds and send the broadcast
{ 
  AWU_CSR1_AWUEN = 0;
  AWU_APR_APR    = 0x3E;        // Auto wakeup prescaler 
  AWU_TBR_AWUTB  = 0xF;         //Auto wake up counter
  AWU_CSR1_AWUEN = 1;
}
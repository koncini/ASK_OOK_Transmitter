/**********************************************************
Company: Dinamica y Desarrollo								
Designed by: Juan Sebastian Rincón Manrique
Date: 10/10/2017
Description: This code includes the necesary functions to 
manage the ASK/OOk transmitter and reciever
from synoxo allows to send and recieve a 32
data block.
Dependencies: ookrxtx.h , Extevents.h
*********************************************************/

//////////////////////////////////////////////////////////////
//Libraries
//////////////////////////////////////////////////////////////

#include "ookrxtx.h"
#include "eeprom.h"

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//Exclusive ussage function variables
//////////////////////////////////////////////////////////////

unsigned int cap_val = 0;
unsigned char transmit_count = 0;
unsigned char dig_rec_count = 0;
unsigned char _state = 0;
unsigned char data_ok = 0;
uint32_t id_buffer = 0;
uint32_t beat_id = 0x0000FFFF;

//////////////////////////////////////////////////////////////

//Functions: The following are avalaible functions
/* 
InitialiseSystemClock(void);
TIM1_ICAP_Init(void);
GPIOTX_Init(void);
GPIORX_Init(void);
TXZero(void);
TXOne(void);
TIM4_Init(void);
TXSendData(uint32_t data);
TXSendDataChain(uint32_t data);
RXRecieveData(uint32_t data);
RXAlert(void);
*/

/************************************************************
** Timers interrupt handlers
************************************************************/

/*
#pragma vector=EXTI3_vector   //Interrupt handler for external interrupt PC3 20pin PD7 32pin 
__interrupt void EXTI_PORTD_IRQHandler(void)
{
  Exti3flag = 1;
  RXReceiveData();
}

#pragma vector = TIM1_OVR_UIF_vector
__interrupt void TIM1_UPD_OVF_IRQHandler(void)
{
  TIM1_CR1_CEN = 0;               //  Stop Timer 1.
  TIM1_SR2_CC1OF = 0;
  cap_val = TIM1_CNTRH + TIM1_CNTRL;
  dig_rec_count ++;
  RXDecodeData();
  TIM1_SR1_UIF = 0;               //  Reset the interrupt otherwise it will fire again straight away.
}*/

/*---------------------------Start init functions------------------------------*/

void GPIOTX_Init(void)
{
  /*Initializes the ports for the transmitter
  pin PA3 as an output                  Page 111
  output Push-pull                      Page 111
  switching speed - up to 10 MHz        Page 112*/
  OOK_TX_INIT_MACRO;
  BEEP_CSR_BEEPDIV = 0X03;
  beat_id = (beat_id & eepromReadShort(2)) << 16;
}

/*void GPIORX_Init(void)
{
  //Initializes the ports for the reciever
  //pin PC3 as an input (TLI 20pin) (TLI PD7 32 pin)     Page 111
  //Enable Interrupts                                    Page 112
  OOK_RX_INIT_MACRO;
  EXTI_CR1_PCIS = 0x0;       // Falling edge and low level to wake up
}

void TIM1_ICAP_Init(void)
{                  
  //Initializes TIM1 module to perform an input capture
  TIM1_PSCRH = 0x00;                        // Prescaler value=1                Page 241
  TIM1_PSCRL = 0xFF;                        // Prescaler value=1                Page 241
  TIM1_ARRH = 0x00;                         // Auto-reload register high byte   Page 241
  TIM1_ARRL = 0x07;                         // Auto-reload register low byte    Page 241
  TIM1_CCMR1 = 0x01;                        // Input capture mode for period    Page 200
  TIM1_CCMR2 = 0x02;                        // Input capture mode pulse length  Page 200
  TIM1_CCER1 = 0x02;                        // Icap enable 1 icap rising edge   Page 238
  TIM1_SMCR  = 0x54;                        // Icap settings                    Page 191
}*/

/*----------------------------End init functions-------------------------------*/


/*-------------------------------Start functions-------------------------------*/

void TXZero(void)
{
  /*Toggles for generate a chain for zero manchester encoding*/
  OOK_TX_PIN = 1;
  for (unsigned int i = 2300;i>0;i--){asm("nop"); }//delay
  OOK_TX_PIN = 0;
  for (unsigned int i = 500;i>0;i--){asm("nop"); }//delay
}

void TXOne(void)
{   
  /*Toggles for generate a chain for one manchester encoding*/
  OOK_TX_PIN = 1;
  for (unsigned int i = 500;i>0;i--){asm("nop"); }//delay
  OOK_TX_PIN = 0;
  for (unsigned int i = 2300;i>0;i--){asm("nop"); }//delay
}

void TXSendData(uint32_t data)
{
  /*Sends an specified word whith ID and information to communicate*/
  if (transmitFlag == 1)
  {                   //Flag for trigger the sending chain
    transmit_count++;
    for (unsigned int i = 10000;i>0;i--){asm("nop"); }//delay
    transmitFlag=0;
    for (unsigned i = 0 ; i <= 32 ; i++)        //Buffer the data
    {   
      uint32_t mask = 0x80000000;           //Data mask
      uint32_t masked = data & ( mask >> i); 
      if (masked == 0)
      {
        TXZero();                           //Generates Zero chain
      }
      else
      {
        TXOne();                            //Generates One chain
      }
    }
  }
}

void TXSendDataChain(uint32_t data) /*Sends a modulated datachain each period of time 20 times*/
{
  BEEP_CSR_BEEPEN = 1;
  BEEP_CSR_BEEPDIV = 0x00;
  TIM4_IER_UIE = 1;                    //Update interrupt enable                Page 253
  while(transmit_count<40)
  {
    OOK_TX_PIN = 1;                //Toggle for stabilizing RX and as an preamble
    for (unsigned int i = 15000;i>0;i--){asm("nop"); }//delay
    OOK_TX_PIN = 0;                //Toggle for stabilizing RX and as an preamble
    for (unsigned int i = 10000;i>0;i--){asm("nop"); }//delay
    OOK_TX_PIN = 1;                //Toggle for stabilizing RX and as an preamble
    for (unsigned int i = 15000;i>0;i--){asm("nop"); }//delay
    OOK_TX_PIN = 0;                //Toggle for stabilizing RX and as an preamble
    for (unsigned int i = 10000;i>0;i--){asm("nop"); }//delay//Toggle for stabilizing RX and as an preamble
    TXSendData(data);                  //Sends data just once
  }
  transmit_count = 0;                 //Clears the the counter
  TIM4_IER_UIE = 0;                  //Update interrupt disable               Page 253
  BEEP_CSR_BEEPDIV = 0x03;
  BEEP_CSR_BEEPEN = 0;
}

void HeartBeat()
{
  if (AWUflag==1)
  {
    AWUflag = 0;
    TXSendData(beat_id);
  }
} 

/*void RXAlert()  //Generates the alert when RX recieves a call
{
  if(id_buffer == beat_id)
  {
    Exti2flag = 0;
    for (unsigned int i = 0; i<33 ; i++)
    {
      BEEP_CSR = (_state<<6)+30;
      if(_state ^ 3) BEEP_CSR |= 32;
      for(unsigned long dummy = 0; dummy<25000;dummy++);
      _state = (_state+1)&3;
    }
    BEEP_CSR_BEEPEN = 0;
  }
}

void RXReceiveData()
{
  if (Exti3flag==1)
  {
    TIM1_EGR_UG = 0;
    TIM1_CCER1 = 0x11;
  }
}

void RXDecodeData()
{
  while (dig_rec_count <= 32){
    if (cap_val == 0x09)
    {
      id_buffer = 0;
    }
    else if (cap_val == 0x02)
    {
      id_buffer = 1;
    }
    else
    {
      
    }
    TIM1_CR1_CEN = 1;
  }
} */


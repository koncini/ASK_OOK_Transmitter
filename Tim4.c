 /****************************************************************************
 Company: Dinamica y Desarrollo
 Designed by: Juan Sebastian Rincón Manrique 
 Date: 09/11/2017
 Description: 
 Dependencies: Tim4.h
 ****************************************************************************/

#include "Tim4.h"

unsigned int time;

void Tim4_Init(uint8_t Counter, uint8_t Prescaler)
{
  TIM4_CR1_CEN = 1;                     //Counter enabled                       Page 250
  TIM4_PSCR = Prescaler;                //Prescaler value/128                   Page 244
  TIM4_CR1_ARPE = 1;                    //Auto-reload preload enable            Page 250
  TIM4_ARR = Counter;  
}

#pragma vector=TIM4_OVR_UIF_vector     //handler add                           
__interrupt void TIMR4_OVR(void)
{ 
  TIM4_SR_UIF = 0;                //reset the interrupt flag from the timer                       
  time++;
  if (time > 16)
  {
    transmitFlag^=1;
    time=0;
  }
}

/*Table 17-3.
   ___________________________ 
  |     |                     |
  |     |       Fcounter      |     COUNTER(TIM4_ARR) = Fcounter*Tinterupt
  |PSCR |  2MHz   HSI   clock |    
  |     |   Fmaster = 2MHz    |             
  |_____|_____________________|
  |     |                     |
  | 000 |         2MHz        |
  |_____|_____________________|
  |     |                     |
  | 001 |         1MHz        |
  |_____|_____________________|
  |     |                     |
  | 010 |        500KHz       |
  |_____|_____________________|
  |     |                     |
  | 011 |        250KHz       |
  |_____|_____________________|
  |     |                     |
  | 100 |        125KHz       |
  |_____|_____________________|
  |     |                     |
  | 101 |       62.5KHz       |
  |_____|_____________________|
  |     |                     |
  | 110 |       31.25KHz      |
  |_____|_____________________|
  |     |                     |
  | 111 |       15.625KHz     |
  |_____|_____________________|


 
*/

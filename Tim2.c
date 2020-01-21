 /****************************************************************************
 Company: Dinamica y  Desarrollo
 Designed by: Juan Sebastián Rincón Manrique
 Date: 01/11/2017
 Description: 
 Dependencies: Tim2.h
 ****************************************************************************/

#include "Tim2.h"
#include "lcd2x16.h"
   
#pragma vector=TIM2_OVR_UIF_vector 
__interrupt void TIMR2_OVR(void)
{
  Tim2flag ^= 1;
  LCDEnableBackLigth();
  TIM2_SR1_UIF = 0;
}

void Tim2_Init(uint8_t CounterH, uint8_t CounterL, uint8_t Prescaler)
{
  TIM2_PSCR = Prescaler;
  TIM2_ARRH = CounterH;                         //Auto-reload register high byte    Page 241
  TIM2_ARRL = CounterL;
  TIM2_CR1_CEN = 1;
}

/*Table 17-3.
   ____________________________ 
  |      |                      |
  |      |       Fcounter       |     COUNTER(TIM2_ARRH & TIM2_ARRL) = Fcounter*Tinterupt
  |PSCR  |  16MHz  HSI   clock  |    
  |      |   Fmaster = 16MHz    |             
  |______|______________________|
  |      |                      |
  | 0000 |         16MHz        |
  |______|______________________|
  |      |                      |
  | 0001 |         8MHz         |
  |______|______________________|
  |      |                      |
  | 0010 |         4MHz         |
  |______|______________________|
  |      |                      |
  | 0011 |         2MHz         |
  |______|______________________|
  |      |                      |
  | 0100 |         1MHz         |
  |______|______________________|
  |      |                      |
  | 0101 |        500KHz        |
  |______|______________________|
  |      |                      |
  | 0110 |        250KHz        |
  |______|______________________|
  |      |                      |
  | 0111 |        125KHz        |
  |______|______________________|
  |      |                      |
  | 1000 |        62.5KHz       |
  |______|______________________|
  |      |                      |
  | 1001 |       31.25KHz       |
  |______|______________________|
  |      |                      |
  | 1010 |       15.625KHz      |
  |______|______________________|
  |      |                      |
  | 1011 |       15.625KHz      |
  |______|______________________|
  |      |                      |
  | 1100 |       7.8125KHz      |
  |______|______________________|
  |      |                      |
  | 1101 |       3.906KHz       |
  |______|______________________|
  |      |                      |
  | 1110 |       1.953KHz       |
  |______|______________________|
  |      |                      |
  | 1111 |       976.562Hz      |
  |______|______________________|

*/
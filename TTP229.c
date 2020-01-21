/**********************************************************
Company: Dinamica y Desarrollo								
Designed by: Juan Sebastian Rincón Manrique
Date: 04/12/2017
Description: This library manages to communicate with the TTP229 chip from an stm8s003f 
microcontroller, it works with a TTP229 (16-Channel Digital Touch Capacitive Switch Sensor)   
using the 2-wires serial interface protocol - only 2 microcontroller pins.
Dependencies: TTP229.h , Extevents.h
*********************************************************/

/*********************************** CIRCUIT **********************************\
| General:                                                                     |
|	* TTP229 VCC to pin VCC                                                |
|	* TTP229 GND to pin GND                                                |
|	* TTP229 SCL to any pin you choose...                                  |
|	* TTP229 SDO to any pin you choose...                                  |
|                                                                              |
| 16 Buttons Mode (else 8 Buttons Mode):                                       |
|	* TTP229 TP2 to GND via 1 Megohm resistor!                             |
|	# Must use the ReadKey16, GetKeys16... else use the ReadKey8, GetKeys8 |
|                                                                              |
| Important:                                                                   |
|	* Must reconnect the TTP229 power so the mode changes will take effect |
|	* The 1 Megohm resistors already exist on some TTP229 modules          |
\******************************************************************************/

//////////////////////////////////////////////////////////////
//Libraries
//////////////////////////////////////////////////////////////

#include "TTP229.h"

uint8_t key_press;
char bit_flag;
uint8_t _count;
volatile uint8_t u8key_press;

/************************************************************
** Timers & interrupt handlers
************************************************************/

#pragma vector = EXTI2_vector
__interrupt void EXTI_PORTC_IRQHandler(void)
{
  for (unsigned int x = 200;x>0;x--){asm("nop");}; //delay arround 10 microseconds
  Exti2flag = 1;
  BEEP_CSR_BEEPEN = 1;
  PE_ODR_ODR5 = 1;
}

void KBWaitLong(void)
{
  for (unsigned long x = 160000;x>0;x--){asm("nop");}; //this delay uses 20 cycles for clock (arround 10 microseconds)  
}

void SCLWaitShort(void) //this delay uses 4 cycles for clock (arround 2 microseconds)
{ 
  for (unsigned int x = 80;x>0;x--){asm("nop");};
}

/*---------------------------Start init functions------------------------------*/

void TTP_Init()
{
  TTP_INIT_MACRO;
  TTP_SCL = 1;
  PE_ODR_ODR5 = 0;
  PE_DDR_DDR5 = 1;
  PE_CR1_C15 = 1;
}

/*-----------------------------End init functions------------------------------*/

uint8_t ReadKey()
{
  key();
  KBWaitLong();
  return key_press;
  bit_flag = 0;
}

void key()
{
  key_press = 'N';
  _count = 0;
  for (uint8_t i = 0; i < 16; i++)
  {
    GetBit();
    if (bit_flag == 1 && TTP_SDOVAL == 1)
    { 
      _count = i + 1; 
    }
    else
    {
      i = 16;
      _count = _count + 1;
      
      switch (_count)
      {
        
      case 0x01: 
        key_press='1';
        u8key_press = 0x01;
        break;
      case 0x02:
        key_press='2';
        u8key_press = 0x02;
        break;
      case 0x03:
        key_press='3';
        u8key_press = 0x03;
        break;
      case 0x04:
        key_press='A';
        break;
      case 0x05: 
        key_press='4';
        u8key_press = 0x04;
        break;
      case 0x06:
        key_press='5';
        u8key_press = 0x05;
        break;
      case 0x07:
        key_press='6';
        u8key_press = 0x06;
        break;
      case 0x08:
        key_press='B';
        break;
      case 0x09: 
        key_press='7';
        u8key_press = 0x07;
        break;
      case 0x0A:
        key_press='8';
        u8key_press = 0x08;
        break;
      case 0x0B:
        key_press='9';
        u8key_press = 0x09;
        break;
      case 0x0C:
        key_press='C';
        break;
      case 0x0D: 
        key_press='*';
        break;
      case 0x0E:
        key_press='0';
        u8key_press = 0x00;
        break;
      case 0x0F:
        key_press='#';
        break;
      case 0x10:
        key_press='D';
        break;
      default:
        key_press='N';    //Means that no key where pressed or a quick pulse 
        break;           //or more than one key where pressed
      }
      
      
    }
  }
}

void GetBit()
{
  TTP_SCL = 0;
  SCLWaitShort();
  TTP_SCL ^= TTP_SDOVAL;
  bit_flag = TTP_SCL;
  TTP_SCL = 1;
  SCLWaitShort();
}
#include <iostm8s003k3.h>
#include <string.h>
#include "ClockSys.h"
#include "lcd2x16.h"
#include "DataType.h"
#include "TTP229.h"
#include "ookrxtx.h"
#include "AWU.h"
#include "Tim2.h"
#include "Tim4.h"
#include "eeprom.h"

uint32_t _frame = 0xFFFFFFFF;   //The info to send
uint8_t u8NetID[2]={0,0};       //Stores values for company ID and machine ID
uint8_t u8Tecla[2]={0,0};       //Variable that carries key pressed
char menu_act = 1;              //The menu index variable
char mast_menu_count = 0;       //Counter for master menu
char pos_char_intro = 6;        //Index for the possition of the introduced char
char num_char_count = 0;        //Controls the number of chars actually introduced
char num_pg_id = 12;            //Counter for each pager id digit

void menu0(void) //Wellcome menu and dummy mode
{ 
  menu_act=0;
  
  LCDWriteString(0,1,"SIST LOCALIZADOR");
  LCDWriteString(0,2,"   D Y D 2018   ");
  
  if(u8Tecla[0]!='N')
  {
    menu_act=1;
    u8Tecla[0]='N';
  }
}

//Shows each menu
void menu1(void) //Calling menu
{ 
  menu_act=1;
  
  LCDWriteString(0,1," INGRESE NUMERO ");
  LCDGotoXY(0,2); 
  
  if(num_char_count == 0)
  {
    for (int i = 0; i < 16; i++) //Clears the remaining line
    { 
      LCDWriteChar(' ');
    }
    num_char_count = 1;
  }
  
  if(u8Tecla[0]=='D')
  {
    mast_menu_count=0;
    num_char_count = 0;
    for (int i = 0; i < 16; i++) //Clears the remaining line
    { 
      LCDWriteChar(' ');
    }
    pos_char_intro = 6;
    u8Tecla[0]='N';
  }
  
  else if (pos_char_intro<9)
  {
    if(u8Tecla[0] == 'A' || u8Tecla[0] == 'B' || u8Tecla[0] == '#')
    {
      u8Tecla[0]='N';
    }
    else
    {
      LCDGotoXY((pos_char_intro),2); 
      LCDWriteChar(u8Tecla[0]);
      _frame = _frame | (u8key_press << num_pg_id);
      pos_char_intro++;
      num_pg_id = num_pg_id - 4;
    }
  }
  
  else if(u8Tecla[0]=='*') //If user press '*' 8 times branch to the master menu
  {
    mast_menu_count++;
    menu_act=1;
    if(mast_menu_count==8)
    {
      menu_act = 2;
      num_char_count = 0;
      pos_char_intro = 6;
    }    
  }
  
  else if(u8Tecla[0]=='C')  //Calls to the specified pager
  {
    LCDWriteString(0,1,"   REALIZANDO   ");
    LCDWriteString(0,2,"    LLAMADA     ");
    for (unsigned int i = 3000;i>0;i--){asm("nop"); }//delay
    TXSendDataChain(_frame);
    num_pg_id = 12;
    mast_menu_count=0;
    num_char_count = 0;
    pos_char_intro = 6;
    for (int i = 0; i < 16; i++) //Clears the remaining line
    {
      LCDWriteChar(' ');
    }
    menu_act=0;
    _frame = _frame & 0xFFFF0000;
  }
}

void menu2(void) //Master menu
{
  menu_act=2;
  
  LCDWriteString(0,1,"INGRESE # DE RED");
  LCDGotoXY(0,2);
  
  if(num_char_count == 0)
  {
    for (int i = 0; i < 16; i++) //Clears the remaining line
    {
      LCDWriteChar(' ');
    }
    num_char_count = 1;
  }
  
  if(u8Tecla[0]=='D')
  {
    mast_menu_count=0;
    num_char_count = 0;
    for (int i = 0; i < 16; i++) //Clears the remaining line
    {
      LCDWriteChar(' ');
    }
    pos_char_intro = 5;
    u8Tecla[0]='N';
  }
  
  else if (pos_char_intro<9)
  {
    LCDGotoXY((pos_char_intro),2); 
    LCDWriteChar(u8Tecla[0]);
    switch (pos_char_intro)
    {
      
    case 5:
      u8NetID[0] = (u8NetID[0] & 0x0F) | ((u8Tecla[0] & 0x0F) << 4);
      break;
      
    case 6:
      u8NetID[0] = (u8NetID[0] & 0xF0) | (u8Tecla[0] & 0x0F);
      break;
      
    case 7:
      u8NetID[1] = (u8NetID[1] & 0x0F) | ((u8Tecla[0] & 0x0F) << 4);
      break;
      
    case 8:
      u8NetID[1] = (u8NetID[1] & 0xF0) | (u8Tecla[0] & 0x0F);
      break;
      
    }
    pos_char_intro++;
  }
  
  if(u8Tecla[0]=='A') //Net ID accepted operation
  {  
    PE_ODR_ODR5 = 1;
    eepromSwipeData();
    BEEP_CSR_BEEPEN = 1;
    eepromWriteData(2, u8NetID);
    LCDWriteString(0,1," NUMERO DE RED  ");
    LCDWriteString(0,2,"    ASIGNADO    ");
    
    for (unsigned long i = 500000;i>0;i--){asm("nop"); }//delay for init display
    u8Tecla[0]='N';
    num_char_count = 0;
    pos_char_intro = 6;
    PE_ODR_ODR5 = 0;
    BEEP_CSR_BEEPEN = 0;
    menu_act = 0;
  }
  
  if(u8Tecla[0]=='B')  //Back to the main menu
  {
    menu_act=0;
    u8Tecla[0]='N';
    num_char_count = 0;
    pos_char_intro = 6;
  } 
}

int main( void )
{
  __disable_interrupt();
  InitialiseSystemClock();
  Tim4_Init(0xFF,0x06);
  GPIOTX_Init();
  TTP_Init();
  LCD_Init();
  LCDEnableBackLigth();
  LCDWriteString(0,1,"SIST LOCALIZADOR");
  LCDWriteString(0,2,"   D Y D 2018   ");
  for (unsigned long i = 5000000;i>0;i--){asm("nop"); }//delay for init display
  Tim2_Init(0x0F,0xFF,0xF);           //Initializes the counter for LCD backligth 
  LCDDisableBackLigth();
  AWU_Init();
  __enable_interrupt();
  
  _frame = (_frame & eepromReadShort(1)) << 16;
  
  while(1)
  {
    if(Exti2flag)
    {
      PE_ODR_ODR5 = 1;
      TIM2_CNTRL = 0x00;                        
      TIM2_CNTRH = 0x00;
      TIM2_IER_UIE = 1;
      LCDEnableBackLigth(); 
      Tim2flag  = 1;
      u8Tecla[0]=ReadKey();
      BEEP_CSR_BEEPEN = 0;
      Exti2flag = 0;
      PE_ODR_ODR5 = 0;
      
      if(u8Tecla[0]!='N')
      {
        
        if(menu_act==1)
        {
          menu1();
        }
        if(menu_act==2)
        {
          menu2();
        }
        if(menu_act==0)
        {
          menu0();
        }
        
      }
    }
    
    if (Tim2flag==0)
    {
      LCDDisableBackLigth();
      __halt();
    }
  }
}
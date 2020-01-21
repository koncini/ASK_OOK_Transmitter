/********************************************************
Company: Dinamica y Desarrollo								
Designed by: Juan Sebastian Rincón Manrique
Date: 01/04/2018
Description: This code includes the necesary functions to 
manage the EEPROM functionalities.
Dependencies: eeprom.h 
*********************************************************/

#include "eeprom.h"

void eepromWriteData(uint8_t numberOfValues, uint8_t data[])
{
  //
  //  Check if the EEPROM is write-protected.  If it is then unlock the EEPROM.
  //
  if (FLASH_IAPSR_DUL == 0)
  {
    FLASH_DUKR = 0xAE;
    FLASH_DUKR = 0x56;
  }
  //
  //  Write the data to the EEPROM.
  //
  //numberOfValues = 2;
  char *address = (char *) EEPROM_DATA_START;     //  Data location in EEPROM.
  *address++ = (char) numberOfValues;
  for(uint8_t index = 0; index < numberOfValues; index++)
  {
      *address++ = data[index];
  }
  //
  //  Now write protect the EEPROM.
  //
  FLASH_IAPSR_DUL = 0;
}

//--------------------------------------------------------------------------------
//
//  Verify that the data in the EEPROM is the same as the data we
//  wrote originally.
//
uint32_t eepromReadLong(uint8_t numberOfValues)
{
  char *address = (char *) EEPROM_DATA_START;     //  Data location in EEPROM.
  uint32_t value = (*address++ << 16);
  for (int index = 0; index < numberOfValues; index++)
  {
    value += *address++;
    if(index < (numberOfValues-1))
    {
      value  = value << 8;
    }
  }
  return value;
}

uint16_t eepromReadShort(uint8_t numberOfValues)
{
  char *address = (char *) EEPROM_DATA_START + 1;     //  Data location in EEPROM.
  uint16_t value = (*address++ << 8);
  for (int index = 0; index < numberOfValues; index++)
  {
    value += *address++;
  }
  return value;
}

void eepromSwipeData()
{
  if (FLASH_IAPSR_DUL == 0)
  {
    FLASH_DUKR = 0xAE;
    FLASH_DUKR = 0x56;
  }
  //
  //  Write the data to the EEPROM.
  //
  char *address = (char *) EEPROM_BASE_ADDRESS;     //  Data location in EEPROM.
  uint8_t i = 0;
  while(i < 128)
  {
    *address++ = 0X00;
    i++;
  }
  //
  //  Now write protect the EEPROM.
  //
  FLASH_IAPSR_DUL = 0;
}
#include "eeprom_emulator.h"


void WriteToFlash(void* ptr, uint16_t size, uint32_t address ){
  
  __disable_irq();
  
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;
  
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = address;
  EraseInitStruct.NbPages = 1; 
  
  HAL_FLASH_Unlock();
  
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_WRPERR);
  
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
    Error_Handler();
  }
  
  const uint16_t * p = (const uint16_t*) ptr;
  
  for (int i = 0; i < size; i=i+2)
  {
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, i+address, (*p++)); 
  }
  
  HAL_FLASH_Lock();
  
  __enable_irq();
  
}

void ReadFromFlash(void* ptr, uint16_t size, uint32_t addres)
{

  __disable_irq();
  
  uint8_t * p = (uint8_t*) ptr;

  for (int j = 0; j < size; j++)
  {
    *p++ = *(uint8_t *)addres++;
  }
  
  __enable_irq();
}
#ifndef eeprom_emulator_H
#define eeprom_emulator_H

#define SECTOR31_FLASH_BEGINING 0x0801F000  //page 62 & 63, 2kb, sector 31
#include "stm32f0xx.h"




void WriteToFlash(void* ptr, uint16_t size, uint32_t address);
void ReadFromFlash(void* ptr, uint16_t size, uint32_t addres);


#endif
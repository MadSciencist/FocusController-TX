#ifndef _button_h_
#define _button_h_

#include "stm32f0xx.h"
#include <string.h> // memcpy
#include "FreeRTOS.h"
#include "task.h"
#include "gpio.h"
#include "eeprom_emulator.h"
#include "wireless_tasks.h"

#define BTN_DBNCE_SHORT 5 //this is abount  BTN_DBNCE_SHORT * 25ms (freeRTOS timer)
#define BTN_DBNCE_MED 20 
#define BTN_DBNCE_LONG 70

#define BTN_DBNCE_MS 50      //debounce 50ms
#define BTN_CLICK_LONG 1000 //1 second

typedef struct{
  uint32_t timeStamp;
  GPIO_PinState state;
  GPIO_PinState lastState;
  uint8_t longClickPendingRequest;
  uint8_t shortClickPendingRequest;
  uint8_t savedPreviousPosition;
  uint8_t setOnceFlag;
} buttonStateHandler_t;

enum pinButtonNames{
  SW1 = 0U,
  SW2,
  SW3,
  SW_E,
  SW_J,
  SW12,
  SW23,
  SW123,
};

void checkButton(buttonStateHandler_t* handler);
void buttonHandlersInit();
void pollButtons();
void processButtonsPendingRequests();


#endif
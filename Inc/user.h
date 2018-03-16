#ifndef _user_h_
#define _user_h_

#include "stm32f0xx.h"
#include <string.h> // memcpy
#include "FreeRTOS.h"
#include "wireless_tasks.h"
#include "task.h"
#include "gpio.h"

#define ADC_NUM_MEASURED_CHANNELS 10

typedef struct{
 uint16_t recallPos[3];
} recall_points_t;

typedef struct{
  uint16_t limitLeftInServoUs[3];
  uint16_t limitRightInServoUs[3];
  uint8_t isServoReversed[3];
  uint8_t isJoystickReversed[4];
  recall_points_t recall[3];
} user_parameters_t;

void initUserStruct(user_parameters_t* _user);
void filterADC(const uint16_t* raw, uint16_t* filtered);

#endif
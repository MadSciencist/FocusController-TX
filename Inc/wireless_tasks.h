#ifndef _wireless_tasks_h_
#define _wireless_tasks_h_

#include <stdlib.h>
#include "user.h"
#include "stm32f0xx.h"
#include "nrf24l01.h"
#include <string.h> // memcpy
#include "button.h"

#define V_DIV_RESISTOR_1  10000.0f
#define V_DIV_RESISTOR_2  10000.0f
#define V_CALIB_RATIO 2.234f
#define V_REFERENCE  3.3f
#define V_DIV_RATIO  (V_DIV_RESISTOR_1 / (V_DIV_RESISTOR_1 + V_DIV_RESISTOR_2))
#define CONV_RESOLUTION 4095.0f
#define BATTERY_EMPTY 3.185f
#define BATTERY_FULL  4.185f
#define SERVO_US_MIN 525
#define SERVO_US_MAX 2475

void NRF_writeNewCalibrations();
void NRF_writeNewPosition(const uint16_t* pos, const uint8_t* speed, const uint8_t* joy);

void NRF_build_message(uint8_t ID, const uint8_t* _buff);
void writeWireless();
void readWireless();

uint16_t writeServoValue(uint8_t servoNumber, uint16_t value);
uint16_t map_u16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);
float get_voltage(uint16_t bit_voltage);
float GetCapacity(float voltage);
float get_temperature(uint16_t bit_temperature);

#endif
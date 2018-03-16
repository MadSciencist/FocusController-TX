#include "user.h"

uint16_t ADC_filtered[ADC_NUM_MEASURED_CHANNELS];

const float encFilterCoeff_1 = 0.94f;
const float encFilterCoeff_2 = 0.06f;

void filterADC(const uint16_t* raw, uint16_t* filtered)
{
  static float lastValue[ADC_NUM_MEASURED_CHANNELS];
  float currentValue;
  
  for(uint8_t i = 0; i < ADC_NUM_MEASURED_CHANNELS; i++)
  {
    currentValue = (float)raw[i];
    currentValue = encFilterCoeff_1 * lastValue[i] + encFilterCoeff_2 * currentValue;
    lastValue[i] = currentValue;
    
    filtered[i] = (uint16_t)currentValue;
  }
}

void initUserStruct(user_parameters_t* _user)
{
  for(uint8_t i = 0; i < 3; i++)
  {
    _user->limitLeftInServoUs[i] = SERVO_US_MIN;
    _user->limitRightInServoUs[i] = SERVO_US_MAX;
    _user->isServoReversed[i] = 0;
    _user->isJoystickReversed[i] = 0;
  }
  _user->isJoystickReversed[3] = 0;
}
#include "wireless_tasks.h"

TM_NRF24L01_Transmit_Status_t transmissionStatus;
uint8_t isReceiverTimeout = 0;
uint8_t dataOut[18], dataIn[18];
extern uint16_t ADC_RAW[ADC_NUM_MEASURED_CHANNELS], ADC_Filtered[ADC_NUM_MEASURED_CHANNELS];
extern user_parameters_t user;
extern char receiverCapacityAscii[];
extern uint8_t newCalibrationsIsNeededToSend, encoderButtonCounter;
extern buttonStateHandler_t button1Handler, button2Handler, button3Handler, buttonEncHandler, buttonJoyHandler;
uint8_t test;

uint8_t joy_raw[3]; //x y z
uint8_t speed_raw[3]; //focus iris zoom
uint16_t pos_raw[3], pos_raw_previous[3], pos_to_write[3]; //focus iris zoom
uint8_t message[17];

void writeWireless()
{
  if(!newCalibrationsIsNeededToSend)
  {
    joy_raw[0] = ADC_Filtered[1] >> 4;
    joy_raw[1] = ADC_Filtered[2] >> 4;
    joy_raw[2] = ADC_Filtered[3] >> 4; 
    speed_raw[0] = ADC_Filtered[7] >> 4;
    speed_raw[1] = ADC_Filtered[6] >> 4;
    speed_raw[2] = ADC_Filtered[8] >> 4;
    pos_raw[0] = ADC_Filtered[4] >> 2; //0 - 1023
    pos_raw[1] = ADC_Filtered[9] >> 2;
    pos_raw[2] = ADC_Filtered[5] >> 2; 
    
    /*check if we have already set the flag  - it is do once only */
    if(button1Handler.shortClickPendingRequest != button1Handler.setOnceFlag){
      if(button1Handler.shortClickPendingRequest){
        /* clear any other requests to avoid colisions */
        button2Handler.shortClickPendingRequest = 0;
        button3Handler.shortClickPendingRequest = 0;
        button1Handler.setOnceFlag = 1;
      }
    }
    if(button2Handler.shortClickPendingRequest != button2Handler.setOnceFlag){
      if(button2Handler.shortClickPendingRequest){
        button1Handler.shortClickPendingRequest = 0;
        button3Handler.shortClickPendingRequest = 0;
        button2Handler.setOnceFlag = 1;
      }
    }   
    if(button3Handler.shortClickPendingRequest != button3Handler.setOnceFlag){
      if(button3Handler.shortClickPendingRequest){
        button1Handler.shortClickPendingRequest = 0;
        button2Handler.shortClickPendingRequest = 0;
        button3Handler.setOnceFlag = 1;
      }
    }
    
    /* button short click request to go to stored position */
    if(button1Handler.shortClickPendingRequest)
    {
      if(encoderButtonCounter == 0) /*this is focus*/
      {
        /* store raw analog position at first time after request */
        if(button1Handler.savedPreviousPosition)
        {
          pos_raw_previous[0] = pos_raw[0]; 
          button1Handler.savedPreviousPosition = 0;
        }
        
        /* if we havent move pot, write stored position
        pos raw is expected to be 0 - 1023 (analog range) so we are scaling it */
        pos_to_write[0] = writeServoValue(0, user.recall[0].recallPos[0]);
        
        /* check if raw analog position is different from the stored one
        in other words, check if we have moved pot, if so then send live positions
        also other requests can cancel this one*/
        if((abs((int)(pos_raw[0] - pos_raw_previous[0])) > 5))
          button1Handler.shortClickPendingRequest = 0;     
      }else if(encoderButtonCounter == 1) /*this is iris*/
      {
        if(button1Handler.savedPreviousPosition)
        {
          pos_raw_previous[1] = pos_raw[1]; 
          button1Handler.savedPreviousPosition = 0;
        }
        
        pos_to_write[1] = writeServoValue(1, user.recall[0].recallPos[1]);
        
        if((abs((int)(pos_raw[1] - pos_raw_previous[1])) > 5))
          button1Handler.shortClickPendingRequest = 0;     
      }else if(encoderButtonCounter == 2) /*this is zoom*/
      {
        if(button1Handler.savedPreviousPosition)
        {
          pos_raw_previous[2] = pos_raw[2]; 
          button1Handler.savedPreviousPosition = 0;
        }
        pos_to_write[2] = writeServoValue(2, user.recall[0].recallPos[2]);
        
        if((abs((int)(pos_raw[2] - pos_raw_previous[2])) > 5))
          button1Handler.shortClickPendingRequest = 0;     
      }
    }
    
    if(button2Handler.shortClickPendingRequest)
    {
      if(encoderButtonCounter == 0)
      {
        if(button2Handler.savedPreviousPosition)
        {
          pos_raw_previous[0] = pos_raw[0]; 
          button2Handler.savedPreviousPosition = 0;
        }
        
        pos_to_write[0] = writeServoValue(0, user.recall[1].recallPos[0]);
        
        if((abs((int)(pos_raw[0] - pos_raw_previous[0])) > 5))
          button2Handler.shortClickPendingRequest = 0;
      }else if(encoderButtonCounter == 1)
      {
        if(button2Handler.savedPreviousPosition)
        {
          pos_raw_previous[1] = pos_raw[1]; 
          button2Handler.savedPreviousPosition = 0;
        }
        
        pos_to_write[1] = writeServoValue(1, user.recall[1].recallPos[1]);
        
        if((abs((int)(pos_raw[1] - pos_raw_previous[1])) > 5))
          button2Handler.shortClickPendingRequest = 0;
      }else if(encoderButtonCounter == 2)
      {
        if(button2Handler.savedPreviousPosition)
        {
          pos_raw_previous[2] = pos_raw[2]; 
          button2Handler.savedPreviousPosition = 0;
        }
        
        pos_to_write[2] = writeServoValue(2, user.recall[1].recallPos[2]);
        
        if((abs((int)(pos_raw[2] - pos_raw_previous[2])) > 5))
          button2Handler.shortClickPendingRequest = 0;
      }
    }
    
    if(button3Handler.shortClickPendingRequest)
    {
      if(encoderButtonCounter == 0) /* focus */
      {
        if(button3Handler.savedPreviousPosition)
        {
          pos_raw_previous[0] = pos_raw[0]; 
          button3Handler.savedPreviousPosition = 0;
        }
        
        pos_to_write[0] = writeServoValue(0, user.recall[2].recallPos[0]);
        
        if((abs((int)(pos_raw[0] - pos_raw_previous[0])) > 5))
          button3Handler.shortClickPendingRequest = 0;
      }else if(encoderButtonCounter ==1)
      {
        if(button3Handler.savedPreviousPosition)
        {
          pos_raw_previous[1] = pos_raw[1]; 
          button3Handler.savedPreviousPosition = 0;
        }
        
        pos_to_write[1] = writeServoValue(1, user.recall[2].recallPos[1]);
        
        if((abs((int)(pos_raw[1] - pos_raw_previous[1])) > 5))
          button3Handler.shortClickPendingRequest = 0;
      }else if(encoderButtonCounter == 2)
      {
        if(button3Handler.savedPreviousPosition)
        {
          pos_raw_previous[2] = pos_raw[2]; 
          button3Handler.savedPreviousPosition = 0;
        }
        
        pos_to_write[2] = writeServoValue(2, user.recall[2].recallPos[2]);
        
        if((abs((int)(pos_raw[2] - pos_raw_previous[2])) > 5))
          button3Handler.shortClickPendingRequest = 0;
      }
    }
    
    
    if((!button1Handler.shortClickPendingRequest) && (!button2Handler.shortClickPendingRequest) && (!button3Handler.shortClickPendingRequest)) //there is no requests
      pos_to_write[0] = writeServoValue(0, pos_raw[0]); //send live value
    else if(encoderButtonCounter != 0) //there is request, but from other device (iris or zoom), still send focus live data
      pos_to_write[0] = writeServoValue(0, pos_raw[0]);
    
    if((!button1Handler.shortClickPendingRequest) && (!button2Handler.shortClickPendingRequest) && (!button3Handler.shortClickPendingRequest)) //there is no requests
      pos_to_write[1] = writeServoValue(1, pos_raw[1]); //send live value
    else if(encoderButtonCounter != 1) //there is request, but from other device (iris or zoom), still send focus live data
      pos_to_write[1] = writeServoValue(1, pos_raw[1]);
    
    if((!button1Handler.shortClickPendingRequest) && (!button2Handler.shortClickPendingRequest) && (!button3Handler.shortClickPendingRequest)) //there is no requests
      pos_to_write[2] = writeServoValue(2, pos_raw[2]); //send live value
    else if(encoderButtonCounter != 2) //there is request, but from other device (iris or zoom), still send focus live data
      pos_to_write[2] = writeServoValue(2, pos_raw[2]);
    
    
    NRF_writeNewPosition(pos_to_write, speed_raw, joy_raw);
    
  }else /*calibration send request */
    NRF_writeNewCalibrations(&user);
  
  TM_NRF24L01_Transmit(message);
  
  do {
    transmissionStatus = TM_NRF24L01_GetTransmissionStatus();
  } while (transmissionStatus == TM_NRF24L01_Transmit_Status_Sending);
  
  if(transmissionStatus == TM_NRF24L01_Transmit_Status_Ok){
    //to implement
  }else if(transmissionStatus == TM_NRF24L01_Transmit_Status_Lost){
    //to implement
  }
  
  TM_NRF24L01_PowerUpRx();
}

uint16_t writeServoValue(uint8_t servoNumber, uint16_t value)
{
  uint16_t servo_pos;
  
  if(user.isServoReversed[servoNumber])
    servo_pos = map_u16(value, 0, 1023, user.limitLeftInServoUs[servoNumber], user.limitRightInServoUs[servoNumber]);
  else
  { 
    servo_pos = map_u16(value, 0, 1023, user.limitLeftInServoUs[servoNumber], user.limitRightInServoUs[servoNumber]);
    servo_pos = user.limitRightInServoUs[servoNumber] - servo_pos + user.limitLeftInServoUs[servoNumber];
  }
  
  return servo_pos;
}

void NRF_writeNewCalibrations()
{
  uint8_t buff[13];
  
  buff[0] = user.limitLeftInServoUs[0];
  buff[1] = user.limitLeftInServoUs[0] >> 8;
  buff[2] = user.limitLeftInServoUs[1];
  buff[3] = user.limitLeftInServoUs[1] >> 8;
  buff[4] = user.limitLeftInServoUs[2];
  buff[5] = user.limitLeftInServoUs[2] >> 8;
  buff[6] = user.limitRightInServoUs[0];
  buff[7] = user.limitRightInServoUs[0] >> 8;
  buff[8] = user.limitRightInServoUs[1];
  buff[9] = user.limitRightInServoUs[1] >> 8;
  buff[10] = user.limitRightInServoUs[2];
  buff[11] = user.limitRightInServoUs[2] >> 8;
  
  buff[12] = 1;
  
  NRF_build_message(2, buff);
}

void readWireless()
{
  static uint32_t previousTime = 0;
  if((HAL_GetTick() - previousTime) > 250)
    isReceiverTimeout = 1;
  else isReceiverTimeout = 0;
  
  for(int i = 0; i< 32; i++)
    dataIn[i] = 0;
  
  if(TM_NRF24L01_DataReady())
  {
    TM_NRF24L01_GetData(dataIn);
    previousTime = HAL_GetTick();
  }
  
  if(dataIn[0] == '<' && dataIn[16] == '>'){
    switch(dataIn[1])
    {
    case 1:
      memcpy(receiverCapacityAscii, &dataIn[2], 4);
      break;
      
    case 2:
      if(dataIn[2] == 'o' && dataIn[3] == 'k') //new calibration values received ok
        newCalibrationsIsNeededToSend = 0; 
      break;
      
    default:
      break;
    }
  }
}

void NRF_writeNewPosition(const uint16_t* pos, const uint8_t* speed, const uint8_t* joy)
{
  uint8_t buff[14];
  
  buff[0] = pos[0];
  buff[1] = pos[0] >> 8;
  
  buff[2] = pos[1];
  buff[3] = pos[1] >> 8;
  
  buff[4] = pos[2];
  buff[5] = pos[2] >> 8;
  
  for(int i = 0; i < 3; i++)
    buff[i+6] = speed[i];
  
  for(int i = 0; i < 3; i++)
    buff[i+9] = joy[i];
  
  buff[12] = 1;
  
  NRF_build_message(1, buff);
}

void NRF_build_message(uint8_t ID, const uint8_t* _buff)
{
  static uint8_t cntr = 0;
  if(cntr++ > 254) cntr = 0;
  
  message[0] = '<';
  message[1] = cntr;
  message[2] = ID;
  
  for(uint8_t i = 0; i<14; i++)
    message[i+3] = _buff[i];
  
  message[16] = '>';
}

uint16_t map_u16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
  uint16_t tmp;
  if((x - in_min) < 0)
    tmp = 0;
  else tmp = x - in_min;
  return (tmp * (out_max - out_min) / (in_max - in_min) + out_min);
}

float get_voltage(uint16_t bit_voltage) //adc0
{
  //return ((bit_voltage * (V_REFERENCE / CONV_RESOLUTION)) / V_DIV_RATIO);
  return ((bit_voltage * (V_REFERENCE / CONV_RESOLUTION)) * V_CALIB_RATIO);
}

float GetCapacity(float _v)
{
  float cap = (_v - BATTERY_EMPTY) * (100.0) / (BATTERY_FULL - BATTERY_EMPTY);
  if(cap > 100.0f)
    return 100.0f;
  else if(cap < 0.0f)
    return 0;
  else return cap;
}

float get_temperature(uint16_t bit_temperature)
{
  const float V30 = 1.43f; // [Volts]
  const float Avg_slope = 0.0043f; //[Volts/degree]
  float Vsense = (V_REFERENCE * bit_temperature) / CONV_RESOLUTION;// Przeliczenie wartosci zmierzonej na napiecie
  return ((Vsense - V30) / Avg_slope) + 30.0f;// Obliczenie temperatury
}
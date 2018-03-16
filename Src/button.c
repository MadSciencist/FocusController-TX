#include "button.h"

extern user_parameters_t user;
extern uint16_t ADC_Filtered[ADC_NUM_MEASURED_CHANNELS];
GPIO_PinState currentPinStates[] = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET};
uint8_t buttonGroupIsBlocked[] = {0, 0, 0}; //button groups 123, 12, 23
uint8_t buttonPendingRequest[] = {0, 0, 0, 0, 0, 0, 0, 0}; 
uint8_t encoderButtonCounter = 0;
buttonStateHandler_t button1Handler, button2Handler, button3Handler, buttonEncHandler, buttonJoyHandler;
uint8_t newCalibrationsIsNeededToSend = 0;

void processButtonsPendingRequests()
{
    if(buttonPendingRequest[SW12])
    {
      uint16_t tmp; 
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_SET);
      
      if(encoderButtonCounter == 0){  //focus
        tmp = map_u16(ADC_Filtered[4] >> 2, 0, 1023, SERVO_US_MIN, SERVO_US_MAX);
        if(tmp < user.limitRightInServoUs[0])
          user.limitLeftInServoUs[0] = tmp;
      }
      else if(encoderButtonCounter == 1){ //iris
        tmp = map_u16(ADC_Filtered[9] >> 2, 0, 1023, SERVO_US_MIN, SERVO_US_MAX);
        if(tmp < user.limitRightInServoUs[1])
          user.limitLeftInServoUs[1] = tmp;
      }
      else if(encoderButtonCounter == 2){ //zoom
        tmp = map_u16(ADC_Filtered[5] >> 2, 0, 1023, SERVO_US_MIN, SERVO_US_MAX);
        if(tmp < user.limitRightInServoUs[2])
          user.limitLeftInServoUs[2] = tmp;
      }
      
      WriteToFlash(&user, sizeof(user), SECTOR31_FLASH_BEGINING);
      newCalibrationsIsNeededToSend = 1;
      buttonPendingRequest[SW12] = 0;
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_RESET);
    }
    
    if(buttonPendingRequest[SW23])
    {
      uint16_t tmp;   
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_SET);

      if(encoderButtonCounter == 0){  //focus
        tmp = map_u16(ADC_Filtered[4] >> 2, 0, 1023, SERVO_US_MIN, SERVO_US_MAX);
        if(tmp > user.limitLeftInServoUs[0])
          user.limitRightInServoUs[0] = tmp;
      }
      else if(encoderButtonCounter == 1){ //iris
        tmp = map_u16(ADC_Filtered[9] >> 2, 0, 1023, SERVO_US_MIN, SERVO_US_MAX);
        if(tmp > user.limitLeftInServoUs[1])
          user.limitRightInServoUs[1] = tmp;
      }
      else if(encoderButtonCounter == 2){ //zoom
        tmp = map_u16(ADC_Filtered[5] >> 2, 0, 1023, SERVO_US_MIN, SERVO_US_MAX);
        if(tmp > user.limitLeftInServoUs[2])
          user.limitRightInServoUs[2] = tmp;
      }
      
      WriteToFlash(&user, sizeof(user), SECTOR31_FLASH_BEGINING);
      newCalibrationsIsNeededToSend = 1;
      buttonPendingRequest[SW23] = 0;
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_RESET);
    }
    
    if(buttonPendingRequest[SW123])
    {
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_SET);
      if(encoderButtonCounter == 0){ //focus
        user.limitLeftInServoUs[0] = SERVO_US_MIN;
        user.limitRightInServoUs[0] = SERVO_US_MAX;
      }else if(encoderButtonCounter == 1){
        user.limitLeftInServoUs[1] = SERVO_US_MIN;
        user.limitRightInServoUs[1] = SERVO_US_MAX;
      }else if(encoderButtonCounter == 2){
        user.limitLeftInServoUs[2] = SERVO_US_MIN;
        user.limitRightInServoUs[2] = SERVO_US_MAX;
      }
      buttonPendingRequest[SW123] = 0;
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_RESET);
    }
    
    //recal[button.recallPos[0-2] 0 - focus, 1 - iris, 2 - zoom
    //recal points are recorder in servo us
    if(button1Handler.longClickPendingRequest)
    {
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_SET);
      if(encoderButtonCounter == 0) //focus
        user.recall[0].recallPos[0] = ADC_Filtered[4] >> 2;
      else if(encoderButtonCounter == 1) //iris
        user.recall[0].recallPos[1] = ADC_Filtered[9] >> 2;
      else if(encoderButtonCounter == 2) //zoom
        user.recall[0].recallPos[2] = ADC_Filtered[5] >> 2;
      
      WriteToFlash(&user, sizeof(user), SECTOR31_FLASH_BEGINING);
      button1Handler.longClickPendingRequest = 0;
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_RESET);
    }
    
    if(button2Handler.longClickPendingRequest)
    {
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_SET);
      if(encoderButtonCounter == 0) //focus
        user.recall[1].recallPos[0] = ADC_Filtered[4] >> 2;
      else if(encoderButtonCounter == 1) //iris
        user.recall[1].recallPos[1] = ADC_Filtered[9] >> 2;
      else if(encoderButtonCounter == 2) //zoom
        user.recall[1].recallPos[2] = ADC_Filtered[5] >> 2;
      
      WriteToFlash(&user, sizeof(user), SECTOR31_FLASH_BEGINING);
      button2Handler.longClickPendingRequest = 0;
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_RESET);
    }
    
    if(button3Handler.longClickPendingRequest)
    {
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_SET);
      if(encoderButtonCounter == 0) //focus
        user.recall[2].recallPos[0] = ADC_Filtered[4] >> 2;
      else if(encoderButtonCounter == 1) //iris
        user.recall[2].recallPos[1] = ADC_Filtered[9] >> 2;
      else if(encoderButtonCounter == 2) //zoom
        user.recall[2].recallPos[2] = ADC_Filtered[5] >> 2;
      
      WriteToFlash(&user, sizeof(user), SECTOR31_FLASH_BEGINING);
      button3Handler.longClickPendingRequest = 0;
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_RESET);
    }
    
    if(buttonEncHandler.shortClickPendingRequest)
    {
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_SET);
      if(encoderButtonCounter++ > 1)
        encoderButtonCounter = 0;
      buttonEncHandler.shortClickPendingRequest = 0;
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_RESET);
    }
    
    if(buttonJoyHandler.shortClickPendingRequest)
    {
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_SET);
      if(encoderButtonCounter == 0){
        if(user.isServoReversed[0] == 0)
          user.isServoReversed[0] = 1;
        else user.isServoReversed[0] = 0;
      }     
      if(encoderButtonCounter == 1){
        if(user.isServoReversed[1] == 0)
          user.isServoReversed[1] = 1;
        else user.isServoReversed[1] = 0;
      }
      if(encoderButtonCounter == 2){
        if(user.isServoReversed[2] == 0)
          user.isServoReversed[2] = 1;
        else user.isServoReversed[2] = 0;
      }    
      buttonJoyHandler.shortClickPendingRequest = 0;
      HAL_GPIO_WritePin(GPIOA, STS_LED_Pin, GPIO_PIN_RESET);
    }
}

void pollButtons() //first abstract level -  used for debounce
{ 
  static uint8_t timerCounter[] = {0, 0, 0, 0, 0, 0, 0, 0}; //50 ms
  /* non-group buttons */
  button1Handler.state = HAL_GPIO_ReadPin(GPIOB, SW1_Pin);
  button2Handler.state  = HAL_GPIO_ReadPin(GPIOC, SW2_Pin);
  button3Handler.state = HAL_GPIO_ReadPin(GPIOC, SW3_Pin);
  buttonEncHandler.state = HAL_GPIO_ReadPin(GPIOB, ENC_SW_Pin);
  buttonJoyHandler.state = HAL_GPIO_ReadPin(GPIOB, JOY_SW_Pin);
  
  currentPinStates[SW1] = button1Handler.state;
  currentPinStates[SW2] = button2Handler.state;
  currentPinStates[SW3] = button3Handler.state;
  currentPinStates[SW_E] = buttonEncHandler.state;
  currentPinStates[SW_J] = buttonJoyHandler.state;
  
  /* group buttons 1-2-3  */
  if((currentPinStates[SW1] == GPIO_PIN_RESET) && (currentPinStates[SW2] == GPIO_PIN_RESET) && (currentPinStates[SW3] == GPIO_PIN_RESET)){
    buttonGroupIsBlocked[0] = 1;
    if(++timerCounter[SW123] > BTN_DBNCE_MED){
      timerCounter[SW123] = 0;
      buttonPendingRequest[SW123] = 1;
    }
  }else{
    buttonGroupIsBlocked[0] = 0;
    timerCounter[SW123] = 0;
  }
  
  /* group buttons 1-2 */
  if(!buttonGroupIsBlocked[0]) { //check if 1-2-3 combination is runing
    if((currentPinStates[SW1] == GPIO_PIN_RESET) && (currentPinStates[SW2] == GPIO_PIN_RESET)){
      buttonGroupIsBlocked[1] = 1;
      if(++timerCounter[SW12] > BTN_DBNCE_MED){
        timerCounter[SW12] = 0;
        buttonPendingRequest[SW12] = 1;
      }
    }else{
      buttonGroupIsBlocked[1] = 0;
      timerCounter[SW12] = 0;
    }
  }
  
  /* group buttons 2-3*/
  if((!buttonGroupIsBlocked[0])) { //check if 1-2-3 combination is runing
    if((currentPinStates[SW2] == GPIO_PIN_RESET) && (currentPinStates[SW3] == GPIO_PIN_RESET)){
      buttonGroupIsBlocked[2] = 1;
      if(++timerCounter[SW23] > BTN_DBNCE_MED){
        timerCounter[SW23] = 0;
        buttonPendingRequest[SW23] = 1;
      }
    }else{
      buttonGroupIsBlocked[2] = 0;
      timerCounter[SW23] = 0;
    }
  }
  
  if((!buttonGroupIsBlocked[0]) && (!buttonGroupIsBlocked[1]))
    checkButton(&button1Handler);
  
  if((!buttonGroupIsBlocked[0]) && (!buttonGroupIsBlocked[1]) && (!buttonGroupIsBlocked[2]))
    checkButton(&button2Handler);
  
  if((!buttonGroupIsBlocked[0]) && (!buttonGroupIsBlocked[2]))
    checkButton(&button3Handler);
  
  checkButton(&buttonEncHandler);
  checkButton(&buttonJoyHandler);   
}

void buttonHandlersInit()
{
    button1Handler.lastState = GPIO_PIN_SET;
    button2Handler.lastState = GPIO_PIN_SET;
    button3Handler.lastState = GPIO_PIN_SET;
    buttonEncHandler.lastState = GPIO_PIN_SET;
    buttonJoyHandler.lastState = GPIO_PIN_SET;
    
    button1Handler.longClickPendingRequest = 0;
    button2Handler.longClickPendingRequest = 0;
    button3Handler.longClickPendingRequest = 0;
    buttonEncHandler.longClickPendingRequest = 0;
    buttonJoyHandler.longClickPendingRequest = 0;
    button1Handler.shortClickPendingRequest = 0;
    button2Handler.shortClickPendingRequest = 0;
    button3Handler.shortClickPendingRequest = 0;
    buttonEncHandler.shortClickPendingRequest = 0;
    buttonJoyHandler.shortClickPendingRequest = 0;  
    
    button1Handler.savedPreviousPosition = 0;
    button2Handler.savedPreviousPosition = 0;
    button3Handler.savedPreviousPosition = 0;
 
    button1Handler.setOnceFlag = 0;
    button2Handler.setOnceFlag = 0;
    button3Handler.setOnceFlag = 0;
}


void checkButton(buttonStateHandler_t* handler)
{  
  if (!handler->state && handler->lastState) {  // just pressed
    handler->lastState = handler->state;
    handler->timeStamp = HAL_GetTick(); // start timer
  }
    
  if (handler->state && !handler->lastState) {   // just released
    if ((HAL_GetTick() - handler->timeStamp) > BTN_DBNCE_MS){
      if (((HAL_GetTick() - handler->timeStamp) <= BTN_CLICK_LONG)) { // short press
        handler->shortClickPendingRequest = 1;
        handler->savedPreviousPosition = 1;
        handler->setOnceFlag = 0;
        handler->lastState = handler->state;
      }
      if ((HAL_GetTick() - handler->timeStamp) > BTN_CLICK_LONG) { // long press
        handler->longClickPendingRequest = 1;
        handler->lastState = handler->state;
      }
    }
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {      
    case(ENC_A_Pin):

      break;
      
  default:
    break;
    
  }
}
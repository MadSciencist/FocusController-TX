/**
******************************************************************************
* File Name          : freertos.c
* Description        : Code for freertos applications
******************************************************************************
* This notice applies to any and all portions of this file
* that are not between comment pairs USER CODE BEGIN and
* USER CODE END. Other portions of this file, whether 
* inserted by the user or by software development tools
* are owned by their respective copyright owners.
*
* Copyright (c) 2018 STMicroelectronics International N.V. 
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted, provided that the following conditions are met:
*
* 1. Redistribution of source code must retain the above copyright notice, 
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. Neither the name of STMicroelectronics nor the names of other 
*    contributors to this software may be used to endorse or promote products 
*    derived from this software without specific written permission.
* 4. This software, including modifications and/or derivative works of this 
*    software, must execute solely and exclusively on microcontroller or
*    microprocessor devices manufactured by or for STMicroelectronics.
* 5. Redistribution and use of this software other than as permitted under 
*    this license is void and will automatically terminate your rights under 
*    this license. 
*
* THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
* RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
* SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "lcd.h"
#include "button.h"
#include "user.h"
#include "wireless_tasks.h"
#include "OLED_Abstract.h"
#include "eeprom_emulator.h"
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId receivingTaskHandle;
osTimerId vLPFTimerHandle;

/* USER CODE BEGIN Variables */
extern uint16_t ADC_RAW[ADC_NUM_MEASURED_CHANNELS];
uint16_t ADC_Filtered[ADC_NUM_MEASURED_CHANNELS];
user_parameters_t user;

extern buttonStateHandler_t button1Handler, button2Handler, button3Handler, buttonEncHandler, buttonJoyHandler;
volatile uint8_t blockEncoder = 0, handleEncoderDebounceRequest = 0;
extern uint8_t buttonPendingRequest[];
extern uint8_t encoderButtonCounter;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void startReceivingTask(void const * argument);
void vLPFTimerCallback(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */
  
  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */
  
  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */
  
  /* Create the timer(s) */
  /* definition and creation of vLPFTimer */
  osTimerDef(vLPFTimer, vLPFTimerCallback);
  vLPFTimerHandle = osTimerCreate(osTimer(vLPFTimer), osTimerPeriodic, NULL);
  
  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */
  
  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityHigh, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
  
  /* definition and creation of receivingTask */
  osThreadDef(receivingTask, startReceivingTask, osPriorityNormal, 0, 256);
  receivingTaskHandle = osThreadCreate(osThread(receivingTask), NULL);
  
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
  
  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  
  /* USER CODE BEGIN StartDefaultTask */
  initUserStruct(&user); //init with max ranges and zero parameters
  ReadFromFlash(&user, sizeof(user), SECTOR31_FLASH_BEGINING); //read calibration data
  osTimerStart(vLPFTimerHandle, 2U);
  buttonHandlersInit();
  /* Infinite loop */
  for(;;)
  {
    pollButtons();
    processButtonsPendingRequests();  
    writeWireless();
    OLED_DrawVoltages();
    
    /* seems to work good
    static uint32_t MessageCounter = 0;
    MessageCounter++;
    uint8_t DataToSend[32];
    uint8_t MessageLength;
    MessageLength = sprintf(DataToSend, "Wiadomosc nr %d\n\r", MessageCounter);
    CDC_Transmit_FS(DataToSend, MessageLength);
    */
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
  /* USER CODE END StartDefaultTask */
}

/* startReceivingTask function */
void startReceivingTask(void const * argument)
{
  /* USER CODE BEGIN startReceivingTask */
  /* Infinite loop */
  for(;;)
  {
    readWireless();
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
  /* USER CODE END startReceivingTask */
}

/* vLPFTimerCallback function */
void vLPFTimerCallback(void const * argument)
{
  /* USER CODE BEGIN vLPFTimerCallback */
  filterADC(ADC_RAW, ADC_Filtered);
  /* USER CODE END vLPFTimerCallback */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

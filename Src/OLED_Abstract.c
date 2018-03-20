#include "OLED_Abstract.h"

extern uint8_t isReceiverTimeout;
extern uint16_t ADC_RAW[], ADC_Filtered[];
char receiverCapacityAscii[4];
extern uint8_t encoderButtonCounter;
extern user_parameters_t user;

void OLED_DrawVoltages()
{
  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_GotoXY(24, 0);
  SSD1306_Puts("FOLLOWFOCUS", &TM_Font_7x10, SSD1306_COLOR_WHITE);
  SSD1306_GotoXY(0, 14);
  SSD1306_Puts("NADAJNIK ODBIORNIK", &TM_Font_7x10, SSD1306_COLOR_WHITE);
  
  float v = get_voltage(ADC_Filtered[0]);
  char voltageChar[3];
  sprintf((char *)voltageChar, "%3d%", (int)GetCapacity(v));
  SSD1306_GotoXY(0, 25);
  SSD1306_Puts(voltageChar, &TM_Font_11x18, SSD1306_COLOR_WHITE);
  
  SSD1306_GotoXY(82, 25);
  if(isReceiverTimeout)
    SSD1306_Puts(":-(", &TM_Font_11x18, SSD1306_COLOR_WHITE);
  else
    SSD1306_Puts(receiverCapacityAscii, &TM_Font_11x18, SSD1306_COLOR_WHITE);
  
  SSD1306_GotoXY(10, 50);
  if(encoderButtonCounter == 0){
    SSD1306_Puts("FOCUS", &TM_Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(60, 50);
      if(!user.isServoReversed[0]) 
        /* note the "!" - the calibration actually needs to be done in reverse - have to figure why, so far we are cheating the user, saying that we are not in reverse to perform calibration (while calibration display does not say reverse) */
        SSD1306_Puts("REVERSED", &TM_Font_7x10, SSD1306_COLOR_BLACK);
  }
  else if(encoderButtonCounter == 1){
    SSD1306_Puts("IRIS", &TM_Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(60, 50);
      if(!user.isServoReversed[1]) 
        SSD1306_Puts("REVERSED", &TM_Font_7x10, SSD1306_COLOR_BLACK);
  }
  else if(encoderButtonCounter == 2){
    SSD1306_Puts("ZOOM", &TM_Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(60, 50);
      if(!user.isServoReversed[2]) 
        SSD1306_Puts("REVERSED", &TM_Font_7x10, SSD1306_COLOR_BLACK);
  }
  
  
  
  SSD1306_UpdateScreen();
}

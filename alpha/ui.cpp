#include "ui.h"
#include "hal.h"
#include "fsm.h"


// Increase as UI thread uses more things
#define THREAD_STACK_SIZE 5000

#define DEBOUNCE_DELAY 50

_TS_UI TS_UI;

// Ctor
_TS_UI::_TS_UI() {}

// Static function to call instance method
void _TS_UI::staticTask(void* parameter)
{
  TS_UI.task(parameter);
}

void _TS_UI::begin()
{
  // UI thread should run really fast
  xTaskCreatePinnedToCore(
    _TS_UI::staticTask, // thread fn
    "UITask",           // identifier
    THREAD_STACK_SIZE,  // stack size
    NULL,               // parameter
    3,                  // increased priority (main loop is running at priority 1, idle is 0)
    NULL,               // handle
    1);                 // core
}

void _TS_UI::task(void* parameter)
{
  UBaseType_t stackHighWaterMark;

  bool lastStateBtnA = 0;
  bool currStateBtnA = 0;
  unsigned long lastDebounceTime = 0;
  bool reading;
  float timeSinceLastDebounce;

  bool SavePower = false;
  
  // Reduce screen brightness to minimum visibility to reduce power consumption
  TS_HAL.lcd_sleep(false);
  TS_HAL.lcd_brightness(12);

  TS_HAL.lcd_cursor(40, 0);
  TS_HAL.lcd_printf("ALPHA TEST");

  while(true)
  {
    // don't update contents if lcd is not turned on
    if(!SavePower)
    {
      TS_DateTime datetime;
      TS_HAL.rtc_get(datetime);
      
      TS_HAL.lcd_cursor(0, 15);
      // TODO: does not work with F()
      TS_HAL.lcd_printf("Date: %04d-%02d-%02d\n",     datetime.year, datetime.month, datetime.day);
      TS_HAL.lcd_printf("Time: %02d : %02d : %02d\n", datetime.hour, datetime.minute, datetime.second);
//      TS_HAL.lcd_printf("Battery: %d%%  \n", TS_HAL.power_get_batt_level(), NULL, NULL);
      if (TS_HAL.power_is_charging()) 
      {
        TS_HAL.lcd_printf("Status: Charging    \n");
      } else {
        TS_HAL.lcd_printf("Status: Not Charging\n");
      }

      if (TS_FSM.current_state == TS_PowerState::LOW_POWER) 
      {
        TS_HAL.lcd_printf("State: Low Power \n");
      } else if (TS_FSM.current_state == TS_PowerState::HIGH_POWER)
      {
        TS_HAL.lcd_printf("State: High Power\n");
      }

    }

//    stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//    Serial.print("UI: ");
//    Serial.println(stackHighWaterMark);

    if (TS_HAL.btn_a_get() == TS_ButtonState::Short) 
    {
      Serial.println("Button A pressed");
    }

    if (TS_HAL.btn_b_get() == TS_ButtonState::Short) 
    {
      Serial.println("Button B pressed");
    }

    TS_ButtonState powerButtonState = TS_HAL.btn_power_get();
    if (powerButtonState == TS_ButtonState::Short) 
    {
      Serial.println("Power button short press");  
    } else if (powerButtonState == TS_ButtonState::Long) 
    {
      Serial.println("Power button long press");
    }
    
    TS_HAL.sleep(TS_SleepMode::Task, 250);
  }
}

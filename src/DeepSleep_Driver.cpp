#include "DeepSleep_Driver.h"


RTC_DATA_ATTR float SLEEP_TIMEOUT = 0;

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void wakeUp(uint8_t measureComplete){
//++bootCount;
  //Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  //print_wakeup_reason();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
if (measureComplete)
{
  esp_sleep_enable_timer_wakeup(LIGHT_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(LIGHT_SLEEP) +
  " Seconds");
}else
{
   esp_sleep_enable_timer_wakeup(DEEP_SLEEP * uS_TO_S_FACTOR);
     Serial.println("Setup ESP32 to sleep for every " + String(DEEP_SLEEP) +
  " Seconds");
  LoRaTX_Complete = 0;

}
}

void goSleep(int sleepTime){

SLEEP_TIMEOUT = SLEEP_TIMEOUT + (millis()/1000)+LIGHT_SLEEP;
Serial.print("Sleep checker:");
Serial.println(SLEEP_TIMEOUT);

if (SLEEP_TIMEOUT >= TIMOUT)
{
  SLEEP_TIMEOUT = 0;
 MEASURE_COMPLETE = 0;
}


 esp_sleep_enable_timer_wakeup(sleepTime * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(sleepTime) +
  " Seconds");


Serial.println("Going to sleep now");
  delay(1000);

  Serial.flush(); 
  
  esp_deep_sleep_start();
}
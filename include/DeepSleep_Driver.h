#include<Arduino.h>
#include "LoRa_Driver.h"
#include "debug.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define DEEP_SLEEP  10      /* Time ESP32 will go to sleep (in seconds) */
#define DEEP_SLEEP_LORA_TEST  120      /* Time ESP32 will go to sleep (in seconds) */
#define SDI12_DEEP_SLEEP  10      /* Time ESP32 will go to sleep (in seconds) */
#define LIGHT_SLEEP 10
#define TIMOUT 300 // Device waits this time (seconds) before measuring again

extern RTC_DATA_ATTR int bootCount;


void print_wakeup_reason();
void wakeUp(uint8_t measureComplete);
void goSleep(int sleepTime);
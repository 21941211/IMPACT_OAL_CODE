#include <Arduino.h>
#include "LoRa_Driver.h"
#include "DHT22_Driver.h"
#include "SD_Driver.h"
#include "DeepSleep_Driver.h"
#include "DataProcessing_Driver.h"
#include "SoilMoisture_Driver.h"
#include "LinPotDendro_Driver.h"
#include "BatteryLevel_Driver.h"
#include <Wire.h>
#include "DS18B20_Driver.h"
#include "SDI-12_Driver.h"
#include "MeasureFlags.h"
#include "SapFlow_Driver.h"
#include "driver/rtc_io.h"

#define PULLDOWN_GPIO GPIO_NUM_5     // Only RTC IO are allowed

void setup()
{
pinMode(GPIO_NUM_5, OUTPUT);
digitalWrite(GPIO_NUM_5,LOW);

  // Slow down CPU for lower power usage
  setCpuFrequencyMhz(40);
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting");
  Serial.println("******************************************************");

  pinMode(SD_ENABLE_PIN, OUTPUT);
  pinMode(LORA_CS_PIN, OUTPUT);
  pinMode(DEBUG_LED_PIN, OUTPUT);
  pinMode(DHT22_SM_ENABLE_PIN, OUTPUT);

    SDSetup();


  if (BATTERY_LOW == 1)
  {
    measBat();
    // this will never run if the battery is low
    BATTERY_LOW = 0;
  }
#ifdef ENABLE_MEASURE
  pinMode(DHT22_SM_ENABLE_PIN, OUTPUT);
  pinMode(DENDROMETER_ENABLE_PIN, OUTPUT);
#endif

#ifdef ENABLE_SDI12
  SDI12_Setup();
  SDI12_CONNECTED = SDI12_Check();
  SDI12_Shutdown();
#endif

#if defined(ENABLE_SD) && defined(ENABLE_SDI12)
  if (SDI12_CONNECTED)
  {
    SDSetup_SDI12();
  }
#endif

#ifdef ENABLE_LORA
  if (MEASURE_COMPLETE)
  {
    LoRaSetup();
  }
#endif

#ifdef ENABLE_LORA_TEST
  {
    LoRaSetup();
  }
#endif

digitalWrite(DEBUG_LED_PIN, HIGH);

dendroSetup();
SFSetup();

digitalWrite(DEBUG_LED_PIN, LOW); // turn the LED off (HIGH is the voltage level)

// while(1){
//   testRead();
//   delay(1000);

// }
}

void loop()
{

if (DHT22_DONE && SM_DONE)
    {
      digitalWrite(DHT22_SM_ENABLE_PIN, LOW); // turn the LED on (HIGH is the voltage level)
    }
    else if (digitalRead(DHT22_SM_ENABLE_PIN) == LOW){
      digitalWrite(DHT22_SM_ENABLE_PIN, HIGH); // turn the LED on (HIGH is the voltage level))
      DHTSetup();
      Serial.println("DHT22, SM and SF now ON");
      
    }

  if (!MEASURE_COMPLETE)
  {

    if(!BATT_DONE) measBat();
    if (!DHT22_DONE)
      DHT_Measure();
    if (!SM_DONE)
      SM_Measure();
    if (!ST_DONE)
      readDS18B20();
    if (!DENDRO_DONE)
      Dendro_Measure();
      if(!SF_DONE)
     SF_Measure();

    


if(DENDRO_DONE&&SF_DONE)
digitalWrite(DENDROMETER_ENABLE_PIN, LOW);

 if (SDI12_CONNECTED)
  {
    SDI12_Measure(SDI12_SOIL_MOISTURE);
    SDI12_Measure(SDI12_TEMPERATURE);
    SDI12_Measure(SDI12_VOLTAGE);
    
  }

if (BATT_DONE && SM_DONE && ST_DONE && DENDRO_DONE && SF_DONE){
      Serial.println("All Measurements Done");
      Serial.println("******************************************************");
      
      MEASURE_COMPLETE = 1;
      }

  }
  else
  {
Serial.println("Writing to SD now...");

while(1);

#ifdef ENABLE_SD
    writeToSD();
    writeToSD_SDI12();
#endif
    LoRaSetup();

#ifdef ENABLE_LORA
 
    os_runloop_once();
#endif

  }
 
}

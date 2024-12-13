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

void setup()
{
  // Slow down CPU for lower power usage
  setCpuFrequencyMhz(40);
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting");


  pinMode(SD_ENABLE_PIN, OUTPUT);
   pinMode(LORA_CS_PIN, OUTPUT);
  SDSetup();



if(BATTERY_LOW == 1){
  measBat();
//this will never run if the battery is low
  BATTERY_LOW = 0;
}


  pinMode(DEBUG_LED_PIN, OUTPUT);

  digitalWrite(DEBUG_LED_PIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(2000);
  digitalWrite(DEBUG_LED_PIN, LOW); // turn the LED off (HIGH is the voltage level)

#ifdef ENABLE_MEASURE
  pinMode(DHT22_SM_ENABLE_PIN, OUTPUT);
  pinMode(DENDROMETER_ENABLE_PIN, OUTPUT);
#endif


#ifdef ENABLE_DENDRO_TEST

  pinMode(DEBUG_LED_PIN, OUTPUT);
  pinMode(LORA_CS_PIN, OUTPUT);

  digitalWrite(DEBUG_LED_PIN, HIGH); // turn the LED on (HIGH is the voltage level)




while(1){
  
      Dendro_Measure(); // Call the measurement function
      readDS18B20();
      SDSetup();
      writeToSD();
    }

#endif

#ifdef ENABLE_SDI12
SDI12_Setup();
 SDI12_CONNECTED = SDI12_Check();
 delay(1000);
#endif


#ifdef ENABLE_SD
  pinMode(SD_ENABLE_PIN, OUTPUT);
  pinMode(DEBUG_LED_PIN, OUTPUT);
  pinMode(LORA_CS_PIN, OUTPUT);

  digitalWrite(DEBUG_LED_PIN, HIGH); // turn the LED on (HIGH is the voltage level)

  SDSetup();

if (SDI12_CONNECTED)
{
SDSetup_SDI12();
}


  digitalWrite(DEBUG_LED_PIN, LOW); // turn the LED off (HIGH is the voltage level)
  digitalWrite(LORA_CS_PIN, LOW);   // Enable LoRa

  Serial.println("SD Setup complete");
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


}





void loop()
{
  if (!MEASURE_COMPLETE)
  {
#ifdef ENABLE_MEASURE
    measBat();

    digitalWrite(DHT22_SM_ENABLE_PIN, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(1000);


    DHTSetup();
    DHT_Measure();
    SM_Measure();
    

    digitalWrite(DHT22_SM_ENABLE_PIN, LOW); // turn the LED on (HIGH is the voltage level)
    Dendro_Measure();
    readDS18B20();

if(SDI12_CONNECTED){
  SDI12_Measure(SDI12_SOIL_MOISTURE);
  SDI12_Measure(SDI12_TEMPERATURE);
  SDI12_Measure(SDI12_VOLTAGE);
  writeToSD_SDI12();
}

#endif

    MEASURE_COMPLETE = 1;
#ifdef ENABLE_SD
    writeToSD();
#endif

    LoRaSetup();
  }

#ifdef ENABLE_LORA
  os_runloop_once();
#endif

#ifdef ENABLE_LORA_TEST
  os_runloop_once();
#endif
}

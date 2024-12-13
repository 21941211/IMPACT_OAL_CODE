
#include "DHT.h"
#include <debug.h>
#include <pinmapping.h>
#include <Arduino.h>
#include "LoRa_Driver.h"


void DHTSetup();
void DHT_Measure();


extern float tempMedian;
extern float humMedian;


extern RTC_DATA_ATTR uint8_t MEASURE_COMPLETE;
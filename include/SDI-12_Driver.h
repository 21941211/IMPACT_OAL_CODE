#include "Arduino.h"
#include "pinmapping.h"
#include "DataProcessing_Driver.h"
#include "debug.h"

#define SDI12_SOIL_MOISTURE 0
#define SDI12_SALINITY 1
#define SDI12_TEMPERATURE 2
#define SDI12_VOLTAGE 3

uint8_t SDI12_Measure(uint8_t measurement);
uint8_t SDI12_Check();
void SDI12_Setup();
void SDI12_TX_RX(String &buffer,String command, int t);
String SDI12_Measurements_To_String();
void SDI12_Shutdown();
void SDI12_TX(String command);
void SDI12_RX(String &buffer);


extern double SDI12_SM[6];
extern double SDI12_Temp[6];
extern double SDI12_Salinity[6];
extern double SDI12_SupplyVoltage[1];

extern uint8_t SDI12_CONNECTED;

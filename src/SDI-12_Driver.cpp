#include "SDI-12_Driver.h"
#include <SDI12.h>


RTC_DATA_ATTR uint8_t SDI12_CONNECTED = 0;


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

struct StringMeasurements {
String Address; 
  String soilMoisture;
  String Salinity;
  String Temperature;
  String Voltage;
};

struct MetaData {
  String soilMoisture;
  String Salinity;
  String Temperature;
  String Voltage;
};



/** Define the SDI-12 bus */
SDI12 mySDI12(SDI12_DATA_PIN);

String myCommand = "JI!";

StringMeasurements stringMeasurements; // Declare an instance of the struct to hold the measurements

MetaData measurementsMeta;

double SDI12_SM[6];
double SDI12_Temp[6];
double SDI12_Salinity[6];
double SDI12_SupplyVoltage[1];

void SDI12_Setup()
{
pinMode(SDI12_EN_PIN, OUTPUT);
digitalWrite(SDI12_EN_PIN,HIGH);
delay(1000);

  Serial.println("Opening SDI-12 bus...");
  mySDI12.begin();
  // allow things to settle
  delay(500);
}

uint8_t SDI12_Check()
{

  stringMeasurements.Address = "";

  SDI12_TX_RX(stringMeasurements.Address, "?!", 300);
  if (stringMeasurements.Address[0] == 0)
  {
    Serial.println("SDI-12 Device not connected");
    Serial.print("Device Address: ");
    Serial.println(stringMeasurements.Address[0]);
    Serial.println("");
    return 0;
  }
  else
  {
    Serial.print("Device Address: ");
    Serial.println(stringMeasurements.Address[0]);
    Serial.println("");
  }

  return 1;
}

void SDI12_Measure(uint8_t measurement)
{

  String commands[] = {"C!", "C1!", "C2!", "C9!", "D0!", "D1!"};
  String measurementNames[] = {"Soil Moisture", "Salinity", "Temperature", "Supply Voltage"};
  String metaData = "";
  String receivedData = ""; // Initialize an empty string for received data

  Serial.println(measurementNames[measurement]);

  SDI12_TX_RX(metaData, stringMeasurements.Address[0] + commands[measurement], 300);
  delay(2000);
  SDI12_TX_RX(receivedData, stringMeasurements.Address[0] + commands[4], 300);
  delay(2000);

  if (measurement != SDI12_VOLTAGE)
  {
    delay(2000);
    SDI12_TX_RX(receivedData, stringMeasurements.Address[0] + commands[5], 300);
  }

  // Append the received data to the relevant measurement string
  switch (measurement)
  {
  case SDI12_SOIL_MOISTURE:
    measurementsMeta.soilMoisture = metaData;
    stringMeasurements.soilMoisture = receivedData;
    extractValuesFromString(stringMeasurements.soilMoisture, SDI12_SM, 6);
    Serial.println(stringMeasurements.soilMoisture);
  

    // Print the extracted values
    for (size_t i = 0; i < 6; ++i)
    {
      Serial.print("Value ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(SDI12_SM[i],6);
    }

    Serial.println("");
    break;
  case SDI12_SALINITY:
    measurementsMeta.Salinity = metaData;
    stringMeasurements.Salinity = receivedData;
    extractValuesFromString(stringMeasurements.Salinity, SDI12_Salinity, 6);
    Serial.println(stringMeasurements.Salinity);
    

    // Print the extracted values
    for (size_t i = 0; i < 6; ++i)
    {
      Serial.print("Value ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(SDI12_Salinity[i]);
    }
    Serial.println("");
    break;
  case SDI12_TEMPERATURE:
    measurementsMeta.Temperature = metaData;
    stringMeasurements.Temperature = receivedData;
    extractValuesFromString(stringMeasurements.Temperature, SDI12_Temp, 6);
    Serial.println(stringMeasurements.Temperature);
 

    // Print the extracted values
    for (size_t i = 0; i < 6; ++i)
    {
      Serial.print("Value ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(SDI12_Temp[i],5);
    }
    Serial.println("");

    break;
  case SDI12_VOLTAGE:
    measurementsMeta.Voltage = metaData;
    stringMeasurements.Voltage = receivedData;
    extractValuesFromString(stringMeasurements.Voltage, SDI12_SupplyVoltage, 1);
    Serial.println(stringMeasurements.Voltage);
    

    // Print the extracted values
    for (size_t i = 0; i < 1; ++i)
    {
      Serial.print("Value ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(SDI12_SupplyVoltage[i],6);
    }
    Serial.println("");

    break;
  default:
    break;
  }
}

void SDI12_TX(String command)
{
  mySDI12.sendCommand(command);
}

void SDI12_RX(String &buffer)
{

  while (mySDI12.available())
  {
    char c = mySDI12.read();
    // Serial.write(c); // write the response to the screen
    buffer += c; // Append the character to the receivedData string
  }
}

void SDI12_TX_RX(String &buffer, String command, int t)
{
  SDI12_TX(command);
  delay(t);
  SDI12_RX(buffer);
}

String SDI12_Measurements_To_String()
{

  String measurements = "";
  for (size_t i = 0; i < 6; ++i)
  {
    measurements += String(SDI12_SM[i], 6);
    measurements += ",";
  }

  for (size_t i = 0; i < 6; ++i)
  {
    measurements += String(SDI12_Temp[i], 5);
    measurements += ",";
  }

  for (size_t i = 0; i < 1; ++i)
  {
    measurements += String(SDI12_SupplyVoltage[i], 6);
    measurements += ",";
  }

  measurements = measurements.substring(0, measurements.length() - 1); // Remove the last comma

  return measurements;
  for (size_t i = 0; i < 6; ++i)
  {
    measurements += String(SDI12_SM[i], 6);
    measurements += " ";
  }

  measurements += "\nTemperature: ";
  for (size_t i = 0; i < 6; ++i)
  {
    measurements += String(SDI12_Temp[i], 5);
    measurements += " ";
  }

  measurements += "\nVoltage: ";
  for (size_t i = 0; i < 1; ++i)
  {
    measurements += String(SDI12_SupplyVoltage[i], 6);
    measurements += " ";
  }

  return measurements;
}
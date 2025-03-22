#include "SDI-12_Driver.h"
#include <SDI12.h>
#include "driver/rtc_io.h"
#include "DeepSleep_Driver.h"

 uint8_t SDI12_CONNECTED = 0;

unsigned long currentMillis_SDI12;
unsigned long previousMillis_SDI12 = 0;

uint8_t MeasurementRequest1 = 0;
uint8_t MeasurementRequest2 = 0;
uint8_t MeasurementRequest3 = 0;

unsigned long timeSinceRequest = 0;
unsigned long requestTime = 0;
unsigned long metaDataRecievedTime = 0;
unsigned long timeSinceMetaDataReceived = 0;
unsigned long timeSinceMetaDataReceivedTime = 0;

uint8_t SM_MEASUREMENT_REQUESTED = 0;
uint8_t SM_DATA_REQUESTED = 0;
uint8_t ST_MEASUREMENT_REQUESTED = 0;
uint8_t ST_DATA_REQUESTED = 0;
uint8_t SDI12_SM_DONE = 0;
uint8_t SDI12_ST_DONE = 0;
uint8_t LAST_REQUEST;
uint8_t METADATA_RECEIVED = 0;

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define MEASUREMENT 0
#define DATA 1

struct StringMeasurements
{
  String Address;
  String soilMoisture;
  String Salinity;
  String Temperature;
  String Voltage;
};

struct MetaData
{
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

int SDI12_90cm_Delay = 2000;
int SDI112_120cm_Delay = 3000;

void SDI12_Setup()
{



  pinMode(SDI12_EN_PIN, OUTPUT);
  digitalWrite(SDI12_EN_PIN, HIGH);
  delay(500);

  rtc_gpio_hold_dis(GPIO_NUM_5);
  gpio_reset_pin(GPIO_NUM_5);


  Serial.println("SDI12 is now ON");
   Serial.println("******************************************************");
  Serial.println("Opening SDI-12 bus...");
  mySDI12.begin(GPIO_NUM_5);

  int datPin = mySDI12.getDataPin();
Serial.print("Data pin: ");
Serial.println(datPin);
  delay(600); // DO NOT CHANGE OR REMOVE!
}

uint8_t SDI12_Check()
{

  //mySDI12.begin();
  //delay(500); // DO NOT CHANGE OR REMOVE!
  stringMeasurements.Address = "";

  SDI12_TX_RX(stringMeasurements.Address, "?!", 300);
  if (stringMeasurements.Address[0] == 0)
  {
    Serial.println("SDI-12 Device not connected");
    Serial.println("******************************************************");
    return 0;
  }
  else
  {
    Serial.println("SDI-12 Device connected");
    Serial.print("Device Address: ");
    Serial.println(stringMeasurements.Address[0]);
   // mySDI12.clearBuffer();
  // mySDI12.end();
    Serial.println("******************************************************");
  }

  return 1;
}

uint8_t SDI12_Measure(uint8_t measurement)
{

  String commands[] = {"C!", "C1!", "C2!", "C9!", "D0!", "D1!", "M!"};
  String measurementNames[] = {"Soil Moisture", "Salinity", "Temperature", "Supply Voltage"};
  String metaData = "";
  String receivedData = ""; // Initialize an empty string for received data

  if (!SM_MEASUREMENT_REQUESTED)
  {
    Serial.println("Requesting SDI-12 " + measurementNames[measurement] + " Measurement");
     Serial.println("******************************************************");
    SDI12_TX(stringMeasurements.Address[0] + commands[measurement]);
    SM_MEASUREMENT_REQUESTED = 1;
    LAST_REQUEST = MEASUREMENT;
    requestTime = millis();
    timeSinceRequest = 0;
    return 0;
  }

  if (!METADATA_RECEIVED)
  {
    if ((LAST_REQUEST == MEASUREMENT) && (timeSinceRequest >= 100))
    {
      switch (measurement)
      {
      case SDI12_SOIL_MOISTURE:
        SDI12_RX(measurementsMeta.soilMoisture);
        break;
      case SDI12_TEMPERATURE:
        SDI12_RX(measurementsMeta.Temperature);
        break;
      default:
        break;
      }

      Serial.println("");
      Serial.println("Metadata received");
       Serial.println("******************************************************");
      METADATA_RECEIVED = 1;
      timeSinceMetaDataReceivedTime = 0;
      metaDataRecievedTime = millis();
      return 0;
    }
    else
    {

      timeSinceRequest = millis() - requestTime;
      return 0;
    }
  }

  if (METADATA_RECEIVED)
  {
    if (!SM_DATA_REQUESTED && timeSinceMetaDataReceived >= 1000)
    {

      Serial.println("Requesting SDI-12" + measurementNames[measurement] + " Data");
       Serial.println("******************************************************");
      SDI12_TX(stringMeasurements.Address[0] + commands[4]);
      SM_DATA_REQUESTED = 1;
      LAST_REQUEST = DATA;
      requestTime = millis();
      timeSinceRequest = 0;

      return 0;
    }
    else if (!SM_DATA_REQUESTED)
    {
      timeSinceMetaDataReceived = millis() - metaDataRecievedTime;
      return 0;
    }

    if ((LAST_REQUEST == DATA) && (timeSinceRequest >= 1000))
    {
      // Serial.print("Available data: ");
      // Serial.println(mySDI12.available());
      // Serial.println("Data available!");
    
      switch (measurement)
      {
      case SDI12_SOIL_MOISTURE:
        SDI12_RX(stringMeasurements.soilMoisture);
        extractValuesFromString(stringMeasurements.soilMoisture, SDI12_SM, 6);
       

        // Print the extracted values
        Serial.println("Extracted SDI-12 Soil Moisture Values:");

         Serial.println(stringMeasurements.soilMoisture);
        for (size_t i = 0; i < 6; ++i)
        {
          Serial.print("Value ");
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.println(SDI12_SM[i], 6);
        }
 Serial.println("******************************************************");
        break;
      case SDI12_TEMPERATURE:
        SDI12_RX(stringMeasurements.Temperature);
        extractValuesFromString(stringMeasurements.Temperature, SDI12_Temp, 6);
       

 Serial.println("Extracted SDI-12 Soil Temperature Values:");

  Serial.println(stringMeasurements.Temperature);
        // Print the extracted values
        for (size_t i = 0; i < 6; ++i)
        {
          Serial.print("Value ");
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.println(SDI12_Temp[i], 5);
        }
        Serial.println("******************************************************");
        break;
      default:
        break;
      }

      SM_DATA_REQUESTED = 0;
      SM_MEASUREMENT_REQUESTED = 0;
      METADATA_RECEIVED = 0;
      timeSinceMetaDataReceived = 0;
      timeSinceMetaDataReceivedTime = 0;
      return 1;
    }
    else
    {

      // Serial.println("Waiting for data");
      timeSinceRequest = millis() - requestTime;
      return 0;
    }
  }
  else
  {
    return 0;
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
    buffer += c;     // Append the character to the receivedData string
  }
  Serial.println("");
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
  char buffer[8]; // Buffer to hold formatted values

  for (size_t i = 0; i < 6; ++i)
  {
    sprintf(buffer, "%07.2f", SDI12_SM[i]); // Ensure at least 6 characters with 2 decimal places
    measurements += String(buffer);
    measurements += ",";
  }

  for (size_t i = 0; i < 6; ++i)
  {
    sprintf(buffer, "%07.2f", SDI12_Temp[i]);
    measurements += String(buffer);
    measurements += ",";
  }

  // Remove the last comma
  measurements = measurements.substring(0, measurements.length() - 1);

  return measurements;
}

void SDI12_Shutdown()
{

mySDI12.end();
Serial.println("Closing SDI-12 bus...");

  rtc_gpio_hold_dis(GPIO_NUM_5);
  pinMode(GPIO_NUM_5, OUTPUT);
  digitalWrite(GPIO_NUM_5, LOW);
  digitalWrite(SDI12_EN_PIN, LOW);
  rtc_gpio_hold_en(GPIO_NUM_5); // Lock GPIO 5 state

  Serial.println("SDI12 is now OFF");

  Serial.println("******************************************************");
  // delay(5000);

  // Serial.println("Data pulled low, going to sleep now:");

  // goSleep(LIGHT_SLEEP);
}

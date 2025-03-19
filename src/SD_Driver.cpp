#include "SD_Driver.h"
#include <vector>
#include <algorithm>

const byte key[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};




// Device ID in HEX
String deviceIDHex;

uint8_t endOfFirstPayload = 0;
uint8_t endOfFirstLoRaPayload = 0;
byte dataRead;
uint8_t dataBuffer[];
std::vector<uint8_t> LoRaBuffer;
std::vector<uint8_t> PayLoadTest;
uint8_t LoRaBuffer_SDI12[13] = {0};

// Converts NODE_NUMBER to a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#ifdef ENABLE_DENDRO_TEST
#define FILE_NAME "/Measurements.csv"
#else
#define FILE_NAME "/Measurements.csv"
#endif
#define FILE_NAME_SDI12 "/SDI_12_Measurements.csv"
#define SYSPARAMS "/parameters.txt"


 const char *fileName = FILE_NAME;
const char *fileName_SDI12 = FILE_NAME_SDI12;

char *paramFile = SYSPARAMS;

void reverseByteOrder(u1_t arr[8]) {
    std::reverse(arr, arr + 8);
}

void parseHexArray(String hexString, uint8_t *outputArray, int length) {
  Serial.print("Parsing hex string: ");
  Serial.println(hexString);

  hexString.trim(); // Remove extra spaces

  if (hexString.length() != length * 2) {
    Serial.println("Error: Hex string length mismatch!");
    return;
  }

  for (int i = 0; i < length; i++) {
    outputArray[i] = strtol(hexString.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
  }

}




bool readParametersFromFile(const char *path) {
  File file = SD.open(path);
  if (!file) {
    Serial.println("Failed to open file.");
    return false;
  }

Serial.println("File opened");

 while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();

     if (line.startsWith("APPEUI=")) {
      
      parseHexArray(line.substring(7), APPEUI, 8);
    } else if (line.startsWith("DEVEUI=")) {
      parseHexArray(line.substring(7), DEVEUI, 8);
    } else if (line.startsWith("APPKEY=")) {
      parseHexArray(line.substring(7), APPKEY, 16);
    } else if (line.startsWith("DEVID=")) {
      String deviceIDStr = line.substring(6); // Extract value after "DEVID="
    deviceIDStr.trim(); // Remove any whitespace or newline characters
    Serial.print("Device ID (DEC): ");
    Serial.println(deviceIDStr);
    if (deviceIDStr.length() == 0 || deviceIDStr.toInt() <= 0 || deviceIDStr.toInt() > 255) {
      Serial.println("Error: Invalid or missing DEVID. Using default ID 1.");
      deviceIDStr = "1"; // Default value
    }
    int deviceID = deviceIDStr.toInt();
    deviceIDHex = String(deviceID, HEX);
    if (deviceIDHex.length() < 2) {
      deviceIDHex = "0" + deviceIDHex; // Add leading zero if necessary
    }
    deviceIDHex.toUpperCase();
    Serial.print("Device ID (HEX): ");
    Serial.println(deviceIDHex);
  }
 }

  
  
  file.close();



reverseByteOrder(DEVEUI);

  return true;
}


void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char *path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path))
  {
    Serial.println("Dir created");
  }
  else
  {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path))
  {
    Serial.println("Dir removed");
  }
  else
  {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path, uint8_t NormalOrSDI12)
{

  uint8_t newLineFound = 0;
  uint8_t newLinePos = 0;

  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Read from file: ");
  file.seek(file.size());

  while (!newLineFound)
  {
    file.seek(file.size());
    file.seek(file.position() - newLinePos);
    dataRead = file.read();
    newLinePos++;
    // Serial.println(newLinePos);
    if (dataRead == '\n')
    {
      newLineFound++;
    }
  }

  file.seek(file.size());
  file.seek(file.position() - newLinePos + 2);

  uint8_t index = 0;
  if (NormalOrSDI12 == 1)
  {
    index = LoRaBuffer.size();
    endOfFirstLoRaPayload = index;
  }

  while (file.available())
  {
    int temp;
    int currentSize = LoRaBuffer.size();

    temp = file.read() - 48;
    if (temp >= 0)
    {
      LoRaBuffer.resize(currentSize + 1);
      LoRaBuffer[index] = temp;
      index++;
    }
  }

  Serial.println("");

  Serial.print("LoRa buffer size: ");
  Serial.println(LoRaBuffer.size());

  Serial.print("LoRa buffer: ");

  for (uint8_t i = 0; i < LoRaBuffer.size(); i++)
  {
    Serial.print(LoRaBuffer[i]);
  }

  Serial.println("");
  file.close();

  if (NormalOrSDI12 == 0)
  {
    int payLoadIndex = 0;


int counter = 0;
   while(counter < LoRaBuffer.size()-1){
      Serial.println(counter);
        PayLoadTest.resize(PayLoadTest.size() + 1);
        PayLoadTest[payLoadIndex] = LoRaBuffer[counter]*10+LoRaBuffer[counter+1];
        counter= counter+2;
        payLoadIndex++;
    }
  }
else {

    endOfFirstPayload = PayLoadTest.size();
    Serial.print("End of first payload: ");
    Serial.println(endOfFirstPayload);
    Serial.print("End of first LoRa payload: ");
    Serial.println(endOfFirstLoRaPayload);

int counter = endOfFirstLoRaPayload;
int payLoadIndex = endOfFirstPayload;
      while(counter < LoRaBuffer.size()-1){
      Serial.println(counter);
        PayLoadTest.resize(PayLoadTest.size() + 1);
        PayLoadTest[payLoadIndex] = LoRaBuffer[counter];
        //PayLoadTest[payLoadIndex] = LoRaBuffer[counter]*10+LoRaBuffer[counter+1];
        // counter= counter+2;
        counter++;
        payLoadIndex++;
    }
  }

  Serial.print("PayLoadTest size: ");
  Serial.println(PayLoadTest.size());

  Serial.print("Payload: ");
  for (int i = 0; i < PayLoadTest.size(); i++)
  {
    Serial.print(PayLoadTest[i]);
  }
  Serial.println("");
}


void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    Serial.println("Message appended");
  }
  else
  {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    Serial.println("File renamed");
  }
  else
  {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path))
  {
    Serial.println("File deleted");
  }
  else
  {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char *path)
{
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file)
  {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len)
    {
      size_t toRead = len;
      if (toRead > 512)
      {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  }
  else
  {
    Serial.println("Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++)
  {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

void SDSetup()
{
  enableSD_ON();
  setSPI(SD_SPI);

int LEDFlash = 0;
  while(!SD.begin(SD_CS_PIN, SPI, 80000000)&&LEDFlash<5)
  {
    Serial.println("Card Mount Failed");

    for (size_t i = 0; i < 2; i++)
    {
    digitalWrite( DEBUG_LED_PIN, HIGH);
    delay(20);
    digitalWrite(DEBUG_LED_PIN,LOW);
    delay(20);
    }
    LEDFlash++;
    delay(1000);
    
  }
if (LEDFlash ==5)
{
  Serial.println("SD connect timeout reached");
  Serial.println("Going to sleep for 10 seconds to reset");
  goSleep(LIGHT_SLEEP);
}


  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

Serial.println("SD Card mounted successfully");
Serial.println("The following files are available:");
listDir(SD, "/", 0);

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

    // Read parameters from file
Serial.println("Reading loRa keys from file");
  if (!readParametersFromFile(paramFile)) {
    Serial.println("Failed to read parameters from file.");
    while (1);
  }


 
  if (!SD.exists(fileName))
  {
    writeFile(SD, fileName, "Dendrometer,Air Temperature, Air Humidity,Soil temperature,Soil Water Volume cm^3/cm^3,Battery,SF T1 Before,SF T2 Before,SF T1 During,SF T2 During,SF T1 After,SF T2 After,HPV,Boot count\n");
  }

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));


   Serial.println("SD Setup complete");
Serial.println("******************************************************");
}

void writeToSD()
{

  uint8_t buffSize = 8;

  char temp_buffer[buffSize];
  char hum_buffer[buffSize];
  char SM_buffer[buffSize];
  char dendro_buffer[buffSize];
  char batt_buffer[buffSize];
  char DS18B20_buffer[buffSize];
  char SF_avgT1Before[buffSize];
  char SF_avgT2Before[buffSize];
  char SF_avgT1During[buffSize];
  char SF_avgT2During[buffSize];
  char SF_avgT1After[buffSize];
  char SF_avgT2After[buffSize];
  char SF_HPV[buffSize];
  char bootCountBuff[buffSize];



  dataToBuff(temp_buffer, tempMedian, buffSize);
  dataToBuff(hum_buffer, humMedian, buffSize);
  dataToBuff(SM_buffer, SM_Vol, buffSize);
  dataToBuff(dendro_buffer, microns, buffSize);
  dataToBuff(DS18B20_buffer, DS18B20median, buffSize);
  dataToBuff(batt_buffer, batPercentage, buffSize);
  dataToBuff(SF_avgT1Before, avgT1Before, buffSize);
  dataToBuff(SF_avgT2Before, avgT2Before, buffSize);
  dataToBuff(SF_avgT1During, avgT1During, buffSize);
  dataToBuff(SF_avgT2During, avgT2During, buffSize);
  dataToBuff(SF_avgT1After, avgT1After, buffSize);
  dataToBuff(SF_avgT2After, avgT2After, buffSize);
  dataToBuff(SF_HPV, HPV, buffSize);
  dataToBuff(bootCountBuff, float(bootCount)/100.0, buffSize);


  SDSetup();

  appendFile(SD, fileName, "\n");
  appendFile(SD, fileName, dendro_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, temp_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, hum_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, DS18B20_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, SM_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, batt_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, SF_avgT1Before);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, SF_avgT2Before);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, SF_avgT1During);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, SF_avgT2During);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, SF_avgT1After);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, SF_avgT2After);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, SF_HPV);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, bootCountBuff);
   appendFile(SD, fileName, ",");
  appendFile(SD, fileName, ":");

  enableSD_OFF();
}

void readLastEntry()
{
  #ifdef ENABLE_SD
  SDSetup();

  readFile(SD, fileName, 0);


  if (SDI12_CONNECTED == 1)
  {
    readFile(SD, fileName_SDI12, 1);
  }
#endif
  enableSD_OFF();
}

void enableSD_ON()
{

  digitalWrite(SD_ENABLE_PIN, LOW);
delay(500);
  digitalWrite(SD_ENABLE_PIN, HIGH);
  digitalWrite(LORA_CS_PIN, HIGH); // SET LoRa CS pin HIGH
  delay(1000);
}

void enableSD_OFF()
{
  digitalWrite(SD_ENABLE_PIN, LOW);
  digitalWrite(LORA_CS_PIN, LOW); // turn the LED on (HIGH is the voltage level)
  delay(100);
}

void SDSetup_SDI12()
{
  enableSD_ON();
  setSPI(SD_SPI);

  if (!SD.begin(SD_CS_PIN, SPI, 80000000))
  {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  if (!SD.exists(fileName_SDI12))
  {
    writeFile(SD, fileName_SDI12, "SM1,SM2,SM3,SM4,SM5,SM6,Temp1,Temp2,Temp3,Temp4,Temp5,Temp6,Voltage (V)\n");
  }

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

  Serial.println("******************************************************");
}

void writeToSD_SDI12()
{
  uint8_t buffSize = 8;

  char SM1_buffer[buffSize];
  char SM2_buffer[buffSize];
  char SM3_buffer[buffSize];
  char SM4_buffer[buffSize];
  char SM5_buffer[buffSize];
  char SM6_buffer[buffSize];
  char Temp1_buffer[buffSize];
  char Temp2_buffer[buffSize];
  char Temp3_buffer[buffSize];
  char Temp4_buffer[buffSize];
  char Temp5_buffer[buffSize];
  char Temp6_buffer[buffSize];
  char Voltage_buffer[buffSize];

  char *bufferArray[] = {SM1_buffer, SM2_buffer, SM3_buffer, SM4_buffer, SM5_buffer, SM6_buffer,
                         Temp1_buffer, Temp2_buffer, Temp3_buffer, Temp4_buffer, Temp5_buffer, Temp6_buffer,
                         Voltage_buffer};

  int numBuffers = sizeof(bufferArray) / sizeof(bufferArray[0]);

  // for (int i = 0; i < numBuffers; i++)
  // {
  //   dataToBuff(bufferArray[i], SDI12_SM[i]*1000.0, buffSize);
  // }

  SDSetup_SDI12();

  String allMeasurements = SDI12_Measurements_To_String();
  Serial.println(allMeasurements);
  appendFile(SD, fileName_SDI12, "\n");
  appendFile(SD, fileName_SDI12, allMeasurements.c_str());
  appendFile(SD, fileName, ":");

  enableSD_OFF();
}

void parsePayload(const String &payload, uint8_t *byteArray)
{
  int byteIndex = 0; // Index to keep track of current position in byteArray

  // Iterate over the payload, skipping spaces
  for (int i = 0; i < payload.length(); i += 2)
  {
    // Extract two characters at a time
    String byteStr = payload.substring(i, i + 2);

    // Convert the string of two characters into a byte
    byteArray[byteIndex] = strtol(byteStr.c_str(), NULL, 16);

    // Increment the byteIndex
    byteIndex++;
  }
}

void decodePayload (){

float dendro ;
float temp;
float hum;
float SM;
float DS18B20;
float batt;
float SF_avgT1Before;
float SF_avgT2Before;
float SF_avgT1During;
float SF_avgT2During;
float SF_avgT1After;
float SF_avgT2After;
float SF_HPV;
int bootCountNum;


//"Dendrometer,Air Temperature, Air Humidity,Soil temperature,Soil Water Volume cm^3/cm^3,Battery,SF T1 Before,SF T2 Before,SF T1 During,SF T2 During,SF T1 After,SF T2 After,HPV\n");
  

dendro = PayLoadTest[0]*100.0 + PayLoadTest[1] + PayLoadTest[2]*0.01;
temp = PayLoadTest[3]*100.0 + PayLoadTest[4] + PayLoadTest[5]*0.01;
hum = PayLoadTest[6]*100.0 + PayLoadTest[7] + PayLoadTest[8]*0.01;
DS18B20 = PayLoadTest[9]*100.0 + PayLoadTest[10] + PayLoadTest[11]*0.01;
SM = PayLoadTest[12]*100.0 + PayLoadTest[13] + PayLoadTest[14]*0.01;
batt = PayLoadTest[15]*100.0 + PayLoadTest[16] + PayLoadTest[17]*0.01;
SF_avgT1Before = PayLoadTest[18]*100.0 + PayLoadTest[19] + PayLoadTest[20]*0.01;
SF_avgT2Before = PayLoadTest[21]*100.0 + PayLoadTest[22] + PayLoadTest[23]*0.01;
SF_avgT1During = PayLoadTest[24]*100.0 + PayLoadTest[25] + PayLoadTest[26]*0.01;
SF_avgT2During = PayLoadTest[27]*100.0 + PayLoadTest[28] + PayLoadTest[29]*0.01;
SF_avgT1After = PayLoadTest[30]*100.0 + PayLoadTest[31] + PayLoadTest[32]*0.01;
SF_avgT2After = PayLoadTest[33]*100.0 + PayLoadTest[34] + PayLoadTest[35]*0.01;
SF_HPV = PayLoadTest[36]*100.0 + PayLoadTest[37] + PayLoadTest[38]*0.01;
bootCountNum = (PayLoadTest[39]*100.0 + PayLoadTest[40] + PayLoadTest[41]*0.01)*100;

Serial.print("Dendrometer: ");
Serial.println(dendro);
Serial.print("Temperature: ");
Serial.println(temp);
Serial.print("Humidity: ");
Serial.println(hum);
Serial.print("Soil Moisture: ");
Serial.println(SM);
Serial.print("DS18B20: ");
Serial.println(DS18B20);
Serial.print("Battery: ");
Serial.println(batt);
Serial.print("SF T1 Before: ");
Serial.println(SF_avgT1Before);
Serial.print("SF T2 Before: ");
Serial.println(SF_avgT2Before);
Serial.print("SF T1 During: ");
Serial.println(SF_avgT1During);
Serial.print("SF T2 During: ");
Serial.println(SF_avgT2During);
Serial.print("SF T1 After: ");
Serial.println(SF_avgT1After);
Serial.print("SF T2 After: ");
Serial.println(SF_avgT2After);
Serial.print("HPV: ");
Serial.println(SF_HPV);
Serial.print("Boot count: ");
Serial.println(bootCountNum);
}
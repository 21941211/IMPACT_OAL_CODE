#include "SD_Driver.h"
#include <vector>

const byte key[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};


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
#define FILE_NAME "/GWD" TOSTRING(NODE_NUMBER) "TemperatureTest.csv"
#else
#define FILE_NAME "/GWD" TOSTRING(NODE_NUMBER) ".csv"
#endif
#define FILE_NAME_SDI12 "/GWD" TOSTRING(NODE_NUMBER) "_SDI12.csv"

const char *fileName = FILE_NAME;
const char *fileName_SDI12 = FILE_NAME_SDI12;

const char* LoRaFileName = "/parameters.txt";
// SPIClass spi = SPIClass(VSPI);


void parseHexArray(String hexString, u1_t* outputArray, int length) {
  // Debugging: Print the input string
  Serial.print("Parsing hex string: ");
  Serial.println(hexString);

  hexString.trim(); // Ensure no extra spaces
  if (hexString.length() != length * 2) {
    Serial.println("Error: Hex string length mismatch!");
    return;
  }

  for (int i = 0; i < length; i++) {
    String byteString = hexString.substring(i * 2, i * 2 + 2);
    outputArray[i] = (u1_t) strtol(byteString.c_str(), NULL, 16);
  }
}

void printHexArray(u1_t* array, int length) {
  for (int i = 0; i < length; i++) {
    if (i > 0) Serial.print(", ");
    Serial.print("0x");
    if (array[i] < 0x10) Serial.print("0");
    Serial.print(array[i], HEX);
  }
  Serial.println();
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
    if (line.startsWith("DEVID="))
    {
      /* code */
    
     parseHexArray(line.substring(2), DEVID, 2);
     else if (line.startsWith("APPEUI=")) {
      parseHexArray(line.substring(7), APPEUI, 8);
    } else if (line.startsWith("DEVEUI=")) {
      parseHexArray(line.substring(7), DEVEUI, 8);
    } else if (line.startsWith("APPKEY=")) {
      parseHexArray(line.substring(7), APPKEY, 16);
    }
    }
  }

  file.close();

  // Verify all parameters are read


  Serial.print("APPEUI: "); printHexArray(APPEUI, 8);
  Serial.print("DEVEUI: "); printHexArray(DEVEUI, 8);
  Serial.print("APPKEY: "); printHexArray(APPKEY, 16);

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

  while(!SD.begin(SD_CS_PIN, SPI, 80000000))
  {
    Serial.println("Card Mount Failed");
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }


listDir(SD, "/", 0);

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

    // Read parameters from file
Serial.println("Reading loRa keys from file");
  if (!readParametersFromFile(LoRaFileName)) {
    Serial.println("Failed to read parameters from file.");
    while (1);
  }


  if (!SD.exists(fileName))
  {
    writeFile(SD, fileName, "Dendrometer,Air Temperature, Air Humidity,Soil temperature,Soil Water Volume cm^3/cm^3,Battery %\n");
  }

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
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

  dataToBuff(temp_buffer, tempMedian, buffSize);
  dataToBuff(hum_buffer, humMedian, buffSize);
  dataToBuff(SM_buffer, SM_Vol, buffSize);
  dataToBuff(dendro_buffer, microns, buffSize);
  dataToBuff(DS18B20_buffer, DS18B20median, buffSize);
  dataToBuff(batt_buffer, batPercentage, buffSize);

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
  appendFile(SD, fileName, ":");

  enableSD_OFF();
}

void readLastEntry()
{
  SDSetup();

  readFile(SD, fileName, 0);


  if (SDI12_CONNECTED == 1)
  {
    readFile(SD, fileName_SDI12, 1);
  }

  enableSD_OFF();
}

void enableSD_ON()
{
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
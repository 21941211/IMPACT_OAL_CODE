#include "Adafruit_MCP3421.h"

Adafruit_MCP3421 mcp;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for serial port to connect. Needed for native USB port only
  }

  // Begin can take an optional address and Wire interface
  if (!mcp.begin(0x68, &Wire)) { 
    Serial.println("Failed to find MCP3421 chip");
    while (1) {
      delay(10); // Avoid a busy-wait loop
    }
  }
  Serial.println("MCP3421 Found!");

  // Options: GAIN_1X, GAIN_2X, GAIN_4X, GAIN_8X
  mcp.setGain(GAIN_1X);
  Serial.print("Gain set to: ");
  switch (mcp.getGain()) {
    case GAIN_1X: Serial.println("1X"); break;
    case GAIN_2X: Serial.println("2X"); break;
    case GAIN_4X: Serial.println("4X"); break;
    case GAIN_8X: Serial.println("8X"); break;
  }

  // The resolution affects the sample rate (samples per second, SPS)
  // Other options: RESOLUTION_14_BIT (60 SPS), RESOLUTION_16_BIT (15 SPS), RESOLUTION_18_BIT (3.75 SPS)
  mcp.setResolution(RESOLUTION_14_BIT); // 240 SPS (12-bit)
  Serial.print("Resolution set to: ");
  switch (mcp.getResolution()) {
    case RESOLUTION_12_BIT: Serial.println("12 bits"); break;
    case RESOLUTION_14_BIT: Serial.println("14 bits"); break;
    case RESOLUTION_16_BIT: Serial.println("16 bits"); break;
    case RESOLUTION_18_BIT: Serial.println("18 bits"); break;
  }

  // Test setting and getting Mode
  mcp.setMode(MODE_CONTINUOUS); // Options: MODE_CONTINUOUS, MODE_ONE_SHOT
  Serial.print("Mode set to: ");
  switch (mcp.getMode()) {
    case MODE_CONTINUOUS: Serial.println("Continuous"); break;
    case MODE_ONE_SHOT: Serial.println("One-shot"); break;
  }
}

uint32_t lastSecond = millis(); // Store the last time we printed SPS
uint32_t sampleCount = 0; // Count how many samples were taken
    
void loop() {
    // Check if MCP3421 has completed a conversion in continuous mode
    if (mcp.isReady()) {
        int32_t adcValue = mcp.readADC(); // Read ADC value
        Serial.print("ADC reading: ");
        Serial.println(adcValue);
        sampleCount++; // Increment the sample count
    }

    uint32_t currentMillis = millis();
    if (currentMillis - lastSecond >= 1000) { // Check if a second has passed
        Serial.print("SPS (Samples Per Second): ");
        Serial.println(sampleCount);

        // Reset the counter and update the time
        sampleCount = 0;
        lastSecond = currentMillis;
    }
}

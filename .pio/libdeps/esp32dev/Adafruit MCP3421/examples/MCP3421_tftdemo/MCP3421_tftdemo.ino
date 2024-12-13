// Example for using the MCP3421 with a Feather ESP32-S2 TFT

#include <Wire.h>
#include <Adafruit_MCP3421.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> 
#include <Fonts/FreeSans24pt7b.h>

Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_MCP3421 mcp;

// Create a GFX canvas
GFXcanvas16 canvas(240, 135);

void setup() {
  Serial.begin(115200);
  //while(!Serial) delay(10);    // wait to get serial running

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(100);
      
  // Initialize MCP3421
  if (!mcp.begin()) {
    Serial.println("Failed to find MCP3421 chip");
    while (1) delay(10);
  }
  Serial.println("MCP3421 Found!");
  mcp.setMode(MODE_CONTINUOUS);
  mcp.setGain(GAIN_8X);
  mcp.setResolution(RESOLUTION_18_BIT); 

  // Initialize display
  display.init(135, 240);           
  display.setRotation(3);
  display.fillScreen(ST77XX_RED);
  canvas.setFont(&FreeSans24pt7b);
  canvas.setTextColor(ST77XX_WHITE);  
}

void loop() {
  static uint32_t sampleCount = 0;
  static int32_t adcValue = 0;

  if (!mcp.isReady()) {
    return;
  }
  adcValue = mcp.readADC();
  sampleCount++;
  Serial.print("ADC reading: ");
  Serial.println(adcValue);

  // Update canvas
  canvas.fillScreen(ST77XX_BLACK);
  canvas.setCursor(0, 50);
  canvas.println("ADC: ");
  canvas.println(adcValue);

  // Draw the canvas buffer to the display
  display.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
}

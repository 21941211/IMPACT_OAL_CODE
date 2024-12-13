#ifndef ADAFRUIT_MCP3421_H
#define ADAFRUIT_MCP3421_H

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

/**
 * @enum mcp3421_gain
 * @brief Enum for setting the gain of the MCP3421.
 */
typedef enum {
  GAIN_1X = 0b00, /**< Gain set to 1X */
  GAIN_2X = 0b01, /**< Gain set to 2X */
  GAIN_4X = 0b10, /**< Gain set to 4X */
  GAIN_8X = 0b11  /**< Gain set to 8X */
} mcp3421_gain;

/**
 * @enum mcp3421_resolution
 * @brief Enum for setting the resolution and sample rate of the MCP3421.
 */
typedef enum {
  RESOLUTION_12_BIT = 0b00, /**< Resolution set to 12-bit (240 SPS) */
  RESOLUTION_14_BIT = 0b01, /**< Resolution set to 14-bit (60 SPS) */
  RESOLUTION_16_BIT = 0b10, /**< Resolution set to 16-bit (15 SPS) */
  RESOLUTION_18_BIT = 0b11  /**< Resolution set to 18-bit (3.75 SPS) */
} mcp3421_resolution;

/**
 * @enum mcp3421_mode
 * @brief Enum for setting the operational mode of the MCP3421.
 */
typedef enum {
  MODE_CONTINUOUS = 0b1, /**< Continuous conversion mode */
  MODE_ONE_SHOT = 0b0    /**< One-shot conversion mode */
} mcp3421_mode;

/**
 * @struct mcp3421_config_t
 * @brief Struct representing the configuration register of the MCP3421.
 */
typedef struct {
  uint8_t gain : 2;       /**< Gain bits */
  uint8_t resolution : 2; /**< Resolution bits */
  uint8_t mode : 1;       /**< Mode bit */
  uint8_t unused : 2;     /**< Unused bits */
  uint8_t ready : 1;      /**< Ready bit */
} mcp3421_config_t;

/**
 * @class Adafruit_MCP3421
 * @brief Class representing the Adafruit MCP3421 ADC breakout.
 */
class Adafruit_MCP3421 {
public:
  Adafruit_MCP3421();
  boolean begin(uint8_t i2c_addr = 0x68, TwoWire *wire = &Wire);

  void setGain(mcp3421_gain gain);
  mcp3421_gain getGain();

  void setResolution(mcp3421_resolution res);
  mcp3421_resolution getResolution();

  void setMode(mcp3421_mode mode);
  mcp3421_mode getMode();

  bool isReady();
  bool startOneShotConversion();
  int32_t readADC();

private:
  Adafruit_I2CDevice *i2c_dev; /**< I2C device */
  mcp3421_config_t config;     /**< Configuration register */
  uint8_t adc_data[3];         /**< ADC data storage */

  bool readData(); /**< Reads data from the device */
};

#endif // ADAFRUIT_MCP3421_H

#include "Adafruit_MCP3421.h"

/**
 * @brief Construct a new Adafruit_MCP3421::Adafruit_MCP3421 object.
 *        Initialize the I2C device and configuration structure.
 */
Adafruit_MCP3421::Adafruit_MCP3421() : i2c_dev(nullptr) { config = {0}; }

/**
 * @brief Initialize the MCP3421 sensor.
 *
 * @param i2c_addr The I2C address of the sensor.
 * @param wire The I2C interface to use.
 * @return true if initialization is successful, false otherwise.
 */
boolean Adafruit_MCP3421::begin(uint8_t i2c_addr, TwoWire *wire) {
  i2c_dev = new Adafruit_I2CDevice(i2c_addr, wire);
  return i2c_dev->begin();
}

/**
 * @brief Set the gain of the MCP3421.
 *
 * @param gain The gain setting.
 */
void Adafruit_MCP3421::setGain(mcp3421_gain gain) {
  config.gain = gain;
  uint8_t configByte = *reinterpret_cast<uint8_t *>(&config);
  i2c_dev->write(&configByte, 1);
}

/**
 * @brief Get the current gain setting of the MCP3421.
 *
 * @return mcp3421_gain The current gain setting.
 */
mcp3421_gain Adafruit_MCP3421::getGain() {
  readData(); // Update data, ignore read failure
  return static_cast<mcp3421_gain>(config.gain);
}

/**
 * @brief Set the resolution of the MCP3421.
 *
 * @param res The resolution setting.
 */
void Adafruit_MCP3421::setResolution(mcp3421_resolution res) {
  config.resolution = res;
  uint8_t configByte = *reinterpret_cast<uint8_t *>(&config);
  i2c_dev->write(&configByte, 1);
}

/**
 * @brief Get the current resolution setting of the MCP3421.
 *
 * @return mcp3421_resolution The current resolution setting.
 */
mcp3421_resolution Adafruit_MCP3421::getResolution() {
  readData(); // Update data, ignore read failure
  return static_cast<mcp3421_resolution>(config.resolution);
}

/**
 * @brief Set the operational mode of the MCP3421.
 *
 * @param mode The operational mode.
 */
void Adafruit_MCP3421::setMode(mcp3421_mode mode) {
  config.mode = mode;
  uint8_t configByte = *reinterpret_cast<uint8_t *>(&config);
  i2c_dev->write(&configByte, 1);
}

/**
 * @brief Get the current operational mode of the MCP3421.
 *
 * @return mcp3421_mode The current operational mode.
 */
mcp3421_mode Adafruit_MCP3421::getMode() {
  readData(); // Update data, ignore read failure
  return static_cast<mcp3421_mode>(config.mode);
}

/**
 * @brief Check if the MCP3421 has completed a conversion,
 * can be used in continuous or one-shot mode
 *
 * @return true if conversion is complete, false otherwise.
 */
bool Adafruit_MCP3421::isReady() {
  if (!readData()) {
    return false; // Could not read data
  }
  return !config.ready;
}

/**
 * @brief Read and store ADC and configuration data from MCP3421.
 *
 * @return true if the read operation is successful, false otherwise.
 */
/**
 * @brief Read and store ADC and configuration data from MCP3421.
 *
 * @return true if the read operation is successful, false otherwise.
 */
bool Adafruit_MCP3421::readData() {
  uint8_t data[4]; // Buffer to read data
  if (!i2c_dev->read(data, 4)) {
    return false; // I2C read failed
  }

  // Extract ADC data
  memcpy(adc_data, data, 3);

  // Update the configuration from the fourth byte
  config = *reinterpret_cast<mcp3421_config_t *>(&data[3]);

  // For 12, 14, or 16-bit resolutions, the config byte is the third byte
  if ((config.resolution & 0b11) != RESOLUTION_18_BIT) {
    config = *reinterpret_cast<mcp3421_config_t *>(&data[2]);
  }

  return true;
}

/**
 * @brief Start a one-shot conversion on the MCP3421.
 *
 * @return true if the I2C write is successful, false otherwise.
 */
bool Adafruit_MCP3421::startOneShotConversion() {
  config.ready = 1;
  uint8_t configByte = *reinterpret_cast<uint8_t *>(&config);
  return i2c_dev->write(&configByte, 1);
}

/**
 * @brief Read the ADC value from MCP3421.
 *
 * @return int32_t The ADC value as a differential reading.
 */
int32_t Adafruit_MCP3421::readADC() {
  if (!readData()) {
    return 0; // Return 0 if read failed
  }

  int32_t adcValue = 0;
  switch (config.resolution) {
  case RESOLUTION_12_BIT:
  case RESOLUTION_14_BIT:
  case RESOLUTION_16_BIT:
    // Directly cast the first two bytes to int16_t
    adcValue = static_cast<int16_t>((adc_data[0] << 8) | adc_data[1]);
    break;
  case RESOLUTION_18_BIT:
    // Use all three bytes, considering the differential nature
    adcValue = (adc_data[0] << 16) | (adc_data[1] << 8) | adc_data[2];
    // Extend the sign if the top bit is set
    if (adc_data[0] & 0x02) {
      adcValue |= 0xFF000000;
    }
    break;
  }

  return adcValue;
}

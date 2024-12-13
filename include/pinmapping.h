//LoRa Pins
#define LORA_SCK_PIN 36
#define LORA_MISO_PIN 37
#define LORA_MOSI_PIN 35
#define LORA_CS_PIN 34
#define LORA_RST_PIN 18
#define LORA_DIO0_PIN 33
#define LORA_DIO1_PIN 38
#define LORA_SPI 1




//DHT22 and SM enable pin
#define DHT22_SM_ENABLE_PIN 11


//Soil moistures sensor pins
#define SOIL_MOISTURE_PIN 9//1


// DHT22 Pins
#define DHTPIN 8


// Temperature sensor pins
#define DS1B20_PIN 6

// Battery level detector pins
#define BATTERY_ENABLE_PIN 3
#define BATTERY_ADC_PIN 1


#define I2C_SDA 17
#define I2C_SCL 21



//Dendrometer pins
#define DENDROMETER_PIN 1//8
#define DENDROMETER_ENABLE_PIN 2//4


//Onboard LED pin
#define DEBUG_LED_PIN 15

//SDI-12 drill and drop sensor pins
#define SDI12_EN_PIN 7
#define SDI12_DATA_PIN 5


//Node define

#define NUMBER_OF_NODES 10

#define NODE_12
#define NODE_NUMBER 12


#ifdef NODE_1
#define NODE_NUMBER 1
#define I2C_SDA 17
#define I2C_SCL 21
#elif defined NODE_2
#define NODE_NUMBER 2
#define I2C_SDA 21
#define I2C_SCL 17
#elif defined NODE_3
#define NODE_NUMBER 3
#define I2C_SDA 21
#define I2C_SCL 17
#elif defined NODE_4
#define NODE_NUMBER 4
#define I2C_SDA 21
#define I2C_SCL 17
#elif defined NODE_5
#define NODE_NUMBER 5
#define I2C_SDA 17
#define I2C_SCL 21
#elif defined NODE_6
#define NODE_NUMBER 6
#define I2C_SDA 17
#define I2C_SCL 21
#elif defined NODE_7

#define NODE_NUMBER 7
#define I2C_SDA 21
#define I2C_SCL 17



// MICROMETER CALIBRATION CONSTANTS
#define M_A  4.26025614e-17
#define M_B  -9.88335767e-13
#define M_C  7.66126854e-09
#define M_D  -1.85180245e-05
#define M_E  9.80831371e-01
#define M_F  -1.28484901e+00


//RIG CALIBRATION CONSTANTS
#define R_A  3.37673695e-17
#define R_B  -7.12719874e-13
#define R_C  4.19104653e-09
#define R_D  1.95365608e-06
#define R_E  9.38004686e-01
#define R_F  7.46228034e+00


#elif defined NODE_8

#define NODE_NUMBER 8
#define I2C_SDA 21
#define I2C_SCL 17



// MICROMETER CALIBRATION CONSTANTS
#define M_A  5.53423952e-17
#define M_B  -1.39203663e-12
#define M_C 1.20505948e-08
#define M_D  -4.24026901e-05
#define M_E  1.04299627e+00
#define M_F  1.82976418e+00


//RIG CALIBRATION CONSTANTS
#define R_A  2.03706921e-16
#define R_B  -5.24739499e-12
#define R_C   4.84677670e-08
#define R_D  -1.92179176e-04
#define R_E   1.28234474e+00
#define R_F  4.97033453e+01

#elif defined NODE_9
#define NODE_NUMBER 9
#define I2C_SDA 21
#define I2C_SCL 17
#elif defined NODE_10
#define NODE_NUMBER 10
#define I2C_SDA 21
#define I2C_SCL 17
#elif defined NODE_11
#define NODE_NUMBER 11
#define I2C_SDA 21
#define I2C_SCL 17
#endif


//SD card defines
#define SD_SCK_PIN  36
#define SD_MISO_PIN  37
#define SD_MOSI_PIN  35
#define SD_CS_PIN    16
#define SD_SPI 2

#define SD_ENABLE_PIN 4



/* File: ads1115.h
* Author: Ethan Lai <ethanlai@stanford.edu>
*
* ads1115.h describes functions to convert analogic input to the ads module into digital output over a I2C protocol
*/

#include "i2c.h"

// enumeration for wired channel of ads1115 device
typedef enum {
    AIN0 = 0,
    AIN1 = 1,
    AIN2 = 2,
    AIN3 = 3,
} ads_input_t;

// clockspeed for i2c devices
#define RATE I2C_100KHZ

// config register of ads1115
#define CONFIG_REG 0b00000001

#define CONVERSION_REG 0b00000000

/*
* 'ads_device_t'
*
* This typedef gives a nickname to the struct that will be used to represent a singel ads1115 device*
*/
typedef struct ads_device ads_device_t; 

/* 'ads_init()'
*
* initializes i2c bus on set rate of 100 khz. Must run before communicating with ads device. The pins 'GPIO_PG12' and
* 'GPIO_PG13' are configured for use as I2C SCL and SDA
*
* @param rate          I2C clock frequency (valid values are 20 khz, 100 khz, and 400 khz)
*/
void ads_init(i2c_clk_freq_t rate);

/* 'ads_new()'*
*
* creates a new ads1115 device for the given I2c address/id. Must run before attempting to read/write from the device
*
* @param addr           I2C address of the ads device (GND: 0b1001000)
* @param input          input address of ads wiring (AIN0, AIN1, AIN2, AIN3) 
* @return               id of newly created ads device
*/
ads_device_t* ads_new(uint8_t addr, ads_input_t input);

/* 'ads_config_for_channel'
*
* creates the 16 bit channel value to write to the ads configuration register. All settings are hardcoded besides the * MUX corresponding to the wired channel the ads1115 device is connected to
* Bit 15: 1 - single conversion
* Bit 14:12: dynamic depending on input wiring (AIN0, AIN1, AIN2, AIN3)
* Bit 11:9: 001 - +- 4.09V for FSR of PGA
* Bit 8: 1 - Single-shot mode
* Bit 7:5: 111 - 860 samples per second
* Bit 4: 0 - default traditional comparator
* Bit 3: 0 - default active low
* Bit 2: 0 - nonlatching comparator
* Bit 1:0: 11 - default disable comparator and set ALERT/RDY pin to hihg-impedance 
*
* @param dev            id of ads device
*/
void ads_config_for_channel(ads_device_t* dev);

/* 'ads_start_conversion'
*
* starts the ads1115 analog to digital conversion by writing the 16 bit channel value from ads_config_for_channel
*
* @param  dev           id of the ads device     
* @return               true if successful write, otherwise false
*/
bool ads_start_conversion(ads_device_t* dev);

/* 'ads_conversion_ready'
*
* temporary function if we want to continoulsy poll from conversion register to check if conversion is ready
*/
 // bool ads_conversion_ready(ads_device_t* dev);

/* 'ads_read_conversion'
*
* reads the raw two byte value from the ads1115 conversion register and stores it a static buffer
*
* @param  dev          id of the ads device
* @return              pointer to static buffer containing the 2 byte conversion
*/
bool ads_read_conversion(ads_device_t* dev);


/* 'ads_read_channel'
*
* full function called from main to start the conversion, wait 2 ms between each sample (default) or continuously
* poll conversion register, and return the raw converted value of analog signal. Returns -1 if writing to the
* config register failed, and -2 if reading from the conversion register failed
*
* @param dev         id of the ads device
* @return            raw analogc->digital value as an int
*/
uint16_t ads_read_channel(ads_device_t* dev);

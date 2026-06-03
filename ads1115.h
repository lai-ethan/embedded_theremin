/* File: ads1115.h
* Author: Ethan Lai <ethanlai@stanford.edu>
*
* ads1115.h describes functions to convert analogic input to the ads module into digital output over a I2C protocl
*/

#include "i2c.h"

#define RATE I2C_100KHZ;

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
*/
void ads_init(i2c_clk_freq_t rate);

/* 'ads_new()'*
*
* creates a new ads1115 device for the given I2c address/id. Must run before attempting to read/write from thh device
*/
ads_device_t* ads_new(uint8_t addr);

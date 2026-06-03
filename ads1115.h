/* File: ads1115.h
* Author: Ethan Lai <ethanlai@stanford.edu>
*
* ads1115.h describes functions to convert analogic input to the ads module into digital output over a I2C protocl
*/

#include "i2c.h"

#define RATE I2C_100KHZ;

/* 'ads_init()'
*
* initializes i2c bus on set rate of 100 khz
*/
void ads_init(i2c_clk_freq_t rate);

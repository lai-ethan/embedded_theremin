/* File: ads1115.c
* Author: Ethan Lai <ethanlai@stanford.edu>
*
* ads1115.c implements functions described by ads115.h to interface with the adc module and return digital output 
* using the I2C communication protocol
*/

#include "i2c.h"

void ads_init(i2c_clk_freq_t RATE) {
    i2c_init(RATE);
}

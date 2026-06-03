/* File: ads1115.c
* Author: Ethan Lai <ethanlai@stanford.edu>
*
* ads1115.c implements functions described by ads115.h to interface with the adc module and return digital output 
* using the I2C communication protocol
*/

#include "i2c.h"
#include "ads115.h"

struct ads_device {
    i2c_device_t* i2c_dev;
};

void ads_init(i2c_clk_freq_t RATE) {
    i2c_init(RATE);
}

ads_device_t* ads_new(uint8_t addr) {
    ads_device_t* dev = malloc(sizeof(*dev));
    dev->i2c_dev = i2c_new(addr);

    return dev;
}

/* File: ads1115.c
* Author: Ethan Lai <ethanlai@stanford.edu>
*
* ads1115.c implements functions described by ads115.h to interface with the adc module and return digital output 
* using the I2C communication protocol
*/

#include "i2c.h"
#include "ads1115.h"
#include "malloc.h"
#include "timer.h"
#include "uart.h"
#include "printf.h"

struct ads_device {
    i2c_device_t* i2c_dev; // i2c device
    ads_input_t input; // ads input (AIN0, AIN1, etc)
    const uint8_t* channel; // 16 bit configuration sequence for ads config register (via ads_config_for_channel)
    uint16_t conversion; // 16 bit conversion value from ads conversion register (via ads_read_conversion)
};

void ads_init(i2c_clk_freq_t RATE) {
    i2c_init(RATE);
}

// address for adc GND pin is 0b1001000, input is wired channel
ads_device_t* ads_new(uint8_t addr, ads_input_t input) {
    ads_device_t* dev = malloc(sizeof(*dev));
    dev->i2c_dev = i2c_new(addr);
    dev->input = input;
    dev->channel = 0;
    dev->conversion = 0;
    

    return dev;
}

// valid reg_size are 1 or 2, valid transition are 0 or 1
void ads_config_device_settings(ads_device_t* dev, uint8_t reg_size, uint8_t transition, int trans_delay_us) {
    i2c_config_device_settings(dev->i2c_dev, reg_size, transition, trans_delay_us);
}

// configures value to write to config register of ads device (hard coded), assuming we wire to GND
void ads_config_for_channel(ads_device_t* dev) {
    static uint8_t buf[2];
    switch (dev->input) {
	// start conversion, MUX (AIN channel), +-4.096V range, single shot mode
	case AIN0:
 	    buf[0] = 0b11000011;
	    break;
 	case AIN1:
	    buf[0] = 0b11010011;
  	    break;
	case AIN2:
	    buf[0] = 0b11100011;
	    break;
	case AIN3:
	    buf[0] = 0b11110011;
 	    break;
    }
    buf[1] = 0b11100011; // 860 samples/sec, traditional comparator, active low, non latching, disable comparator
    dev->channel = buf;
}

// writes config channel to config register to start conversion
bool ads_start_conversion(ads_device_t* dev) {
    return i2c_write_reg_n(dev->i2c_dev, CONFIG_REG, dev->channel, 2);
}

// possible function if we want to poll to see if conversion is ready instead of a small delay sampling
// bool ads_conversion_ready(ads_device_t* dev)

// reads and assembles two bytes from conversion register of ads device 
bool ads_read_conversion(ads_device_t* dev) {
    static uint8_t conversion_buf[2];
    bool result = i2c_read_reg_n(dev->i2c_dev, CONVERSION_REG, conversion_buf, 2);
    dev->conversion = (conversion_buf[0] << 8) | conversion_buf[1];
    return result;
}

// full function setting up conversion, waiting/polling, then reading conversion and returing raw value
uint16_t ads_read_channel(ads_device_t* dev) {
    ads_config_for_channel(dev);
    if (!ads_start_conversion(dev)) {
	return -1;
     }
     timer_delay_ms(20); // delay 2 ms for 860 samples/sec -> 1 sample takes 860/sec = 1.16 ms
     if (!ads_read_conversion(dev)) {
	return -2;
     }
     return dev->conversion;
}

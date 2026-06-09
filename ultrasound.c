/* File: ultrasound.c
*  Author: Ethan Lai <ethanlai@stanford.edu>
*
* implemented functions described by ultrasound.h
*/

#include "ultrasound.h"
#include "interrupts.h"
#include "timer.h"
#include "gpio_extra.h"
#include "gpio_interrupt.h"
#include "malloc.h"

/*
* A ultrasound_device is a structure that stores all the state and information needed
* for a ultrasound_device. The trigger field stores the gpio id for the trigger pin,
* and the echo field stores the gpio id for the echo pin.
*/
struct ultrasound_device {
    gpio_id_t trigger;
    gpio_id_t echo;
    volatile unsigned long start_time; // interrupts
    volatile unsigned long end_time;
    bool is_ready;
    unsigned long prev_1_read;
    unsigned long prev_2_read;
};

// interrupt handler for both rising edge and falling edge of echo line
static void handle_distance(void* aux_data) {
    ultrasound_device_t* dev = (ultrasound_device_t*)aux_data;
    gpio_interrupt_clear(dev->echo);

    if (gpio_read(dev->echo) == 1) { 
        // if rising edge then get start time
	    dev->start_time = timer_get_ticks(); 
    } 
    
    else if (gpio_read(dev->echo) == 0) { // if falling edge get end time
	    dev->end_time = timer_get_ticks();
	    dev->is_ready = true;
    }
}

ultrasound_device_t* ultrasound_new(gpio_id_t TRIG_GPIO, gpio_id_t ECHO_GPIO) {
    ultrasound_device_t* dev = malloc(sizeof(*dev));

    dev->trigger = TRIG_GPIO;
    gpio_set_output(dev->trigger);

    dev->echo = ECHO_GPIO;
    gpio_set_input(dev->echo);
    gpio_set_pullup(dev->echo);

    dev->prev_1_read = -1;
    dev->prev_2_read = -1;
    
    return dev;
}

// wait 60ms between each reading to prevent corruption of reading
unsigned long ultrasound_read(ultrasound_device_t* dev, int timeout) {

    gpio_write(dev->trigger, 1); // set trig high for ~10 us
    timer_delay_us(10);
    gpio_write(dev->trigger, 0);
    
    unsigned long curr_ticks = timer_get_ticks();

    while (gpio_read(dev->echo) != 1) {
	unsigned long now = timer_get_ticks();
        if (now - curr_ticks > timeout) {
            return -2;
        }
    }; // wait until high
    unsigned long start_time = timer_get_ticks(); // get start time

    while (gpio_read(dev->echo) == 1) {
	unsigned long now = timer_get_ticks();
        if (now - start_time > timeout) {
            return -2;
        }
    }; // while echo held high

    unsigned long end_time = timer_get_ticks(); // get end time

    unsigned long pulse_width = (end_time - start_time);
    unsigned long distance = pulse_width; // 58 for cm or 148 for inch

    return distance;
}


// wait 60ms between each reading to prevent corruption of reading
unsigned long ultrasound_read_filtered(ultrasound_device_t* dev, int timeout) {
    unsigned long curr_read = ultrasound_read(dev, timeout);
    unsigned long ans = curr_read;

    if (dev->prev_1_read == -1 || dev->prev_2_read == -1) { }
    else if (curr_read == -2) { return -2; }
    else {
        if ( (dev->prev_2_read <= curr_read && dev->prev_2_read >= dev->prev_1_read) ||
            (dev->prev_2_read >= curr_read && dev->prev_2_read <= dev->prev_1_read) ) {
            ans = dev->prev_2_read;
        }
        else if ( (dev->prev_1_read <= curr_read && dev->prev_1_read >= dev->prev_2_read) ||
            (dev->prev_1_read >= curr_read && dev->prev_1_read <= dev->prev_2_read) ) {
            ans = dev->prev_1_read;
        }
    }
    
    dev->prev_2_read = dev->prev_1_read;
    dev->prev_1_read = curr_read;

    return ans;
}

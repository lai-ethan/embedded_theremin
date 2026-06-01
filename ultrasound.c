/* File: ultrasound.c
*  Author: Ethan Lai <ethanlai@stanford.edu>
*
* implemented functions described by ultrasound.h
*/

#include "ulrasound.h"
#include "interrupts.h"
#include "timer.h"
#include "gpio_extra.h"
#include "gpio_interrupt.h"


/*
* A ultrasound_device is a structure that stores all the state and information needed
* for a ultrasound_device. The trigger field stores the gpio id for the trigger pin,
* and the echo field stores the gpio id for the echo pin.
*/
struct ultrasound_device {
    gpio_id_t trigger;
    gpio_id_t echo;
    unsigned long rise_time; // interrupts
    unsigned long end_time;
    bool is_ready;
};

// interrupt handler for both rising edge and falling edge of echo line
void handle_distance(void* aux_data) {
    ultrasound_device_t* dev = (ultrasound_device_t*)aux_data;
    gpio_interrupt_clear(dev->echo);

    if (gpio_read(dev->echo) == 1) { // if rising edge then get start time
	dev->rise_time = timer_get_ticks(); 
    } else if (gpio_read(dev->echo) == 0) // if falling edge get end time
	dev->end_time = timer_get_ticks();
	dev->is_ready = true;
  	
    }
}

ultrasound_device_t* ultrasound_new(gpio_id_t TRIG_GPIO, gpio_id_t ECHO_GPIO) {
    gpio_device_t* dev = malloc(sizeof(*dev));

    dev->trig = TRIG_GPIO;
    gpio_set_output(dev->trig);

    dev->echo = ECHO_GPIO;
    gpio_set_input(dev->echo);
    gpio_set_pullup(dev->echo);

    // interrupts
    // dev->start_time = 0;
    // dev->end_time = 0;
    // dev->is_ready = false;

    // gpio_interrupt_init(); // config gpio interrupts
    // gpio_interrupt_config(dev->echo, GPIO_INTERRUPT_DOUBLE_EDGE, false); // debounce?
    // gpio_interrupt_set_handler(dev->echo, handle_distance, dev); // set handler

    return dev;
}

// wait 60ms between each reading to prevent corruption of reading
unsigned long ultrasound_read(ultrasound_device_t* dev) {

    gpio_write(dev->trigger, 1); // set trig high for ~10 us
    timer_delay_us(10);
    gpio_write(dev->trigger, 0);

    while (gpio_read(dev->echo) != 1) ; // wait until high
    unsigned long start_time = timer_get_ticks(); // get start time

    while (gpio_read(dev->echo) == 1) ; // while echo held high
    unsigned long end_time = timer_get_ticks(); // get end time

    unsigned long pulse_width = (end_time - start_time) / TICKS_PER_USEC;
    unsigned long distance = pulse_width / 58; // 58 for cm or 148 for inch

    return distance;
}



//          INTERRUPT SECTION 
// main still has to do interrupts_init() and interrupts_global_enable()

// called to start ultrasound reading
void start_read_pitch(ultrasound_device_t* dev) {
    gpio_write(dev->trigger, 1); // set trig high for ~10 us
    timer_delay_us(10);
    gpio_write(dev->trigger, 0); 
}

// called from main once dev->is_ready = true
unsigned long calc_distance(ultrasound_device_t* dev) {
    dev->is_ready = false;
    unsigned long pulse_width = (dev->end_time - dev->start_time) / TICKS_PER_USEC;
    return pulse_width / 58; // 58 for cm or 148 for inch
}

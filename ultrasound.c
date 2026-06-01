/* File: ultrasound.c
*  Author: Ethan Lai <ethanlai@stanford.edu>
*
* implemented functions described by ultrasound.h
*/

#include "ulrasound.h"
#include "interrupts.h"

const 

/*
* A ultrasound_device is a structure that stores all the state and information needed
* for a ultrasound_device. The trigger field stores the gpio id for the trigger pin,
* and the echo field stores the gpio id for the echo pin.
*/
struct ultrasound_device {
    gpio_id_t trigger;
    gpio_id_t echo;

};

ultrasound_device_t* ultrasound_new(TRIG_GPIO, ECHO_GPIO) {
    gpio_device_t* dev = malloc(sizeof(*dev));

    dev->trig = TRIG_GPIO;
    gpio_set_output(dev->trig);

    dev->echo = ECHO_GPIO;
    gpio_set_input(dev->echo);
    gpio_set_pullup(dev->echo);

    return dev;
}

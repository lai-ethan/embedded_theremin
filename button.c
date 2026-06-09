/* File: button.c
* Author: Ethan Lai <ethanlai@stanford.edu>
*
* functions to interface with a button, as described in button.h
*/

#include "button.h"
#include "gpio.h"
#include "interrupts.h"
#include "gpio_interrupt.h"
#include "malloc.h"
#include "gpio_extra.h"

/*
* a button_device is a struct that stores all the information and state needed for a singular button. 
*/
struct button_device {
    gpio_id_t button; // gpio pin of button
    volatile bool pressed; // whether pushed or not
};

// interrupt handler for negative edge of button (clicked)
static void button_pressed(void* aux_data) {
    button_device_t* dev = (button_device_t*)aux_data;
    gpio_interrupt_clear(dev->button);
    dev->pressed = !dev->pressed;
}

// initializes new button device and configures interrupts for it
button_device_t* button_new(gpio_id_t pin) {
    button_device_t* dev = malloc(sizeof(*dev));

    dev->button = pin;
    dev->pressed = false;
 
    gpio_set_input(dev->button);
    gpio_set_pullup(dev->button);

    gpio_interrupt_config(dev->button, GPIO_INTERRUPT_NEGATIVE_EDGE, true);
    gpio_interrupt_set_handler(dev->button, button_pressed, dev);

    return dev;
}

// function called by main to check whether the button has been pressed or not
bool is_button_pressed(button_device_t* dev) {
    return dev->pressed;
}

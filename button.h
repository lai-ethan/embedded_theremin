/* File: button.h
* Author: Ethan Lai <ethanlai@stanford.edu>
*
* implements functions to interface with an external button, with interrupts that register a boolean so that main.c
* knows if the button has been pressed
*/

#include "gpio.h"
#include <stdbool.h>

/* Type: 'button_t'
*
* This typedef gives a nickname to the struct that will be used to represent a single button device.
*/
typedef struct button_device button_device_t;

/* 'button_new'
*
* Initializes a new button_device_t connected to specified GPIO pin and returns a pointer to the button device.
*
* @param pin         id of gpio button is wired to
* @return            pointer to button_device_t struct
*/
button_device_t* button_new(gpio_id_t pin);

/* 'is_button_pressed'
*
* Function called by main that checks whether the button has been pressed or not. Details of implementation and
* the contents of the button_device_t struct are hidden from the client. Resets to default (button_pressed = false);
*
* @param dev         pointer to button struct
* @return            true/false depending if button has been pressed or not
*/
bool is_button_pressed(button_device_t* dev);

/* File: ultrasound.h
*  Author: Ethan Lai <ethanlai@stanford.edu>
*
* Logic to interface with an HC-SR04 Ultrasound Module, with each function building
* on top of each other to ultimately calculate physical distance to an object
*/

#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>

// gpio pins for connected ultrasound trig/echo lines
#define TRIG_GPIO GPIO_PB3
#define ECHO_GPIO GPIO_PB4

/* Type: 'ultrasound_device_t'
*
* This typedef gives a nickname to the struct that will be used to represent a single 
* ultrasound device. The internald details of the struct are given in ultrasound.c
*
*/
typedef struct ultrasound_device ultrasound_device_t;

/* 'ultrasound_new'
*
* Initializes a new ultrasound_device device connected to specified trigger and echo pins and
* returns a pointer.
*
* To configure a new ultrasound_device in your code:
* 	
*    ultrasound_device_t* = ultrasound_new(TRIG_GPIO, ECHO_GPIO);
*
* @param trig          the id of gpio connected to the trigger line of the ultrasound device
* @param echo          the id of gpio connected to the echo line of the ultrasound device
* @return              pointer to new ultrasound device or NULL if failed to create
*/
ultrasound_device_*t ultrasound_new(gpio_id_t trig, gpio_id_t echo);

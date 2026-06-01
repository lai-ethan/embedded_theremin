#include "uart.h"
#include "mymodule.h"
#include "timer.h"
#include "ultrasound.h"
#include "printf.h"
#include "strings.h"

void main(void) {
    timer_init();
    gpio_init();
    uart_init();

    ultrasound_device_t* dev = ultrasound_new(TRIG_GPIO, ECHO_GPIO);
    printf("STARTING READING\n");
    while (1) {
	unsigned long cm = ultrasound_read(dev);
        printf("Sensing: %ld cm\n", cm);
        timer_delay_ms(250);
    }
}

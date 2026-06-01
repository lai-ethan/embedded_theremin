#include "uart.h"
#include "mymodule.h"
#include "timer.h"
#include "ultrasound.h"
#include "printf.h"
#include "strings.h"

static void show_reading(unsigned long cm) {
    if (cm > MAX_RANGE) return; // don't print anything if nothing deteted
    char buf[MAX_RANGE+1];
    memset(buf, '-', sizeof(buf));
    buf[cm] = '|';
    buf[cm+1] = '\0';
    printf("%s %ld\n", buf, cm);
}

void main(void) {
    timer_init();
    gpio_init();
    uart_init();
    // interrupts_init();

    ultrasound_device_t* dev = ultrasound_new(TRIG_GPIO, ECHO_GPIO);
    // interrupts_global_enable();
    printf("STARTING READING\n");
    while (1) {
        //  start_read_pitch(dev);
        //  
  	//  if (dev->is_ready) {
	//      unsigned long cm  = ultrasound_calc_distance(dev)
	//      show_reading(cm);
	//  } else {
	//      uart_putchar('+');
	//  }
	unsigned long cm = ultrasound_read(dev);
  	show_reading(cm);
        timer_delay_ms(250);
    }
}

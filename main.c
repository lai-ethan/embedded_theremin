#include "uart.h"
#include "mymodule.h"
#include "timer.h"
#include "ultrasound.h"
#include "printf.h"
#include "strings.h"
#include "ads1115.h"
#include "button.h"

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

//    ULTRASOUND/BUTTON INTERRUPT TESTING

    interrupts_init();
//
//    ultrasound_device_t* dev = ultrasound_new(TRIG_GPIO, ECHO_GPIO);
    button_device_t* dev = button_new(GPIO_PB8);
//
    interrupts_global_enable();
//
    printf("STARTING READING BUTTON\n");
    while(1) {
	bool result = is_button_pressed(dev);
	if (result) {
	    printf("pressed\n");
 	} else {
	    printf("not pressed\n");	
 	}
    }

//    printf("STARTING READING\n");
//    while (1) {
//        start_read_pitch(dev);
//          
//  	  if (ultrasound_is_ready(dev)) {
//	      unsigned long cm  = ultrasound_calc_distance(dev);
//	      show_reading(cm);
//	  } else {
//	      uart_putchar('+');
//	  }
//	// unsigned long cm = ultrasound_read(dev);
//  	// show_reading(cm);
//        // timer_delay_us(60);
//    }

//       ADS TESTING SECTION

//    ads_init(RATE);
//    ads_device_t* dev = ads_new(0b1001000, AIN0);
//    while (1) {
//	uint16_t conversion = ads_read_channel(dev);
//        printf("value is %d\n", conversion);
//    }
}


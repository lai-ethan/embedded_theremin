#include "dma.h"
#include "i2s.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "math.h"
#include "malloc.h"
#include "ultrasound.h"
#include "strings.h"
#include "speaker.h"

typedef struct {
    float min_freq;
    float max_freq;
    float min_dist;
    float max_dist;
    float tolerance;
    float prev_dist;
} distance_params;

distance_params params = {
    .min_freq = 0,
    .max_freq = 1000,
    .min_dist = 3,
    .max_dist = 200,
};

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
    speaker_init();

    ultrasound_device_t* dev = ultrasound_new(TRIG_GPIO, ECHO_GPIO);
    printf("STARTING READING\n");
    
    // Initialize sound generation
    int duration_ms = 90;
    i2s_frame_type_t ftype = I2S_STEREO;

    // Calc num samples needed
    int num_samples = count_num_samples(duration_ms, ftype);

    // Set aside memory for samples
    int16_t * samples = malloc(sizeof(int16_t) * num_samples);
    int16_t * new_samples = malloc(sizeof(int16_t) * num_samples);

    // Sound generation params 
    int amplitude = 500;
    float freq = 400;
    
    // Sample pointers
    int16_t * update_samples = new_samples;
    int16_t * curr_samples = samples; 
    
    // Initialize
    gen_sample_fast(samples, num_samples, freq, amplitude, ftype);

    while (true) {

        // Generate sample and play sound
        i2s_stream_playback_nb(curr_samples, num_samples, ftype);
                
        // Update frequency / amplitude based on interrupts
        unsigned long cm = ultrasound_read(dev) / 50;
  	    show_reading(cm);

        if (cm < 3000) {
            gen_sample_fast(update_samples, num_samples, cm, amplitude, ftype);
        }
        else {
            gen_sample_fast(update_samples, num_samples, freq, amplitude, ftype);
        }

        // Switch sample
        if (update_samples == new_samples) {
            update_samples = samples;
            curr_samples = new_samples;
        }
        else {
            update_samples = new_samples;
            curr_samples = samples;
        }

        // Wait till sample is sent        
        i2s_await();

    }  
}

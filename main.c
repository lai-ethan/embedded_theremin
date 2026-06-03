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
    float min_amp;
    float max_amp;
} distance_params;

distance_params params = {
    .min_freq = 0,
    .max_freq = 1000,
    .min_amp = 0,
    .max_amp = 3000
};

static void show_reading(unsigned long cm) {
    
    if (cm > MAX_RANGE) return; // don't print anything if nothing deteted
    char buf[MAX_RANGE+1];
    memset(buf, '-', sizeof(buf));
    buf[cm] = '|';
    buf[cm+1] = '\0';
    printf("%s %ld\n", buf, cm);
    // printf("%ld\n", cm);
}

static float convert_ticks_frequency (unsigned long ticks) {
    ticks /= 80;

    float normalized = ((float) ticks - params.min_freq) / (params.max_freq - params.min_freq);
    float curve = 1 - (1 - normalized) * (1 - normalized) * (1 - normalized);
    return params.min_freq + ( curve * (params.max_freq - params.min_freq));
}

static float update_frequency(ultrasound_device_t* dev, float prev_freq, int threshold) {

    unsigned long ticks = ultrasound_read_filtered(dev, threshold);
    float ans = prev_freq;

    if (ticks < threshold && ticks != -2) {
        ans = convert_ticks_frequency(ticks);
    }
    
    return ans;

}

static float update_amplitude(ultrasound_device_t* dev, float prev_amp, int threshold) {

    unsigned long ticks = ultrasound_read_filtered(dev, threshold);
    float ans = prev_amp;

    if (ticks < threshold && ticks != -2) {
        ans = (ticks * (params.max_amp - params.min_amp)) / threshold;
    }

    return ans;

}

void main(void) {
    timer_init();
    gpio_init();
    uart_init();
    speaker_init();

    ultrasound_device_t* dev_freq = ultrasound_new(GPIO_PB12, GPIO_PB11);
    ultrasound_device_t* dev_amp = ultrasound_new(GPIO_PD22, GPIO_PD21);
    
    // Initialize sound generation
    int duration_ms = 90;
    i2s_frame_type_t ftype = I2S_STEREO;

    // Calc num samples needed
    int num_samples = count_num_samples(duration_ms, ftype);

    // Set aside memory for samples
    int16_t * samples = malloc(sizeof(int16_t) * num_samples);
    int16_t * new_samples = malloc(sizeof(int16_t) * num_samples);

    // Sound generation params 
    int amp = 1200;
    int amp_dynamic = amp;
    float freq = 400;
    float freq_dynamic = freq;

    // Sample pointers
    int16_t * update_samples = new_samples;
    int16_t * curr_samples = samples; 
    
    // Initialize
    gen_sample_fast(samples, num_samples, freq, amp, ftype);

    while (true) {

        // Generate sample and play sound
        i2s_stream_playback_nb(curr_samples, num_samples, ftype);
                
        // Update Frequency
        freq_dynamic = update_frequency(dev_freq, freq_dynamic, 50000);
        printf("FREQ: %d\n", (int) freq_dynamic);

        // Update Amplitude
        amp_dynamic = update_amplitude(dev_amp, amp_dynamic, 50000);
        printf("AMP: %d\n", amp_dynamic);

        // Generate New Sample
        gen_sample_fast(update_samples, num_samples, freq_dynamic, amp_dynamic, ftype);

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

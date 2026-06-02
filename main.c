#include "dma.h"
#include "i2s.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "math.h"
#include "malloc.h"

#define PI 3.14159265358979323846f

int SAMPLE_RATE = I2S_48KHZ;
int LUT_SIZE = 1024;
float *sin_table;
float phase = 0;

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

void sin_init(void) {

    sin_table = malloc(sizeof(float) * LUT_SIZE);

    for (int i = 0; i < LUT_SIZE; i++) {
        sin_table[i] = sin(2 * PI * i / LUT_SIZE);
    }

}

void gen_sample_fast (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype) {
    float phase_step = LUT_SIZE * frequency / SAMPLE_RATE;

    printf("Start");
    
    if (ftype == I2S_STEREO) {
        for (int i = 0; i < num_samples-1; i+=2) {
           
           samples[i] = (int) (amplitude * sin_table[(int) phase]);
           samples[i+1] = samples[i];
        
           phase += phase_step;

           if (phase > LUT_SIZE) {
               phase = phase - LUT_SIZE;
           }

        }
    }
    else {
        for (int i = 0; i < num_samples; i++) {
           samples[i] = (int) (amplitude * sin_table[(int) phase]);
       
           phase += phase_step;

           if (phase > LUT_SIZE) {
               phase = phase - LUT_SIZE;
           }
        }
    }

    printf("Done");
    
}

void main(void) {

    uart_init();
    dma_init();
    i2s_init(SAMPLE_RATE);

    // Initialize sound generation
    int duration_ms = 100;
    i2s_frame_type_t ftype = I2S_STEREO;

    // Initialize sin table
    sin_init();

    // Calc num samples needed
    int num_samples = duration_ms * SAMPLE_RATE / 1000;
    if (ftype == I2S_STEREO) {
        num_samples *= 2;
    }

    // Set aside memory for samples
    int16_t * samples = malloc(sizeof(int16_t) * num_samples);
    int16_t * new_samples = malloc(sizeof(int16_t) * num_samples);
    
    // Flags to read from ultrasounds
    bool read_pitch = true;
    bool read_vol = true;

    // Sound generation params 
    int amplitude = 16000;
    float freq = 1000;
    // int increment = 1;
    
    // Sample pointers
    int16_t * update_samples = new_samples;
    int16_t * curr_samples = samples; 
    
    // Initialize
    gen_sample_fast(samples, num_samples, freq, amplitude, ftype);

    while (true) {

        // Read from ultrasonic sensors if flag is true
        // if (read_pitch) {
        //     start_read_pitch();   
        // }
        // if (read_vol) {
        //     start_read_vol();   
        // }

        // Effects
        // read_reverb();
        // read_base();
        
        // Generate sample and play sound
        i2s_stream_playback_nb(curr_samples, num_samples, ftype);
        gen_sample_fast(update_samples, num_samples, freq, amplitude, ftype);

        // Switch sample
        if (update_samples == new_samples) {
            update_samples = samples;
            curr_samples = new_samples;
        }
        else {
            update_samples = new_samples;
            curr_samples = samples;
        }

        // Update frequency / amplitude based on interrupts

        // Test: Simulare Siren
        // freq -= 10 * increment;

        // if (freq == 0) {
        //     increment = -1;
        // }
        // if (freq == 1000) {
        //     increment = 1;
        // }

        
        // Wait till sample is sent        
        i2s_await();

    }  
}

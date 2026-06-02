#include "assert.h"
#include "dma.h"
#include "i2s.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "math.h"
#include "all-right-now.h"
#include "malloc.h"
#include "timer.h"

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

bool check_dist_change (float distance) {
    return distance - params.prev_dist > params.tolerance;
}

float distance_to_freq (float distance) {
    return params.max_freq + (distance - params.min_dist) / (distance - params.max_dist) * (params.max_freq - params.min_freq);
}

void gen_sample_fast (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype) {
    float phase_step = LUT_SIZE * frequency / SAMPLE_RATE;

    printf("start");
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
           samples[i] = amplitude * sin(2*PI * frequency * i / SAMPLE_RATE);
        }
    }

    printf("Done");
    
}

void add_base(int16_t samples[], int16_t new_samples[], int num_samples, float alpha, float gain) {
    
    // Filter out frequencies
    for (int n = 0; n < num_samples; n++) {
        new_samples[n] = alpha * samples[n];
        if (n == 0) {
            continue;
        }
        new_samples[n] += (1 - alpha) * new_samples[n-1];
    }

    // Add gain
    for (int n = 0; n < num_samples; n++) {
        new_samples[n] = new_samples[n] * gain + samples[n] * 0.2;
    }
}

// void main(void) {
//     uart_init();
//     dma_init();
//     i2s_init(SAMPLE_RATE);

//     // EFFECTS
//     int duration_ms = 100;
//     i2s_frame_type_t ftype = I2S_STEREO;

//     sin_init();

//     int num_samples = duration_ms * SAMPLE_RATE / 1000;
//     if (ftype == I2S_STEREO) {
//         num_samples *= 2;
//     }

//     int16_t * samples = malloc(sizeof(int16_t) * num_samples);
//     int16_t * new_samples = malloc(sizeof(int16_t) * num_samples);
    
//     int amplitude = 4000;
//     float freq = 1000;
//     int iters = 0;

//     gen_sample_fast(samples, num_samples, freq, amplitude, ftype);

//     while (true) {
//         while (freq > 60) {

//             i2s_stream_playback_nb(samples, num_samples, ftype);
//             // freq -= 10; 
//             gen_sample_fast(new_samples, num_samples, freq, amplitude, ftype);
//             i2s_await();

//             i2s_stream_playback_nb(new_samples, num_samples, ftype);
//             // freq -= 10;   
//             gen_sample_fast(samples, num_samples, freq, amplitude, ftype);
//             i2s_await();
//         }
        
        
//         while (freq < 998) {

//             i2s_stream_playback_nb(samples, num_samples, ftype);
//             freq += 10;   
//             gen_sample_fast(new_samples, num_samples, freq, amplitude, ftype);
//             i2s_await();

//             i2s_stream_playback_nb(new_samples, num_samples, ftype);
//             freq += 10;   
//             gen_sample_fast(samples, num_samples, freq, amplitude, ftype);
//             i2s_await();
//         }
//     }    
// }


void main(void) {
    uart_init();
    dma_init();
    i2s_init(SAMPLE_RATE);

    // EFFECTS
    int duration_ms = 100;
    i2s_frame_type_t ftype = I2S_STEREO;

    sin_init();

    int num_samples = duration_ms * SAMPLE_RATE / 1000;
    if (ftype == I2S_STEREO) {
        num_samples *= 2;
    }

    int16_t * samples = malloc(sizeof(int16_t) * num_samples);
    int16_t * new_samples = malloc(sizeof(int16_t) * num_samples);
    
    int amplitude = 4000;
    float freq = 1000;
    int iters = 0;
    
    gen_sample_fast(samples, num_samples, freq, amplitude, ftype);

    while (true) {

            i2s_stream_playback_nb(samples, num_samples, ftype);
            // freq -= 10; 
            gen_sample_fast(new_samples, num_samples, freq, amplitude, ftype);
            i2s_await();

            i2s_stream_playback_nb(new_samples, num_samples, ftype);
            // freq -= 10;   
            gen_sample_fast(samples, num_samples, freq, amplitude, ftype);
            i2s_await();
        }
        
        
        while (freq < 998) {

            i2s_stream_playback_nb(samples, num_samples, ftype);
            freq += 10;   
            gen_sample_fast(new_samples, num_samples, freq, amplitude, ftype);
            i2s_await();

            i2s_stream_playback_nb(new_samples, num_samples, ftype);
            freq += 10;   
            gen_sample_fast(samples, num_samples, freq, amplitude, ftype);
            i2s_await();
        }
    }    
}
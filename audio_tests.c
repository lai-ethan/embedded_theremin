// #include "uart.h"
// #include "speaker.h"

// void main(void) {
//     uart_init();
//     say_hello("CS107e");
// }

/* File: audio_demo.c
 * ------------------
 * Demo I2S playback and capture
 */
#include "assert.h"
#include "dma.h"
#include "i2s.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "math.h"
#include "all-right-now.h" // header file contains array of samples converted from wav file
#include "malloc.h"
#define PI 3.14159265358979323846f

int SAMPLE_RATE = I2S_48KHZ;
int LUT_SIZE = 1024;
float *sin_table;
float phase = 0;

struct {
    float min_freq;
    float max_freq;
    float min_dist;
    float max_dist;
    float tolerance;
    float prev_dist;
} distance_params;

distance_params params;
params->min_freq = 0;
params->max_freq = 1000;
params->min_dist = 0;
params->max_dist  = 1000;

// void gen_sample (int16_t samples[], int num_samples, int frequency, int amplitude, i2s_frame_type_t ftype) {
//     printf("start");
//     if (ftype == I2S_STEREO) {
//         for (int i = 0; i < num_samples-1; i+=2) {
//            samples[i] = (int) ((amplitude) * sin(2 * PI * frequency * i / SAMPLE_RATE));
//            samples[i+1] = samples[i];
//         }
//     }
//     else {
//         for (int i = 0; i < num_samples; i++) {
//            samples[i] = amplitude * sin(2*PI * frequency * i / SAMPLE_RATE);
//         }
//     }

//     printf("Done");
    
// }
void sin_init(void) {

    sin_table = malloc(sizeof(float) * LUT_SIZE);

    for (int i = 0; i < LUT_SIZE; i++) {
        sin_table[i] = sin(2 * PI * i / LUT_SIZE);
    }

}

bool check_dist_change (float distance) {
    return distance - params->prev_distance > params->tolerance;
}

float distance_to_freq (float distance) {
    return params->max_freq + (distance - params->min_dist) / (distance - params->max_dist) * (params->max_freq - params->min_freq);
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

void main(void) {
    uart_init();
    dma_init();
    i2s_init(SAMPLE_RATE);

    // DEMO SONG
    // assert(gwav->bits_per_sample == 16); // DAC support only tested for 16-bit
    // int sample_freq = gwav->frames_per_sec;
    
    // i2s_init(sample_freq);
    // i2s_frame_type_t ftype = (gwav->samples_per_frame == 1) ? I2S_MONO : I2S_STEREO;

    // i2s_stream_playback_nb(gwav->data, gwav->n_data, ftype);
    // i2s_await();

    // // DESCENDING NOTES
    // int duration_ms = 1;
    // i2s_frame_type_t ftype = I2S_STEREO;

    // int num_samples = duration_ms * SAMPLE_RATE / 100;
    // if (ftype == I2S_STEREO) {
    //     num_samples *= 2;
    // }

    // int16_t * samples = malloc(sizeof(int16_t) * num_samples);
    
    // int freq = 1000;
    // int iters = 0;
    // while (iters < 5) {
    //     while (freq > 0) {
            
    //         freq -= 10;
    //         i2s_stream_playback_nb(samples, num_samples, ftype);   
    //         gen_sample(samples, num_samples, freq, 4000, ftype);
    //         i2s_await();
        
    //     }
    //     while (freq < 1000) {
            
    //         freq += 10;
    //         i2s_stream_playback_nb(samples, num_samples, ftype);   
    //         gen_sample(samples, num_samples, freq, 4000, ftype);
    //         i2s_await();
    //     }
    //     iters++;
        
    // }

    // EFFECTS
    int duration_ms = 1;
    i2s_frame_type_t ftype = I2S_STEREO;

    sin_init();

    int num_samples = duration_ms * SAMPLE_RATE / 1000;
    if (ftype == I2S_STEREO) {
        num_samples *= 2;
    }

    int16_t * samples = malloc(sizeof(int16_t) * num_samples);
    int16_t * new_samples = malloc(sizeof(int16_t) * num_samples);
    
    float freq = 1000;
    int iters = 0;

    gen_sample_fast(samples, num_samples, freq, 4000, ftype);

    while (true) {
        while (freq > 2) {

            i2s_stream_playback_nb(samples, num_samples, ftype);
            freq -= 0.3;   
            gen_sample_fast(new_samples, num_samples, freq, 4000, ftype);
            i2s_await();

            i2s_stream_playback_nb(new_samples, num_samples, ftype);
            freq -= 0.3;   
            gen_sample_fast(samples, num_samples, freq, 4000, ftype);
            i2s_await();
        }
        
        
        while (freq < 998) {

            i2s_stream_playback_nb(samples, num_samples, ftype);
            freq += 0.3;   
            gen_sample_fast(new_samples, num_samples, freq, 4000, ftype);
            i2s_await();

            i2s_stream_playback_nb(new_samples, num_samples, ftype);
            freq += 0.3;   
            gen_sample_fast(samples, num_samples, freq, 4000, ftype);
            i2s_await();
        }
    }    

    // add_base(samples, new_samples, num_samples, 0.2, 0.6);
        
    
    // while (iters >= 0) {
        
    //     i2s_stream_playback_nb(new_samples, num_samples, ftype);   
    //     gen_sample(new_samples, num_samples, freq, 4000, ftype);
    //     i2s_await();

    //     iters -= 1;
    // }
    
    // printf("Done");

    // interrupts_init();
    // interrupts_global_enable();
    // interrupts_set_handler(interrupt_source_t source, handlerfn_t fn, void *aux_data);


}
#include "math.h"
#include "dma.h"
#include "i2s.h"
#include "printf.h"
#include "timer.h"
#include "uart.h"
#include "malloc.h"
#include "ultrasound.h"
#include "strings.h"
#include "speaker.h"

#define PI 3.14159265358979323846f

int SAMPLE_RATE = I2S_48KHZ;
int LUT_SIZE = 1024;

float *sin_table;
float phase = 0;

void speaker_init (void);
int count_num_samples (int duration_ms, i2s_frame_type_t ftype);
void gen_sample_fast (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype);
void sin_init(void);

void speaker_init (void) {

    dma_init();
    i2s_init(SAMPLE_RATE);
    sin_init();

}

void sin_init(void) {

    sin_table = malloc(sizeof(float) * LUT_SIZE);

    for (int i = 0; i < LUT_SIZE; i++) {
        sin_table[i] = sin(2 * PI * i / LUT_SIZE);
    }

}

int count_num_samples (int duration_ms, i2s_frame_type_t ftype) {

    int num_samples = duration_ms * SAMPLE_RATE / 1000;

    if (ftype == I2S_STEREO) {
        num_samples *= 2;
    }

    return num_samples;
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
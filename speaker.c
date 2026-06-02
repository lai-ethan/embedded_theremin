#include "math.h"
#include "dma.h"
#include "i2s.h"

void gen_sample_fast (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype, float *sin_table) {
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

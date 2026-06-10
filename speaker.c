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
float phase_3 = 0;
float phase_5 = 0;

float vibrato_phase = 0;
float vibrato_phase_3 = 0;
float vibrato_phase_5 = 0;

void speaker_init (void);
int count_num_samples (int duration_ms, i2s_frame_type_t ftype);
void gen_sample_fast (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype, float vib_freq, float vib_depth);
void gen_sample_fast_chord (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype, float vib_freq, float vib_depth);
void sin_init(void);

// Speaker Initialize
void speaker_init (void) {

    dma_init();
    i2s_init(SAMPLE_RATE);
    sin_init();

}

// Fill sin Table
void sin_init(void) {

    sin_table = malloc(sizeof(float) * LUT_SIZE);

    for (int i = 0; i < LUT_SIZE; i++) {
        sin_table[i] = sin(2 * PI * i / LUT_SIZE);
    }

}

// Calculate number of samples for duration
int count_num_samples (int duration_ms, i2s_frame_type_t ftype) {

    int num_samples = duration_ms * SAMPLE_RATE / 1000;

    if (ftype == I2S_STEREO) {
        num_samples *= 2;
    }

    return num_samples;
}

// Generate audio sample
void gen_sample_fast (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype, float vib_freq, float vib_depth) {
    
    float phase_step = LUT_SIZE * frequency / SAMPLE_RATE;
    float vibrato_phase_step = LUT_SIZE * vib_freq / SAMPLE_RATE;
    float vibrato_depth = LUT_SIZE * vib_depth / SAMPLE_RATE;

    if (ftype == I2S_STEREO) {
        for (int i = 0; i < num_samples-1; i+=2) {
                        
            samples[i] = (int) ( (amplitude * sin_table[(int) phase]));
            samples[i+1] = samples[i];
            
            float modifier = (vibrato_depth * sin_table[(int) vibrato_phase]);

            phase += phase_step + modifier;
            vibrato_phase += vibrato_phase_step;

            if (phase > LUT_SIZE) {
                phase -= LUT_SIZE;
            }
            
            if (vibrato_phase > LUT_SIZE) {
                vibrato_phase -= LUT_SIZE;
            }
        }
    }
    else {
        for (int i = 0; i < num_samples; i++) {
            samples[i] = (int) (amplitude * sin_table[(int) phase]);
        
            float modifier = (vibrato_depth * sin_table[(int) vibrato_phase]);
            
            phase += phase_step + modifier;
            vibrato_phase += vibrato_phase_step;

            if (phase > LUT_SIZE) {
                phase -= LUT_SIZE;
            }
            
            if (vibrato_phase > LUT_SIZE) {
                vibrato_phase -= LUT_SIZE;
            }
        }
    }
    
}

// Generate audio sample for a chord
void gen_sample_fast_chord (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype, float vib_freq, float vib_depth) {
    
    float phase_step = LUT_SIZE * frequency / SAMPLE_RATE;
    float phase_step_3 = LUT_SIZE * (frequency * 1.26) / SAMPLE_RATE;
    float phase_step_5 = LUT_SIZE * (frequency * 1.50) / SAMPLE_RATE;
    
    float vibrato_phase_step = LUT_SIZE * vib_freq / SAMPLE_RATE;
    float vibrato_depth = LUT_SIZE * vib_depth / SAMPLE_RATE;
    
    if (ftype == I2S_STEREO) {
        for (int i = 0; i < num_samples-1; i+=2) {
           
            samples[i] = (int) ( ( (amplitude * sin_table[(int) phase]) + (amplitude * sin_table[(int) phase_3]) + (amplitude * sin_table[(int) phase_5])) / 3);
            samples[i+1] = samples[i];
            
            float modifier = (vibrato_depth * sin_table[(int) vibrato_phase]);

            phase += phase_step + modifier;
            phase_3 += phase_step_3 + modifier;
            phase_5 += phase_step_5 + modifier;
            vibrato_phase += vibrato_phase_step;

            
            if (vibrato_phase > LUT_SIZE) {
                vibrato_phase -= LUT_SIZE;
            }
            if (phase > LUT_SIZE) {
                phase = phase - LUT_SIZE;
            }
            if (phase_3 > LUT_SIZE) {
                phase_3 = phase_3 - LUT_SIZE;
            }
            if (phase_5 > LUT_SIZE) {
                phase_5 = phase_5 - LUT_SIZE;
            }
        }
    }
    else {
        for (int i = 0; i < num_samples; i++) {
            samples[i] = (int) ( ( (amplitude * sin_table[(int) phase]) + (amplitude * sin_table[(int) phase_3]) + (amplitude * sin_table[(int) phase_5])) / 3);
            
            phase += phase_step;
            phase_3 += phase_step_3;
            phase_5 += phase_step_5;

            if (phase > LUT_SIZE) {
                phase = phase - LUT_SIZE;
            }
            if (phase_3 > LUT_SIZE) {
                phase_3 = phase_3 - LUT_SIZE;
            }
            if (phase_5 > LUT_SIZE) {
                phase_5 = phase_5 - LUT_SIZE;
            }
        }
    }
    
}


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
#include "ads1115.h"

#include "sound_files/Xmas_minus_6.h"
#include "sound_files/Xmas_original.h"
#include "sound_files/Xmas_plus_5.h"

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
    .max_amp = 5000
};

float major_scales[12][8] = {
    // C Major
    {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25},
    // C♯ / D♭ Major
    {277.18, 311.13, 349.23, 369.99, 415.30, 466.16, 523.25, 554.37},
    // D Major
    {293.66, 329.63, 369.99, 392.00, 440.00, 493.88, 554.37, 587.33},
    // D♯ / E♭ Major
    {311.13, 349.23, 392.00, 415.30, 466.16, 523.25, 587.33, 622.25},
    // E Major
    {329.63, 369.99, 415.30, 440.00, 493.88, 554.37, 622.25, 659.25},
    // F Major
    {349.23, 392.00, 440.00, 466.16, 523.25, 587.33, 659.25, 698.46},
    // F♯ / G♭ Major
    {369.99, 415.30, 466.16, 493.88, 554.37, 622.25, 698.46, 739.99},
    // G Major
    {392.00, 440.00, 493.88, 523.25, 587.33, 659.25, 739.99, 783.99},
    // G♯ / A♭ Major
    {415.30, 466.16, 523.25, 554.37, 622.25, 698.46, 783.99, 830.61},
    // A Major
    {440.00, 493.88, 554.37, 587.33, 659.25, 739.99, 830.61, 880.00},
    // A♯ / B♭ Major
    {466.16, 523.25, 587.33, 622.25, 698.46, 783.99, 880.00, 932.33},
    // B Major
    {493.88, 554.37, 622.25, 659.25, 739.99, 830.61, 932.33, 987.77}
};

static void show_reading(unsigned long cm) {
    
    if (cm > MAX_RANGE) return; // don't print anything if nothing deteted
    char buf[MAX_RANGE+1];
    memset(buf, '-', sizeof(buf));
    buf[cm] = '|';
    buf[cm+1] = '\0';
    // printf("%s %ld\n", buf, cm);
    // printf("%ld\n", cm);
}

static float convert_ticks_frequency (unsigned long ticks) {
    ticks /= 80;

    float normalized = ((float) ticks - params.min_freq) / (params.max_freq - params.min_freq);
    float curve = 1 - (1 - normalized) * (1 - normalized) * (1 - normalized);
    return params.min_freq + ( curve * (params.max_freq - params.min_freq));
}

static float convert_ticks_frequency_scale (unsigned long ticks, int scale) {
    ticks /= 80;

    float normalized = ((float) ticks - params.min_freq) / (params.max_freq - params.min_freq);
    int note = (int) (15 * normalized);
    float freq = 0;
    if (freq >= 0 && freq < 8) {
        if (note <= 1) {
            note = 1;    
        }
        freq = major_scales[scale][note-1];
    }

    return freq;
}

static float update_frequency(ultrasound_device_t* dev, float prev_freq, int threshold) {

    unsigned long ticks = ultrasound_read_filtered(dev, threshold);
    float ans = prev_freq;

    if (ticks < threshold && ticks != -2) {
        ans = convert_ticks_frequency(ticks);
    }
    
    return ans;

}

static float update_frequency_scale(ultrasound_device_t* dev, float prev_freq, int threshold, int scale) {

    unsigned long ticks = ultrasound_read_filtered(dev, threshold);
    float ans = prev_freq;

    if (ticks < threshold && ticks != -2) {
        ans = convert_ticks_frequency_scale(ticks, scale);
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

static float update_amplitude_scale(ultrasound_device_t* dev, int threshold) {

    unsigned long ticks = ultrasound_read_filtered(dev, threshold);
    ticks /= 80;

    float normalized = ((float) ticks - params.min_freq) / (params.max_freq - params.min_freq);

    if (normalized < 0.5) {
        return params.max_amp;
    }
    
    return 0;

}

static int16_t * update_frequency_song(ultrasound_device_t* dev, int16_t * prev, int threshold) {

    unsigned long ticks = ultrasound_read_filtered(dev, threshold);
    int16_t * freq = prev;

    ticks /= 80;

    float normalized = ((float) ticks - params.min_freq) / (params.max_freq - params.min_freq);
    int note = (int) (9 * normalized);
    
    printf("%ld \n", ticks);
    if (note < 2) {
        freq = (int16_t *) gwav_0->data;
    }
    else if (note < 4) {
        freq = (int16_t *) gwav_6->data;
    }  
    else if (note < 6) {
        freq = (int16_t *) gwav_11->data;
    }

    return freq;

}

uint16_t ads_read_channel_vol(ads_device_t* dev, uint16_t prev_reading) {
    uint16_t conversion_raw = ads_read_channel(dev);
    
    uint16_t converted = ((float) conversion_raw / 27000.0) * 8;

    if (converted > 8) {
        return prev_reading;
    }
    return converted;
}

typedef struct {
    uint16_t vib_depth;
    uint16_t vib_freq;
} vib_params;

vib_params ads_read_channel_vib(ads_device_t* dev, vib_params prev) {

    uint16_t conversion_raw = ads_read_channel(dev);
    uint16_t converted = ((float) conversion_raw / 27000.0) * 15;

    if (converted > 15) {
        return prev;
    }
        
    vib_params params = {converted, 5};
    
    return params;
}

void main(void) {
    timer_init();
    gpio_init();
    uart_init();
    speaker_init();

    ads_init(RATE);
    ads_device_t* dev_vib = ads_new(0b1001000, AIN0);
    ads_device_t* dev_pitch = ads_new(0b1001000, AIN1);

    uint16_t conversion_vib = 0;
    uint16_t conversion_pitch = 0;

    // Initialize ultrasound devices
    ultrasound_device_t* dev_freq = ultrasound_new(GPIO_PB12, GPIO_PB11);
    ultrasound_device_t* dev_amp = ultrasound_new(GPIO_PD22, GPIO_PD21);
    
    // Initialize sound generation
    int duration_ms = 100;
    i2s_frame_type_t ftype = I2S_STEREO;

    // Set aside memory for samples
    int num_samples = count_num_samples(duration_ms, ftype);
    int16_t * samples = malloc(sizeof(int16_t) * num_samples);
    int16_t * new_samples = malloc(sizeof(int16_t) * num_samples);

    // Sound generation params 
    int amp = 1200;
    float freq = 400;
    int amp_dynamic = amp;
    float freq_dynamic = freq;

    // Sample pointers
    int16_t * update_samples = new_samples;
    int16_t * curr_samples = samples; 

    // Chord and scale modes
    bool chord = false;
    bool scale = false;
    bool song = true;

    // Parameters
    vib_params params = {0, 0};
    
    int count = 5000;
    
    if (song) {
        curr_samples = gwav_6->data;
	// double buffer
	update_samples = gwav_6->data;
    }

    else {
        // Initialize
        gen_sample_fast(samples, num_samples, freq, amp, ftype, params.vib_freq, params.vib_depth);
    }
    
    while (true) {
        
        if (song) {

            for (int i = 0; i < gwav_6->n_data - count; i+= count) {
                i2s_stream_playback_nb(&curr_samples[i], count, ftype);
		// single buffer
                //curr_samples = update_frequency_song(dev_freq, update_samples, 50000);
		
		// double buff
                update_samples = update_frequency_song(dev_freq, update_samples, 50000);
 	        curr_samples = update_samples; 

                i2s_await();
            }
            
        }

        else {
            // Generate sample and play sound
            i2s_stream_playback_nb(curr_samples, num_samples, ftype);
                  
            // vib_depth = ads_read_channel_vol(dev_vib, conversion_vib);
	        params = ads_read_channel_vib(dev_vib, params);
            conversion_pitch = ads_read_channel_vol(dev_pitch, conversion_pitch);
                    
            // Update Frequency
            if (scale) {
                freq_dynamic = update_frequency_scale(dev_freq, freq_dynamic, 50000, conversion_pitch);
                amp_dynamic = update_amplitude_scale(dev_amp, 50000);
            }
            else {
                freq_dynamic = update_frequency(dev_freq, freq_dynamic, 50000);
                amp_dynamic = update_amplitude(dev_amp, amp_dynamic, 50000);
            }
        

            // Generate New Sample
            if (chord) {
                gen_sample_fast_chord(update_samples, num_samples, freq_dynamic, amp_dynamic, ftype, params.vib_freq, params.vib_depth);
            }
            else {
                gen_sample_fast(update_samples, num_samples, freq_dynamic, amp_dynamic, ftype, params.vib_freq, params.vib_depth);
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
}


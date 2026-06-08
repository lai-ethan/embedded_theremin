
/*
 * C header of sound data extracted from a wav file.
 *
 * Input file 'Trimmed Xmas_plus_5.wav' converted on date 2026-06-04
 * using version 1.1 of script generate_h.py
 */

#pragma once
#include <stdint.h>

#ifndef _WAV_STRUCT
struct wav_file_t {
    const char *name;
    int frames_per_sec; // sampling frequency in Hz
    int samples_per_frame; // 1=mono, 2=stereo
    int bits_per_sample; // data can be 8, 16, or 32-bit
    int min_val, max_val;
    int n_data;
    union {
        int8_t  *data8;
        int16_t *data16;
        int32_t *data32;
        void *data;
    };
};
#define _WAV_STRUCT
#endif

extern const struct wav_file_t *gwav_11;
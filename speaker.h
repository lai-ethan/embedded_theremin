#ifndef SPEAKER_H
#define SPEAKER_H

void speaker_init (void);

int count_num_samples (int duration_ms, i2s_frame_type_t ftype);

void gen_sample_fast (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype);

void gen_sample_fast_chord (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype);
#endif
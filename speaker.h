#ifndef SPEAKER_H
#define SPEAKER_H

void gen_sample_fast (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype, float *sin_table);

#endif
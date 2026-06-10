#ifndef SPEAKER_H
#define SPEAKER_H

// Initialize Speaker
void speaker_init (void);

// Calculate Number of Samples in Time Duration 
int count_num_samples (int duration_ms, i2s_frame_type_t ftype);

/* Generate Samples for Individual Notes
 
   @param samples      Array to store samples
   @param num_samples  Number of samples
   @param frequency    Frequency to play
   @param amplitude    Amplitude of note
   @param ftype        Type of speaker
   @param vib_freq     Vibrato freq
   @param vib_depth    Vibrato depth 

*/

void gen_sample_fast (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype, float vib_freq, float vib_depth);

/* Generate Samples for Chords
 
   @param samples      Array to store samples
   @param num_samples  Number of samples
   @param frequency    Frequency to play
   @param amplitude    Amplitude of note
   @param ftype        Type of speaker
   @param vib_freq     Vibrato freq
   @param vib_depth    Vibrato depth 
 
 */
void gen_sample_fast_chord (int16_t samples[], int num_samples, float frequency, int amplitude, i2s_frame_type_t ftype, float vib_freq, float vib_depth);
#endif

// This example writes 2 sin waves in succession to a file.
// The first one plays 260hz for 0.5 seconds then immediately plays 500hz for
// 0.5 seconds.

#include "wav.h"
#include <math.h>

// The start_pos and samples represent the sample to start writing at in the
// buffer and how many samples of sin wave to write (length)
uint32_t sin_wave(uint8_t *buffer, const uint32_t start_pos,
                  const uint32_t samples, const double freq,
                  const int32_t amp) {
  for (uint32_t i = 0; i < samples; i++) {
    const double t = (double)(start_pos + i) / sample_rate;
    const int32_t sample = (int32_t)(amp * sin(2.0 * M_PI * freq * t));

    // write_sample automatically handles differences writing in writing stereo
    // and varying bit depths
    write_wav_sample(buffer, start_pos + i, sample);
  }
  return samples;
}

int main() {
  // Init the wav system with 2 channels (stereo), 44100Hz sample rate, 24 bit
  // depth. Defaults are 1, 8000, 16
  if (!wav_init(2, 44100, 24)) {
    printf("wav_init error, quiting\n");
    return 1;
  }

  // Create a byte array for holding the audio buffer
  const size_t total_len = bytes_from_seconds(10);
  uint8_t buffer[total_len] = {};

  // Track how many samples you've written with a playhead
  uint32_t playhead = 0;

  // Write 0.5 seconds of 260hz sin wave into the buffer,
  // starting at sample position 0, 5000000 amplitude
  // NOTE: amplitude values will be much lower for 16 bit depth audio
  playhead += sin_wave(buffer, playhead, 0.5 * sample_rate, 260.0, 5000000);

  // Write 0.5 seconds of 500hz sin wave into the buffer,
  // starting immediately at the end of the first note, 5000000 amplitude
  playhead += sin_wave(buffer, playhead, 0.5 * sample_rate, 500.0, 5005000);

  // Write the buffer to a given filename that is n samples long (playhead in
  // this case)
  write_wav_file("audio.wav", buffer, playhead);

  return 0;
}

# WAV Files in C

This library is meant to be a dead simple header-only C library for working with audio buffers and files in WAV format. Has support for stereo, custom sample rate, up to 32 bit depth.

## Basic Example

```c
#include "wav.h"

#define CHANNELS (2)
#define SAMPLE_RATE (44100)
#define BIT_DEPTH (24)

int main() {
    // Init library context
    wav_init(CHANNELS, SAMPLE_RATE, BIT_DEPTH);

    // Create a byte array for holding the audio buffer
    const size_t total_len = bytes_from_seconds(10);
    uint8_t buffer[total_len] = {};

    // Write data to the buffer
    // This example just plays clicking, but you can see
    // how to write to the buffer
    int16_t sample = 6969;
    const uint32_t samples_to_write = samples_from_seconds(3);
    for (uint32_t i = 0; i < samples_to_write; i++) {
        write_wav_sample(buffer, i, sample);
        sample += 1;
    }

    // Write to file
    write_wav_file("audio.wav", buffer, samples_to_write);
}
```

Also check out the [`examples/`](./examples/) folder.

## Resources

- <https://docs.fileformat.com/audio/wav/>

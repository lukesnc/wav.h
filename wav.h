#ifndef WAV_H
#define WAV_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Lib context
static int16_t channels = 1;
static int32_t sample_rate = 8000;
static int16_t bit_depth = 16;

typedef struct __attribute__((packed)) {
  char riff[4];
  int32_t file_size;
  char type[4];
  char fmt[4];
  int32_t chunk_size;
  int16_t format;
  int16_t channels;
  int32_t sample_rate;
  int32_t bytes_sec;
  int16_t bytes_samp;
  int16_t bits_samp;
  char data_header[4];
  int32_t data_size;
} WavHeader;
const static uint32_t header_size = sizeof(WavHeader);

bool wav_init(const int16_t __channels, const int32_t __sample_rate,
              const int16_t __bit_depth) {
  channels = __channels;
  sample_rate = __sample_rate;
  bit_depth = __bit_depth;
  return true;
}

void write_sample(uint8_t *buffer, const uint32_t pos, const int32_t sample) {
  if (bit_depth > 16) {
    if (channels == 2) {
      // Left channel
      buffer[pos * 6] = (sample & 0xFF);           // Low byte
      buffer[pos * 6 + 1] = (sample >> 8) & 0xFF;  // Middle byte
      buffer[pos * 6 + 2] = (sample >> 16) & 0xFF; // High byte

      // Right channel
      buffer[pos * 6 + 3] = (sample & 0xFF);       // Low byte
      buffer[pos * 6 + 4] = (sample >> 8) & 0xFF;  // Middle byte
      buffer[pos * 6 + 5] = (sample >> 16) & 0xFF; // High byte
    } else {
      buffer[pos * 3] = (sample & 0xFF);           // Low byte
      buffer[pos * 3 + 1] = (sample >> 8) & 0xFF;  // Middle byte
      buffer[pos * 3 + 2] = (sample >> 16) & 0xFF; // High byte
    }
  } else {
    if (channels == 2) {
      // Left channel
      buffer[pos * 4] = (sample & 0xFF);          // Low byte
      buffer[pos * 4 + 1] = (sample >> 8) & 0xFF; // High byte

      // Right channel
      buffer[pos * 4 + 2] = (sample & 0xFF);      // Low byte
      buffer[pos * 4 + 3] = (sample >> 8) & 0xFF; // High byte
    } else {
      buffer[pos * 2] = (sample & 0xFF);          // Low byte
      buffer[pos * 2 + 1] = (sample >> 8) & 0xFF; // High byte
    }
  }
}

uint32_t samples_from_seconds(const uint32_t seconds) {
  return seconds * sample_rate;
}

uint32_t bytes_sample() { return bit_depth / 8 * channels; }

uint32_t bytes_from_seconds(const uint32_t seconds) {
  return seconds * sample_rate * (bit_depth / 8 * channels);
}

uint32_t bytes_from_samples(const uint32_t samples) {
  return samples * (bit_depth / 8 * channels);
}

bool write_wav(const char *filename, const uint8_t *buffer,
               const uint32_t samples) {
  // Build header
  WavHeader header;
  strncpy(header.riff, "RIFF", 4);
  strncpy(header.type, "WAVE", 4);
  strncpy(header.fmt, "fmt ", 4);
  header.chunk_size = 16;
  header.format = 1;
  header.channels = channels;
  header.sample_rate = sample_rate;
  header.bytes_sec = sample_rate * channels * bit_depth / 8;
  header.bytes_samp = bit_depth / 8 * channels;
  header.bits_samp = bit_depth;
  strncpy(header.data_header, "data", 4);
  header.data_size = samples * header.bytes_samp;
  header.file_size = header.data_size + header_size;

  // Write file
  FILE *f = fopen(filename, "w");
  fwrite(&header, 1, header_size, f);
  fwrite(buffer, 1, header.data_size, f);
  fclose(f);

  return true;
}

#endif // WAV_H

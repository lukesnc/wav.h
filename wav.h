// Copyright (c) 2024 Luke Simone
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef WAV_H
#define WAV_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Lib context
static uint16_t channels = 1;
static uint32_t sample_rate = 8000;
static uint16_t bit_depth = 16;

#pragma pack(push, 1)
typedef struct {
  char riff[4];
  uint32_t file_size;
  char description[4];
  char fmt[4];
  uint32_t chunk_size;
  uint16_t format;
  uint16_t channels;
  uint32_t sample_rate;
  uint32_t bytes_sec;
  uint16_t bytes_samp;
  uint16_t bits_samp;
  char data_header[4];
  uint32_t data_size;
} WavHeader;
#pragma pack(pop)

const static uint32_t header_size = sizeof(WavHeader);

bool wav_init(const uint16_t __channels, const uint32_t __sample_rate,
              const uint16_t __bit_depth) {
  channels = __channels;
  sample_rate = __sample_rate;
  bit_depth = __bit_depth;

  // Make sure bit depth is a valid option
  if (bit_depth != 16 && bit_depth != 24 && bit_depth != 32) {
    printf("warning: recommended values for bit depth are [16, 24, 32]\n");
    return false;
  }
  return true;
}

void write_sample(uint8_t *buffer, const uint32_t pos, const int32_t sample) {
  if (bit_depth > 24) {
    if (channels == 2) {
      // Left channel
      buffer[pos * 8] = (sample & 0xFF);           // Low byte
      buffer[pos * 8 + 1] = (sample >> 8) & 0xFF;  // Second byte
      buffer[pos * 8 + 2] = (sample >> 16) & 0xFF; // Third byte
      buffer[pos * 8 + 3] = (sample >> 24) & 0xFF; // High byte

      // Right channel
      buffer[pos * 8 + 4] = (sample & 0xFF);       // Low byte
      buffer[pos * 8 + 5] = (sample >> 8) & 0xFF;  // Second byte
      buffer[pos * 8 + 6] = (sample >> 16) & 0xFF; // Third byte
      buffer[pos * 8 + 7] = (sample >> 24) & 0xFF; // High byte
    } else {
      buffer[pos * 4] = (sample & 0xFF);           // Low byte
      buffer[pos * 4 + 1] = (sample >> 8) & 0xFF;  // Second byte
      buffer[pos * 4 + 2] = (sample >> 16) & 0xFF; // Third byte
      buffer[pos * 4 + 3] = (sample >> 24) & 0xFF; // High byte
    }
  } else if (bit_depth > 16) {
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
  strncpy(header.description, "WAVE", 4);
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
  printf("Wrote %.2f MB to %s\n", (double)header.data_size / (1024 * 1024),
         filename);

  return true;
}

#endif // WAV_H

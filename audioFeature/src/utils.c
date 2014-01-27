// utils.c
// Implementation of utility functions
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

#include "utils.h"

#include <assert.h>
#include <stdint.h>

// #define _BSD_SOURCE
#include <endian.h>

int isFloat(snd_pcm_format_t format) {
  return (format == SND_PCM_FORMAT_FLOAT_LE
      || format == SND_PCM_FORMAT_FLOAT_BE
      || format == SND_PCM_FORMAT_FLOAT64_LE
      || format == SND_PCM_FORMAT_FLOAT64_BE);
}

double pcm_to_double(snd_pcm_format_t format, void* data) {
  int format_bits = snd_pcm_format_width(format);
  int bps = format_bits / 8; /* bytes per sample */
  int big_endian = snd_pcm_format_big_endian(format) == 1;
  int is_unsigned = snd_pcm_format_unsigned(format) == 1;
  int is_float = isFloat(format);

  double value = 0.0;
  double maxval = 1.0;

  if (bps == 1) {
    // no endian problem
    int8_t* vp = (int8_t*) data;
    int8_t vc = *vp;
    if (is_unsigned) {
      assert(format == SND_PCM_FORMAT_U8);
      vc ^= 1U << (format_bits - 1);
    }
    else {
      assert(format == SND_PCM_FORMAT_S8);
    }
    int8_t im = INT8_MAX;
    maxval = (double) im;
    value = (double) vc / maxval;
  }
  else if (bps == 2) {
    uint16_t vcu = *((uint16_t*) data);
    if (big_endian) {
      vcu = be16toh(vcu);
    }
    else {
      assert(snd_pcm_format_little_endian(format) == 1);
      vcu = le16toh(vcu);
    }

    assert(!is_float);
    if (is_unsigned) {
      vcu ^= 1U << (format_bits - 1);
    }
    else {
      assert(snd_pcm_format_signed(format) == 1);
    }

    int16_t im = INT16_MAX;
    maxval = (double) im;
    int16_t vcs = *((int16_t*) &vcu);
    value = (double) vcs / maxval;
  }
  else if (bps == 4) {
    uint32_t vcu = *((uint32_t*) data);
    if (big_endian) {
      vcu = be32toh(vcu);
    }
    else {
      assert(snd_pcm_format_little_endian(format) == 1);
      vcu = le32toh(vcu);
    }

    if (!is_float) {
      if (is_unsigned) {
        vcu ^= 1U << (format_bits - 1);
      }
      else {
        assert(snd_pcm_format_signed(format) == 1);
      }

      int32_t im = INT32_MAX;
      maxval = (double) im;
      int32_t vcs = *((int32_t*) &vcu);
      value = (double) vcs / maxval;
    }
    else {
      float vcs = *((float*) &vcu);
      value = (double) vcs;
    }
  }
  else if (bps == 8) {
    assert(is_float);
    uint64_t vcu = *((uint64_t*) data);
    if (big_endian) {
      vcu = be64toh(vcu);
    }
    else {
      assert(snd_pcm_format_little_endian(format) == 1);
      vcu = le64toh(vcu);
    }

    value = *((double*) &vcu);
  }
  else {
    // bps == 3 ?? ignore
    // TODO : fix it
    assert(0);
  }

  return value;
}

int double_to_pcm(snd_pcm_format_t format, double value, void* data) {
  assert(value <= 1.0 && value >= -1.0);

  int format_bits = snd_pcm_format_width(format);
  int bps = format_bits / 8; /* bytes per sample */
  int big_endian = snd_pcm_format_big_endian(format) == 1;
  int is_unsigned = snd_pcm_format_unsigned(format) == 1;
  int is_float = isFloat(format);

  double maxval = 1.0;

  if (bps == 1) {
    int8_t im = INT8_MAX;
    maxval = (double) im;
    int8_t vc = (int8_t) (maxval * value);

    if (is_unsigned) {
      assert(format == SND_PCM_FORMAT_U8);
      vc ^= 1U << (format_bits - 1);
    }
    else {
      assert(format == SND_PCM_FORMAT_S8);
    }

    // no endian problem

    int8_t* vp = (int8_t*) data;
    *vp = vc;
  }
  else if (bps == 2) {
    assert(!is_float);

    int16_t im = INT16_MAX;
    maxval = (double) im;
    int16_t vcs = (int16_t) (maxval * value);
    uint16_t vcu = *((uint16_t*) &vcs);

    if (is_unsigned) {
      vcu ^= 1U << (format_bits - 1);
    }
    else {
      assert(snd_pcm_format_signed(format) == 1);
    }

    if (big_endian) {
      vcu = htobe16(vcu);
    }
    else {
      assert(snd_pcm_format_little_endian(format) == 1);
      vcu = htole16(vcu);
    }

    uint16_t* vp = (uint16_t*) data;
    *vp = vcu;
  }
  else if (bps == 4) {
    uint32_t vcu;

    if (!is_float) {
      int32_t im = INT32_MAX;
      maxval = (double) im;
      int32_t vcs = (int32_t) (maxval * value);
      vcu = *((uint32_t*) &vcs);

      if (is_unsigned) {
        vcu ^= 1U << (format_bits - 1);
      }
      else {
        assert(snd_pcm_format_signed(format) == 1);
      }
    }
    else {
      float vcf = (float) value;
      vcu = *((uint32_t*) &vcf);
    }

    if (big_endian) {
      vcu = htobe32(vcu);
    }
    else {
      assert(snd_pcm_format_little_endian(format) == 1);
      vcu = htole32(vcu);
    }

    uint32_t* vp = (uint32_t*) data;
    *vp = vcu;
  }
  else if (bps == 8) {
    assert(is_float);
    uint64_t vcu = *((uint64_t*) &value);

    if (big_endian) {
      vcu = be64toh(vcu);
    }
    else {
      assert(snd_pcm_format_little_endian(format) == 1);
      vcu = le64toh(vcu);
    }

    uint64_t* vp = (uint64_t*) data;
    *vp = vcu;
  }
  else {
    // bps == 3 ?? ignore
    // TODO : fix it
    assert(0);
  }

  return 0;
}

unsigned long int read_file(FILE* file, unsigned long int count,
    unsigned int channels, snd_pcm_format_t format,
    double*** data) {
  int rc; // return code

  rc = snd_pcm_format_physical_width(format);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    exit(EXIT_FAILURE);
  }
  size_t sampleSize = rc / 8; // sample count in bytes

  // Allocate space
  *data = malloc(sizeof(double*) * channels);
  unsigned int c;
  for (c = 0; c < channels; c++) {
    (*data)[c] = malloc(sizeof(double) * sampleSize * count);
  }

  // Read data
  size_t bufferSize = sampleSize * channels;
  char* buffer = malloc(bufferSize);
  unsigned long int readCount = 0;
  for (readCount = 0; readCount < count; readCount++) {
    size_t rs = fread(buffer, 1, bufferSize, file);
    if (rs < bufferSize) { // !NOTE : Ignore incomplete samples
      break;
    }
    for (c = 0; c < channels; c++) {
      (*data)[c][readCount] = pcm_to_double(format, buffer + (c * sampleSize));
    }
  }
  free(buffer);

  return readCount;
}

void free_data(double** data, unsigned int channels) {
  int i;
  for (i = 0; i < channels; i++) {
    free(data[i]);
  }
  free(data);
}

unsigned long int write_file(FILE* file, unsigned long int count,
    unsigned int channels, snd_pcm_format_t format, double** data) {
  int rc; // return code

  rc = snd_pcm_format_physical_width(format);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    exit(EXIT_FAILURE);
  }
  size_t sampleSize = rc / 8; // sample count in bytes

  // Write data
  size_t bufferSize = sampleSize * channels;
  char* buffer = malloc(bufferSize);
  unsigned long int writeCount = 0;
  unsigned int c;
  for (writeCount = 0; writeCount < count; writeCount++) {
    for (c = 0; c < channels; c++) {
      double_to_pcm(format, data[c][writeCount], buffer + (c * sampleSize));
    }

    size_t rs = fwrite(buffer, 1, bufferSize, file);
    if (rs < bufferSize) { // !NOTE : Ignore incomplete write 
      break;
    }
  }
  free(buffer);

  return writeCount;
}

unsigned long int pcm_size(FILE* file, unsigned int channels,
    snd_pcm_format_t format) {
  fseek(file, 0L, SEEK_END);
  long size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  return size / (channels * snd_pcm_format_physical_width(format) / 8);
}

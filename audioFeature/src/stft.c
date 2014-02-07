// stft.c
// Short Time Fourier Transform
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "stft.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gsl/gsl_fft_complex.h>

double hann_window(int n, int size) {
  return 0.5 * (1 - cos(2 * M_PI * n / (size - 1)));
}

double (*window)(int, int) = hann_window;

TimeFreq* alloc_tf(int window_size, int nos) {
  TimeFreq* tf = malloc(sizeof(TimeFreq));
  if (tf == NULL) {
    return NULL;
  }

  Spectra* data = malloc(nos * sizeof(Spectra));
  if (data == NULL) {
    free(tf);
    return NULL;
  }

  int i;
  for (i = 0; i < nos; i++) {
    data[i] = malloc(2 * window_size * sizeof(double));
    if (data[i] == NULL) {
      int j;
      for (j = 0; j < i; j++) {
        free(data[j]);
      }
      free(data);
      free(tf);
      return NULL;
    }
  }

  tf->window_size = window_size;
  tf->nos = nos;
  tf->data = data;

  return tf;
}

void free_tf(TimeFreq* tf) {
  int i;
  for (i = 0; i < tf->nos; i++) {
    free(tf->data[i]);
  }
  free(tf->data);
  free(tf);
}

Spectra get_spectra(TimeFreq* tfr, int index) {
  return tfr->data[index];
}

inline double get_real(Spectra spec, int index) {
  return spec[index * 2];
}

inline double get_imag(Spectra spec, int index) {
  return spec[index * 2 + 1];
}

inline double get_magnitude(Spectra spec, int index) {
  return sqrt(spec[index * 2] * spec[index * 2]
      + spec[index * 2 + 1] * spec[index * 2 + 1]);
}

inline double get_phase(Spectra spec, int index) {
  return atan2(spec[index * 2 + 1], spec[index * 2]);
}

inline void set_value(Spectra spec, int index, double real,
    double imag) {
  spec[index * 2] = real;
  spec[index * 2 + 1] = imag;
}

inline void set_by_polar(Spectra spec, int index, double mag,
    double phase) {
  set_value(spec, index, mag * cos(phase), mag * sin(phase));
}

int number_of_spectrum(int length, int window_size, int shift) {
  return (length - window_size) / shift + 1;
}

void stft(double* data, int length, int window_size, int shift,
    TimeFreq* result) {
  result->window_size = window_size;
  result->shift = shift;
  result->nos = number_of_spectrum(length, window_size, shift);
  int i;
  for (i = 0; i < result->nos; i++) {
    Spectra s = get_spectra(result, i);
    double* offset = data + i * shift;
    int j;
    for (j = 0; j < window_size; j++) {
      set_value(s, j, offset[j] * window(j, window_size), 0);
    }
    gsl_fft_complex_radix2_forward(s, 1, window_size);
  }
}

void istft(TimeFreq* tf, double* data) {
  int i, j;
  for (i = 0; i < tf->nos * tf->shift + tf->window_size; i++) {
    data[i] = 0;
  }

  size_t spsize = 2 * tf->window_size * sizeof(double);
  Spectra buf = malloc(spsize);

  double scale = 0.5 * tf->window_size / tf->shift;

  for (i = 0; i < tf->nos; i++) {
    memcpy(buf, tf->data[i], spsize);
    gsl_fft_complex_radix2_inverse(buf, 1, tf->window_size);
    for (j = 0; j < tf->window_size; j++) {
      data[i * tf->shift + j] += get_real(buf, j) / scale;
      // Ignore imaginary part
    }
  }

  free(buf);
}


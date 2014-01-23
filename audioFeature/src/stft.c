// stft.c
// Short Time Fourier Transform
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "stft.h"

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_fft_complex.h>

double hann_window(size_t n, size_t size) {
  return 0.5 * (1 - cos(2 * M_PI * n / (size - 1)));
}

double (*window)(size_t, size_t) = hann_window;

TimeFreq* alloc_tf(size_t window_size, size_t nos) {
  TimeFreq* tf = malloc(sizeof(TimeFreq));
  if (tf == NULL) {
    return NULL;
  }

  Spectra* data = malloc(nos * sizeof(Spectra));
  if (data == NULL) { 
    free(tf);
    return NULL;
  }

  size_t i;
  for (i = 0; i < nos; i++) {
    data[i] = malloc(2 * window_size * sizeof(double));
    if (data[i] == NULL) {
      size_t j;
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
  size_t i;
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

void set_value(Spectra spec, int index, double real, double imag) {
  spec[index * 2] = real;
  spec[index * 2 + 1] = imag;
}

size_t number_of_spectrum(size_t length, size_t window_size, size_t shift) {
  return (length - window_size) / shift + 1;
}

void stft(double* data, size_t length, size_t window_size,
    size_t shift, TimeFreq* result) {
  result->window_size = window_size;
  result->shift = shift;
  result->nos = number_of_spectrum(length, window_size, shift);
  int i;
  for (i = 0; i < result->nos; i++) {
    Spectra s = get_spectra(result, i);
    double* offset = data + i * shift;
    size_t j;
    for (j = 0; j < window_size; j++) {
      set_value(s, j, offset[j] * window(j, window_size), 0);
    }
    gsl_fft_complex_radix2_forward(s, 1, window_size);
  }
}


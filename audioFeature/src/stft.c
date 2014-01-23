// stft.c
// Short Time Fourier Transform
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_fft_complex.h>

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

double hann_window(size_t n, size_t size) {
  return 0.5 * (1 - cos(2 * M_PI * n / (size - 1)));
}

double (*window)(size_t, size_t) = hann_window;

void stft(double* data, size_t length, size_t window_size,
    size_t shift, double** result) {
  int i;
  for (i = 0; i * shift + window_size < length; i++) {
    double* offset = data + i * shift;
    size_t j;
    for (j = 0; j < window_size; j++) {
      REAL(result[i], j) = offset[j] * window(j, window_size);
      IMAG(result[i], j) = 0;
    }
    gsl_fft_complex_radix2_forward(result[i], 1, window_size);
  }
}


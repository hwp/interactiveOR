// spectral_sub.c
// Spectral Subtraction
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "spectral_sub.h"

#include <math.h>

void cal_avg_spectrum(TimeFreq* noise, double* avg_spectrum) {
  int i, j;
  for (j = 0; j < noise->window_size / 2 + 1; j++) {
    avg_spectrum[j] = 0;
  }

  for (i = 0; i < noise->nos; i++) {
    for (j = 0; j < noise->window_size / 2 + 1; j++) {
      avg_spectrum[j] += get_magnitude(get_spectra(noise, i), j)
        / noise->nos;
    }
  }
}

void mag_spectral_sub(double* avg_spectrum, TimeFreq* original,
    TimeFreq* result) {
  result->window_size = original->window_size;
  result->shift = original->shift;
  result->nos = original->nos;

  int i, j;
  for (i = 0; i < original->nos; i++) {
    for (j = 0; j < original->window_size / 2 + 1; j++) {
      double re = get_real(get_spectra(original, i), j);
      double im = get_imag(get_spectra(original, i), j);
      double mago = get_magnitude(get_spectra(original, i), j);
      double magn = mago - avg_spectrum[j];
      double scale;
      if (magn <= 0 || mago == 0) {
        scale = 0;
      }
      else {
        scale = magn / mago;
      }

      set_value(get_spectra(result, i), j, re * scale,
          im * scale);
      // Assume time-domain signal is real,
      // thus freq-domain vector is symmetrical (conjugation)
      if (j > 0 && j < original->window_size / 2) {
        set_value(get_spectra(result, i),
            original->window_size - j, re * scale, -im * scale);
      }
    }
  }
}


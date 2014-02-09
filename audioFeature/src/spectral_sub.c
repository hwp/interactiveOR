// spectral_sub.c
// Spectral Subtraction
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "spectral_sub.h"

#include <math.h>
#include <assert.h>

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
  assert(result->window_size == original->window_size);
  assert(result->nos == original->nos);

  result->shift = original->shift;

  int i, j;
  for (i = 0; i < original->nos; i++) {
    for (j = 0; j < original->window_size / 2 + 1; j++) {
      double mag = get_magnitude(get_spectra(original, i), j)
        - avg_spectrum[j];
      if (mag < 0.0) {
        mag = 0.0;
      }
      double phase = get_phase(get_spectra(original, i), j);

      set_by_polar(get_spectra(result, i), j, mag, phase);

      // Assume time-domain signal is real,
      // thus freq-domain vector is symmetrical (conjugation)
      if (j > 0 && j < original->window_size / 2) {
        set_by_polar(get_spectra(result, i),
            original->window_size - j, mag, -phase);
      }
    }
  }
}


// som_denoise.c
// Denoising using SOM
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "som_denoise.h"

#include <stdlib.h>
#include <assert.h>

void som_denoise(SOM* net, TimeFreq* original, TimeFreq* result) {
  assert(net->dims == original->window_size / 2 + 1);
  assert(result->window_size == original->window_size);
  assert(result->nos == original->nos);
  
  result->shift = original->shift;

  int i, j;
  double* mspec = malloc(net->dims * sizeof(double));

  for (i = 0; i < original->nos; i++) {
    Spectra s = get_spectra(original, i);
    for (j = 0; j < net->dims; j++) {
      mspec[j] = get_magnitude(s, j);
    }

    int nid = som_map(net, mspec);
    double* w = get_weight_by_id(net, nid);

    for (j = 0; j < net->dims; j++) {
      double mag = w[j];
      double phase = get_phase(s, j);

      set_by_polar(get_spectra(result, i), j, mag, phase);
      // Assume time-domain signal is real,
      // thus freq-domain vector is symmetrical (conjugation)
      if (j > 0 && j < original->window_size / 2) {
        set_by_polar(get_spectra(result, i),
            original->window_size - j, mag, -phase);
      }
    }
  }

  free(mspec);
}


// filter.c
// Time-Frequency Domain Filter
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "filter.h"

#include <assert.h>

void do_filter(filter_func f, void* args, TimeFreq* source,
    TimeFreq* result) {
  assert(result->window_size == source->window_size);
  assert(result->nos == source->nos);

  result->shift = source->shift;

  int i;
  for (i = 0; i < source->nos; i++) {
    f(source->window_size, args, get_spectra(source, i),
        get_spectra(result, i));
  }
}


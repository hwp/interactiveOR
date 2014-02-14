// filter.h
// Time-Frequency Domain Filter
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef FILTER_H_
#define FILTER_H_

#include "stft.h"

typedef void (*filter_func)(int window_size, void* args,
    Spectra source, Spectra result);

/**
 * Filter the source signal with the given filter function
 * Memory for result shall be allocated in advance.
 */
void do_filter(filter_func f, void* args, TimeFreq* source,
    TimeFreq* result);

#endif  // FILTER_H_


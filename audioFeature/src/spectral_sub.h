// spectral_sub.h
// Spectral Substraction
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef SPECTRAL_SUB_H_
#define SPECTRAL_SUB_H_

#include "stft.h"

/**
 * Calculate the avarage spctrum of noise.
 * The memory of avg_spectrum shall be allocated in advance.
 * Its length is
 *   window_size / 2 + 1.
 */
void cal_avg_spectrum(TimeFreq* noise, double* avg_spectrum);

/**
 * Magnitude spectrum substraction
 */
void mag_spectral_sub(double* avg_spectrum, TimeFreq* original,
    TimeFreq* result);

#endif  // SPECTRAL_SUB_H_

// som_denoise.h
// Denoising using SOM
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef SOM_DENOISE_H_
#define SOM_DENOISE_H_

#include "som.h"
#include "stft.h"

/**
 * Map spectra to SOM nodes and reconstruct to known spetrca.
 * Memory for result should be allocated in advance.
 */
void som_denoise(SOM* net, TimeFreq* original, TimeFreq* result);

#endif  // SOM_DENOISE_H_


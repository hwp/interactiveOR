// stft.h
// Short Time Fourier Transform
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef STFT_H_
#define STFT_H_

#include <stdlib.h>

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i) + 1])

/**
 * A spectra is a vector of complex numbers.
 * It is a result of a short-time dft of a signal.
 * Real and imaginary parts are placed in alternate neighboring
 * elements (see fft in GSL).
 */
typedef double* Spectra;

/**
 * Time-Frequency Representation
 */
typedef struct {
  size_t window_size;
  size_t shift;
  size_t nos;       // Number of Spectrum (Windows) 
  
  Spectra* data;
} TimeFreq;

/**
 * Allocate memory for TimeFreq.
 * Only window_size and nos are initiated.
 */
TimeFreq* alloc_tf(size_t window_size, size_t nos);

void free_tf(TimeFreq* tfr);

Spectra get_spectra(TimeFreq* tfr, int index);

double get_real(Spectra spec, int index);
double get_imag(Spectra spec, int index);
void set_value(Spectra spec, int index, double real, double imag);

size_t number_of_spectrum(size_t length, size_t window_size, size_t shift);

/**
 * Calculate the Short Time Fourier Transform of the signal data.
 * Hann Windows is used.
 * Memory for result should be allocated before calling the
 * function. 
 * The size of result is number of windows * (2 * size of window). 
 */
void stft(double* data, size_t length, size_t window_size,
    size_t shift, TimeFreq* result);
 
/**
 * Reconstruct the time-domain signal using ifft and
 * overlap and add.
 */
void istft(TimeFreq* tf);

#endif  // STFT_H_

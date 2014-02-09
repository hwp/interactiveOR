// stft.h
// Short Time Fourier Transform
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef STFT_H_
#define STFT_H_

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
  int window_size;
  int shift;
  int nos;       // Number of Spectrum (Windows) 
  
  Spectra* data;
} TimeFreq;

/**
 * Allocate memory for TimeFreq.
 * Only window_size and nos are initiated.
 */
TimeFreq* alloc_tf(int window_size, int nos);

void free_tf(TimeFreq* tfr);

Spectra get_spectra(TimeFreq* tfr, int index);

double get_real(Spectra spec, int index);
double get_imag(Spectra spec, int index);
double get_magnitude(Spectra spec, int index);
double get_phase(Spectra spec, int index);
void set_value(Spectra spec, int index, double real, double imag);
void set_by_polar(Spectra spec, int index, double mag, double phase);

int number_of_spectrum(int length, int window_size, int shift);

/**
 * Calculate the Short Time Fourier Transform of the signal data.
 * Hann Windows is used.
 * Memory for result should be allocated before calling the
 * function. 
 * The size of result is number of windows * (2 * size of window). 
 */
void stft(double* data, int length, int window_size,
    int shift, TimeFreq* result);

/**
 * Calculate the length of inverse transformation
 *   (time domain signal)
 */
int istft_size(TimeFreq* tf);

/**
 * Inverse STFT.
 * Reconstruct the time-domain signal using ifft and
 * overlap and add.
 * Memory for the data (e.g. time-domain signal) should be allocated
 * before calling this function. Its length shall be 
 *   (tf->nos - 1) * tf->shift + tf->window_size.
 *   see istft_size()
 */
void istft(TimeFreq* tf, double* data);

#endif  // STFT_H_

// stft.h
// Short Time Fourier Transform
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef STFT_H_
#define STFT_H_

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

/**
 * Calculate the Short Time Fourier Transform of the signal data.
 * Hann Windows is used.
 * Memory for result should be allocated before calling the
 * function. 
 * The size of result is number of windows x (2 x size of window). * Each row of result is a STFT at a time, with real and imaginary
 * parts placed in alternate neighboring elements (see fft in GSL).
 */
void stft(double* data, size_t length, size_t window_size,
    size_t step, double** result);
 
#endif  // STFT_H_

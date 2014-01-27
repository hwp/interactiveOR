// gsltest.c
// Test GSL Functions.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_rng.h>

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

int main (int argc, char** argv) {
  size_t size = 128;
  double* data = malloc(size * 2 * sizeof(double));

  size_t i;
  for (i = 0; i < size; i++) {
    REAL(data, i) = i;
    IMAG(data, i) = 2.0 / (i + 1);
  }

  for (i = 0; i < size; i++) {
    printf("%g + i%g\n", REAL(data, i), IMAG(data, i));
  }
  printf("\n");

  gsl_fft_complex_radix2_forward(data, 1, size);

  for (i = 0; i < size; i++) {
    printf("%g + i%g\n", REAL(data, i), IMAG(data, i));
  }
  printf("\n");

  gsl_fft_complex_radix2_inverse(data, 1, size);

  for (i = 0; i < size; i++) {
    printf("%g + i%g\n", REAL(data, i), IMAG(data, i));
  }
  printf("\n");
  
  free(data);
  
  gsl_rng_env_setup();
  gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
  long seed = time(NULL) * getpid();
  gsl_rng_set(r, seed);

  for (i = 0; i < 10; i++) {
    printf("%g\n", gsl_rng_uniform(r));
  }
  
  gsl_rng_free(r);

  return 0 ;
}


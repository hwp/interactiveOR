// somtest.c
// Test SOM with simple data
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "som.h"

#include <assert.h>

#include <gsl/gsl_rng.h>

#define DIMS 2
#define NSAMPLE 1000
#define ROWS 6
#define COLS 6
#define ITERS 100
#define FILE_NAME "som.temp"


int main(int argc, char** argv) {
  int i, j, k;

  // Generating training data
  double* data = malloc(NSAMPLE * DIMS * sizeof(double));

  gsl_rng_env_setup();
  gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
  long seed = time(NULL) * getpid();
  gsl_rng_set(r, seed);

  printf("# name: train\n");
  printf("# type: matrix\n");
  printf("# rows: %d\n", NSAMPLE);
  printf("# columns: %d\n", DIMS);
  for (i = 0; i < NSAMPLE; i++) {
    double x = gsl_rng_uniform(r);
    double y = gsl_rng_uniform(r);
    y = y * 2 + x;

    data[i * DIMS] = x;
    data[i * DIMS + 1] = y;
    printf("%g %g\n", x, y);
  }

  gsl_rng_free(r);

  // Train SOM
  SOM* net = som_alloc(ROWS, COLS, DIMS);
  som_train(net, data, NSAMPLE, ITERS);
  free(data);

  // Test Save and Load
  FILE* file = fopen(FILE_NAME, "w");
  assert(file != NULL);
  som_save(net, file);
  fclose(file);

  file = fopen(FILE_NAME, "r");
  assert(file != NULL);
  SOM* net2 = som_load(file);
  fclose(file);
  remove(FILE_NAME);

  assert(net->rows == net2->rows);
  assert(net->cols == net2->cols);
  assert(net->dims == net2->dims);

  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < COLS; j++) {
      double* w1 = get_weight(net, i, j);
      double* w2 = get_weight(net2, i, j);
      for (k = 0; k < DIMS; k++) {
        assert(w1[k] == w2[k]);
      }
    }
  }

  // Output weights
  printf("# name: weights\n");
  printf("# type: matrix\n");
  printf("# rows: %d\n", ROWS * COLS);
  printf("# columns: %d\n", DIMS);
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < COLS; j++) {
      double* w = get_weight(net, i, j);
      for (k = 0; k < DIMS; k++) {
        printf("%g ", w[k]);
      }
      printf("\n");
    }
  }

}


// somtest.c
// Test SOM with simple data
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "som.h"

#include <unistd.h>
#include <assert.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#define DIMS 2
#define FILE_NAME "som.temp"

int main(int argc, char** argv) {
  int i, j, k;

  int showhelp = 0;

  int rows = 6;
  int cols = 6;
  int iters = 100;
  int nsample = 1000;

  int opt;
  while ((opt = getopt(argc, argv, "hr:c:i:n:")) != -1) {
    switch (opt) {
      case 'r':
        rows = atoi(optarg);
        break;
      case 'c':
        cols = atoi(optarg);
        break;
      case 'i':
        iters = atoi(optarg);
        break;
      case 'n':
        nsample = atoi(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

  if (showhelp) {
    fprintf(stderr, "Usage: %s [-r rows] [-c columns] "
        "[-i iterations] [-n #samples]\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  // Generating training data
  double* data = malloc(nsample * DIMS * sizeof(double));

  gsl_rng_env_setup();
  gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
  long seed = time(NULL) * getpid();
  gsl_rng_set(r, seed);

  printf("plot '-' title 'samples' w dots, '-' title 'nodes' with p ps 2 lw 2\n");
  for (i = 0; i < nsample; i++) {
    double x = gsl_rng_uniform(r) - 0.5;
    double y = gsl_rng_uniform(r) - 0.5;
    //double y = gsl_ran_exponential(r, 1.0);
    y = y * 2 + x;
/*    if (x >= 0.0) {
      x += 1;
    }
    if (y >= 0.0) {
      y += 1;
    } */

    data[i * DIMS] = x;
    data[i * DIMS + 1] = y;
    printf("%g %g\n", x, y);
  }
  printf("e\n");

  gsl_rng_free(r);

  // Train SOM
  SOM* net = som_alloc(rows, cols, DIMS);
  som_train(net, data, nsample, iters);
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

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      double* w1 = get_weight(net, i, j);
      double* w2 = get_weight(net2, i, j);
      for (k = 0; k < DIMS; k++) {
        assert(w1[k] == w2[k]);
      }
    }
  }

  // Output weights
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      double* w = get_weight(net, i, j);
      for (k = 0; k < DIMS; k++) {
        printf("%g ", w[k]);
      }
      printf("\n");
    }
  }
  printf("e\n");
}


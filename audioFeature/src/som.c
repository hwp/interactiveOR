// som.c
// Self Organizing Map
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "som.h"

#include <stdlib.h>
#include <math.h>

// Euclidean Distance
double f_euc_dis(double* f1, double* f2, size_t size) {
  double s = 0;
  size_t i;
  // TODO floating-point summation problem ignored.
  // Assume size is small.
  for (i = 0; i < size; i++) {
    s += (f1[i] - f2[i]) * (f1[i] - f2[i]);
  }

  return sqrt(s);
}

// Feature Distance
double (*fdistance)(double*, double*, size_t) = f_euc_dis;


double* get_weight(SOM* net, int row, int col) {
  return net->weight + (row * net->cols + col) * (s)->dims;
}

SOM* som_alloc(size_t rows, size_t cols, size_t dims) {
  double* w = malloc(rows * cols * dims * sizeof(double));
  if (w == NULL) {
    return NULL;
  }

  SOM* r = malloc(sizeof(SOM));
  if (r == NULL) {
    free(w);
    return NULL;
  }
  
  r->rows = rows;
  r->cols = cols;
  r->dims = dims;
  r->weight = w;

  return r;
}

void som_free(SOM* net) {
  free(net->weight);
  free(net);
}

int som_map(SOM* net, double* data) {
  size_t i, j;
  double mind = INFINITY;
  int r = 0;
  for (i = 0; i < net->rows; i++) {
    for (j = 0; j < net->cols; j++) {
      double d = fdistance(get_weight(net, i, j), data, net->dims);
      if (d < mind) {
        mind = d;
        r = i * net->cols + j;
      }
    }
  }

  return r;
}


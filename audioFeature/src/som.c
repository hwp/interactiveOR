// som.c
// Self Organizing Map
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "som.h"

#include <stdlib.h>
#include <math.h>

#include <gsl/gsl_rng.h>

#define ALPHA_0 1.0
#define SIGMA_0 3.0
#define SIGMA_INF 0.5
#define K       10.0

// Euclidean Distance
double f_euc_dis(double* f1, double* f2, int size) {
  double s = 0;
  int i;
  // TODO floating-point summation problem ignored.
  // Assume size is small.
  for (i = 0; i < size; i++) {
    s += (f1[i] - f2[i]) * (f1[i] - f2[i]);
  }

  return sqrt(s);
}

// Feature Distance
double (*fdistance)(double*, double*, int) = f_euc_dis;

double* get_weight(SOM* net, int row, int col) {
  return get_weight_by_id(net, row * net->cols + col);
}

double* get_weight_by_id(SOM* net, int id) {
  return net->weight + id * net->dims;
}

SOM* som_alloc(int rows, int cols, int dims) {
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

double learn_rate(int noi, int total) {
  double c = 0.01 * total;
  return ALPHA_0 * c / (c + noi);
}

/**
 * n(d, i, t) 
 *   = (k^2 * exp(-d^2/s^2) + exp(-d^2/(k^2*s^2))) / (k^2 - 1);
 */
double neighborhood(int dx, int dy, int noi, int total) {
  double s = SIGMA_0 - (SIGMA_0 - SIGMA_INF) * noi / total;
  double s2 = s * s;
  double k2 = K * K;
  double d2 = dx * dx + dy * dy;
  return k2 / (k2 - 1) * exp(-d2 / s2) -
    1 / (k2 - 1) * exp(-d2 / (k2 * s2));
}

void update_weights(SOM* net, int bmu, double* data, int noi,
    int total) {
  int br = bmu / net->cols;
  int bc = bmu % net->cols;
  double a = learn_rate(noi, total);

  int i, j, k;
  for (i = 0; i < net->rows; i++) {
    for (j = 0; j < net->cols; j++) {
      double f = neighborhood(i - br, j - bc, noi, total) * a;
      double* w = get_weight(net, i, j);
      for (k = 0; k < net->dims; k++) {
        w[k] += f * (data[k] - w[k]);
      }
    }
  }
}

void som_train(SOM* net, double* data, int length, int iters) {
  int i, j, k;

  // Random initial value ranges in the sample range.
  double* smin = malloc(net->dims * sizeof(double));
  double* smax = malloc(net->dims * sizeof(double));
  for (i = 0; i < net->dims; i++) {
    smin[i] = INFINITY;
    smax[i] = -INFINITY;
    for (j = 0; j < length; j++) {
      double x = data[j * net->dims + i];
      if (x < smin[i]) {
        smin[i] = x;
      }
      if (x > smax[i]) {
        smax[i] = x;
      }
    }
  }

  gsl_rng_env_setup();
  gsl_rng* r = gsl_rng_alloc(gsl_rng_default);

  for (i = 0; i < net->rows; i++) {
    for (j = 0; j < net->cols; j++) {
      double* w = get_weight(net, i, j);
      for (k = 0; k < net->dims; k++) {
        w[k] = smin[k] + (smax[k] - smin[k]) * gsl_rng_uniform(r);
      }
    }
  }

  free(smin);
  free(smax);
  gsl_rng_free(r);

  // Train 
  for (i = 0; i < iters; i++) {
    fprintf(stderr, "Iteration #%d (learn_rate = %g,"
        " neighbor@1,2,3 = [%g, %g, %g])\n",
        i, learn_rate(i, iters), neighborhood(0, 1, i, iters),
        neighborhood(0, 2, i, iters), neighborhood(0, 3, i, iters));
    for (j = 0; j < length; j++) {
      double* sample = data + j * net->dims;
      // Best matching unit
      int bmu = som_map(net, sample);
      // Update weights
      update_weights(net, bmu, sample, i, iters);
    }
  }
}

int som_map(SOM* net, double* data) {
  int i, j;
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

int som_save(SOM* net, FILE* file) {
  // FIXME : Endianness ignored
  size_t r = 0;
  r += fwrite(&(net->rows), sizeof(int), 1, file);
  r += fwrite(&(net->cols), sizeof(int), 1, file);
  r += fwrite(&(net->dims), sizeof(int), 1, file);
  if (r != 3) {
    return -1;
  }
  
  int nmem = net->rows * net->cols * net->dims; 
  r = fwrite(net->weight, sizeof(double), nmem, file);
  if (r != nmem) {
    return -1;
  }

  return 0;
}

SOM* som_load(FILE* file) {
  int rows, cols, dims;
  size_t r = 0;
  r += fread(&rows, sizeof(int), 1, file);
  r += fread(&cols, sizeof(int), 1, file);
  r += fread(&dims, sizeof(int), 1, file);
  if (r != 3) {
    return NULL;
  }

  SOM* net = som_alloc(rows, cols, dims);
  if (net == NULL) {
    return NULL;
  }
  
  int nmem = net->rows * net->cols * net->dims; 
  r = fread(net->weight, sizeof(double), nmem, file);
  if (r != nmem) {
    som_free(net);
    return NULL;
  }

  return net;
}


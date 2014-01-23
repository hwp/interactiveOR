// som.h
// Self Organizing Map
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef SOM_H_
#define SOM_H_

#include <stdlib.h>

typedef struct {
  size_t rows;  // Number of rows
  size_t cols;  // Number of Columns
  size_t dims;  // Input Dimension

  double* weight;
} SOM;

double* get_weight(SOM* net, int row, int col);

/**
 * Allocate memory for SOM.
 * Set rows, cols and dims. However, weight is not initialized.
 */
SOM* som_alloc(size_t rows, size_t cols, size_t dims);

/**
 * Free the allocated memory
 */
void som_free(SOM* net);

/**
 * Train with given data (dimension = length * net->dims).
 * The memory for net should be allocated before calling
 * this function.
 * Whole training data will be traversed iters times.
 */
void som_train(SOM* net, double* data, size_t length,
    size_t iters);

/**
 * Given on sample of data (dimension = net->dims).
 * maps to a neuron.
 * return the ID of the neuron, that is 
 *   row * # of columns + colummn.
 */
int som_map(SOM* net, double* data);

#endif  // SOM_H_


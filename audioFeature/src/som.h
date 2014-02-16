// som.h
// Self Organizing Map
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef SOM_H_
#define SOM_H_

#include <stdio.h>

typedef struct {
  int rows;  // Number of rows
  int cols;  // Number of Columns
  int dims;  // Input Dimension

  double* weight;
} SOM;

double* get_weight(SOM* net, int row, int col);
double* get_weight_by_id(SOM* net, int id);

/**
 * Allocate memory for SOM.
 * Set rows, cols and dims. However, weight is not initialized.
 */
SOM* som_alloc(int rows, int cols, int dims);

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
void som_train(SOM* net, double* data, int length, int iters);

/**
 * Given on sample of data (dimension = net->dims).
 * maps to a neuron.
 * return the ID of the neuron, that is 
 *   row * # of columns + colummn.
 */
int som_map(SOM* net, double* data);

/**
 * Another version of som_map, also return the distance
 */
int som_map_dis(SOM* net, double* data, double* distance);

/**
 * Save SOM to file.
 * 
 * @return 0 on success
 */
int som_save(SOM* net, FILE* file);

/**
 * Load SOM from file. Memory is allocated and should be freed.
 *
 * @return null if error occured.
 */
SOM* som_load(FILE* file);

#endif  // SOM_H_


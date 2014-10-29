// classify.h
// High level abstraction for classification.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef CLASSIFY_H_
#define CLASSIFY_H_

/**
 * An instance of data.
 */
typedef struct {
  void* feature;
  unsigned int label;
} clfy_instance;

/**
 * Discription of data.
 */
typedef struct {
  unsigned int nclass;
  const char** names;
} clfy_metadata;

/**
 * A set of data.
 */
typedef struct {
  unsigned int size;
  unsigned int capacity;
  clfy_metadata* metadata;
  clfy_instance* instances;
} clfy_dataset;

/**
 * Free function.
 */
typedef void (*free_func)(void* ptr);

/**
 * Classify function.
 */
typedef unsigned int (*classify_func)(void* fields, void* feature);

/**
 * A classifier.
 */
typedef struct {
  classify_func classify;
  free_func free_self;
  void* fields;
} clfy_classifier;

/**
 * Train function
 */
typedef clfy_classifier* (*clfy_train_func)
  (clfy_dataset* train_data);

/**
 * Confusion matrix.
 */
typedef struct {
  unsigned int size;
  unsigned int* counter;
} clfy_confmat;

/**
 * Allocate an empty data set.
 */
clfy_dataset* clfy_dataset_alloc(void);

/**
 * Free the dataset.
 * However the elements aka features are remained untouched.
 */
void clfy_dataset_free(clfy_dataset* data);

/**
 * Free the dataset as well as the elements.
 */
void clfy_dataset_freeall(clfy_dataset* data,
    free_func free_feature);

/**
 * Add an instance to the dataset.
 */
void clfy_dataset_add(clfy_dataset* data, clfy_instance ins);

/**
 * Allocate memory for a confusion matrix.
 */
clfy_confmat* clfy_confmat_alloc(unsigned int size);

/**
 * Free memory for a confusion matrix.
 */
void clfy_confmat_free(clfy_confmat* confmat);

/**
 * Evaluate the classifier using the train and data.
 * If confmat is not NULL, the confusion matrix is saved.
 * The confmat should be allocated with the correct size.
 *
 * @return the precision.
 */
double clfy_performance(clfy_dataset* train_data,
    clfy_dataset* test_data, clfy_train_func method,
    clfy_confmat* confmat);

/**
 * Evaluate the classifier using the given data with 
 * cross validation.
 * If confmat is not NULL, the confusion matrix is saved.
 * The confmat should be allocated with the correct size.
 *
 * @return the precision.
 */
double clfy_cross_validate(clfy_dataset* data, 
    clfy_train_func method, unsigned int nfold,
    clfy_confmat* confmat);

#endif  // CLASSIFY_H_


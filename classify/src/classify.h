// classify.h
// High level abstraction for classification.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef CLASSIFY_H_
#define CLASSIFY_H_

#include <stdio.h>

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
  unsigned int capacity;
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
typedef void (*clfy_free_func)(void* ptr);

/**
 * Classify function.
 */
typedef unsigned int (*clfy_classify_func)(void* fields,
    void* feature);

/**
 * A classifier.
 */
typedef struct {
  clfy_classify_func classify;
  clfy_free_func free_self;
  void* fields;
} clfy_classifier;

/**
 * Train function.
 */
typedef clfy_classifier* (*clfy_train_func)
  (clfy_dataset* train_data, void* param);

/**
 * Confusion matrix.
 */
typedef struct {
  unsigned int size;
  unsigned int* counter;
} clfy_confmat;

/**
 * Loader function.
 */
typedef void* (*clfy_loader_func)(FILE* stream, void* param);

/**
 * Allocate metadata with empty name list.
 */
clfy_metadata* clfy_metadata_alloc(void);

/**
 * Free metadata.
 */
void clfy_metadata_free(clfy_metadata* meta);

/**
 * Lookup the associated index of the name.
 * If the name does not exist in the name list, add it
 * to the end and return the new index.
 *
 * @note the name will be duplicated and saved in the list.
 */
unsigned int clfy_metadata_lookup(clfy_metadata* meta,
    const char* name);

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
    clfy_free_func free_feature);

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
 * Print confusion matrix.
 */ 
void clfy_confmat_fprintf(FILE* stream,
    clfy_confmat* confmat, clfy_metadata* meta);

/**
 * Print confusion matrix with customized width.
 */
void clfy_confmat_fprintf_wide(FILE* stream,
    clfy_confmat* confmat, clfy_metadata* meta,
    unsigned int width);

/**
 * Evaluate the classifier using the train and data.
 * If confmat is not NULL, the confusion matrix is saved.
 * The confmat should be allocated with the correct size.
 *
 * @return the precision.
 */
double clfy_performance(clfy_dataset* train_data,
    clfy_dataset* test_data, clfy_train_func method,
    void* train_param, clfy_confmat* confmat);

/**
 * Evaluate the classifier using the given data with 
 * cross validation.
 * If confmat is not NULL, the confusion matrix is saved.
 * The confmat should be allocated with the correct size.
 *
 * @return the precision.
 */
double clfy_cross_validate(clfy_dataset* data, 
    clfy_train_func method, void* train_param,
    unsigned int nfold, clfy_confmat* confmat);

/**
 * Load data from files.
 * This function will search all files with extension 
 * ".fvec" in the given path and load the with the given
 * function.
 */
unsigned int clfy_load_dataset(clfy_dataset* data,
    const char* path, clfy_loader_func loader, void* load_param);

#endif  // CLASSIFY_H_


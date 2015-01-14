// tagged.h
// Classify extension with tagged data
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2015, All rights reserved.

#ifndef TAGGED_H_
#define TAGGED_H_

#include "classify.h"

/**
 * An instance of tagged data.
 */
typedef struct {
  void* feature;
  unsigned int ntags;
  unsigned int capacity;
  unsigned int* tags;
} tagged_instance;

/**
 * Discription of tagged data.
 */
typedef struct {
  unsigned int ntags;
  unsigned int capacity;
  const char** names;
} tagged_metadata;

/**
 * A set of tagged data.
 */
typedef struct {
  unsigned int size;
  unsigned int capacity;
  tagged_metadata* metadata;
  tagged_instance* instances;
} tagged_dataset;

/**
 * Tag function.
 */
typedef void (*tagged_tag_func)(void* fields,
    void* feature, double* result);

/**
 * A tag model.
 */
typedef struct {
  tagged_tag_func tag;
  clfy_free_func free_self;
  void* fields;
} tagged_model;

/**
 * Train function.
 */
typedef tagged_model* (*tagged_train_func)
  (tagged_dataset* train_data, void* param);

/**
 * Tagged performance result.
 */
typedef struct {
  unsigned int size;
  unsigned int* tp;     // True Positive
  unsigned int* fp;     // False Positive
  unsigned int* fn;     // False Negative 
} tagged_result;

/**
 * Allocate metadata with empty name list.
 */
tagged_metadata* tagged_metadata_alloc(void);

/**
 * Free metadata.
 */
void tagged_metadata_free(tagged_metadata* meta);

/**
 * Lookup the associated index of the name.
 * If the name does not exist in the name list, add it
 * to the end and return the new index.
 *
 * @note the name will be duplicated and saved in the list.
 */
unsigned int tagged_metadata_lookup(tagged_metadata* meta,
    const char* name);

/**
 * Allocate an empty data set.
 */
tagged_dataset* tagged_dataset_alloc(void);

/**
 * Free the dataset.
 * However the elements aka features are remained untouched.
 */
void tagged_dataset_free(tagged_dataset* data);

/**
 * Free the dataset as well as the elements.
 */
void tagged_dataset_freeall(tagged_dataset* data,
    clfy_free_func free_feature);

/**
 * Add an instance to the dataset.
 */
void tagged_dataset_add(tagged_dataset* data, tagged_instance ins);

/**
 * Allocate memory for tagged result.
 */
tagged_result* tagged_result_alloc(unsigned int size);

/**
 * Free memory for tagged result.
 */
void tagged_result_free(tagged_result* result);

/**
 * Print tagged result.
 */ 
void tagged_result_fprintf(FILE* stream,
    tagged_result* result, clfy_metadata* meta);

/**
 * Evaluate the tagging model using the train and data.
 * If result is not NULL, the tagged result is saved.
 * The result should be allocated with the correct size.
 *
 * @return the precision.
 */
double tagged_performance(tagged_dataset* train_data,
    tagged_dataset* test_data, tagged_train_func method,
    void* train_param, tagged_result* result);

/**
 * Evaluate the tagging model using the given data with 
 * cross validation.
 * If result is not NULL, the tagged result is saved.
 * The result should be allocated with the correct size.
 *
 * @return the precision.
 */
double tagged_cross_validate(tagged_dataset* data, 
    tagged_train_func method, void* train_param,
    unsigned int nfold, tagged_result* result);

/**
 * Load data from files.
 * This function will search all files with extension 
 * ".fvec" in the given path and load the with the given
 * function.
 * Each file should have a corresponding ".tag" file.
 */
unsigned int tagged_load_dataset(tagged_dataset* data,
    const char* path, clfy_loader_func loader, void* load_param);

#endif  // TAGGED_H_


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
  char* source;
} tagged_instance;

/**
 * A set of tagged data.
 */
typedef struct {
  unsigned int size;
  unsigned int capacity;
  clfy_metadata* metadata;
  tagged_instance** instances;
} tagged_dataset;

/**
 * Tag probability function.
 * 
 */
typedef double (*tagged_tagprob_func)(void* fields, void* feature);

/**
 * A tag model.
 */
typedef struct {
  tagged_tagprob_func tag_prob;
  clfy_free_func free_self;
  void* fields;
} tagged_model;

/**
 * Train function.
 */
typedef tagged_model* (*tagged_train_func) (tagged_dataset* train_data,
    unsigned int tag, void* param);

/**
 * Tagged performance result.
 */
typedef struct {
  /**
   * Total instances.
   */
  unsigned int total;

  /**
   * True Positive.
   */
  unsigned int tp;

  /**
   * False Positive.
   */
  unsigned int fp;

  /**
   * False Negative.
   */
  unsigned int fn;
} tagged_result;

#define TAGGED_PRECISION(r) \
  ((double) (r).tp / (double) ((r).tp + (r).fp)) 

#define TAGGED_RECALL(r) \
  ((double) (r).tp / (double) ((r).tp + (r).fn)) 

#define TAGGED_FMEASURE(r) \
  (2.0 / (1.0 / TAGGED_PRECISION(r) + 1.0 / TAGGED_RECALL(r)))

#define TAGGED_ACCURACY(r) \
  ((double) ((r).total - (r).fn - (r).fp) / (double) (r).total) 

#define TAGGED_PE(r) \
  ((double) (((r).tp + (r).fp) * ((r).tp + (r).fn) \
    + ((r).total - (r).tp - (r).fp) * ((r).total - (r).tp - (r).fn)) \
   / (double) ((r).total * (r).total))

#define TAGGED_KAPPA(r) \
  ((TAGGED_ACCURACY(r) - TAGGED_PE(r)) / (1.0 - TAGGED_PE(r)))

/**
 * Allocate a tagged data instance.
 */
tagged_instance* tagged_instance_alloc(void);

/**
 * Allocate a tagged data instance.
 */
void tagged_instance_free(tagged_instance* ins,
    clfy_free_func feature_free);

/**
 * Add a tag to the instance.
 */
void tagged_instance_add(tagged_instance* ins, unsigned int tag);

/**
 * Check if the instance has the tag.
 *
 * @return 1 if the instance has the tag, otherwise return 0.
 */
unsigned int tagged_instance_hastag(tagged_instance* ins, unsigned int tag);

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
    clfy_free_func feature_free);

/**
 * Add an instance to the dataset.
 * @note the pointer is to the instance is stored,
 *       but the data is not copied.
 */
void tagged_dataset_add(tagged_dataset* data, tagged_instance* ins);

/**
 * Print tagged result.
 */ 
void tagged_result_fprintf(FILE* stream, tagged_result* result,
    const char* name);

/**
 * Calculate the probability of each instance being of the tag.
 * probability and gold_std should be size of data->size.
 */
void tagged_evaluate(tagged_model* model, tagged_dataset* data,
    unsigned int tag, double* probability, unsigned int* gold_std);

/**
 * Calculate the binary classification result according to a probablity
 * threshold.
 *
 * @return F1 Measure.
 */
double tagged_performance(double prob_threshold, unsigned int size,
    double* probability, unsigned int* gold_std, tagged_result* result);

/**
 * Calculate the probabilities using the given data with 
 * cross validation.
 */
void tagged_cross_validate(tagged_dataset* data, unsigned int tag,
    tagged_train_func method, void* train_param, unsigned int nfold,
    double* probability, unsigned int* gold_std);

/**
 * Load data from files.
 * This function will search all files with extension 
 * ".fvec" in the given path and load the with the given
 * function.
 * Each file should have a corresponding ".tag" file, which
 * contains the tags (one each line) of the data instance.
 */
unsigned int tagged_load_dataset(tagged_dataset* data,
    const char* path, clfy_loader_func loader, void* load_param);

#endif  // TAGGED_H_


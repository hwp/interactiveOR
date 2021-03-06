// sequence.h
// Functions related to classifying sequences.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "classify.h"
#include "tagged.h"

#include <stdio.h>

#include <gsl/gsl_rng.h>
#include <notghmm/notghmm.h>

#define SEQ_LOADER ((clfy_loader_func) seq_load)
#define SEQ_FREE ((clfy_free_func) seq_free)
#define SEQ_HMM_TRAIN ((clfy_train_func) seq_hmm_train)
#define SEQ_BIMODAL_TRAIN ((clfy_train_func) seq_bimodal_train)

typedef enum {
  SEQ_INIT_RANDOM,
  SEQ_INIT_KMEANS
} seq_init_t;

typedef struct {
  unsigned int dim;
} seq_load_param;

typedef struct {
  unsigned int nclass;
  hmmgmm_t** models; 
} seq_hmm_attr;

typedef struct {
  unsigned int n;
  unsigned int k;
  unsigned int dim;
  int cov_diag;
  seq_init_t init_type;
  gsl_rng* rng;
} seq_hmm_train_param;

typedef struct {
  unsigned int nclass;
  hmmgmm_t** vmodels;
  hmmgmm_t** amodels;
} seq_bimodal_attr;

typedef struct {
  unsigned int n;
  unsigned int k;
  unsigned int vdim;
  unsigned int adim;
  seq_init_t init_type;
  int cov_diag;
  gsl_rng* rng;
} seq_bimodal_train_param;

/**
 * @warning returned pointer should be freed after use.
 */
seq_t* seq_load(FILE* stream, seq_load_param* param);

unsigned int seq_hmm_classify(seq_hmm_attr* attr, seq_t* seq);

/**
 * @warning returned pointer should be freed after use.
 */
clfy_classifier* seq_hmm_train(clfy_dataset* train_data,
    seq_hmm_train_param* param);

unsigned int seq_bimodal_classify(seq_bimodal_attr* attr, seq_t* seq);

/**
 * @warning returned pointer should be freed after use.
 */
clfy_classifier* seq_bimodal_train(clfy_dataset* train_data,
    seq_bimodal_train_param* param);

#endif  // SEQUENCE_H_


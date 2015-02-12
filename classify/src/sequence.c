// sequence.c
// Functions related to classifying sequences.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "sequence.h"

#include <math.h>
#include <assert.h>

#include <notghmm/notghmm.h>

#define BUFFER_SIZE 128

seq_t* seq_load(FILE* stream, seq_load_param* param) {
  fseek(stream, 0L, SEEK_END);
  size_t length = ftell(stream);
  fseek(stream, 0L, SEEK_SET);

  assert(length % (param->dim * sizeof(double)) == 0);
  size_t size = length / (param->dim * sizeof(double));

  seq_t* res = seq_alloc(size, param->dim);

  double buf[BUFFER_SIZE];
  size_t sr, i;
  size_t ctr = 0;

  while((sr = fread(buf, sizeof(double), BUFFER_SIZE,
          stream)) > 0) {
    for (i = 0; i < sr; i++) {
      gsl_vector_set(res->data[ctr / res->dim],
          ctr % res->dim, buf[i]);
      ctr++;
    }
  }

  return res;
}

unsigned int seq_hmm_classify(seq_hmm_attr* attr,
    seq_t* seq) {
  size_t n = attr->models[0]->n;
  gsl_matrix* logalpha = gsl_matrix_alloc(seq->size, n);

  unsigned int i;
  double like;
  unsigned int res = 0;
  double max_like = -HUGE_VAL;
  for (i = 0; i < attr->nclass; i++) {
    forward_proc_log(attr->models[i], seq, logalpha);
    like = hmm_log_likelihood(logalpha);
    if (like > max_like) {
      res = i;
      max_like = like;
    }
  }

  gsl_matrix_free(logalpha);

  return res;
}

void seq_hmm_free(clfy_classifier* cl) {
  unsigned int i;
  seq_hmm_attr* attr = cl->fields;

  for (i = 0; i < attr->nclass; i++) {
    hmmgmm_free(attr->models[i]);
  }
  free(attr->models);

  free(attr);
  free(cl);
}

clfy_classifier* seq_hmm_train(clfy_dataset* train_data,
    seq_hmm_train_param* param) {
  fprintf(stderr, "Training HMM classifier using train "
      "data set of size %u.\n\tThe input vector is %u "
      "dimensional.\n\tEach model has %u states and %u "
      "components for each state.\n", train_data->size,
      param->dim, param->n, param->k);

  unsigned int i, j;

  clfy_classifier* res = malloc(sizeof(clfy_classifier));
  seq_hmm_attr* attr = malloc(sizeof(seq_hmm_attr));
  assert(res && attr);

  res->classify = (clfy_classify_func) seq_hmm_classify;
  res->free_self = (clfy_free_func) seq_hmm_free;
  res->fields = attr;

  attr->nclass = train_data->metadata->nclass;
  attr->models = malloc(attr->nclass * sizeof(hmmgmm_t*));

  seq_t** seqs = malloc(train_data->size * sizeof(seq_t*));

  for (i = 0; i < attr->nclass; i++) {
    hmmgmm_t* model = hmmgmm_alloc(param->n, param->k,
        param->dim, param->cov_diag);
    attr->models[i] = model;

    unsigned int nos = 0;
    for (j = 0; j < train_data->size; j++) {
      clfy_instance ins = train_data->instances[j];
      if (ins.label == i) {
        seqs[nos] = ins.feature;
        nos++;
      }
    }

    fprintf(stderr, "Training model for class %s with %u instances\n", train_data->metadata->names[i], nos);
    fprintf(stderr, "Initiating model...\n");
    random_init(model, seqs, nos, param->rng);
    fprintf(stderr, "Initiating model DONE.\n");
    fprintf(stderr, "Reestimating model...\n");
    baum_welch(model, seqs, nos);
    fprintf(stderr, "Restimating model DONE.\n");
  }

  free(seqs);
  return res;
}


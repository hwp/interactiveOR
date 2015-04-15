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

    fprintf(stderr, "Initiating model ");
    switch (param->init_type) {
      case SEQ_INIT_RANDOM:
        fprintf(stderr, "randomly...\n");
        random_init(model, seqs, nos, param->rng);
        break;
      case SEQ_INIT_KMEANS:
        fprintf(stderr, "with k-means...\n");
        kmeans_init(model, seqs, nos, param->rng);
        break;
      default:
        assert(0);
        break;
    }
    fprintf(stderr, "Initiating model DONE.\n");

    assert(hmmgmm_valid(model));

    fprintf(stderr, "Reestimating model...\n");
    baum_welch(model, seqs, nos);
    fprintf(stderr, "Reestimating model DONE.\n");
    
    assert(hmmgmm_valid(model));
  }

  free(seqs);
  return res;
}

void seq_bimodal_free(clfy_classifier* cl) {
  unsigned int i;
  seq_bimodal_attr* attr = cl->fields;

  for (i = 0; i < attr->nclass; i++) {
    hmmgmm_free(attr->vmodels[i]);
    hmmgmm_free(attr->amodels[i]);
  }
  free(attr->vmodels);
  free(attr->amodels);

  free(attr);
  free(cl);
}

static void seq_subvector(seq_t* dest, seq_t* src, size_t offset, size_t n) {
  assert(dest->dim == n);
  assert(dest->size == src->size);
  assert(offset + n <= src->dim);
  
  size_t i;
  for (i = 0; i < dest->size; i++) {
    gsl_vector_view vv = gsl_vector_subvector(src->data[i], offset, n);
    gsl_vector_memcpy(dest->data[i], &vv.vector);
  }
}

unsigned int seq_bimodal_classify(seq_bimodal_attr* attr, seq_t* seq) {
  size_t i;

  size_t vn = attr->vmodels[0]->n;
  size_t vdim = attr->vmodels[0]->dim;
  size_t an = attr->amodels[0]->n;
  size_t adim = attr->amodels[0]->dim;

  assert(vdim + adim == seq->dim);

  // construct two separate seq
  seq_t* vseq = seq_alloc(seq->size, vdim);
  seq_t* aseq = seq_alloc(seq->size, adim);
  seq_subvector(vseq, seq, 0, vdim);
  seq_subvector(aseq, seq, vdim, adim);

  gsl_matrix* vlogalpha = gsl_matrix_alloc(seq->size, vn);
  gsl_matrix* alogalpha = gsl_matrix_alloc(seq->size, an);

  double like;
  unsigned int res = 0;
  double max_like = -HUGE_VAL;
  for (i = 0; i < attr->nclass; i++) {
    forward_proc_log(attr->vmodels[i], vseq, vlogalpha);
    forward_proc_log(attr->amodels[i], aseq, alogalpha);

    double vlike = hmm_log_likelihood(vlogalpha);
    double alike = hmm_log_likelihood(alogalpha);
    like = vlike + alike;

    if (like > max_like) {
      res = i;
      max_like = like;
    }
  }

  seq_free(vseq);
  seq_free(aseq);
  gsl_matrix_free(vlogalpha);
  gsl_matrix_free(alogalpha);

  return res;
}

clfy_classifier* seq_bimodal_train(clfy_dataset* train_data,
    seq_bimodal_train_param* param) {
  fprintf(stderr, "Training Bimodal HMM classifier using train "
      "data set of size %u.\n\tThe input vector is %u + %u "
      "dimensional.\n\tEach model has %u states and %u "
      "components for each state.\n", train_data->size,
      param->vdim, param->adim, param->n, param->k);

  unsigned int i, j;

  clfy_classifier* res = malloc(sizeof(clfy_classifier));
  seq_bimodal_attr* attr = malloc(sizeof(seq_bimodal_attr));
  assert(res && attr);

  res->classify = (clfy_classify_func) seq_bimodal_classify;
  res->free_self = (clfy_free_func) seq_bimodal_free;
  res->fields = attr;

  attr->nclass = train_data->metadata->nclass;
  attr->vmodels = malloc(attr->nclass * sizeof(hmmgmm_t*));
  attr->amodels = malloc(attr->nclass * sizeof(hmmgmm_t*));

  seq_t** vseqs = malloc(train_data->size * sizeof(seq_t*));
  seq_t** aseqs = malloc(train_data->size * sizeof(seq_t*));

  for (i = 0; i < attr->nclass; i++) {
    hmmgmm_t* vmodel = hmmgmm_alloc(param->n, param->k,
        param->vdim, param->cov_diag);
    hmmgmm_t* amodel = hmmgmm_alloc(param->n, param->k,
        param->adim, param->cov_diag);
    attr->vmodels[i] = vmodel;
    attr->amodels[i] = amodel;

    unsigned int nos = 0;
    for (j = 0; j < train_data->size; j++) {
      clfy_instance ins = train_data->instances[j];
      seq_t* src = ins.feature;
      assert(src->dim == param->vdim + param->adim);
      if (ins.label == i) {
        vseqs[nos] = seq_alloc(src->size, param->vdim);
        seq_subvector(vseqs[nos], src, 0, param->vdim);
        aseqs[nos] = seq_alloc(src->size, param->adim);
        seq_subvector(aseqs[nos], src, param->vdim, param->adim);
        nos++;
      }
    }

    fprintf(stderr, "Training model for class %s with %u instances\n", train_data->metadata->names[i], nos);

    fprintf(stderr, "Initiating visual model ");
    switch (param->init_type) {
      case SEQ_INIT_RANDOM:
        fprintf(stderr, "randomly...\n");
        random_init(vmodel, vseqs, nos, param->rng);
        break;
      case SEQ_INIT_KMEANS:
        fprintf(stderr, "with k-means...\n");
        kmeans_init(vmodel, vseqs, nos, param->rng);
        break;
      default:
        assert(0);
        break;
    }
    fprintf(stderr, "Initiating visual model DONE.\n");
    
    fprintf(stderr, "Initiating audio model ");
    switch (param->init_type) {
      case SEQ_INIT_RANDOM:
        fprintf(stderr, "randomly...\n");
        random_init(amodel, aseqs, nos, param->rng);
        break;
      case SEQ_INIT_KMEANS:
        fprintf(stderr, "with k-means...\n");
        kmeans_init(amodel, aseqs, nos, param->rng);
        break;
      default:
        assert(0);
        break;
    }
    fprintf(stderr, "Initiating audio model DONE.\n");
    
    assert(hmmgmm_valid(vmodel));
    assert(hmmgmm_valid(amodel));
    
    fprintf(stderr, "Reestimating visual model...\n");
    baum_welch(vmodel, vseqs, nos);
    fprintf(stderr, "Reestimating visual model DONE.\n");

    fprintf(stderr, "Reestimating audio model...\n");
    baum_welch(amodel, aseqs, nos);
    fprintf(stderr, "Reestimating audio model DONE.\n");

    assert(hmmgmm_valid(vmodel));
    assert(hmmgmm_valid(amodel));
    
    for (j = 0; j < nos; j++) {
      seq_free(vseqs[j]);
      seq_free(aseqs[j]);
    }
  }

  free(vseqs);
  free(aseqs);
  return res;
}


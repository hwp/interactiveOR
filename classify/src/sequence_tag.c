// sequence_tag.c
// sequence with tagged data.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2015, All rights reserved.

#include "sequence_tag.h"

#include <math.h>
#include <assert.h>

#include <notghmm/notghmm.h>

double seq_tag_prob(seq_hmm_attr* attr, seq_t* seq) {
  assert(attr->nclass == 2);

  size_t n = attr->models[0]->n;
  gsl_matrix* logalpha = gsl_matrix_alloc(seq->size, n);

  forward_proc_log(attr->models[0], seq, logalpha);
  double pos = exp(hmm_log_likelihood(logalpha));

  forward_proc_log(attr->models[1], seq, logalpha);
  double neg = exp(hmm_log_likelihood(logalpha));

  gsl_matrix_free(logalpha);
  
  if (pos + neg == 0.0) {
    // Unseen object consider as negative
    fprintf(stderr, "Warning: Unseen object. Tag negative.\n");
    return 0.0;
  }
  else { // pos + neg > 0
    return pos / (pos + neg);
  }
}

void seq_tag_free(tagged_model* model) {
  unsigned int i;
  seq_hmm_attr* attr = model->fields;

  for (i = 0; i < attr->nclass; i++) {
    hmmgmm_free(attr->models[i]);
  }
  free(attr->models);

  free(attr);
  free(model);
}

tagged_model* seq_tag_train(tagged_dataset* train_data,
    unsigned int tag, seq_hmm_train_param* param) {
  fprintf(stderr, "Training HMM generative model using train "
      "data set of size %u.\n\tThe input vector is %u "
      "dimensional.\n\tEach model has %u states and %u "
      "components for each state.\n", train_data->size,
      param->dim, param->n, param->k);

  unsigned int i;

  tagged_model* res = malloc(sizeof(tagged_model));
  seq_hmm_attr* attr = malloc(sizeof(seq_hmm_attr));
  assert(res && attr);

  res->tag_prob = (tagged_tagprob_func) seq_tag_prob;
  res->free_self = (clfy_free_func) seq_tag_free;
  res->fields = attr;

  attr->nclass = 2;
  attr->models = malloc(attr->nclass * sizeof(hmmgmm_t*));

  seq_t** seqs = malloc(train_data->size * sizeof(seq_t*));
  unsigned int cp = 0;
  unsigned int cn = 0;

  for (i = 0; i < train_data->size; i++) {
    tagged_instance* ins = train_data->instances[i];
    if (tagged_instance_hastag(ins, i)) {
      seqs[cp] = ins->feature;
      cp++;
    }
    else {
      cn++;
      seqs[train_data->size - cn] = ins->feature;
    }
  }

  assert(cp + cn == train_data->size);

  hmmgmm_t* model = hmmgmm_alloc(param->n, param->k, param->dim);
  attr->models[0] = model; 

  fprintf(stderr, "Training positive model with %u instances\n", cp);
  fprintf(stderr, "Initiating model...\n");
  random_init(model, seqs, cp, param->rng);
  fprintf(stderr, "Initiating model DONE.\n");
  fprintf(stderr, "Reestimating model...\n");
  baum_welch(model, seqs, cp);
  fprintf(stderr, "Restimating model DONE.\n");

  model = hmmgmm_alloc(param->n, param->k, param->dim);
  attr->models[1] = model; 

  fprintf(stderr, "Training negative model with %u instances\n", cn);
  fprintf(stderr, "Initiating model...\n");
  random_init(model, seqs + cp, cn, param->rng);
  fprintf(stderr, "Initiating model DONE.\n");
  fprintf(stderr, "Reestimating model...\n");
  baum_welch(model, seqs + cp, cn);
  fprintf(stderr, "Restimating model DONE.\n");

  free(seqs);
  return res;
}

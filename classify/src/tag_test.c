// tag_test.c
// Test the functions of sequence_tag.h
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"
#include "sequence_tag.h"
#include "debug_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <gsl/gsl_rng.h>

int main(int argc, char** argv) {
  debug_init();

  // get options
  int showhelp = 0;
  unsigned int dim = 0;
  unsigned int n_states = 0;
  unsigned int n_comp = 0;
  int cov_diag = 0;

  int opt;
  while ((opt = getopt(argc, argv, "d:n:k:c:h")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
      case 'd':
        dim = atoi(optarg);
        break;
      case 'n':
        n_states = atoi(optarg);
        break;
      case 'k':
        n_comp = atoi(optarg);
        break;
      case 'c':
        cov_diag = atoi(optarg);
        break;
      default:
        showhelp = 1;
        break;
    }
  }

  if (showhelp || dim <= 0 || n_states <= 0 
      || n_comp <= 0 || argc - optind < 1) {
    fprintf(stderr, "Usage: %s -d dimension -n num_states "
        "-k num_components [-c cov_diag] data_dir\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  gsl_rng_env_setup();
  gsl_rng* rng = gsl_rng_alloc(gsl_rng_default);

  tagged_dataset* data = tagged_dataset_alloc();
  clfy_metadata* meta = clfy_metadata_alloc();
  data->metadata = meta;

  seq_load_param param;
  param.dim = dim;

  unsigned int size = tagged_load_dataset(data, argv[optind],
      SEQ_LOADER, &param);
  printf("%u instances loaded\n", size);

  seq_hmm_train_param train_param;
  train_param.n = n_states;
  train_param.k = n_comp;
  train_param.dim = param.dim;
  train_param.rng = rng;
  train_param.cov_diag = cov_diag;

  double* prob = malloc(data->size * sizeof(double));
  unsigned int* gold = malloc(data->size * sizeof(unsigned int));

  unsigned int i;
  for(i = 0; i < meta->nclass; i++) {
    tagged_object_cv(data, i, SEQ_TAG_TRAIN, &train_param, prob, gold);

    tagged_result result;
    tagged_performance(.5, data->size, prob, gold, &result);

    tagged_result_fprintf(stdout, &result, meta->names[i]);
  }

  free(prob);
  free(gold);
  tagged_dataset_freeall(data, SEQ_FREE);
  clfy_metadata_free(meta);
  gsl_rng_free(rng);

  return 0;
}


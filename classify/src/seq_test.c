// seq_test.c
// Test the functions of sequence.h
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"
#include "sequence.h"
#include "debug_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <fenv.h>

#include <gsl/gsl_rng.h>

int main(int argc, char** argv) {
//  feenableexcept(FE_INVALID | FE_DIVBYZERO);
  debug_init();

  // get options
  int showhelp = 0;
  unsigned int dim = 0;
  unsigned int n_states = 0;
  unsigned int n_comp = 0;
  int cov_diag = 0;
  seq_init_t init_type = SEQ_INIT_RANDOM;

  int opt;
  while ((opt = getopt(argc, argv, "d:n:k:c:i:h")) != -1) {
    int a;
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
      case 'i':
        a = atoi(optarg);
        if (a) {
          init_type = SEQ_INIT_KMEANS;
        }
        break;
      default:
        showhelp = 1;
        break;
    }
  }

  if (showhelp || dim <= 0 || n_states <= 0 
      || n_comp <= 0 || argc - optind < 1) {
    fprintf(stderr, "Usage: %s -d dimension [-i (0:random|1:kmeans)] -n num_states "
        "-k num_components [-c cov_diag] data_dir\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  gsl_rng_env_setup();
  gsl_rng* rng = gsl_rng_alloc(gsl_rng_default);

  clfy_dataset* data = clfy_dataset_alloc();
  clfy_metadata* meta = clfy_metadata_alloc();
  data->metadata = meta;

  seq_load_param param;
  param.dim = dim;

  unsigned int size = clfy_load_dataset(data, argv[optind],
      SEQ_LOADER, &param);
  printf("%u instances loaded\n", size);

  clfy_confmat* confmat = clfy_confmat_alloc(meta->nclass);

  seq_hmm_train_param train_param;
  train_param.n = n_states;
  train_param.k = n_comp;
  train_param.dim = param.dim;
  train_param.rng = rng;
  train_param.cov_diag = cov_diag;
  train_param.init_type = init_type;

  double precision = clfy_cross_validate(data,
      SEQ_HMM_TRAIN, &train_param, 5, confmat);
  printf("Precision = %g\n", precision);
  clfy_confmat_fprintf_wide(stdout, confmat, meta, 4);

  clfy_confmat_free(confmat);
  clfy_dataset_freeall(data, SEQ_FREE);
  clfy_metadata_free(meta);
  gsl_rng_free(rng);

  return 0;
}


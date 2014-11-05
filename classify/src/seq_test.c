// seq_test.c
// Test the functions of sequence.h
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"
#include "sequence.h"

#include <stdio.h>

#define _GNU_SOURCE
#include <fenv.h>

#include <gsl/gsl_rng.h>

int main(int argc, char** argv) {
  // feenableexcept(FE_INVALID | FE_DIVBYZERO);

  gsl_rng_env_setup();
  gsl_rng* rng = gsl_rng_alloc(gsl_rng_default);

  clfy_dataset* data = clfy_dataset_alloc();
  clfy_metadata* meta = clfy_metadata_alloc();
  data->metadata = meta;

  seq_load_param param;
  param.dim = 33;

  unsigned int size = clfy_load_dataset(data, argv[1],
      SEQ_LOADER, &param);
  printf("%u instances loaded\n", size);

  clfy_confmat* confmat = clfy_confmat_alloc(meta->nclass);

  seq_hmm_train_param train_param;
  train_param.n = 3;
  train_param.k = 2;
  train_param.dim = param.dim;
  train_param.rng = rng;

  double precision = clfy_cross_validate(data,
      SEQ_HMM_TRAIN, &train_param, 5, confmat);
  printf("Precision = %g\n", precision);
  clfy_confmat_fprintf_wide(stdout, confmat, meta, 4);

  clfy_confmat_free(confmat);
  clfy_dataset_freeall(data, free);
  clfy_metadata_free(meta);
  gsl_rng_free(rng);

  return 0;
}


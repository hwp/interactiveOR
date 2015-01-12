// stab_test.c
// Stability Test of HMM-GMM random initialization
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2015, All rights reserved.

#include "sequence.h"
#include "debug_utils.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_matrix.h>
#include <notghmm/notghmm.h>

int main(int argc, char** argv) {
  debug_init();

  int i, j; 

  // get options
  int showhelp = 0;
  unsigned int trials = 0;
  unsigned int dim = 0;
  unsigned int n_states = 0;
  unsigned int n_comp = 0;

  int opt;
  while ((opt = getopt(argc, argv, "t:d:n:k:h")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
      case 't':
        trials = atoi(optarg);
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
      default:
        showhelp = 1;
        break;
    }
  }

  if (showhelp || trials <= 0 || dim <= 0 || n_states <= 0 
      || n_comp <= 0 || argc - optind < 1) {
    fprintf(stderr, "Usage: %s -t num_trials -d dimension "
        "-n num_states -k num_components seq_files\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  gsl_rng_env_setup();
  gsl_rng* rng = gsl_rng_alloc(gsl_rng_default);

  seq_load_param param;
  param.dim = dim;

  int nos = argc - optind;
  int nost = nos;
  if (nost > 1) {
    nost--;
  }
  
  seq_t** seqs = malloc(nos * sizeof(seq_t*));
  for (i = 0; i < nos; i++) {
    FILE* in = fopen(argv[optind + i], "r");
    assert(in);
    seqs[i] = seq_load(in, &param);
    fclose(in);
  }

  hmmgmm_t* model = hmmgmm_alloc(n_states, n_comp, dim);

  double like;
  for (i = 0; i < trials; i++) {
    random_init(model, seqs, nost, rng);
    
    like = 0;
    for (j = 0; j < nost; j++) {
      seq_t* seq = seqs[j];
      gsl_matrix* logalpha = gsl_matrix_alloc(seq->size, n_states);
      forward_proc_log(model, seq, logalpha);
      like += hmm_log_likelihood(logalpha);
      gsl_matrix_free(logalpha);
    }
    printf("%g", like / nost);

    if (nost < nos) {
      seq_t* seq = seqs[nost];
      gsl_matrix* logalpha = gsl_matrix_alloc(seq->size, n_states);
      forward_proc_log(model, seq, logalpha);
      like = hmm_log_likelihood(logalpha);
      gsl_matrix_free(logalpha);
      printf("\t%g", like);
    }

    baum_welch(model, seqs, nost);

    like = 0;
    for (j = 0; j < nost; j++) {
      seq_t* seq = seqs[j];
      gsl_matrix* logalpha = gsl_matrix_alloc(seq->size, n_states);
      forward_proc_log(model, seq, logalpha);
      like += hmm_log_likelihood(logalpha);
      gsl_matrix_free(logalpha);
    }
    printf("\t%g", like / nost);

    if (nost < nos) {
      seq_t* seq = seqs[nost];
      gsl_matrix* logalpha = gsl_matrix_alloc(seq->size, n_states);
      forward_proc_log(model, seq, logalpha);
      like = hmm_log_likelihood(logalpha);
      gsl_matrix_free(logalpha);
      printf("\t%g", like);
    }

    printf("\n");
  }

  for (i = 0; i < nos; i++) {
    seq_free(seqs[i]);
  }
  free(seqs);
  hmmgmm_free(model);
  gsl_rng_free(rng);

  return 0;
}

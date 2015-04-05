// svm_test.c
// Test the functions of svm_tag.h
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"
#include "sequence.h"
#include "svm_tag.h"
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
  double cost = 1.0;
  double gamma = 0.0;
  double cache_size = 100.0;
  int unknown = 1;

  int opt;
  while ((opt = getopt(argc, argv, "d:c:g:m:u:h")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
      case 'd':
        dim = atoi(optarg);
        break;
      case 'c':
        cost = atof(optarg);
        break;
      case 'g':
        gamma = atof(optarg);
        break;
      case 'm':
        cache_size = atof(optarg);
        break;
      case 'u':
        unknown = atoi(optarg);
        break;
      default:
        showhelp = 1;
        break;
    }
  }

  if (gamma == 0.0) {
    gamma = 1.0 / dim;
  }

  if (showhelp || dim <= 0 || cost <= 0.0 || gamma <= 0.0 
      || argc - optind < 1) {
    fprintf(stderr, "Usage: %s -d dimension -c cost -g gamma "
        "[-m cache_size] [-u unknown] data_dir\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  tagged_dataset* data = tagged_dataset_alloc();
  clfy_metadata* meta = clfy_metadata_alloc();
  data->metadata = meta;

  seq_load_param param;
  param.dim = dim;

  unsigned int size = tagged_load_dataset(data, argv[optind],
      SEQ_LOADER, &param);
  printf("%u instances loaded\n", size);

  svm_tag_param train_param;
  train_param.cost = cost;
  train_param.gamma = gamma;
  train_param.cache_size = cache_size;
  char* method_desc = svm_tag_description(&train_param);
  char* desc = NULL;
  if (unknown) {
    asprintf(&desc, "%s - unknown cv", method_desc);
  }
  else {
    asprintf(&desc, "%s - known cv", method_desc);
  }

  tagged_probability* prob = malloc(data->size * sizeof(tagged_probability));
  unsigned int* gold = malloc(data->size * sizeof(unsigned int));

  unsigned int i;
  for(i = 0; i < meta->nclass; i++) {
    if (unknown) {
      tagged_object_cv(data, i, SVM_TAG_TRAIN, &train_param,
          prob, gold);
    }
    else {
      tagged_cross_validate(data, i, SVM_TAG_TRAIN, &train_param,
          5, prob, gold);
    }
    tagged_log(stdout, data, i, prob, gold, desc);
  }

  free(method_desc);
  free(desc);
  free(prob);
  free(gold);
  tagged_dataset_freeall(data, SEQ_FREE);
  clfy_metadata_free(meta);

  return 0;
}


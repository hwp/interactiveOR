// classify_test.c
// Test file
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"

#include <stdio.h>
#include <string.h>

#include <gsl/gsl_rng.h>

typedef struct {
  double x;
  double y;
} point;

unsigned int classify(double* th, point* p) {
  if (p->x + p->y >= *th) {
    return 1;
  }
  else {
    return 0;
  }
}

void free_s(void* ptr) {
  clfy_classifier* cl = ptr;
  free(cl->fields);
  free(cl);
}

clfy_classifier* train(clfy_dataset* train_data) {
  clfy_classifier* cl = malloc(sizeof(clfy_classifier));
  double* th = malloc(sizeof(double));

  cl->classify = (classify_func) classify;
  cl->free_self = free_s;
  cl->fields = th;

  *th = 0.0;
  unsigned int i;
  for (i = 0; i < train_data->size; i++) {
    point* p = (point*) train_data->instances[i].feature;
    *th += p->x + p->y;
  }
  *th /= (double) train_data->size;

  return cl;
}

void* loader(FILE* stream, gsl_rng* rng) {
  double dx, dy;
  int r = fscanf(stream, "%lg %lg", &dx, &dy);
  if (r == 2) { 
    point* p = malloc(sizeof(point));
    p->x = gsl_rng_uniform(rng) + dx;
    p->y = gsl_rng_uniform(rng) + dy;
    return p;
  }
  else {
    return NULL;
  }
}

int main(int argc, char** argv) {
  gsl_rng_env_setup();
  const gsl_rng_type* type = gsl_rng_default;
  gsl_rng* rng = gsl_rng_alloc(type);

  unsigned int i;
  clfy_dataset* data = clfy_dataset_alloc();
  clfy_metadata* meta = clfy_metadata_alloc();
  data->metadata = meta;

  for (i = 0; i < 100; i++) {
    clfy_load_dataset(data, argv[1], (loader_func) loader, rng);
  }

  clfy_confmat* confmat = clfy_confmat_alloc(meta->nclass);
  double precision = clfy_cross_validate(data, train,
      3, confmat);
  printf("Precision = %g\n", precision);
  clfy_confmat_fprintf(stdout, confmat, meta);

  clfy_confmat_free(confmat);
  clfy_dataset_freeall(data, free);
  clfy_metadata_free(meta);
  gsl_rng_free(rng);
  return 0;
}


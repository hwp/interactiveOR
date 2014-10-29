// classify_test.c
// Test file
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"

#include <stdio.h>

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

int main(int argc, char** argv) {
  gsl_rng_env_setup();
  const gsl_rng_type* type = gsl_rng_default;
  gsl_rng* rng = gsl_rng_alloc(type);

  unsigned int i;
  clfy_instance ins;
  clfy_dataset* data = clfy_dataset_alloc();
  clfy_metadata meta;
  meta.nclass = 2;
  const char* names[2] = {"c1", "c2"};
  meta.names = names;
  data->metadata = &meta;

  for (i = 0; i < 100; i++) {
    point* p = malloc(sizeof(point));
    p->x = gsl_rng_uniform(rng);
    p->y = gsl_rng_uniform(rng);

    ins.feature = p;
    ins.label = 0;
    clfy_dataset_add(data, ins);
  }

  for (i = 0; i < 200; i++) {
    point* p = malloc(sizeof(point));
    p->x = gsl_rng_uniform(rng) + 0.5;
    p->y = gsl_rng_uniform(rng) + 0.5;

    ins.feature = p;
    ins.label = 1;
    clfy_dataset_add(data, ins);
  }

  clfy_confmat* confmat = clfy_confmat_alloc(meta.nclass);
  double precision = clfy_cross_validate(data, train,
      3, confmat);
  printf("Precision = %g\n", precision);
  clfy_confmat_fprintf(stdout, confmat, &meta);

  clfy_confmat_free(confmat);
  clfy_dataset_freeall(data, free);
  gsl_rng_free(rng);
  return 0;
}


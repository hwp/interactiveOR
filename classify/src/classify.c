// classify.c
// High level abstraction for classification.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DATASET_INIT_SIZE 128

#define CONFMAT_GET(mat, i, j) \
  ((mat)->counter[(mat)->size * (i) + (j)])

clfy_dataset* clfy_dataset_alloc(void) {
  clfy_dataset* ret = malloc(sizeof(clfy_dataset));
  clfy_instance* instances = malloc(DATASET_INIT_SIZE * sizeof(clfy_instance));

  if (ret && instances) {
    ret->size = 0;
    ret->capacity = DATASET_INIT_SIZE;
    ret->metadata = NULL;
    ret->instances = instances;
  }
  else {
    free(ret);
    free(instances);
    ret = NULL;
  }

  return ret;
}

void clfy_dataset_free(clfy_dataset* data) {
  if (data) {
    free(data->instances);
    free(data);
  }
}

void clfy_dataset_freeall(clfy_dataset* data,
    free_func free_feature) {
  unsigned int i;
  for (i = 0; i < data->size; i++) {
    free_feature(data->instances[i].feature);
  }
  clfy_dataset_free(data);
}

void clfy_dataset_add(clfy_dataset* data, clfy_instance ins) {
  if (data->size == data->capacity) {
    data->capacity *= 2;
    clfy_instance* new_ins = malloc(data->capacity 
        * sizeof(clfy_instance));
    assert(new_ins);
    memcpy(new_ins, data->instances,
        data->size * sizeof(clfy_instance));
    free(data->instances);
    data->instances = new_ins;
  }
  
  data->instances[data->size] = ins;
  data->size++;
}

clfy_confmat* clfy_confmat_alloc(unsigned int size) {
  assert(size > 0);

  unsigned int* counter = calloc(size * size, sizeof(unsigned int));
  clfy_confmat* ret = malloc(sizeof(clfy_confmat));

  if (counter && ret) {
    ret->size = size;
    ret->counter = counter;
  }
  else {
    free(counter);
    free(ret);
    ret = NULL;
  }

  return ret;
}

void clfy_confmat_free(clfy_confmat* confmat) {
  if (confmat) {
    free(confmat->counter);
    free(confmat);
  }
}

double clfy_performance(clfy_dataset* train_data,
    clfy_dataset* test_data, clfy_train_func method,
    clfy_confmat* confmat) {
  clfy_classifier* cl = method(train_data);

  unsigned int i;
  unsigned int correct = 0;
  for (i = 0; i < test_data->size; i++) {
    clfy_instance ins = test_data->instances[i];
    unsigned int cr = cl->classify(cl->fields, ins.feature);
    if (cr == ins.label) {
      correct++;
    }
    if (confmat) {
      CONFMAT_GET(confmat, ins.label, cr)++;
    }
  }

  if (cl->free_fields) {
    cl->free_fields(cl->fields);
  }

  return (double) correct / (double) test_data->size;
}

void divide(clfy_dataset* data, unsigned int* group,
    unsigned int fold) {
  // TODO
}

double clfy_cross_validate(clfy_dataset* data, 
    clfy_train_func method, unsigned int num_fold,
    clfy_confmat* confmat) {
  unsigned int* group = malloc(data->size * sizeof(unsigned int));
  divide(data, group, num_fold);

  double sum = 0.0;
  unsigned int i, j;
  for (i = 0; i < num_fold; i++) {
    clfy_dataset* train = clfy_dataset_alloc();
    train->metadata = data->metadata;
    clfy_dataset* test = clfy_dataset_alloc();
    test->metadata = data->metadata;

    for (j = 0; j < data->size; j++) {
      if (group[j] == i) {
        clfy_dataset_add(test, data->instances[j]);
      }
      else {
        clfy_dataset_add(train, data->instances[j]);
      }
    }

    sum += clfy_performance(train, test, method, confmat) * test->size;

    clfy_dataset_free(train);
    clfy_dataset_free(test);
  }

  free(group);

  return sum / (double) data->size;
}


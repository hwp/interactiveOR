// classify.c
// High level abstraction for classification.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>

#define METADATA_INIT_CAPACITY 20
#define DATASET_INIT_CAPACITY 128

#define DEFAULT_WIDTH 8

#define CONFMAT_GET(mat, i, j) \
  ((mat)->counter[(mat)->size * (i) + (j)])

clfy_metadata* clfy_metadata_alloc(void) {
  clfy_metadata* ret = malloc(sizeof(clfy_metadata));
  const char** names = malloc(METADATA_INIT_CAPACITY
      * sizeof(const char*));

  if (ret && names) {
    ret->nclass = 0;
    ret->capacity = METADATA_INIT_CAPACITY;
    ret->names = names;
  }
  else {
    free(ret);
    free(names);
    ret = NULL;
  }

  return ret;
}

void clfy_metadata_free(clfy_metadata* meta) {
  if (meta) {
    unsigned int i;
    for (i = 0; i < meta->nclass; i++) {
      free((char*) meta->names[i]);
    }
    free(meta->names);
    free(meta);
  }
}

unsigned int clfy_metadata_lookup(clfy_metadata* meta,
    const char* name) {
  unsigned int i;
  for (i = 0; i < meta->nclass; i++) {
    if (strcmp(meta->names[i], name) == 0) {
      return i;
    }
  }

  if (meta->nclass == meta->capacity) {
    meta->capacity *= 2;
    meta->names = realloc(meta->names,
        meta->capacity * sizeof(const char*));    
    assert(meta->names);
  }

  meta->names[meta->nclass] = strdup(name);
  assert(meta->names[meta->nclass]);
  meta->nclass++;

  return meta->nclass - 1;
}

clfy_dataset* clfy_dataset_alloc(void) {
  clfy_dataset* ret = malloc(sizeof(clfy_dataset));
  clfy_instance* instances = malloc(DATASET_INIT_CAPACITY
      * sizeof(clfy_instance));

  if (ret && instances) {
    ret->size = 0;
    ret->capacity = DATASET_INIT_CAPACITY;
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
    data->instances = realloc(data->instances,
        data->capacity * sizeof(clfy_instance));
    assert(data->instances);
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

void clfy_confmat_fprintf(FILE* stream,
    clfy_confmat* confmat, clfy_metadata* meta) {
  clfy_confmat_fprintf_wide(stream, confmat, meta,
      DEFAULT_WIDTH);
}

void clfy_confmat_fprintf_wide(FILE* stream,
    clfy_confmat* confmat, clfy_metadata* meta,
    unsigned int width) {
  assert(meta->nclass == confmat->size);

  unsigned int i, j;

  char name_format[10];
  snprintf(name_format, 10, "%%%us", width);
  char number_format[10];
  snprintf(number_format, 10, "%%%uu", width);

  fprintf(stream, name_format, "gs\\pr");
  for (i = 0; i < meta->nclass; i++) {
    fprintf(stream, name_format, meta->names[i]);
  }
  fprintf(stream, "\n");
  for (i = 0; i < meta->nclass; i++) {
    fprintf(stream, name_format, meta->names[i]);
    for (j = 0; j < meta->nclass; j++) {
      fprintf(stream, number_format,
          CONFMAT_GET(confmat, i, j));
    }
    fprintf(stream, "\n");
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

  if (cl->free_self) {
    cl->free_self(cl);
  }

  return (double) correct / (double) test_data->size;
}

void divide(clfy_dataset* data, unsigned int* group,
    unsigned int nfold) {
  unsigned int* ctr = calloc(data->metadata->nclass,
      sizeof(unsigned int));

  unsigned int i;
  for (i = 0; i < data->size; i++) {
    unsigned int c = data->instances[i].label;
    group[i] = ctr[c];
    ctr[c] = (ctr[c] + 1) % nfold;
  }

  free(ctr);
}

double clfy_cross_validate(clfy_dataset* data,
    clfy_train_func method, unsigned int nfold,
    clfy_confmat* confmat) {
  unsigned int* group = malloc(data->size * sizeof(unsigned int));
  divide(data, group, nfold);

  double sum = 0.0;
  unsigned int i, j;
  for (i = 0; i < nfold; i++) {
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

unsigned int clfy_load_dataset(clfy_dataset* data,
    const char* path, loader_func loader, void* load_param) {
  unsigned int n = 0;

  DIR* dir = opendir(path);
  if (dir) {
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
      char* name = ent->d_name;
      char* dot = strrchr(name, '.');
      if (dot && strcmp(dot, ".fvec") == 0) {
        char filepath[512];
        strcpy(filepath, path);
        strcat(filepath, "/");
        strcat(filepath, name);
        FILE* in = fopen(filepath, "r");
        if (in) {
          clfy_instance ins;
          ins.feature = loader(in, load_param);
          fclose(in);

          if (ins.feature) {
            char class[256];
            strcpy(class, name);
            char* dash = strchr(class, '_');
            if (dash) {
              *dash = '\0';
            }
            else {
              dash = strchr(class, '.');
              *dash = '\0';
            }
            ins.label = clfy_metadata_lookup(data->metadata, class);

            clfy_dataset_add(data, ins);
            n++;
          }
          else {
            fprintf(stderr, "Error: Failed to load data "
                "from file: %s\n", filepath);
          }
        }
        else {
          fprintf(stderr, "Error: Failed to open file: %s\n"
              "Error: %s\n", filepath, strerror(errno));
        }
      }
    }
    closedir(dir);
  }
  else {
    fprintf(stderr, "Error: Cannot open directory: %s\n"
        "%s\n", path, strerror(errno));
  }

  return n;
}


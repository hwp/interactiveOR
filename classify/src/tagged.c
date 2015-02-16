// tagged.c
// Classify extension with tagged data
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2015, All rights reserved.

#include "tagged.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>

#define INSTANCE_INIT_CAPACITY 10
#define DATASET_INIT_CAPACITY 128

tagged_instance* tagged_instance_alloc(void) {
  tagged_instance* ret = malloc(sizeof(tagged_instance));
  unsigned int* tags = malloc(INSTANCE_INIT_CAPACITY
      * sizeof(unsigned int));

  if (ret && tags) {
    ret->feature = NULL;
    ret->ntags = 0;
    ret->capacity = INSTANCE_INIT_CAPACITY;
    ret->tags = tags;
    ret->source = NULL;
  }
  else {
    free(ret);
    free(tags);
    ret = NULL;
  }

  return ret;
}

void tagged_instance_free(tagged_instance* ins,
    clfy_free_func feature_free) {
  if (ins) {
    if (feature_free) {
      feature_free(ins->feature);
    }
    free(ins->tags);
    free(ins->source);
    free(ins->obj_id);
    free(ins);
  }
}

void tagged_instance_add(tagged_instance* ins, unsigned int tag) {
  if (ins->ntags == ins->capacity) {
    ins->capacity *= 2;
    ins->tags = realloc(ins->tags,
        ins->capacity * sizeof(unsigned int));
    assert(ins->tags);
  }

  ins->tags[ins->ntags] = tag;
  ins->ntags++;
}

unsigned int tagged_instance_hastag(tagged_instance* ins, unsigned int tag) {
  unsigned int j;
  for (j = 0; j < ins->ntags; j++) {
    if (ins->tags[j] == tag) {
      return 1;
    }
  }
  return 0;
}

tagged_dataset* tagged_dataset_alloc(void) {
  tagged_dataset* ret = malloc(sizeof(tagged_dataset));
  tagged_instance** instances = malloc(DATASET_INIT_CAPACITY
      * sizeof(tagged_instance*));

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

void tagged_dataset_free(tagged_dataset* data) {
  if (data) {
    free(data->instances);
    free(data);
  }
}

void tagged_dataset_freeall(tagged_dataset* data,
    clfy_free_func feature_free) {
  unsigned int i;
  for (i = 0; i < data->size; i++) {
    tagged_instance_free(data->instances[i], feature_free);
  }
  tagged_dataset_free(data);
}

void tagged_dataset_add(tagged_dataset* data, tagged_instance* ins) {
  if (data->size == data->capacity) {
    data->capacity *= 2;
    data->instances = realloc(data->instances,
        data->capacity * sizeof(tagged_instance*));
    assert(data->instances);
  }

  data->instances[data->size] = ins;
  data->size++;
}

void tagged_result_fprintf(FILE* stream, tagged_result* result,
    const char* name) {
  int name_width = 8;
  int number_width = 8;

  fprintf(stream, "%*s %*s %*s %*s %*s %*s %*s %*s %*s\n",
      name_width, "name", number_width, "tp",
      number_width, "fp", number_width, "fn",
      number_width, "total", number_width, "prec",
      number_width, "reca", number_width, "fmea",
      number_width, "kappa");
  fprintf(stream, "%*s %*d %*d %*d %*d %*g %*g %*g %*g\n",
      name_width, name, number_width, result->tp,
      number_width, result->fp, number_width, result->fn,
      number_width, result->total,
      number_width, TAGGED_PRECISION(*result),
      number_width, TAGGED_RECALL(*result),
      number_width, TAGGED_FMEASURE(*result),
      number_width, TAGGED_KAPPA(*result));
}

void tagged_evaluate(tagged_model* model, tagged_dataset* data,
    unsigned int tag, double* probability, unsigned int* gold_std) {
  unsigned int i;
  for (i = 0; i < data->size; i++) {
    tagged_instance* e = data->instances[i];
    probability[i] = model->tag_prob(model->fields, e->feature);
    gold_std[i] = tagged_instance_hastag(e, tag);

    printf("%*s %*d %*.3f\n", 15, e->source, 5, gold_std[i], 8, probability[i]);
  }
}

double tagged_performance(double prob_threshold, unsigned int size,
    double* probability, unsigned int* gold_std, tagged_result* result) {
  unsigned int i;

  result->tp = 0;
  result->fp = 0;
  result->fn = 0;
  result->total = size;

  for (i = 0; i < size; i++) {
    if (probability[i] >= prob_threshold) {
      if (gold_std[i]) {
        result->tp++;
      }
      else {
        result->fp++;
      }
    }
    else {
      if (gold_std[i]) {
        result->fn++;
      }
    }
  }

  return TAGGED_FMEASURE(*result);
}

static void divide(tagged_dataset* data, unsigned int tag,
    unsigned int* group, unsigned int nfold) {
  unsigned int pc = 0;
  unsigned int nc = 0;

  unsigned int i;
  for (i = 0; i < data->size; i++) {
    if (tagged_instance_hastag(data->instances[i], tag)) {
      group[i] = pc;
      pc = (pc + 1) % nfold;
    }
    else {
      group[i] = nc;
      nc = (nc + 1) % nfold;
    }
  }
}

void tagged_cross_validate(tagged_dataset* data, unsigned int tag,
    tagged_train_func method, void* train_param, unsigned int nfold,
    double* probability, unsigned int* gold_std) {
  unsigned int* group = malloc(data->size * sizeof(unsigned int));
  divide(data, tag, group, nfold);

  unsigned int i, j;
  unsigned int res_offset = 0;
  for (i = 0; i < nfold; i++) {
    tagged_dataset* train = tagged_dataset_alloc();
    train->metadata = data->metadata;
    tagged_dataset* test = tagged_dataset_alloc();
    test->metadata = data->metadata;

    for (j = 0; j < data->size; j++) {
      if (group[j] == i) {
        tagged_dataset_add(test, data->instances[j]);
      }
      else {
        tagged_dataset_add(train, data->instances[j]);
      }
    }

    fprintf(stderr, "CV Partition %u: %u train, %u test\n",
        i, train->size, test->size);
    tagged_model* model = method(train, tag, train_param);
    tagged_evaluate(model, test, tag, probability + res_offset,
        gold_std + res_offset);
    res_offset += test->size;

    model->free_self(model);
    tagged_dataset_free(train);
    tagged_dataset_free(test);
  }

  free(group);
}

void tagged_object_cv(tagged_dataset* data, unsigned int tag,
    tagged_train_func method, void* train_param,
    double* probability, unsigned int* gold_std) {
  unsigned int i, j;
  char** objects = malloc(data->size * sizeof(char*));
  unsigned int n_obj = 0;

  for (i = 0; i < data->size; i++) {
    char* obj = data->instances[i]->obj_id;
    for (j = 0; j < n_obj; j++) {
      if (strcmp(obj, objects[j]) == 0) {
        break;
      }
    }
    if (j == n_obj) {
      objects[n_obj] = obj;
      n_obj++;
    }
  }

  unsigned int res_offset = 0;
  for (i = 0; i < n_obj; i++) {
    tagged_dataset* train = tagged_dataset_alloc();
    train->metadata = data->metadata;
    tagged_dataset* test = tagged_dataset_alloc();
    test->metadata = data->metadata;

    for (j = 0; j < data->size; j++) {
      if (strcmp(objects[i], data->instances[j]->obj_id) == 0) {
        tagged_dataset_add(test, data->instances[j]);
      }
      else {
        tagged_dataset_add(train, data->instances[j]);
      }
    }

    fprintf(stderr, "CV Leave %s out: %u train, %u test\n",
        objects[i], train->size, test->size);
    tagged_model* model = method(train, tag, train_param);
    tagged_evaluate(model, test, tag, probability + res_offset,
        gold_std + res_offset);
    res_offset += test->size;

    model->free_self(model);
    tagged_dataset_free(train);
    tagged_dataset_free(test);
  }

  free(objects);
}

unsigned int tagged_load_dataset(tagged_dataset* data,
    const char* path, clfy_loader_func loader, void* load_param) {
  unsigned int n = 0;

  DIR* dir = opendir(path);
  if (dir) {
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
      char* name = ent->d_name;
      char* dot = strrchr(name, '.');
      if (dot && strcmp(dot, ".fvec") == 0) {
        char filepath[512];
        char tagpath[512];
        strcpy(filepath, path);
        strcat(filepath, "/");
        strcat(filepath, name);
        FILE* in = fopen(filepath, "r");

        strcpy(tagpath, filepath);
        char* tdot = strrchr(tagpath, '.');
        strcpy(tdot, ".tag");
        FILE* tagin = fopen(tagpath, "r");

        if (in && tagin) {
          tagged_instance* ins = tagged_instance_alloc();

          ins->source = strndup(name, dot - name);

          char* dash = strchr(name, '_');
          if (dash) {
            ins->obj_id = strndup(name, dash - name);
          }
          else {
            ins->obj_id = strdup(ins->source);
          }

          ins->feature = loader(in, load_param);

          if (ins->feature) {
            char* line = NULL;
            size_t len = 0;
            ssize_t read;
            while ((read = getline(&line, &len, tagin)) != -1)  {
              size_t l = strlen(line);
              if (l > 0 && line[l-1] == '\n') {
                line[l - 1] = '\0';
                l--;
              }
              if (l > 0) {
                int tid = clfy_metadata_lookup(data->metadata, line);
                tagged_instance_add(ins, tid);
              }
            }

            free(line);

            tagged_dataset_add(data, ins);
            n++;
          }
          else {
            fprintf(stderr, "Error: Failed to load data "
                "from file: %s\n", filepath);
            free(ins->source);
            free(ins->obj_id);
          }

          fclose(in);
          fclose(tagin);
        }
        else {
          fprintf(stderr, "Error: Failed to open file: %s or %s\n"
              "Error: %s\n", filepath, tagpath, strerror(errno));
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


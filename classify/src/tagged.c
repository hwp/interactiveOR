// tagged.c
// Classify extension with tagged data
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2015, All rights reserved.

#include "tagged.h"

#include <assert.h>

#define INSTANCE_INIT_CAPACITY 10
#define DATASET_INIT_CAPACITY 128

tagged_instance* tagged_instance_alloc(void) {
  tagged_instance* ret = malloc(sizeof(tagged_instance));
  const char** tags = malloc(INSTANCE_INIT_CAPACITY
      * sizeof(unsigned int));

  if (ret && tags) {
    ret->feature = NULL;
    ret->ntags = 0;
    ret->capacity = INSTANCE_INIT_CAPACITY;
    ret->tags = tags;
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
    free(ins->names);
    free(ins);
  }
}

void tagged_instance_add(tagged_instance ins,
    unsigned int tag) {
  if (ins->ntags == ins->capacity) {
    ins->capacity *= 2;
    ins->tags = realloc(ins->tags,
        ins->capacity * sizeof(unsigned int));
    assert(ins->tags);
  }

  ins->tags[ins->ntags] = tag;
  ins->ntags++;

  return ins->ntags - 1;
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

tagged_result* tagged_result_alloc(unsigned int size) {
  tagged_result* ret = malloc(sizeof(tagged_result));
  unsigned int* tp = calloc(size, sizeof(unsigned int));
  unsigned int* fp = calloc(size, sizeof(unsigned int));
  unsigned int* fn = calloc(size, sizeof(unsigned int));

  if (ret && tp && fp && fn) {
    ret->size = size;
    ret->tp = tp;
    ret->fp = fp;
    ret->fn = fn;
    ret->total = 0;
  }
  else {
    free(ret);
    free(tp);
    free(fp);
    free(fn);
    ret = NULL;
  }

  return ret;
}

void tagged_result_free(tagged_result* result) {
  if (result) {
    free(result->tp);
    free(result->fp);
    free(result->fn);
    free(result);
  }
}

void tagged_result_fprintf(FILE* stream, tagged_result* result,
    const char* name) {
  name_width = 8;
  number_width = 6;

  fprintf(stream, "%*s %*d %*d %*d %*d %*g %*g %*g\n",
      name_width, name, number_width, result->tp,
      number_width, result->fp, number_width, result->fn,
      number_width, result->total,
      number_width, TAGGED_PRECISION(*result),
      number_width, TAGGED_RECALL(*result),
      number_width, TAGGED_FMEASURE(*result));
}

void tagged_evaluate(tagged_model* model, tagged_dataset* data,
    unsigned int tag, double* probability, unsigned int* gold_std) {
  unsigned int i, j;
  for (i = 0; i < data->size; i++) {
    tagged_instance* e = data->instances[i];
    probability[i] = model->tag_prob(model->fields, e->feature);

    gold_std[i] = 0;
    for (j = 0; j < e->ntags; j++) {
      if (e->tags[j] == tag) {
        gold_std[i] = 1;
        break;
      }
    }
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
}

void tagged_cross_validate(tagged_dataset* data, 
    tagged_train_func method, void* train_param, unsigned int nfold,
    double* probability, unsigned int* gold_std) {
  //TODO
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
        dot = strrchr(tagpath, '.');
        strcpy(dot, ".tag");
        FILE* tagin = fopen(filepath, "r");

        if (in && tagin) {
          tagged_instance* ins = tagged_instance_alloc();

          ins->feature = loader(in, load_param);

          if (ins.feature) {
            char* line;
            size_t len;
            ssize_t read;
            while ((read = getline(&line, &len, tagin)) != -1)  {
              int tid = clfy_metadata_lookup(data->metadata, line);
              tagged_instance_add(ins, tid);
            }

            free(line);

            tagged_dataset_add(data, ins);
            n++;
          }
          else {
            fprintf(stderr, "Error: Failed to load data "
                "from file: %s\n", filepath);
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


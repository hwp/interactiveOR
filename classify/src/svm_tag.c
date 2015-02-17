// svm_tag.c
// svm for tagged data
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2015, All rights reserved.

#include "svm_tag.h"

#include <assert.h>

/**
 * the returned pointer should be freed after use.
 */
static struct svm_node* vector2node(gsl_vector* vector) {
  struct svm_node* ret = malloc((vector->size + 1) * sizeof(struct svm_node));
  int i, c;
  c = 0;
  for (i = 0; i < vector->size; i++) {
    if (gsl_vector_get(vector, i) != 0.0) {
      ret[c].index = i;
      ret[c].value = gsl_vector_get(vector, i);
      c++;
    }
  }
  ret[c].index = -1;
  
  return ret;
}

double svm_tag_prob(svm_tag_attr* attr, seq_t* seq) {
  assert(seq->size == 1);
  struct svm_node* node = vector2node(seq->data[0]);
  double ret = svm_predict(attr->model, node);
  free(node);
  return ret;
}

static void svm_tag_free(tagged_model* model) {
  svm_tag_attr* attr = model->fields;
  svm_free_and_destroy_model(&attr->model);
  int i;
  for (i = 0; i < attr->problem->l; i++) {
    free(attr->problem->x[i]);
  }
  free(attr->problem->y);
  free(attr->problem->x);
  free(attr->problem);
  free(attr);
  free(model);
}

tagged_model* svm_tag_train(tagged_dataset* train_data, unsigned int tag,
    svm_tag_param* param) {
  struct svm_parameter svm_param;
  svm_param.svm_type = C_SVC;
  svm_param.kernel_type = RBF;
  svm_param.degree = 2;       // not used
  svm_param.gamma = param->gamma;
  svm_param.coef0 = 0;        // not used
  svm_param.cache_size = param->cache_size;
  svm_param.eps = 0.001;
  svm_param.C = param->cost;
  svm_param.nr_weight = 0;
  svm_param.weight_label = NULL;
  svm_param.weight = NULL;
  svm_param.nu = .5;          // not used
  svm_param.p = .1;           // not used
  svm_param.shrinking = 1;
  svm_param.probability = 1;  // probability model

  struct svm_problem* problem = malloc(sizeof(struct svm_problem));
  assert(problem);
  problem->l = train_data->size;
  problem->y = malloc(problem->l * sizeof(double));
  problem->x = malloc(problem->l * sizeof(struct svm_node*));
  assert(problem->y);
  assert(problem->x);

  int i;
  for (i = 0; i < train_data->size; i++) {
    problem->y[i] = tagged_instance_hastag(
        train_data->instances[i], tag) ? 1.0 : 0.0;
    seq_t* s = train_data->instances[i]->feature;
    problem->x[i] = vector2node(s->data[0]);
  }

  struct svm_model* model = svm_train(problem, &svm_param);

  svm_tag_attr* attr = malloc(sizeof(svm_tag_attr));
  assert(attr);
  attr->model = model;
  attr->problem = problem;

  tagged_model* ret = malloc(sizeof(tagged_model));
  assert(ret);
  ret->tag_prob = (tagged_tagprob_func) svm_tag_prob;
  ret->free_self = (clfy_free_func) svm_tag_free;
  ret->fields = attr;
 
  return ret;
}


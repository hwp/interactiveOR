// svm_tag.h
// svm for tagged data
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2015, All rights reserved.

#ifndef SVM_TAG_H_
#define SVM_TAG_H_

#include "tagged.h"

#include <notghmm/notghmm.h>

#include <libsvm/svm.h>

#define SVM_TAG_TRAIN ((tagged_train_func) svm_tag_train)

typedef struct {
  struct svm_model* model;
  struct svm_problem* problem;
} svm_tag_attr;

typedef struct {
  double cost;
  double gamma;
  double cache_size;
} svm_tag_param;

/**
 * assume seq is of size 1.
 */
double svm_tag_prob(svm_tag_attr* attr, seq_t* seq);

tagged_model* svm_tag_train(tagged_dataset* train_data, unsigned int tag,
    svm_tag_param* param);

#endif  // SVM_TAG_H_


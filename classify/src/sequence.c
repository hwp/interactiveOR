// sequence.c
// Functions related to classifying sequences.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "sequence.h"

#include <assert.h>

#include <notghmm/notghmm.h>

#define BUFFER_SIZE 128 

seq_t* seq_load(FILE* stream, seq_load_param* param) {
  fseek(stream, 0L, SEEK_END);
  size_t length = ftell(stream);
  fseek(stream, 0L, SEEK_SET);

  assert(length % (param->dim * sizeof(double)) == 0);
  size_t size = length / (param->dim * sizeof(double));

  seq_t* res = seq_alloc(size, param->dim);

  double buf[BUFFER_SIZE];
  size_t sr, i;
  size_t ctr = 0;
  
  while((sr = fread(buf, sizeof(double), BUFFER_SIZE,
          stream)) > 0) {
    for (i = 0; i < sr; i++) {
      gsl_vector_set(res->data[ctr / res->dim],
          ctr % res->dim, buf[i]);
      ctr++;
    }
  }

  return res;
}


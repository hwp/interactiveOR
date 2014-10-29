// classify_test.c
// Test file
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"

#include <stdio.h>

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

clfy_classifier cl;
double th;

clfy_classifier* train(clfy_dataset* train_data) {
  cl.classify = (classify_func) classify;
  cl.free_fields = NULL;
  th = 2.0;
  cl.fields = &th;

  return &cl;
}

int main(int argc, char** argv) {
  point p1 = {0.0, 1.0};
  point p2 = {0.1, 1.0};
  point p3 = {0.0, 2.0};
  point p4 = {-1.0, 0.0};
  clfy_instance ins;

  clfy_dataset* train_data = clfy_dataset_alloc();
  clfy_dataset* test_data = clfy_dataset_alloc();
  ins = (clfy_instance){&p1, 0};
  clfy_dataset_add(test_data, ins);
  ins = (clfy_instance){&p2, 0};
  clfy_dataset_add(test_data, ins);
  ins = (clfy_instance){&p3, 1};
  clfy_dataset_add(test_data, ins);
  ins = (clfy_instance){&p4, 1};
  clfy_dataset_add(test_data, ins);

  double precision = clfy_performance(train_data,
      test_data, train, NULL);
  printf("Precision = %g\n", precision);

  clfy_dataset_free(train_data);
  clfy_dataset_free(test_data);
  return 0;
}


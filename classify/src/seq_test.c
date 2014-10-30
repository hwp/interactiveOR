// seq_test.c
// Test the functions of sequence.h
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "classify.h"
#include "sequence.h"

#include <stdio.h>

int main(int argc, char** argv) {
  clfy_dataset* data = clfy_dataset_alloc();
  clfy_metadata* meta = clfy_metadata_alloc();
  data->metadata = meta;

  seq_load_param param;
  param.dim = 129;

  unsigned int size = clfy_load_dataset(data, argv[1],
      SEQ_LOADER, &param);
  printf("%u instances loaded\n", size);

  clfy_dataset_freeall(data, free);
  clfy_metadata_free(meta);
  return 0;
}


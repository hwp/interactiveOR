// stfttest.c
// Test STFT 
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "utils.h"
#include "stft.h"

int main(int argc, char** argv) {
  int showhelp = 0;

  unsigned int channels = 1;
  size_t window_size;
  size_t step;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;
  unsigned long int count = 1;

  int opt;
  while ((opt = getopt(argc, argv, "hw:s:f:C:")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
      case 'w':
        window_size = atoi(optarg);
        break;
      case 's':
        step = atoi(optarg);
        break;
      case 'f':
        format = snd_pcm_format_value(optarg);
        break;
      case 'C':
        count = atol(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

  if (showhelp || argc - optind < 1) {
    fprintf(stderr, "Usage: %s [-w window_size] [-s step] "
        "[-f format] <inputFile>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (format == SND_PCM_FORMAT_UNKNOWN) {
    fprintf(stderr, "Unknown format\n");
    exit(EXIT_FAILURE);
  }
 
  FILE* input = fopen(argv[optind], "r");
  if (input == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n", argv[optind], strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Load Audio File
  double** data;
  count = readFile(input, count, channels, format, &data);
  fprintf(stderr, "%lu samples read\n", count);
  fclose(input);

  // Transform
  size_t now = (count - window_size) / step + 1;
  double** result = malloc(now * sizeof(double*));
  size_t i;
  for (i = 0; i < now; i++) {
    result[i] = malloc(window_size * 2 * sizeof(double));
  }

  fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); // FIXME:REMOVE
  stft(data[0], count, window_size, step, result);

  fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); // FIXME:REMOVE
  printf("# name: a\n# type: complex matrix\n"
      "# rows: %d\n# columns: %d\n", window_size, now);
  size_t j;
  for (j = 0; j < window_size; j++) {
    for (i = 0; i < now; i++) {
      printf("(%g,%g) ", REAL(result[i], j), IMAG(result[i], j));
    }
    printf("\n");
  }

  // Free memory
  freeData(data, channels);
  for (i = 0; i < now; i++) {
    free(result[i]);
  }
  free(result);

  exit(EXIT_SUCCESS);
}


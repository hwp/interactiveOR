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
  int window_size;
  int shift;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;

  int opt;
  while ((opt = getopt(argc, argv, "hw:s:f:")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
      case 'w':
        window_size = atoi(optarg);
        break;
      case 's':
        shift = atoi(optarg);
        break;
      case 'f':
        format = snd_pcm_format_value(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

  if (showhelp || argc - optind < 1) {
    fprintf(stderr, "Usage: %s [-w window_size] [-s shift] "
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
  unsigned long int count = pcm_size(input, channels, format);
  count = read_file(input, count, channels, format, &data);
  fprintf(stderr, "%lu samples read\n", count);
  fclose(input);

  // Transform
  int nos = number_of_spectrum(count, window_size, shift);
  TimeFreq* tf = alloc_tf(window_size, nos);

  stft(data[0], count, window_size, shift, tf);

  printf("# name: spec\n# type: complex matrix\n"
      "# rows: %d\n# columns: %d\n", window_size, nos);
  int i, j;
  for (j = 0; j < window_size; j++) {
    for (i = 0; i < nos; i++) {
      printf("(%g,%g) ", get_real(get_spectra(tf, i), j),
          get_imag(get_spectra(tf, i), j));
    }
    printf("\n");
  }

  // Free memory
  free_data(data, channels);
  free_tf(tf);

  exit(EXIT_SUCCESS);
}


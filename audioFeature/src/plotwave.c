// plotwave.c
// Generate wave plot with gnuplot script 
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "utils.h"

int main(int argc, char** argv) {
  int showhelp = 0;

  unsigned int channels = 1;
  int rate = 8000;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;

  int opt;
  while ((opt = getopt(argc, argv, "hw:s:r:f:")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
      case 'r':
        rate = atoi(optarg);
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
    fprintf(stderr, "Usage: %s [-r sampling rate] "
        "[-f format] <inputFile>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (format == SND_PCM_FORMAT_UNKNOWN) {
    fprintf(stderr, "Unknown format\n");
    exit(EXIT_FAILURE);
  }
 
  FILE* input = fopen(argv[optind], "r");
  if (input == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n",
        argv[optind], strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Load Audio File
  double** data;
  unsigned long int count = pcm_size(input, channels, format);
  count = read_file(input, count, channels, format, &data);
  fprintf(stderr, "%lu samples read\n", count);
  fclose(input);

  printf("set style line 11 lc rgb '#808080' lt 1\n"
      "set border 3 front ls 11\n"
      "set tics nomirror out scale 0.75\n"
      "unset key\n"
      "set xlabel 'Time (s)'\n"
      "set ylabel 'Amplitude'\n");
  printf("plot '-' with lines\n");
  unsigned long int i;
  for (i = 0; i < count; i++) {
    printf("%g %g\n", (double) i / (double) rate, data[0][i]);
  }
  printf("e\n");

  // Free memory
  free_data(data, channels);

  exit(EXIT_SUCCESS);
}


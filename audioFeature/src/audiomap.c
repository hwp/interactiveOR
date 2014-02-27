// audiomap.c
// Map an audio file to SOM node sequence
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "utils.h"
#include "stft.h"
#include "som.h"

#include <math.h>
#include <string.h>

#define DEFAULT_SOMFILE "audio.som"

int main(int argc, char** argv) {
  int i, j;
  int showhelp = 0;

  unsigned int channels = 1;
  int window_size = 1024;
  int shift = 256;
  char* somfile = NULL;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;

  int opt;
  while ((opt = getopt(argc, argv, "hw:s:f:o:")) != -1) {
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
      case 'o':
        somfile = strdup(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

  if (showhelp || argc - optind < 1) {
    fprintf(stderr, "Usage: %s [-w window_size] [-s shift] "
        "[-f format] [-o somfile] <inputFile>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (format == SND_PCM_FORMAT_UNKNOWN) {
    fprintf(stderr, "Unknown format\n");
    exit(EXIT_FAILURE);
  }

  if (somfile == NULL) {
    somfile = strdup(DEFAULT_SOMFILE);
  }

  // Load SOM model
  fprintf(stderr, "Read SOM from %s\n", somfile);
  FILE* file = fopen(somfile, "r");
  if (file == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n", somfile,
        strerror(errno));
    exit(EXIT_FAILURE);
  }
  SOM* net = som_load(file);
  fclose(file);

  fprintf(stderr, "SOM Loaded (size: %d * %d, dimension: %d)\n",
      net->rows, net->cols, net->dims);

  // Load Data
  fprintf(stderr, "Loading Audio File %s (%d channel(s), %s)\n",
      argv[optind], channels, snd_pcm_format_name(format));
  FILE* input = fopen(argv[optind], "r");
  if (input == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n", argv[optind],
        strerror(errno));
    exit(EXIT_FAILURE);
  }

  double** data;
  unsigned long int count = pcm_size(input, channels, format);
  count = read_file(input, count, channels, format, &data);
  fprintf(stderr, "%lu samples read\n", count);
  fclose(input);

  // Transform
  fprintf(stderr, "Calculating STFT (windows size %d, shift %d)\n",
      window_size, shift);
  int nos = number_of_spectrum(count, window_size, shift);
  TimeFreq* tf = alloc_tf(window_size, nos);

  stft(data[0], count, window_size, shift, tf);
  free_data(data, channels);

  fprintf(stderr, "%d STFT Calculated\n", nos);

  // Mapping
  double* mspec = malloc(net->dims * sizeof(double));
  for (i = 0; i < nos; i++) {
    Spectra s = get_spectra(tf, i);
    for (j = 0; j < net->dims; j++) {
      mspec[j] = get_magnitude(s, j);
    }
    double dis;
    int id = som_map_dis(net, mspec, &dis);
    printf("%d %g\n", id, dis);
  }

  fprintf(stderr, "Mapping finished\n");

  // Free memory
  free_tf(tf);
  free(mspec);
  som_free(net);

  exit(EXIT_SUCCESS);
}


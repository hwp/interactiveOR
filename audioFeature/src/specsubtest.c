// specsubtest.c
// Test spectral substraction
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "utils.h"
#include "spectral_sub.h"

#include <string.h>

int main(int argc, char** argv) {
  int showhelp = 0;

  unsigned int channels = 1;
  int window_size = 1024;
  int shift = 256;
  char* noisefile = NULL;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;

  int opt;
  while ((opt = getopt(argc, argv, "hw:s:f:n:")) != -1) {
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
      case 'n':
        noisefile = strdup(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

  if (showhelp || argc - optind < 2) {
    fprintf(stderr, "Usage: %s [-w window_size] [-s shift] "
        "[-f format] [-n noiseFile] <inputFile> <outputFile>\n"
        , argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (format == SND_PCM_FORMAT_UNKNOWN) {
    fprintf(stderr, "Unknown format\n");
    exit(EXIT_FAILURE);
  }
  else if (noisefile == NULL || strlen(noisefile) == 0) {
    fprintf(stderr, "No noise sample given\n");
    exit(EXIT_FAILURE);
  }

  // Calculate average magnitude spectrum
  FILE* input = fopen(noisefile, "r");
  if (input == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n", noisefile,
        strerror(errno));
    exit(EXIT_FAILURE);
  }

  double** data;
  unsigned long int count = pcm_size(input, channels, format);
  count = read_file(input, count, channels, format, &data);
  fprintf(stderr, "%lu samples read\n", count);
  fclose(input);

  int nos = number_of_spectrum(count, window_size, shift);
  TimeFreq* tf = alloc_tf(window_size, nos);
  stft(data[0], count, window_size, shift, tf);
  free_data(data, channels);

  double* avg_spectrum = malloc((window_size / 2 + 1) *
      sizeof(double));
  cal_avg_spectrum(tf, avg_spectrum);
  free_tf(tf);

  // Load Audio File
  input = fopen(argv[optind], "r");
  if (input == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n", argv[optind],
        strerror(errno));
    exit(EXIT_FAILURE);
  }

  count = pcm_size(input, channels, format);
  count = read_file(input, count, channels, format, &data);
  fprintf(stderr, "%lu samples read\n", count);
  fclose(input);

  // Transform
  nos = number_of_spectrum(count, window_size, shift);
  tf = alloc_tf(window_size, nos);
  stft(data[0], count, window_size, shift, tf);
  free(data[0]);

  // Spectral subtraction
  TimeFreq* ntf = alloc_tf(window_size, nos);
  mag_spectral_sub(avg_spectrum, tf, ntf);

  // Inverse transform
  count = istft_size(tf);
  data[0] = malloc(count * sizeof(double));
  istft(ntf, data[0]);

  FILE* output = fopen(argv[optind + 1], "w");
  if (output == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n",
        argv[optind + 1], strerror(errno));
    exit(EXIT_FAILURE);
  }

  count = write_file(output, count, channels, format, data);
  fclose(output);

  free_data(data, channels);
  free_tf(tf);
  free_tf(ntf);
  free(avg_spectrum);

  exit(EXIT_SUCCESS);
}


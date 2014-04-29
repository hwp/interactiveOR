// addnoise.c
// Add white noise to audio files
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "utils.h"

int main(int argc, char** argv) {
  int showhelp = 0;

  unsigned int channels = 1;
  double scale = 0.5;
  char* noisefile = NULL;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;

  int opt;
  while ((opt = getopt(argc, argv, "hs:f:n:")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
      case 's':
        scale = atof(optarg);
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
    fprintf(stderr, "Usage: %s [-s scale] "
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

  // Load Noise File
  fprintf(stderr, "Loading Noise File %s (%d channel(s), %s)\n",
      noisefile, channels, snd_pcm_format_name(format));
  FILE* input = fopen(noisefile, "r");
  if (input == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n", noisefile,
        strerror(errno));
    exit(EXIT_FAILURE);
  }

  double** noise;
  unsigned long int nsize= pcm_size(input, channels, format);
  nsize= read_file(input, nsize, channels, format, &noise);
  fprintf(stderr, "%lu samples read\n", nsize);
  fclose(input);

  // Load Audio File
  fprintf(stderr, "Loading Audio File %s (%d channel(s), %s)\n",
      argv[optind], channels, snd_pcm_format_name(format));
  input = fopen(argv[optind], "r");
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

  // Add noise
  fprintf(stderr, "Add noise with scale = %g\n", scale);
  int i;
  for (i = 0; i < count; i++) {
    double v = data[0][i] + scale * noise[0][i % nsize];
    if (v >= 1.0) {
      v = 1.0;
    }
    else if(v <= -1.0) {
      v = -1.0;
    }
    data[0][i] = v;
  }

  // Write Result
  fprintf(stderr, "Saving Audio File %s (%d channel(s), %s)\n",
      argv[optind + 1], channels, snd_pcm_format_name(format));
  FILE* output = fopen(argv[optind + 1], "w");
  if (output == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n",
        argv[optind + 1], strerror(errno));
    exit(EXIT_FAILURE);
  }

  count = write_file(output, count, channels, format, data);
  fprintf(stderr, "%lu samples written\n", count);
  fclose(output);

  free_data(noise, channels);
  free_data(data, channels);

  exit(EXIT_SUCCESS);
}


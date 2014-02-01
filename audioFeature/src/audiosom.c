// audiosom.c
// Train a SOM using audios
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "utils.h"
#include "stft.h"
#include "som.h"

#include <math.h>

#define DEFAULT_OUTFILE "audio.som"

int main(int argc, char** argv) {
  int i, j, k;
  int showhelp = 0;

  unsigned int channels = 1;
  int window_size = 1024;
  int shift = 256;
  int som_rows = 6;
  int som_cols = 6;
  int iters = 100;
  char* outfile = NULL;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;

  int opt;
  while ((opt = getopt(argc, argv, "hw:s:f:r:c:i:o")) != -1) {
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
      case 'r':
        som_rows = atoi(optarg);
        break;
      case 'c':
        som_rows = atoi(optarg);
        break;
      case 'i':
        iters = atoi(optarg);
        break;
      case 'o':
        outfile = strdup(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

  if (showhelp || argc - optind < 1) {
    fprintf(stderr, "Usage: %s [-w window_size] [-s shift] "
        "[-f format] [-r rows] [-c columns] [-i iterations] "
        "[-o output] <inputFile(s)>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (format == SND_PCM_FORMAT_UNKNOWN) {
    fprintf(stderr, "Unknown format\n");
    exit(EXIT_FAILURE);
  }

  if (outfile == NULL) {
    outfile = strdup(DEFAULT_OUTFILE);
  }
 
  // Load Data 
  int totalnos = 0;
  for (i = optind; i < argc; i++) {
    FILE* input = fopen(argv[i], "r");
    if (input == NULL) {
      fprintf(stderr, "Cannot Open File %s : %s\n", argv[i],
          strerror(errno));
      exit(EXIT_FAILURE);
    }

    unsigned long int count = pcm_size(input, channels, format);
    fclose(input);

    totalnos += number_of_spectrum(count, window_size, shift);
  }

  int som_dims = window_size / 2;
  // Magnitude spectrum
  double* mspec = malloc(som_dims * totalnos * sizeof(double));
  int np = 0;

  for (i = optind; i < argc; i++) {
    fprintf(stderr, "Loading Audio File %s (%d channel(s), %s)\n",
        argv[i], channels, snd_pcm_format_name(format));
    FILE* input = fopen(argv[i], "r");
    if (input == NULL) {
      fprintf(stderr, "Cannot Open File %s : %s\n", argv[i],
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

    for (j = 0; j < nos; j++) {
      for (k = 0; k < som_dims; k++) {
        double re =  get_real(get_spectra(tf, j), k);
        double im =  get_imag(get_spectra(tf, j), k);
        mspec[np * som_dims + k] = sqrt(re * re + im * im);
      }
      np++;
    }

    // Free memory
    free_data(data, channels);
    free_tf(tf);
  }
  
  fprintf(stderr, "Total %d STFT Calculated\n", np);

  // Train
  fprintf(stderr, "Training SOM (size: %d * %d, dimension: %d)\n",
      som_rows, som_cols, som_dims);
  fprintf(stderr, "with %d iterations\n", iters);
  SOM* net = som_alloc(som_rows, som_cols, som_dims);
  som_train(net, mspec, totalnos, iters);
  free(mspec);
  fprintf(stderr, "Finished\n");
  
  // Save SOM model
  fprintf(stderr, "Saving to file %s\n", outfile);
  FILE* file = fopen(outfile, "w");
  if (file == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n", outfile,
        strerror(errno));
    exit(EXIT_FAILURE);
  }
  som_save(net, file);
  fclose(file);

  exit(EXIT_SUCCESS);
}


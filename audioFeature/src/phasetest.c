// phasetest.c
// Test the influence of changin phase of a tf signal.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "utils.h"
#include "filter.h"

#include <gsl/gsl_rng.h>
#include <math.h>

void zerophase(int window_size, void* args, Spectra source,
    Spectra result) {
  gsl_rng* r = gsl_rng_alloc(gsl_rng_default);

  int i;
  for (i = 0; i < window_size / 2 + 1; i++) {
    set_by_polar(result, i, get_magnitude(source, i),
         gsl_rng_uniform (r) * 2 * M_PI);
    fprintf(stderr, "%f\n", get_phase(result, i));
    if (i > 0 && i < window_size / 2) {
      set_value(result, window_size - i, get_real(result, i),
          get_imag(result, i));
    }
  }
}

int main(int argc, char** argv) {
  int showhelp = 0;

  unsigned int channels = 1;
  int window_size = 1024;
  int shift = 256;
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

  if (showhelp || argc - optind < 2) {
    fprintf(stderr, "Usage: %s [-w window_size] [-s shift] "
        "[-f format] <inputFile> <outputFile>\n" , argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (format == SND_PCM_FORMAT_UNKNOWN) {
    fprintf(stderr, "Unknown format\n");
    exit(EXIT_FAILURE);
  }

  // Load Audio File
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
  int nos = number_of_spectrum(count, window_size, shift);
  TimeFreq* tf = alloc_tf(window_size, nos);
  stft(data[0], count, window_size, shift, tf);
  free(data[0]);

  // Spectral subtraction
  TimeFreq* ntf = alloc_tf(window_size, nos);
  do_filter(zerophase, NULL, tf, ntf);

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

  exit(EXIT_SUCCESS);
}


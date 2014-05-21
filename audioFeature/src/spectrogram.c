// spectrogram.c
// Generate spectrogram with gnuplot script 
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2014, All rights reserved.

#include "utils.h"
#include "stft.h"

int main(int argc, char** argv) {
  int showhelp = 0;

  unsigned int channels = 1;
  int window_size = 1024;
  int shift = 256;
  int rate = 8000;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;

  int opt;
  while ((opt = getopt(argc, argv, "hw:s:r:f:")) != -1) {
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
    fprintf(stderr, "Usage: %s [-w window_size] [-s shift] "
        "[-r sampling rate] [-f format] <inputFile>\n", argv[0]);
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

  printf("set style line 11 lc rgb '#808080' lt 1\n"
      "set border 3 front ls 11\n"
      "set tics nomirror out scale 0.75\n"
      "unset key\n"
      "unset colorbox\n"
      "set palette defined (0 '#000090', 1 '#000fff', 2 '#0090ff', 3 '#0fffee', 4 '#90ff70', 5 '#ffee00', 6 '#ff7000', 7 '#ee0000', 8 '#7f0000')\n"
      "set xlabel 'Time (s)'\n"
      "set ylabel 'Frequency (Hz)'\n");
  printf("set yrange [0:%g]\n", (double) rate / 2.0);
  printf("set xrange [0:%g]\n", (double) count / rate);
  printf("plot '-' matrix using (($2 + 0.5) * %g) : (($1 + 0.5) * %g) : (log($3))"
    " with image\n", (double) shift / rate, 
    (double) rate / window_size);
  int i, j;
  for (i = 0; i < nos; i++) {
    Spectra s = get_spectra(tf, i);
    for (j = 0; j < window_size / 2 + 1; j++) {
      printf("%g ", get_magnitude(s, j));
    }
    printf("\n");
  }
  printf("e\n");

  // Free memory
  free_data(data, channels);
  free_tf(tf);

  exit(EXIT_SUCCESS);
}

